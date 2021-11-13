// ReadInputFiles.cpp :
#include <iostream>
#include <iostream>
#include <fstream>
#include "IO.h"
#include <algorithm>

std::string  filename = "election_ballots_1.csv";
struct CSVRead {
    std::string header1;
    vector<std::string> header2;
    typedef vector<long long> oneLine;
    vector<oneLine>   datafile;
};
void readcvsfile(const std::string filename, CSVRead& data);
int main()
{
    std::cout << "Hello World!\n";
    CSVRead data;
    int i;
    std::cin >> i;
    std::cout << std::cin.good() << ' ';
    std::cout << i << std::endl;
    // readcvsfile(filename, data);
}
bool eolfunction(char ch)
{
    return ch == '\n';
}
bool whitespace(char ch)
{
    return ch == ' ';
}
void printstring(const string& str)
{
    std::cout << str << ' ';
}
void readcvsfile(const std::string filename, CSVRead& data)
{
    std::ifstream ist(filename.c_str());
    if (!ist) std::runtime_error("open file error");
    bool success;// read first line
    IO::read<std::string>(ist, data.header1, success, eolfunction, eolfunction, IO::isComma);
    IO::fill_vector<std::string>(ist, data.header2, IO::isComma, eolfunction, IO::falseFunction);
    std::cout << data.header1 << std::endl;
    std::for_each(data.header2.begin(), data.header2.end(), printstring);
    std::cout << std::endl;
    while (ist.good())
    {
        std::vector<long long> one_line;
        IO::fill_vector(ist, one_line, IO::isComma, eolfunction, IO::falseFunction);
        if (!ist)
            data.datafile.push_back(one_line);
    }
}

#include "IO.cpp"
