// Source from http://www.cs.umd.edu/~hollings/cs412/s98/project/proj1/scancode
// and https://github.com/arjun024/mkeykernel/blob/master/keyboard_map.h

#ifndef SCANCODE_H
#define SCANCODE_H

#define ESC 27     /* ASCII escape */

/* Scancodes for shift and capslock keys: */

#define LSHIFT_PRESSED 0x2A          /* Scan codes for shift press, */
#define LSHIFT_RELEASED 0xAA        /* shift release and capslock  */
#define RSHIFT_PRESSED 0x36        /* keys.                       */
#define RSHIFT_RELEASED 0xB6
#define CAPSLOCK 0x3A
#define CAPSLOCK_RELEASED 0xBA 

/* scancodes for arrow keys: */

#define CODE_UP    72
#define CODE_DOWN  80
#define CODE_LEFT  75
#define CODE_RIGHT 77

/* scancodes for function keys (for project 5) */

#define F1 0x3B
#define F2 0x3C

/* ASCII codes to use for arrow keys: */

#define ASC_UP    '\200'
#define ASC_DOWN  '\201'
#define ASC_LEFT  '\202'
#define ASC_RIGHT '\203'

#define BACKSPACE  8       /* Ascii codes for Backspace, Tab and enter */
#define TAB        9      /* keys.                                    */
#define ENTER_KEY  13

/*  Note, not every single ASCII character has a scancode.  Only,
    the first 58 do. */

char asccode[58][2] =       /* Array containing ascii codes for
			       appropriate scan codes */
     {
       {   0,0   } ,
       { ESC,ESC } ,
       { '1','!' } ,
       { '2','@' } ,
       { '3','#' } ,
       { '4','$' } ,
       { '5','%' } ,
       { '6','^' } ,
       { '7','&' } ,
       { '8','*' } ,
       { '9','(' } ,
       { '0',')' } ,
       { '-','_' } ,
       { '=','+' } ,
       {'\b','\b'} ,
       {'\t','\t'} ,
       { 'q','Q' } ,
       { 'w','W' } ,
       { 'e','E' } ,
       { 'r','R' } ,
       { 't','T' } ,
       { 'y','Y' } ,
       { 'u','U' } ,
       { 'i','I' } ,
       { 'o','O' } ,
       { 'p','P' } ,
       { '[','{' } ,
       { ']','}' } ,
       {'\n','\n'} ,
       {   0,0   } ,
       { 'a','A' } ,
       { 's','S' } ,
       { 'd','D' } ,
       { 'f','F' } ,
       { 'g','G' } ,
       { 'h','H' } ,
       { 'j','J' } ,
       { 'k','K' } ,
       { 'l','L' } ,
       { ';',':' } ,
       {'\'','\"'} ,
       { '`','~' } ,
       {   0,0   } ,
       { '\\','|'} ,
       { 'z','Z' } ,
       { 'x','X' } ,
       { 'c','C' } ,
       { 'v','V' } ,
       { 'b','B' } ,
       { 'n','N' } ,
       { 'm','M' } ,
       { ',','<' } ,
       { '.','>' } ,
       { '/','?' } ,
       {   0,0   } ,
       {   0,0   } ,
       {   0,0   } ,
       { ' ',' ' } ,
   };

#endif // SCANCODE_H