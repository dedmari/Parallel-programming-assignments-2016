#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "helper.h"
#include "loop_alignment.h"
#include "loop_alignment_ref.h"

unsigned long *get_int64_onedim_array(size_t num)
{
	unsigned long *array = (unsigned long*)malloc(num * sizeof(*array));
	if (array == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}

	for (size_t j = 0; j < num; j++)
			array[j] = num - j;

	return array;
}

int isEqual(unsigned long *a_seq, unsigned long *b_seq, unsigned long *c_seq, unsigned long *d_seq,
            unsigned long *a_par, unsigned long *b_par, unsigned long *c_par, unsigned long *d_par, int N)
{
	for (int i = 1; i < N + 1; i++) {
	
			if ( a_seq[i] != a_par[i] ||
			        b_seq[i] != b_par[i] ||
			        c_seq[i] != c_par[i] ||
			        d_seq[i] != d_par[i])
				return 0;
		
	}
	return 1;
}

int main(int argc, char** argv) {

	int N = 1000;
	int num_threads = 4;
	int input;

	while ((input = getopt(argc, argv, "t:n:")) != -1)
	{
		switch (input)
		{
		case 't':
			if (sscanf(optarg, "%d", &num_threads) != 1)
				goto error;
			break;

		case 'n':
			if (sscanf(optarg, "%d", &N) != 1)
				goto error;
			break;

		case '?':
			error: printf(
			    "Usage:\n"
			    "-t \t number of threads used in computation\n"
			    "-n \t number of iterations\n"
			    "\n"
			    "Example:\n"
			    "%s -t 4 -n 2500\n", argv[0]);
			exit(EXIT_FAILURE);
			break;
		}
	}

	int exit_status = 0;
	unsigned long *a_seq = get_int64_onedim_array(N + 1);
	unsigned long *b_seq = get_int64_onedim_array(N + 1);
	unsigned long *c_seq = get_int64_onedim_array(N + 1);
	unsigned long *d_seq = get_int64_onedim_array(N + 1);

	unsigned long *a_par = get_int64_onedim_array(N + 1);
	unsigned long *b_par = get_int64_onedim_array(N + 1);
	unsigned long *c_par = get_int64_onedim_array(N + 1);
	unsigned long *d_par = get_int64_onedim_array(N + 1);


	compute_ref(a_seq, b_seq, c_seq, d_seq, N, num_threads);

	compute(a_par, b_par, c_par, d_par, N, num_threads);


	if (isEqual(a_seq, b_seq, c_seq, d_seq, a_par, b_par, c_par, d_par, N))
	{
		fprintf(stderr, "Correct!\n");
		exit_status = EXIT_SUCCESS;
	}
	else
	{

		fprintf(stderr, "ERROR: Sequential implementation and parallel implementation gives different arrays\n");
		exit_status = EXIT_FAILURE;
	}



	free(a_seq);
	free(b_seq);
	free(c_seq);
	free(d_seq);

	free(a_par);
	free(b_par);
	free(c_par);
	free(d_par);

	return exit_status;
}
