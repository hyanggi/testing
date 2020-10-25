// This program generates random integers in the range [0, target_np).

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

int main(void)
{
	srand((unsigned int)time(NULL));

	// "np" stands for "number of possibilities"
	unsigned int target_np = 10; // The number of possibilities we want
	unsigned int original_np = RAND_MAX + 1u; // The number of possibilities provided by rand()
	unsigned int adjusted_np = original_np - original_np % target_np; // Make the number of possibilities divisible by target_np
	assert(adjusted_np % target_np == 0);

	printf("Random:");
	for (int i=0; i<20; ++i) {
		unsigned int ro;
		do
			ro = (unsigned int)rand();
		while (ro >= adjusted_np);
		unsigned int rt = ro % target_np;
		printf(" %u", rt);
	}
	printf("\n");

	return 0;
}
