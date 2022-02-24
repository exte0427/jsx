#pragma once
#include <string>
#include <vector>

using namespace std;

namespace html {

	class setting {
	public:
		string domMaker;
		string stateMaker;
		string parentTag;
	};

	namespace parser {
		enum class tokenForm {
			domFirst,
			domLast,
			endDom,
			endItself,
			string,
			same,
			word,
		};

		class token {
		public:

			tokenForm tokenType;
			string strVal;
			long long int numVal;

			token(tokenForm tokenType_, string strVal_ = "", long long int numVal_ = 0) {
				tokenType = tokenType_;
				strVal = strVal_;
				numVal = numVal_;
			}
		};

		vector<token> parse(string code) {
			vector<token> returnTokens;

			for (long long int i = 0; i < code.size(); i++) {

				if (code[i] == '<') {
					if (code[i + 1] == '/') {
						returnTokens.push_back(token(tokenForm::domLast));
						i++;
					}
					else
						returnTokens.push_back(token(tokenForm::domFirst));
				}
				else if (code[i] == '/' && code[i + 1] == '>') {
					returnTokens.push_back(token(tokenForm::endItself));
					i++;
				}
				else if (code[i] == '>') {
					returnTokens.push_back(token(tokenForm::endDom));
				}
				else if (code[i] == '=')
					returnTokens.push_back(token(tokenForm::same));
				else if (code[i] == '"') {

					string data;
					i++;
					while (code[i] != '"') {
						data += code[i];
						i++;
					}

					returnTokens.push_back(token(tokenForm::string, data));
				}
				else if (('A' < code[i] && 'Z' > code[i]) || ('a' < code[i] && 'z' > code[i]) || ('0' < code[i] && '9' > code[i])) {
					string data;

					while (('A' < code[i] && 'Z' > code[i]) || ('a' < code[i] && 'z' > code[i]) || ('0' < code[i] && '9' > code[i])) {
						data += code[i];
						i++;
					}

					returnTokens.push_back(token(tokenForm::word, data));
					i--;
				}
			}

			return returnTokens;
		}
	}

	namespace maker {
		class state {
		public:
			string name;
			string value;

			state(string name_, string value_) {
				name = name_;
				value = value_;
			}
		};

		class dom {
		public:
			string type;
			vector<state> states;
			string data;
			vector<dom> childs;

			dom(string type_, vector<state> states_, string data_, vector<dom> childs_) {
				type = type_;
				states = states_;
				data = data_;
				childs = childs_;
			}
		};

		namespace domMake {
			class partDom {
			public:
				int start;
				int end;

				partDom() {
					start = 0;
					end = 0;
				}

				partDom(int start_, int end_) {
					start = start_;
					end = end_;
				}
			};

			class dom {
			public:
				vector<parser::token> firstTokens;
				vector<parser::token> lastTokens;
				vector<dom> kids;

				dom(vector<parser::token> firstTokens_, vector<parser::token> lastTokens_, vector<dom> kids_) {
					firstTokens = firstTokens_;
					lastTokens = lastTokens_;
					kids = kids_;
				}
			};
		}

		vector<state> findStates(vector<parser::token> tokens) {
			vector<state> returnStates;

			for (int i = 1; i < tokens.size(); i++)
				if (tokens[i].tokenType == parser::tokenForm::same)
					returnStates.push_back(state(tokens[i - 1].strVal, tokens[i + 1].strVal));

			return returnStates;
		}

		vector<dom> makeDom(vector<domMake::dom> doms) {
			vector<dom> returnDom;

			for (auto element : doms) {
				vector<dom> kidsDom = makeDom(element.kids);
				string domType = element.firstTokens[0].strVal;
				vector<state> states = findStates(element.firstTokens);

				dom myDom = dom(domType, states, element.lastTokens[0].strVal, kidsDom);

				returnDom.push_back(myDom);
			}

			return returnDom;
		}

		vector<dom> parse(vector<parser::token> tokens) {
			vector<int> domStartStart;
			vector<domMake::partDom> domStart;
			vector<int> domEndStart;
			vector<domMake::dom> childDom;
			vector<domMake::dom> finalDom;

			for (int index = 0; index < tokens.size(); index++) {
				auto element = tokens[index];

				if (element.tokenType == parser::tokenForm::word && (!domStartStart.size() && !domEndStart.size())) {
					vector<parser::token> firstToken, lastToken;
					firstToken.push_back(parser::token(parser::tokenForm::word, "string"));
					lastToken.push_back(parser::token(parser::tokenForm::word, element.strVal));

					childDom.push_back(domMake::dom(firstToken, lastToken, {}));
				}
				if (element.tokenType == parser::tokenForm::domFirst)
					domStartStart.push_back(index + 1);
				if (element.tokenType == parser::tokenForm::domLast)
					domEndStart.push_back(index - 1);
				if (element.tokenType == parser::tokenForm::endDom) {
					if (domStartStart.size() > 0) {
						int firstData = domStartStart[domStartStart.size() - 1];
						int lastData = index - 1;

						domStart.push_back(domMake::partDom(firstData, lastData));

						domStartStart.pop_back();
					}
					else if (domEndStart.size() > 0) {
						domMake::partDom firstData = domStart[domStart.size() - 1];
						domMake::partDom lastData = domMake::partDom(domEndStart[domEndStart.size() - 1], index - 1);

						vector<parser::token> firstTokens(tokens.begin() + firstData.start, tokens.begin() + firstData.end + 1);
						vector<parser::token> lastTokens(tokens.begin() + lastData.start, tokens.begin() + lastData.end + 1);

						vector<domMake::dom> kidsDom = childDom;
						domMake::dom addDom = domMake::dom(firstTokens, lastTokens, kidsDom);

						childDom.clear();
						domStart.pop_back();
						domEndStart.pop_back();

						if (domStart.size() == 0)
							finalDom.push_back(addDom);
						else
							childDom.push_back(addDom);
					}
				}
			}

			vector<dom> returnDom = makeDom(finalDom);
			return returnDom;
		}
	}

	namespace jsx {
		string makeState(vector<maker::state> states , setting set) {
			string returnVal = "[";

			for (auto element : states)
				returnVal += set.stateMaker + "(\"" + element.name + "\",\"" + element.value + "\"),";

			return returnVal + "]";
		}

		string makeCode(vector<maker::dom> myDom, setting set) {
			string retrunVal = "";

			for (auto element : myDom) {
				if (element.type == "string")
					retrunVal += "Var.text(\"" + element.data + "\"),";
				else {
					retrunVal += set.domMaker + "(\"" + element.type + "\"," + makeState(element.states, set) + "," + makeCode(element.childs, set) + "),";
				}
			}

			return retrunVal;
		}

		string make(string htmlCode, setting set) {
			vector<parser::token> tokens = parser::parse(htmlCode);
			vector<maker::dom> domData = maker::parse(tokens);
			string jsCode = set .domMaker+"(\"" + set.parentTag + "\",[]," + makeCode(domData, set) + ");";

			return jsCode;
		}
	}

}
