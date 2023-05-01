#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <map>
using std::map;
using std::vector;

enum allocate_enum
{
    writealloc,
    nowritealloc
};
enum write_enum
{
    writethrough,
    writeback
};
enum evict_type_enum
{
    LRU,
    FIFO
};
enum commands
{
    LOAD,
    STORE,
    UNDEF
};

struct Address
{
    unsigned idx;
    unsigned tag;
    unsigned cmd;
};

struct Param
{
    unsigned num_s;
    unsigned num_Blo;
    unsigned num_byBlo;
    unsigned Write_Alloc;
    unsigned Write_Thru;
    unsigned Evict_Type;
};

struct Stats
{
    unsigned long ttl_loads = 0;
    unsigned long ttl_stores = 0;
    unsigned long load_hits = 0;
    unsigned long load_misses = 0;
    unsigned long store_hits = 0;
    unsigned long store_misses = 0;
    unsigned long ttl_cycles = 0;
};

struct Slots
{
    unsigned tag = 0;
    bool valid = false;
    unsigned long fifo = 0;
    unsigned long lru = 0;
    bool dirty = false;
};

struct Sets
{
    vector<Slots> slots;
    map<unsigned, Slots *> pointer;
};


class Cache
{
private:
    vector<Sets> sets;
    Param param;
    Stats stat;
    unsigned long count;

public:
    Cache(Param param);

    unsigned log2(unsigned val);

    Address Separate_Address(unsigned address);

    void miss(Address address_data);

    unsigned check_for_slot(Address address_data);

    void run();
};

#endif // CACHE_H
