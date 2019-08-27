#pragma once

#include <list>
#include "RCB.h"
//#include "CTree.h"
#include <string>
#include <map>

class RCB;//forward declaration
class Request;

//The PCB (Process Control Block) Class consists of:
//PID - unique identifying name - for this project, either "init" or a 1 letter name
//Priority - 0 (init only), 1, or 2, with lower numbers being higher priority
//otherResources - a list of resources that a process is currently utilizing
//status - a two part class:
//status.type = 0 (ready), 1 (running), 2 (blocked)
//status list = either the ready list (for ready or running processes) or a resource wait list (blocked)
//creationTree:
//parent - init does not have one
//list of children

class PCB {
public:
	PCB(std::list<PCB*>& RL); //to create the init
	PCB(std::string PID, int priority, std::list<PCB*>& RL, PCB* parent);
	~PCB();
	std::string getPID();
	std::map<RCB*, int> getResources();
	int getStatusType();
	std::list<PCB*> getRL();
	std::list<Request*> getWL();
	PCB* getParent();
	std::list<PCB*> getChildrenList();
	int getPriority();
	bool hasDescendant(std::string name);

	void setPriority(int p);
	void setStatusType(int type);
	void addChild(PCB* child);
	void removeChild(PCB* child);
	void addResource(RCB* resource, int num);
	void removeResource(std::string RID);
	void editResource(std::string RID, int num); //num = new value
	void setWL(std::list<Request*> wl);
	void clearWL();
	void setRL(std::list<PCB*> rl);
	void clearRL();

private:
	class CTree
	{
	public:
		CTree(); //only for init, which has no parent 
		CTree(PCB* parent);
		~CTree();

		bool hasDescendant(std::string name);
		void addChild(PCB* child);
		void removeChild(PCB* child);
		PCB* getParent();
		std::list<PCB*> getChildren();

	private:
		PCB* parent;
		std::list<PCB*> children;
	};

	class Status
	{
	public:
		Status(int type, std::list<PCB*>& RL);
		~Status();
		int getType();
		std::list<PCB*> getRL();
		std::list<Request*> getWL();

		void setType(int type);
		void setRList(std::list<PCB*> rl);
		void setWList(std::list<Request*> wl);
		void clearWList();
		void clearRList();
	private:
		int type; //0 = ready, 1 = running, 2 = blocked
		std::list<Request*> resourceWaitList;
		std::list<PCB*> readyList;
	};



	std::string PID; //init is always called "init". Other processes are named by the user
	std::map<RCB*, int> otherResources;
	Status* status; //type and list
	CTree* creationTree; //parent and children
	int priority; //0 - init, 1 - user, 2 - system
};
