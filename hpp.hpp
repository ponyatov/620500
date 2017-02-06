#ifndef _H_HPP
#define _H_HPP

#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
using namespace std;

struct Sym {					// universal symbolic type (AST node)
	string tag;					// T: type/class marker
	string val;					// V: data item value (string: universal repr)
	Sym(string T,string V);		// <T:V> pair constructor
	Sym(string V);				// lexer token constructor
	vector<Sym*> nest;			// \ nested tree elements
	void push(Sym*);			// / push nest[]ed element
	string dump(int depth=0);	// \ dump data item in tree form
	string head();
	string pad(int);			// /
};

struct Op:Sym {
};

extern int yylex();				// \ lexer interface
extern int yylineno;			// line number
extern char* yytext;			// / selected lexeme pointer
#define TOC(C,X) { yylval.o = new Sym(yytext); return X; } /* token */
extern int yyparse();			// \ syntax parser interface
extern void yyerror(string);
#include "ypp.tab.hpp"			// /

#endif // _H_HPP
