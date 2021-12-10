//read elements in vector until the term found
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <iterator>
// #define NDEBUG


IO::charIn IO::isPeriod = [](char ch) {
	return ch == '.';
};
IO::charIn IO::eol = [](char ch) {
	return ch == '\n';
};
IO::charIn IO::isspace = [](char ch) {
	return ::isspace(ch);
};
IO::charIn IO::isComma = [](char ch) {
	return ch == ',';
};
IO::charIn IO::isSemiColon=[](char ch){
	return ch == ';';
};
IO::charIn IO::isTab = [](char ch) {
	return ch == '\t';  
};

IO::TestChar IO::TestChar::defaultTestChar{ IO::isComma, IO::eol, IO::isspace };
//Constructor  Initialize variables
void IO::TestChar::updateDefault(const TestChar& test)
{
	defaultTestChar = test;
}
IO::TestChar::TestChar():term{defaultTestChar.term}, stop{defaultTestChar.stop},
		trim{ defaultTestChar.trim }{}
template<typename T> IO::readStream<T>::readStream(istream& str, T& d, IO::charIn termF, IO::charIn stopF, IO::charIn
	TrimF,bool skipwsS, onError errorType) :instr{ str }, value{ d },
	Func{ termF, stopF, TrimF }, state{ false, false },
	count{ 0, 0 }, skipwsState{ skipwsS }, type{ errorType }
{
	instr.exceptions(instr.exceptions() | std::ios_base::badbit);
	// This throws exception if instr is bad 
	// If that fails the program can't recover so we might as well throw  an
	// exception. Best explained p 763 Jossutis. Stream State Exceptions
	// 15.4.4
	(skipwsState) ? skipws():noskipws();
	 // default behavior is to keep all whitespaces so that they can be
			// terminators, and so more than one word can enter a string
}
IO::readStream<string>::readStream(istream& str, string& d, IO::charIn termF, IO::charIn stopF, IO::charIn
	TrimF, bool skipwsS, onError errorType) :instr{ str }, value{ d },
	Func{ termF, stopF, TrimF }, state{ false, false },
	count{ 0, 0 }, skipwsState{ skipwsS }, type{ errorType }
{
	instr.exceptions(instr.exceptions() | std::ios_base::badbit);
	// This throws exception if instr is bad 
	// If that fails the program can't recover so we might as well throw  an
	// exception. Best explained p 763 Jossutis. Stream State Exceptions
	// 15.4.4

	noskipws(); // default behavior is to keep all whitespaces so that they can be
			// terminators, and so more than one word can enter a string
	(skipwsState) ? skipws() : noskipws();
}
IO::streamState::streamState() :lineEnd{}, formatError{}{}
IO::streamState::streamState(bool le, bool fe):lineEnd{le}, formatError{fe}{}
IO::readCount::readCount():reads{},lines{}{}
IO::readCount::readCount(int r, int l) : reads{r}, lines{l}{}
//Report an error along with line and read number
void IO::error(const string& msg, const IO::onError type, const IO::readCount count)
{
   std::ostringstream oss{msg, std::ios_base::ate}; // position to end of stream
   oss << "; Line " << count.lines <<"; readCount " <<count.reads;
   switch(type){
   case  IO::onError::Throw:
         throw std::runtime_error(oss.str());
	 break;
   case  IO::onError::Print:
   	 std::cerr << oss.str()<< std::endl;
	 break;
   default:
         break;
   }
   return;
}
void IO::readStream<string>::updateDefault(const string& str)
{
	value = str;
}
template <typename T> void IO::readStream<T>::updateDefault(const T& v)
{
	value = v;
}
// very simple read for vectors.  
template<typename T> istream&  IO::readStream<T>::read(vector<T> & vec, unsigned iter)
{
	T val{};
	for (unsigned i = 0; i < iter; ++i)
	{
		read(val);
		vec.push_back(val);
		if  (state.lineEnd or instr.eof())
		{ 	
			break;
		}
	}
	++count.lines;
	count.reads = 0;
	return instr;
}
istream&  IO::readStream<string>::read(vector<string> & vec, unsigned iter)
{
	string val{};
	
	for (unsigned i = 0; i < iter; ++i)
	{
		read(val);
		vec.push_back(val);
		if  (state.lineEnd or instr.eof())
		{ 	
			break;
		}
	}
	++count.lines;
	return instr;
}
template<typename T>  IO::streamState  IO::readStream<T>::reportState()
{
	return state;
} // tells whether data is good or stream is
IO::streamState  IO::readStream<string>::reportState()
{
	return state;
} // tells whether data is good or stream is
// remove will remove  chars as long as stopFunc is false, and stores the re
// remove will store characters in a string as long as stopFunction not triggered.
//  The stop function puts the character back into the stream.
 istream& IO::remove(istream& instr, const IO::charIn stopFunc, string& str)
{
	std::ostringstream oss;
	using str_it = std::istream_iterator<char>;
	str_it readchar{instr};
	str_it  eos{};
	IO::charIn transferChars = [&oss, stopFunc](char i)
	{
		bool stopFound = stopFunc(i);
		if (!stopFound) {
			oss << i;
		}
		return stopFound;
	};
	str_it foundloc = std::find_if( readchar, eos, transferChars);
	if (foundloc !=eos)
	{
		instr.unget();
	}
	str = oss.str();
  	return instr;
}
// removes characters at the end of the string.
//  This function makes sure no trim values are starting the stream and removes
//  them. First nonmatching character put back.

istream& IO::remove(istream& instr, const IO::charIn stopFunc)
{
	//this iterator works as follows. When constructed the stream is read as with >> and a 
	// copy is held ready to dereference. Essentially that is removed. The increment ++ operator
	// will do the next read and store result in *
	using str_it = std::istream_iterator<char>;
	str_it readchar{instr};
	str_it eos{};
	str_it foundloc = std::find_if( readchar, eos, stopFunc);
	if (foundloc !=eos)// need unget here because when found it has been read.
	{
		instr.unget();
	}	return instr;
}
// removes until stopFunc true, returns currVal and piorVal in strChar Uses peek. 
// does not unget any characters
istream& IO::remove(istream& istr, const IO::charIn stopFunc , IO::StreamChars& strChar)
{

   strChar.current = istr.peek();
   strChar.priorValid = false;
   while( !(strChar.current == std::char_traits<char>::eof() ))
   {
      char ch{static_cast<char>(strChar.current)};
      if ( stopFunc(ch)) {
      		break;
      }
      //ch is not the stop update;
      char chCopy;
      istr >> chCopy;
      assert( chCopy == ch);
      strChar.prior = strChar.current;
      strChar.priorValid = true;
      strChar.current = istr.peek();
  }
  return istr;
}
// decides to put terminator back into stream or not based on current and prior chars
istream& IO::putTermBack(istream& istr, const IO::TestChar& func, const IO::StreamChars strChar)
{
    char curr = static_cast<char>(strChar.current);
    char prior = static_cast<char>(strChar.prior);// prior won't ever be eol 
    if (strChar.priorValid && func.term(prior))
    {// current not trim nor term nor stop.  current won't ever be trim I think
        bool currentData{ ! (func.trim(curr) || func.term(curr)  || func.stop(curr))};
	if (currentData)
	{
		istr.putback(prior);
	}
    }
    return istr;
}


// remove all but the last occurance of a matching character. The character on
// the stream will be  the match if found, but if not found,  the stream remains
// as it was. This removes all chars while tr is false except the last occurance

istream& remove_keepLast(istream& instr, const IO::charIn tr)
{
	int peekVal;
        bool match=false;
        // enter if not eof
	while ((peekVal = instr.peek()) != std::char_traits<char>::eof())
	{
		char ch{ static_cast<char>(peekVal) };
		char  test;
		// match found
		if (!tr(ch)) {
			match = true;
			instr >> test; // read char
			assert(test == ch); // should be the same
		}
		else {
			if (match) { instr.unget(); }
			break;
		}
	}
        return instr;
}

// readSimple a type T from the stream and report on stream state. It updates
// state.formatError if unexpected chars found. If the stop or term char is found but
// no data that is considered missing data and the default is used, not a format
// error.
template<typename T> istream& IO::readStream<T>::readSimple(T& tval)
{
   // if at End of File stream will be failed so read won't work.
   // Hear EOF is treated as a term character as it should be at the end of reading a
   // line of code
   instr >> tval;
   (skipwsState) ? skipws() : noskipws(); // in case read changes stream state
   // value is bad but is this a format error or a missing data element?
   if (instr.fail())
   {
      tval= value; 
      if (!instr.eof()) // characters left
      {
           instr.clear();
	       charIn stopread = [this](char i) -> bool {return  Func.stop(i) ||
				   Func.trim(i) || Func.term(i); };
	   // there was a format error on read
           char ch{ static_cast<char>(instr.peek())};
	       assert(instr);// should be able to get chars
	       if ( state.formatError =!stopread(ch)){
           	IO::error("Format Error on read", type, count); //data entry missing;  char is 
	       }
      }
    }
   else {
	   ++count.reads;
   }
    return instr;
}


//Reads the string assuming trim cleared in the beginning 
istream& IO::readStream<string>::readSimple(string& str) //read a string value one character at a time
{

	charIn stopread = [this](char i){return  Func.stop(i) ||
		Func.term(i); };
	IO::remove(instr, stopread, str);
	++count.reads;
	if (str.size()== 0){
		str = value;
	}
	IO::trimStringEnd(str, Func.trim);
	return instr;
}

void IO::readStream<string>::skipws(void)
{
	instr.setf(std::ios_base::skipws);
}
template <typename T> void IO::readStream<T>::skipws(void)
{
	instr.setf(std::ios_base::skipws);
}
void IO::readStream<string>::noskipws(void)
{
	instr.unsetf(std::ios_base::skipws);
}
template <typename T> void IO::readStream<T>::noskipws(void)
{
	instr.unsetf(std::ios_base::skipws);
}
void IO::streamState::reset() {
	lineEnd = false;
	formatError = false;
} //default state
//IO::removes chars befor
istream& IO::removeTrimBefore(istream& instr, const IO::TestChar& func)
{
	//!Trim true means stop removing; eol means stop removing; others get IO::removed
	IO::charIn notTrimOrStop = [func](char i) { return !func.trim(i) || func.stop(i); };
	IO::remove(instr, notTrimOrStop); // non trim char left on stream  should be T if eof fail.
	return instr;
}
// removes the characters after the read
istream& IO::removeCharsAfter(istream& instr, const IO::TestChar& Func, const bool formatError)
{
	// if formatError remove till term
	IO::charIn stopread = [Func](char i) -> bool {return  Func.stop(i) ||
		Func.term(i); };
	if (formatError) { // there is a non term on stream.
		IO::remove(instr, stopread); // clear stream till term found
	}
	
	//as in removeChars before 
	IO::charIn  notTrimOrEOL = [Func](char i) -> bool {return
		!Func.trim(i) || Func.stop(i); };
	// if trim overlaps with term all but the last is removed.
	IO::StreamChars streamchars{ 0, 0, false };
	IO::remove(instr, notTrimOrEOL, streamchars);
	IO::putTermBack(instr, Func, streamchars);
	return instr;
}
template<typename T> istream& IO::readStream<T>::read(T& tval)// read a type T using operator>>()
{
        //clear past read state:
        state.reset(); 
		(skipwsState) ? skipws() : noskipws();
		IO::removeTrimBefore(instr, Func);
		readSimple(tval);  // 
		IO::removeCharsAfter(instr, Func, state.formatError);
		IO::readTerminator(instr, Func.term, Func.stop, state, count, type);
		return instr;
}
istream& IO::readStream<string>::read(string& tval)// read a type T using operator>>()
{
	//clear past read state:
	state.reset();
	(skipwsState) ? skipws() : noskipws(); //incase first call after wsSkip was reset by user
	// stop when not trim or when the stop isfound 
	IO::removeTrimBefore(instr, Func);
	readSimple(tval);  // 
	IO::removeCharsAfter(instr, Func, state.formatError);
	// all trims have been removed char on stream should be terminator.
	IO::readTerminator(instr, Func.term, Func.stop, state, count, type);
	return instr;
}
   
 void IO::trimStringEnd(string& instring, const IO::charIn
		trimFunc)
{
	// reverse iterator to find the last charactermatching
	// will return iterator to first not Trim function or last good
	// character.
	// because reverse iterator's value is one behind, the first !trimFunc
	// has a value of the nontrim character but points one ahead.
	string::const_reverse_iterator  lastGood=
		std::find_if_not(instring.crbegin(), instring.crend(), trimFunc);
	// turn it into a forward iterator for purposes of erase. THis now
	// points to the first trimcharacter that is consecutive from the end.
	string::const_iterator forwardit {lastGood.base()};
	// do the erasing.
	instring.erase(forwardit, instring.end());
}
// a single char read looking for a record terminator, end of line or end of
// file
istream& IO::readTerminator(istream& instr, const IO::charIn sep,
	 const IO::charIn stop, IO::streamState& state, const IO::readCount count, const IO::onError type)
{
      IO::charIn stopread = [stop, sep](char i) -> bool {return  stop(i) ||
	   sep(i); };
      char ch;
      if ( instr >> ch)
      {
      		state.formatError = !stopread(ch);
		if (state.formatError){
		    IO::error("Terminator not found", type, count); //data entry missing;  char is
                    IO::remove(instr, stopread); // remove characters from the stream. 
		}
		else if (stop(ch)){
			state.lineEnd = true;
		}
      }
	  return instr;
}


/*    istream& IO::check_failed_stream(istream& ist, Op termfunc, const string& message)
is meant to test a stream after it has failed.  It isn't meant for good streams!. For example
if the read succeeded even if stream is now EOF or bad, the stream will say success.  It throws an exception
if the stream is bad.  Below pertains to failed streams that are not bad.  

   preconditions:  stream has failed.
   1.  If it returns true it means terminator was found, and if it returns false it means terminator
       wasn't found

*/
istream& IO::check_failed_stream(istream& ist, charIn termfunc, const string& message)
{
	if (ist.fail()) 
	{ // use term as terminator and/or separator
		ist.clear();
		check_good_stream(ist, termfunc, message);
	}
	return ist;
}
// checks a good stream for a terminator charactor termfunc(ch) true and sets stream to fail otherwise
// this will check the next char in the stream (may be a white space)
istream& IO::check_good_stream(istream& ist, charIn termfunc, const string& message)
{
	char ch;  // this has to work if the stream isn't over
	ist.get(ch);
	if (ist  && !termfunc(ch)){ //read successful but not terminator
		ist.unget();
		ist.clear(std::ios_base::failbit);
	}
	if (ist.bad()) 
		throw std::runtime_error{ message };
	return ist;
}

template< class T> ostream&  IO :: print_vector(ostream& ostr, const  T& invec, const int modulo)
{
	if (modulo < 1) 
		throw std::runtime_error("Modulo too small");
	for (vector<int>::size_type i = 0; i < unsigned(invec.size()); i++)
	{
		ostr << invec[i];
		ostr << (((i + 1) % modulo) ? '\t' : '\n');
	}
	ostr << '\n';
	return ostr;
}


//will read the input until either the token is found or eof().
// optionally can return the token to the stream;
void IO::read_past_token(istream& instr, charIn termfunc, bool eatToken) // if we don't find terminator let stream stay failed
{
	if (instr.fail()) {
		instr.clear();
		char ch;
		while (instr.get(ch)){       // throw away till )
			if (termfunc(ch))
			{
				if (!eatToken) instr.unget();
				instr.clear();
				return;
			}
		}
	}
	 //if fail to read ) then we have to be eof; otherwise 
	// exception
	if (instr.bad()) 
		throw std::runtime_error("bad stream...");
	     // bad
}


bool readOver(std::istream& ist, IO::charIn termFunc, IO::charIn stopFunc, IO::charIn ignore);
// 
//bool trimStream(std::istream& ist, IO::charIn trim);


IO::charIn IO::falseFunction = [](char ch) {
	return false;
};
template <typename T> istream& IO::getValue(istream& instr, T& val)
{
	instr >> val;
	return instr;
}
IO::charIn  IO::findFunction(int i)
{
	IO::charIn func;
	switch (i) {
	case 0:
		func = IO::falseFunction;
		break;
	case 1:
		func = IO::isComma;
		break;
	case 2:
		func = IO::isSemiColon;
		break;
	case 3:
		func = IO::isPeriod;
		break;
	case 4:
		func = IO::isspace;
		break;
	case 5:
		func = IO::eol;
		break;
	default:
		func = IO::isComma;
		break;
	}
	return func;
}

IO::TestChar  IO::getFunctions(istream& instr)
{
	int i, j, k;
	instr >> i >> j >> k;
	IO::charIn term{ findFunction(i) };
	IO::charIn stop{ findFunction(j) };
	IO::charIn trim{ findFunction(k) };
	return IO::TestChar{ term, stop, trim };
}
bool          IO::getskipwsState(istream& istr)
{
	bool skipState;
	istr >> skipState;
	return skipState;
}
template <typename T> istream& IO::readCommandLine(istream& instr, 
			T& val, IO::TestChar& sep, bool& skipws, string& descrip)
{
	instr.setf(std::ios_base::skipws);
	IO::getValue<T>(instr, val);
	sep = IO::getFunctions(instr);// separators
	skipws = IO::getskipwsState(instr);
	instr.unsetf(std::ios_base::skipws);
	charIn NotIsSpace = [=](char i)
	{return !IO::isspace(i) || IO::eol(i);  };
	IO::remove(instr, NotIsSpace);
	IO::remove(instr, IO::eol, descrip);

	IO::streamState state{};
	IO::readCount count{};
	IO::onError  type{ IO::onError::Throw };
	if (!instr) {
		IO::error("read of input parameters failed", type, count);
	}
	IO::readTerminator(instr, IO::eol, IO::eol, state, count, type);
	instr.setf(std::ios_base::skipws);
	return instr;
}
//reads T types separated by characters given by ignore(ch) == true, which can be many in a row; filling of vector
// continues until termfunc is found and fails if not found. vector<T> grows for each successful read;
// str good:   stop term found
// str bad:    eof, bad char read, or read failed--latter two cases char returned to stream
template<class T> std::istream& IO::fill_vector(istream& ist, vector<T>& v, charIn termfunc, charIn ignore)
// read integers from ist into v until we reach eof() or terminator
{

	T tval;
	bool readsuccess;
	while (read(ist, tval, readsuccess, termfunc, ignore) && readsuccess)
	{
			  v.push_back(tval);
			  readsuccess = false;
	}
	if (readsuccess){
		v.push_back(tval);
	}
	return check_failed_stream(ist, termfunc, "Bad Stream");
}
//reads T types separated by char tokens in ignore, where successive tokens are ignored. count reads are done in a row.
// once termfunc if found read is over (stream good then).
//str good:  count equal to # requested or count is fewer and term found
//str  bad:  eof or failed read
template<class T> istream& IO::fill_vector(istream& ist, vector<T>& v, int& count, charIn termfunc, charIn ignore)
// read integers from ist into v until we reach eof() or terminator
{

	T tval;
	bool readsuccess;
	int max = count;
	if (max < 0){
		 throw std::runtime_error("max out of bounds");
	}
	count = 0;
	while ((count < max) && read(ist, tval, readsuccess, termfunc, ignore) && readsuccess)
	{
		v.push_back(tval);
		++count;
		readsuccess = false;

	}//readsuccess is true whenever the stream is good.
	if (readsuccess){
		v.push_back(tval);
		++count;
	}
	if (count == max && !ist.fail()) // check to see if next char is stopFunc--remove it so successive read works
	{
		readOver(ist, termfunc, falseFunction, ignore);//treat stopFunc as Term to clear the char
	}
	return check_failed_stream(ist, termfunc, "Bad Stream");
}
//reads stream looking for a type T and a termfunc afterwards;  stops read with endArray and ignores chars ignore.
// stream is good if termfunc or endArray encountered; if data found without term fail stream. Successive terms with
// no data end stream.
//Read over once a failed read (no data between termfunc) or endArray encountered or stream over. 
// str good:  terminator found or endArray found
// str bad :  eof, read failed, or nonterm char read after successful read. (offending chars back on stream)
template<class T> istream& IO::fill_vector(istream& ist, vector<T>& v, charIn termfunc, charIn endArray, charIn ignore)
// read integers from ist into v until we reach eof() or terminator
{

	T tval;
	bool readsuccess;
	while (read(ist, tval, readsuccess, termfunc, endArray, ignore) && readsuccess)
	{
		v.push_back(tval);

	}
	if (readsuccess){
		v.push_back(tval);
	}
	return check_failed_stream(ist, endArray, "Bad Stream");
}
//this fill vector will read count items and expects items separted by termfunc. stream fails when in looking for termfunc or endArray
// a data char found--failed means a bad read and requires user attention. count is updated with the number of reads done. stream state 
//str good:  count equals max or endArray is true
//str bad:  eof, failed read or data char (non-terminator) read after successful read.
template<class T> istream& IO::fill_vector(istream& ist, vector<T>& v, int& count, charIn termfunc, charIn endArray, charIn ignore)
// read integers from ist into v until we reach eof() or terminator
{

	T tval;
	bool readsuccess;
	int max = count;
	if (max < 0){
		std::runtime_error("max out of bounds");
	}
	count = 0;
	while ((count < max) && read(ist, tval, readsuccess, termfunc, endArray, ignore) && readsuccess)
	{
		v.push_back(tval);
		++count;
		readsuccess = false;

	}
	if (readsuccess){
		v.push_back(tval);
		++count;
	}
	//count counts all successful reads and is always correct.
	/* stream could be in one of two states: 1.  good termfunc found--keep stream good
	   2.  bad  either endArray found or a data char read or eof.  If term found no need to bother user.*/
	return check_failed_stream(ist, endArray, "Bad Stream");
}


//reads a vector of strings separated by termfunc and terminated with stopfunc
//str good:  stop found
//str bad :  eof
template<> istream& IO::fill_vector<string>(istream& ist, vector<string>& v, charIn stopFunc, charIn ignore)
// read integers from ist into v until we reach eof() or terminator
{
	string test{ "" };
	bool readsuccess = false;
	while (read(ist, test, readsuccess, stopFunc, ignore) && readsuccess)
	{
		v.push_back(test);
		test = "";
	}
	
	if (readsuccess) // good whenever ist is good 
	{
		v.push_back(test);
	}
	return check_failed_stream(ist, stopFunc, "Bad Stream"); //look for end of read
}
//reads a vector of strings separated by termfunc and terminated with stopfunc. Reads up to count chars.
//str good:  stop found or count strings reached
//str bad :  eof
template<> istream& IO::fill_vector<string>(istream& ist, vector<string>& v, 
	                              int& count, charIn stopFunc, charIn ignore)
// read integers from ist into v until we reach eof() or terminator
{
	string test{ "" };
	bool readsuccess = false;
	int max = count;
	if (max < 0){
		throw std::runtime_error("max out of bounds");
	}
	count = 0;
	while ((count < max) && read(ist, test, readsuccess, stopFunc, ignore) && readsuccess)
	{
		v.push_back(test);
		test = "";
		++count;
		readsuccess = false;
	}

	if (readsuccess) // good whenever ist is good 
	{
		v.push_back(test);
		++count;
	}
	if (count == max && !ist.fail()) // check to see if next char is stopFunc--remove it so successive read works
	{
		readOver(ist, stopFunc, falseFunction, ignore);//treat stopFunc as Term to clear the char
	}
	return check_failed_stream(ist, stopFunc, "Bad Stream"); //look for end of read
}
//fill_vector with strings, requiring termfunc after each string; ignores ignore and stops on stopfunc.
// str good:  termFunc found or stop func found
// str  bad:  eof,  or bad char read from stream 
template<> istream& IO::fill_vector<string>(istream& ist, vector<string>& v, charIn termFunc, charIn stopFunc, charIn ignore)
// read integers from ist into v until we reach eof() or terminator
{
	string test{ "" };
	bool readsuccess = false;
	while (read(ist, test, readsuccess, termFunc, stopFunc, ignore) && readsuccess)
	{
		v.push_back(test);
		test = "";
		readsuccess = false;
	}
	if (readsuccess) // ist failed after reading data
	{
		v.push_back(test);
	}
	return check_failed_stream(ist, stopFunc, "Bad Stream"); //look for end of read
}

//fill_vector with strings, up to count strings, requiring termfunc after each string; ignores ignore and stops on stopfunc.
// str good:  termFunc found or stop func found
// str  bad:  eof,  or bad char (nonterm nor stopfunc) read from stream 
template<> istream& IO::fill_vector<string>(istream& ist, vector<string>& v, int& count,  
	                            charIn termFunc, charIn stopFunc, charIn ignore)
// read integers from ist into v until we reach eof() or terminator
{
	string test{ "" };
	bool readsuccess = false;
	int max = count;
	if (max < 0){
		throw std::runtime_error("max out of bounds");
	}
	count = 0;
	while ((count < max) && read(ist, test, readsuccess, termFunc, stopFunc, ignore) && readsuccess)
	{
		v.push_back(test);
		test = "";
		readsuccess = false;
		++count;
	}
	if (readsuccess) // ist failed after reading data
	{
		v.push_back(test);
		++count;
	}
	return check_failed_stream(ist, stopFunc, "Bad Stream"); //look for end of read
}
//read from istream an object of type T, where following each T is a separator termfunc.
/* meaning of booleans:
success T/F  chars read/not read
str good:   terminator found ; 
str bad:   terminator not found--may be eof, stopfunc, failed read, or some other char after data read

*/
//readT will read anyclass T.  The elements are separated by a single char that matches termfunc, and the read ends
//with stopfunc true.   After the read, the terminator is looked for; if found all good. 
//  If stop or other char found fail the stream.

/* Invariants:
   readsuccess only true if read the T val successfully
   str state true- if read the terminator
  
*/

template<class T> istream& IO::read(istream& ist, T& tval, bool& readsuccess, 
	                                charIn termfunc, charIn stopFunc, charIn ignore)
{
	
	readsuccess = false;
	
	if (readOver(ist, termfunc, stopFunc, ignore))
	{
		return ist; //termfunc, stopfunc or stream end found
	}
	
	//term, stopFunc, and space have been checked for and not found.
	ist >>  tval;
	if (ist) // stream good so update readsuccess
	{
		readsuccess = true;
	}
	// if stream fails stream keeps failing bit on stream anyway so no need to code that.
	if ( !readOver(ist, termfunc, stopFunc, ignore) ) // data found on stream
	{
		ist.clear(std::ios_base::failbit);
		// non terminator or data found (and returned to stream ) fail stream.
	}
	return ist;
}
/*
template<typename T > istream& IO::read(istream& ist, T& tval, bool& readsuccess, charIn termfunc,
	charIn stopFunc, charIn trim, const T& d)
{
	readsuccess = false;
	//TrimStream(ist, trim, stopFunc); // remove trim characters
	ist >> tval;
	charIn findTerm = [termfunc, stopFunc] (char ch) ->bool
	{
		return termfunc(ch) || stopFunc(ch);
	};
	if (ist)
		readsuccess = true;
	    
	else
		tval = d;

	//TrimStream(ist, trim, stopFunc);
}
*/
//ReadT will read tval assuming before and after are spaces, ' '.  fails stream if stopFuncFound
// if str is good read success
/*
success T/F  T read either T/F
str good:ignore, read successful     bad: eof, stopFunc or read fail
*/
template<class T> istream& IO::read(istream& ist, T& tval, bool& readsuccess, charIn stopFunc, charIn ignore)
{
	readsuccess = false;
	if (!readOver(ist, falseFunction, stopFunc, ignore))
	{
		ist >> tval; //stream end found or stopFuncfound
		if (ist){// update readsuccess if stream good; if stream failed bad char left on stream
		readsuccess = true;
		}
	}
	//not term, not stopfunc so read spaces
	//term, stopFunc, and space have been checked for and not found.
	return ist;
}


//read string a character at a time, clearing stream prior to reading chars, reading the string, but stopping read if any
// nondata character found.   stopFunc stops read putting char back and 
// failing stream only if read failed or if stream is over. checks char only prior to string read.
/* 
success T/F  string read either T/F
str good: ignore, successful read;      
str  bad: eof, stopFunc-read failed
*/
template<> istream& IO::read<string>(istream& ist, string& strread, bool& success, charIn stopFunc, charIn ignoreFunc)
{
	char i;
	success = false;
	if (readOver(ist, falseFunction, stopFunc, ignoreFunc)) // handles termfunc, stopfunc or failed stream
	{
		return ist; // with falseFunction in, will only enter here if stopFunc true stream will be failed.
	}

	auto stopread = [stopFunc, ignoreFunc](char i) {return  stopFunc(i) || ignoreFunc(i); };
	while (ist.get(i) && !stopread(i))// if ist is good will enter loop first time since stop,
		                              // ignore checked above
	{
		strread += i; // read successful
		success = true;
	}
	if (ist && stopFunc(i)){
		ist.unget(); // stop analyzed during next read.
	}
	return ist;
}
// fills string; success indicates chars read-T read, F not read; fill_string with term requires a terminator after read.
// stream ist good means---term found,  false means -- stream failed, other found, or stop found.
// ignore chars are read and ignored while looking for data or term.  stream failed could mean stream over or stop found.
/* meaning of booleans:
   success T/F  chars read/not read
   str good:   terminator found ; 
   str bad:   terminator not found--may be eof, stopfunc or some string char

*/
template<> istream& IO::read<string>(istream& ist, string& strread, bool& success, charIn termFunc, 
	                                                             charIn stopFunc, charIn ignoreFunc)
{
	char i;
	success = false;
	if (readOver(ist, termFunc, stopFunc, ignoreFunc)) // handles termfunc, stopfunc or failed stream
	{
		return ist; // will only enter here if termFunc, stopFunc true stream will be failed.
	}

	auto stopread = [termFunc, stopFunc, ignoreFunc](char i) {return  termFunc(i) || stopFunc(i) || ignoreFunc(i); };
	while (ist.get(i) && !stopread(i)) // guaranteed to enter loop if termfunc stop func not found
	{
		strread += i; // read successful
		success = true;
	}
	if (ist){
		ist.unget(); // return char to stream so readOver can analyze
	}
	if (!readOver(ist, termFunc, stopFunc, ignoreFunc)) // data found -not terminator
	{
		ist.clear(std::ios_base::failbit);
	}
	return ist;
}
bool readOver(istream& ist, IO::charIn termFunc, IO::charIn stopFunc, IO::charIn ignore)
{
	char ch;
	bool readDone = true;
	while (ist.get(ch)) {
		if (termFunc(ch)) {
			break;
		}
		else if (stopFunc(ch)) {
			ist.unget();
			ist.clear(std::ios_base::failbit);
			break;
		}
		else if (!ignore(ch))// valid char that should be tested
		{
			ist.unget();
			readDone = false;
			break;
		}
	}
	return readDone;  //unexpected char could be data
}
/*

//clear whitespace in stream. requires stream is not failed!
//precondition stream not failed
istream& clearwhitespace(istream& ist)
{
	//auto spacefunc = [](char ch){ return isspace(ch)!= 0; };
	char ch;
	ist >> ch; // read the next nonwhite space
	if (ist)
	{
		ist.unget();
	}
	return ist;
}
//this function will clear only the spaces not tabs cr etc so that 
//reading strings won't skip over these
istream& clearonlyspace(istream& ist)
{
	//auto spacefunc = [](char ch){ return isspace(ch)!= 0; };
	char ch;
	while (ist.get(ch) && ch == ' '); // read simple spaces leave cr and 
	if (ist)
	{
		ist.unget();
	}
	return ist;
}
/
//endofStreamLogic returns T/F to know whether read is over.  It checks these 

failed    termfunc		stopFunc	space	done?	strstate	putback
T			X			x			x		true	failed		no
F			T			x			x		true	good		no   normal case
F			F			T			x		true	failed		yes
F			F			F			T		false	return with no action space or other char not expected


bool endOfStreamLogic(istream& ist, char inchar, IO::charIn termfunc, IO::charIn stopFunc)
{
	if (!ist || termfunc(inchar)){
		return true;
	}
	if (stopFunc(inchar)){
		ist.unget();
		ist.clear(ios_base::failbit);
		return true;
	}
	return false;  // must have been a space or unexpected char
}



//fill_string_while will append characters to a string as long as pred is true;
// then it will return to the stream the character that caused term to say false;
istream& IO::fill_string_while(istream& ist, string& strread, charIn pred,
	int& size, int max)
{
	char i;
	size = 0;
	auto readmore = [&max, &size]() {return max == -1 || size < max; };
	while (readmore() && ist.get(i) && pred(i))
	{
		strread += i;
		size++;
	}
	if (readmore() && ist)
		ist.unget(); // failed because of pred(i)
	return ist;
}
*/
void IO::skip_to_int(istream& instr)
{
	if (instr.fail()) {          // we found something that wasn't an integer
		instr.clear();           // we'd like to look at the characters
		char ch;
		while (instr >> ch){       // throw away non-digits
			if (isdigit(ch) || '-' || '+') {
				instr.unget();   // put the digit back,
				// so that we can read the number
				return;
			}
		}
	}
	printstate(instr);// no int to read now
	throw std::runtime_error("no input");         // eof or bad: give up
}
void IO::printstate(const istream& instr)
{

	std::cerr << "Last Success: " << std::boolalpha << !!instr;
	std::cerr <<
		"; Good: " << std::boolalpha << instr.good() <<
		"; Eof : " << std::boolalpha << instr.eof() <<
		"; fail: " << std::boolalpha << instr.fail() <<
		"; bad: " << std::boolalpha << instr.bad() << std::endl;

}

ostream&  IO::printdouble(ostream& outstr, const double val, int precis)
{
	if (_isnan(val)) {
		outstr << "NaN";
		return outstr;
	}

	if (abs(val) > 1E4 || abs(val) < 1E-4){
		outstr << std::setprecision(3) << val;
		outstr << std::setprecision(precis);
		return outstr;
	}
	outstr << std::setprecision(precis);
	outstr << val;
	return outstr;

}


/*
bool readOver(istream& ist, IO::charIn termFunc, IO::charIn stopFunc, IO::charIn ignore)
{
	char ch;
	bool readDone = true;
	while (ist.get(ch)) {
		if (termFunc(ch)) {
			break;
		}
		else if (stopFunc(ch)) {
			ist.unget();
			ist.clear(std::ios_base::failbit);
			break;
		}
		else if (!ignore(ch))// valid char that should be tested
		{
			ist.unget();
			readDone = false;
			break;
		}
	}
	return readDone;  //unexpected char could be data
}*/
