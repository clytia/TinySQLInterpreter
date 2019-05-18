#ifndef _CONVERTHELPER_H
#define _CONVERTHELPER_H

#include<string>
#include<vector>
#include<iostream>
#include<map>
#include"Node.h"


#include "..\StorageManager\Block.h"
#include "..\StorageManager\Config.h"
#include "..\StorageManager\Disk.h"
#include "..\StorageManager\Field.h"
#include "..\StorageManager\MainMemory.h"
#include "..\StorageManager\Relation.h"
#include "..\StorageManager\Schema.h"
#include "..\StorageManager\SchemaManager.h"
#include "..\StorageManager\Tuple.h"






namespace Utility
{


	void getTableName(Node *t, vector<string> &tableName);

	void getAttributeName(Node *a, vector<string> &attrName);

	void getAttributeType(Node *a, vector<string> &attrName, vector<enum FIELD_TYPE> &attrType);

	int getFirstBlock(MainMemory mem);

	void appendTupleToRelation(Relation* relation_ptr, MainMemory& mem, int memory_block_index, Tuple& tuple);

	void getColumnName(Node *t, vector<string> &colName);

	bool isHigher(Node* top_op, Node* now);

	bool searchCondition(Tuple t, Node *n);
	//Operations
	Relation* crossJoin(string table1, string table2, SchemaManager &sm, MainMemory* mem);

    Relation* naturalJoin(string table1,string table2);
    
	vector<pair<int, int>> tableScan(Relation *r, MainMemory *mem, Node *searchTree,bool isDelete);
    
    bool hasAllAttrbute(vector<string> attr, vector<string> field_names);
    
    bool canNaturalJoin();

	

};


#endif