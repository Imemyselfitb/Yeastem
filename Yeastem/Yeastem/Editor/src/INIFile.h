#pragma once

YEASTEM_BEGIN

struct SectionView
{
	struct Value
	{
		union
		{
			float real;
			uint64_t uint;
			std::string_view stringview; // Prioritised, for reading data from buffer
			std::string string; // Only used for writing data to buffer

			char data[40]; // Size of Largest Item
		};

		enum class TypeID
		{
			None = 0,
			Real,
			Uint,
			StringView,
			String
		};

		TypeID type { TypeID::None };

		Value() :uint(0) {}

		Value(float v)		:type(TypeID::Real), real(v) {}
		Value(double v)		:type(TypeID::Real), real((float)v) {}

		Value(char v)		:type(TypeID::Uint), uint((uint64_t)v) {}
		Value(short v)		:type(TypeID::Uint), uint((uint64_t)v) {}
		Value(int v)		:type(TypeID::Uint), uint((uint64_t)v) {}
		Value(uint8_t v)	:type(TypeID::Uint), uint((uint64_t)v) {}
		Value(uint16_t v)	:type(TypeID::Uint), uint((uint64_t)v) {}
		Value(uint32_t v)	:type(TypeID::Uint), uint((uint64_t)v) {}
		Value(uint64_t v)	:type(TypeID::Uint), uint(v) {}

		Value(std::string_view v) :type(TypeID::StringView), stringview(v) {}
		Value(const std::string& v) :type(TypeID::String), string(v) {}
		Value(const char* v) :type(TypeID::StringView), stringview(v) {}

		Value(const Value& other);

		~Value()
		{
			if (type == TypeID::String)
				string.~basic_string();
			else if (type == TypeID::StringView)
				stringview.~basic_string_view();
		}

		friend std::ostream& operator<<(std::ostream& os, const Value& val);
		inline operator std::string() const
		{
			switch (type)
			{
			case TypeID::Real:
				return std::to_string(real);
			case TypeID::Uint:
				return std::to_string(uint);
			case TypeID::StringView:
				return std::string(stringview);
			case TypeID::String:
				return string;
			default:
				return {};
			}
		}
	};

	struct KV
	{
		std::string_view key;
		Value value;
	};

	struct KV_Multi
	{
		std::string_view key;
		size_t values_start{ 0 };
		size_t values_count{ 0 };
	};

	std::string_view header;
	std::vector<KV> attributes;

	std::vector<KV_Multi> properties; // Can have multiple values [list]
	std::vector<Value> allProperties; // All values, stored in order of appearance

	Value getAttribute(std::string_view key) const;
	const KV_Multi* getProperty(std::string_view key) const;
};

struct INI_File
{
	std::string file; // Text Data, Will be referenced in SectionView
	std::vector<SectionView> sections;

	const SectionView* getSection(std::string_view name, uint32_t appearanceIdx = 0);
};

class INI_Parser
{
public:
	static void ReadFile(INI_File& outData, const std::filesystem::path& projectPath);
	static void Print(const INI_File& data);
	static void WriteFile(const INI_File& data, const std::filesystem::path& projectPath);

private:
	// Helper/Utils functions
	static std::string_view Trim(std::string_view str);
	static void SplitPropertiesList(std::vector<SectionView::Value>& result, std::string_view s);
};

YEASTEM_END
