#include "xml_file.hpp"

XMLFile::XMLFile(std::string file_path): XMLFile(File(std::move(file_path))){}

XMLFile::XMLFile(File raw_file): raw_file(std::move(raw_file)){}

XMLFile::XMLFile(MDLFile mdl_file)
{
    for(const auto& [tag_name, tag_value] : mdl_file.get_parsed_tags())
    {

    }
}
