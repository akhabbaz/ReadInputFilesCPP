// ReadInputFiles.cpp :
#include <iostream>
#include <fstream>
#include "IO.h"
#include <algorithm>
#include <stdexcept>
#include <sstream>


//std::string  filename = "election_ballots_1.csv";
// std::string  filename = "OneLineSpaces.txt";

// this structure is read by first reading the name as a 
// single string terminated according to the read, and then
// parsing the string using >>.  ReadStream<string> will take care
// to get the terminator, trim and line end. Reading this record is 
// straightforward because no need to worry about
// about removing terminators from the stream. The format of the record
// for reading is below in the >> function and it is simple.
struct Name {
    string first{}, last{};
    Name(string f, string l) : first{ f }, last{ l }{}
    Name():first{defaultName.first}, last{defaultName.last}{}
    Name(const string& str); // initialize with string
    // return name as a string "Emily Dickinson"
    string toString(void) const;
    static Name defaultName;
    static void updateDefaultName(const Name& defaultname);
};
// This structures goal is to use ReadStream<Life> to read. As a result,
// we need to make sure if the read fails important terminators and data 
// from the stream won't get removed inadvertently. This read >> takes care
// to avoid ReadStream<Life> terminators. For this to work easily, the following
// rules should be observed-- Preconditions:
//       1. The record should have a character start and stop terminator.
//       2. The record should not use the stream terminator or eol since if
//               the read fails and these  characters remain on the stream
//               they will mess up the ReadStream.
//       3. It should have the same trim characters as the read. I use same
//              remove function as used in ReadStream. 
//       4. I recommend not setting skip white space.  That could remove the eol 
//               character and so mess up the vector read.
//       5.  If this read fails I leave readStream to remove characters
struct Life {
    unsigned born{}, died{};
    static IO::TestChar  sep; // use the separators to remove chars
                               // while reading;
    static Life defaultLife;
    Life(unsigned b, unsigned d):born{b}, died{d}{}
    Life() { *this = defaultLife; }
    Life(const string& str); // initialize with a string
    static void updateDefaultLife(const Life&);
    static void updateSeparators(IO::TestChar& newSep);// update the separator code
};
// a struct to hold all the CSV data on art and data
struct CSVRead {
    vector<std::string> header1;
    vector<Name> names;// names of artists
    vector<std::string>  work;//a work they are known for
    vector<Life>     lifespan; // lifespan
    typedef vector<long long> oneLine;
    vector<oneLine>   datafile;
};
// holds a first and last name

istream& operator>>(istream& i, Name& nm);
ostream& operator<<(ostream& i, const Name& nm);
istream& operator>>(istream& i, Life& nm);
ostream& operator<<(ostream& i, const Life& nm);
// structure for reading an integer and a string
//holds the input to get all the defaults for the CSV file
struct DefaultRecord {
    long long val{-1};
    string defValue;
    Name   name;
    Life   span;
};
istream& operator>>(istream& i, DefaultRecord& rec);
void readstringfile(const std::string filename, CSVRead& data);
void readIntfile(const std::string filename, CSVRead& data);
// read default value
void testistreamIterator(const std::string filename);
void readCSVfile(const std::string filename, CSVRead& data);
// specializations to optimize reading for small types
template <typename T> void  print(const vector<T>& vec);
template<>           void print(const vector<int>& vec);
template<>           void print(const vector<long long>& vec); 
Name Name::defaultName{ "Emily", "Dickinson" };

Life Life::defaultLife{ 0, 0 };
 int main(int argc, char* argv[])
{
    if (argc > 5) {
        string filename{ argv[1] };
        CSVRead data;
        readstringfile(filename, data);
        std::cout << "**** End of File 1\n" << std::endl;
        filename = string{ argv[2] };
        testistreamIterator(filename);
        std::cout << "**** End of File 2\n" << std::endl;
        filename = string{ argv[3] };
        data = CSVRead{};
        readIntfile(filename, data);
        std::cout << "**** End of File 3\n" << std::endl;
        filename = string{ argv[4] };
        data = CSVRead{};
        readIntfile(filename, data);
        std::cout << "**** End of File 4\n" << std::endl;
        filename = string{ argv[5] };
        data = CSVRead{};
        readCSVfile(filename, data);
        std::cout << "**** End of File 5" << std::endl;
    }
}

 
void readstringfile(const std::string filename, CSVRead& data)
{
    std::ifstream ist{ filename.c_str() };
    if (ist.fail())
        throw std::runtime_error("open file error");
    // get parameters to determine how to parse file
    string defaultstr{};
    IO::TestChar sep{IO::falseFunction, IO::falseFunction, IO::falseFunction};// separators
    bool skipspace{ false }; // false means no skip white space
    string commentString{};
    readCommandLine(ist, defaultstr, sep, skipspace, commentString);
    std::cout << commentString << std::endl;
    IO::readStream<string> stringRead{ ist, defaultstr,
        sep.term, sep.stop, sep.trim, skipspace };
    stringRead.read(data.header1);
    print(data.header1);
    std::cout << std::endl;
     }
void readIntfile(const std::string filename, CSVRead& data)// example integerReadWithSpaceSep.txt
{
    std::ifstream ist{ filename.c_str() };
    if (ist.fail())
        throw std::runtime_error("open file error");
    // get parameters to determine how to parse file
    int val;
    IO::TestChar sep{ IO::falseFunction, IO::falseFunction, IO::falseFunction };// separators
    bool skipspace{ false }; // false means no skip white space
    string  commentString{};
    readCommandLine(ist, val, sep, skipspace, commentString);
    std::cout << commentString << std::endl;
    IO::readStream<int> intread(ist, val,
        sep.term, sep.stop, sep.trim, skipspace);
    vector<int> oneline;
    intread.read(oneline);
    print(oneline);
    oneline = vector<int>{};
    intread.read(oneline);
    print(oneline);
    if (ist.eof())
        std::cout << "EOf reached" << std::endl;
    else
    {
        oneline = vector<int>{};
        intread.read(oneline);
        print(oneline);
        if (ist.eof()) std::cout << "Eof reached" << std::endl;
    }
}
void testistreamIterator(const std::string filename)
{
    std::ifstream ist{ filename.c_str() };
    if (ist.fail())
        throw std::runtime_error("open file error");
    // get parameters to determine how to parse file
    ist >> std::noskipws;
    using str_it = std::istream_iterator<char>;
    // this it works as follows. 
    // initialization reads a character, takes it out of the stream!
    str_it readchar{ ist};
    str_it eos{};
    std::cout << "This demonstrates stream iterators actually use the same operator as >>" 
        << std::endl;
    while ( readchar != eos)
    {
        // deferencing shows that character
        std::cout << *readchar;
        char ch{};
        std::cout << *readchar;
        // addition advances the stream pulling next character out of stream
        ++readchar;
    }
    std::cout << std::endl;
}
void readCSVfile(const std::string filename, CSVRead& data)
{
    std::ifstream ist{ filename.c_str() };
    if (ist.fail())
        throw std::runtime_error("open file error");
    // get parameters to determine how to parse file
    DefaultRecord rec;
    //Separators needed to read from file in the >> operator
    // a default must be set.  Unfortunately this can't be done in 
    // the global space because in the size is not easy to figure out
    IO::TestChar sep{ IO::isComma, IO::eol, IO::isspace };
    // separators for reading the file
    Life::updateSeparators(sep); //this fails...
    bool skipspace{ false }; // false means no skip white space
    string  commentString{};
    readCommandLine( ist, rec, sep, skipspace, commentString );
    std::cout << commentString << std::endl;
    // update defaults so In cases of failure
    Life::updateDefaultLife(rec.span);
    Name::updateDefaultName(rec.name);
    Life::updateSeparators(sep);
    IO::readStream<string> stringRead{ ist, rec.defValue,
        sep.term, sep.stop, sep.trim, skipspace };
    stringRead.read(data.header1);
    print(data.header1);
    // to read strings that need to be formatted into a name it
    // is easiest to read strings first and then parse each one.
    // This removes the terminator according to the rules and spares
    // the user from repeating that code
    //IO::readStream<Name> nameRead{ ist, rec.name,
     //   sep.term, sep.stop, sep.trim, skipspace };
    vector<string> names;
    string nameString = Name::defaultName.toString();
    stringRead.updateDefault(nameString);
    stringRead.read(names);
    //print(names); printed below better format
    // use constructor to convert string to a name
    auto  convertNames = [&data](const string& in) {
        data.names.push_back(in);
    };
    for_each(names.begin(), names.end(), convertNames);
    print(data.names);
    //next line is again strings
    stringRead.updateDefault(rec.defValue);
    stringRead.read(data.work);
    print(data.work);
    IO::readStream<Life> lifeRead{ ist, rec.span,
        sep.term, sep.stop, sep.trim, skipspace };
    lifeRead.read(data.lifespan);
    print(data.lifespan);
    //now read the data in a loop
    IO::readStream<long long> longRead( ist, rec.val,
        sep.term, sep.stop, sep.trim, skipspace );
    while (ist) {
        CSVRead::oneLine dataVec;
        longRead.read(dataVec);
        print(dataVec);
        data.datafile.push_back(dataVec );
    }
    if (ist.eof()) {
        std::cout << "Eof Found" << std::endl;
    }
}

void Name::updateDefaultName(const Name& name)
{
    defaultName = name;
}
string Name::toString(void) const
{
    return first + " " + last;
}

void Life::updateDefaultLife(const Life& life)
{
    defaultLife = life;
}
void Life::updateSeparators(IO::TestChar& newsep)
{
    Life::sep = newsep;

}
Name::Name(const string& inputstr)
{
    std::istringstream iss{ inputstr };
    iss >> *this;
    if (!iss) // read failed format error
    {
        *this = defaultName;
    }
}
Life::Life(const string& inputstr)
{
    std::istringstream iss{ inputstr };
    iss >> *this;
    if (!iss) // read failed format error
    {
        *this = defaultLife;
    }
}
istream& operator>>(istream& instr, Name& nm)
{
   instr >> std::skipws;
   instr >>  nm.first >> nm.last;
   return instr;
}
ostream& operator<<(ostream& ostr, const Name& nm)
{
    ostr << nm.last << ", " << nm.first;
    return ostr;
}
// here the goal is to read Life from the stream directly, not after 
// separating the string.  That means that if the read fails, this read should not
// start reading important characters from. THe record should be ( 1650 - 1785).
// skipws not used because there may be the end of line there.
istream& operator>>(istream& instr, Life& nm)
{
    auto  termMatch= [&instr](const char ch, const char def) {
        bool success{instr && ch == def };
        if (instr && !success) {
            instr.unget();
            instr.clear(std::ios_base::failbit);
        }
        return success;
    };
        char sep{};
        IO::removeTrimBefore(instr, nm.sep);
        instr >> sep;
        if (!termMatch(sep, '(')) {
            // start not found
            return instr;
        }
        IO::removeTrimBefore(instr, nm.sep); // use the same function
        instr >> nm.born;
        IO::removeTrimBefore(instr, nm.sep); //
        instr >> sep;
        if (!termMatch(sep, '-')) {
            // from to symbol not found
            return instr;
        }
        IO::removeTrimBefore(instr, Life::sep); //
        instr >> nm.died;
        IO::removeTrimBefore(instr, Life::sep); //
        instr >> sep;
        if (!termMatch(sep, ')')) {
            // stop not found
            return instr;
        }
        return instr;
}
ostream& operator<<(ostream& ostr, const Life& nm)
{
    return ostr << '[' << nm.born << " - " <<
       nm.died << ']';

}
istream& operator>>(istream& i, DefaultRecord& rec)
{
    char interm{};
    char outterm{};
    i >> std::skipws;
    i >> interm >> rec.name >> rec.span >>  rec.defValue >> rec.val >> outterm;
    if (interm != '{' || outterm != '}')
        i.clear(std::ios_base::failbit);
    return i;
}
// different terminator shows which print is used!
template <typename T> void  print(const vector<T>& vec)
{
    for (const T& val :vec)
    {
        std::cout << val << " ; ";
    }
    std::cout << std::endl;
}
template<>           void print(const vector<int>& vec)
{
    for (const int val : vec)
    {
        std::cout << val << " ;* ";
    }
    std::cout << std::endl;
}
template<>           void print(const vector<long long>& vec)
{
    for (const long long val : vec)
    {
        std::cout << val << " ;** ";
    }
    std::cout << std::endl;
}

#include "IO.cpp"
// this needs to be here because defaultTestChar is defined in IO.cpp
IO::TestChar Life::sep{ IO::TestChar::defaultTestChar };