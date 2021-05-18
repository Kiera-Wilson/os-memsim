#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>

enum DataType : uint8_t {FreeSpace, Char, Short, Int, Float, Long, Double};

typedef struct Variable {
    std::string name;
    DataType type;
    uint32_t virtual_address;
    uint32_t size;
} Variable;

typedef struct Process {
    uint32_t pid;
    std::vector<Variable*> variables;
} Process;

class Mmu {
private:
    uint32_t _next_pid;
    uint32_t _max_size;
    std::vector<Process*> _processes;

public:
    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess();
    void addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address);
    uint32_t getVariableVirtualAddress(int pid);
    int findOpenPageSpace(uint32_t pid, int page_size, int page_number, uint32_t virtual_address);
    uint32_t insertVariableOnPage(int pid, int page_size, std::string variable_name, uint32_t num_elements, DataType type);
    int getVariableSize(DataType varType);
    std::vector<Variable*> getVariables(int pid);
    void removeVariable(int pid, int pos);
    bool isValidPID(int pid);
    bool isValidVarName(int pid, std::string variable_name);
    bool isValidSetVar(int pid, std::string variable_name);
    DataType getVarDataType(int pid, std::string var_name);
    void print();
};

#endif // __MMU_H_
