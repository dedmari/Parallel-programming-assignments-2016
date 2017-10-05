#include "familytree.h"

/*void computetraverse(tree *node)
{
	if(node != NULL)
	{
		#pragma omp section
		{
			node->IQ = compute_IQ(node->data);
			genius[node->id] = node->IQ;
		}
		#pragma omp section
		{
			computetraverse(node->right);
		}
		#pragma omp section

		{
			computetraverse(node->left);
		}
	}



}*/
void traverse(tree *node, int numThreads)
{
	if(node != NULL)
	{	
		#pragma omp parallel sections  num_threads(numThreads)
		{ 
			
			
				#pragma omp section
				{
					node->IQ = compute_IQ(node->data);
					genius[node->id] = node->IQ;
				}
				#pragma omp section
				{
					traverse(node->right,numThreads);
				}
				#pragma omp section

				{
					traverse(node->left,numThreads);
				}
			
		}
	}
	

}

