//Bianca Tang

#include "Manager.h"
//--------------------------------------------------------------------------------------
//SETUP AND DELETION

Manager::Manager()
{
	//Create initial process
	readyList.push_back(&zeroList);
	readyList.push_back(&oneList);
	readyList.push_back(&twoList);
	PCB* init = new PCB(zeroList); //create initial process
	zeroList.push_back(init); //add init into level 0 priority list
	runningProcess = init;
	resourceList.push_back(new RCB("R1", 1));
	resourceList.push_back(new RCB("R2", 2));
	resourceList.push_back(new RCB("R3", 3));
	resourceList.push_back(new RCB("R4", 4));
	pNames.insert("init");
}


Manager::~Manager()
{
	for (int i = 0; i < resourceList.size(); i++) {
		delete resourceList.at(i);
	}
	while (zeroList.size() > 0) {
		delete zeroList.front();
	}
	while (oneList.size() > 0) {
		delete oneList.front();
	}
	while (twoList.size() > 0) {
		delete twoList.front();
	}
}

//--------------------------------------------------------------------------
//CREATE, DESTROY

std::string Manager::create(std::string PID, int p) //creates a new process with a single letter name at a priority level 1 or 2
{
	//ignore any requests to create a process with the same name as an existing one
	if (pNames.count(PID) != 0 || PID.length() > 1) { return "error "; }
	else if (p == 1) {
		PCB* process = new PCB(PID, p, oneList, runningProcess);
		oneList.push_back(process);
		runningProcess->addChild(process);
	}
	else if (p == 2) {
		PCB* process = new PCB(PID, p, twoList, runningProcess);
		twoList.push_back(process);
		runningProcess->addChild(process);
	}
	else {
		return "error ";
	}
	pNames.insert(PID);
	return scheduler();
}

std::string Manager::destroy(std::string PID) //destroys the process with the name indicated
{
	//ignore any requests to destroy non-existant processes. Init cannot destroy itself
	if (pNames.count(PID) == 0 || PID.compare("init") == 0) { return "error "; }
	//if the running process wants to destroy itself
	if (runningProcess->getPID().compare(PID) == 0) {
		destroyTree(*runningProcess);
		delete runningProcess;
		runningProcess = NULL;
	}
	//ignore any requests to destroy non-descendant processes
	else if (runningProcess->getPID().compare(PID) == 0 || runningProcess->hasDescendant(PID)) {
		//find that descendant and then destroy the subtree with that as the root
		std::list<PCB*>::iterator it1, it2;
		PCB* root = NULL;
		//iterate through oneList to find the descendant
		for (it1 = oneList.begin(); it1 != oneList.end() && root == NULL; it1++) {
			if ((*it1)->getPID().compare(PID) == 0) {
				root = (*it1);
			}
		}
		//iterate through twoList to find the descendant
		for (it2 = twoList.begin(); it2 != twoList.end() && root == NULL; it2++) {
			if ((*it2)->getPID().compare(PID) == 0) {
				root = (*it2);
			}
		}
		//Iterate through resource waiting lists to find descendant
		for (int i = 0; i < resourceList.size() && root == NULL; i++) {
			//check to see if each resource has it in their wait list
			std::list<Request*>::iterator it3;
			for (it3 = resourceList.at(i)->waitingList.begin(); it3 != resourceList.at(i)->waitingList.end() && root == NULL; it3++) {
				if ((*it3)->process->getPID().compare(PID) == 0) {
					root = (*it3)->process;
				}
			}
		}
		destroyTree(*root);
	}
	else { return "error "; } //the named process is not a descendant
	return scheduler();
}

//------------------------------------------------------------------------------------------------------------------------------
//Helper functions for destroy

void Manager::destroyTree(PCB& root) {
	if (!root.getChildrenList().empty()) {
		std::list<PCB*> cList = root.getChildrenList();
		std::list<PCB*>::iterator it1 = cList.begin();
		while (it1 != cList.end()) {
			//delete bottom up
			destroyTree(**it1);
			it1 = cList.erase(it1);
			if (it1 != cList.end()) {
				it1++;
			}
		}
	}
	//once the node has no more children:
	//get rid of any RCB wait list pointers
	removeFromWaitLists(root);
	//release any held resources
	releaseAllResources(root);
	//delete the pointer in RL to this PCB
	std::list<PCB*>::iterator it;
	if (root.getPriority() == 1) {
		it = oneList.begin();
		while (it != oneList.end() && (*it)->getPID().compare(root.getPID()) != 0) { it++; }
		if (it != oneList.end()) {
			oneList.erase(it);
		}
	}
	else if (root.getPriority() == 2) {
		it = twoList.begin();
		while (it != twoList.end() && (*it)->getPID().compare(root.getPID()) != 0) { it++; }
		if (it != twoList.end()) {
			twoList.erase(it);
		}
	}
	//remove from pNames to free up the name
	//also remove from it's parent's "children" list
	root.getParent()->removeChild(&root);
	pNames.erase(root.getPID());
	//delete root;
	return;
}
void Manager::removeFromWaitLists(PCB& root) {
	//each resource can only be in ONE waitlist - it's impossible for a blocked/waitlisted resource to run and request another resource
	for (int i = 0; i < resourceList.size(); i++) {
		//check to see if each resource has it in their wait list
		std::list<Request*> waitlist = resourceList.at(i)->waitingList;
		std::list<Request*>::iterator it = resourceList.at(i)->waitingList.begin();
		while (it != resourceList.at(i)->waitingList.end()) {
			if ((*it)->getProcess()->getPID().compare(root.getPID()) == 0) {
				resourceList.at(i)->waitingList.erase(it++);
				return;
			}
			else {
				++it;
			}
		}
	}
	return;
}

//For each held resource, updates/increases the resource's free count -> ok
//If the first process in the wait list for the freed resource can now run:
//Give it the resource
//Remove it from the wait list, point its waitlist pointer to a blank list
//Set its status to ready
//Move it to a ready list
void Manager::releaseAllResources(PCB& root) {
	std::map<RCB*, int> resources = root.getResources();
	std::map<RCB*, int>::iterator it;
	for (it = resources.begin(); it != resources.end(); it++) { //for each held resource
		it->first->setFree(it->first->getFree() + it->second); //increase Free amount

		std::list<Request*> reqList = it->first->waitingList;
		//see if the first item in that resource's waitlist can now be satisfied: if so, set it to ready and give it the resources
		while (reqList.size() > 0 && reqList.front()->getNum() <= it->first->getFree()) {
			reqList.front()->process->setStatusType(0); //set to ready
			reqList.front()->process->addResource(it->first, reqList.front()->getNum());//add to process' resource list
			it->first->setFree(it->first->getFree() - reqList.front()->getNum());//decrease resource's Free amount
																				 //make the status list of the newly ready process point to the ready list
			if (reqList.front()->process->getPriority() == 1) {
				oneList.push_back(reqList.front()->process);
				reqList.front()->process->setRL(oneList);
			}
			else if (reqList.front()->process->getPriority() == 2) {
				twoList.push_back(reqList.front()->process);
				reqList.front()->process->setRL(twoList);
			}
			reqList.front()->process->clearWL();
			it->first->waitingList.pop_front();
			reqList = it->first->waitingList; //newly added
		}
	}
	return;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//REQUEST, RELEASE, TIMEOUT, SCHEDULER

std::string Manager::request(std::string RID, int num) //currently running process requests the given number of units from a named resource
													   //Able to go from running to blocked
{
	//ignore if the request comes from "init", which is not allowed to request
	if (RID.compare("init") == 0) {
		return "error ";
	}
	//check to see if there are enough free units in the first place
	std::vector<RCB*>::iterator it = resourceList.begin();
	while ((*it)->getRID().compare(RID) != 0 && it != resourceList.end()) { it++; }
	//request cannot be more than the total number of units -> dont put request on waitlist
	//If request + currently held units > total, also reject.
	std::map<RCB*, int> r = runningProcess->getResources();
	std::map<RCB*, int>::iterator rit = r.begin();
	int owned = 0;
	while (rit != r.end() && rit->first->getRID().compare(RID) != 0) { ++rit; }
	if (rit != r.end()) {
		owned = rit->second;
	}
	if ((num + owned) > (*it)->getTotal()) {
		return "error ";
	}
	//also check to see that the wait list is empty
	else if ((*it)->getFree() >= num && (*it)->getList().size() == 0) { //then the process will get the resources it requested
		runningProcess->addResource((*it), num);
		(*it)->setFree((*it)->getFree() - num);
	}
	else { //process becomes blocked
		runningProcess->setStatusType(2); //2 for blocked, 1 for running, 0 for ready
										  //remove process from the ready list
		std::list<PCB*>::iterator rlit;
		if (runningProcess->getPriority() == 1) {
			rlit = oneList.begin();
			while ((*rlit)->getPID().compare(runningProcess->getPID()) != 0 && rlit != oneList.end()) { rlit++; }
			oneList.erase(rlit);
		}
		else if (runningProcess->getPriority() == 2) {
			rlit = twoList.begin();
			while ((*rlit)->getPID().compare(runningProcess->getPID()) != 0 && rlit != twoList.end()) { rlit++; }
			twoList.erase(rlit);
		}
		//process PCB is added into the resource's wait list
		(*it)->addProcess(*runningProcess, num);
		//process status.list points to the resource wait list
		runningProcess->setWL((*it)->getList());
	}
	return scheduler();
}

std::string Manager::release(std::string RID, int num) //currently running process releases the given number units of a resource
													   //Able to go from blocked to ready
{
	//check to see if the current process is even holding onto any of those resources
	int count = 0;
	std::map<RCB*, int> resources = runningProcess->getResources();
	std::map<RCB*, int>::iterator it;
	for (it = resources.begin(); it != resources.end(); it++) {
		if (it->first->getRID().compare(RID) == 0) {
			count = it->second;
		}
	}
	if (count >= num) {
		//Then we have enough resources to release
		std::map<RCB*, int>::iterator it2 = resources.begin();
		while (it2 != resources.end() && it2->first->getRID().compare(RID) != 0) { it2++; }
		if (it2->first->getRID().compare(RID) == 0) {
			it2->first->setFree(it2->first->getFree() + num); //increase resource's Free count
			if (count - num > 0) {
				runningProcess->editResource(RID, count - num); //reduce the number of resources
			}
			//-----------------------------
			//Now see if, for that resource, the first item in the waiting list can be satisfied
			std::list<Request*> reqList = it2->first->waitingList;
			//see if the first item in that resource's waitlist can now be satisfied: if so, set it to ready and give it the resources
			while (reqList.size() > 0 && reqList.front()->getNum() <= it2->first->getFree()) {
				reqList.front()->process->setStatusType(0); //set to ready
				reqList.front()->process->addResource(it2->first, reqList.front()->getNum()); //add to process' resource list
				it2->first->setFree(it2->first->getFree() - reqList.front()->getNum()); //decrease resource's Free amount
																						//make the status list of the newly ready process point to the ready list
				if (reqList.front()->process->getPriority() == 1) {
					oneList.push_back(reqList.front()->process);
					reqList.front()->process->setRL(oneList);
				}
				else if (reqList.front()->process->getPriority() == 2) {
					twoList.push_back(reqList.front()->process);
					reqList.front()->process->setRL(twoList);
				}
				reqList.front()->process->clearWL();
				it2->first->waitingList.pop_front();
				reqList = it2->first->waitingList; //newly added
			}
			//-----------------------------
			if (count - num == 0) {
				runningProcess->removeResource(RID); //remove from otherResources if releasing num resources brings the count to 0
			}
		}
	}
	else {
		return "error ";
	}
	return scheduler();
}

std::string Manager::timeout()
{//Grabs the running process, sets it to ready, and then moves it to the back of its priority queue in the RL
	runningProcess->setStatusType(0); //set to ready
									  //remove process from the queue and add to back
	std::list<PCB*>::iterator it;
	if (runningProcess->getPriority() == 1) {
		it = readyList.at(1)->begin();
		while ((*it)->getPID().compare(runningProcess->getPID()) != 0 && it != readyList.at(1)->end()) { it++; }
		it = readyList.at(1)->erase(it);
		readyList.at(1)->push_back(runningProcess);
	}
	else if (runningProcess->getPriority() == 2) {
		it = readyList.at(2)->begin();
		while ((*it)->getPID().compare(runningProcess->getPID()) != 0 && it != readyList.at(2)->end()) { it++; }
		it = readyList.at(2)->erase(it);
		readyList.at(2)->push_back(runningProcess);
	}
	//Find highest priority process and set it to running. set it to runningProcess
	//go through both lists. If both are empty or if all processes are blocked, run init.
	bool foundReady = false;
	std::list<PCB*>::iterator it2;
	if (readyList.at(2)->size() != 0) { //go through priority list 2 and try to find a ready process
		for (it2 = readyList.at(2)->begin(); it2 != readyList.at(2)->end() && !foundReady; it2++) {
			if ((*it2)->getStatusType() == 0) {
				foundReady = true;
				(*it2)->setStatusType(1);
				runningProcess = (*it2);
			}
		}
	}
	if (!foundReady && readyList.at(1)->size() != 0) {
		for (it2 = readyList.at(1)->begin(); it2 != readyList.at(1)->end() && !foundReady; it2++) {
			if ((*it2)->getStatusType() == 0) {
				foundReady = true;
				(*it2)->setStatusType(1);
				runningProcess = (*it2);
			}
		}
	}
	if (!foundReady) {
		readyList.at(0)->front()->setStatusType(1); //init runs
		runningProcess = readyList.at(0)->front();
	}
	return scheduler();
}

std::string Manager::scheduler() //is able to change from ready to running or running to ready
{
	//Find highest priority process p
	std::list<PCB*>::iterator it;
	PCB* p = NULL;
	if (readyList.at(2)->size() != 0) { //go through priority list 2 and try to find a ready process
		for (it = readyList.at(2)->begin(); it != readyList.at(2)->end() && p == NULL; it++) {
			if ((*it)->getStatusType() == 0) {
				p = (*it);
			}
		}
	}
	if (p == NULL && readyList.at(1)->size() != 0) {
		for (it = readyList.at(1)->begin(); it != readyList.at(1)->end() && p == NULL; it++) {
			if ((*it)->getStatusType() == 0) {
				p = (*it);
			}
		}
	}
	if (p == NULL) {
		p = readyList.at(0)->front();
	}
	//If the current running process has a lower priority, then switch to the higher priority ready process
	if (runningProcess == NULL || runningProcess->getPriority() < p->getPriority() || runningProcess->getStatusType() != 1) {
		if (runningProcess != NULL && runningProcess->getStatusType() == 1) {
			runningProcess->setStatusType(0);
		}
		p->setStatusType(1);
		runningProcess = p;
	}
	return runningProcess->getPID();
}

//--------------------------------------------------------------------------------------------
//DEBUGGING PRINTOUTS

void Manager::printProcesses() //list all processes and their statuses
{
	for (int i = 0; i < 3; i++) { //for each priority list
		std::list<PCB*>::iterator it;
		std::cout << "Priority: " << i << std::endl;
		for (it = readyList.at(i)->begin(); it != readyList.at(i)->end(); it++) {
			int stat = (*it)->getStatusType();
			std::string PID = (*it)->getPID();
			std::cout << "Process: " << PID << " Status: " << stat << std::endl;
		}
	}
	return;
}

void Manager::printResources() //list all resources and their statuses
{
	for (int i = 0; i < resourceList.size(); i++) { //for each of 4 resources
		std::cout << "Resource: " << resourceList.at(i)->getRID() << " Total: " << resourceList.at(i)->getTotal() << " Free: " << resourceList.at(i)->getFree() << std::endl;
		std::list<Request*> rList = resourceList.at(i)->getList();
		for (std::list<Request*>::iterator it = rList.begin(); it != rList.end(); it++) {
			//For each waiting PCB, print out its name and how many it requested
			std::cout << "Waiting Process: " << (*it)->getProcess()->getPID() << " Units: " << (*it)->getNum() << std::endl;
		}
	}
	return;
}

void Manager::printProcess(std::string PID) //print one process' PCB info
{
	bool found = false;
	if (pNames.count(PID) == 0) {
		std::cout << "Process doesn't exist." << std::endl;
		return;
	}
	else {
		for (int i = 0; i < 3 && !found; i++) { //for each priority list
			std::list<PCB*>::iterator it;
			for (it = readyList.at(i)->begin(); it != readyList.at(i)->end() && !found; i++) {
				if ((*it)->getPID().compare(PID) == 0) {
					found = true;
					std::cout << "Process: " << (*it)->getPID() << " Status: " << (*it)->getStatusType() << " Priority: " << i << " ParentID: " << (*it)->getParent()->getPID() << std::endl;
					//Print each one of its children's IDs
					std::list<PCB*>::iterator it2;
					for (it2 = (*it)->getChildrenList().begin(); it2 != (*it)->getChildrenList().end(); it2++) {
						std::cout << "ChildID: " << (*it2)->getPID() << std::endl;
					}
					//Print each one of its taken resources
					std::map<RCB*, int>::iterator it3;
					for (it3 = (*it)->getResources().begin(); it3 != (*it)->getResources().end(); it3++) {
						std::cout << "RID: " << it3->first->getRID() << " Units: " << it3->second << std::endl;
					}
				}
			}
		}
		std::cout << std::endl;
	}

}

void Manager::printResource(std::string RID) //print one resource's RCB info
{
	std::vector<RCB*>::iterator it;
	for (it = resourceList.begin(); it != resourceList.end(); it++) {
		if ((*it)->getRID().compare(RID) == 0) { //print out that resource's info
			std::cout << "Resource: " << (*it)->getRID() << " Total: " << (*it)->getTotal() << " Free: " << (*it)->getFree() << std::endl;
			for (std::list<Request*>::iterator it2 = (*it)->getList().begin(); it2 != (*it)->getList().end(); it2++) {
				//For each waiting PCB, print out its name and how many it requested
				std::cout << "Waiting Process: " << (*it2)->getProcess()->getPID() << " Units: " << (*it2)->getNum() << std::endl;
			}
		}
	}

}