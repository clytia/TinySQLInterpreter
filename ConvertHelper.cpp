#include "ConvertHelper.h"
#include <stack>
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


	void getTableName(Node *t, vector<string> &tableName)
	{
		if (t == NULL) {
			cout << "No Node Detected!" << endl;
			return;
		}
		if (t->type == TABLE_NAME) { tableName.push_back(t->word); }
		else if (t->type == TABLE_LIST)
		{
			tableName.push_back(t->child[0]->word);
			if (t->child.size() != 1)
			{
				getTableName(t->child[2], tableName);
			}
		}
		else
		{
			cout << "Invalid Node Type!" << endl;
		}
		return;
	}

	void getAttributeName(Node *t, vector<string> &attrName)
	{
		if (t == NULL) {
			cout << "No Node Detected!" << endl;
			return;
		}
		if (t->type == ATTR_NAME) { attrName.push_back(t->word); }
		else if (t->type == ATTR_LIST)
		{
			attrName.push_back(t->child[0]->word);
			if (t->child.size() != 1)
			{
				getAttributeName(t->child[2], attrName);
			}
		}
		else
		{
			cout << "Invalid Node Type!" << endl;
		}
		return;
	}

	void getAttributeType(Node *a, vector<string> &attrName, vector<enum FIELD_TYPE> &attrType)
	{
		if (a == NULL) {
			cout << "No Node Detected!" << endl;
			return;
		}
		if (a->type == ATTR_TYPE_LIST)
		{
			attrName.push_back(a->child[0]->word);
			if (a->child[1]->word == "INT")
			{
				attrType.push_back(INT);
			}
			else
			{
				attrType.push_back(STR20);
			}

			if (a->child.size() != 2)
			{
				getAttributeType(a->child[3], attrName, attrType);
			}

		}
		else
		{
			cout << "Invalid Node Type!" << endl;
		}
		return;
	}

	void getColumnName(Node *t, vector<string> &colName)
	{
		if (t == NULL) {
			cout << "No Node Detected!" << endl;
			return;
		}
		if (t->type == COL_NAME) { colName.push_back(t->word); }
		else if (t->type == SELECT_SUBLIST)
		{
			colName.push_back(t->child[0]->word);
			if (t->child.size() != 1)
			{
				getColumnName(t->child[2], colName);
			}
		}
		else
		{
			cout << "Invalid Node Type!" << endl;
		}
		return;
	}

	int getFirstBlock(MainMemory mem) {
		int index = 0;
		cout << mem.getMemorySize() << endl;
		while (index != mem.getMemorySize()) {
			Block* block_ptr = mem.getBlock(index);
			if (!block_ptr->isEmpty()) { index++; }
			else { return index; }
		}

		Block* block_ptr = mem.getBlock(0);
		block_ptr->clear();
		return 0;
	}

	void appendTupleToRelation(Relation* relation_ptr, MainMemory& mem, int memory_block_index, Tuple& tuple) {
		Block* block_ptr;
		if (relation_ptr->getNumOfBlocks() == 0) {
			cout << "The relation is empty" << endl;
			cout << "Get the handle to the memory block " << memory_block_index << " and clear it" << endl;
			block_ptr = mem.getBlock(memory_block_index);
			block_ptr->clear(); //clear the block
			block_ptr->appendTuple(tuple); // append the tuple
			cout << "Write to the first block of the relation" << endl;
			relation_ptr->setBlock(relation_ptr->getNumOfBlocks(), memory_block_index);
		}
		else {
			cout << "Read the last block of the relation into memory block 5:" << endl;
			relation_ptr->getBlock(relation_ptr->getNumOfBlocks() - 1, memory_block_index);
			block_ptr = mem.getBlock(memory_block_index);

			if (block_ptr->isFull()) {
				cout << "(The block is full: Clear the memory block and append the tuple)" << endl;
				block_ptr->clear(); //clear the block
				block_ptr->appendTuple(tuple); // append the tuple
				cout << "Write to a new block at the end of the relation" << endl;
				relation_ptr->setBlock(relation_ptr->getNumOfBlocks(), memory_block_index); //write back to the relation
			}
			else {
				cout << "(The block is not full: Append it directly)" << endl;
				block_ptr->appendTuple(tuple); // append the tuple
				cout << "Write to the last block of the relation" << endl;
				relation_ptr->setBlock(relation_ptr->getNumOfBlocks() - 1, memory_block_index); //write back to the relation
			}
		}
	}


	bool isHigher(Node* top_op, Node* now)
	{
		if (top_op->word == now->word || now->word == ")") { return true; }
		if (top_op->type == OPERATOR)
		{
			if (now->type != OPERATOR) { return true; }
			else {
				if ((top_op->word == "*" || top_op->word == "/") && (now->word == "+" || now->word == "-")) return true;
			}
		}
		if (top_op->type == COMP_OP)
		{
			if (now->type != OPERATOR) { return true; }

		}
		if (top_op->type == LOG_OP)
		{
			if (now->type != LOG_OP) { return false; }
			else {
				if (top_op->word == "AND"&&now->word == "OR") return true;
				if (top_op->word == "NOT" && (now->word == "OR" || now->word == "AND"))return true;
			}
		}


		return false;
	}

	bool searchCondition(Tuple t, Node *n)
	{
		stack<Node*> result;
		Schema s = t.getSchema(); 
		if (n == nullptr) { return true; }
		if (n->type != SEARCH_COND) { return false; }
		
		Field f;
		Node *rt;
		Node *lt;
		Node *tmp;
		vector<string> fn = s.getFieldNames();
		for (vector<Node*>::iterator it = n->child.begin(); it != n->child.end(); ++it)
		{
			switch ((*it)->type)
			{
			case DATA:
				result.push(*it);
				break;
			case WORD:
				//check if it is a column_name
				if(find(begin(fn),end(fn), (*it)->word)== end(fn))
				{
					result.push(*it);
					break;
				}
			case COL_NAME:
			case ATTR_NAME:
				f = t.getField((*it)->word);
				if (s.getFieldType((*it)->word) == INT)
				{
					result.push(new Node(DATA, f.integer));
				}
				else
				{
					result.push(new Node(WORD, *(f.str)));
				}
				break;
			case OPERATOR:
				rt = result.top(); result.pop();
				lt = result.top(); result.pop();
				tmp = new Node(DATA, 0);
				if ((*it)->word == "+")
				{
					if (rt->type == DATA && lt->type == DATA)
					{
						tmp->data = lt->data + rt->data;
					}
				}
				else if ((*it)->word == "-") {
					if (rt->type == DATA && lt->type == DATA)
					{
						tmp->data = lt->data - rt->data;
					}
				}
				else if ((*it)->word == "*") {
					if (rt->type == DATA && lt->type == DATA)
					{
						tmp->data = lt->data * rt->data;
					}
				}
				else if ((*it)->word == "/") {
					if (rt->type == DATA && lt->type == DATA)
					{
						tmp->data = lt->data / rt->data;
					}
				}
				result.push(tmp);
				break;
			case COMP_OP:
				rt = result.top(); result.pop();
				lt = result.top(); result.pop();
				tmp = new Node(RESULT, 0);
				if ((*it)->word == ">")
				{
					if (rt->type == DATA && lt->type == DATA)
					{
						tmp->data = lt->data > rt->data ? 1 : 0;
					}
				}
				else if ((*it)->word == "<") {
					if (rt->type == DATA && lt->type == DATA)
					{
						tmp->data = lt->data < rt->data ? 1 : 0;
					}
				}
				else if ((*it)->word == "=") {
					if (rt->type == DATA && lt->type == DATA)
					{
						tmp->data = lt->data == rt->data ? 1 : 0;
					}
					else if (rt->type == WORD && lt->type == WORD)
					{
						tmp->data = lt->word == rt->word ? 1 : 0;
					}
				}
				result.push(tmp);
				break;

			case LOG_OP:
				rt = result.top(); result.pop();
				lt = result.top(); result.pop();
				tmp = new Node(RESULT, 0);
				if ((*it)->word == "AND")
				{
					if (rt->type == RESULT && lt->type == RESULT)
					{
						tmp->data = ((lt->data == 1) && (rt->data == 1)) ? 1 : 0;
					}
				}
				else if ((*it)->word == "OR") {
					if (rt->type == RESULT && lt->type == RESULT)
					{
						tmp->data = ((lt->data == 1) || (rt->data == 1)) ? 1 : 0;
					}
				}
				else if ((*it)->word == "NOT") {
					result.push(lt);
					tmp->data = rt->data == 1 ? 0 : 1;
				}
				result.push(tmp);
				break;

			default:
				break;
			}
		}
		return result.top()->data == 1;
	}
    
    bool hasAllAttribute(vector<string> attr, vector<string> field_names)
    {
        bool ret=true;
        
        for(vector<string>::iterator it= attr.begin();it!=attr.end();++it)
        {
           ret = ret &&( find(begin(field_names),end(field_names),*it)!=end(field_names));
        }
        return ret;
    }
    
    bool canNaturalJoin()
    {
        
    }

	void appendFieldToTuple(Tuple &t, Tuple t1, Tuple t2)
	{
		//int offset = t1.getNumOfFields();
		int i = 0;
		for (; i != t1.getNumOfFields(); i++)
		{
			if (t1.getSchema().getFieldType(i) == INT)
			{
				t.setField(i, t1.getField(i).integer);

			}
			else
			{
				t.setField(i, *(t1.getField(i).str));
			}
		}
		for (int j = 0; j != t2.getNumOfFields(); j++)
		{
			if (t2.getSchema().getFieldType(j) == INT)
			{
				t.setField(i + j, t2.getField(j).integer);

			}
			else
			{
				t.setField(i + j, *(t2.getField(j).str));
			}
		}


	}

	Relation* crossJoin(string table1, string table2, SchemaManager &sm, MainMemory* mem)
	{
		Relation* relation_ptr_s = sm.getRelation(table1);
		Relation *relation_ptr_l = sm.getRelation(table2);
		int size1 = relation_ptr_s->getNumOfTuples();
		int size2 = relation_ptr_l->getNumOfTuples();

		if (size1 > size2) { swap(relation_ptr_s, relation_ptr_l); swap(table1, table2); }

		Schema s1 = relation_ptr_s->getSchema();
		Schema s2 = relation_ptr_l->getSchema();


		vector<string> field_names;
		vector<enum FIELD_TYPE> field_types;

		vector<string> fn_1 = s1.getFieldNames();
		vector<string> fn_2 = s2.getFieldNames();

		for (vector<string>::iterator it = fn_1.begin(); it != fn_1.end(); ++it)
		{
			field_names.push_back(table1 + "." + (*it));
			field_types.push_back(s1.getFieldType(*it));
		}
		for (vector<string>::iterator it = fn_2.begin(); it != fn_2.end(); ++it)
		{
			field_names.push_back(table2 + "." + (*it));
			field_types.push_back(s2.getFieldType(*it));
		}

		Schema schema(field_names, field_types);

		//string relation_name = table_name[0];

		Relation* ret_ptr = sm.createRelation(table1 + 'X' + table2, schema);

		relation_ptr_s->getBlocks(0, 0, relation_ptr_s->getNumOfBlocks());



		//One pass algorithm: read large relation block by block
		for (int i = 0; i < relation_ptr_l->getNumOfBlocks(); i++)
		{
			int block_id = getFirstBlock(*mem);
			relation_ptr_l->getBlock(i, block_id);
			for (int j = 0; j < relation_ptr_s->getNumOfBlocks(); j++)
			{
				Block* block_ptr_s = mem->getBlock(j);
				Block* block_ptr_l = mem->getBlock(block_id);

				vector<Tuple> t_s_list = block_ptr_s->getTuples();
				vector<Tuple> t_l_list = block_ptr_l->getTuples();
				for (int t_off_s = 0; t_off_s != t_s_list.size(); t_off_s++)
				{

					if (t_s_list[t_off_s].isNull()) { continue; }

					for (int t_off_l = 0; t_off_l != t_l_list.size(); t_off_l++)
					{
						if (t_l_list[t_off_l].isNull()) { continue; }

						Tuple tuple = ret_ptr->createTuple();
						//tuple = t_s_list[t_off_s];
						appendFieldToTuple(tuple, t_s_list[t_off_s], t_l_list[t_off_l]);
						//appendFieldToTuple(tuple, t_l_list[t_off_l]);
						int i = getFirstBlock(*mem);
						appendTupleToRelation(ret_ptr, *mem, i, tuple);
						cout << *ret_ptr << endl;
					}
				}



			}
		}

		cout << *ret_ptr << endl;
		return ret_ptr;
	}

	vector<pair<int,int>> tableScan(Relation *r, MainMemory *mem, Node *searchTree, bool isDelete) 
	{
		vector<pair<int, int>> ret;
		for (int i = 0; i < r->getNumOfBlocks(); i++)
		{

			//int block_id = getFirstBlock(*mem);
			r->getBlock(i, 0);
			Block* block_ptr = mem->getBlock(0);


			vector<Tuple> tuples = block_ptr->getTuples();

			for (int j = 0; j != tuples.size(); j++)
			{

				if (tuples[j].isNull()) { continue; }

				if (searchCondition(tuples[j], searchTree))
				{
					if (isDelete){
						block_ptr->nullTuple(j);
						r->setBlock(i,0);
					}
					ret.push_back(make_pair(i,j));
				}

			}
		}
			return ret;
	}

};