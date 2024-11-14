#include <iostream>
#include <vector>
#include <unordered_set>
#include <stdexcept>


#include <curl/curl.h>
#include <graphviz/gvc.h>
#include <archive.h>
#include <archive_entry.h>
#include <tinyxml2.h>


// Функция для записи полученных данных в файл
size_t WriteCallback(void* ptr, size_t size, size_t nmemb, void* data) {
    size_t total_size = size * nmemb;
    std::vector<char>* buffer = static_cast<std::vector<char>*>(data);
    buffer->insert(buffer->end(), (char*)ptr, (char*)ptr + total_size);
    return total_size;
}

struct MyHash {
    template <typename T1,typename T2>
    std::size_t operator()(const std::pair<const T1, const T2>& p) const {
        std::size_t h1 = std::hash<T1>{}(p.first);
        std::size_t h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

class XMLParseException : public std::runtime_error {
public:
    explicit XMLParseException(const std::string& message)
        : std::runtime_error(message) {}
};

std::string ExtractFile(const std::vector<char>& archive_data) {
    struct archive* a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_compression_all(a);

    if (archive_read_open_memory(a, archive_data.data(), archive_data.size()) != ARCHIVE_OK) {
        std::cerr << "Failed to open archive" << std::endl;
        return "";
    }

    struct archive_entry* entry;
    std::string file_content;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char* filename = archive_entry_pathname(entry);
        if (strstr(filename, ".nuspec") == nullptr) {
            continue;
        }

        std::cout << "Extracting and displaying content of: " << filename << std::endl;

        // Размер файла
        size_t size = archive_entry_size(entry);
        char buffer[8192];  // Буфер для хранения данных

        size_t bytes_read;
        
        // Чтение данных из архива и сохранение в буфер
        while ((bytes_read = archive_read_data(a, buffer, sizeof(buffer))) > 0) {
            file_content.append(buffer, bytes_read);
        }

        if (bytes_read < 0) {
            std::cerr << "Error reading data for " << filename << ": "
                << archive_error_string(a) << std::endl;
        }
    }

    archive_read_free(a);  // Освобождаем ресурсы
    return file_content;
}

void curl(std::vector<char>& archive_data) {
    CURL* curl;
    CURLcode response;
    curl = curl_easy_init();
    
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.nuget.org/api/v2/package/Serilog/4.1.1-dev-02318");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &archive_data);
    response = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}


using namespace tinyxml2;


std::unordered_set<std::pair<const std::string, const std::string>, MyHash> ExtractDependenciesFromFile(const std::string xml_string) {
    
    tinyxml2::XMLDocument doc;
    std::unordered_set<std::pair<const std::string, const std::string>, MyHash> result;

    XMLError eResult = doc.Parse(xml_string.c_str());
    if (eResult != XML_SUCCESS) {
        std::cerr << "Ошибка парсинга XML: " << eResult << std::endl;
        throw XMLParseException("XML Parse Error");
    }
    doc.Print();


    XMLElement* dependenciesElement = doc.FirstChildElement("package")->FirstChildElement("metadata")->FirstChildElement("dependencies");

    std::cout << dependenciesElement->Name() << std::endl;
    if (dependenciesElement == nullptr) {
        std::cout << "Зависимости не найдены." << std::endl;
        return result;
    }

    for (XMLElement* group_el = dependenciesElement->FirstChildElement("group"); group_el != nullptr; group_el = group_el->NextSiblingElement("group")) {
        for (XMLElement* depElement = group_el->FirstChildElement("dependency"); depElement != nullptr; depElement = depElement->NextSiblingElement("dependency")) {
            std::string id = depElement->Attribute("id");
            std::string version = depElement->Attribute("version");

            result.emplace(id, version);
            std::cout << "ID: " << id << ", Version: " << version << std::endl;
        }
    }
    std::cout << "Now in set" << std::endl;
    for (const auto& elem : result) {
        std::cout << "ID: " << elem.first << ", Version: " << elem.second << std::endl;
    }
    return result;
}



int main()
{
    std::vector<char> archive_data;
    curl(archive_data);
    std::string test_string = ExtractFile(archive_data);

    std::unordered_set<std::pair<const std::string, const std::string>, MyHash> set = ExtractDependenciesFromFile(test_string);
}

//https://www.nuget.org/api/v2/package/System.Drawing.Common/9.0.0

//https://www.nuget.org/api/v2/package/AutoMapper/13.0.1

//https://www.nuget.org/api/v2/package/Serilog/4.1.1-dev-02318