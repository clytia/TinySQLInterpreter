#ifndef _DATABASEENGINE_H
#define _DATABASEENGINE_H

#include<string>
#include<vector>
#include <fstream>
#include <sstream>
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



class ParseTree;
class MainMemory;
class Disk;

class DatabaseEngine {
private:

	MainMemory* mem;
	Disk* disk;
	SchemaManager schema_manager;
	//vector<ParseTree> queryTree;
	
	bool dropTable(Node *t);
	bool createTable(Node *t);
	bool selectFromTable(Node *t);
	bool insertIntoTable(Node *t);
	bool deleteFromTable(Node *t);
	bool executer(vector<ParseTree> queryTree);
public:

	

	DatabaseEngine(MainMemory* mem, Disk* disk);

	//bool createTable();
	void executer(string &s, bool isFile);

};


#endif