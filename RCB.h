#pragma once


#include <string>
#include <list>
#include "PCB.h"
#include <map>
//RCB is the resource control block, made up of:
//RID - unique, identifying name - in this project, R1, R2, R3, or R4
//totalUnits - number of units that one resource has in total
//freeUnits - number of units that are free for one resource and can be requested
//waitingList - a list of pointers to process control blocks that are blocked and waiting
//for the specific resource.

class PCB; //forward declaration

class Request
{
public:
	Request(PCB& proc, int num);
	~Request();
	PCB* getProcess();
	int getNum();

	//private:
	PCB* process;
	int units;
};

class RCB
{
public:
	RCB(std::string name, int total);
	~RCB();
	std::string getRID();
	int getTotal();
	int getFree();
	std::list<Request*> getList();

	void setTotal(int tot);
	void setFree(int num);
	void addProcess(PCB& process, int num); //to the waiting list, with # of requested resources
	void removeProcess(std::string PID); //from the waiting list
	std::list<Request*> waitingList;
private:
	std::string RID;
	int totalUnits; //"status"
	int freeUnits; //"status"

};
