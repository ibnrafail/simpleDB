#include "Btree.h"

Btree::Btree(char *treeFileName)
{ 
	theTreeIsDeleted = false;
	pointerToRoot = NULLPOINTER;
	emptyNodePtr = NULLPOINTER;
	fopen_s(&treeFile, treeFileName, "r+b");
	if (treeFile == NULL) 
	{ 
		if(fopen_s(&treeFile, treeFileName, "w+b") == 0)
		{
			this->treeFileName = treeFileName;
			Write();
			this->createdAndOpened = true;
		}
		else
			this->createdAndOpened = false;
	}
	else 
	{
		this->treeFileName = treeFileName;
		this->createdAndOpened = true;
		Read();
	}
}

Btree::~Btree()
{
	if(theTreeIsDeleted == false && this->createdAndOpened == true)
	{
		if(this->pointerToRoot == NULLPOINTER)
		{
			emptyNodePtr = NULLPOINTER;
			FILE *file;
			fopen_s(&file, "treeTemp", "w+b");
			fclose(file);
			fclose(treeFile);

			remove(this->treeFileName.c_str());
			rename("treeTemp", this->treeFileName.c_str());
			fopen_s(&treeFile, this->treeFileName.c_str(), "w+b");
		}
		Write();
		fclose(treeFile);
	}
}

void Btree::functionError(string function)
{
	string error = "In function " + function;
	ferror(treeFile);
	cout << error << endl;
	exit(1);
}

void Btree::Read()
{
	if (fseek(treeFile, 0, SEEK_SET))
		functionError("Read()");
	if (fread(initial, sizeof(long), 2, treeFile) == 0)
		functionError("Read()");
	readNodeFromFile(initial[0], &rootNode);
	pointerToRoot = initial[0];
	emptyNodePtr = initial[1];
}

void Btree::Write()
{
	initial[0] = pointerToRoot;
	initial[1] = emptyNodePtr;
	if (fseek(treeFile, 0, SEEK_SET))
		functionError("Write()");
	if (fwrite(initial, sizeof(long), 2, treeFile) == 0)
		functionError("Write()");
	fflush(treeFile);
	if (pointerToRoot != NULLPOINTER) 
		writeNodeToFile(pointerToRoot, &rootNode);
}

long Btree::getNode()
{
	long offset;
	Node node;

	if (emptyNodePtr == NULLPOINTER) 
	{
		if (fseek(treeFile, 0L, SEEK_END)) {
			functionError("In function getNode()");
		}
		offset = ftell(treeFile);
		//writeNodeToFile(offset, &node);  
	}
	else
	{
		offset = emptyNodePtr;
		readNodeFromFile(offset, &node);
		emptyNodePtr = node.pointers[0];
	}
	return offset;
}

void Btree::readNodeFromFile(long offset, Node *node)
{
	if (offset == pointerToRoot)
	{
		*node = rootNode;
		return;
	}
	
	if (fseek(treeFile, offset, SEEK_SET))
		functionError("readNodeFromFile()");
	if (fread(node, sizeof(Node), 1, treeFile) == 0)
		functionError("readNodeFromFile()");
}

void Btree::writeNodeToFile(long offset, Node *node)
{
	long currentOffset = ftell(this->treeFile);
	if (offset == pointerToRoot)
		rootNode = *node;
	if(currentOffset != offset)
	{
		if (fseek(treeFile, offset, SEEK_SET))
			functionError("writeNodeToFile()");
	}
	if (fwrite(node, sizeof(Node), 1, treeFile) == 0) 
		functionError("writeNodeToFile()");
	fflush(treeFile);
}

state Btree::insertElement(char *key, long elementOffset)
{
	if(theTreeIsDeleted == false && this->createdAndOpened == true)
	{
		long newOffset, newPointerToRoot;
		Element newElement, element;
		element.counter = 1;
		strcpy_s(element.key, MYSIZE, key);
		element.offset = elementOffset;
		if(insertHelper(element, pointerToRoot, &newElement, &newOffset) == incomplete)
		{
			newPointerToRoot = getNode();
			rootNode.amountOfElements = 1;
			rootNode.elements[0] = newElement;

			rootNode.pointers[0] = pointerToRoot;
			rootNode.pointers[1] = newOffset;
			pointerToRoot = newPointerToRoot;
			writeNodeToFile(pointerToRoot, &rootNode);
		}
	}
	else return error;
	return success;
}

state Btree::insertHelper(Element element, long offset, Element *elementHelper, long *offsetHelper)
{
	long newOffset;
	Element newElement;
	int position, i;
	Node node, newNode;

	if(offset == NULLPOINTER)
	{
		*offsetHelper = offset;
		*elementHelper = element;
		return incomplete;
	}

	readNodeFromFile(offset, &node);
	position = binarySearch(element.key, node.elements, node.amountOfElements);

	if((position < node.amountOfElements) && (strcmp(element.key, node.elements[position].key) == 0))
	{
		FILE *fileHelper;
		fopen_s(&fileHelper, element.key, "a+b");
		if (treeFile == NULL) { fopen_s(&fileHelper, element.key, "w+b");}

		fwrite(&element.offset, sizeof(long), 1, fileHelper);
		node.elements[position].counter++;
		writeNodeToFile(offset, &node);

		fflush(fileHelper);
		fclose(fileHelper);
		return success;
	}

	if (insertHelper(element, node.pointers[position], &newElement, &newOffset) != incomplete)
		return success;

	if (node.amountOfElements < (2 * DEGREE)){
		position = binarySearch(newElement.key, node.elements, node.amountOfElements);
		for (i = node.amountOfElements; i > position; i--){
			node.elements[i] = node.elements[i - 1];
			node.pointers[i + 1] = node.pointers[i];
		}

		node.elements[position] = newElement;
		node.pointers[position + 1] = newOffset;
		node.amountOfElements++;
		writeNodeToFile(offset, &node);
		return success;
	}

	if (position == (2 * DEGREE))
	{
		newNode.elements[DEGREE-1] = newElement;
		newNode.pointers[DEGREE] = newOffset;
	}
	else
	{
		newNode.elements[DEGREE-1] = node.elements[2 * DEGREE - 1];
		newNode.pointers[DEGREE] = node.pointers[2 * DEGREE];
		for (i = 2 * DEGREE - 1; i > position; i--){
			node.elements[i] = node.elements[i - 1];
			node.pointers[i + 1] = node.pointers[i];
		}
		node.elements[position] = newElement;
		node.pointers[position + 1] = newOffset;
	}
	*elementHelper = node.elements[DEGREE];
	node.amountOfElements = DEGREE;


	*offsetHelper = getNode();

	newNode.amountOfElements = DEGREE;
	for (i = 0; i < (DEGREE - 1); i++){
		newNode.elements[i] = node.elements[i + DEGREE + 1];
		newNode.pointers[i] = node.pointers[i + DEGREE + 1];
	}
	newNode.pointers[DEGREE-1] = node.pointers[2 * DEGREE];
	writeNodeToFile(offset, &node);
	writeNodeToFile(*offsetHelper, &newNode);
	return incomplete;
}

state Btree::searchElement(char *key, vector<long> *returnOffsets)
{
	if(theTreeIsDeleted == false && this->createdAndOpened == true)
	{
		int position, amountOfElements;
		Element Array[2*DEGREE];
		Node node;
		long ptrToRoot = pointerToRoot;

		if(MYDEBUG)
			cout << "Path:" << endl;

		while(ptrToRoot != NULLPOINTER)
		{
			readNodeFromFile(ptrToRoot, &node);

			amountOfElements = node.amountOfElements;
			copyElements(node.elements, Array, amountOfElements);

			if(MYDEBUG)
			{
				for (int j = 0; j < amountOfElements; j++)
					cout << "  " << Array[j].key;
				cout << endl;
			}

			position = binarySearch(key, Array, amountOfElements);

			if((position < amountOfElements) && (strcmp(key, Array[position].key) == 0))
			{
				/**/
				int count;
				if(MYDEBUG)
					cout << "Found in position" << position + 1 << "of node:  ";
				readNodeFromFile(ptrToRoot, &node);

				returnOffsets->push_back(node.elements[position].offset);
				count = node.elements[position].counter;

				if(count > 1)
				{
					FILE *fileHelper;
					fopen_s(&fileHelper, node.elements[position].key, "rb");
					if(fileHelper != NULL)
					{
						long k = 0;
						while(fread(&k, sizeof(long), 1, fileHelper) >= 1)
						{
							returnOffsets->push_back(k);
						}
						fclose(fileHelper);
					}
				}
				if(MYDEBUG)
				{
					for (int i = 0; i < node.amountOfElements; i++)
						cout << "  " << node.elements[i].key;
					cout << endl;
				}

				return success;
			}
			ptrToRoot = node.pointers[position];
		}
		return notfound;
	}
	else return error;
} 

int Btree::binarySearch(char *key, Element *Array, int amountOfElements)
{
	int median, start = 0, end = amountOfElements - 1;
	if(strcmp(key, Array[0].key) <= 0) return 0;
	if(strcmp(key, Array[amountOfElements - 1].key) > 0) return amountOfElements;

	while(end - start > 1)
	{
		median = (start + end) / 2;
		if(strcmp(key, Array[median].key) <= 0) end = median;
		else start = median;
	}
	return end;
}

state Btree::deleteElement(char *key, bool deleteAll, vector<long> *returnOffset)
{
	if(theTreeIsDeleted == false && this->createdAndOpened == true)
	{
		state value;
		long newPointerToRoot;
		value = deleteHelper(key, pointerToRoot, deleteAll, returnOffset);
		if (value == underflow){
			newPointerToRoot = rootNode.pointers[0];
			updateEmptyNode(pointerToRoot);
			if (newPointerToRoot != NULLPOINTER)
				readNodeFromFile(newPointerToRoot, &rootNode);
			pointerToRoot = newPointerToRoot;
			return success;
		}
		return value;
	}
	else return error;
}

state Btree::deleteHelper(char *key, long offset, bool deleteAll, vector<long> *returnOffset)
{
	int position, i;
	bool flag = false;
	long pointerToLeft, pointerToRight, offsetHelper;
	Element *elements, *elementsHelper;
	long *pointers;
	int *amountOfElements;
	Element *leftElementsHelper, *rightElementsHelper;
	int *leftAmountOfElements, *rightAmountOfElements;
	long *leftOffset, *rightOffset;
	Node node, node1, node2, leftNode, rightNode;
	state value;

	if(offset == NULLPOINTER)
		return notfound;

	readNodeFromFile(offset, &node);

	elements = node.elements;
	pointers = node.pointers;
	amountOfElements = &node.amountOfElements;

	position = binarySearch(key, elements, *amountOfElements);

	if (pointers[0] == NULLPOINTER)
	{
		if (position == *amountOfElements || strcmp(key, elements[position].key) < 0)
			return notfound;

		if(elements[position].counter == 1)
		{
			returnOffset->clear();
			returnOffset->push_back(elements[position].offset);
		}

		if((elements[position].counter > 1) && (deleteAll == false))
		{
			elements[position].counter--;
			FILE *fileHelper1, *fileHelper2;
			fopen_s(&fileHelper1, key, "rb");
			fopen_s(&fileHelper2, "deleteTemp", "w+b");
			long k = 0;
			if(elements[position].offset == (*returnOffset)[0])
			{

				if(fileHelper1 != NULL && fileHelper2 != NULL)
				{
					fread(&k, sizeof(long), 1, fileHelper1);
					elements[position].offset = k;
					if(elements[position].counter > 1)
					{
						while(fread(&k, sizeof(long), 1, fileHelper1) > 0)
						{
							fwrite(&k, sizeof(long), 1, fileHelper2);
						}
						fclose(fileHelper1);
						fclose(fileHelper2);
						remove(key);
						rename("deleteTemp", key);
					}
					else if(elements[position].counter == 1)
					{
						fclose(fileHelper1);
						fclose(fileHelper2);
						remove(key);
						remove("deleteTemp");
					}
				}
			}
			else{
				if(fileHelper1 != NULL && fileHelper2 != NULL){
					if(elements[position].counter > 1)
					{
						while(fread(&k, sizeof(long), 1, fileHelper1) > 0)
						{
							if((*returnOffset)[0] != k)
								fwrite(&k, sizeof(long), 1, fileHelper2);
						}
						fclose(fileHelper1);
						fclose(fileHelper2);
						remove(key);
						rename("deleteTemp", key);
					}
					else if(elements[position].counter == 1)
					{
						fclose(fileHelper1);
						fclose(fileHelper2);
						remove(key);
						remove("deleteTemp");
					}
				}
			}
			writeNodeToFile(offset, &node);
			return success;
		}

		if(elements[position].counter > 1 && deleteAll == true)
		{
			returnOffset->push_back(elements[position].offset);
			FILE *fileHelper;
			fopen_s(&fileHelper, key, "rb");
			if(fileHelper != NULL)
			{
				long k = 0;
				while(fread(&k, sizeof(long), 1, fileHelper) >= 1)
				{
					returnOffset->push_back(k);
				}
				fclose(fileHelper);
				remove(key);
			}
		}

		for (i = position + 1; i < *amountOfElements; i++){
			elements[i-1] = elements[i];
			pointers[i] = pointers[i + 1];
		}
		--*amountOfElements;
		writeNodeToFile(offset, &node);
		if(offset == pointerToRoot)
		{
			if(*amountOfElements >= 1) return success;
			else return underflow;
		}
		else
		{
			if(*amountOfElements >= DEGREE) return success;
			else return underflow;
		}
	}

	elementsHelper = elements + position;
	pointerToLeft = pointers[position];
	readNodeFromFile(pointerToLeft, &node1);
	leftAmountOfElements = &node1.amountOfElements;

	if(position < *amountOfElements && (strcmp(key, (*elementsHelper).key) == 0))
	{
		Element *temp;
		readNodeFromFile(pointers[position], &node1);
		long off = pointers[position];
		int amOfEl = node1.amountOfElements;
		while (offsetHelper = node1.pointers[amOfEl], offsetHelper != NULLPOINTER)
		{
			off = offsetHelper;
			readNodeFromFile(off, &node1);
			amOfEl = node1.amountOfElements;
		}

		Element e = *elementsHelper;
		temp = node1.elements + amOfEl - 1;
		*elementsHelper = *temp; 
		*temp = e;
		writeNodeToFile(offset, &node);
		writeNodeToFile(off, &node1);
	}

	value = deleteHelper(key, pointerToLeft, deleteAll, returnOffset);
	if (value != underflow)
		return value;
	if (position < *amountOfElements)
		readNodeFromFile(pointers[position + 1], &rightNode);
	if (position == *amountOfElements || rightNode.amountOfElements == DEGREE){
		if (position > 0)
		{
			readNodeFromFile(pointers[position - 1], &leftNode);
			if (position == *amountOfElements || leftNode.amountOfElements > DEGREE)
				flag = true;
		}
	}

	if (flag){
		elementsHelper = elements + position - 1;
		pointerToLeft = node.pointers[position - 1];
		pointerToRight = node.pointers[position];
		node1 = leftNode;
		readNodeFromFile(pointerToRight, &node2);
		leftAmountOfElements = &node1.amountOfElements;
	}
	else
	{
		pointerToRight = pointers[position + 1];
		readNodeFromFile(pointerToLeft, &node1);
		node2 = rightNode;
	}

	rightAmountOfElements = &node2.amountOfElements;
	leftElementsHelper = node1.elements;
	rightElementsHelper = node2.elements;
	leftOffset = node1.pointers;
	rightOffset = node2.pointers;

	if (flag){
		node2.pointers[*rightAmountOfElements + 1] = node2.pointers[*rightAmountOfElements];
		for (i = *rightAmountOfElements; i > 0; i--){
			rightElementsHelper[i] = rightElementsHelper[i-1];
			rightOffset[i] = rightOffset[i-1];
		}
		++*rightAmountOfElements;
		rightElementsHelper[0] = *elementsHelper;
		rightOffset[0] = leftOffset[*leftAmountOfElements];
		*elementsHelper = leftElementsHelper[*leftAmountOfElements - 1];

		if (--*leftAmountOfElements >= DEGREE){
			writeNodeToFile(offset, &node);
			writeNodeToFile(pointerToLeft, &node1);
			writeNodeToFile(pointerToRight, &node2);
			return success;
		}
	}
	else if (*rightAmountOfElements > DEGREE)
	{
		leftElementsHelper[DEGREE-1] = *elementsHelper;
		leftOffset[DEGREE] = rightOffset[0];
		*elementsHelper = rightElementsHelper[0];
		++*leftAmountOfElements;
		--*rightAmountOfElements;
		for (i = 0; i < *rightAmountOfElements; i++)
		{
			rightOffset[i] = rightOffset[i + 1];
			rightElementsHelper[i] = rightElementsHelper[i + 1];
		}

		rightOffset[*rightAmountOfElements] = rightOffset[*rightAmountOfElements + 1];
		writeNodeToFile(offset, &node);
		writeNodeToFile(pointerToLeft, &node1);
		writeNodeToFile(pointerToRight, &node2);
		return success;
	}
	leftElementsHelper[DEGREE - 1] = *elementsHelper;
	leftOffset[DEGREE] = rightOffset[0];
	for (i = 0; i < DEGREE; i++){
		leftElementsHelper[DEGREE + i] = rightElementsHelper[i];
		leftOffset[DEGREE + i + 1] = rightOffset[i + 1];
	}
	*leftAmountOfElements = 2 * DEGREE;
	updateEmptyNode(pointerToRight);
	for (i = position + 1; i < *amountOfElements; i++){
		elements[i - 1] = elements[i];
		pointers[i] = pointers[i + 1];
	}
	--*amountOfElements;
	writeNodeToFile(offset, &node);
	writeNodeToFile(pointerToLeft, &node1);

	if(offset == pointerToRoot)
	{
		if(*amountOfElements >= 1) return success;
		else return underflow;
	}
	else
	{
		if(*amountOfElements >= DEGREE) return success;
		else return underflow;
	}
}

void Btree::updateEmptyNode(long offset)
{
	Node node;
	readNodeFromFile(offset, &node);
	node.pointers[0] = emptyNodePtr;
	emptyNodePtr = offset;
	writeNodeToFile(offset, &node);
}

void Btree::copyElements(Btree::Element *from, Btree::Element *to, int amountOfElements)
{
	for(int i = 0; i < amountOfElements; i++)
		to[i] = from[i];
}

state Btree::printTree(long offset)
{
	if(theTreeIsDeleted == false && this->createdAndOpened == true)
	{
		static int position=0;
		int i;
		Node node;
		static string s;
		if (offset != NULLPOINTER){
			position += 4;
			readNodeFromFile(offset, &node);
			for(int j = 0; j < position; j++)
				s.push_back(' ');
			cout << s;
			for (i = 0; i < node.amountOfElements; i++)
				cout << node.elements[i].key <<' '<< node.elements[i].offset << ' ' << node.elements[i].counter << ' ';
			cout << endl;
			for (i = 0; i <= node.amountOfElements; i++)
				printTree(node.pointers[i]);
			for(int j = 0; j < position; j++)
				s.pop_back();
			position -= 4;
		}
	}
	else return error;
	return success;
}

state Btree::createOrOpenTheTree(char *treeFileName)
{
	if(theTreeIsDeleted == true)
	{
		theTreeIsDeleted = false;
		this->treeFileName = treeFileName;
		fopen_s(&treeFile, treeFileName, "r+b");
		if (treeFile == NULL) 
		{ 
			fopen_s(&treeFile, treeFileName, "w+b"); 
			Write(); 
		}
		else Read();
	}
	else 
		return error;
	return success;
}

state Btree::clearTheTree()
{
	if(theTreeIsDeleted == false && this->createdAndOpened == true)
	{
		pointerToRoot = NULLPOINTER;
		emptyNodePtr = NULLPOINTER;
		FILE *file;
		fopen_s(&file, "clearTemp", "w+b");
		if(file != NULL)
		{
			fclose(file);
			fclose(treeFile);
			remove(this->treeFileName.c_str());
			rename("clearTemp", this->treeFileName.c_str());
			fopen_s(&treeFile, this->treeFileName.c_str(), "w+b");
			Write();
		}
		else return incomplete;
	}
	else return error;
	return success;
}

state Btree::deleteTheTree()
{
	if(treeFile != NULL)
		fclose(treeFile);
	if(remove(treeFileName.c_str()))
		return incomplete;
	else
	{
		theTreeIsDeleted = true;
		this->createdAndOpened = false;
		pointerToRoot = NULLPOINTER;
		emptyNodePtr = NULLPOINTER;
		treeFileName = "";
		return success;
	}
}

state Btree::findAndUpdate(char *key, long oldElementOffset, long newElementOffset)
{
	if(theTreeIsDeleted == false && this->createdAndOpened == true)
	{
		int position, amountOfElements;
		Element Array[2*DEGREE];
		Node node;
		long ptrToRoot = pointerToRoot;

		while(ptrToRoot != NULLPOINTER)
		{
			readNodeFromFile(ptrToRoot, &node);

			amountOfElements = node.amountOfElements;
			copyElements(node.elements, Array, amountOfElements);

			position = binarySearch(key, Array, amountOfElements);

			if((position < amountOfElements) && (strcmp(key, Array[position].key) == 0))
			{
				readNodeFromFile(ptrToRoot, &node);

				if(node.elements[position].counter > 1)
				{
					if(node.elements[position].offset == oldElementOffset)
					{
						node.elements[position].offset = newElementOffset;
						this->writeNodeToFile(ptrToRoot, &node);
					}
					else
					{
						FILE *fileHelper1, *fileHelper2;
						fopen_s(&fileHelper1, node.elements[position].key, "rb");
						fopen_s(&fileHelper2, "findAndUpdateTemp", "w+b"); 
						if(fileHelper1 != NULL)
						{
							long k = 0;
							while(fread(&k, sizeof(long), 1, fileHelper1) >= 1)
							{
								if(k == oldElementOffset)
									fwrite(&newElementOffset, sizeof(long), 1, fileHelper2);
								else
									fwrite(&k, sizeof(long), 1, fileHelper2); 
							}
							fclose(fileHelper1);
							fclose(fileHelper2);
							remove(node.elements[position].key);
							rename("findAndUpdateTemp", node.elements[position].key); 
						}
					}
				}
				if(node.elements[position].counter == 1)
				{
					node.elements[position].offset = newElementOffset;
					this->writeNodeToFile(ptrToRoot, &node);
				}
				return success;
			}
			ptrToRoot = node.pointers[position];
		}
		return notfound;
	}
	else return error;
}