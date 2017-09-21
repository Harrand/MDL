#include "mdl.hpp"
#include <fstream>
#include <sstream>

RawFile::RawFile(std::string path): path(std::move(path)){}

const std::string& RawFile::getPath() const
{
	return this->path;
}

std::vector<std::string> RawFile::getLines() const
{
	std::ifstream file(this->path.c_str());
	std::string line;
	std::vector<std::string> lines;
	while (std::getline(file, line))
	{
		lines.push_back(line);
	}
	return lines;
}

std::string RawFile::getData() const
{
	std::stringstream sstr;
    sstr << std::ifstream(path.c_str()).rdbuf();
    return sstr.str();
}

void RawFile::clear() const
{
	std::ofstream out;
	out.open((this->path).c_str(), std::ofstream::out | std::ofstream::trunc);
}

void RawFile::write(std::string data, bool clear) const
{
	std::ofstream out;
	if(clear)
		out.open((this->path).c_str(), std::ofstream::out | std::ofstream::trunc);
	else
		out.open((this->path).c_str(), std::ofstream::out | std::ofstream::app);
	if(data != "")
		out << (data + "\n").c_str();
	out.close();
}

void RawFile::writeLine(std::string data, unsigned int line) const
{
	std::vector<std::string> lines = this->getLines();
	if(line >= lines.size())
	{
		this->write(data, false);
		return;
	}
	lines.at(line) = data;
	this->clear();
	for(unsigned int i = 0; i < lines.size(); i++)
	{
		std::string l = lines.at(i);
		this->write(l, false);
	}
}

MDLF::MDLF(std::string file_path): MDLF(RawFile(file_path)){}

MDLF::MDLF(RawFile rf): rf(rf)
{
	this->update();
}

const RawFile& MDLF::getRawFile() const
{
	return this->rf;
}

bool MDLF::existsTag(const std::string& tag_name) const
{
	for(auto iter : getParsedTags())
		if(iter.first == tag_name)
			return true;
	return false;
}

bool MDLF::existsSequence(const std::string& sequence_name) const
{
	for(auto iter : getParsedSequences())
		if(iter.first == sequence_name)
			return true;
	return false;
}

void MDLF::addTag(std::string tag_name, std::string data) const
{
	rf.write(tag_name + ": " + data, false);
}

void MDLF::addSequence(std::string sequence_name, std::vector<std::string> data) const
{
	rf.write(sequence_name + ": %[", false);
	if(data.size() > 0)
		for(std::size_t i = 0; i < data.size(); i++)
		{
			std::string suffix = (i == data.size() - 1) ? "]%" : "";
			rf.write("- " + data.at(i) + suffix, false);
		}
	else
		rf.write("]%", false);
}

void MDLF::deleteTag(std::string tag_name) const
{
	if(existsTag(tag_name))
	{
		std::vector<std::string> lines = rf.getLines();
		for(std::size_t i = 0; i < lines.size(); i++)
		{
			std::string s = lines.at(i);
			if(mdl::util::getTagName(s) == tag_name)
			{
				rf.writeLine("", i);
				i++;
			}
		}
	}
}

void MDLF::deleteSequence(std::string sequence_name) const
{
	if(existsSequence(sequence_name))
	{
		std::vector<std::string> lines = rf.getLines();
		for(std::size_t i = 0; i < lines.size(); i++)
		{
			std::string s = lines.at(i);
			if(mdl::util::getTagName(s) == sequence_name)
			{
				std::size_t sequence_size = mdl::util::findSequence(lines, i).size();
				for(std::size_t j = 0; j <= sequence_size; j++)
				{
					rf.writeLine("", i + j);
				}
			}
		}
	}
}

void MDLF::editTag(std::string tag_name, std::string data) const
{
	this->deleteTag(tag_name);
	this->addTag(tag_name, data);
}

void MDLF::editSequence(std::string sequence_name, std::vector<std::string> data) const
{
	this->deleteSequence(sequence_name);
	this->addSequence(sequence_name, data);
}

std::string MDLF::getTag(const std::string& tag_name) const
{
	for(auto iter : getParsedTags())
		if(iter.first == tag_name)
			return iter.second;
	return "0";
}

std::vector<std::string> MDLF::getSequence(const std::string& sequence_name) const
{
	for(auto iter : getParsedSequences())
		if(iter.first == sequence_name)
			return iter.second;
	return std::vector<std::string>();
}

const std::map<std::string, std::string>& MDLF::getParsedTags() const
{
	return this->parsed_tags;
}

const std::map<std::string, std::vector<std::string>>& MDLF::getParsedSequences() const
{
	return this->parsed_sequences;
}

void MDLF::update() const
{
	this->parsed_tags.clear();
	this->parsed_sequences.clear();
	std::vector<std::string> lines = rf.getLines();
	for(std::size_t i = 0; i < lines.size(); i++)
	{
		std::string line = lines.at(i);
		if(mdl::syntax::isComment(line))
			continue;
		if(mdl::syntax::isTag(line))
			this->parsed_tags[mdl::util::getTagName(line)] = mdl::util::findTagValue(line);
		if(mdl::syntax::isSequence(line))
			this->parsed_sequences[mdl::util::getTagName(line)] = mdl::util::findSequence(lines, i);
	}
}

namespace mdl
{
	namespace syntax
	{
		bool isComment(const std::string& line)
		{
			return line.c_str()[0] == '#';
		}
		
		bool isTag(const std::string& line)
		{
			return line.find(": ") != std::string::npos && !mdl::syntax::isSequence(line);
		}
		
		bool isSequence(const std::string& line)
		{
			return line.find(": ") != std::string::npos && mdl::util::endsWith(line, "%[");
		}
		
		bool isEndOfSequence(const std::string& line)
		{
			return mdl::util::endsWith(line, "]%");
		}
	
	}
	
	namespace util
	{
		std::vector<std::string> splitString(const std::string& string, const std::string& delimiter)
		{
			std::vector<std::string> v;
			// Start of an element.
			std::size_t element_start = 0;
			// We start searching from the end of the previous element, which
			// initially is the start of the string.
			std::size_t element_end = 0;
			// Find the first non-delim, i.e. the start of an element, after the end of the previous element.
			while((element_start = string.find_first_not_of(delimiter, element_end)) != std::string::npos)
			{
				// Find the first delem, i.e. the end of the element (or if this fails it is the end of the string).
				element_end = string.find_first_of(delimiter, element_start);
				// Add it.
				v.emplace_back(string, element_start, element_end == std::string::npos ? std::string::npos : element_end - element_start);
			}
			// When there are no more non-spaces, we are done.
			return v;
		}
		
		std::string getTagName(std::string tag)
		{
			std::string r;
			std::vector<std::string> sp = mdl::util::splitString(tag, ":");
			constexpr std::size_t minimum_split_quantity = 2;
			if(sp.size() < minimum_split_quantity) 
				return "0";
			return sp.at(0);
		}
		
		bool endsWith(const std::string& string, const std::string& suffix)
		{
			if(string.length() >= suffix.length())
				return (0 == string.compare(string.length() - suffix.length(), suffix.length(), suffix));
			else
				return false;
		}
		
		bool beginsWith(const std::string& string, const std::string& prefix)
		{
			return string.compare(0, prefix.length(), prefix) == 0;
		}
		
		std::string findTagValue(std::string line)
		{
			std::string r;
			std::vector<std::string> sp = mdl::util::splitString(line, ":");
			constexpr std::size_t minimum_split_quantity = 2;
			if(sp.size() < minimum_split_quantity)
				return "0";
			for(std::size_t i = 1; i < sp.size(); i++)
			{
				sp.at(i).erase(0, 1);
				r += sp.at(i);
			}
			return r;
		}
		
		std::vector<std::string> findSequence(std::vector<std::string> lines, std::size_t index)
		{
			bool end = false;
			std::vector<std::string> r;
			if(!mdl::syntax::isSequence(lines.at(index)))
				return r;
			while(++index < lines.size() && end == false)
			{
				std::string cur = lines.at(index);
				if(mdl::util::beginsWith(cur, "- "))
				{
					cur.erase(0, 2);
					if(mdl::syntax::isEndOfSequence(cur))
					{
						cur.erase(cur.length() - 2, 2);
						end = true;
					}
					r.push_back(cur);
				}
			}
			return r;
		}
	}
}