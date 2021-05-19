#include "pagetable.h"

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
}

PageTable::~PageTable()
{
}

std::vector<std::string> PageTable::sortedKeys()
{
    std::vector<std::string> keys;

    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        keys.push_back(it->first);
    }

    std::sort(keys.begin(), keys.end(), PageTableKeyComparator());

    return keys;
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    std::map<std::string, int>::iterator it;

    int frame = 0; 
    std::vector<std::string> tableKeys = sortedKeys();
    int keySize = tableKeys.size();
    bool frames[keySize];
    // Find free frame
    // TODO: implement this!
    if (_table.count(entry) > 0)
    {
        //do nothing bc an entry for this page already exists
        
    }else{
        //loop through table, set mapped entries to true
        for( it = _table.begin(); it != _table.end(); it++){
            frames[it->second] = true;
        }
        //loop through frames
        for(int i =0; i < keySize; i++){
            //if a frame didnt get set to true its not being used
            if(frames[i]!= true){
                frame = i;
            }
        }

    }
    _table[entry] = frame;
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{
    // Convert virtual address to page_number and page_offset
    // TODO: implement this!
    int page_number = 0;
    int page_offset = 0;
    page_number = virtual_address / _page_size;
    page_offset = virtual_address % _page_size;

    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    if (_table.count(entry) > 0)
    {
        //calculate physical address
        address = (_table.at(entry) * _page_size) + page_offset;
        
    }

    return address;
}

void PageTable::print()
{
    int i;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    std::vector<std::string> keys = sortedKeys();

    for (i = 0; i < keys.size(); i++)
    {
        // TODO: print all pages
        std::string keyString = keys.at(i);
        //find the bar seperating the pid from the page #
        size_t seperatorPos = keyString.find("|");
        //extract the PID
        std::string stringPid = keyString.substr(0, seperatorPos);
        int intPid = std::stoi(stringPid);
        //extract the page number
        std::string stringPageNum = keyString.substr(seperatorPos +1);
        int intPageNum = std::stoi(stringPageNum);
        //find the frame number
        int frameNum = _table.at(keys[i]);
        //print
        printf(" %4d | %11d | %12d \n", intPid, intPageNum, frameNum);
    }
}

int PageTable::getPageSize()
{
    return _page_size;
}
