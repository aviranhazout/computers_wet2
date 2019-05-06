//
// Created by nater on 5/6/2019.
//

#include "HW2_cache.h"

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

void cache_sys::snoop(int address)
{
    int block_head =  address - (adress % block_size);
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
            if (this->victimCache[i].LRU == num_of_ways - 1)
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

void update_lru(int level, int min_lru)
{
    /*
     * update the lru of all the  with at least min_lru
     * {after inserting)
     *
     * if we want to update all the blocks min_lru = 0
     * if we want to update after accessing one of the entries
     */
}

bool search_in_cache(int level, int address)
{

}