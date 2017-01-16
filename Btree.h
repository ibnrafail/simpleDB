#pragma once
#define MYDEBUG 0
#define DEGREE 4
#define MYSIZE 80
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <io.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

const long NULLPOINTER = -1L;
enum state{error, incomplete, success, underflow, notfound, duplicate};
class Btree
{
private:
	string treeFileName;
	bool theTreeIsDeleted;
	bool createdAndOpened;
	struct Element{
		char key[MYSIZE];
		long offset;
		int counter;
	};
	struct Node{
		int amountOfElements;
		Element elements[2*DEGREE];
		long pointers[2*DEGREE + 1];
	};
	FILE *treeFile;
	Node rootNode;
	long pointerToRoot, emptyNodePtr, initial[2];
	long getNode();
	void readNodeFromFile(long offset, Node *node);
	void writeNodeToFile(long offset, Node *node);
	state insertHelper(Element, long, Element *, long*);
	state deleteHelper(char *key, long offset, bool deleteAll, vector<long> *returnOffsets);
	int binarySearch(char *key, Element *Array, int amountOfElements);
	void Read();
	void Write();
	void updateEmptyNode(long);
	void copyElements(Element *from, Element *to, int amountOfElements);
	void functionError(string);
public:
	Btree(char *treeFileName);
	~Btree();

	state insertElement(char *key, long elementOffset);
	state searchElement(char *key, vector<long> *);
	state deleteElement(char *key, bool deleteAll, vector<long> *returnOffset);
	state createOrOpenTheTree(char*);
	state clearTheTree();
	state deleteTheTree();
	state findAndUpdate(char *key, long oldElementOffset, long newElementOffset);

	Node getRootNode(){return rootNode;}
	long getPointerToRoot(){return pointerToRoot;}
	long getEmptyNodePtr(){return emptyNodePtr;}

	bool isCreatedAndOpened(){return this->createdAndOpened;}
	state printTree(long offset);
};