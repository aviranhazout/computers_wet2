//
// Created by nater on 5/6/2019.
//

#include "HW2_cache.h"

#define VICTIM_CACHE_SIZE 4
#define VICTIM_CACHE_ACCESS_TIME 1
#define VICTIM_CACHE_LEVEL 0


/*if (address exists in L1)
 *      update lru in L1
 *      update trace data
 *      update dirty
 *else if (address exists in L2)
 *      update lru in L2
 *      if (write allocate or read-op)
 *          find empty place or remove some block in L1 (update dirty)
 *          copy to L1
 *          update lru in L1
 *      else
 *          update dirty bi
 *      update the trace data
 *else if(victim and address exists in victim)
 *      find empty place or remove some block in L2
 *      if removed
 *          snoop in L1
 *      find empty place or remove some block in L1
 *      copy to L2
 *      update lru in L2
 *      copy to L1
 *      update lru in L1
 *      remove from victim cache
 *      update trace data
 *  else    --fetching from mem
 *      find empty place or remove some block in L2
 *      if removed
 *          snoop in L1
 *      find empty place or remove some block in L1
 *      copy to L2
 *      update lru in L2
 *      copy to L1
 *      update lru in L1
 *      update trace data
 */
void access_cache(cache_sys& CS, char operation, int address)
{
    block* to;
    block* from;
    if (address == 510448)
        printf("hi\n");//for debugg
    if (CS.search_in_cache(1,address))
    {   //found in L1
        int way = CS.get_way(1,address);
        int set = CS.get_set_from_address(1, address);
        int tmp = CS.get_lru(1, address);
        CS.L1[way][set].LRU = -1;
        CS.update_lru(1, tmp, address);
        if (operation == 'w')
            CS.mark_dirty(1,address);
    }
    else if (CS.search_in_cache(2,address))
    {   //found in L2 , doesn't exist in L1
        block* tmp_block = CS.get_block(2,address);
        int tmp_lru = tmp_block->LRU;
        tmp_block->LRU = -1;
        CS.update_lru(2, tmp_lru, address);
        if (CS.WrAlloc == 0 && operation == 'w')
            CS.write_back(address);
        else    //copy to L1
        {
            int way = CS.find_place(1,address);
            int set_and_tag = address / CS.block_size;
            int set = set_and_tag % CS.L1_way_entries_num;
            to = &(CS.L1[way][set]);
            if (!(to->invalid) && to->dirty)
                CS.write_back(address);
            from = CS.get_block(2,address);
            int tmp_lru = to->LRU;
            CS.copy_data(from, to, 1);
            CS.update_lru(1, tmp_lru, address);
            if (operation == 'w')
                to->dirty = true;
        }
    }
    else if (CS.VicCache == 1 && CS.search_in_cache(3,address))  //found in victim-cache
    {
        CS.vic_access++;
        if (CS.WrAlloc == 0 && operation == 'w')
            CS.mark_dirty(3,address);
        else
        {   //we want to copy the data
            block tmp;
            block* vic_block;
            block* L2_block;
            block* L1_block;
            vic_block = CS.get_block(3, address);
            CS.copy_data(vic_block, &tmp, 2);
            //from->invalid = true;
            int way2 = CS.find_place(2,address);
            int set_and_tag2 = address / CS.block_size;
            int set2 = set_and_tag2 % CS.L2_way_entries_num;
            L2_block = &(CS.L2[way2][set2]);
            if (!(L2_block->invalid))
            {
                CS.copy_data(L2_block, vic_block, 3);
                for (int i = 0; i < VICTIM_CACHE_SIZE; i++)
                    CS.victimCache[i].LRU++;
            }
            int way1 = CS.find_place(1,address);
            int set_and_tag1 = address / CS.block_size;
            int set1 = set_and_tag1 % CS.L1_way_entries_num;
            L1_block = &(CS.L1[way1][set1]);
            if (!(L1_block->invalid) && L1_block->dirty)
                CS.write_back(address);
            CS.copy_data(&tmp, L2_block, 2);
            CS.update_lru(2, CS.L2_way_num, address);
            //free(tmp);

            CS.copy_data(L2_block, L1_block, 1);
            CS.update_lru(1, CS.L1_way_num, address);

            if (operation == 'w')
                L1_block->dirty = true;

        }
    }
    else    //copy from memory
    {
        if (CS.VicCache == 1)
        {
            CS.vic_access++;
            CS.mem_access++;
        }
        else
        {
            CS.mem_access++;
        }

        if (CS.WrAlloc == 1 || operation == 'r')
        {
            block* L2_block;
            block* L1_block;
            int way2 = CS.find_place(2,address);
            int set2 = CS.get_set_from_address(2,address);
            L2_block = &(CS.L2[way2][set2]);
            if (!(L2_block->invalid) && CS.VicCache == 1)
            {
                int vic_place = CS.find_place(3, address);
                block* vic_block = &(CS.victimCache[vic_place]);
                CS.copy_data(L2_block, vic_block, 3);
                for (int i = 0; i < VICTIM_CACHE_SIZE; i++)
                    CS.victimCache[i].LRU++;
            }
            CS.copy_from_memory(L2_block, address);
            CS.update_lru(2, CS.L2_way_num, address);

            int way1 = CS.find_place(1,address);
            int set1 = CS.get_set_from_address(1, address);
            L1_block = &(CS.L1[way1][set1]);
            if (!(L1_block->invalid) && L1_block->dirty)
                CS.write_back(L1_block->address);
            CS.copy_data(L2_block, L1_block, 1);
            CS.update_lru(1, CS.L1_way_num, address);

            if (operation == 'w')
                L1_block->dirty = true;
        }

    }
}

// MRU = 0  LRU = array-size
// search: true = hit

void cache_sys::write_back(int address)
{
    block *tmp_block;
    int tmp_lru;
    this->mark_dirty(2, address);
    tmp_block = this->get_block(2, address);
    tmp_lru = tmp_block->LRU;
    tmp_block->LRU = -1;
    this->update_lru(2, tmp_lru, address);
}
/**
 * Snoop - checks higher cache lever (L1) for identical blocks in L1 and L2
 * If exists in L1, marks it as invalid
 * !! does not mark L2 block as invalid !!
 * @param address
 */
bool cache_sys::snoop(int address)
{
    int tag = this->get_tag_from_address(1, address);
    int set = this->get_set_from_address(1, address);

    for (int i = 0; i < this->L1_way_entries_num; i++)
    {
        if (this->L1[i][set].tag == tag && !(this->L1[i][set].invalid))
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
    int set = this->get_set_from_address(level, address);
    if (level == 1 || level == 2)
    {
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
            if (cache[i][set].invalid)
                return i;
        }

        //else select a block to replace
        for (int i = 0; i < num_of_ways; i++)
        {
            if (cache[i][set].LRU == num_of_ways - 1)
            {
                if (level == 2)
                    cache[i][set].dirty = this->snoop(cache[i][set].address);
                return i;
            }
        }
    }
    else    //victim cache
    {
        //search for empty place
        for (int i = 0; i < VICTIM_CACHE_SIZE; i++)
        {
            if (this->victimCache[i].invalid)
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
void cache_sys::update_lru(int level, int max_lru, int address)
{
    block **cache;
    int set = this->get_set_from_address(level, address);
    int num_of_ways = this->get_num_ways(level);
    switch (level)
    {
        case 1:
            cache = this->L1;
            break;
        case 2:
            cache = this->L2;
            break;
    }
    for (int i = 0; i < num_of_ways; i++)
    {
        if (cache[i][set].LRU < max_lru)
            (cache[i][set].LRU)++;
    }
    return;
    /*
     * update the lru of all the  with at most max_lru
     * {after inserting)
     *
     * if we want to update all the blocks min_lru = 0
     * if we want to update after accessing one of the entries
     */
}

int cache_sys::get_lru(int level, int address)
{
    block* block_to_find;
    block_to_find = this->get_block(level,address);
    return block_to_find->LRU;
}

void cache_sys::mark_dirty(int level, int address)
{
    //we know that the address exists
    block* block_to_find = this->get_block(level,address);
    block_to_find->dirty = true;
}

void cache_sys::copy_data(block* from, block* to, int to_level)
{
    int tag = this->get_tag_from_address(to_level, from->address);
    to->dirty = from->dirty;
    from->dirty = false;
    to->LRU = -1;
    to->tag = tag;
    to->invalid = false;
    to->address = from->address;
}

void cache_sys::copy_from_memory(block* to, int address)
{
    int tag = this->get_tag_from_address(2, address);
    to->dirty = false;
    to->LRU = -1;
    to->tag = tag;
    to->invalid = false;
    to->address = address;
}

block* cache_sys::get_block(int level, int address)
{
    int set = this->get_set_from_address(level, address);
    int tag = this->get_tag_from_address(level,address);
    if (level == 1 || level == 2)
    {
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
            if (cache[i][set].tag == tag && !(cache[i][set].invalid)) {
                return &(cache[i][set]);
            }
        }
    }
    else    //victim cache
    {
        for (int i = 0; i < VICTIM_CACHE_SIZE; i++) {
            if (this->victimCache[i].tag == tag) {
                return &(this->victimCache[i]);
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
    block **cache;
    int num_of_ways = get_num_ways(level);
    int set = this->get_set_from_address(level, address);
    int tag = this->get_tag_from_address(level, address);
    switch (level) {
        case 1:
            cache = this->L1;
            //increase L1 access attempts
            this->L1Access++;
            printf("L1 access\n");
            break;
        case 2:
            cache = this->L2;
            //increase L2 access attempts
            this->L2Access++;
            printf("L2 access\n");
            break;
    }
    for (int i = 0; i < num_of_ways; i++)
    {
        if (cache[i][set].tag == tag && !(cache[i][set].invalid))
        {
            if (level == 1) {
                this->L1Hit++;
                printf("L1 hit\n");
            }
            else {
                this->L2Hit++;
                printf("L2 hit\n");
            }
            return true;
        }
    }
    return false;
}




/**
 * returns number of ways at a given level
 * @param level
 * @return
 */
int cache_sys::get_num_ways(int level){
    return level == 1 ?  this->L1_way_num : this->L2_way_num;
}

int cache_sys::get_num_entries(int level){
    return level == 1 ?  this->L1_way_entries_num : this->L2_way_entries_num;
}

/**
 * cuts tag from given address
 * if only single way at level, remove Bsize bits
 * else remove Bsize bits + num_ways bits
 * @param level
 * @param address
 * @return
 */
int cache_sys::get_tag_from_address(int level, int address){
    if (get_num_entries(level) == 1 || level == VICTIM_CACHE_LEVEL){
        int tag = address / (this->block_size);
        return tag;
    }
    int tag = address / (this->block_size * get_num_entries(level));
    return tag;
}

/**
 * gets set from address, if 1 set at level then return 0 (access cache[][0])
 * else return way number from address
 * @param level
 * @param address
 * @return
 */
int cache_sys::get_set_from_address(int level, int address) {
    if (get_num_entries(level) == 1 || level == VICTIM_CACHE_LEVEL) return 0;
    int set = address / this->block_size;
    set = set % get_num_entries(level);
    return set;
};

void cache_sys::print_all()
{
    if (true) //if we want to print just a part of the lines
    {
        int L1_out_loop = get_num_ways(1);
        int L1_in_loop = get_num_entries(1);
        block* to_print;
        printf("\n ---L1--- \n");
        for (int i = 0; i < L1_out_loop; i++)
        {
            printf("\n -way %d-\n",i);
            for (int j  = 0; j < L1_in_loop; j++)
            {
                to_print = &this->L1[i][j];
                printf("block %d: address = %d, tag = %d\n", j, to_print->address, to_print->tag);
                printf("         %s",(to_print->invalid ? "invalid" : "valid"));
                printf(", %s, LRU = %d\n", (to_print->dirty ? "dirty" : "clean"), to_print->LRU);
            }
        }
        int L2_out_loop = get_num_ways(2);
        int L2_in_loop = get_num_entries(2);
        printf("\n ---L2--- \n");
        for (int i = 0; i < L2_out_loop; i++)
        {
            printf("\n -way %d-\n",i);
            for (int j  = 0; j < L2_in_loop; j++)
            {
                to_print = &this->L2[i][j];
                printf("block %d: address = %d, tag = %d\n", j, to_print->address, to_print->tag);
                printf("         %s",(to_print->invalid ? "invalid" : "valid"));
                printf(", %s, LRU = %d\n", (to_print->dirty ? "dirty" : "clean"), to_print->LRU);
            }
        }
        if (this->VicCache)
        {
            printf("\n ---victim-cache--- \n");
            for (int j  = 0; j < VICTIM_CACHE_SIZE; j++)
            {
                to_print = &this->victimCache[j];
                printf("block %d: address = %d, tag = %d\n", j, to_print->address, to_print->tag);
                printf("         %s",(to_print->invalid ? "invalid" : "valid"));
                printf(", %s, LRU = %d\n", (to_print->dirty ? "dirty" : "clean"), to_print->LRU);
            }
        }
        printf("\n\n");
    }
}
int cache_sys::get_way(int level, int address)
{
    int num_ways = get_num_ways(level);
    int set = get_set_from_address(level, address);
    int tag = get_tag_from_address(level, address);
    block** cache;
    if (level == 1)
        cache = this->L1;
    else
        cache = this->L2;
    for (int i = 0; i < num_ways; i++)
    {
        if (cache[i][set].tag == tag)
            return i;
    }

}
