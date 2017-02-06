#ifndef _H_HPP
#define _H_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <map>
using namespace std;

struct Sym {					// universal symbolic type (AST node)
								// _ MUST be virtual for algebraic class tree _
	string tag;					// T: type/class marker
	string val;					// V: data item value (string: universal repr)
	Sym(string T,string V);		// <T:V> pair constructor
	Sym(string V);				// lexer token constructor
	vector<Sym*> nest;			// \ nested tree elements
	void push(Sym*);			// / push nest[]ed element
	string dump(int depth=0);	// \ dump data item in tree form
	virtual string head();		//   <T:V> representation string
	string pad(int);			// /
};

struct Num:Sym { Num(string);	// number
	float val;					// redef value as machine float
	string head(); };			// redef header for float:val

struct Op:Sym { Op(string); };	// operator
struct Fn:Sym { Fn(string); };	// (internal) function

extern int yylex();				// \ lexer interface
extern int yylineno;			// line number
extern char* yytext;			// / selected lexeme pointer
#define TOC(C,X) { yylval.o = new C(yytext); return X; } /* token */

typedef struct yy_buffer_state * YY_BUFFER_STATE;		// \ lexer from string
extern YY_BUFFER_STATE yy_scan_buffer(char *, size_t);
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);	// /

extern int yyparse();			// \ syntax parser interface
extern void yyerror(string);
#include "ypp.tab.hpp"			// /

#endif // _H_HPP
