#include <omp.h>

void compute(unsigned long *a, unsigned long *b, unsigned long *c, unsigned long *d, int N, int num_threads) {

	// perform loop alignment to transform this loop and parallelize it with OpenMP	

	
	a[1] = d[1] * b[1];
	c[0] = a[1] * d[1];

	#pragma omp parallel for num_threads(num_threads)
		for (int i = 1; i < N-1; i++) {

			b[i + 1] = 2 * c[i];
			a[i+1] = d[i+1] * b[i+1];
			c[i] = a[i+1] * d[i+1];
		
		}

	b[N] = 2 * c[N-1];		
}
