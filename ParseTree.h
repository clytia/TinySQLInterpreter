#ifndef _PARSETREE_H
#define _PARSETREE_H

using namespace std;

#include<string>;
#include<vector>;
#include"Node.h";

class ParseTree {

	


	Node* root;

	Type Scanner(const string& query, int &index, string &word, int &data);
	

	//Type 
	Node* createTableTree(string &query,int &index);
	Node* dropTableTree(string &query, int &index);
	Node* insertTree(string &query, int &index);
	Node* deleteTree(string &query, int &index);
	Node* selectTree(string &query, int &index);
	Node* attrListTree(string &query, int &index);
	Node* attrTypeTree(string &query, int &index);
	Node* insertTupleTree(string &query, int &index);
	Node* tableListTree(string &query, int &index);
	Node* selectListTree(string &query, int &index);
	Node* selectSubListTree(string &query, int &index);
	Node* valueListTree(string &query, int &index);
	Node* searchConditionTree(string &query, int &index);
	Node* boolTermTree(string &query, int &index);
	Node* boolFactorTree(string &query, int &index);
	Node* expressionTree(string &query, int &index);
	Node* termTree(string &query, int &index);


	Node* createParseTree(string &query);

	//void printParseTree(Node* t);

public:
	friend class DatabaseEngine;
	friend class ConvertHelper;
	friend class LogicPlan;
	ParseTree(string query) { root = createParseTree(query); }
	void printParseTree() const; //print all relations and their schema
	void printParseTree(ostream &out) const;
	friend ostream &operator<<(ostream &out, const ParseTree &pt);
	/*void printParseTree() {
		if (root == NULL) { cout << "There is no parse tree generated!" << endl; return; }
		else { printParseTree(root); };
		
	};*/
};


#endif
