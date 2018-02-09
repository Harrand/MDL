#ifndef XML_FILE_HPP
#define XML_FILE_HPP
#include "mdl_file.hpp"

class XMLFile
{
public:
    explicit XMLFile(std::string file_path);
    explicit XMLFile(File raw_file = File());
    explicit XMLFile(MDLFile mdl_file);

    XMLFile(const XMLFile& copy) = default;
    XMLFile(XMLFile&& move) = default;
    ~XMLFile() = default;
    XMLFile& operator=(const XMLFile& rhs) = default;
private:
    File raw_file;
};

#endif
