#include "hpp.hpp"
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cout<<YYERR; cerr<<YYERR; exit(-1); }
int main(int argc, char *argv[]) {
	stringstream SRC_stream; string SRC; YY_BUFFER_STATE lex_buffer;
	for (int i=1;(argc>1 && i<argc);i++) { // interpret files from command line
		cout << "\n************* " << argv[i] << " *************\n";
		// read source (TextInput form emulation)
		SRC_stream << ifstream("src.src").rdbuf(); // read via sstream
		SRC = SRC_stream.str();
		// set flex buffer to string input & parse
		lex_buffer = yy_scan_string(SRC.c_str());	// alloc with cstring
		yyparse();									// parse
		yy_delete_buffer(lex_buffer);				// free
	}
return 0; }

Sym::Sym(string T, string V) { tag = T; val = V; }
Sym::Sym(string V):Sym("sym",V){}
void Sym::push(Sym*o) { nest.push_back(o); }

string Sym::head() { ostringstream os;
	os << "<" << tag << ":" << val << "> #" << this;
	return os.str(); }
string Sym::pad(int n) { string S; for (int i=0;i<n;i++) S+='\t'; return S; }
string Sym::dump(int depth) { string S = "\n" + pad(depth) + head();
	for (auto it=attr.begin(),e=attr.end();it!=e;it++) { // attr{}ibutes
		S += "\n"+pad(depth+1) + it->first + " =";
		S += it->second->dump(depth+2);
	}
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) // nest[]ed elements
		S += (*it)->dump(depth+1); // recurse over tree
	return S; }

Op::Op(string V):Sym("op",V){}
Fn::Fn(string V):Sym("fn",V){}

Num::Num(string V):Sym("num","") { val = atof(V.c_str()); }
string Num::head() { ostringstream os;
	os << "<" << tag << ":" << val << "> #" << this;
	return os.str(); }
