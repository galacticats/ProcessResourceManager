#include "PCB.h"

PCB::PCB(std::list<PCB*>& RL) { //for creating init
	PID = "init";
	priority = 0;
	otherResources = std::map<RCB*, int>();
	status = new Status(1, RL); //init will always run first
	creationTree = new CTree(); //no parent
	return;
}

PCB::PCB(std::string name, int pri, std::list<PCB*>& RL, PCB* parent)
{
	PID = name;
	priority = pri;
	otherResources = std::map<RCB*, int>();
	status = new Status(0, RL); //processes always start off in the Ready state
	creationTree = new CTree(parent);
	return;
}

PCB::~PCB()
{
	delete status;
	delete creationTree;
}

std::string PCB::getPID() {
	return PID;
}

std::map<RCB*, int> PCB::getResources() {
	return otherResources;
}

int PCB::getStatusType() {
	return status->getType();
}

std::list<PCB*> PCB::getRL() {
	return status->getRL();
}

std::list<Request*> PCB::getWL() {
	return status->getWL();
}

PCB* PCB::getParent() {
	return creationTree->getParent();
}

std::list<PCB*> PCB::getChildrenList() {
	return creationTree->getChildren();
}

int PCB::getPriority() {
	return priority;
}

bool PCB::hasDescendant(std::string name) {
	return creationTree->hasDescendant(name);
}

void PCB::setPriority(int p) {
	priority = p;
	return;
}

void PCB::setStatusType(int type) {
	status->setType(type);
	return;
}

void PCB::addChild(PCB* child) {
	creationTree->addChild(child);
	return;
}

void PCB::removeChild(PCB* child) {
	creationTree->removeChild(child);
	return;
}

void PCB::addResource(RCB* resource, int num) {
	otherResources[resource] += num;
	return;
}

void PCB::removeResource(std::string RID) {
	std::map<RCB*, int>::iterator it = otherResources.begin();
	while (it->first->getRID().compare(RID) != 0 && it != otherResources.end()) { ++it; }
	if (it->first->getRID().compare(RID) == 0) {
		it = otherResources.erase(it);
	}
	return;
}

void PCB::editResource(std::string RID, int num) {
	std::map<RCB*, int>::iterator it = otherResources.begin();
	while (it->first->getRID().compare(RID) != 0 && it != otherResources.end()) { ++it; }
	if (it->first->getRID().compare(RID) == 0) {
		it->second = num;
	}
}

void PCB::setWL(std::list<Request*> wl) {
	status->setWList(wl);
	return;
}

void PCB::clearWL() {
	status->clearWList();
	return;
}

void PCB::setRL(std::list<PCB*> rl) {
	status->setRList(rl);
	return;
}

void PCB::clearRL() {
	status->clearRList();
	return;
}


//---------------------------------------NESTED CLASS CTREE-------------------------------------------------

PCB::CTree::CTree() {//only for init, which has no parent 
	parent = nullptr;
	children = std::list<PCB*>();
}

PCB::CTree::CTree(PCB* p) {
	parent = p;
	children = std::list<PCB*>();
}

PCB::CTree::~CTree() {
}

bool PCB::CTree::hasDescendant(std::string name) {
	if (children.empty()) {
		return false;
	}
	else {
		bool found = false;
		for (std::list<PCB*>::iterator it = children.begin(); it != children.end() && !found; ++it) {
			if ((*it)->getPID().compare(name) == 0) {
				return true;
			}
			else { //check the child's children
				found = (*it)->hasDescendant(name);
			}
		}
		return found;
	}
}

void PCB::CTree::addChild(PCB* child) {
	children.push_back(child);
	return;
}

void PCB::CTree::removeChild(PCB* child) {
	children.remove(child);
	return;
}

PCB* PCB::CTree::getParent() {
	return parent;
}

std::list<PCB*> PCB::CTree::getChildren() {
	return children;
}

//-------------------------------------------NESTED CLASS STATUS-------------------------------------------
PCB::Status::Status(int t, std::list<PCB*>& RL) {
	type = t;
	readyList = RL;
	return;
}

PCB::Status::~Status() {

}

int PCB::Status::getType() {
	return type;
}

std::list<PCB*> PCB::Status::getRL() {
	return readyList;
}

std::list<Request*> PCB::Status::getWL() {
	return resourceWaitList;
}

void PCB::Status::setType(int t) {
	type = t;
	return;
}

void PCB::Status::setRList(std::list<PCB*> rl) {
	readyList = rl;
	return;
}

void PCB::Status::setWList(std::list<Request*> wl) {
	resourceWaitList = wl;
	return;
}

void PCB::Status::clearWList() {
	resourceWaitList = std::list<Request*>();
	return;
}

void PCB::Status::clearRList() {
	readyList = std::list<PCB*>();
	return;
}