//
// Created by nater on 5/6/2019.
//

#ifndef COMPUTERS_WET2_HW2_CACHE_H
#define COMPUTERS_WET2_HW2_CACHE_H

#include <cmath>

double L1MissRate = 0;
double L2MissRate = 0;
double avgAccTime = 0;


class block{
public:
    int tag;
    bool dirty;
    int LRU;
    bool invalid;

    block(): LRU(0), invalid(true){};
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
    int access_time;

    cache_sys(int MemCyc, int BSize, int L1Size, int L2Size, int L1Assoc, int L2Assoc,
              int L1Cyc, int L2Cyc, int WrAlloc, int VicCache) : MemCyc(MemCyc), BSize(BSize),
              L1Size(L1Size), L2Size(L2Size), L1Assoc(L1Assoc), L2Assoc(L2Assoc), L1Cyc(L1Cyc),
              L2Cyc(L2Cyc), WrAlloc(WrAlloc), VicCache(VicCache), totalTime(0), totalAcc(0), L1Hit(0),
              L1Access(0), L2Hit(0), L2Access(0){

        if(VicCache == 1){
            victimCache = new block[4];
        }

        int L1Ways = pow(2,L1Assoc);
        int L1numTags = pow(2,L1Size-L1Assoc-BSize);
        L1 = new block*[L1Ways];
        for (int i = 0; i < L1Ways; i++){
            L1[i] = new block[L1numTags];
        }

        int L2Ways = pow(2,L2Assoc);
        int L2numTags = pow(2,L2Size-L2Assoc-BSize);
        L2 = new block*[L2Ways];
        for (int i = 0; i < L2Ways; i++){
            L2[i] = new block[L2numTags];
        }
        block_size = pow(2,BSize);
        L1_way_num = L1Ways;
        L2_way_num = L2Ways;
        L1_way_entries_num = L1numTags;
        L2_way_entries_num = L2numTags;
        access_time = 0;
    };

    bool snoop(int address);
    int find_place(int level, int address);
    bool search_in_cache(int level, int address);
    void update_lru(int level, int min_lru);
    int get_lru(int level, int address);
    void mark_dirty(int level, int address);
    void copy_data(block* from, block* to);
    void get_block(int level, int address, block* ret);
};

void access_cache(cache_sys CS, char operation, int address);


#endif //COMPUTERS_WET2_HW2_CACHE_H
