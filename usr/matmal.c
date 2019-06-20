// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn

// multiply-threaded matrix multiplication program modified from geeksforgeeks.org

#include "stdio.h"
#include "pthread.h"

// maximum size of matrix 
#define MAX 6

// maximum number of threads 
#define MAX_THREAD 3

int matA[MAX][MAX];
int matB[MAX][MAX];
int matC[MAX][MAX];

void multiply(void* arg)
{
	int tid = pthread_self();
	printf("This is thread:%d Arg:%d\n", tid, (int)arg);

	int core = (int) arg;
	// Each thread computes 1/4th of matrix multiplication 
	for (int i = core * MAX / MAX_THREAD; i < (core + 1) * MAX / MAX_THREAD; i++) 
		for (int j = 0; j < MAX; j++) 
			for (int k = 0; k < MAX; k++){
				// printf("%d ", i);
				matC[i][j] += matA[i][k] * matB[k][j];
			}
}

// Driver Code 
int main() 
{ 
	// Generating random values in matA and matB 
	for (int i = 0; i < MAX; i++) { 
		for (int j = 0; j < MAX; j++) { 
			matA[i][j] = i + j; 
			matB[i][j] = i * j;
			matC[i][j] = 0;
		} 
	} 

	// Displaying matA 
	printf("Matrix A\n"); 
	for (int i = 0; i < MAX; i++) { 
		for (int j = 0; j < MAX; j++) 
			printf("%d ", matA[i][j]); 
		printf("\n");
	} 

	// Displaying matB 
	printf("Matrix B\n");
	for (int i = 0; i < MAX; i++) { 
		for (int j = 0; j < MAX; j++) 
			printf("%d ", matB[i][j]); 
		printf("\n");
	} 

	// declaring four threads 
	pthread_t threads[MAX_THREAD]; 

	// Creating four threads, each evaluating its own part 
	for (int i = 0; i < MAX_THREAD; i++)
		pthread_create(&threads[i], (pthread_addr)multiply, (void*)(i));

	// joining and waiting for all threads to complete 
	for (int i = 0; i < MAX_THREAD-1; i++) 
		pthread_join(threads[i], NULL);
	pthread_join(threads[MAX_THREAD-1], NULL);

	// Displaying the result matrix 
	printf("Multiplication of A and B\n");
	for (int i = 0; i < MAX; i++) { 
		for (int j = 0; j < MAX; j++) 
			printf("%d ", matC[i][j]); 
		printf("\n");
	} 
	return 0; 
}