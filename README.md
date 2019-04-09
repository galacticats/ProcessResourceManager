# ProcessResourceManager

Full description of the project is in the PowerPoint and FullProjectDescription.pdf<br>

<b>Basic description:</b>
- The Process Resource Manager simulates the creation and destruction of processes, as well as the request and release of resources.
- There are 3 priority levels, 0 which is only used by the Init process, 1, and 2. 
- There are 4 resource types: R1 with 1 unit, R2 with 2 units, R3 with 3 units, and R4 with 4 units. 
- Processes in higher priorities will run before processes in lower priorities. The first ready process in priority order is that process that runs at any given time. 
- Only the currently running process is allowed to create, destruct, request, or release resources. 
- If a process requests a resource that is not available, it becomes blocked and enters the resource's waiting list. Resource waiting list requests are fulfilled in FIFO order to prevent starvation. 
- Deadlocks between non-init processes are possible in this implementation. If this happens, Init will be the active process. 
- Init is a special process in that it is not allowed to request or release resources, and it cannot self-destruct. It is the only process that is allowed to be at priority level 0.

<b>Basic commands for input file:</b>
- init <br>
  Restores the process resource manager to the initial state.
- cr \<name\> \<priority\> <br>
  Creates a new process \<name\> at priority level \<priority\>. Name is a single character, and priority is 1 or 2. 0 is saved for the Init process.
- de \<name\> <br>
  Destroys the process \<name\> and all its decendants
- req \<rName\> \<#ofUnits\> <br>
  Current running process requests \<#ofUnits\> of the resource \<rName\> <br> 
  Resources: "R1" has 1 unit, "R2" has 2 units, "R3" has 3 units, and "R4" has 4 units
- rel \<name\> \<#ofUnits\> <br>
  Current running process releases \<#ofUnits\> of the resource \<rName\> <br>
- to <br>
  Invokes a timeout
  
<b>Output File</b>:
- The output file contains a separate line for each test sequence.
- Each line starts with "init" and is followed by a series of single character process names separated by blanks. This represents the order in which the processes run based on the input file. 
- Errors caused by illegal commands are represented by "error". This can include but isn't limited to: trying to create a process with a nonexistent priority level, trying to release more resources than the current process is holding on to, trying to request more resource units than that resource has in total (free and taken), etc.
- It is possible for processes to become deadlocked in this implementation.
  
To run after building: ProcessResourceManager.exe \<inputFile\> \<outputFile\>
