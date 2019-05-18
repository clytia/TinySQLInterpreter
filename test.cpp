#include <iostream>
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cctype>
#include "ParseTree.h"
#include "ConvertHelper.h"
#include "DatabaseEngine.h"
#include "Node.h"
#include <stack>
#include <fstream>
#include <sstream>

using namespace std;

int main() {

	string test = "SELECT * FROM course,course2 WHERE (exam + homework) = 200";
	//string test = "INSERT INTO r (a, b) VALUES (0, 0)";
	//WHERE course.sid = course2.sid AND course.grade = \"A\" AND course2.grade = \"A\" ORDER BY course.exam

	/*string test = "CREATE TABLE course (sid INT, homework INT, project INT, exam INT, grade STR20)";
	string test2 = "INSERT INTO course(sid, homework, project, exam, grade) VALUES(1, 99, 100, 100, \"A\")";
	string test3 = "INSERT INTO course(sid, homework, project, exam, grade) VALUES(1, 100, 100, 100, \"A\")";
	string test4 = "DELETE FROM course WHERE sid = 1";
	string test5 = "SELECT * FROM course";*/
	ifstream readFile("test.txt");
	ofstream fout("test_out.txt");

	//ParseTree t(test);
	//cout << t;
	//string test;
	vector<ParseTree> result;
	MainMemory mem;
	Disk disk;
	cout << "The memory contains " << mem.getMemorySize() << " blocks" << endl;
	cout << mem << endl << endl;
	DatabaseEngine de(&mem, &disk);

	de.executer(test, false);
/*	de.executer(test2,false);
	de.executer(test3,false);
	de.executer(test4, false);*/
	//while (getline(readFile, test))
	//{
	//string testFile = "test.txt";
	//de.executer(testFile,true);

	//}







	return 0;
}
