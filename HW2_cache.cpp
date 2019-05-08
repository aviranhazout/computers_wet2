//
// Created by nater on 5/6/2019.
//

#include "HW2_cache.h"

#define VICTIM_CACHE_SIZE 4
#define VICTIM_CACHE_ACCESS_TIME 1

//the tag will be different for each level, we need to find a solution for this
void access_cache(cache_sys CS, char operation, int address)
{
    block* to;
    block* from;
    if (CS.search_in_cache(1,address))
    {   //found in L1
        CS.update_lru(1, CS.get_lru(1, address));
        if (operation == 'w')
            CS.mark_dirty(1,address);
        CS.access_time += CS.L1Access;
    }
    else if (CS.search_in_cache(2,address))
    {   //found in L2 , doesn't exist in L1
        CS.update_lru(2, CS.get_lru(2, address));
        CS.access_time += (CS.L1Access + CS.L2Access);
        if (CS.WrAlloc == 0 && operation == 'w')
            CS.mark_dirty(2,address);
        else    //copy to L1
        {
            int way = CS.find_place(1,address);
            int set_and_tag = address >> CS.BSize;
            int set = set_and_tag % CS.L1_way_entries_num;
            to = &(CS.L1[way][set]);
            if (!(to->invalid) && to->dirty)
                CS.mark_dirty(2,address);
            CS.get_block(2,address,from);
            CS.copy_data(from,to);
            CS.update_lru(1, to->LRU);
            if (operation == 'w')
                to->dirty = true;
        }
    }
    else if (CS.VicCache == 1 && CS.search_in_cache(3,address))  //found in victim-cache
    {
        CS.access_time += (CS.L1Access + CS.L2Access + VICTIM_CACHE_ACCESS_TIME);
        if (CS.WrAlloc == 0 && operation == 'w')
            CS.mark_dirty(3,address);
        else
        {   //we want to copy the data
            block tmp = new block;
            block* vic_block;
            block* L2_block;
            block* L1_block;
            CS.get_block(3, address, vic_block);
            CS.copy_data(vic_block, &tmp);
            //tmp.tag = (tmp.tag) / CS.L2_way_num;
            from->invalid = true;
            int way2 = CS.find_place(2,address);
            int set_and_tag2 = address >> CS.BSize;
            int set2 = set_and_tag2 % CS.L2_way_entries_num;
            L2_block = &(CS.L2[way2][set2]);
            if (!(L2_block->invalid))
            {
                CS.copy_data(L2_block, vic_block)
                //vic_block->tag = (vic_block->tag * CS.L2_way_entries_num)
                for (int i = 0; i < VICTIM_CACHE_SIZE; i++)
                {
                    CS.victimCache[i].LRU++;
                }
            }
            int way1 = CS.find_place(1,address);
            int set_and_tag1 = address >> CS.BSize;
            int set1 = set_and_tag1 % CS.L1_way_entries_num;
            L1_block = &(CS.L1[way1][set1]);
            if (!(L1_block->invalid) && L1_block->dirty)
                CS.mark_dirty(2,address);
            CS.copy_data(&tmp, L2_block);
            CS.update_lru(2, CS.L2_way_num);

            CS.copy_data(L2_block, L1_block);
            CS.update_lru(1, CS.L1_way_num);

            if (operation == 'w')
                L1_block->dirty = true;

        }
    }
    else    //copy from memory
    {

    }
    /*if (address exists in L1)
     *{
     *      update lru in L1                                                    V
     *      update the trace data                                               V
     *      update dirty bit if needed                                          V
     *else if (address exists in L2)
     *      update lru in L2                                                    V
     *      if (write allocate and write-op)
     *          find empty place or remove some block in L1                     V
     *          copy to L1 (don't forget to move the dirty bit if needed)       V
     *          update lru in L1                                                V
     *      else if(write-op)
     *          update dirty bit if needed                                      V
     *      update the trace data                                               V
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
bool cache_sys::snoop(int address)
{
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
            return this->L1[i][set].dirty;
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
                    cache[i][set].dirty = this->snoop(address);
                return i;
            }
        }
    }
    else    //victim cache
    {
        //search for empty place
        for (int i = 0; i < VICTIM_CACHE_SIZE; i++)
        {
            if (this->victimCache[i].invalid == true)
                return i;
        }

        //else select a block to replace
        for (int i = 0; i < VICTIM_CACHE_SIZE; i++)
        {
            if (this->victimCache[i].LRU == VICTIM_CACHE_SIZE - 1)    //victim cache is always size 4 blocks
                return i;
        }
    }
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

int cache_sys::get_lru(int level, int address)
{
    block* block_to_find;
    this->get_block(level,address,block_to_find);
    return block_to_find->LRU;
}

void cache_sys::mark_dirty(int level, int address)
{
    //we know that the address exists
    block* block_to_find;
    this->get_block(level,address,block_to_find);
    block_to_find->dirty = true;
}

void cache_sys::copy_data(block* from, block* to)
{
    to->dirty = from->dirty;
    from->dirty = false;
    to->LRU = -1;
    to->tag = from->tag;
    to->invalid = false;
}

void cache_sys::get_block(int level, int address, block* ret)
{
    int set_and_tag = address >> this->BSize;

    if (levvel == 1 || level == 2) {
        int set = set_and_tag % this->L1_way_entries_num;
        block **cache;
        int num_of_ways;
        if (level == 1) {
            cache = this->L1;
            num_of_ways = this->L1_way_num;
        } else {
            cache = this->L2;
            num_of_ways = this->L2_way_num;
        }
        //search for empty place
        for (int i = 0; i < num_of_ways; i++) {
            if (cache[i][set].tag == tag) {
                ret = &(cache[i][set]);
                return;
            }
        }
    }
    else {
        for (int i = 0; i < VICTIM_CACHE_SIZE; i++) {
            if (cache[i][set].tag == set_and_tag) {
                ret = &(cache[i][set]);
                return;
            }
        }
    }
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


int remove(int level)
{
    /*
     * find the lru and invalidate it
     */
}

