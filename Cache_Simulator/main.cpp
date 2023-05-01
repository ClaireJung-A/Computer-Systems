#include <iostream>
#include <sstream>
#include <string>
#include "cache.h"

using std::endl;
using std::string;
using std::cerr;
using std::stoi;
using std::exception;

bool isPower(unsigned val) {
    return (val != 0) && ((val & (val - 1)) == 0);
}

bool is_valid_int(string input){
    try {
        stoi(input);
    } catch (exception const &e) {
        cerr << "Number of set must be a positive power of 2!" << endl;
        return false;
    }
    return true;
}

// Check for Cache size
bool get_num_sets(Param *params, string numOfSet) {
    if (!is_valid_int(numOfSet)) {
        return false;
    }
    params->num_s = stoi(numOfSet);
    if (!isPower(params->num_s) || params->num_s <= 0) {
        cerr << "The given number of sets is invalid!" << endl;
        return false;
    }
    return true;
}

// Check for Sets size
bool get_num_blocks(Param *params, string num_blocks) {
    if (!is_valid_int(num_blocks)) {
        return false;
    }
    params->num_Blo = stoi(num_blocks);
    if (!isPower(params->num_Blo) || params->num_Blo <= 0) {
        cerr << "The given number of sets is invalid!" << endl;
        return false;
    }
  return true;
}

// Check for Block size
bool get_num_bytes_per_block(Param *params, string num_bytes_per_block) {
    if (!is_valid_int(num_bytes_per_block)) {
        return false;
    } else {
        params->num_byBlo = stoi(num_bytes_per_block);
    }
    if (params->num_byBlo < 4 || !isPower(params->num_byBlo))
    {
        // The number of bytes per block must be a power of 2 greater than 4
        cerr << "The given number of bytes in each block is invalid!" << endl;
        return false;
    }
    return true;
}

// Check for Write allocate
    bool get_write_allocate(Param *params, string write_allocate) {
        if (write_allocate.compare("write-allocate") == 0) {
            params->Write_Alloc = writealloc;
        } else if (write_allocate.compare("no-write-allocate") == 0) {
            params->Write_Alloc = nowritealloc;
        } else {
            cerr << "Write_allocate must be either Write_Alloc or no-write-allocate!" << endl;
            return false;
        }
        return true;
    }

// Check for Write type validation
    bool get_write_through(Param *params, string write_through) {
        if (write_through.compare("write-through") == 0) {
            params->Write_Thru = writethrough;
        } else if (write_through.compare("write-back") == 0) {
            if (params->Write_Alloc == nowritealloc)
            {
                // Cannot specify both no-write-allocate and write-back
                cerr << "You cannot specify both no-write-allocate and write-back!" << endl;
                return false;
            }
            params->Write_Thru = writeback;
        } else {
            cerr << "Write_through must be either Write_Thru or write-back!" << endl;
            return false;
        }
        return true;
    }

// Check for Evict type
    bool get_evict_type(Param *params, string evict_type) {
        if (evict_type.compare("lru") == 0) {
            params->Evict_Type = LRU;
        } else if (evict_type.compare("fifo") == 0) {
            params->Evict_Type = FIFO;
        } else {
            cerr << "Evict type must be either lru or fifo!" << endl;
            return false;
        }
        return true;
    }

    int main(int argc, char *argv[]) {
        //if not 6 parameters, return 1 and exit
        if (argc != 7) {
            cerr << "Invalid Argument Number" << endl;
            return 1;
        }

        Param params;
        if (!get_num_sets(&params, argv[1]))
            return 1;
        if (!get_num_blocks(&params, argv[2]))
            return 1;
        if (!get_num_bytes_per_block(&params, argv[3]))
            return 1;
        if (!get_write_allocate(&params, string(argv[4])))
            return 1;
        if (!get_write_through(&params, string(argv[5])))
            return 1;
        if (!get_evict_type(&params, string(argv[6])))
            return 1;

        Cache cache = Cache(params);
        cache.run();

        return 0;
    }


