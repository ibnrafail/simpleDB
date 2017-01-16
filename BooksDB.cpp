#include "BooksDB.h"
#include <ctime>
#include <fstream>

std::wstring convert(LPWSTR str) {
    return std::wstring(str);
}

BOOL IsDots (const std::wstring &strName)
{
    return strName == L"." || strName == L".." ;
}

BOOL IsDir (const WIN32_FIND_DATA &fdFile)
{
    return (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ;
}

BOOL removeDirectory (const std::wstring &strDir)
{
	WIN32_FIND_DATA fdFile;
	memset(&fdFile, 0, sizeof(fdFile));

	HANDLE hFind = INVALID_HANDLE_VALUE ;

	std::wstring strSearch = strDir + L"\\*.*" ;

	hFind = FindFirstFile(strSearch.data(), &fdFile) ;

	if (hFind == INVALID_HANDLE_VALUE) {
		return FALSE ;
	}

	do {    
		std::wstring strDelete = strDir + L"\\" + fdFile.cFileName ;

		if (IsDir (fdFile) == TRUE) {
			if (IsDots (fdFile.cFileName) == TRUE) {
				continue ;
			}

			removeDirectory(strDelete) ;
		}

		else {

			DeleteFile(strDelete.data()) ;
		}

	} while (FindNextFile(hFind, &fdFile) == TRUE) ;

	FindClose (hFind) ;
	RemoveDirectory (strDir.data()) ;

	return TRUE ;
}

BooksDB::BooksDB()
{
	btree = NULL;
	dataBaseFile = NULL;
}

BooksDB::~BooksDB()
{	
	this->closeDB();
}

int BooksDB::openDB(char *title)
{
	string helper = title;
	LPWSTR ws = new wchar_t[helper.size() + 1];
	copy( helper.begin(), helper.end(), ws );
	ws[helper.size()] = 0;
	DWORD dwRet;
	dwRet = GetCurrentDirectory(MAX_PATH, initialDirectory);
	if( dwRet == 0 )
	{
		return 8;
	}
	if(dwRet > MAX_PATH)
	{
		return 9;
	}

	if( !SetCurrentDirectory(ws))
	{
		return 20;
	}
	
	string btreeFileName = helper + "btree";
	btree = new Btree((char*)btreeFileName.c_str());
	if(btree->isCreatedAndOpened() == true && btree != NULL)
	{
		if (fopen_s(&dataBaseFile, title, "r+b") != 0 && dataBaseFile == NULL) 
		{ 
			if(fopen_s(&dataBaseFile, title, "w+b") == 0)
			{
				this->title = title;
			}
		}
		else
		{
			this->title = title;
		}
	}
	else
	{
		if( !SetCurrentDirectory(initialDirectory))
		{
			return 10;
		}
		btree = NULL;
		dataBaseFile = NULL;
		return 12;
	}
	return 0;
}

int BooksDB::createDB(char *title)
{
	string helper = title;
	LPWSTR ws = new wchar_t[helper.size() + 1];
	copy( helper.begin(), helper.end(), ws );
	ws[helper.size()] = 0;
	BOOL result = CreateDirectory(ws, NULL);
	if (result)
	{
		DWORD dwRet;
		dwRet = GetCurrentDirectory(MAX_PATH, initialDirectory);
		if( dwRet == 0 )
		{
			return 8;
		}
		if(dwRet > MAX_PATH)
		{
			return 9;
		}

		if( !SetCurrentDirectory(ws))
		{
			return 10;
		}

		string btreeFileName = helper + "btree";
		btree = new Btree((char*)btreeFileName.c_str());
		if(btree->isCreatedAndOpened() == true && btree != NULL)
		{

			if (fopen_s(&dataBaseFile, title, "r+b") != 0 && dataBaseFile == NULL) 
			{ 
				if(fopen_s(&dataBaseFile, title, "w+b") == 0)
				{
					fclose(this->dataBaseFile);
				}
			}
			else
			{
				fclose(this->dataBaseFile);
			}

			delete btree;
			btree = NULL;
		}
		else
		{
			btree = NULL;
			dataBaseFile = NULL;
			return 12;
		}

		if( !SetCurrentDirectory(initialDirectory))
		{
			return 10;
		}
	}
	else
	{
		if(ERROR_ALREADY_EXISTS == GetLastError())
			return 11;
	}
	this->dataBaseFile = NULL;
	return 0;
}

int BooksDB::closeDB()
{
	if(this->dataBaseFile != NULL)
	{
		fflush(dataBaseFile);
		FILE *fileHelper;
		fopen_s(&fileHelper, "closeDBTemp", "w+b");
		rewind(this->dataBaseFile);
		while(feof(this->dataBaseFile) == 0)
		{
			getc(this->dataBaseFile);
		}
		long size = ftell(this->dataBaseFile);
		if(fileHelper != NULL)
		{
			Item item;
			rewind(this->dataBaseFile);
			while(ftell(this->dataBaseFile) != size)
			{
				long oldElementOffset = ftell(this->dataBaseFile);
				if(fread(&item, sizeof(Item), 1, this->dataBaseFile) == 1)
				{
					if(item.deleted == false)
					{
						long newElementOffset = ftell(fileHelper);
						int res = btree->findAndUpdate(item.name, oldElementOffset, newElementOffset);
						if(res == success)
						{	
							fwrite(&item, sizeof(Item), 1, fileHelper);
						}
					}

				}
			}
		}
		fclose(fileHelper);
		fclose(this->dataBaseFile);
		std::wstring stemp = std::wstring(title.begin(), title.end());
		LPCWSTR sw = stemp.c_str();
		DeleteFile(sw);
		rename("closeDBTemp", title.c_str());

		if(this->btree != NULL)
			delete this->btree;
		this->btree = NULL;
		dataBaseFile = NULL;
		if(!SetCurrentDirectory(initialDirectory))
		{
			return 10;
		}
	}
	else return 12;
	return 0;
}

int BooksDB::deleteDB(char* title)
{
	if(this->dataBaseFile == NULL && btree == NULL)
	{
		string helper = title;
		LPWSTR ws = new wchar_t[helper.size() + 1];
		copy(helper.begin(), helper.end(), ws);
		ws[helper.size()] = 0;
		if(!removeDirectory(ws))
		{
			return 13;
		}
	}
	else return 12;
	return 0;
}

int BooksDB::clearDB()
{
	if(this->dataBaseFile != NULL)
	{
		if(btree != NULL)
		{
			if(btree->clearTheTree() == success)
			{
				fflush(dataBaseFile);
				fclose(dataBaseFile);
				remove(this->title.c_str());
				fopen_s(&dataBaseFile, this->title.c_str(), "w+b");
			}
			else return 12;
		}
		else return 12;
	}
	else return 12;
	return 0;
}

void BooksDB::printDB()
{
	if(this->dataBaseFile != NULL)
	{
		rewind(this->dataBaseFile);
		while(feof(this->dataBaseFile) == 0)
		{
			getc(this->dataBaseFile);
		}
		long size = ftell(this->dataBaseFile);
		rewind(this->dataBaseFile);
		Item item;
		long id;
		while(ftell(this->dataBaseFile) != size)
		{
			id = ftell(this->dataBaseFile);
			if(fread(&item, sizeof(Item), 1, this->dataBaseFile) == 1)
				item.printFields(id);
		}
	}
}

int BooksDB::addItem(char *name, int amountOfPages, char *author, char *publisher, int yearOfPublishing)
{
	if(this->dataBaseFile != NULL && this->btree != NULL)
	{
		rewind(this->dataBaseFile);
		long offset;
		Item newItem(name, amountOfPages, author, publisher, yearOfPublishing, false);
		if (fseek(this->dataBaseFile, 0L, SEEK_END))
			exit(1);
		offset = ftell(this->dataBaseFile);
		if (fwrite(&newItem, sizeof(Item), 1, this->dataBaseFile) == 0) 
			exit(1);
		fflush(this->dataBaseFile);
		if(btree->insertElement(name, offset) != success)
			return 12;
	}
	else
		return 12;
	return 0;
}

int BooksDB::deleteItemByName(char *name)
{
	if(this->dataBaseFile != NULL && this->btree != NULL)
	{
		vector<long> result;
		Item newItem;
		if(btree->deleteElement(name, true, &result) == success)
		{
			for(int i = 0; (unsigned)i < result.size(); i++)
			{
				if (fseek(this->dataBaseFile, result[i], SEEK_SET))
					exit(1);
				if (fread(&newItem, sizeof(Item), 1, this->dataBaseFile) == 0) 
					exit(1);
				if (fseek(this->dataBaseFile, result[i], SEEK_SET))
					exit(1);
				newItem.deleted = true;
				if (fwrite(&newItem, sizeof(Item), 1, this->dataBaseFile) == 0) 
					exit(1);
			}
		}
		else return 19;
	}
	else
		return 12;
	return 0;
}

int BooksDB::deleteItemInteractively(char *name, long id)
{
	if(this->dataBaseFile != NULL && this->btree != NULL)
	{
		vector<long> result;
		result.push_back(id);
		Item newItem;
		if(btree->deleteElement(name, false, &result) == success)
		{
			if(fseek(this->dataBaseFile, result[0], SEEK_SET))
				exit(1);
			if(fread(&newItem, sizeof(Item), 1, this->dataBaseFile) == 0) 
				exit(1);
			if(fseek(this->dataBaseFile, result[0], SEEK_SET))
				exit(1);
			newItem.deleted = true;
			if (fwrite(&newItem, sizeof(Item), 1, this->dataBaseFile) == 0) 
				exit(1);
		}
		else
			return 19;
	}
	else
		return 12;
	return 0;
}

int BooksDB::editItem(char *oldName, char *newName, int newAmountOfPages, char *newAuthor, char *newPublisher, int newYearOfPublishing, long id)
{
	if(this->dataBaseFile != NULL && this->btree != NULL)
	{
		vector<long> result;
		result.push_back(id);
		Item item;
		if(btree->deleteElement(oldName, false, &result) == success)
		{
			if(btree->insertElement(newName, id) == success)
			{
				if(fseek(this->dataBaseFile, id, SEEK_SET))
					exit(1);
				if(fread(&item, sizeof(Item), 1, this->dataBaseFile) == 0) 
					exit(1);
				if(fseek(this->dataBaseFile, id, SEEK_SET))
					exit(1);
				strcpy_s(item.name, MYSIZE, newName);
				strcpy_s(item.author, MYSIZE, newAuthor);
				strcpy_s(item.publisher, MYSIZE, newPublisher);
				item.amountOfPages = newAmountOfPages;
				item.yearOfPublishing = newYearOfPublishing;
				item.deleted = false;
				if (fwrite(&item, sizeof(Item), 1, this->dataBaseFile) == 0) 
					exit(1);
			}
			else
				return 19;
		}
		else
			return 19;
	}
	return 0;
}

int BooksDB::searchItemByName(char *name, vector<string> &ItemsFields)
{
	if(this->dataBaseFile != NULL && this->btree != NULL)
	{
		vector<long> result;
		Item newItem;
		if(btree->searchElement(name, &result) == success)
		{
			for(int i = 0; (unsigned)i < result.size(); i++)
			{
				fseek(this->dataBaseFile, result[i], SEEK_SET);
				fread(&newItem, sizeof(Item), 1, this->dataBaseFile);
				string s;
				s += std::to_string((_Longlong)result[i]);
				s += " ";
				s += newItem.name;
				s += " ";
				s += newItem.author;
				s += " ";
				s += newItem.publisher;
				s += " ";
				s += std::to_string((_Longlong)newItem.amountOfPages);
				s += " ";
				s += std::to_string((_Longlong)newItem.yearOfPublishing);
				ItemsFields.push_back(s);
			}
		}
		else return 19;
	}
	else
		return 12;
	return 0;
}

BooksDB::Item::Item(char *name, int amountOfPages, char *author, char *publisher, int yearOfPublishing, bool deleted)
{
	strncpy_s(this->name, 80, name, 79);
	strncpy_s(this->author, 80, author, 79);
	strncpy_s(this->publisher, 80, publisher, 79);
	this->yearOfPublishing = yearOfPublishing;
	this->amountOfPages = amountOfPages;
	this->deleted = deleted;
}

int BooksDB::checkTitle(string title)
{
	vector<short> result;
	char *pointer, *next_token1;
	char *str = new char[title.length() + 1];
	strcpy_s(str, title.length() + 1, title.c_str());
	pointer = strtok_s(str, "\\/:*?\"<>|", &next_token1);
	if(pointer != NULL && title.length() > strlen(pointer)){ 
		return 0;	
	}
	else
	{ 
		this->title = title;
		deleteSpaces(this->title);
		return -1;
	}
}

void BooksDB::Item::printFields(long id)
{
	cout << id << "  " << name << "   " << author << "   " << publisher << "   " << yearOfPublishing << "   " << amountOfPages << "   " << deleted << endl;
}

vector<short> BooksDB::checkFields(char *name, int amountOfPages, char *author, char *publisher, int yearOfPublishing)
{
	vector <short> result;
	if(name == "") result.push_back(1);
	char *p = NULL, *str1, *str2, *nextToken1, *nextToken2;
	str1 = new char [80];
	str2 = new char [80];
	strcpy_s(str1, 80, author);
	strcpy_s(str2, 80, publisher);
	p = strtok_s(str1, "0123456789\\/|=+()*&?^%$;#@!\"\'{}<>:~`.", &nextToken1);
	if(p != NULL && strlen(author) > strlen(p)) result.push_back(2);
	p = strtok_s(str2, "0123456789\\/|=+()*&?^%$;#@!\"\'{}<>:~`.", &nextToken2);
	if(p != NULL && strlen(publisher) > strlen(p)) result.push_back(3);
	struct tm *ptr = NULL;
	time_t lt;
	lt = time(NULL);
	ptr = localtime(&lt);
	int yearHelper = ptr->tm_year + 1900;
	if(yearOfPublishing > yearHelper || yearOfPublishing < -1 || yearOfPublishing == 0) result.push_back(4);
	if(amountOfPages <= 48) result.push_back(5);
	return result;
}

void BooksDB::printErrorToConsole(int errorNum)
{
	string e[20] = {		"The data base title cannot consist of the next symbols: \\/:*?\"<>|", 
							"The \'name\' field is empty", 
							"The \'author\' field cannot consist of the next symbols: 0123456789\\/|=+()*&?^%$;#@!\"\'{}<>:~`.", 
							"The \'publisher\' field cannot consist of the next symbols: 0123456789\\/|=+()*&?^%$;#@!\"\'{}<>:~`.", 
							"Invalid \'yearOfPublishing\' field.", 
							"Invalid \'amountOfPages\' field (minimum 48 pages are considered).", 
							"The data base has been already opened.", 
							"GetCurrentDirectory failed.", 
							"Buffer too small.", 
							"SetCurrentDirectory failed.",
							"The data base already exists.",
							"Function fail.",
							"Error while deleting directory.",
							"Invalid <name> field.",
							"Invalid <author> field.",
							"Invalid <publisher> field.",
							"Invalid <yearOfPublishing> field.",
							"Invalid <amountOfPages> field.",
							"The item is not found.",
							"The data base does not exist."};
	if(errorNum - 1 >= 0 && errorNum - 1 < 20)
		cout << e[errorNum - 1] << endl;
}
