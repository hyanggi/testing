// This program generates random numbers according to a probability distribution.

#include <random>
#include <iostream>

int main()
{
	std::random_device rd; // This may be a true random number generator, so it may be slow.
	std::default_random_engine gen(rd()); // Default random number generator
	std::uniform_int_distribution<int> dist(0, 9); // Uniform integer distribution

	std::cout << "Random:";
	for (int i=0; i<20; ++i)
		std::cout << ' ' << dist(gen);
	std::cout << std::endl;

	return 0;
}
