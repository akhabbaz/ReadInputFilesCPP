//read elements in vector until the term found
#include <stdexcept>
#include <iomanip>
bool readOver(std::istream& ist, IO::charIn termFunc, IO::charIn stopFunc, IO::charIn ignore);
// 
bool trimStream(std::istream& ist, IO::charIn trim);

bool IO::falseFunction(char ch) {
	return false;
}


//reads T types separated by characters given by ignore(ch) == true, which can be many in a row; filling of vector
// continues until termfunc is found and fails if not found. vector<T> grows for each successful read;
// str good:   stop term found
// str bad:    eof, bad char read, or read failed--latter two cases char returned to stream
template<class T> istream& IO::fill_vector(istream& ist, vector<T>& v, charIn termfunc, charIn ignore)
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
		std::runtime_error("max out of bounds");
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
		std::runtime_error("max out of bounds");
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
		std::runtime_error("max out of bounds");
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
		std::runtime_error{ message };
	return ist;
}

template< class T> ostream&  IO :: print_vector(ostream& ostr, const  T& invec, const int modulo)
{
	if (modulo < 1) 
		std::runtime_error("Modulo too small");
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
		std::runtime_error("bad stream...");
	     // bad
}

bool IO::isPeriod(char ch) {
	return ch == '.';
}
bool IO::isspace(char ch) {
	return ::isspace(ch) != 0;
}
bool IO::isComma(char ch) {
	return ch == ',';
}
bool IO::isSemiColon(char ch){
	return ch == ';';
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
//clearSpacesWhileCheckingStop will remove spaces from stream failing stream if stopfunc
// true (in that case puts char back.) and leaving stream ready to read data with stream
// good if a nonstopFunc found
istream& clearSpacesWhileCheckingStop(istream& ist, IO::charIn Ignore, IO::charIn stopFunc)
{
	char ch;
	while (ist.get(ch) && Ignore(ch));
	//either stream failed or nonspace found
	if (ist) //nonspace found
	{
		ist.unget();
		if (stopFunc(ch))
		{
			ist.clear(ios_base::failbit);
		}
	}
	return ist;
}
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


/*readOver clears  known chars from stream so that read can work.  It returns readDone;
Functions:

termFunc  indicates read of this value over but more values can be read; stream good
stopFunc  indicates read over, but the end of the array is reached stream failed
ignore    chars like space not chars of data nor terminators. These chars removed from stream, but read not over.
the checks are ordered failedstream, termfunc, stopfunc, ignore; so that ignore is only tested for nonmatching chars.

failed    termfunc		stopFunc	Ignore	ReadOver?	strstate	putback
T			X			x			x		true	failed		no
F			T			x			x		true	good		no      normal case
F			F			T			x		true	failed		yes
F			F			F			T		---  	good        no     continue reading until not ignore
F           F           F           F       false    good        yes     data char found.  End looking for chars and
																		terminate

this means that when it returns

ReadOver      strstate    meaning
T               Good       term found
T               bad        eof found or endofArray found
F               good       non control char found



*/
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
}
*/