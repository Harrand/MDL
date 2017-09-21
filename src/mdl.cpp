#include "mdl.hpp"

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

MDLF::MDLF(RawFile rf): rf(rf)
{
	parse();
}

const RawFile& MDLF::getRawFile() const
{
	return this->rf;
}

void MDLF::update() const
{
	this->parsed_tags.clear();
	this->parsed_sequences.clear();
	parse();
}

bool MDLF::existsTag(const std::string& tag_name) const
{
	for(auto &iter : getParsedTags())
		if(iter.first == tag_name)
			return true;
	return false;
}

bool MDLF::existsSequence(const std::string& sequence_name) const
{
	for(auto &iter : getParsedSequences())
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
		for(unsigned int i = 0; i < data.size(); i++)
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
		for(unsigned int i = 0; i < lines.size(); i++)
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
		for(unsigned int i = 0; i < lines.size(); i++)
		{
			std::string s = lines.at(i);
			if(mdl::util::getTagName(s) == sequence_name)
			{
				unsigned int sequenceSize = mdl::util::getSequences(lines, i).size();
				for(unsigned int j = 0; j <= sequenceSize; j++)
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
	for(auto &iter : getParsedTags())
		if(iter.first == tag_name)
			return iter.second;
	return "0";
}

std::vector<std::string> MDLF::getSequence(const std::string& sequence_name) const
{
	for(auto &iter : getParsedSequences())
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

void MDLF::parse() const
{
	std::vector<std::string> lines = rf.getLines();
	auto isComment = [](std::string l) -> bool{return l.c_str()[0] == '#';};
	auto isTag = [&](std::string s) -> bool{return s.find(": ") != std::string::npos && !mdl::util::isSequence(s);};
	//getValue is necessary for tag but not for sequences. This is because the retrieval function for sequences is also used in the deletion functions, so therefore I used a private member function instead of a local lambda.
	auto getValue = [&](std::string s) -> std::string{std::string r;std::vector<std::string> sp = mdl::util::splitString(s, ':');if(sp.size() < 2) return "0";for(unsigned int i = 1; i < sp.size(); i++){sp.at(i).erase(0, 1);r += sp.at(i);}return r;};
	for(unsigned int i = 0; i < lines.size(); i++)
	{
		std::string line = lines.at(i);
		if(isComment(line))
		{
			continue;
		}
		if(isTag(line))
		{
			this->parsed_tags[mdl::util::getTagName(line)] = getValue(line);
		}
		if(mdl::util::isSequence(line))
		{
			this->parsed_sequences[mdl::util::getTagName(line)] = mdl::util::getSequences(lines, i);
		}
	}
}

namespace mdl
{
	namespace util
	{
		std::vector<std::string> splitString(std::string string, char delimiter)
		{
			std::istringstream ss(string);
			std::string t;
			std::vector<std::string> sp;
			while(std::getline(ss, t, delimiter))
				sp.push_back(t);
			return sp;
		}
		
		std::string getTagName(std::string tag)
		{
			std::string r;
			std::vector<std::string> sp = mdl::util::splitString(tag, ':');
			if(sp.size() < 2) 
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
		
		bool isSequence(const std::string& s)
		{
			return s.find(": ") != std::string::npos && mdl::util::endsWith(s, "%[");
		}
		
		bool isEndOfSequence(const std::string& s)
		{
			return mdl::util::endsWith(s, "]%");
		}
		
		std::vector<std::string> getSequences(std::vector<std::string> lines, std::size_t index)
		{
			bool end = false;
			std::vector<std::string> r;
			if(!mdl::util::isSequence(lines.at(index)))
				return r;
			while(++index < lines.size() && end == false)
			{
				std::string cur = lines.at(index);
				if(mdl::util::beginsWith(cur, "- "))
				{
					cur.erase(0, 2);
					if(mdl::util::isEndOfSequence(cur))
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