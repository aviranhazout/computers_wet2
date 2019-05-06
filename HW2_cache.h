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


    cache_sys(int MemCyc, int BSize, int L1Size, int L2Size, int L1Assoc, int L2Assoc,
              int L1Cyc, int L2Cyc, int WrAlloc, int VicCache) : MemCyc(MemCyc), BSize(BSize),
              L1Size(L1Size), L2Size(L2Size), L1Assoc(L1Assoc), L2Assoc(L2Assoc), L1Cyc(L1Cyc),
              L2Cyc(L2Cyc), WrAlloc(WrAlloc), VicCache(VicCache), totalTime(0), totalAcc(0){

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

    };



};

void access_cache(char operation, int address);


#endif //COMPUTERS_WET2_HW2_CACHE_H
