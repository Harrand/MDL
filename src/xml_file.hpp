#ifndef XML_FILE_HPP
#define XML_FILE_HPP
#include "file.hpp"

class XMLFile
{
    explicit XMLFile(std::string file_path);
    explicit XMLFile(File raw_file = File());
};

#endif
