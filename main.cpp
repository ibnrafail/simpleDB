#include "BooksDB.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>

string GenerateAlphaNumStr_N(size_t n)
{
    static char characters[] =
    {
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g',
        'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '1', '2', '3', '4',
        '5', '6', '7', '8', '9', '0'
    };
    string result;
 
    for(size_t i = 0; i < n; ++i)
        result += characters[rand() % 36];
 
    return result;
}

unsigned testAddElement(unsigned amountOfElements, string &testName)
{
	unsigned end_time, work_time = 0, start_time;
	BooksDB testDB;
	testName = "ADD_ELEMENT_TEST_0";
	long long i = 0;
	while(testDB.createDB((char*)testName.c_str()) == 11)
	{
		testName = "ADD_ELEMENT_TEST_";
		testName += std::to_string((_Longlong)i++);
	}
	testDB.openDB((char*)testName.c_str());
	if(testDB.IsCreatedAndOpened())
	{
		for(unsigned i = 0; (unsigned)i < amountOfElements; i++){
			srand((unsigned)time(NULL)+i);
			string helper = GenerateAlphaNumStr_N(MYSIZE-1);
			testDB.addItem((char*)helper.c_str(), 200, "author", "publisher", i);
		}
		
		for(unsigned i = amountOfElements; (unsigned)i < amountOfElements + 100; i++){
			srand((unsigned)time(NULL)+i);
			string helper = GenerateAlphaNumStr_N(MYSIZE-1);
			start_time =  clock();
			testDB.addItem((char*)helper.c_str(), 200, "author", "publisher", i);
			end_time = clock(); 
			work_time += (end_time - start_time);
		}
		testDB.closeDB();
	}
	return work_time;
}
unsigned testDeleteElement(unsigned amountOfElements, string &testName)
{
	unsigned end_time, work_time, start_time;
	BooksDB testDB;
	testName = "DELETE_ELEMENT_TEST_0";
	long long i = 0;
	while(testDB.createDB((char*)testName.c_str()) == 11)
	{
		testName = "DELETE_ELEMENT_TEST_";
		testName += std::to_string((_Longlong)i++);
	}
	testDB.openDB((char*)testName.c_str());
	if(testDB.IsCreatedAndOpened())
	{
		vector<string> strings;
		for(unsigned i = 0; (unsigned)i < amountOfElements; i++){
			srand((unsigned)time(NULL)+i);
			string helper = GenerateAlphaNumStr_N(MYSIZE-1);
			strings.push_back(helper);
			testDB.addItem((char*)helper.c_str(), 200, "author", "publisher", i);
		}

		
		for(unsigned i = 0; (unsigned)i < 100; i++){
			start_time =  clock();
			testDB.deleteItemByName((char*)strings[i].c_str());
			end_time = clock(); 
			work_time += (end_time - start_time);
		}
		testDB.closeDB();
	}
	return work_time;
}
unsigned testFindElement(unsigned amountOfElements, string &testName)
{
	unsigned end_time, work_time, start_time;
	BooksDB testDB;
	testName = "FIND_ELEMENT_TEST_0";
	long long i = 0;
	while(testDB.createDB((char*)testName.c_str()) == 11)
	{
		testName = "FIND_ELEMENT_TEST_";
		testName += std::to_string((_Longlong)i++);
	}
	testDB.openDB((char*)testName.c_str());
	if(testDB.IsCreatedAndOpened())
	{
		vector<string> strings, findings;
		for(unsigned  i = 0; (unsigned)i < amountOfElements; i++){
			srand((unsigned)time(NULL)+i);
			string helper = GenerateAlphaNumStr_N(MYSIZE-1);
			strings.push_back(helper);
			testDB.addItem((char*)helper.c_str(), 200, "author", "publisher", i);
		}
		
		for(unsigned i = 0; (unsigned)i < 100; i++){
			start_time =  clock();
			testDB.searchItemByName((char*)strings[i].c_str(), findings);
			end_time = clock(); 
			work_time += (end_time - start_time);
		}
		testDB.closeDB();
	}
	return work_time;
}

vector<string> in(string str)
{
	vector<string> result;
	string s;
	int begin = 0;
	for(int i = 0; (unsigned)i < str.size(); i++)
	{
		if(str[i] == '|' || str[i] == ',' || str[i] == ';')
		{
			s.clear();
			s = str.substr(begin, i - begin);
			result.push_back(s);
			begin = i + 1;
		}
	}
	s = str.substr(begin, str.size() - 1);
	result.push_back(s);
	return result;
}

state testBtreeInsert(unsigned amountOfElements, string testName)
{
	unsigned int end_time, work_time, start_time;
	Btree tree((char*)testName.c_str());
	string helper;
	for(int i = 0; (unsigned)i < amountOfElements; i++)
	{
		srand((unsigned)time(NULL)+i);
		helper = GenerateAlphaNumStr_N(MYSIZE-1);
		tree.insertElement((char*)helper.c_str(), i);
	}
	
	for(int i = amountOfElements; (unsigned)i < amountOfElements + 1000; i++)
	{
		srand((unsigned)time(NULL)+i);
		helper = GenerateAlphaNumStr_N(MYSIZE-1);
		start_time =  clock();
		tree.insertElement((char*)helper.c_str(), i);
		end_time = clock(); 
		work_time += (end_time - start_time);
	}
	printf("%u\n", work_time);
	return success;
}

void menu()
{
	string input;
	BooksDB db;
	int result;
	while(1)
	{
		system("cls");
		cout << "Books data base management system.\nUsage:\n";
		cout << "1. Create data base;\n2. Open data base;\n3. Delete data base;\n4. Exit.\n";
		cout << "Enter the command (create, open, delete, exit):\n";
		getline(cin, input);

		if(_strcmpi(input.c_str(), "create") == 0 || _strcmpi(input.c_str(), "1") == 0)
		{
			system("cls");
			cout << "Enter the data base name:" << endl;
			getline(cin, input);
			result = db.createDB((char*)input.c_str());
			if(result == 0)
				cout << "The data base with the name <" << input << "> has been created." << endl;
			else
				db.printErrorToConsole(result);
			system("pause");
			system("cls");
		}
		else if(_strcmpi(input.c_str(), "open") == 0 || _strcmpi(input.c_str(), "2") == 0)
		{
			system("cls");
			cout << "Enter the data base name:" << endl;
			getline(cin, input);
			result = db.openDB((char*)input.c_str());
			if(result == 0)
			{
				cout << "The data base with the name <" << input << "> has been opened." << endl;
				system("pause");
				system("cls");
				while(1)
				{
					system("cls");
					cout << "1. Insert element;\n2. Delete element;\n3. Search element;\n4. Edit element;\n5. Delete the data base;\n6. Clear the data base\n7. Close the data base.\n";
					cout << "Enter the command (insert, delete, search, deleteDB, clearDB, closeDB):\n";
					getline(cin, input);
					if(_strcmpi(input.c_str(), "insert") == 0 || _strcmpi(input.c_str(), "1") == 0)
					{
						system("cls");
						cout << "Enter the fields.\nFormat: name|author|publisher|amount of pages|year of publishing.\n";
						getline(cin, input);
						vector<string> res = in(input);
						if(res.size() < 5)
							cout << "All fields are compulsory.";
						else
						{
							string name, author, publisher;
							unsigned amountOfPages, yearOfPublishing;
							amountOfPages = atoi(res[3].c_str());
							yearOfPublishing = atoi(res[4].c_str());
							vector<short> value = db.checkFields((char*)res[0].c_str(), amountOfPages, (char*)res[1].c_str(), (char*)res[2].c_str(), yearOfPublishing);
							if(value.empty())
							{
								result = db.addItem((char*)res[0].c_str(), amountOfPages, (char*)res[1].c_str(), (char*)res[2].c_str(), yearOfPublishing);
								if(result == 0)
								{
									cout << "The element has been inserted." << endl;
									system("pause");
								}
								else
								{
									db.printErrorToConsole(result);
									system("pause");
								}
							}
							else
							{
								for(int i = 0; (unsigned)i < value.size(); i++)
								{
									db.printErrorToConsole(value[i]);
								}
								system("pause");
							}
						}
					}
					else if(_strcmpi(input.c_str(), "delete") == 0 || _strcmpi(input.c_str(), "2") == 0)
					{
						system("cls");
						cout << "Enter the book name.\n";
						getline(cin, input);
						if(input.size() > 0)
						{
							vector<string> res;
							int value = db.searchItemByName((char*)input.c_str(), res);
							if(value == 0)
							{
								string helper = input;
								cout << "Found elements:\n";
								for(int i = 0; (unsigned)i < res.size(); i++)
								{
									cout << res[i] << endl;
								}
								cout << "Enter the id:\n";
								getline(cin, input);
								if(db.deleteItemInteractively((char*)helper.c_str(), atoi(input.c_str())) == 0)
									cout << "The element has been deleted." << endl;
								system("cls");
							}
							else
							{
								db.printErrorToConsole(value);
								system("pause");
							}
						}
						else
						{
							cout << "Empty input." << endl;
							system("pause");
						}
					}
					else if(_strcmpi(input.c_str(), "search") == 0 || _strcmpi(input.c_str(), "3") == 0)
					{
						system("cls");
						cout << "Enter the book name.\n";
						getline(cin, input);
						if(input.size() > 0)
						{
							vector<string> res;
							int value = db.searchItemByName((char*)input.c_str(), res);
							if(value == 0)
							{
								string helper = input;
								cout << "Found elements:\n";
								for(int i = 0; (unsigned)i < res.size(); i++)
								{
									cout << res[i] << endl;
								}
								system("pause");
							}
							else
							{
								db.printErrorToConsole(value);
								system("pause");
							}
						}
						else
						{
							cout << "Empty input." << endl;
							system("pause");
						}
					}
					else if(_strcmpi(input.c_str(), "edit") == 0 || _strcmpi(input.c_str(), "4") == 0)
					{
						system("cls");

					}
					else if(_strcmpi(input.c_str(), "clearDB") == 0 || _strcmpi(input.c_str(), "6") == 0)
					{
						system("cls");
						int value = db.clearDB();
						if(value == 0)
						{
							cout << "The data base has been cleared." << endl;
							system("pause");
						}
						else
						{
							db.printErrorToConsole(value);
							system("pause");
						}
					}
					else if(_strcmpi(input.c_str(), "closeDB") == 0 || _strcmpi(input.c_str(), "7") == 0)
					{
						system("cls");
						int value = db.closeDB();
						if(value == 0)
						{
							cout << "The data base has been closed." << endl;
							system("pause");
						}
						else
						{
							db.printErrorToConsole(value);
							system("pause");
						}
						break;
					}
					else
					{
						cout << "Input error." << endl;
						system("pause");
					}
				}
			}
			else
				db.printErrorToConsole(result);

		}
		else if(_strcmpi(input.c_str(), "delete") == 0 || _strcmpi(input.c_str(), "3") == 0)
		{
			system("cls");
			cout << "Enter the data base name:" << endl;
			getline(cin, input);
			if(db.IsCreatedAndOpened() == false)
			{
				int value = db.deleteDB((char*)input.c_str());
				if(value == 0)
				{
					cout << "The data base <" << input << "> was deleted." << endl;
					system("pause");
				}
				else
				{
					db.printErrorToConsole(value);
					system("pause");
				}
			}
		}
		else if(_strcmpi(input.c_str(), "exit") == 0 || _strcmpi(input.c_str(), "4") == 0)
		{
			system("cls");
			cout << "bye-bye..." << endl;
			break;
		}
		else
		{
			system("cls");
			cout << "Input error. Try again." << endl;
			system("pause");
			system("cls");
		}
	}
}

void insertTests()
{
	unsigned amountOfElements1 = 1000, amountOfElements2 = 2000, amountOfElements3 = 3000;
	string testName1, testName2, testName3;
	unsigned res1 = testAddElement(amountOfElements1, testName1);
	unsigned res2 = testAddElement(amountOfElements2, testName2);
	unsigned res3 = testAddElement(amountOfElements3, testName3);
	cout << "TEST NAME *************** AMOUNT OF ELEMENTS ******** TIME IN (MS)**********\n" << endl;
	
	cout << setw(15) << testName1 << "            " << amountOfElements1 << "                          " << res1 << endl;
	cout << setw(15) << testName2 << "            " << amountOfElements2 << "                          " << res2 << endl;
	cout << setw(15) << testName3 << "            " << amountOfElements3 << "                          " << res3 << endl;
}

void deleteTests()
{
	unsigned amountOfElements1 = 1000, amountOfElements2 = 2000, amountOfElements3 = 3000;
	string testName1, testName2, testName3;
	unsigned res1 = testDeleteElement(amountOfElements1, testName1);
	unsigned res2 = testDeleteElement(amountOfElements2, testName2);
	unsigned res3 = testDeleteElement(amountOfElements3, testName3);
	cout << "TEST NAME *************** AMOUNT OF ELEMENTS ******** TIME IN (MS)**********\n" << endl;
	
	cout << setw(15) << testName1 << "            " << amountOfElements1 << "                          " << res1 << endl;
	cout << setw(15) << testName2 << "            " << amountOfElements2 << "                          " << res2 << endl;
	cout << setw(15) << testName3 << "            " << amountOfElements3 << "                          " << res3 << endl;
}

void searchTests()
{
	
	unsigned amountOfElements1 = 1000, amountOfElements2 = 2000, amountOfElements3 = 3000;
	string testName1, testName2, testName3;
	unsigned res1 = testFindElement(amountOfElements1, testName1);
	unsigned res2 = testFindElement(amountOfElements2, testName2);
	unsigned res3 = testFindElement(amountOfElements3, testName3);
	cout << "TEST NAME *************** AMOUNT OF ELEMENTS ******** TIME IN (MS)**********\n" << endl;
	
	cout << setw(15) << testName1 << "            " << amountOfElements1 << "                          " << res1 << endl;
	cout << setw(15) << testName2 << "            " << amountOfElements2 << "                          " << res2 << endl;
	cout << setw(15) << testName3 << "            " << amountOfElements3 << "                          " << res3 << endl;
}

int main()
{
	insertTests();
	//menu();
	getchar();
	return 0;
}