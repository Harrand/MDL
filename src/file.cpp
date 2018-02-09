#include "file.hpp"
#include <fstream>
#include <sstream>

File::File(std::string path): path(std::move(path)){}

std::string_view File::get_path() const
{
	return {this->path.c_str(), this->path.length()};
}

std::vector<std::string> File::get_lines() const
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

std::string File::get_data() const
{
	std::stringstream sstr;
    sstr << std::ifstream(path.c_str()).rdbuf();
    return sstr.str();
}

void File::clear() const
{
	std::ofstream out;
	out.open((this->path).c_str(), std::ofstream::out | std::ofstream::trunc);
}

void File::write(std::string data, bool clear) const
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

void File::write_line(std::string data, std::size_t line) const
{
	std::vector<std::string> lines = this->get_lines();
	if(line >= lines.size())
	{
		this->write(data, false);
		return;
	}
	lines.at(line) = data;
	this->clear();
	for(std::size_t i = 0; i < lines.size(); i++)
		this->write(lines.at(i), false);
}