#include <omp.h>
#include "familytree.h"


int n = 0;


void computetraverse(tree *node)
{
	if(node != NULL)
	{
		n++;
		#pragma omp task final(n > 30)
		{
			node->IQ = compute_IQ(node->data);
			genius[node->id] = node->IQ;
		}
		#pragma omp task final(n > 30)
		{
			computetraverse(node->right);
		}
		#pragma omp task final(n > 30)
		{
			computetraverse(node->left);
		}
	}



}
void traverse(tree *node, int numThreads)
{
		
	#pragma omp parallel num_threads(numThreads)
	{ 
		#pragma  omp single nowait
		{
			
			computetraverse(node);
		}
	}
	

}

