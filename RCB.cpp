#include "RCB.h"

RCB::RCB(std::string name, int total)
{
	RID = name;
	totalUnits = total;
	freeUnits = total;
	waitingList = std::list<Request*>();
}


RCB::~RCB()
{
}

std::string RCB::getRID() {
	return RID;
}

int RCB::getTotal() {
	return totalUnits;
}

int RCB::getFree() {
	return freeUnits;
}

std::list<Request*> RCB::getList() {
	return waitingList;
}

void RCB::setTotal(int tot) {
	totalUnits = tot;
	return;
}

void RCB::setFree(int num) {
	freeUnits = num;
	return;
}

void RCB::addProcess(PCB& process, int num) {
	waitingList.push_back(new Request(process, num));
	return;
}

void RCB::removeProcess(std::string PID) {
	//a process is ONLY REMOVED when there are enough resources, or when the entire process is destroyed
	//cannot remove part of a process. must remove the whole thing at once.
	std::list<Request*>::iterator it = waitingList.begin();
	while ((*it)->process->getPID().compare(PID) != 0 && it != waitingList.end()) { ++it; }
	if ((*it)->getProcess()->getPID().compare(PID) == 0) {
		waitingList.erase(it);
	}
	return;
}

//---------------------------------------REQUEST CLASS------------------------------------------
Request::Request(PCB& proc, int num) {
	process = &proc;
	units = num;
	return;
}

Request::~Request() {
}

PCB* Request::getProcess() {
	return process;
}

int Request::getNum() {
	return units;
}