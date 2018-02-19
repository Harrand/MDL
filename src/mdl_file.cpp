#include "mdl_file.hpp"

MDLFile::MDLFile(std::string file_path): File(std::move(file_path))
{this->update();}

bool MDLFile::exists_tag(const std::string& tag_name) const
{
	return this->parsed_tags.find(tag_name) != this->parsed_tags.end();
}

bool MDLFile::exists_sequence(const std::string& sequence_name) const
{
	return this->parsed_sequences.find(sequence_name) != this->parsed_sequences.end();
}

void MDLFile::add_tag(std::string tag_name, std::string data)
{
	this->write(tag_name + ": " + data, false);
	this->parsed_tags[tag_name] = data;
}

void MDLFile::add_sequence(std::string sequence_name, std::vector<std::string> data)
{
	this->write(sequence_name + ": %[", false);
	if(!data.empty())
		for(std::size_t i = 0; i < data.size(); i++)
		{
			std::string suffix = (i == data.size() - 1) ? "]%" : "";
			this->write("- " + data.at(i) + suffix, false);
		}
	else
		this->write("]%", false);
	this->parsed_sequences[sequence_name] = data;
}

void MDLFile::delete_tag(std::string tag_name)
{
	if(exists_tag(tag_name))
	{
		std::vector<std::string> lines = this->get_lines();
		for(std::size_t i = 0; i < lines.size(); i++)
		{
			std::string s = lines.at(i);
			if(mdl::util::find_tag_name(s) == tag_name)
			{
				this->write_line("", i);
				i++;
			}
		}
		this->parsed_tags.erase(tag_name);
	}
}

void MDLFile::delete_sequence(std::string sequence_name)
{
	if(exists_sequence(sequence_name))
	{
		std::vector<std::string> lines = this->get_lines();
		for(std::size_t i = 0; i < lines.size(); i++)
		{
			std::string s = lines.at(i);
			if(mdl::util::find_tag_name(s) == sequence_name)
			{
				std::size_t sequence_size = mdl::util::find_sequence_values(lines, i).size();
				for(std::size_t j = 0; j <= sequence_size; j++)
				{
					this->write_line("", i + j);
				}
			}
		}
		this->parsed_tags.erase(sequence_name);
	}
}

void MDLFile::edit_tag(std::string tag_name, std::string data)
{
	this->delete_tag(tag_name);
	this->add_tag(tag_name, std::move(data));
}

void MDLFile::edit_sequence(std::string sequence_name, std::vector<std::string> data)
{
	this->delete_sequence(sequence_name);
	this->add_sequence(sequence_name, std::move(data));
}

std::string MDLFile::get_tag(const std::string& tag_name) const
{
    if(this->exists_tag(tag_name))
	    return this->get_parsed_tags().at(tag_name);
    return mdl::default_string;
}

std::vector<std::string> MDLFile::get_sequence(const std::string& sequence_name) const
{
    if(this->exists_sequence(sequence_name))
        return this->get_parsed_sequences().at(sequence_name);
    return {};
}

const std::map<std::string, std::string>& MDLFile::get_parsed_tags() const
{
	return this->parsed_tags;
}

const std::map<std::string, std::vector<std::string>>& MDLFile::get_parsed_sequences() const
{
	return this->parsed_sequences;
}

void MDLFile::update()
{
	this->parsed_tags.clear();
	this->parsed_sequences.clear();
	std::vector<std::string> lines = this->get_lines();
	for(std::size_t i = 0; i < lines.size(); i++)
	{
		std::string line = lines.at(i);
		if(mdl::syntax::is_comment(line))
			continue;
		if(mdl::syntax::is_tag(line))
			this->parsed_tags[mdl::util::find_tag_name(line)] = mdl::util::find_tag_value(line);
		if(mdl::syntax::is_sequence(line))
			this->parsed_sequences[mdl::util::find_sequence_name(line)] = mdl::util::find_sequence_values(lines, i);
	}
}

namespace mdl
{
	std::vector<std::string> read_lines(const std::string& filename)
	{
		return File(filename).get_lines();
	}
	
	std::string read(const std::string& filename)
	{
		return File(filename).get_data();
	}
	
	namespace syntax
	{
		bool is_comment(const std::string& line)
		{
			return line.c_str()[0] == '#';
		}
		
		bool is_tag(const std::string& line)
		{
			return line.find(": ") != std::string::npos && !mdl::syntax::is_sequence(line);
		}
		
		bool is_sequence(const std::string& line)
		{
			return line.find(": ") != std::string::npos && mdl::util::ends_with(line, "%[");
		}
		
		bool is_end_of_sequence(const std::string& line)
		{
			return mdl::util::ends_with(line, "]%");
		}
	
	}
	
	namespace util
	{
		std::vector<std::string> split_string(const std::string& string, const std::string& delimiter)
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
		
		bool ends_with(const std::string& string, const std::string& suffix)
		{
			if(string.length() >= suffix.length())
				return (0 == string.compare(string.length() - suffix.length(), suffix.length(), suffix));
			else
				return false;
		}
		
		bool begins_with(const std::string& string, const std::string& prefix)
		{
			return string.compare(0, prefix.length(), prefix) == 0;
		}
		
		std::string find_tag_name(const std::string& line)
		{
			std::string r;
			std::vector<std::string> sp = mdl::util::split_string(line, ":");
			constexpr std::size_t minimum_split_quantity = 2;
			if(sp.size() < minimum_split_quantity) 
				return mdl::default_string;
			return sp.at(0);
		}
		
		std::string find_tag_value(const std::string& line)
		{
			std::string r;
			std::vector<std::string> sp = mdl::util::split_string(line, ":");
			constexpr std::size_t minimum_split_quantity = 2;
			if(sp.size() < minimum_split_quantity)
				return mdl::default_string;
			for(std::size_t i = 1; i < sp.size(); i++)
			{
				sp.at(i).erase(0, 1);
				r += sp.at(i);
			}
			return r;
		}
		
		std::string find_sequence_name(const std::string& line)
		{
			// Identical to finding tag name
			return find_tag_name(line);
		}
		
		std::vector<std::string> find_sequence_values(const std::vector<std::string>& lines, std::size_t index)
		{
			bool end = false;
			std::vector<std::string> r;
			if(!mdl::syntax::is_sequence(lines.at(index)))
				return r;
			while(++index < lines.size() && !end)
			{
				std::string cur = lines.at(index);
				if(mdl::util::begins_with(cur, "- "))
				{
					cur.erase(0, 2);
					if(mdl::syntax::is_end_of_sequence(cur))
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