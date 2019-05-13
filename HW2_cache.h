//
// Created by nater on 5/6/2019.
//

#ifndef COMPUTERS_WET2_HW2_CACHE_H
#define COMPUTERS_WET2_HW2_CACHE_H

#include <cmath>
//#include "HW2_cache.cpp"



class block{
public:
    int tag;
    bool dirty;
    int LRU;
    bool invalid;
    int address;
    block* next;
    block(): LRU(0), invalid(true), tag(0), dirty(false), address(0){};
};

class cache_sys {
public:
    block **L1;
    block **L2;
    block *victimCache;
    int totalTime;
    int totalAcc;
    int MemCyc;
    int BSize;
    int L1Size;
    int L2Size;
    int L1Assoc;
    int L2Assoc;
    int L1Cyc;
    int L2Cyc;
    int WrAlloc;
    int VicCache;
    int block_size;
    int L1_way_num;
    int L2_way_num;
    int L1_way_entries_num;
    int L2_way_entries_num;
    int L1Hit;
    int L1Access;
    int L2Hit;
    int L2Access;
    int vic_access;
    int mem_access;

    cache_sys(int MemCyc, int BSize, int L1Size, int L2Size, int L1Assoc, int L2Assoc,
              int L1Cyc, int L2Cyc, int WrAlloc, int VicCache) : MemCyc(MemCyc), BSize(BSize),
              L1Size(L1Size), L2Size(L2Size), L1Assoc(L1Assoc), L2Assoc(L2Assoc), L1Cyc(L1Cyc),
              L2Cyc(L2Cyc), WrAlloc(WrAlloc), VicCache(VicCache), totalTime(0), totalAcc(0), L1Hit(0),
              L1Access(0), L2Hit(0), L2Access(0), vic_access(0), mem_access(0){

        if(VicCache == 1){
            victimCache = new block[4];
        }

        int L1Ways = (int)pow(2,L1Assoc);
        int L1numTags = (int)pow(2,L1Size-L1Assoc-BSize);
        L1 = new block*[L1Ways];
        for (int i = 0; i < L1Ways; i++){
            L1[i] = new block[L1numTags];
            if (i > 0)
                L1[i-1]->next = L1[i];
        }
        int L2Ways = (int)pow(2,L2Assoc);
        int L2numTags = (int)pow(2,L2Size-L2Assoc-BSize);
        L2 = new block*[L2Ways];
        for (int i = 0; i < L2Ways; i++){
            L2[i] = new block[L2numTags];
            if (i > 0)
                L2[i-1]->next = L2[i];
        }
        block_size = (int)pow(2,BSize);
        L1_way_num = L1Ways;
        L2_way_num = L2Ways;
        L1_way_entries_num = L1numTags;
        L2_way_entries_num = L2numTags;
    };

    void write_back(int address);
    bool snoop(int address);
    int find_place(int level, int address);
    bool search_in_cache(int level, int address);
    void update_lru(int level, int min_lru, int address);
    int get_lru(int level, int address);
    void mark_dirty(int level, int address);
    void copy_data(block* from, block* to, int to_level);
    void copy_from_memory(block* to, int address);
    block* get_block(int level, int address);
    int get_set_from_address(int level, int address);
    int get_tag_from_address(int level, int address);
    int get_num_ways(int level);
    int get_num_entries(int level);
    void print_all();//for debug
    int get_way(int level, int address);
};

void access_cache(cache_sys& CS, char operation, int address);


#endif //COMPUTERS_WET2_HW2_CACHE_H

