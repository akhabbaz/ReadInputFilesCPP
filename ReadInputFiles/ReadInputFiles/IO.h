# pragma once
#include <vector>
#include <iostream>
#include <string>
#include <limits>

using std::istream;
using std::ostream;
using std::string;
using std::vector;
/* 
Reading text data files is general, but there is an immense variety of text based data files. 
This program is meant to read a large subset of them, including CSV files.

The purpose of this file is to read data text files of various forms that consist of
a columnar arrangement of data. One line has data fields that may be empty separated by a 
terminator function term.  It also can be used to read a series of any record
type.  The data can be strings or numeric types, but all the same type.
The program has a read function which reads one field and a vector function which reads a series
fields.

Some features that make this general. Logic 
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
	// streamState reports all information back to vector Read so that it can
	// decide what to do.
	struct streamState {
		bool lineEnd;      // after this read vector should terminate
		bool formatError; // UnexpectedRead
        void   reset();  // sets stream to false.
	};
	// counts the number of reads in the line and the number of vectors read for
	// error reporting.
	struct  readCount{
	       int   reads;
	       int   lines;
	};
	// We expect the T classes to be separated by a terminator. If no T is found
	// at all, then that is not considered an error--the default is used. The
	// data file may be that way as a CSV file is. If
	// characters are found that are not lineEnds or terminators or ignores or
	// characters that define T, then an error is found. 
	enum class onError { Throw, Print, Ignore}; // behavior when an unexpected action happens
	
	//helper functions to read a stream, remove chars from a stream.
	template <typename T> class readStream{
	public:
		readStream(istream& str, T& d, charIn termF, charIn stopF, charIn
			TrimF,   onError errType = onError::Print); // set up read
		istream& read(T& tval);// read a type T using operator>>() 
		streamState  reportState(); // tells whether data is good or stream is
		                        // good
                istream&  read(vector<T> &, int iter =
					std::numeric_limits<int>::max());

		// the actual stream
	        istream& instr;  // the stream

	private:
	   T&    value  ;  // default when value not found
	   TestChar Func;  // the character testing functions
           streamState state; // state after the last read
	   readCount   count; // count of reads and vectors
	   onError     type;  // type of error
           // accumulate characters in the stream
	   void error(const string& message);  // report the error
		// removes characters from stream matching ignoreFunc and stopping
		// with stopFunc; if stop found it is returned to stream
	   void     readTerminator(); // a single char read looking for record
					// terminator, end of line, or end of file
	   // remove will remove  chars as long as tr is true.
	   //  This function makes sure no trim values are starting the stream and removes
	   //  them. First nonmatching character put back.
	   istream& remove(const IO::charIn tr);
  	   // remove_KeepLast(const IO::charIn tr) removes all but the last
  	   // occurance of the matching character. This removes repeat
  	   // characters but keeps the last one on the stream so that the user
  	   // can read it and classify it.
           istream& remove_keepLast(const IO::charIn tr);
	   // readSimpl a type T from the stream and report on stream state. It updates
	   // state.formatError if unexpected chars found. If the stop or term char is found but
	   // no data that is considered missing data and the default is used, not a format
	   // error.
	   istream& readSimple(T& tval);
	};
        // specialization for strings
	template <> class readStream<string>{
	public:
		readStream(istream& str, string& d, charIn termF, charIn stopF, charIn
			TrimF, onError errType = onError::Print); // set up read
		istream& read(string& tval);// read a type string using operator>>() 
		streamState  reportState(); // tells whether data is good or stream is
		                        // good
        istream&  read(vector<string> &, int iter =
					std::numeric_limits<int>::max());

		// the actual stream
	        istream& instr;  // the stream

	private:
	   string&    value  ;  // default when value not found
	   TestChar Func;  // the character testing functions
       streamState state; // state after the last read
	   readCount   count; // count of reads and vectors
	   onError     type;  // type of error
           // accumulate characters in the stream
       istream& accumulate( string& str, const charIn stopFunc);
	   void error(const string& message);  // report the error
		// removes characters from stream matching ignoreFunc and stopping
		// with stopFunc; if stop found it is returned to stream
	   void     readTerminator(); // a single char read looking for record
					// terminator, end of line, or end of file
	   // remove will remove  chars as long as tr is true.
	   //  This function makes sure no trim values are starting the stream and removes
	   //  them. First nonmatching character put back.
	   istream& remove(const IO::charIn tr);
  	   // remove_KeepLast(const IO::charIn tr) removes all but the last
  	   // occurance of the matching character. This removes repeat
  	   // characters but keeps the last one on the stream so that the user
  	   // can read it and classify it.
           istream& remove_keepLast(const IO::charIn tr);
	   // readSimpl a type string from the stream and report on stream state. It updates
	   // state.formatError if unexpected chars found. If the stop or term char is found but
	   // no data that is considered missing data and the default is used, not a format
	   // error.
	   istream& readA(string& tval);
           //Reads the string assuming trim cleared in the beginning 
	   // removes characters at the end of the string.
       void trimStringEnd(string& instring, const charIn
				trimFunc);
           // 
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
        // not sure this template works yet
	template<typename T > istream& read(istream& ist, T& tval, bool& readsuccess, charIn termfunc,
	charIn stopFunc, charIn trim, const T& d);
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
