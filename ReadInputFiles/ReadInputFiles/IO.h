     # pragma once
#include <vector>
#include <iostream>
#include <string>
#include <limits>
#include <functional>
#include <stdexcept>
using std::istream;
using std::ostream;
using std::string;
using std::vector;
/* 
Reading text data files is general, but there is an immense variety of text based data files. 
This program is meant to read a large subset of them, including CSV files.  It
consiss of a header file IO.h, a definitions file, IO.cpp, that should be in the same
translation unit, a main test file that demonstrates its abilities,
ReadInputFiles.cpp, and several example files that can be read that set up the read
and then read.  These are all read in ReadInput.cpp, and are in the visual studios
set up.

The purpose of this file is to read data text files of various forms that consist of
a columnar arrangement of data, or a series of record.  The data can be strings or
numeric types or any records.  Essentially this is a class that has two member
function; read(), to read individual elements, and read() to read a vector of
elements, separated by a terminator.  This is accomplished by having a terminator
between records and a different one to signal end of line or end of vector.  In
addition there is a trim function that removes extra characters in the beginning and
end of the record. These may be extra spaces or other white characters. This makes it
possible to get records purged of extra characters, even when the file hase them. 
There are also helper function to remove characters from a stream or trim a string
that work a character at a time that can be used outside of these classes. 



The main
class is templated so that any type for which the >> operator is defined can be read,
again making this general.  One line is read at a time so one can define different
reads with different types and separators for different lines.  In general for any
class T the read will look for terminators, end of lines, and trim characters, but
leave the read of the record to  operator>>() or >> so to standard or a user supplied
overload code.  There is an example of how to write this function in
ReadInputFiles.cpp.  The template is specialized to read strings and those are read a
character at a time, essentially not skipping white space. This allows a record to
consist of a line of text or a phrase;  internal white spaces won't be trimmed, but
leading and trailing ones will be (if the trim function is white space).


Errors are handled as follows.  On set-up a default value for the read is inputted.
If there is a format error, that value it put in the data field. Errors occur when
the structure read fails, data is missing, or when the terminator is not found.  One can report
errors by either printing them to cerr, throwing a runtime error, or not reporting and
letting the default value speak for the error.  If an error occurs during the read
the report identifies the record number (starting with 0), and line number (of
successive reads with that structure, and reports that, either by throwing an
exception, printing out a message or letting the default record replacement indicate
the error.  The stream is then cleared of characters until the next terminator and
the read continues.

Critical characters are identified with a function object defined below, charIn type,
not by single characters.  This is much more expressive and allows for more elaborate
matching. For example the isspace function will use C++'s isspace to report true if a
character matches.  These functions can be lambdas and can implement logic like
!isspace || eol.  This simplifies the logic and makes the program much more flexible.
Care was taken by including this logic, so that the resulting vector reads are
exactly the correct length, not bigger or smaller.


Three charIn functions define how the read takes place.

term      the record ends with a term character match or true
stop      the record read stops with a stopFunc match
trim      trims the beginning and end of a field removing repeated characters 

There are three functions of type charIn defined below  that takes a character input and returns a
boolean function.  This is more general than a C++ style char test because you can have multiple 
characters match that you define.  


term are characters used to separate
records, stop are characters that stop a vector read that terminate a series of
records, and trim are characters that should be ignored.  This program handles all
three. This allows many characters to be trim, term or stop and
gives alot of flexibility.  For example whitespace could be both a trim and a
separator and that consists of several characters.  It will trim extra white spaces
between records but keep the last determine the terminator. The end of line may also
be a '\n' which would be a white space, and this will still not trim the white space.


When the separators are all disjoint, the program handles all cases well. In addition
when there is overlap the program handles that and has certain rules that make
defining functions simple to get the desired outcome.  This is a common case.  For
example the terminator and trim could be both isspace and the program then will still
trim repeats and still find the terminator.  The eol character may be '\n' and the trim may
be isspace and the program will not trim away the eol.

Here are the simple rules that are used in order of priority.

1. eol or the stop function always stops the read and stops trimming characters.
Repeated eols will not be trimmed away.
2. trim removed repeats that are not eol.
3  if the last trim removed the terminator (such as a space), it is put back so that
    the terminator is still found with no error.

term can be a simple isComma function that matches ',' or it could be more complex like 
',' || ' '|| '\t'.  This is useful for example if the data has integers separated by many spaces 
commas.  Records end with either a terminator or eol.

Trim is the most complex.  Characters in the beginning and end of the field that
match trim get removed but not trims in the middle of a field.  It is used to 
clean up the strings.  It also is there to define how repeat terminators are dealt with. 
For example if there are characters and several spaces between them or tabs trim can remove the repeats
so that multiple matches in a row get removed and logically turn into one match.
Without a trim function repeated spaces could end up creating lots of spurious
records in vectors.  

Each field is parsed as follows:

1. trim removes matching characters in the beginning trim but not eol.
2. the read is done
3. trim removes chars at the end but not last terminator.
4. terminator read.

A default is passed.
Some use case this was designed for:

  123   ,  132  \n   
     ;where eol is term, isspace is trim and isComma is separator
  This is a test,   ,  Another string \n  
     where phrases end up in one record
     0x23  , 0xAF \n  
        special numerics can be read

The reads keep track of all characters so it is critical to know whether skipws is
set or not. In most cases noskipws is set so eol is not simply removed. Changing this
setting dramatically changes the read, so it must be set correctly.

The example files show how to read strings, integers, long integers, and records such
as a Name, or a Life (a two integer record) defined in the examples.   


To read records correctly care needs to be taken to  read the record but not to read
extra characters, even in cases of failure, so that separators, and terminators are
not removed from the stream. One way to do this, perhaps the easiest way, is to first
read the record using the readStream<string>:  This will get the records as a string
and do the trimming and identify all the separators. Next use a special function to
parse the string into a record.  This function is now easier to write because you can
skip white spaces without needing to worry about removing stream terminators.  The
other way is by reading a record with defined terminators and not skipping white
spaces. Remove helper functions can be used. The record should not have the
terminator characters or eol characters.  This is done in Life record in the example
file.  See comments near the Life declaration.

*/
namespace IO{
	// all functions use this typedefinition for terminators
	//typedef bool(*charIn) (char);
	using charIn = std::function<bool(char)>;
	extern charIn falseFunction, isComma, isSemiColon, isPeriod, isspace,
		eol, isTab;  // these function numbers are 0, 1, 2 .. etc
	// read in test values from file
	struct TestChar {
		charIn term;  // term function
		charIn stop; // stop vector read
		charIn trim;     // character to trim
		TestChar(charIn sep, charIn st, charIn tr) :term{ sep }, stop{ st }, trim{ tr } {}
		static TestChar defaultTestChar;
		static void updateDefault(const TestChar& test);
		TestChar();
	};
	// read from instr 3 integers to choose term, stop and trim functions
	TestChar  getFunctions(istream& instr);
	// convert an int to a function using the number scheme above
	IO::charIn    findFunction(int i);
	// streamState reports all information back to vector Read so that it can
	// decide what to do.
	// read a bool to indicate skip white space
	bool          getskipwsState(istream& istr);
	//getValue will read the template value from the stream
	template <typename T> istream& getValue(istream& instr, T& val);
	// readCommandLine will read the first line for the readSet up.
	template <typename T> istream& readCommandLine(istream& instr, T& val, 
					TestChar& sep, bool& skipws, string& descrip);
	struct streamState {
		bool lineEnd;      // after this read vector should terminate
		bool formatError; // UnexpectedRead
        void   reset();  // sets stream to false.
		streamState();
		streamState(bool le, bool fe);
	};
	// counts the number of reads in the line and the number of vectors read for
	// error reporting.
	struct  readCount{
	       int   reads;
	       int   lines;
		   readCount(); 
		   readCount(int r, int l);
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
			TrimF,  bool skipState = false, onError errType = onError::Print); // set up read
		istream& read(T& tval);// read a type T using operator>>() 
		readStream(const readStream&) = default;
		streamState  reportState(); // tells whether data is good or stream is
		                        // good
        	istream&  read(vector<T> &, unsigned iter =
					std::numeric_limits<unsigned>::max());
		// the actual stream
	        istream& instr;  // the stream
		void skipws(void);  // instr skips white space on input
		void noskipws(void); // treats all whitespaces as input. Whitespaces can be
				// terminators 
		void updateDefault(const T& v);
	private:
	   T    value  ;  // default when value not found
	   TestChar Func;  // the character testing functions
           streamState state; // state after the last read
	   readCount   count; // count of reads and vectors
	   onError     type;  // type of error
	   bool        skipwsState; // true means skipws false means noskipws
		   // readSimple a type T from the stream and report on stream state. It updates
	   // state.formatError if unexpected chars found. If the stop or term char is found but
	   // no data that is considered missing data and the default is used, not a format
	   // error.
	   istream& readSimple(T& tval);
           // accumulate characters in the stream
	   //void error(const string& message);  // report the error
		
	   //void     readTerminator(); // a single char read looking for record
					// terminator, end of line, or end of file
	   // remove will remove  chars as long as tr is true.
	   //  This function makes sure no trim values are starting the stream and removes
	   //  them. First nonmatching character put back.
	  // istream& remove(const IO::charIn tr);
  	   // remove_KeepLast(const IO::charIn tr) removes all but the last
  	   // occurance of the matching character. This removes repeat
  	   // characters but keeps the last one on the stream so that the user
  	   // can read it and classify it.
       //istream& remove_keepLast(const IO::charIn tr);

	};
        // specialization for strings
	template <> class readStream<string>{
	public:
		readStream(istream& str, string& d, charIn termF, charIn stopF, charIn
			TrimF,bool skipState = false, onError errType = onError::Print); // set up read
		istream& read(string& tval);// read a type string using operator>>()
		readStream(const readStream&) = default;
		streamState  reportState(); // tells whether data is good or stream is
		                        // good
        	istream&  read(vector<string> &, unsigned iter =
					std::numeric_limits<unsigned>::max());

		// the actual stream
	        istream& instr;  // the stream
		void skipws(void);  // instr skips white space on input
		void noskipws(void); // treats all whitespaces as input. Whitespaces can be
				// terminators 
		void updateDefault(const string& str);// update default string

	private:
	   string    value  ;  // default when value not found
	   TestChar Func;  // the character testing functions
       streamState state; // state after the last read
	   readCount   count; // count of reads and vectors
	   onError     type;  // type of error
	   bool        skipwsState; // true means skipws false means noskipws
           // remove characters in the stream, and return them in str
       //istream& remove(const charIn stopFunc, string& str);
	   // remove will remove  chars as long as tr is true.
	   //  This function makes sure no trim values are starting the stream and removes
	   //  them. First nonmatching character put back.
	   //istream& remove(const IO::charIn tr);
  	   // remove_KeepLast(const IO::charIn tr) removes all but the last
  	   // occurance of the matching character. This removes repeat
  	   // characters but keeps the last one on the stream so that the user
  	   // can read it and classify it.
       //istream& remove_keepLast(const IO::charIn tr);
	   // readSimpl a type string from the stream and report on stream state. It updates
	   // state.formatError if unexpected chars found. If the stop or term char is found but
	   // no data that is considered missing data and the default is used, not a format
	   // error.
	   //void error(const string& message);  // report the error
		// removes characters from stream matching ignoreFunc and stopping
		// with stopFunc; if stop found it is returned to stream
	   //void     readTerminator(); // a single char read looking for record
					// terminator, end of line, or end of file
	   istream& readSimple(string& tval);
           //Reads the string assuming trim cleared in the beginning 
	   // removes characters at the end of the string.
       
           // 
	};

	//remove helper functions
// removes until stopFunc True
	istream& remove(istream& istr, const IO::charIn stopFunc);
	// remove until stop function true return string removed
	istream& remove(istream& istr, const IO::charIn stopFunc, string& str);
	// streamChars holds the prior and current stream Characters 
	struct StreamChars {
		int current, prior;
		bool priorValid; // true if prior had a valid
	};

	/*   These are the functions that remove characters from the stream. They can be used
		 Independently to remove characters till stop or to extract characters.  They have a stop
		 Function that will stop removal when true.   */
	// removes until stopFunc true, returns currVal and piorVal in strChar Uses peek. 
	istream& remove(istream& istr, const IO::charIn stopFunc, StreamChars& strChar);
	// decides to put terminator back into stream or not based on current and prior chars.
	// if current is not trim nor term nor stop and prior is a term it puts the prior back.
	istream& putTermBack(istream& istr, const IO::TestChar& func, const StreamChars strChar);
	// removes trim from the stream but not stop.
	istream& removeTrimBefore(istream& instr, const IO::TestChar& func);
	// this implements the order 1. stop removing trims on eol found.  Trim repeats,
	// unget only if data is current and prior is a term
	istream& removeCharsAfter(istream& instr, const IO::TestChar& Func, const bool formatError);
	//removes characters from the end of the stream that match trimFunc.
	void trimStringEnd(string& instring, const IO::charIn
		trimFunc);
	// read the single terminator char and report error if not found
	istream& readTerminator(istream& instr, const IO::charIn sep,
		const IO::charIn stop, IO::streamState& state, const IO::readCount count, const IO::onError type);
	void error(const string& msg, const IO::onError type, const IO::readCount count);


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
