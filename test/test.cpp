#include "mdl.hpp"
#include <iostream>

int main()
{
	MDLF test(RawFile("test.mdl"));
	/*
	std::cout << "Printing tags for testing:\n";
	std::map<std::string, std::string> t = test.getParsedTags();
	for(auto &it : t)
	{
		std::cout << it.first << ": " << it.second << "\n";
	}
	std::cout << "Finished printing tags.\n";
	*/
	test.addTag("newTag1", "mongrel pup");
	test.update();
	std::cout << "Added a new tag by the name of tag1.\n";
	//std::map<std::string, std::string> tags = test.getParsedTags();
	std::cout << "Does the tag newTag1 exist in the list after updating it? " << test.existsTag("newTag1") << "\n";
	std::cout << "The value stored in newTag1: " << test.getTag("newTag1") << "\n";
	std::cout << "\n\nRemoving newTag1...\n";
	test.deleteTag("newTag1");
	test.update();
	std::cout << "Does the tag newTag1 exist in the list after updating it? " << test.existsTag("newTag1") << "\n";
	return 0;
}