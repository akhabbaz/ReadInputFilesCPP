# pragma once
#include <vector>
#include <iostream>
#include <string>

using std::istream;
using std::ostream;
using std::string;
using std::vector;
/* 
Reading text data files is general, but there is an immense variety of text based data files. 
This program is meant to read a large subset of them, including CSV files.

The purpose of this file is to read data text files of various forms that consist of
a columnar arrangement of data. One line has data fields that may be empty separated by a 
terminator function term. The data can be strings or numeric types, but all the same type.
The program has a read function which reads one field and a vector function which reads a series
fields.

Some features that make this general: Matching is done with functions not characters so logic 
or several different characters can make a match. Standard functions like isspace can be used. Next,
the type of read is a template, so anything can be read, any type of variable. The read will be defined
by the >> operator. Lastly care is taken to read single characters at a time so that if the read fails
or the stream gets bad the read terminates.

There are three functions of type charIn defined below  that takes a character input and returns a
boolean function.  This is more general than a C++ style char test because you can have multiple 
characters match that you define.  


Three charIn functions define how the read takes place.

term      the field ends with a term match or true
stopFunc  the vector read stops with a stopFunc match
trim      trims the beginning and end of a field.


The stopFunc will terminate the vector read.  It also terminates a field read  like in the end of 
a line.

term can be a simple isComma function that matches ',' or it could be more complex like 
',' || ' '|| '\t'.  This is useful for example if the data has integers separated by many spaces 
commas.

Trim is the most complex.  Characters in the beginning and end of the field that match trim get removed.
If the data file has extra spaces between fields trim can remove the extra spaces. It is used to 
clean up the strings.  It also is there to define how repeat terminators are dealt with. 
For example if there are characters and several spaces between them or tabs trim can remove the repeats
so that multiple matches in a row get removed and logically turn into one match. This keeps the 
vectors to be of the correct size.

Each field is parsed as follows:

1. trim removes matching characters in the beginning.
2. the read is done and the read terminates with a stopFunc or term match.
3. trim removes characters in the end.

A defalut is passed.
Some use case this was designed for:
							  termFunc stopFunc trim       output
 123 ' ' ' ',' '132 ' ''\n'   ' '||,    \n      ' '||,     123, 132;  term is comma or space
 123,123, \n                    ,       \n      ' '        123, 123,def; no data in last field
  h' 'a' '' 'b\n               ' '      \n      ' '        h, a, b ;    repeated terms ignored 
tst 1, tst t' ',tst three' '\n   ,      \n      ' '        tst 1,tst t, tst three; inter word trims
																					dont match
*/
namespace IO{
	// all functions use this typedefinition for terminators
	typedef bool(*charIn) (char);
	bool falseFunction(char ch);
	bool isspace(char ch);
	bool isComma(char ch);
	bool isSemiColon(char ch);
	bool isPeriod(char ch);
	struct TestChar {
		charIn term;  // term function
		charIn stop; // stop vector read
		charIn trim;     // character to trim
		TestChar(charIn sep, charIn st, charIn tr) :term(sep), stop(st), trim(tr) {}
	};
	struct readSuccess {
		bool datafound;
		bool termfound;
		bool stopfound;
	};

	//fills a vector of T, where T types are separated by the required term, stopFunc char ends the filling, and Ignore chars are removed
	// ignore character is the case when you have multiple separators in a row like many spaces and you don't want
	// a blank entry for each space.
	template< class T> istream& fill_vector(istream& ist, vector<T>& v, charIn term, charIn stopFunc, charIn ignore);
	// fills a vector of type T for count times, where T is termfunc separated, ignoring ignore chars.  It expects  
	template<class T> istream& fill_vector(istream& ist, vector<T>& v, int& count, charIn termfunc, charIn endArray, charIn ignore);
	//template< typename Op> void fill_vector(istream& ist, vector<int>& v, Op term, Op sep); will fill the vector
	// ending read with term but reading the separator between reads;
	// reads a space separated record of type T.
	template< class T> istream& fill_vector(istream& ist, vector<T>& v, charIn stopfunction, charIn ignore);
	template< class T> istream& fill_vector(istream& ist, vector<T>& v, int& count, charIn stopfunction, charIn ignore);
	// readT will read a single value of type T; prior to reading will skipping Ignore and will fail read on stopFunc 
	// this works for nonspace termfunc stop func;  will work also for space termfunc and \n stop func
	template<> istream& IO::fill_vector<string>(istream& ist, vector<string>& v, int& count,
		charIn termFunc, charIn stopFunc, charIn ignore);
	template<> istream& IO::fill_vector<string>(istream& ist, vector<string>& v, charIn termFunc, charIn stopFunc, charIn ignore);
	// but shouldn't have spaces at the end of the line prior to the end of line function.  Will fail on format error
																	
	template<class T> istream& read(istream& ist, T& tval, bool& readsuccess, charIn stopFunc, charIn ignore);
	template<class T> istream& read(istream& ist, T& tval, bool& readsuccess, charIn termFunc, charIn stopFunc, charIn ignore);
	template<> istream& read<string>(istream& ist, string& tval, bool& readsuccess, charIn stopFunc, charIn ignore);
	template<> istream& read<string>(istream& ist, string& tval, bool& readsuccess, charIn termFunc, charIn stopFunc, charIn ignore);


	//reads a string until a terminator is found.  This function is for reading a string with no white space characters.
	// read terminates with space, endString true, or EndArray true.
	//  After the string, it will eat white space chars looking for Op endString char to be true; if so stream is 
	// fine and can read again.  If EndArray is true, read stops and char stays back in stream.
	// stream goes to bad state to indicate last valid read, not only end of string
	
	
	
	
	// throws out bad characters in the stream chars that are not ints.
	// print out vector of type T elements using << separated by tabs and modulo per line
	template< class T> ostream&  print_vector(ostream& ost, const T& invec, const int modulo=8);
	void skip_to_int(istream&);
	//check_failed_stream checks failed streams for a one char terminator; stream state good
	//  if term found, failed otherwise. Throws bad Stream exception
	//  preconditions (stream failed)
	istream& check_failed_stream(istream& ist, charIn term, const string& message);
	// checks a good stream for a terminator charactor termfunc(ch) true and sets stream to fail otherwise
    istream& check_good_stream(istream& ist, charIn termfunc, const string& message);
	//Some format error occured.  Clear the stream until Op: (char)-> bool is true or oef().
	//  if read chars successfully then optionally can return last char (bool EatToken = true)
    void read_past_token(istream& instr, charIn termfunc, bool EatToken=true);
	//void printstate(const istream& instr) wil print out the stream state
	void printstate(const istream& instr);
	//ostream& printdouble(ostream&, const double, int precis = 5) prints out a double
	// with precision and NaN alert
	ostream&  printdouble(ostream& outstr, const double val, int precis);
	//print out a vector tab separated
	
}
//#include "IO.cpp"

//read from istream a vector of objects of type T until a terminator is found.
// failures (eof() bad, or not terminator) return stream to fail state, 
// else stream is good
// If EOF or bad file return stream in eof fail state
// if terminator not found return stream to fail
/*
bool whitespaceonly(char ch)
{
	return ch == ' ';
}
bool termfuncDefault(char ch)
{
	return ch == ' ' || ch == '\t';
}
bool nonspaceWhiteSpace(char ch){
	return isspace(ch) && ch != ' ';
};  */