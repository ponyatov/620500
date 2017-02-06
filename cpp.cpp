#include "hpp.hpp"
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cout<<YYERR; cerr<<YYERR; exit(-1); }
int main(int argc, char *argv[]) {
	glob_init();
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

Sym* Sym::operator%(Sym*o) { attr[o->val] = o; return this; }

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

Sym* Sym::eval(Sym*E) {
	if (E->attr.count(val) != 0) return E->attr[val];	// symbol lookup
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)	// recursive eval
		(*it) = (*it)->eval(E); // !!!! ALARM:: MEMORY LEAK HERE !!!!
	return this; }

Sym* Sym::pfxadd()   { return new Error(" + "+head()); }
Sym* Sym::pfxsub()   { return new Error(" - "+head()); }
Sym* Sym::add(Sym*o) { return new Error(head()+" + "+o->head()); }
Sym* Sym::sub(Sym*o) { return new Error(head()+" - "+o->head()); }
Sym* Sym::mul(Sym*o) { return new Error(head()+" * "+o->head()); }
Sym* Sym::div(Sym*o) { return new Error(head()+" / "+o->head()); }
Sym* Sym::pow(Sym*o) { return new Error(head()+" ^ "+o->head()); }
Sym* Sym::at(Sym*o)  { return new Error(head()+" @ "+o->head()); }

Error::Error(string V):Sym("error",V) { yyerror(V); }

Str::Str(string V):Sym("str",V){}
string Str::head() { ostringstream os;
	os << "'" << val << "' #" << this; return os.str(); }

Num::Num(string V):Sym("num","") { val = atof(V.c_str()); }
Num::Num(double F):Sym("num","") { val = F; }
string Num::head() { ostringstream os;
	os << "<" << tag << ":" << val << "> #" << this;
	return os.str(); }
Sym* Num::pfxadd() { return this; }
Sym* Num::pfxsub() { return new Num(-val); }
Sym* Num::add(Sym*o) {
	if (o->tag == "num") return new Num(val + dynamic_cast<Num*>(o)->val);
	return Sym::add(o); }
Sym* Num::sub(Sym*o) {
	if (o->tag == "num") return new Num(val - dynamic_cast<Num*>(o)->val);
	return Sym::sub(o); }
Sym* Num::mul(Sym*o) {
	if (o->tag == "num") return new Num(val * dynamic_cast<Num*>(o)->val);
	return Sym::mul(o); }
Sym* Num::div(Sym*o) {
	if (o->tag == "num") return new Num(val / dynamic_cast<Num*>(o)->val);
	return Sym::div(o); }
Sym* Num::pow(Sym*o) {
	if (o->tag == "num")
		return new Num(std::pow(val,dynamic_cast<Num*>(o)->val));
	return Sym::pow(o); }

Op::Op(string V):Sym("op",V){}
Sym* Op::eval(Sym*E) { Sym::eval(E);
	switch (nest.size()) {
		case 1:
			if (val=="+") return nest[0]->pfxadd();
			if (val=="-") return nest[0]->pfxsub();
		case 2:
			if (val=="+") return nest[0]->add(nest[1]);
			if (val=="-") return nest[0]->sub(nest[1]);
			if (val=="*") return nest[0]->mul(nest[1]);
			if (val=="/") return nest[0]->div(nest[1]);
			if (val=="^") return nest[0]->pow(nest[1]);
			if (val=="@") return nest[0]->at(nest[1]);
	}
	return this; }

Fn::Fn(string V,FN F):Sym("fn",V) { fn = F; }
Sym* Fn::at(Sym*o) { return fn(o); }

Sym* Fn::sin(Sym*o) {
	if (o->tag=="num")
		return new Num(std::sin(dynamic_cast<Num*>(o)->val));
	else return new Error("sin( "+o->head()+" )"); }
Sym* Fn::cos(Sym*o) {
	if (o->tag=="num")
		return new Num(std::cos(dynamic_cast<Num*>(o)->val));
	else return new Error("cos( "+o->head()+" )"); }

Sym* Fn::date(Sym*) { return new Str("DD/MM/YYYY"); }

Env::Env(string V):Sym("env",V){}

Env glob("global");
#include "meta.hpp"
void glob_init() {
	//metainfo
	glob.attr["MODULE"]	= new Str(MODULE);
	glob.attr["TITLE"]	= new Str(TITLE);
	glob.attr["ABOUT"]	= new Str(ABOUT);
	glob.attr["AUTHOR"]	= new Str(AUTHOR);
	glob.attr["LICENCE"]= new Str(LICENCE);
	glob.attr["GITHUB"]	= new Str(GITHUB);
	glob.attr["COPYRIGHT"] = new Op("+");
	glob.attr["COPYRIGHT"]->push(new Str("(c) "));
	glob.attr["COPYRIGHT"]->push(new Sym("AUTHOR"));
	glob.attr["COPYRIGHT"]->push(new Sym("LICENCE"));
	glob.attr["COPYRIGHT"]->push(new Sym("DATE"));
	glob.attr["COPYRIGHT"]->attr["DATE"] = new Fn("date",Fn::date);
	// trigonometry
	glob.attr["pi"] = new Num(M_PI);
	glob % new Fn("sin",Fn::sin);
	glob % new Fn("cos",Fn::cos);
}
