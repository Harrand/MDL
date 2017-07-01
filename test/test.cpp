#include "mdl.hpp"
#include <iostream>

#define TEST_STOP "stop"
#define TEST_INVALID "invalid"

std::string inputCommand(std::string cmd, MDLF& mdlf)
{
	auto splitString = [](std::string s, char d) -> std::vector<std::string>{std::istringstream ss(s);std::string t;std::vector<std::string> sp;while(std::getline(ss, t, d))sp.push_back(t);return sp;};
	std::vector<std::string> splitCmd = splitString(cmd, ' ');
	std::string cmdName = splitCmd.at(0);
	if(cmdName == "stop" || cmdName == "exit")
		return TEST_STOP;
	else if(cmdName == "path")
		return mdlf.getRawFile().getPath() + "\n";
	else if(cmdName == "addTag")
	{
		if(splitCmd.size() < 3)
			return TEST_INVALID;
		std::string tagName = splitCmd.at(1);
		std::string tagValue = "";
		for(unsigned int i = 2; i < splitCmd.size(); i++)
			i == (splitCmd.size() - 1) ? tagValue.append(splitCmd.at(i)) : tagValue.append(splitCmd.at(i) + " ");
		mdlf.addTag(tagName, tagValue);
		return "Added the tag named '" + tagName + "' with the value '" + tagValue + "'.\n";
	}
	else if(cmdName == "addSequence")
	{
		if(splitCmd.size() < 3)
			return TEST_INVALID;
		std::string seqName = splitCmd.at(1);
		std::vector<std::string> seq;
		for(unsigned int i = 2; i < splitCmd.size(); i++)
			seq.push_back(splitCmd.at(i));
		mdlf.addSequence(seqName, seq);
		std::ostringstream ss;
		ss << seq.size();
		return "Added the sequence named '" + seqName + "' with sequence-size " + ss.str() + ".\n";
	}
	else if(cmdName == "deleteTag")
	{
		if(splitCmd.size() < 2)
			return TEST_INVALID;
		std::string tagName = splitCmd.at(1);
		mdlf.deleteTag(tagName);
		return "Attempted to delete the tag named '" + tagName + "'.\n";
	}
	else if(cmdName == "deleteSequence")
	{
		if(splitCmd.size() < 2)
			return TEST_INVALID;
		std::string seqName = splitCmd.at(1);
		mdlf.deleteSequence(seqName);
		return "Attempted to delete the sequence named '" + seqName + "'.\n";
	}
	else if(cmdName == "getTag")
	{
		if(splitCmd.size() < 2)
			return TEST_INVALID;
		std::string tagName = splitCmd.at(1);
		return mdlf.getTag(tagName) + "\n";
	}
	else if(cmdName == "getSequence")
	{
		if(splitCmd.size() < 2)
			return TEST_INVALID;
		std::string seqName = splitCmd.at(1);
		std::vector<std::string> seq = mdlf.getSequence(seqName);
		std::string ret = "";
		for(unsigned int i = 0; i < seq.size(); i++)
			i == (seq.size() -1) ? ret.append(seq.at(i) + "\n") : ret.append(seq.at(i) + ", ");
		return ret;
	}
	else if(cmdName == "tags")
	{
		std::map<std::string, std::string> tags = mdlf.getParsedTags();
		std::string tagStr = "";
		for(auto &iter : tags)
			tagStr.append(iter.first + ", " + iter.second + "\n");
		return tagStr;
	}
	else if(cmdName == "sequences")
	{
		std::map<std::string, std::vector<std::string>> seqs = mdlf.getParsedSequences();
		std::string seqStr = "";
		for(auto &iter : seqs)
		{
			std::string seqName = iter.first + "\n";
			std::string seq = "";
			for(unsigned int i = 0; i < iter.second.size(); i++)
				seq.append("- " + iter.second.at(i) + "\n");
			seqStr.append(seqName + seq + "\n");
		}
		return seqStr;
	}
	else if(cmdName == "update")
	{
		mdlf.update();
		return "Updated.\n";
	}
	else
		return "Unknown command.\n";
}

int main(int argc, char** argv)
{
	std::string filename = "";
	if(argc < 2)
	{
		std::cout << "Did not receive a custom filename for the data file. Using 'test.mdl' as default.\n";
		filename = "test.mdl";
	}
	else
	{
		for(unsigned int i = 1; i < argc; i++)
			filename.append(argv[i]);
		std::cout << "Using '" << filename << "'.\n";
	}
	MDLF input = MDLF(RawFile(filename));
	std::string cmd, status;
	while(status != TEST_STOP)
	{
		std::cout << "Enter a command:\n";
		std::getline(std::cin, cmd);
		status = inputCommand(cmd, input);
		std::cout << "\n" << status;
		//blah
	}
	return 0;
}