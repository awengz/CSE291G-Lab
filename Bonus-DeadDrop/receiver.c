
#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>

#define BUFF_SIZE (1<<21)
//#define BUFF_SIZE 500144 //TODO 4608?
int main(int argc, char **argv)
{
    // [Bonus] TODO: Put your covert channel setup code here
    //volatile uint8_t *eviction_buffer = (uint8_t *)malloc(BUFF_SIZE);


    volatile uint8_t *eviction_buffer= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE |
		                        MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);

        if (eviction_buffer == (void*) - 1) {
		        perror("mmap() error\n");
			        exit(EXIT_FAILURE);
				    }
	// The first access to a page triggers overhead associated with
	// page allocation, TLB insertion, etc.
	// Thus, we use a dummy write here to trigger page allocation
	// so later access will not suffer from such overhead.
	eviction_buffer[0] = 1; // dummy write to trigger page allocation


    uint8_t tmp;
    uint64_t probe_time[256]; // probe time for each set
    //uint64_t probetimeset0[800];
    uint64_t cur_time;
    //uint64_t probe_time[4];
    int avg = 0;
    int cntr=0;
    // step 1: Prime
    // Fill each set in L2 with data.
    for (int j = 0; j<4096; j++) {  
	    tmp = eviction_buffer[j*64];
    }

    for (int j=0; j<256; j++){ 
	    probe_time[j] = 0;
    }
    printf("Please press enter.\n");

    char text_buf[2];
    fgets(text_buf, sizeof(text_buf), stdin);

    printf("Receiver now listening.\n");

    for (int j = 0; j<256; j++) {
	    for (int k=0; k<4; k++){ 
		    tmp = eviction_buffer[j*64 + k*65536];
	    }
    }

    for (int j=0 ; j< 50000; j++){
	    avg +=1;
    }

    bool listening = true;
    while (listening) {
	    //cntr++;
	    //printf("cntr = %d", cntr);
	    // [Bonus] TODO: Put your covert channel code here

	    for (int j = 0; j<256; j++) {
		    for (int k=0; k<4; k++){ 
			    tmp = eviction_buffer[j*64 + k*65536];
		    }
	    }

	    for (int j=0 ; j< 50000; j++){
		    avg +=1;
	    }
	    // step 2: probe
	    //int i=0;
	    for (int i=0; i<800; i++){ //100 samples 
		    for (int j=0; j<256; j++){ // Probe set-wise
			    cur_time = 0;
			    for (int k=0; k<4; k++){ // iterate over 4 ways
				    cur_time += measure_one_block_access_time((uint64_t) eviction_buffer + j*64 + k*65536); //65536 = 1024*64 -- map to same set
			    }
				    probe_time[j] += cur_time/4;
				    //if (j==0) probetimeset0[i] = cur_time/4;
		    }
	    }	    

	    //for(int j=0; j<256; j++){
	    //	if(probe_time[j] > 40*500){
	    //    	listening = false;
	    //    	printf("set j :");
	    //    }
	    //}
	    listening = false;
	    //listening = false;
	//for(int j = 0; j<256; j++){
		//avg = (probe_time[j][0] + probe_time[j][1] + probe_time[j][2] + probe_time[j][3])/4;
		//printf("FOr set %d: avg probe time is equal to %d \n", j, avg);
		//if (avg > 35){
		//       	listening = false;
		//	printf("received message %d\n", j);
		//	break;
		//}
	//}

	//listening = false;
    }

    for(int j=0;j<800;j++){
//    	printf("set 0 sample %d is eqial to %d\n", j, probetimeset0[j]);
    }
    for(int j = 0; j<256; j++){
	    avg = probe_time[j]/800;
            printf("FOr set %d: avg probe time is equal to %d \n", j, avg);
    }
    printf("Receiver finished.\n");

    return 0;
}


