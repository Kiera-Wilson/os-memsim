#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->type = DataType::FreeSpace;
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

void Mmu::addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
{
    int i;
    Process *proc = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }

    Variable *var = new Variable();
    var->name = var_name;
    var->type = type;
    var->virtual_address = address;
    var->size = size;
    if (proc != NULL)
    {
        proc->variables.push_back(var);
    }
}

void Mmu::print()
{
    int i, j;


    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        int pid = _processes[i]->pid;
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            // TODO: print all variables (excluding <FREE_SPACE> entries)
            std::string varName = _processes[i]->variables[j]->name;
            if(varName!= "<FREE_SPACE>"){
                uint32_t virtualAddress = _processes[i]->variables[j]->virtual_address;
                int varSize = _processes[i]->variables[j]->size;
                printf(" %4d | %-13s | 0x%08X | %-10d \n", pid, varName.c_str(), virtualAddress, varSize);
            }

        }
    }
}

void Mmu::printProcesses(){
    std::cout << " PID " <<std::endl;
    std::cout << "------" << std::endl;
    //loop through and print the pid of each process
    for(int i =0; i < _processes.size(); i++){
        std::cout << _processes[i]->pid <<std::endl;
    }
}

void Mmu::printVarValue(){
    //take in the var name
    //find the variable
    //variable size/ variable type = num elements
    //print up to the first 4 elements
    //if there are more than 4 elements, print the elemen total after the 1st 4

}

uint32_t Mmu::getMemLeft(){
    uint memleft = 67108864;
    for(int i =0; i < _processes.size();i++){
        std::vector<Variable*> currProcVars;
        currProcVars = getVariables(_processes[i]->pid);
        for(int j=0; j<currProcVars.size(); j++){
            memleft = memleft - currProcVars[j]->size;
        }
    }
    return memleft;
}


int Mmu::findOpenPageSpace(uint32_t pid, int page_size, int page_number, uint32_t virtual_address){
    int pageSpace = page_size;
    for(int i = 0; i<_processes.size(); i++){
        Process *currProc =_processes[i];
        if(currProc->pid==pid){
            for(int k = 0; k <currProc->variables.size(); k++){
                //chek if we are still on the same page & if the variable has a size
                int currPageNum = currProc->variables[k]->virtual_address / page_size;
                if(currProc->variables[k]->name !="<FREE_SPACE>" && currPageNum==page_number){
                    pageSpace = pageSpace + currProc->variables[k]->size;
                }

            }
        }
       
    }
    return pageSpace;
}

/*uint32_t Mmu::getVariableVirtualAddress(int pid){
    for(int i =0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            return _processes[i];
        }
    }
}*/
bool Mmu::isValidPID(int pid){
    bool isValid =false;
    for(int i =0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
            isValid = true;;
        }
    }
    return isValid;
}

bool Mmu::isValidVarName(int pid, std::string variable_name){
    std::vector<Variable*> currProcVars;
    currProcVars = getVariables(pid);
    bool isValid = true;

    for(int i =0; i< currProcVars.size(); i++){
        //if a variable alread has that name it is not valid
        if(currProcVars[i]->name == variable_name){
            isValid = false;
        }
    }
    return isValid;
}

bool Mmu::isValidSetVar(int pid, std::string variable_name){
    std::vector<Variable*> currProcVars;
    currProcVars = getVariables(pid);
    bool isValid = false;

    for(int i =0; i< currProcVars.size(); i++){
        //if a variable alread has that name it is not valid
        if(currProcVars[i]->name == variable_name){
            isValid = true;
        }
    }
    return isValid;
}



std::vector<Variable*> Mmu::getVariables(int pid){
    std::vector<Variable*> currProcVars;
    Process *currProc = NULL;

    for(int i =0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){
           currProc = _processes[i];
           break;
        }
    }
    if(currProc == NULL){
        //std::cout << "Get variables: process not found";
        //moved this error message to main.cpp
    }else{
        for (int j =0; j<currProc->variables.size(); j++){
            currProcVars.push_back(currProc->variables[j]);
        }
    }
    return currProcVars;
}

DataType Mmu::getVarDataType(int pid, std::string var_name){
    std::vector<Variable*> currProcVars;
    currProcVars = getVariables(pid);
    DataType dataType;
        for(int i =0; i< currProcVars.size(); i++){
        //if a variable alread has that name it is not valid
        if(currProcVars[i]->name == var_name){
            dataType = currProcVars[i]->type;
        }
    }
    return dataType;
}


int Mmu::getVariableSize(DataType varType)
{
    int dataTypeSize =0;
    //check which type it is and then return the size
    if(varType== DataType::Char){
        dataTypeSize = 1;
    }else if(varType==DataType::Short){
        dataTypeSize = 2;
    }else if(varType==DataType::Float ||varType == DataType::Int ){
        dataTypeSize = 4;
    }else if(varType==DataType::Double ||varType == DataType::Long ){
        dataTypeSize = 8;
    }


    return dataTypeSize;
}

void Mmu::removeVariable(int pid, int pos){
    //find the process using pid
    Process *currProc = NULL;
    std::vector<Variable*> currProcVars;
    for(int i=0; i++, i< _processes.size(); i++){
        if(_processes[i]->pid ==pid){
            currProc = _processes[i];
        }
    }
    if(currProc == NULL){
        std::cout << "error: process not found" << std::endl;
    }else{ //remove the variable at the given positon
        //currProcVars = getVariables(pid); //idk if I need this
        currProc->variables.erase(currProc->variables.begin()+pos);
    }
   
}

uint32_t Mmu::insertVariableOnPage(int pid, int page_size, std::string variable_name, uint32_t num_elements, DataType type)
{
    std::vector<Variable*> currProcVars;
    currProcVars = getVariables(pid);
    int dataTypeSize = getVariableSize(type);
    int variableSize = dataTypeSize * num_elements;
    //std::cout << "dataTypeSize:  " << dataTypeSize << std::endl;
    //std::cout << "variable size after assignment: " << variableSize << std::endl;
    //var to hold the new variables address
    uint32_t newVariableAddress= 0;
    for(int i =0; i<currProcVars.size();i++){
        //if the variable is free space
        if(currProcVars[i]->name == "<FREE_SPACE>")
        {
            uint32_t memoryLeft = getMemLeft();
            if(memoryLeft- variableSize >= 0){
             //check if the free space is large enough to hold the new variable
                if(currProcVars[i]->size>variableSize){
                    int pageNumber = currProcVars[i]->virtual_address/ page_size;
                    //find out how much space there is between the start of the free space and end of the page
                    int pageSpaceLeft = page_size -(currProcVars[i]->virtual_address/pageSpaceLeft);
                // std::cout << "space left on page "<< pageSpaceLeft <<std::endl;
                    //offset will tell us if the elements will fit evenly on a page or if they will cross over
                    int offset = (page_size - (currProcVars[i]->virtual_address/page_size))%dataTypeSize;
                    //if there enough page space to add the entire variable
                    if(pageSpaceLeft > variableSize || offset == 0){
                        //add the variable, we dont have to worry about elements crossing over bc theres enough room on the page for the whole variable
                        newVariableAddress = currProcVars[i]->virtual_address;
                    // std::cout << "Old free space address which is new variable address" << newVariableAddress << std::endl;

                        //add the variable
                        addVariableToProcess(pid, variable_name, type, variableSize, newVariableAddress);
                        //change the freespace address to start after the newly inserted variable
                        currProcVars[i]->virtual_address = newVariableAddress + variableSize;
                        //std::cout << "Variable size: " << variableSize << std::endl;
                        //std::cout << "New Free space adresss: " << currProcVars[i]-> virtual_address << std::endl;
                        //change size of free space
                        currProcVars[i]->size= currProcVars[i]->size - variableSize;
                        //TODO: remove free space if the size is now 0
                    }else if(offset !=0) { //if the elements dont fit evenly, we need to add offset to virtual address
                        newVariableAddress = currProcVars[i]->virtual_address + offset;
                        //add variable to process with new offset address
                        addVariableToProcess(pid, variable_name, type, variableSize, newVariableAddress);
                        //variable to hold where the original free space variable started
                        uint32_t freeSpaceAddressHold = currProcVars[i]->virtual_address;
                        //add a new free space variable to take the place of the offset
                        addVariableToProcess(pid, "FREE_SPACE", DataType::FreeSpace, offset, freeSpaceAddressHold);
                        //change the freespace address to start after the newly inserted variable
                        currProcVars[i]->virtual_address = newVariableAddress + variableSize;
                        //change size of free space
                        currProcVars[i]->size= currProcVars[i]->size - variableSize;
                    }
                    //if the free space was the same size as the newly added variable
                    if(currProcVars[i]->size <= 0){
                        //free space has a size of 0 or less so just get rid of it
                        removeVariable(pid, i);
                    }
                }
            }else{
                //it will exceed memory allocation
                std::cout << "error: allocation would exceed system memory" <<std::endl;
            }
        }

    }

    return newVariableAddress;
}