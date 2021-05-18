#include <iostream>
#include <string>
#include <cstring>
#include <bits/stdc++.h>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);
int splitString(std::string text, char d, char **result);
void allocateArrayOfCharArrays(char ***array_ptr, size_t array_length, size_t item_size);
DataType getDataType(char *input);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint32_t mem_size = 67108864;
    void *memory = malloc(mem_size); // 64 MB (64 * 1024 * 1024)
    char **arg_list;

    // Create MMU and Page Table
    Mmu *mmu = new Mmu(mem_size);
    PageTable *page_table = new PageTable(page_size);

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline(std::cin, command);
    while (command != "exit")
    {
        // Handle command
        // TODO: implement this!
        allocateArrayOfCharArrays(&arg_list, 10, 1250);
        int arg_listSize = splitString(command, ' ', arg_list); //split up command by spaces
        //1. create
        if (strcmp(arg_list[0], "create") == 0)
        {
            //void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
            createProcess(std::stoi(arg_list[1]), std::stoi(arg_list[2]), mmu, page_table);

        } //2. allocate
        else if (strcmp(arg_list[0], "allocate") == 0)
        {
            //void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
            DataType dataType = getDataType(arg_list[3]);
            allocateVariable(std::stoi(arg_list[1]), arg_list[2], dataType, std::stoi(arg_list[4]), mmu, page_table);
        } //3. set
        else if (strcmp(arg_list[0], "set") == 0)
        {
           
            //void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
            uint32_t pid = std::stoi(arg_list[1]);
            std::string var_name = arg_list[2];
            bool validVarName = mmu->isValidSetVar(pid, var_name);
            bool validPID = mmu->isValidPID(pid);
            //if it is not a valid process
            if (!validPID)
            {
                //print error message
                std::cout << "error: process does not exist" << std::endl;
            }else if (!validVarName)
            {
                //print error message
                std::cout << "error: variable not found " << std::endl;
            }else{
                uint32_t offset = std::stoi(arg_list[3]);
                DataType varType = mmu->getVarDataType(pid, var_name);
                int typeSize = mmu->getVariableSize(varType);

                for (int i = 4; i < arg_listSize; i++)
                {
                    if (varType == DataType::Char)
                    {
                        char value = *arg_list[i];
                        setVariable(pid, var_name, offset, &value, mmu, page_table, memory);
                    }
                    else if (varType == DataType::Short)
                    {
                        int conversionHold = std::stoi(arg_list[i]);
                        short value = short(conversionHold);
                        setVariable(pid, var_name, offset, &value, mmu, page_table, memory);
                    }
                    else if (varType == DataType::Float)
                    {
                        float value = std::stof(arg_list[i]);
                        setVariable(pid, var_name, offset, &value, mmu, page_table, memory);
                    }
                    else if (varType == DataType::Int)
                    {
                        int value = std::stoi(arg_list[i]);
                        setVariable(pid, var_name, offset, &value, mmu, page_table, memory);
                    }
                    else if (varType == DataType::Double)
                    {
                        double value = std::stod(arg_list[i]);
                        setVariable(pid, var_name, offset, &value, mmu, page_table, memory);
                    }
                    else if (varType == DataType::Long)
                    {
                        long value = std::stol(arg_list[i]);
                        setVariable(pid, var_name, offset, &value, mmu, page_table, memory);
                    }
                    offset = offset + typeSize;
                }
            }

        } //4. free
        else if (strcmp(arg_list[0], "free") == 0)
        {

        } //5. Terminate
        else if (strcmp(arg_list[0], "terminate") == 0)
        {

        } //6. print
        else if (strcmp(arg_list[0], "print") == 0)
        {
        }
        else
        {
            std::cout << "error: command not recognized";
        }

        // Get next command
        std::cout << "> ";
        std::getline(std::cin, command);
    }

    // Cean up
    free(memory);
    delete mmu;
    delete page_table;

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std::endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std::endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std::endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std::endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std::endl;
    std::cout << "  * print <object> (prints data)" << std::endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std::endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std::endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std::endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std::endl;
    std::cout << std::endl;
}

void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - create new process in the MMU
    uint32_t newProcess;
    newProcess = mmu->createProcess();
    //   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    allocateVariable(newProcess, std::string("<TEXT>"), DataType::Char, text_size, mmu, page_table);
    allocateVariable(newProcess, std::string("<GLOBALS>"), DataType::Char, data_size, mmu, page_table);
    allocateVariable(newProcess, std::string("<STACK>"), DataType::Char, 65536, mmu, page_table);
    //   - print pid
    std::cout << newProcess << std::endl;
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    bool validPID = mmu->isValidPID(pid);
    bool validVarName = mmu->isValidVarName(pid, var_name);
    //if it is not a valid process
    if (!validPID)
    {
        //print error message
        std::cout << "error: process does not exist" << std::endl;
        return;
    }
    else if (!validVarName)
    { //check if variable already exists or not
        //print error message
        std::cout << "error:variable already exists" << std::endl;
        return;
    }
    else
    {
        int page_size = page_table->getPageSize();
        int dataTypeSize = mmu->getVariableSize(type);
        int variableSize = dataTypeSize * num_elements;
        //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
        //   - if no hole is large enough, allocate new page(s)
        //   - insert variable into MMU
        uint32_t newVariableAddress = mmu->insertVariableOnPage(pid, page_size, var_name, num_elements, type);
        //Potentially add new entries to Page Table
        int firstPage = newVariableAddress / page_size;
        //add the size to find out where the variable ends
        int lastPage = (newVariableAddress + variableSize - 1) / page_size;
        //add new pages
        for (int i = firstPage; i < lastPage; i++)
        {
            page_table->addEntry(pid, i);
        }
        //   - print virtual memory address
        if (var_name != "<TEXT>" && var_name != "<GLOBALS>" && var_name != "<STACK>")
        {
            std::cout << "New Var Address: " << newVariableAddress << std::endl;
            std::cout << "Var Name : " << var_name << std::endl;
        }
        mmu->addVariableToProcess(pid, var_name, type, variableSize, newVariableAddress);
    }
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    bool validPID = mmu->isValidPID(pid);
    bool validVarName = mmu->isValidSetVar(pid, var_name);
    if (!validPID)
    {
        //print error message
        std::cout << "error: process does not exist" << std::endl;
        return;
    }
    else if (!validVarName)
    {
        //print error message
        std::cout << "error: variable not found " << std::endl;
        return;
    }
    //get the processes variables
    std::vector<Variable *> processVariables;
    processVariables = mmu->getVariables(pid);
    //var to holf virtual address
    uint32_t virtualAddress = 0;
    DataType varDataType;
    //find the virtual address of the variable
    for (int i = 0; i < processVariables.size(); i++)
    {
        if (processVariables[i]->name == var_name)
        {
            //if we found the variable, store vistual address of it
            virtualAddress = processVariables[i]->virtual_address;
            varDataType = processVariables[i]->type;
        }
    }
    //get the size of the data type
    int dataTypeSize = mmu->getVariableSize(varDataType);
    //   - look up physical address for variable based on its virtual address / offset
    uint32_t newVirtAddress = virtualAddress + (offset * dataTypeSize);
    int physicalAddress = page_table->getPhysicalAddress(pid, newVirtAddress);
    //   - insert `value` into `memory` at physical address
    memcpy((char *)memory + physicalAddress, value, dataTypeSize);
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array)
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process
}

DataType getDataType(char *input)
{
    DataType dataType;
    std::cout << "input is: " << input << std::endl;
    if (strcmp(input, "char") == 0 || strcmp(input, "Char") == 0)
    {
        dataType = DataType::Char;
    }
    else if (strcmp(input, "double") == 0 || strcmp(input, "Double") == 0)
    {
        dataType = DataType::Double;
    }
    else if (strcmp(input, "float") == 0 || strcmp(input, "Float") == 0)
    {
        dataType = DataType::Float;
    }
    else if (strcmp(input, "int") == 0 || strcmp(input, "Int") == 0)
    {
        dataType = DataType::Int;
    }
    else if (strcmp(input, "long") == 0 || strcmp(input, "Long") == 0)
    {
        dataType = DataType::Long;
    }
    else if (strcmp(input, "short") == 0 || strcmp(input, "Short") == 0)
    {
        dataType = DataType::Short;
    }
    return dataType;
}

int splitString(std::string text, char d, char **result)
{
    enum states
    {
        NONE,
        IN_WORD,
        IN_STRING
    } state = NONE;

    int i;
    std::vector<std::string> list;
    std::string token;
    for (i = 0; i < text.length(); i++)
    {
        char c = text[i];
        switch (state)
        {
        case NONE:
            if (c != d)
            {
                if (c == '\"')
                {
                    state = IN_STRING;
                    token = "";
                }
                else
                {
                    state = IN_WORD;
                    token = c;
                }
            }
            break;
        case IN_WORD:
            if (c == d)
            {
                list.push_back(token);
                state = NONE;
            }
            else
            {
                token += c;
            }
            break;
        case IN_STRING:
            if (c == '\"')
            {
                list.push_back(token);
                state = NONE;
            }
            else
            {
                token += c;
            }
            break;
        }
    }
    if (state != NONE)
    {
        list.push_back(token);
    }

    for (i = 0; i < list.size(); i++)
    {
        strcpy(result[i], list[i].c_str());
    }
    result[list.size()] = NULL;
    return list.size();
}

void allocateArrayOfCharArrays(char ***array_ptr, size_t array_length, size_t item_size)
{
    int i;
    *array_ptr = new char *[array_length];
    for (i = 0; i < array_length; i++)
    {
        (*array_ptr)[i] = new char[item_size];
    }
}
