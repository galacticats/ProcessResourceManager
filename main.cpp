
#include "Manager.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <istream>


int main(int argc, char* argv[]) {
	std::string iFile = argv[1];
	std::string oFile = argv[2];
	//start up the manager and create a parser
	Manager* prm = new Manager();
	//open up the input file
	std::ifstream inFile(iFile);
	//create an output file
	std::ofstream outFile(oFile);
	//std::cout << "Files created" <<std::endl; //debugging

	if (inFile.is_open() && outFile.is_open()) {
		std::string line;
		outFile << "init ";
		while (getline(inFile, line)) { //while the file hasn't hit the end, process lines
			try {
				//Parse the command
				std::istringstream lineStream(line);
				std::string word;
				std::vector<std::string> command;
				std::string current;
				bool newTest = false;
				while (getline(lineStream, word, ' ')) {
					if (word.length() > 0) {
						command.emplace_back(word);
					}
				}
				//debugging
				std::cout << line << std::endl;
				if (command.size() != 0) {
					//std::cout << command.size() << std::endl;
					//Do the command
					if (command.at(0).compare("init") == 0 && command.size() == 1) {
						//return system to original state
						Manager* prm = new Manager(); //just remake the system
						outFile << std::endl << "init ";
						newTest = true;
					}
					else if (command.at(0).compare("cr") == 0 && command.size() == 3) {
						//cr <name> <priority> - create
						current = prm->create(command.at(1), stoi(command.at(2)));
						std::cout << current << std::endl;
					}
					else if (command.at(0).compare("de") == 0 && command.size() == 2) {
						//de <name> - delete
						current = prm->destroy(command.at(1));
						std::cout << current << std::endl;
					}
					else if (command.at(0).compare("req") == 0 && command.size() == 3) {
						//req <name> <# of units> - request
						current = prm->request(command.at(1), stoi(command.at(2)));
						std::cout << current << std::endl;
					}
					else if (command.at(0).compare("rel") == 0 && command.size() == 3) {
						//rel <name> <# of units> - release
						current = prm->release(command.at(1), stoi(command.at(2)));
						std::cout << current << std::endl;
					}
					else if (command.at(0).compare("to") == 0 && command.size() == 1) {
						//to - timeout
						current = prm->timeout();
						std::cout << current << std::endl;
					}
					else if (command.at(0).compare("allp") == 0 && command.size() == 1) {
						//allp - list all processes
						prm->printProcesses();
					}
					else if (command.at(0).compare("allr") == 0 && command.size() == 1) {
						//allr - list all resources
						prm->printResources();
					}
					else if (command.at(0).compare("lp") == 0 && command.size() == 2) {
						//lp <PID> - list info of process with PID
						prm->printProcess(command.at(1));
					}
					else if (command.at(0).compare("lr") == 0 && command.size() == 2) {
						//lr <RID> - list info of resource with RID
						prm->printResource(command.at(1));
					}
					if ((current.compare(" ") != 0 && current.length() == 1) || current.compare("init") == 0) {
						outFile << current << " ";
					}
					else if (!newTest) {
						outFile << "error ";
					}
				}
			}
			catch (...) {
				outFile << "error ";
			}
		}
		outFile.flush();
		delete prm;
		inFile.close();
		outFile.close();
	}
	else {
		std::cout << "File couldn't be opened" << std::endl;
	}
	return 0;
}