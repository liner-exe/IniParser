// Ini Parser for Modern C++
// Version 2.1.0
// https://github.com/liner-exe/IniParser
// 2024-2024 liner-exe
// License: MIT


#ifndef INI_PARSER_LINEREXE
#define INI_PARSER_LINEREXE

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace ini {
	class IniParser
	{
	public:
		using KeyValuePair = std::pair<std::string, std::string>;
		using Section = std::pair<std::string, std::vector<KeyValuePair>>;

	private:
		std::vector<Section> m_data;

		static void trim(std::string& str)
		{
			const size_t first = str.find_first_not_of(" \t\r\n");
			if (first == std::string::npos)
			{
				str.clear();
				return;
			}

			const size_t last = str.find_last_not_of(" \t\r\n");
			str = str.substr(first, last - first + 1);
		}

		static std::string toLower(const std::string &str)
		{
			std::string result = str;
			std::transform(result.begin(), result.end(), result.begin(), ::tolower);
			return result;
		}

		static bool contains(const std::vector<std::string>& vec, const std::string& sub)
		{
			return std::find(vec.begin(), vec.end(), sub) != vec.end();
		}

	public:
		void read(const std::string& filename)
		{
			std::ifstream file(filename);

			std::vector<KeyValuePair> innerData;
			std::string line, section;

			while (getline(file, line))
			{
				trim(line);
				if (line.empty() || line[0] == ';')
				{
					continue;
				}

				const size_t openBracketPos = line.find('[');
				const size_t closeBracketPos = line.find(']');

				if (openBracketPos == std::string::npos)
				{
					size_t equalSign = line.find('=');
					std::string key = line.substr(0, equalSign);
					std::string value = line.substr(equalSign + 1);

					trim(key);
					trim(value);

					innerData.emplace_back(key, value);
					continue;
				}

				if (!section.empty())
				{
					m_data.emplace_back(section, innerData);
					innerData.clear();
				}

				section = line.substr(openBracketPos + 1, closeBracketPos - 1);
				trim(section);
			}

			if (!section.empty())
			{
				m_data.emplace_back(section, innerData);
				innerData.clear();
			}

			file.close();
		}

		std::string get(const std::string& section, const std::string& key)
		{
			for (const auto& [storedSection, storedKeyValuePair] : m_data)
			{
				if (storedSection == section)
				{
					for (const auto& [storedKey, storedValue] : storedKeyValuePair)
					{
						if (storedKey == key)
						{
							return storedValue;
						}
					}
				}
			}

			return "-1";
		}

		std::vector<std::string> getSectionNames()
		{
			std::vector<std::string> result;

			for (const auto& [storedSection, storedKeyValuePair] : m_data)
			{
				result.push_back(storedSection);
			}

			return result;
		}

		std::vector<KeyValuePair> getSectionData(const std::string& section) const
		{
			for (const auto& [storedSection, storedKeyValuePair] : m_data)
			{
				if (storedSection == section) {
					return storedKeyValuePair;
				}
			}

			throw std::invalid_argument("Section not found");
		}

		void addSection(const std::string& section)
		{
			m_data.emplace_back(section, std::vector<KeyValuePair>());
		}

		void addOption(const std::string& section, const std::string& key, const std::string& value)
		{
			for (auto& [storedSection, storedKeyValuePair] : m_data)
			{
				if (storedSection == section)
				{
					storedKeyValuePair.emplace_back(key, value);
					return;
				}
			}

			m_data.emplace_back(section, std::vector<std::pair<std::string, std::string>> {{ key, value }});
		}

		bool hasSection(const std::string &sectionName) const
		{
			for (const auto& [storedSection, storedKeyValuePair] : m_data) {
				if (storedSection == sectionName)
				{
					return true;
				}
			}

			return false;
		}

		bool hasKey(const std::string& section, const std::string& key)
		{
			for (const auto& [storedSection, storedKeyValuePair] : m_data)
			{
				if (storedSection == section)
				{
					for (const auto& [storedKey, storedValue] : storedKeyValuePair)
					{
						if (storedKey == key)
						{
							return true;
						}
					}
				}
			}

			return false;
		}

		bool getBoolean(const std::string& section, const std::string& key)
		{
			std::vector<std::string> trueValues { "1", "yes", "true", "on" };
			std::vector<std::string> falseValues { "0", "no", "false", "off" };

			for (const auto& [storedSection, storedKeyValuePair] : m_data)
			{
				if (storedSection == section)
				{
					for (const auto& [storedKey, storedValue] : storedKeyValuePair)
					{
						if (storedKey == key)
						{
							std::string storedValue_ = toLower(storedValue);
							if (contains(trueValues, storedValue_))
							{
								return true;
							}
							if (contains(falseValues, storedValue_))
							{
								return false;
							}
						}
					}
				}
			}

			throw std::invalid_argument("Can't get boolean");
		}

		int getInteger(const std::string& section, const std::string& key)
		{
			for (const auto& [storedSection, storedKeyValuePair] : m_data)
			{
				if (storedSection == section)
				{
					for (const auto& [storedKey, storedValue] : storedKeyValuePair)
					{
						if (storedKey == key)
						{
							try
							{
								return std::stoi(storedValue);
							} catch (const std::invalid_argument&)
							{
								throw std::invalid_argument("Invalid integer value");
							} catch (const std::out_of_range&)
							{
								throw std::invalid_argument("Integer value out of range");
							}
						}
					}
				}
			}

			return -1;
		}

		void print() const
		{
			for (const auto& pair : m_data)
			{
				std::cout << '[' << pair.first << ']' << std::endl;

				for (const auto& innerPair : pair.second)
				{
					std::cout << innerPair.first << " = " << innerPair.second << std::endl;
				}
				std::cout << std::endl;
			}
		}
	};
} // namespace ini

#endif // INI_PARSER_LINEREXE
