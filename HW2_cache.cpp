//
// Created by nater on 5/6/2019.
//

#include "HW2_cache.h"

#define VICTIM_CACHE_SIZE 4

void access_cache(char operation, int address)
{
    /*if (address exists in L1)
     *{
     *      update lru in L1
     *      update the trace data
     *      update dirty bit if needed
     *else if (address exists in L2)
     *      update lru in L2
     *      if (write allocate and write-op)
     *          find empty place or remove some block in L1
     *          copy to L1 (don't forget to move the dirty bit if needed)
     *          update lru in L1
     *      else if(write-op)
     *          update dirty bit if needed
     *      update the trace data
     *else
     *      if(victim and address exists in victim) read or write and write-allocate
     *          find empty place or remove some block in L2
     *          if removed
     *              snoop in L1
     *          find empty place or remove some block in L1
     *          copy to L2 (don't forget to move the dirty bit if needed)
     *          update lru in L2
     *          copy to L1 (don't forget to move the dirty bit if needed)
     *          update lru in L1
     *          update the trace data
     *      else    --fetching from mem
     *          find empty place or remove some block in L2
     *          if removed
     *              snoop in L1
     *          find empty place or remove some block in L1
     *          copy to L2 (don't forget to move the dirty bit if needed)
     *          update lru in L2
     *          copy to L1 (don't forget to move the dirty bit if needed)
     *          update lru in L1
     *          update the trace data
     */
}

// MRU = 0  LRU = array-size
// search: true = hit
/**
 * Snoop - checks higher cache lever (L1) for identical blocks in L1 and L2
 * If exists in L1, marks it as invalid
 * !! does not mark L2 block as invalid !!
 * @param address
 */
void cache_sys::snoop(int address)
{
    int block_head =  address - (address % block_size);
    int set_and_tag = address >> this->BSize;
    int tag = set_and_tag >> this->L1Assoc;
    int set = set_and_tag % this->L1_way_entries_num;

    for (int i = 0; i < this->L1_way_entries_num; i++)
    {
        if (this->L1[i][set].tag == tag)
        {
            this->L1[i][set].invalid = true;
            //update the lru
            for (int j = 0; j < this->L1_way_entries_num; j++)
            {
                if(this->L1[j][set].LRU > this->L1[i][set].LRU)
                    (this->L1[j][set].LRU)--;
            }
            return;
        }
    }
};

int cache_sys::find_place(int level, int address)
{
    if (level == 1 || level == 2)
    {
        int set_and_tag = address >> this->BSize;
        int set = set_and_tag % this->L1_way_entries_num;
        block** cache;
        int num_of_ways;
        if (level == 1)
        {
            cache = this->L1;
            num_of_ways = this->L1_way_num;
        }
        else
        {
            cache = this->L2;
            num_of_ways = this->L2_way_num;
        }
        //search for empty place
        for (int i = 0; i < num_of_ways; i++)
        {
            if (cache[i][set].invalid == true)
                return i;
        }

        //else select a block to replace
        for (int i = 0; i < num_of_ways; i++)
        {
            if (cache[i][set].LRU == num_of_ways - 1)
            {
                if (level == 2)
                    this->snoop(address);
                return i;
            }
        }
    }
    else    //victim cache
    {
        //search for empty place
        for (int i = 0; i < 4; i++)
        {
            if (this->victimCache[i].invalid == true)
                return i;
        }

        //else select a block to replace
        for (int i = 0; i < 4; i++)
        {
            if (this->victimCache[i].LRU == VICTIM_CACHE_SIZE - 1)    //victim cache is always size 4 blocks
                return i;
        }
    }
}

int remove(int level)
{
    /*
     * find the lru and invalidate it
     */
}


/**
 * Given a cache level and an minimum LRU, update all LRU's above give min
 * @param level: cache level
 * @param min_lru: all LRU rates above need to be decreased by 1
 */
void cache_sys::update_lru(int level, int min_lru)
{
    block **cache;
    int num_of_ways;
    switch (level) {
        case 1:
            cache = this->L1;
            num_of_ways = this->L1_way_num;
            //increase L1 access attempts
            this->L1Access++;
            break;
        case 2:
            cache = this->L2;
            num_of_ways = this->L2_way_num;
            //increase L1 access attempts
            this->L2Access++;
            break;
    }
    for (int i = 0; i < num_of_ways; i++) {
        if (cache[i][set].tag == set){
        if (level == 1)
        this->L1Hit++;
            else
            this->L2Hit++;
                return i;
                }
                return -1;
    }
    /*
     * update the lru of all the  with at least min_lru
     * {after inserting)
     *
     * if we want to update all the blocks min_lru = 0
     * if we want to update after accessing one of the entries
     */
}


/**
 * Finds address within given cache level, updates number of access to each level, and updates if hit
 * @param level: cache level to check
 * @param address: address to find
 * @return
 */
bool cache_sys::search_in_cache(int level, int address)
{
    int set_and_tag = address >> this->BSize;
    block **cache;
    int num_of_ways;
    int set;
        switch (level) {
            case 1:
                cache = this->L1;
                num_of_ways = this->L1_way_num;
                set = set_and_tag % this->L1_way_entries_num;
                //increase L1 access attempts
                this->L1Access++;
                break;
            case 2:
                cache = this->L2;
                num_of_ways = this->L2_way_num;
                set = set_and_tag % this->L2_way_entries_num;
                //increase L1 access attempts
                this->L2Access++;
                break;
        }
        //search for empty place
        for (int i = 0; i < num_of_ways; i++) {
            if (cache[i][set].tag == set){
                if (level == 1)
                    this->L1Hit++;
                else
                    this->L2Hit++;
                return i;
        }
            return -1;
    }
}