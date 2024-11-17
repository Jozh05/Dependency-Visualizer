#pragma once
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <fstream>
#include <cstdlib>
#include <string>


#include <curl/curl.h>
#include <graphviz/gvc.h>
#include <archive.h>
#include <archive_entry.h>
#include <tinyxml2.h>

std::string ExtractFile(const std::vector<char>& archive_data);

int test_function(int a, int b);

size_t WriteCallback(void* ptr, size_t size, size_t nmemb, void* data);

struct MyHash {
	template <typename T1, typename T2>
	std::size_t operator()(const std::pair<const T1, const T2>& p) const;
};

class XMLParseException : public std::runtime_error {
public:
	explicit XMLParseException(const std::string& message);
};

void curl(const char* url, std::vector<char>& archive_data);

using namespace tinyxml2;

std::unordered_set<std::pair<const std::string, const std::string>, MyHash> ExtractDependenciesFromFile(const std::string xml_string);

void ProcessPackage(std::string current_name, std::string current_url, std::vector<char>& buffer, std::vector<std::string>& vec,
	std::unordered_set<std::string>& visited);

