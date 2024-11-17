
#include "Header.h"


int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "rus"); 
    std::vector<char> archive_data;
    std::vector<std::string> vec;
    std::unordered_set<std::string> visited;
    std::string current_name = argv[2];
    std::string current_url = argv[4];

    ProcessPackage(current_name, current_url, archive_data, vec, visited);

    const std::size_t bufferSize = 1024 * 1024 * 5;
    char* buffer = new char[bufferSize];

    std::ofstream dot("file.txt");
    dot.rdbuf()->pubsetbuf(buffer, bufferSize);

    if (!dot) {
        std::cerr << "Ошибка при открытии файла!" << std::endl;
        return 1;
    }
    dot << "digraph dependencies {\n";

    for (const auto& dep : vec) {
        dot << dep;
    }

    dot << "}\n";

    // Закрываем файл
    dot.close();
    delete[] buffer;
    std::string arg_1 = argv[1]; std::string arg_3 = argv[3];
    std::string command = arg_1 + " -Tdot file.txt -o " + arg_3;
    
    system(command.c_str());

    std::ifstream file(arg_3);
    std::string line;
    while (std::getline(file, line)) {  
        std::cout << line << std::endl;
    }

    file.close();
}

//https://www.nuget.org/api/v2/package/System.Drawing.Common/9.0.0

//https://www.nuget.org/api/v2/package/AutoMapper/13.0.1

//https://www.nuget.org/api/v2/package/Serilog/4.1.1-dev-02318