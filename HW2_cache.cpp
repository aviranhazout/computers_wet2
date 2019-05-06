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

void snoop(int address){
    /*
     * search in L1 for address
     * if exists
     *      mark invalid
     *      update lru
     */
};

int find_place(int level)
{
    /*
     * search in level for invalid
     * else
     *      remove block by lru
     *      if needed - snoop
     */
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