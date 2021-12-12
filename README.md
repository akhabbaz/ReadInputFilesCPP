# ReadInputFilesCPP
Original code by Anton Khabbaz (anton.khabbaz@gmail.com)
  
 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any
 damages arising from the use of this software.
  
 Permission is granted to anyone to use this software for any
 purpose, including commercial applications, and to alter it and
 redistribute it freely, subject to the following restrictions:
  
 1. The origin of this software must not be misrepresented; you must
 not claim that you wrote the original software. If you use this
 software in a product, an acknowledgment in the product documentation
 would be appreciated but is not required.
  
 2. Altered source versions must be plainly marked as such, and
 must not be misrepresented as being the original software.
  
 3. This notice may not be removed or altered from any source
 distribution.
 */
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



Here is the output of the unit test file that loads in all the example files:

paces within data preserved, outside removed; default replace missing data, even after final comma<br>
comment 0 ; Select Your Choices for Assembly (candidates are randomly ordered) ; comment 2 ; Empty ; comment 4 ; Empty ; Empty ;

**** End of File 1

This demonstrates stream iterators actually use the same operator as >>
001122334455<br>
**** End of File 2

template constructor used; default used during error; Sep and term disjoint; no third line <br>
0 ;* 1 ;* 2 ;* 3 ;* 4 ;* 5 ;*<br>
Format Error on read; Line 1; readCount 0<br>
-1 ;* 2 ;* 3 ;* 4 ;* 5 ;* 6 ;*<br>
EOf reached<br>
**** End of File 3<br>

Trim and term overlap; both isspace, eol is '\n'; empty third line--default
used<br>
0 ;* 1 ;* 2 ;* 3 ;* 4 ;* 5 ;*<br>
-7 ;* 2 ;* 3 ;* 4 ;* 5 ;* 6 ;*<br>
-1 ;*<br>
Eof reached<br>
**** End of File 4<br>

CSV file; Read/Write Records, strings, errors<br>
Passport ; Great Artists ; Passport ; Passport ; Passport ; Passport ; Passport ;<br>
Chopin, Frederick ; Bach, JS ; Lennon, John ; Metallica, Band ; Darwish, Mahmoud ; Swift, Taylor ; Darwish, Mahmoud ;<br>
Nocturne Op. 9 in E-flat Major ; Siciliano ; Imagine ; Unforgiven ; Passport ; Shake it off ; Passport ;<br>
Format Error on read; Line 0; readCount 3<br>
[1810 - 1849] ; [1685 - 1750] ; [1940 - 1980] ; [0 - 0] ; [0 - 0] ; [1989 - 0] ; [0 - 0] ;<br>
12490596065 ;** -1 ;** 1 ;** -1 ;** -1 ;** -1 ;** 2 ;**<br>
12490612221 ;** 2 ;** -1 ;** -1 ;** 1 ;** -1 ;** -1 ;**<br>
12490613670 ;** 4 ;** 2 ;** 1 ;** -1 ;** 3 ;** -1 ;**<br>
12490613762 ;** -1 ;** -1 ;** 1 ;** -1 ;** -1 ;** -1 ;**<br>
12490613889 ;** -1 ;** 2 ;** -1 ;** 1 ;** -1 ;** -1 ;**<br>
12490613940 ;** 1 ;** 4 ;** 2 ;** 3 ;** 5 ;** -1 ;**<br>
12490614046 ;** 4 ;** 2 ;** 5 ;** 1 ;** 3 ;** -1 ;**<br>
12490614062 ;** 1 ;** 2 ;** 4 ;** 3 ;** 5 ;** -1 ;**<br>
-1 ;**<br>
Eof Found<br>
**** End of File 5<br>


