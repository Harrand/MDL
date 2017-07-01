#include "mdl.hpp"

RawFile::RawFile(std::string path): path(path){}

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
	this->parsedTags.clear();
	this->parsedSequences.clear();
	parse();
}

bool MDLF::existsTag(std::string tagName) const
{
	for(auto &iter : getParsedTags())
		if(iter.first == tagName)
			return true;
	return false;
}

bool MDLF::existsSequence(std::string sequenceName) const
{
	for(auto &iter : getParsedSequences())
		if(iter.first == sequenceName)
			return true;
	return false;
}

void MDLF::addTag(std::string tagName, std::string data) const
{
	rf.write(tagName + ": " + data, false);
}

void MDLF::addSequence(std::string sequenceName, std::vector<std::string> data) const
{
	rf.write(sequenceName + ": %[", false);
	if(data.size() > 0)
		for(unsigned int i = 0; i < data.size(); i++)
		{
			std::string suffix = (i == data.size() - 1) ? "]%" : "";
			rf.write("- " + data.at(i) + suffix, false);
		}
	else
		rf.write("]%", false);
}

void MDLF::deleteTag(std::string tagName) const
{
	if(existsTag(tagName))
	{
		auto splitString = [](std::string s, char d) -> std::vector<std::string>{std::istringstream ss(s);std::string t;std::vector<std::string> sp;while(std::getline(ss, t, d))	sp.push_back(t);return sp;};
		auto getTagName = [&splitString](std::string tag) -> std::string{std::string r;std::vector<std::string> sp = splitString(tag, ':');if(sp.size() < 2) return "0";return sp.at(0);};
		std::vector<std::string> lines = rf.getLines();
		for(unsigned int i = 0; i < lines.size(); i++)
		{
			std::string s = lines.at(i);
			if(getTagName(s) == tagName)
			{
				rf.writeLine("", i);
				i++;
			}
		}
	}
}

void MDLF::deleteSequence(std::string sequenceName) const
{
	if(existsSequence(sequenceName))
	{
		std::vector<std::string> lines = rf.getLines();
		for(unsigned int i = 0; i < lines.size(); i++)
		{
			std::string s = lines.at(i);
			if(getTagName(s) == sequenceName)
			{
				unsigned int sequenceSize = getSequences(lines, i).size();
				for(unsigned int j = 0; j <= sequenceSize; j++)
				{
					rf.writeLine("", i + j);
				}
			}
		}
	}
}

void MDLF::editTag(std::string tagName, std::string data) const
{
	this->deleteTag(tagName);
	this->addTag(tagName, data);
}

void MDLF::editSequence(std::string sequenceName, std::vector<std::string> data) const
{
	this->deleteSequence(sequenceName);
	this->addSequence(sequenceName, data);
}

std::string MDLF::getTag(std::string tagName) const
{
	for(auto &iter : getParsedTags())
		if(iter.first == tagName)
			return iter.second;
	return "0";
}

std::vector<std::string> MDLF::getSequence(std::string sequenceName) const
{
	for(auto &iter : getParsedSequences())
		if(iter.first == sequenceName)
			return iter.second;
	return std::vector<std::string>();
}

std::map<std::string, std::string> MDLF::getParsedTags() const
{
	return this->parsedTags;
}

std::map<std::string, std::vector<std::string>> MDLF::getParsedSequences() const
{
	return this->parsedSequences;
}

std::vector<std::string> MDLF::splitString(std::string s, char d) const
{
	std::istringstream ss(s);
	std::string t;
	std::vector<std::string> sp;
	while(std::getline(ss, t, d))
		sp.push_back(t);
	return sp;
}

std::string MDLF::getTagName(std::string tag) const
{
	std::string r;
	std::vector<std::string> sp = splitString(tag, ':');
	if(sp.size() < 2) 
		return "0";
	return sp.at(0);
}

bool MDLF::hasEnding(std::string s, std::string e) const
{
	if(s.length() >= e.length())
	{
		return (0 == s.compare (s.length() - e.length(), e.length(), e));
	}
	else
	{
		return false;
	};
}

bool MDLF::hasBeginning(std::string s, std::string b) const
{
	return s.compare(0, b.length(), b) == 0;
}

bool MDLF::isSequence(std::string s) const
{
	return s.find(": ") != std::string::npos && hasEnding(s, "%[");
}

bool MDLF::isEndOfSequence(std::string s) const
{
	return hasEnding(s, "]%");
}

std::vector<std::string> MDLF::getSequences(std::vector<std::string> lines, unsigned int index) const
{
	bool end = false;
	std::vector<std::string> r;
	if(!isSequence(lines.at(index)))
		return r;
	while(++index < lines.size() && end == false)
	{
		std::string cur = lines.at(index);
		if(hasBeginning(cur, "- "))
		{
			cur.erase(0, 2);
			if(isEndOfSequence(cur))
			{
				cur.erase(cur.length() - 2, 2);
				end = true;
			}
			r.push_back(cur);
		}
	}
	return r;
}

void MDLF::parse() const
{
	std::vector<std::string> lines = rf.getLines();
	auto isComment = [](std::string l) -> bool{return l.c_str()[0] == '#';};
	auto isTag = [&](std::string s) -> bool{return s.find(": ") != std::string::npos && !isSequence(s);};
	//getValue is necessary for tag but not for sequences. This is because the retrieval function for sequences is also used in the deletion functions, so therefore I used a private member function instead of a local lambda.
	auto getValue = [&](std::string s) -> std::string{std::string r;std::vector<std::string> sp = splitString(s, ':');if(sp.size() < 2) return "0";for(unsigned int i = 1; i < sp.size(); i++){sp.at(i).erase(0, 1);r += sp.at(i);}return r;};
	for(unsigned int i = 0; i < lines.size(); i++)
	{
		std::string line = lines.at(i);
		if(isComment(line))
		{
			continue;
		}
		if(isTag(line))
		{
			this->parsedTags[getTagName(line)] = getValue(line);
		}
		if(isSequence(line))
		{
			this->parsedSequences[getTagName(line)] = getSequences(lines, i);
		}
	}
}