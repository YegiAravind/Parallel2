#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX 10240
#define NUM_THREADS  10

int n1,n2;
char *s1,*s2;
FILE *fp;
int countArray[NUM_THREADS]={0};

// Function to read input file and generate strings s1, s2, and their lengths n1, n2
int readf(FILE *fp) {
    if((fp=fopen("strings.txt", "r"))==NULL){
        printf("ERROR: can't open string.txt!\n");
        return 0;
    }
    s1 = (char *)malloc(sizeof(char)*MAX);
    s2 = (char *)malloc(sizeof(char)*MAX);
    
    s1 = fgets(s1, MAX, fp);
    s2 = fgets(s2, MAX, fp);
    n1 = strlen(s1);  // length of s1
    n2 = strlen(s2) - 1; // length of s2 (excluding newline character)

    if(s1 == NULL || s2 == NULL || n1 < n2)  // error check
        return -1;
    return 0;
}

// Function to count the number of substrings in a segment of s1
int num_substring(int start, int end) {
    int count = 0;
    for(int i = start; i <= end - n2; i++) {
        if(strncmp(&s1[i], s2, n2) == 0) {
            count++;
        }
    }
    return count;
}

// Thread function to calculate the number of substrings
void *calSubStringThread(void *threadid) {
    long tid = (long)threadid;
    int start = tid * (n1 / NUM_THREADS);
    int end = (tid == NUM_THREADS - 1) ? n1 : start + (n1 / NUM_THREADS);
    
    printf("This is thread %ld, processing from index %d to %d\n", tid, start, end);
    int num = num_substring(start, end);
    countArray[tid] = num;
    printf("Thread %ld found %d substrings\n", tid, num);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int t, rc;
    int totalNum = 0;

    readf(fp);

    // Create threads
    for(t = 0; t < NUM_THREADS; t++) {
        rc = pthread_create(&threads[t], NULL, calSubStringThread, (void *) (size_t)t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Wait for all threads to complete
    for(t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
        totalNum += countArray[t];
    }

    printf("The number of substrings is: %d\n", totalNum);
    free(s1);
    free(s2);
    return 0;
}
