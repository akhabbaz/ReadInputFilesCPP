// CSVRead.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "IO.h"
#include "histogram.h"
#include  "std_lib_facilities.h"
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
    readcvsfile(filename, data);
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
    if (!ist) error("open file error");
    bool success;// read first line
    IO::read<std::string>(ist, data.header1, success, eolfunction, eolfunction, IO::isComma);
    IO::fill_vector<std::string>(ist, data.header2, IO::isComma, eolfunction, IO::falseFunction);
    std::cout << data.header1 << std::endl;
    for_each(data.header2.begin(), data.header2.end(), printstring);
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
