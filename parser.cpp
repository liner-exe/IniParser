#include <fstream>
#include <iostream>
#include <string>
#include <map>

class IniParser
{
	std::map <std::string, std::map<std::string, std::string>> m_data;

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
public:
	void read(const std::string& filename)
	{
		std::ifstream file(filename);

		std::map<std::string, std::string> innerData;
		std::string line;
		std::string section;
		std::string key;
		std::string value;

		while (getline(file, line))
		{
			trim(line);
			if (line.empty() || line[0] == ';')
			{
				continue;
			}

			size_t openBracketPos = line.find('[');
			size_t closeBracketPos = line.find(']');

			if (openBracketPos == std::string::npos)
			{
				size_t equalSign = line.find('=');
				key = line.substr(0, equalSign);
				value = line.substr(equalSign + 1);

				trim(key);
				trim(value);

				innerData[key] = value;
				continue;
			}

			if (!section.empty())
			{
				m_data[section] = innerData;
				innerData.clear();
			}

			section = line.substr(openBracketPos + 1, closeBracketPos - 1);
		}

		file.close();
	}

	std::string get(const std::string& section, const std::string& key)
	{
		return m_data[section][key];
	}

	void show() const
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

int main()
{
	IniParser parser;
	parser.read("config.ini");
	std::cout << parser.get("bot", "token") << std::endl;
	return 0;
}