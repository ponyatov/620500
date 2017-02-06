#ifndef _H_HPP
#define _H_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <cmath>
#include <cassert>
using namespace std;

struct Sym {					// universal symbolic type (AST node)
								// _ MUST be virtual for algebraic class tree _
	string tag;					// T: type/class marker
	string val;					// V: data item value (string: universal repr)
	Sym(string T,string V);		// <T:V> pair constructor
	Sym(string V);				// lexer token constructor
	vector<Sym*> nest;			// \ nested tree elements
	void push(Sym*);			// / push nest[]ed element
	map<string,Sym*> attr;		// attributes
	Sym* operator%(Sym*);		// /
	string dump(int depth=0);	// \ dump data item in tree form
	virtual string head();		//   <T:V> representation string
	string pad(int);			// /
	virtual Sym* eval(Sym*);	// evaluate/execute/interpret tree
	virtual Sym* pfxadd();		// + A
	virtual Sym* pfxsub();		// - A
	virtual Sym* add(Sym*);		// A + B
	virtual Sym* sub(Sym*);		// A - B
	virtual Sym* mul(Sym*);		// A * B
	virtual Sym* div(Sym*);		// A / B
	virtual Sym* pow(Sym*);		// A ^ B
	virtual Sym* at(Sym*);		// A @ B apply
};

struct Env:Sym { Env(string); };// environment
extern Env glob;				// global environment
extern void glob_init();		// initialize glob.env

struct Error:Sym { Error(string); };// error object

struct Str:Sym { Str(string);	// string
	string head(); };

struct Num:Sym { Num(string);	// number
	Num(double);				// float constructor
	double val;					// redef value as machine float
	string head(); 				// redef header for float:val
	Sym* pfxadd();				// redef: + num:A
	Sym* pfxsub();				// redef: - num:A
	Sym* add(Sym*);				// redef: num:A + B
	Sym* sub(Sym*);				// redef: num:A - B
	Sym* mul(Sym*);				// redef: num:A * B
	Sym* div(Sym*);				// redef: num:A / B
	Sym* pow(Sym*);				// redef: num:A ^ B
};

struct Op:Sym { Op(string);		// operator
	Sym* eval(Sym*);			// required for expressions evaluate
	};

typedef Sym* (*FN)(Sym*);		// (internal) function
struct Fn:Sym { Fn(string,FN);
	FN fn;						// pointer to internal in-compiled function
	Sym* at(Sym*);				// must be redefined as *fn(Sym*) call
								// predefined core functions:
	static Sym* sin(Sym*);		// \ trigonometry
	static Sym* cos(Sym*);		// /
	static Sym* date(Sym*);		// date&time
};

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
