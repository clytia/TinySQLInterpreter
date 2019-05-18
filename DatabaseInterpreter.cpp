#include <iostream>
#include <ctime>
#include <climits>
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include<map>
#include <iterator>
#include <cctype>
#include "ParseTree.h"
#include "Node.h"
#include "ConvertHelper.h"
#include "DatabaseEngine.h"
#include <stack>
#include <queue>
#include<fstream>
#include<iterator>
#include<cstdlib>
#include<string>
#include<ctime>

#include <sstream>
#include "QueryNode.h"
#include "..\StorageManager\Block.h"
#include "..\StorageManager\Config.h"
#include "..\StorageManager\Disk.h"
#include "..\StorageManager\Field.h"
#include "..\StorageManager\MainMemory.h"
#include "..\StorageManager\Relation.h"
#include "..\StorageManager\Schema.h"
#include "..\StorageManager\SchemaManager.h"
#include "..\StorageManager\Tuple.h"


using namespace std;

using namespace Utility;

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
		return DATA;
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
		if (find(begin(keyword), end(keyword), tmp) != end(keyword)) { 
			
			word = tmp; ret = KEYWORD;
			if (word == "OR" || word == "AND" || word == "NOT") 
			{
				ret = LOG_OP;
			}
	
				

		}
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
		case DATA:
			root->child.push_back(new Node(DATA, data));
			break;
		/*case KEYWORD:
			root->child.push_back(new Node(KEYWORD, word));
			break;*/
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


//this is for direct select: need to integrate with logic plan tree
Node* ParseTree::searchConditionTree(string &query, int &index)
{
	Node *root = new Node(SEARCH_COND);
	Type tokenType;
	int data;
	string word;
	stack<Node*> st;
	int start = index;

	//create a postfix expression tree
	while (index != query.size() && word != "WHERE" && word != "ORDER")
	{
		start = index;
		tokenType = Scanner(query, index, word, data);
		if (tokenType != LOG_OP && tokenType != OPERATOR && tokenType != COMP_OP)
		{
			switch (tokenType)
			{
			case COL_NAME:
				root->child.push_back(new Node(COL_NAME, word));
				break;
			case WORD:
				root->child.push_back(new Node(WORD, word));
				break;
			case DATA:
				root->child.push_back(new Node(DATA, data));
				break;
			default:
				break;
			}
		
		}
		else
		{
			Node* tmp = new Node(tokenType, word);
			if (st.empty())
				st.push(tmp);
			else if (isHigher(st.top(), tmp))
			{
				if (tmp->word != ")") 
				{
					do
					{
						root->child.push_back(st.top());
						st.pop();
					} while ((!st.empty()) && (isHigher(st.top(), tmp)));

					st.push(tmp);
				}
				else 
				{
					while ((!st.empty()) && (st.top()->word != "("))
					{
						root->child.push_back(st.top());
						st.pop();
					}
					if ((!st.empty()) && (st.top()->word == "("))
						st.pop();
				}
			}
			else if (!isHigher(st.top(), tmp))
			{
				st.push(tmp);
			}
		}

	}

	while (!st.empty())
	{
		root->child.push_back(st.top());
		st.pop();
	}
	if (word == "WHERE" || word == "ORDER") 
	{
		index = start;
	}
	/*while (index != query.size()) {
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
		case DATA:
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

	}*/
	return root;
}
/*
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
		case DATA:
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
	//Node *root = new Node(TERM);
	Type tokenType;
	int data;
	string word;
	int start = index;
		tokenType = Scanner(query, index, word, data);

		switch (tokenType) {
		
		default:
			index = start;
			return nullptr;
		}

	
	return nullptr;

}*/
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
			if (tmp->type == DATA) { out << tmp->data << " "; }
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


QueryNode* LogicPlan::createLogicPlan(Node *t)
{
	if (t->type != SELECT_SMT) { return nullptr; }
	QueryNode *ret = new QueryNode(PROJECTION);
	QueryNode *root = ret;
	vector<string> table_name;
	bool hasDinstinct = false;
	bool selectAll = false;
	bool hasWhere = false;
	bool hasOrderBy = false;

	
	hasDinstinct = t->child[1]->word == "DINSTINCT";
	//todo:logic plan
	if (hasDinstinct)
	{
		//selectAll = t->child[2]->child[0]->word == "*";
		getTableName(t->child[4], table_name);
		ret->attr = t->child[2];
		if (t->child.size() > 5)
		{
			hasWhere = t->child[5]->word == "WHERE";
			hasOrderBy = t->child[5]->word == "ORDER";
		}
		if (t->child.size() > 7)
		{
			hasOrderBy = true;
		}
		if (hasWhere) 
		{
			QueryNode *tmp = new QueryNode(SELECTION);
			tmp->attr = t->child[6];
			ret->child.push_back(tmp);
			ret = ret->child[0];
		}
		//ret->child.push_back(new QueryNode(PRODUCT));
		//ret = ret->child[0];
	}
	else
	{
		ret->attr = t->child[1];
		selectAll = t->child[1]->child[0]->word == "*";
		getTableName(t->child[3], table_name);
		if (t->child.size() > 4)
		{
			hasWhere = t->child[4]->word == "WHERE";
			hasOrderBy = t->child[4]->word == "ORDER";
		}
		if (t->child.size() > 6)
		{
			hasOrderBy = true;
		}
		if (hasWhere)
		{
			QueryNode *tmp = new QueryNode(SELECTION);
			tmp->attr = t->child[5];
			ret->child.push_back(tmp);
			ret = ret->child[0];
		}
		//ret->child.push_back(new QueryNode(PRODUCT));
		//ret = ret->child[0];


	}

    
    if(table_name.size()==1)
    {
        ret->child.push_back(new QueryNode(SCAN));
        ret = ret->child[0];
        ret->attr = new Node(TABLE_NAME,table_name[0]);
    }
    else{
        ret->child.push_back(new QueryNode(PRODUCT));
        ret = ret->child[0];
        
        for (int i = 0; i != table_name.size(); i++)
        {
            QueryNode *tmp = new QueryNode(SCAN);
            tmp->attr = new Node(TABLE_NAME,table_name[i]);
            ret->child.push_back(tmp);
        }
    }
   
	
	return root;
}


QueryNode* LogicPlan::Optimizer(QueryNode *selectionNode,SchemaManager &sm,vector<String> table_name)
{
    QueryNode* ret = root;
    //Only optimized selection in search condition at this moment
    
    if(selectionNode->Operation_Type!=SELECTION)
    {
        return ret;
    }
    map<string,vector<string>> table_attr_name;
    for (int i = 0; i != table_name.size(); i++)
    {
        Relation *relation_ptr=sm.getRelation(table_name[i]);
        vector<string> attr_name = relation_ptr->getSchema->getFieldNames();
        table_attr_name.push(table_name[i],attr_name);
    }
    
    
    Node *searchTree = selectionNode->attr;
    //if
    if(searchTree->child.back()->word=="OR")
    {
        
    }
    
    
    
    
    
    
    
    return ret;
}


//DatabaseEngine functions

DatabaseEngine::DatabaseEngine(MainMemory* mem, Disk* disk):schema_manager(mem,disk) {
	this->mem = mem;
	this->disk = disk;
	//this->queryTree.push_back(ParseTree(s));
	//this->schema_manager = SchemaManager(&mem, &disk);
}

void DatabaseEngine::executer(string& s, bool isFile) {
	if (isFile) {
		ifstream readFile(s);
		vector<ParseTree> queryTree;
		string query_s;
		while (getline(readFile, query_s))
		{
			
			queryTree.push_back(query_s);
		}
		cout << (executer(queryTree) ? "True" : "False") << endl;
		return;
	}
	else {
		vector<ParseTree> queryTree;
		queryTree.push_back(s);
		cout << (executer(queryTree) ? "True" : "False") << endl;
		return;
	}
};

bool DatabaseEngine::executer(vector<ParseTree> qt)
{

	bool result=true;
	for (vector<ParseTree>::iterator it = qt.begin(); it != qt.end(); ++it)
	{
		switch ((*it).root->type)
		{
		case CREATE_TABLE_SMT:
			result = result && createTable((*it).root);
			break;
		case DROP_TABLE_SMT:
			result = result&& dropTable((*it).root);
			break;
		case SELECT_SMT:
			result = result&& selectFromTable((*it).root);
			break;
		case INSERT_SMT:
			result = result&& insertIntoTable((*it).root);
			break;
		case DELETE_SMT:
			result = result&& deleteFromTable((*it).root);
			break;
		default:
			break;
		} 

	}
	return result;
}

bool DatabaseEngine::createTable(Node *t) 
{
	
	cout << "Creating table" << endl;
	vector<string> field_names;
	vector<enum FIELD_TYPE> field_types;
	vector<string> table_name;
	
	getTableName(t->child[2],table_name);
	getAttributeType(t->child[4], field_names,field_types);

	Schema schema(field_names, field_types);

	//string relation_name = table_name[0];
	
	Relation* relation_ptr = schema_manager.createRelation(table_name[0], schema);

	cout << schema_manager << endl;
	cout << (*relation_ptr) << endl;
	return schema_manager.relationExists(table_name[0]);

}

bool DatabaseEngine::dropTable(Node *t) 
{
	cout << "Dropping table" << endl;
	vector<string> table_name;
	
	getTableName(t->child[2], table_name);


	//string relation_name = table_name[0];

	
	bool ret= schema_manager.deleteRelation(table_name[0]);
	cout << schema_manager << endl;
	return ret;

}

//todo: where condition valuation
bool DatabaseEngine::insertIntoTable(Node *t)
{
	cout << "Inserting into table" << endl;
	vector<string> table_name;
	vector<string> field_names;
	vector<enum FIELD_TYPE> field_types;
	Node* valueList;
	int index=0;
	getTableName(t->child[2], table_name);

	Relation* relation_ptr= schema_manager.getRelation(table_name[0]);
	Schema schema = relation_ptr->getSchema();
	Tuple tuple = relation_ptr->createTuple();
	getAttributeName(t->child[4], field_names); 

	if (t->child[6]->type == SELECT_SMT) {  }
	else {
		valueList= t->child[6]->child[2];
		while (valueList->child.size() != 1) 
		{
			//if(index==field_names.size()){cout<<""}
			if (!schema.fieldNameExists(field_names[index])) { cout << "No such field name in schema!" << endl; return false; }
			if (valueList->child[0]->type == DATA && schema.getFieldType(field_names[index]) == INT) 
			{
				tuple.setField(field_names[index], valueList->child[0]->data);
			}
			else if (valueList->child[0]->type == WORD && schema.getFieldType(field_names[index]) == STR20) 
			{
				tuple.setField(field_names[index], valueList->child[0]->word);
			}
			else 
			{
				cout << "Wrong tuple value type is found!" << endl;
				return false;
			}
			valueList = valueList->child[2];
			index++;
			
		}
		if (!schema.fieldNameExists(field_names[index])) { cout << "No such field name in schema!" << endl; return false; }
		if (valueList->child[0]->type == DATA && schema.getFieldType(field_names[index]) == INT)
		{
			tuple.setField(field_names[index], valueList->child[0]->data);
		}
		else if (valueList->child[0]->type == WORD && schema.getFieldType(field_names[index]) == STR20)
		{
			tuple.setField(field_names[index], valueList->child[0]->word);
		}
		else
		{
			cout << "Wrong tuple value type is found!" << endl;
			return false;
		}

		
		
	}
	int i = getFirstBlock(*mem);
	appendTupleToRelation(relation_ptr, *mem, i, tuple);
	cout << "Now the memory contains: " << endl;
	cout << *mem << endl;
	cout << "Now the relation contains: " << endl;
	cout << *relation_ptr << endl << endl;

	return true;


	



	//return schema_manager.deleteRelation(relation_name);

}

bool DatabaseEngine::deleteFromTable(Node *t)
{
	cout << "Deleting from table" << endl;
	vector<string> table_name;
	vector<pair<int, int>> ret;

	getTableName(t->child[2], table_name);

	Relation* relation_ptr = schema_manager.getRelation(table_name[0]);
	//no where condition
	if (t->child.size() == 3) 
	{
		//return schema_manager.deleteRelation(relation_name);
		ret=tableScan(relation_ptr, this->mem,nullptr, true);
		cout << *relation_ptr << endl;
	}
	else 
	{
		ret=tableScan(relation_ptr,this->mem,t->child[4],true);
		cout << *relation_ptr << endl;
	}


	return true;
	

}

bool DatabaseEngine::selectFromTable(Node *t)
{
	cout << "Selecting from table" << endl;
	LogicPlan lp(t);
	vector<string> table_name;
	vector<string> column_name;

	bool hasDinstinct = false;
	bool selectAll = false;
	bool hasWhere = false;
	bool hasOrderBy = false;


	hasDinstinct = t->child[1]->word == "DINSTINCT";
	if (hasDinstinct)
	{
		selectAll = t->child[2]->child[0]->word == "*";
		if (!selectAll) { getColumnName(t->child[2]->child[0], column_name); }
		getTableName(t->child[4], table_name);
		if (t->child.size() > 5)
		{
			hasWhere = t->child[5]->word == "WHERE";
			hasOrderBy = t->child[5]->word == "ORDER";
		}
		if (t->child.size() > 7)
		{
			hasOrderBy = true;
		}
	}
	else
	{
		selectAll = t->child[1]->child[0]->word == "*";
		if (!selectAll) { getColumnName(t->child[1]->child[0], column_name); }
		getTableName(t->child[3], table_name);
		if (t->child.size() > 4)
		{
			hasWhere = t->child[4]->word == "WHERE";
			hasOrderBy = t->child[4]->word == "ORDER";
		}
		if (t->child.size() > 6)
		{
			hasOrderBy = true;
		}


	}

	Relation* relation_ptr = schema_manager.getRelation(table_name[0]);
	if (table_name.size() == 1) { relation_ptr = schema_manager.getRelation(table_name[0]); }
	else
	{
		for (vector<string>::iterator it = table_name.begin() + 1; it != table_name.end(); it++)
		{
			relation_ptr = crossJoin(relation_ptr->getRelationName(), *it, this->schema_manager, this->mem);
		}


	}
	//only select all from one table
	if (selectAll)
	{
		cout << *relation_ptr << endl;
	}





	return true;
}


	//return schema_manager.deleteRelation(relation_name);

