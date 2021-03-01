#include "StdAfx.h"
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>

using namespace std;

#include "CallGroupList.h"

CallGroupList::CallGroupList(void)
{
}

CallGroupList::~CallGroupList(void)
{
}


bool CallGroupList::operator==(const string &other)
{
	if( this->strGroupName == other)
		return 1;
	return 0;
}
bool CallGroupList::operator>(const string &other)
{
	if( this->strGroupName > other)
		return 1;
	return 0;
}
bool CallGroupList::operator<(const string &other)
{
	if( this->strGroupName < other)
		return 1;
	return 0;
}

void CallGroupList::DelPerson(string PID)
{
	if( listPerson.size() > 0 )
	{
		if( PID != ""){
			list<string>::iterator mPID = find(this->listPerson.begin(), listPerson.end(), PID);
			if( mPID != listPerson.end() )
			{
				listPerson.erase(mPID);
			}
		}
		else
			listPerson.erase(listPerson.begin(), listPerson.end());
	}
	else
	{
		listPerson.erase(listPerson.begin(), listPerson.end());
	}
}

void CallGroupList::AddPerson(string PID)
{
	if( PID.size() > 0 )
	{
		list<string>::iterator mPID = find(this->listPerson.begin(), listPerson.end(), PID);
		if( mPID == listPerson.end() )
		{
			listPerson.push_back(PID);
		}
	}
}

string	CallGroupList::GetFirstPerson()
{
	if( listPerson.size() > 0 )
		return (string)(*listPerson.begin());
	return "";
}
string	CallGroupList::GetNextPerson(string CurPerson)
{
	if( listPerson.size() <= 0 )
		return "";
	
	list<string>::iterator mPID = find(this->listPerson.begin(), listPerson.end(), CurPerson);
	if( mPID++ != listPerson.end() )
	{
		if( mPID != listPerson.end() )
		{
			return *mPID;
		}
	}
	return "";
}

CallGroupList &CallGroupList::operator =(const CallGroupList& other)
{
	CallGroupList *pOther = (CallGroupList*)&other;
	this->strGroupName = other.strGroupName;
	this->listPerson.erase( listPerson.begin(), listPerson.end());
	int iCount = pOther->listPerson.size();
	string person= pOther->GetFirstPerson();
	while(person != "" )
	{
		listPerson.push_back(person);
		person = pOther->GetNextPerson(person);
	}
	return *this;
}