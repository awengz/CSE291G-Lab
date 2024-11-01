
#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>

// [Bonus] TODO: define your own buffer size
#define BUFF_SIZE (1<<21)
//#define BUFF_SIZE 2097152 // 2 MB

int main(int argc, char **argv)
{
    // Allocate a buffer using huge page
    // See the handout for details about hugepage management
    void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE |
                    MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    
    if (buf == (void*) - 1) {
        perror("mmap() error\n");
        exit(EXIT_FAILURE);
    }
    // The first access to a page triggers overhead associated with
    // page allocation, TLB insertion, etc.
    // Thus, we use a dummy write here to trigger page allocation
    // so later access will not suffer from such overhead.
    *((char *)buf) = 1; // dummy write to trigger page allocation


    // [Bonus] TODO:
    // Put your covert channel setup code here
    int i = 0;
    int asci = -1;
    char tmp;
    int dummy=0;

    printf("Please type a message.\n");

    bool sending = true;
    while (sending) {
        char text_buf[128];
        fgets(text_buf, sizeof(text_buf), stdin);

        // [Bonus] TODO:
        // Put your covert channel code here
	// while (text_buf[i] != '\0'){
		// printf("%d: %c\n", i, text_buf[i]);
		// asci = (int) text_buf[i];
		asci = string_to_int(text_buf);
		//printf ("asci val = %d\n", asci); // dbg print

		// fill up asci-th set in L2 with data
	while(true){
		for (int j=0; j<8; j++){ // do we need 8? 4 should be enough to fill up L2 cache bc associativity is 4.
			// asci * 64 points to the required index
			// 65536 is equal to 1024*64 -- the next address that will map to the same index is 65536 away from the original address (bc 1024 sets in L2)
			tmp = *((char *)buf + asci*64 + j*65536);
		}

		// TODO: Add waiting logic ??
		dummy=0;
		for (int j = 0; j < 20000; j++){
			dummy+=2;	
		}

		i++;
	} // TODO: temp loop, remove
	// }
	// send null character -- use 0th set of L2 cache.
	//for (int j=0; j<8; j++){ // do we need 8? 4 should be enough to fill up L2 cache bc associativity is 4.
	//	tmp = *((char *)buf + j*65536);
	//}
	
	//sending=false;
	//printf("starting pntr %p\n", buf); // dbg print, comment out
    
    }

    printf("Sender finished.\n");
    return 0;
}


