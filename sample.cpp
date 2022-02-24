#include <iostream>
#include "html.h"
using namespace html;

int main() {
	string code = "<h1 a=\"a\"><div>hi</div></hi><bye c=\"d\" asd=\"sad\">j</bye>";

	setting set;
	set.domMaker = "Var.dom";
	set.textMaker = "Var.text";
	set.stateMaker = "Var.state";
	set.parentTag = "jsx";

	cout << main::make(code,set) << endl;

	return 0;
}
