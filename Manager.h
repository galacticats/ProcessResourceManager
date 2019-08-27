#pragma once
#include <list>
#include "PCB.h"
#include "RCB.h"
#include <vector>
#include <set>
#include <stdio.h>
#include <iostream>
#include <string>
class Manager
{
public:
	Manager(); //creates init process with priority 0 and runs it
	~Manager();
	std::string create(std::string PID, int p); //creates a new process with a single letter name at a priority level 1 or 2
	std::string destroy(std::string PID); //destroys the process with the name indicated
	std::string request(std::string RID, int num); //currently running process requests the given number of units from a resource
	std::string release(std::string RID, int num); //currently running process releases the given number units of a resource
	std::string timeout();
	std::string scheduler(); //returns the name of the currently running process

	void printProcesses(); //list all processes and their statuses
	void printResources(); //list all resources and their statuses
	void printProcess(std::string PID); //print one process' PCB info
	void printResource(std::string RID); //print one resource's RCB info
private:
	std::vector<std::list<PCB*>*> readyList;
	std::list<PCB*> zeroList;
	std::list<PCB*> oneList;
	std::list<PCB*> twoList;
	std::vector<RCB*> resourceList; //1 for R1, 2 for R2, 3 for R3, 4 for R4
	PCB* runningProcess;
	std::set<std::string> pNames; //names of all existing processes, for error checking

	void destroyTree(PCB& root);
	void removeFromWaitLists(PCB& root);
	void releaseAllResources(PCB& root);
};

