#include <iostream>
#include <ctime>
#include <climits>
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cctype>
#include "ParseTree.h"
#include "Node.h"
#include <stack>
#include <queue>
#include "stdafx.h"
#include<fstream>


using namespace std;


Type ParseTree::Scanner(const string& query, int &index,string &word, int &data)
{
	vector<string> keyword = { "CREATE", "TABLE", "DROP","SELECT", "DISTINCT", "FROM", "WHERE", "ORDER", "BY", "DELETE", "FROM","INT","STR20",
		"INSERT", "INTO", "VALUES", "NULL", "OR", "AND", "NOT","INT","STR20" };
	Type ret;
	while ((index != query.size()) && (query[index] == ' '|| query[index] == '"'))++index;
	
	//if read int
	if (query[index] >= '0' && query[index] <= '9') 
	{
		data = 0;
		while (query[index] >= '0' && query[index] <= '9') { data = data * 10 + query[index] - '0'; ++index; }
		return INT;
	}

	if ((query[index] >= 'a' && query[index] <= 'z') || (query[index] >= 'A' && query[index] <= 'Z'))
	{
		word = ""; ret = WORD;
		int start = index;
		while ((query[index] >= '0' && query[index] <= '9') || (query[index] >= 'a' && query[index] <= 'z') || (query[index] >= 'A' && query[index] <= 'Z')||query[index]=='.')
		{
			if (query[index] == '.') { ret = COL_NAME; }
			++index;
		}
		word = query.substr(start, index-start);
		string tmp = word;
		transform(tmp.begin(), tmp.end(),tmp.begin(), ::toupper);
		if (find(begin(keyword), end(keyword), tmp) != end(keyword)) { word = tmp; ret = KEYWORD; }
		return ret;
	}

	if (query[index] == '\0')return EOL;
	++index; word = "";
	switch (query[index - 1])
	{
	case'<':
	case'>':
	case'=':
		ret = COMP_OP;
		break;
	case'+':
	case'-':
	case'*':
	case'(':
	case')':
	case'[':
	case']':
	case',':
	case'/':
		ret = OPERATOR;
		break;
	default:
		cerr << "scanner ERROR: invalid input charactor" << query[index - 1] << endl;
		ret = OPERATOR;
	}
	word = query[index - 1];
	return ret;


};

Node* ParseTree::createParseTree(string &query) 
{
	Node* root = NULL;
	int index = 0;
	Type tokenType; 
	int data;
	string word;

		tokenType = Scanner(query, index, word, data);


			if (word == "CREATE") { root = createTableTree(query, index); }
			else if (word == "INSERT") { root = insertTree(query, index); }
			else if (word == "DELETE") { root = deleteTree(query, index); }
			else if (word == "DROP") { root = dropTableTree(query, index); }
			else if (word == "SELECT") { root = selectTree(query, index); }

		
		else
		{
			cerr << "create Parse Tree ERROR: No statement is found." << endl;
		}
	
	return root;

}

Node* ParseTree::createTableTree(string &query, int &index)
{
	Type tokenType;
	int data;
	string word;
	Node *root = new Node(CREATE_TABLE_SMT);
	root->child.push_back(new Node(KEYWORD, "CREATE"));

	
	while (index != query.size()) {
		tokenType = Scanner(query, index, word, data);
		switch (tokenType) {
		case KEYWORD:
			if (word == "TABLE" && root->child.size() == 1) {
				root->child.push_back(new Node(KEYWORD, "TABLE"));
			}
			else { cerr << "invalid input ERROR:drop table statement." << endl; }
			break;
		case WORD:
			if (root->child.size() == 2) {
				root->child.push_back(new Node(TABLE_NAME, word));
			}
			break;
		case OPERATOR:
			if (word == "(") {
				root->child.push_back(new Node(OPERATOR, word));
				root->child.push_back(attrTypeTree(query, index));
				break;
			}
			if (word == ")") { root->child.push_back(new Node(OPERATOR, word)); }
			break;
		case EOL:
			return root;
		}
	}
	return root;
}

Node* ParseTree::dropTableTree(string &query, int &index)
{
	Type tokenType;
	int data;
	string word;
	
	Node *root = new Node(DROP_TABLE_SMT);
	root->child.push_back(new Node(KEYWORD, "DROP"));

	while (index != query.size()) {
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		case KEYWORD:
			if (word == "TABLE" && root->child.size() == 1) { 
				root->child.push_back(new Node(KEYWORD, "TABLE"));
			}
			else { cerr << "invalid input ERROR:drop table statement." << endl; }
			break;
		case WORD:
			if (root->child.size() == 2) {
				root->child.push_back(new Node(TABLE_NAME, word)); 
			}
			break;
		case EOL:
			return root;
		}

	}
	return root;
}

Node* ParseTree::insertTree(string &query, int &index) 
{
	Type tokenType;
	int data;
	string word;

	Node *root = new Node(INSERT_SMT);
	root->child.push_back(new Node(KEYWORD, "INSERT"));

	while (index != query.size()) {
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		case KEYWORD:
			if (word == "INTO"&&root->child.size() == 1) { root->child.push_back(new Node(KEYWORD, "INTO")); }
			else { cerr << "invalid input ERROR:insert table statement." << endl; }
			break;
		case WORD:
			if (root->child.size() == 2) { root->child.push_back(new Node(TABLE_NAME, word)); }
			break;
		case OPERATOR:
			if (word == "(") { root->child.push_back(new Node(OPERATOR, word));
			root->child.push_back(attrListTree(query, index));}
			if (word == ")") { root->child.push_back(new Node(OPERATOR, word));
			root->child.push_back(insertTupleTree(query, index));
			}
			break;
		case EOL:
			return root;
		}

	}
	return root;

}

Node* ParseTree::deleteTree(string &query, int &index)
{
	Type tokenType;
	int data;
	string word;

	Node *root = new Node(DELETE_SMT);
	root->child.push_back(new Node(KEYWORD, "DELETE"));

	while (index != query.size()) {
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		case KEYWORD:
			if (word == "FROM"&&root->child.size() == 1) { root->child.push_back(new Node(KEYWORD, "FROM")); }
			else if (word == "WHERE") {root->child.push_back(new Node(KEYWORD, "WHERE"));
			root->child.push_back(searchConditionTree(query, index));
			return root;
			}
			else { cerr << "invalid input ERROR:delete table statement." << endl; }
			break;
		case WORD:
			if (root->child.size() == 2) { root->child.push_back(new Node(TABLE_NAME, word)); }
			break;
		case EOL:
			return root;
		}

	}
	return root;
}

Node* ParseTree::selectTree(string &query, int &index)
{
	Type tokenType;
	int data;
	string word;

	Node *root = new Node(SELECT_SMT);
	root->child.push_back(new Node(KEYWORD, "SELECT"));

	while (index != query.size()) {
		int start = index;
		tokenType = Scanner(query, index, word, data);

		
		
		switch (tokenType) {
		case OPERATOR:
			if (root->child.back()->word == "DISTINCT" || root->child.back()->word == "SELECT")
			{
				index = start;
				root->child.push_back(selectListTree(query, index));
			}
			break;
		case KEYWORD:
			if (word == "DISTINCT"&&root->child.size() == 1) {
				root->child.push_back(new Node(KEYWORD, "DISTINCT"));
				break;
			}
			if (word == "FROM" && root->child.back()->type==SELECT_LIST) {
				root->child.push_back(new Node(KEYWORD, "FROM"));
				root->child.push_back(tableListTree(query, index));
				break;
			}
			if (word == "WHERE" && root->child.back()->type == TABLE_LIST) {
				root->child.push_back(new Node(KEYWORD, "WHERE"));
				root->child.push_back(searchConditionTree(query, index));
				break;
			}
			if (word == "ORDER" && (root->child.back()->type == TABLE_LIST || root->child.back()->type == SEARCH_COND)) {
				root->child.push_back(new Node(KEYWORD, "ORDER"));
				break;
			}
			if (word == "BY" && root->child.back()->word == "ORDER") 
			{
				root->child.push_back(new Node(KEYWORD, "BY"));
				break;
			}
			else { cerr << "invalid input ERROR:select table statement." << endl;
			break;
			}
			
		case COL_NAME:
		case WORD:
			if (root->child.back()->word == "DISTINCT" || root->child.back()->word == "SELECT") {
				index = start;
				root->child.push_back(selectListTree(query, index));
				break;
			}
			else if(root->child.back()->word == "BY")
			{
				root->child.push_back(new Node(COL_NAME, word));
				break;
			}
			else { cerr << "invalid input ERROR:select table statement." << endl; 
			break;
			}
		case EOL:
			return root;

		}

	}
	return root;

}

Node* ParseTree::tableListTree(string &query, int &index)
{
	Node *root = new Node(TABLE_LIST);
	Type tokenType;
	int data;
	string word;
	while (index!=query.size()) {
		int start = index;
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		case WORD:
			root->child.push_back(new Node(TABLE_NAME, word));
			break;
		case OPERATOR:
			if (word == ",") {
				root->child.push_back(new Node(OPERATOR, word));
				root->child.push_back(tableListTree(query, index));
			}
			else { cerr << "attr list Tree build ERROR: invalid operator is found." << endl;}
			break;
		case KEYWORD:
			index = start;
			return root;
		case EOL:
			return root;
		}

	}
	return root;

}

Node* ParseTree::selectListTree(string& query, int &index) 
{
	Node *root = new Node(SELECT_LIST);
	Type tokenType;
	int data;
	string word;
	int start = index;
	tokenType = Scanner(query, index, word, data);

	if (word == "*")
	{
		root->child.push_back(new Node(WORD, word));
	}
	else
	{
		index = start;
		root->child.push_back(selectSubListTree(query,index));
	}
	return root;

}

Node* ParseTree::selectSubListTree(string &query, int &index)
{
	Node *root = new Node(SELECT_SUBLIST);
	Type tokenType;
	int data;
	string word;
	while (word != "FROM") {
		int start = index;
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		case COL_NAME:
			root->child.push_back(new Node(COL_NAME, word));
			break;
		case OPERATOR:
			if (word == ",") {
				root->child.push_back(new Node(OPERATOR, word));
				root->child.push_back(selectSubListTree(query, index));
				return root;
			}
			else
			{
				cout << "select subList build ERROR: invalid operator found!" << word << endl;
				break;
			}
			
		case KEYWORD:
			if (word == "FROM") 
			{
			   index = start;
				return root;
			}
			else
			{
				cout << "select subList build ERROR: invalid KEYWORD found!" << word << endl;
			}
			break;
		case EOL:
			return root;
		}

	}
	
	return root;

}

Node* ParseTree::attrTypeTree(string &query, int &index)
{
	Node *root = new Node(ATTR_TYPE_LIST);
	Type tokenType;
	int data;
	string word;
	while ( word!=")") {
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		case WORD:
			root->child.push_back(new Node(ATTR_NAME, word));
			break;
		case KEYWORD:
			if (word == "INT" || word == "STR20") { root->child.push_back(new Node(DATA_TYPE, word)); }
			else { cerr << "attr list Tree build ERROR: invalid operator is found." << endl; }
			break;
		case OPERATOR:
			if (word == ",") {
				root->child.push_back(new Node(OPERATOR, word));
				root->child.push_back(attrTypeTree(query, index));
				break;
			}
			if (word == ")") { index--; return root; }
			break;
		case EOL:
			return root;
		}

	}
	return root;

}

Node* ParseTree::attrListTree(string &query, int &index)
{
	Node *root = new Node(ATTR_LIST);
	Type tokenType;
	int data;
	string word;
	while (word != ")") {
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		case WORD:
			root->child.push_back(new Node(ATTR_NAME, word)); 
			break;
		case OPERATOR:
			if (word == ",") { 
				root->child.push_back(new Node(OPERATOR, word)); 
				root->child.push_back(attrListTree(query,index));
				break;
			}
			if (word == ")") { index--; return root; }
			else { cerr << "attr list Tree build ERROR: invalid operator is found." << endl; break; }
		case EOL:
			return root;
		}

	}
	return root;

}

Node* ParseTree::insertTupleTree(string &query, int &index)
{
	Node *root = new Node(INSERT_TUPLES);
	Type tokenType;
	int data;
	string word;
	while (index != query.size()) {
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		case KEYWORD:
			if(word=="SELECT")
			{
				root->child.push_back(selectTree(query, index));
			}
			if (word == "VALUES") 
			{
				root->child.push_back( new Node(KEYWORD,word));
			}
			break;
		case OPERATOR:
			if (word == "(") {
				root->child.push_back(new Node(OPERATOR, word));
				root->child.push_back(valueListTree(query,index));
				break;
			}
			if (word == ")") { root->child.push_back(new Node(OPERATOR, word)); }
			else { cerr << "attr list Tree build ERROR: invalid operator is found." << endl;  }
			break;
		case EOL:
			return root;
		}

	}
	return root;

}

Node* ParseTree::valueListTree(string &query, int &index)
{
	Node *root = new Node(VALUE_LIST);
	Type tokenType;
	int data;
	string word;
	while (word != ")") {
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		case WORD:
			root->child.push_back(new Node(WORD, word));
			break;
		case INT:
			root->child.push_back(new Node(INT, data));
			break;
		case KEYWORD:
			root->child.push_back(new Node(KEYWORD, word));
			break;
		case OPERATOR:
			if (word == ",") {
				root->child.push_back(new Node(OPERATOR, word));
				root->child.push_back(valueListTree(query, index));
				break;
			}
			if (word == ")") {index--; return root;}
			else { cerr << "value list Tree build ERROR: invalid operator is found." << endl;  }
			break;
		case EOL:
			return root;
		}

	}
	return root;

}

Node* ParseTree::searchConditionTree(string &query, int &index)
{
	Node *root = new Node(SEARCH_COND);
	Type tokenType;
	int data;
	string word;
	while (index != query.size()) {
		int start = index;
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		case KEYWORD:
			if (word == "OR" && root->child.size() != 0 && root->child[0]->type == BOOL_TERM)
			{
				root->child.push_back(new Node(KEYWORD, word));
				root->child.push_back(searchConditionTree(query, index));
				return root;
			}
			else {
				index = start;
				return root;
			}
		case EOL:
			return root;
		case INT:
		case WORD:
		case OPERATOR:
		case COMP_OP:
		case COL_NAME:
			index = start;
			root->child.push_back(boolTermTree(query, index));
			break;
		default:
			index = start;
			return root;
		}

	}
	return root;
}

Node* ParseTree::boolTermTree(string &query, int &index)
{
	Node *root = new Node(BOOL_TERM);
	Type tokenType;
	int data;
	string word;
	while (index != query.size()) {
		int start = index;
		tokenType = Scanner(query, index, word, data);
	

		switch (tokenType) {
		case KEYWORD:
			if (word == "AND" && root->child.size() != 0 && root->child[0]->type == BOOL_FACTOR)
			{
				root->child.push_back(new Node(KEYWORD, word));
				root->child.push_back(boolTermTree(query, index));
				return root;
			}
			else {
				index = start;
				return root;
			}
		case EOL:
			return root;
		case INT:
		case WORD:
		case OPERATOR:
		case COMP_OP:
		case COL_NAME:
			index = start;
			root->child.push_back(boolFactorTree(query, index));
			break;
		default:
			index = start;
			return root;
		}

	}
	return root;

}

Node* ParseTree::boolFactorTree(string &query, int &index)
{
	Node *root = new Node(BOOL_FACTOR);
	Type tokenType;
	int data;
	string word;


	if (root->child.size() == 0)
	{
		root->child.push_back(expressionTree(query, index));
	}
	if(index!=query.size() && root->child.size() == 1 && root->child[0]->type == EXPRESSION)
	{
		tokenType = Scanner(query, index, word, data);
		root->child.push_back(new Node(COMP_OP,word));
		root->child.push_back(expressionTree(query, index));
	}
	else {
		cerr << "boolean Factor Tree build ERROR: invalid charctor is found." << endl;
	}
	return root;

}

Node* ParseTree::expressionTree(string &query, int &index)
{
	Node *root = new Node(EXPRESSION);
	Type tokenType;
	int data;
	string word;

	while (index!=query.size()) {
		int start = index;
		tokenType = Scanner(query, index, word, data);
		switch (tokenType) {
		case OPERATOR:
			if (root->child.size() == 0 && word == "(") { root->child.push_back(new Node(OPERATOR, word)); break; }
			if ((root->child.size() == 4 && root->child.back()->type==TERM && word == ")")){ root->child.push_back(new Node(OPERATOR, word)); return root; }
			if ((word == "+" || word == "-" || word == "*") && root->child.size() != 0 && (root->child.back()->type == TERM))
			{
				root->child.push_back(new Node(OPERATOR, word));
				break;
			}
			
		case EOL:
			return root;
		case COL_NAME:
		case WORD:
		case INT:
			index = start;
			root->child.push_back(termTree(query, index));
			break;
		default:
			index = start;
			return root;
		}

	}
	return root;

}

Node* ParseTree::termTree(string &query, int &index) 
{
	Node *root = new Node(TERM);
	Type tokenType;
	int data;
	string word;
	int start = index;
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		case COL_NAME:
			root->child.push_back(new Node(COL_NAME, word));
			break;
		case WORD:
			root->child.push_back(new Node(WORD,word));
			break;
		case INT:
			root->child.push_back(new Node(INT,data));
			break;
		case EOL:
			return root;
		default:
			index = start;
			return root;
		}

	
	return root;

}
/*
void ParseTree::printParseTree(Node* t)
{

	stack<Node*> ret;
	stack<Node*> outPut;
	//stack<vector<Node*>> childList;
	Node* tmp;
	
	ret.push(t);


	while (!ret.empty())
	{
		tmp = ret.top();
		ret.pop();
		int index = 0;
		if (tmp->child.size() == 0)
		{
			outPut.push(tmp);
		}
		else
		{
			while (index != tmp->child.size())
			{
				ret.push(tmp->child[index]);
				index++;
			}
		}

	}

	while (!outPut.empty()) 
	{
		tmp = outPut.top();
		outPut.pop();
		if (tmp->type == INT) { out << tmp->data << " "; }
		else { out << tmp->word << " "; }
	}
	out << endl;
	return;
}*/



void ParseTree::printParseTree() const {
	printParseTree(cout);
}

void ParseTree::printParseTree(ostream &out) const {
	if (root!=NULL) {
		stack<Node*> ret;
		stack<Node*> outPut;
		//stack<vector<Node*>> childList;
		Node* tmp;

		ret.push(root);
		while (!ret.empty())
		{
			tmp = ret.top();
			ret.pop();
			int index = 0;
			if (tmp->child.size() == 0)
			{
				outPut.push(tmp);
			}
			else
			{
				while (index != tmp->child.size())
				{
					ret.push(tmp->child[index]);
					index++;
				}
			}

		}

		while (!outPut.empty())
		{
			tmp = outPut.top();
			outPut.pop();
			if (tmp->type == INT) { out << tmp->data << " "; }
			else { out << tmp->word << " "; }
		}
		out << endl;
	}
	else 
	{
		out << "No tree is detected!" << endl;
	}
	return;
}

ostream &operator<<(ostream &out, const ParseTree &pt) {
	pt.printParseTree(out);
	return out;
}