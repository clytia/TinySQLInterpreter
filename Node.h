#ifndef _NODE_H
#define _NODE_H

#include<string>
#include<vector>
using namespace std;

enum Type {
	STATEMENT, CREATE_TABLE_SMT, DROP_TABLE_SMT, SELECT_SMT, DELETE_SMT, INSERT_SMT,
	ATTR_LIST, ATTR_TYPE_LIST, DATA_TYPE, SELECT_LIST, SELECT_SUBLIST, TABLE_LIST, INSERT_TUPLES,
	VALUE, VALUE_LIST, SEARCH_COND, BOOL_TERM, BOOL_FACTOR, BOOL_PRIM, TERM, EXPRESSION, COMPARE_PRED,
	KEYWORD, WORD, COMP_OP,LOG_OP,OPERATOR,TABLE_NAME,ATTR_NAME,COL_NAME,DATA,EOL,RESULT,
};
class ParseTree;


class Node {

public:

	Type type;
	string word;
	int data;
	
	vector<Node*> child;

	Node(Type t) { type = t; }
	Node(Type t, string s ) { type = t; word = s;}

	Node(Type t, int i ) { type = t; data = i; }
	Node(Type t, string s , int i) { type = t; word = s; data = i; }
	~Node() {}

};


#endif