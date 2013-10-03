#ifndef __LATENCYUTILS3OCT2013__
#define __LATENCYUTILS3OCT2013__

/*
 * List = int array of size n (permutation of 0..n-1)
 * count = number of accesses
 * index = 0 initially
 * there after index = List[index]%n
 * returns number of repeats in count chained accesses
 */
int countrepeats(long int *List, long int n, int count);

/*
 * n = size of array
 * count = number of accesses
 * return probability of no repeat in chained access
 */
double probNoR(long int n, int count);

/*
 * list[] = int array of size n
 * list[0..n-1] initialized to random permutation of 0..n-1
 */
void random_perm(int *list, int n);

/*
 * list[] = int array of size n
 * list[0..n-1] initialized to random CYCLE
 */
void random_cycle(int *list, int n);
#endif