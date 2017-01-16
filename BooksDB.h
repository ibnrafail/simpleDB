#pragma once
#include <iostream>
#include <string>
#include "additionalFunctions.h"
#include "Btree.h"
#include <vector>
#include <cstdlib>
#include <windows.h>
using namespace std;

enum status{FAILED, SUCCESS, NOT_FOUND, alreadyExists};
class BooksDB
{
	TCHAR initialDirectory[MAX_PATH];
	FILE *dataBaseFile;
	string title;
	Btree *btree;
	class Item{
	public:
		char name[80];
		char author[80];
		char publisher[80];
		unsigned yearOfPublishing;
		unsigned amountOfPages;
		bool deleted;
		Item(char *name, int amountOfPages, char *author, char *publisher, int yearOfPublishing, bool deleted);
		Item(){}
		~Item(void){}
		void printFields(long id);
		
	};
	int checkTitle(string title);
public:
	BooksDB();
	~BooksDB();
	int openDB(char *title);
	int createDB(char *title);
	status createOrOpenDB(char *title);
	int closeDB();
	int deleteDB(char *title);
	int clearDB();
	int addItem(char *name, int amountOfPages, char *author, char *publisher, int yearOfPublishing);
	int deleteItemByName(char *name);
	int deleteItemInteractively(char *name, long id);
	int editItem(char *oldName, char *newName, int newAmountOfPages, char *newAuthor, char *newPublisher, int newYearOfPublishing, long id);
	int searchItemByName(char *name, vector<string> &ItemsFields);
	string getTitle(){return title;}
	vector<short> checkFields(char *name, int amountOfPages, char *author, char *publisher, int yearOfPublishing);
	bool IsCreatedAndOpened(){if(this->dataBaseFile != NULL && btree != NULL) return true; else return false;}
	void printErrorToConsole(int);

	void printDB();
};
