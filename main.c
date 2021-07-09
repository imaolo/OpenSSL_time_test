// -fopenmp -lycrypto
// Tests the run time of openssl's hash function
// and places the results in result.csv.
// It is set to test SHA1

// Arguments:
// 1. Min message size(MB)
// 2. Max message size(MB)
// 3. Granularity
// 4. Number of tests

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <openssl/sha.h>
#include <omp.h>

#define FILE_NAME "results.csv"
#define HASH_FUNCTION SHA1
#define DIGEST_LENGTH SHA_DIGEST_LENGTH

typedef uint8_t BYTE;

void printArguments();
int checkArguments(int,char**);

int main(int argc, char **argv){
	if (!checkArguments(argc,argv))
    	return 0;
    printf("running... \n");

	const size_t MIN_MESSAGE_SIZE = atof(argv[1]) * pow(10,6);
    const size_t MAX_MESSAGE_SIZE = atof(argv[2]) * pow(10,6);
    const size_t GRANULARITY      = atoi(argv[3]);
    const size_t NUMBER_OF_TESTS  = atoi(argv[4]);
    const size_t INCREMENT        = (MAX_MESSAGE_SIZE-MIN_MESSAGE_SIZE)/GRANULARITY;
    BYTE* MESSAGE                 = calloc(MAX_MESSAGE_SIZE,sizeof(BYTE));
    double time  = 0;
    double start = 0;
    BYTE result[DIGEST_LENGTH];
    FILE *of;

	//output file configuration
    of = fopen(FILE_NAME,"w");
    fprintf(of,"SHA1 Metric Data\n");	
    fprintf(of,"Min Message Size(MB): %0.6f\n",MIN_MESSAGE_SIZE/pow(10,6));
    fprintf(of,"Max Message Size(MB): %0.6f\n",MAX_MESSAGE_SIZE/pow(10,6));
    fprintf(of,"Granularity         : %ld\n",GRANULARITY);
    fprintf(of,"Tests per Cycle     : %ld\n\n",NUMBER_OF_TESTS);
    fprintf(of,"Message Size(MB), Time\n");
    fclose(of);

    size_t size = MIN_MESSAGE_SIZE;
    size_t cycles  = MIN_MESSAGE_SIZE == MAX_MESSAGE_SIZE ? GRANULARITY-1:GRANULARITY;
    for (int i = 0;i<=cycles;i++){
        for (int j = 0;j<NUMBER_OF_TESTS;j++){
            start = omp_get_wtime();
            HASH_FUNCTION(MESSAGE,size,result);
            time += omp_get_wtime()-start;
        }
        of = fopen(FILE_NAME,"a");
        fprintf(of,"%0.6f,        %0.9f\n",size/pow(10,6),time/NUMBER_OF_TESTS);
        size+= INCREMENT;
        time = 0;
        fclose(of);
    }
    printf("Done\n");
    
    free(MESSAGE);
    return 0;
}

void printArguments(){
    printf("The arguments are: \n");
    printf("1. Min message size(MB)\n");
    printf("2. Max message size(MB)\n");
    printf("3. Granularity\n");
    printf("4. Number of tests\n");
}
int checkArguments(int argc, char **argv){
    if (argc != 5){
        printf("Please include the proper amount of arguments.\n");
        printArguments();
        return 0;
    }
    else if (atof(argv[1]) <= 0| atof(argv[2]) <= 0){
        printf("Please enter a float greater than zero for message size.\n");
        printArguments();
        return 0;
    }
    else if (atof(argv[1]) > atof(argv[2])){
        printf("Ensure the min message size is less than the max message size.\n");
        printArguments();
        return 0 ; 
    }
    else if (atoi(argv[3]) <= 0){
        printf("Please enter an integer greater than zero for granularity.\n");
        printArguments();
        return 0;
    }
    else if (atoi(argv[4]) <= 0){
        printf("Please enter an integer greater than zero for number of tests.\n");
        printArguments();
        return 0;
    }
    return 1;
}