#pragma once

class CallGroupList
{
public:
	CallGroupList(void);
	virtual ~CallGroupList(void);
	
	void	AddPerson(string PID);
	void	DelPerson(string PID="");
	string	GetFirstPerson();
	string	GetNextPerson(string CurPerson);
	
	CallGroupList &operator=(const CallGroupList &other);

	bool operator==(const string &other);
	bool operator>(const string &other);
	bool operator<(const string &other);
	
	string strGroupName;
	list<string> listPerson;
};
