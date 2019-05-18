#ifndef _QUERYNODE_H
#define _QUERYNODE_H

#include<string>
#include<vector>
#include "ParseTree.h"
#include"Node.h"
using namespace std;

enum Operation_Type {
	PROJECTION,SELECTION,PRODUCT,JOIN,DUP_REMOVE,SORT,SCAN
};



class QueryNode {

public:

	Operation_Type op_type;

	Node* attr;
	vector<QueryNode*> child;

	QueryNode(Operation_Type t) { op_type = t; }

    
	~QueryNode() {}

};

class LogicPlan
{
	QueryNode* root;
	QueryNode* createLogicPlan(Node *t);
    QueryNode* Optimizer();
public:
	
	LogicPlan(Node* pt) {root=createLogicPlan(pt); }
    
    Optimizer(){root=Optimizer();};
    
};

#endif