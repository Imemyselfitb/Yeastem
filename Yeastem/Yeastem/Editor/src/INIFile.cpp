#include "Yeastem.h"

#include "INIFile.h"

#include <fstream>

YEASTEM_BEGIN

SectionView::Value::Value(const Value& other)
{
	type = other.type;
	switch (other.type)
	{
	case TypeID::Real:
		real = other.real;
		break;
	case TypeID::Uint:
		uint = other.uint;
		break;
	case TypeID::StringView:
		stringview = other.stringview;
		break;
	case TypeID::String:
		new(&string) std::string(other.string);
		break;
	}
}

std::ostream& operator<<(std::ostream& os, const SectionView::Value& val)
{
	switch (val.type)
	{
	case SectionView::Value::TypeID::Real:
		os << val.real;
		break;
	case SectionView::Value::TypeID::Uint:
		os << val.uint;
		break;
	case SectionView::Value::TypeID::String:
		os << "\"" << val.string << "\"";
		break;
	case SectionView::Value::TypeID::StringView:
		os << "\"" << val.stringview << "\"";
		break;
	}

	return os;
}

SectionView::Value SectionView::getAttribute(std::string_view key) const
{
	for (KV attr : attributes)
		if (attr.key == key)
			return attr.value;

	return {};
}

const SectionView::KV_Multi* SectionView::getProperty(std::string_view key) const
{
	for (const KV_Multi& prop : properties)
		if (prop.key == key)
			return &prop;

	return nullptr;
}

const SectionView* INI_File::getSection(std::string_view name, uint32_t appearanceIdx)
{
	uint32_t currentAppearanceIdx = 0;
	for (const SectionView& section : sections)
	{
		if (section.header == name)
		{
			if (currentAppearanceIdx >= appearanceIdx)
				return &section;

			currentAppearanceIdx++;
		}
	}

	return nullptr;
}

std::string_view INI_Parser::Trim(std::string_view str)
{
	while (!str.empty() && isspace(str.front())) str.remove_prefix(1);
	while (!str.empty() && isspace(str.back())) str.remove_suffix(1);
	return str;
}

void INI_Parser::SplitPropertiesList(std::vector<SectionView::Value>& result, std::string_view s)
{
	s = Trim(s);
	while (!s.empty())
	{
		s = Trim(s);
		if (s.front() == '"')
		{
			s.remove_prefix(1);
			size_t endQuote = s.find('"');
			if (endQuote == std::string_view::npos)
				break;

			result.emplace_back(s.substr(0, endQuote));
			s.remove_prefix(endQuote + 1);
		}
		else
		{
			size_t comma = s.find(',');
			result.emplace_back(Trim(s.substr(0, comma)));
			if (comma == std::string_view::npos)
				break;

			s.remove_prefix(comma);
		}

		size_t comma = s.find(',');
		if (comma == std::string_view::npos)
			break;

		s.remove_prefix(comma + 1);
	}
}

void INI_Parser::Print(const INI_File& data)
{
	for (auto& section : data.sections)
	{
		YEASTEM_INFO("Section: " << section.header);
		for (auto& attr : section.attributes)
			YEASTEM_INFO("\t[[Attribute]] " << attr.key << " = " << attr.value.string);

		for (auto& prop : section.properties)
		{
			if (prop.values_count == 1)
			{
				YEASTEM_INFO("\t[[Property]] " << prop.key << " = " << section.allProperties[prop.values_start]);
			}
			else
			{
				std::cout << "\t[[Property]] " << prop.key << " = [";
				for (uint32_t i = 0; i < prop.values_count; i++)
				{
					if (i > 0)
						std::cout << ", ";

					std::cout << section.allProperties[prop.values_start + i];
				}
				std::cout << "]\n";
			}
		}
		YEASTEM_INFO("");
	}
}

void INI_Parser::WriteFile(const INI_File& data, const std::filesystem::path& projectPath)
{
	std::ofstream file(projectPath);
	if (!file.is_open())
	{
		YEASTEM_ERROR("WRITING PROJECT: File " << projectPath << " could not be opened!");
		return;
	}

	bool isFirst = true;
	for (const SectionView& section : data.sections)
	{
		if (!isFirst)
			file << "\n";
		
		isFirst = false;

		file << "[" << section.header << " ";
		for (uint32_t i = 0; i < section.attributes.size(); i++)
		{
			if (i > 0)
				file << " ";
			file << section.attributes[i].key << "=" << section.attributes[i].value;
		}

		file << "]\n";

		for (const SectionView::KV_Multi& prop : section.properties)
		{
			file << prop.key << "=";
			for (size_t i = 0; i < prop.values_count; i++)
			{
				if (i > 0)
					file << ",";
				file << section.allProperties[prop.values_start + i];
			}
			file << "\n";
		}
	}
	file.close();
}

void INI_Parser::ReadFile(INI_File& outData, const std::filesystem::path& projectPath)
{
	if (!FileIO::checkIfExists(projectPath))
	{
		YEASTEM_ERROR("LOADING PROJECT: File " << projectPath << " does not exist!");
		return;
	}

	outData.file = FileIO::readFileText(projectPath);
	const char* ptr = outData.file.data();
	const char* end = outData.file.data() + outData.file.size();

	SectionView currentSection;

	while (ptr < end)
	{
		const char* lineStart = ptr;
		while (ptr < end && *ptr != '\n' && *ptr != '\r')
			ptr++;

		std::string_view line(lineStart, ptr - lineStart);
		line = Trim(line);

		if (line.empty() || line.front() == '#')
		{
			ptr++;
			continue;
		}

		if (line.front() == '[')
		{
			if (!currentSection.header.empty())
			{
				outData.sections.push_back(currentSection);
				currentSection = SectionView();
			}

			std::string_view inner = line.substr(1, line.size() - 2);
			inner = Trim(inner);

			size_t space = inner.find(' ');
			if (space == std::string_view::npos)
			{
				currentSection.header = inner;
			}
			else
			{
				currentSection.header = inner.substr(0, space);
				std::string_view attrs = inner.substr(space + 1);
				while (!attrs.empty())
				{
					size_t eq = attrs.find('=');
					if (eq == std::string_view::npos)
						break;

					std::string_view key = Trim(attrs.substr(0, eq));
					attrs.remove_prefix(eq + 1);

					std::string_view val;
					if (!attrs.empty() && attrs.front() == '"')
					{
						attrs.remove_prefix(1);
						size_t endQuote = attrs.find('"');
						if (endQuote == std::string_view::npos)
							break;

						val = attrs.substr(0, endQuote);
						attrs.remove_prefix(endQuote + 1);
					}
					else
					{
						size_t space = attrs.find(' ');
						val = Trim(attrs.substr(0, space));
						attrs = (space == std::string_view::npos) ? "" : attrs.substr(space + 1);
					}

					currentSection.attributes.push_back(SectionView::KV{ key, val });
					attrs = Trim(attrs);
				}
			}
		}
		else
		{
			auto eqPos = line.find('=');
			if (eqPos == std::string_view::npos)
				break;

			SectionView::KV_Multi entry;
			entry.key = Trim(line.substr(0, eqPos));
			entry.values_start = currentSection.allProperties.size();

			std::string_view values = Trim(line.substr(eqPos + 1));
			SplitPropertiesList(currentSection.allProperties, values);

			entry.values_count = currentSection.allProperties.size() - entry.values_start;
			currentSection.properties.push_back(entry);
		}
	}

	if (!currentSection.header.empty())
		outData.sections.push_back(currentSection);
}

YEASTEM_END
