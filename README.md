# ReadInputFilesCPP

 
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
