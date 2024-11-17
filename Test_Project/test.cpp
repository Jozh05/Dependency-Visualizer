#include "pch.h"
#include "../Source.cpp"

namespace Test_functions {

	// Тест на пустой архив
	TEST(ExtractFileTest, EmptyArchive) {
		std::vector<char> empty_archive_data;  // Пустой архив
		std::string content = ExtractFile(empty_archive_data);

		// Ожидаем пустую строку для пустого архива
		EXPECT_TRUE(content.empty());
	}

	// Тест на некорректный архив
	TEST(ExtractFileTest, InvalidArchive) {
		std::vector<char> invalid_data = { 'i', 'n', 'v', 'a', 'l', 'i', 'd' };  // Некорректные данные
		std::string content = ExtractFile(invalid_data);

		// Ожидаем, что функция вернет пустую строку для некорректного архива
		EXPECT_TRUE(content.empty());
	}
	TEST(WriteCallbackTest, WriteDataToBuffer) {
		std::vector<char> buffer;
		const char* data = "test data";

		// Вызов WriteCallback
		WriteCallback(const_cast<void*>(static_cast<const void*>(data)), 1, strlen(data), &buffer);

		// Проверка содержимого буфера
		EXPECT_EQ(buffer.size(), strlen(data));
		EXPECT_EQ(std::string(buffer.begin(), buffer.end()), "test data");
	}

	TEST(MyHashTest, HashingPairs) {
		MyHash hash_fn;

		std::pair<const std::string, const std::string> p1("key1", "value1");
		std::pair<const std::string, const std::string> p2("key2", "value2");

		size_t hash1 = hash_fn(p1);
		size_t hash2 = hash_fn(p2);

		EXPECT_NE(hash1, hash2);  // Ожидаем, что хеши будут разные для разных пар
	}

	TEST(ExtractDependenciesFromFileTest, ValidXML) {
		std::string xml = R"(<package><metadata><dependencies><group><dependency id="dep1" version="1.0"/></group></dependencies></metadata></package>)";

		auto result = ExtractDependenciesFromFile(xml);

		EXPECT_EQ(result.size(), 1);
		EXPECT_EQ(result.begin()->first, "dep1");
		EXPECT_EQ(result.begin()->second, "1.0");
	}

	TEST(ExtractDependenciesFromFileTest, InvalidXML) {
		std::string xml = "asdasdaca";

		// При неправильном XML мы ожидаем исключение
		EXPECT_THROW(ExtractDependenciesFromFile(xml), XMLParseException);
	}
}