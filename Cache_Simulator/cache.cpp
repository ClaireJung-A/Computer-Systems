#include "cache.h"
#include <iostream>
#include <sstream>
#include <map>

using std::cin;
using std::cout;
using std::endl;
using std::string;

// constructor - make sets and slots
Cache::Cache(Param params) {
  this->param = params;

  for (unsigned i = 0; i < params.num_s; i++) {
    Sets set;
    for (unsigned j = 0; j < params.num_Blo; j++) {
      Slots slot;
      set.slots.push_back(slot);
    }
    this->sets.push_back(set);
  }
}

unsigned Cache::log2(unsigned memory) {
    unsigned result = 0;
    while (memory >>= 1) result++;
    return result;
}

Address Cache::Separate_Address(unsigned address) {
    unsigned offset_size = log2(this->param.num_byBlo);
    unsigned index_size = log2(this->param.num_s);

    address >>= offset_size;
    unsigned idxmodify = (1 << index_size) - 1;
    unsigned index = address & idxmodify;
    address >>= index_size;

    unsigned tagmodify = (1 << (32 - index_size - offset_size)) - 1;
    unsigned tag = (address & tagmodify);

    Address addy = {index, tag, UNDEF};
    return addy;
}

void Cache::miss(Address address_data) {
    this->stat.ttl_cycles += 1 + ((this->param.num_byBlo >> 2) * 100);

    vector<Slots> *slots = &this->sets.at(address_data.idx).slots;
    unsigned idx;

    bool has_invalid = false;
    for (unsigned i = 0; i < this->param.num_Blo; i++) {
        if (!slots->at(i).valid) {
            idx = i;
            has_invalid = true;
            break;
        }
    }
    if (!has_invalid) {
        idx = check_for_slot(address_data);
    }

    slots->at(idx).tag = address_data.tag;
    slots->at(idx).valid = true;
    if (this->param.Evict_Type == LRU) {
        slots->at(idx).lru = count;
    } else {
        slots->at(idx).fifo = count;
    if (this->param.Write_Thru == writeback && address_data.cmd == STORE) {
        slots->at(idx).dirty = true;
    }
    }
    this->sets.at(address_data.idx).pointer.insert(std::pair<unsigned, Slots *>(address_data.tag, &slots->at(idx)));
}

unsigned Cache::check_for_slot(Address address_data) {
    vector<Slots> *slots = &this->sets.at(address_data.idx).slots;
    unsigned idx = 0;

    if (this->param.Evict_Type == LRU) {
        for (unsigned i = 1; i < this->param.num_Blo; i++) {
            if (slots->at(i).lru < slots->at(idx).lru) {
                idx = i;
            }
        }
    } else {
        for (unsigned j = 1; j < this->param.num_Blo; j++) {
            if (slots->at(j).fifo < slots->at(idx).fifo) {
                idx = j;
            }
        }
    } if (slots->at(idx).dirty) {
        this->stat.ttl_cycles += ((this->param.num_byBlo / 4) * 100);
    }
    this->sets.at(address_data.idx).pointer.erase(slots->at(idx).tag);
    return idx;
}

void Cache::run()
{
    string first_field;
    string second_field_0x;
    unsigned adjusted_address;
    string third_field;
    while (cin >> first_field >> second_field_0x >> third_field) {
        std::stringstream ss; // convert hex string to unsigned
        ss << std::hex << second_field_0x.substr(2, 8);
        ss >> adjusted_address;

        Address address_data = Separate_Address(adjusted_address);

        Sets *pointer = &this->sets.at(address_data.idx);
        map<unsigned, Slots *> *cursor = &pointer->pointer;
        map<unsigned, Slots *>::iterator find = cursor->find(address_data.tag);

        if (first_field == "l") {
            this->stat.ttl_loads++;
            address_data.cmd = LOAD;

            if (find != cursor->end()) {
                this->stat.load_hits++;
                this->stat.ttl_cycles++;
                if (this->param.Evict_Type == LRU) {
                    find->second->lru = count;
                }
            } else {
                this->stat.load_misses++;
                miss(address_data);
            }
        } else if (first_field == "s") {
            this->stat.ttl_stores++;
            if (this->param.Write_Thru == writethrough) {
                this->stat.ttl_cycles += 100;
            }
            address_data.cmd = STORE;

            if (find != cursor->end()) {
                this->stat.store_hits++;
                this->stat.ttl_cycles++;
                if (this->param.Write_Thru == writeback) {
                    find->second->dirty = true;
                }
                if (this->param.Evict_Type == LRU) {
                    find->second->lru = count;
                }
            }
            else {
                this->stat.store_misses++;
                if (this->param.Write_Alloc == writealloc) {
                    miss(address_data);
                }
            }
        }
        this->count++;
    }

    cout << "Total loads: " << this->stat.ttl_loads << endl
        << "Total stores: " << this->stat.ttl_stores << endl
        << "Load hits: " << this->stat.load_hits << endl
        << "Load misses: " << this->stat.load_misses << endl
        << "Store hits: " << this->stat.store_hits << endl
        << "Store misses: " << this->stat.store_misses << endl
        << "Total cycles: " << this->stat.ttl_cycles << endl;
}
