// This program first displays the current time, and then measures a time period.

#include <chrono>
#include <iostream>

void show_current_time()
{
	auto time = std::chrono::system_clock::now();

	auto duration = time.time_since_epoch();
	std::cout << "Current UNIX Time:" << std::endl;
	std::cout << std::chrono::duration<double>(duration).count() << " s" << std::endl;
	std::cout << std::chrono::duration<double, std::ratio<60>>(duration).count() << " min" << std::endl;
	std::cout << std::chrono::duration<double, std::ratio<3600>>(duration).count() << " h" << std::endl;
}

void measure_elapsed_time()
{
	std::cout << "Waiting for Enter to be pressed: ";
	auto start = std::chrono::steady_clock::now();
	std::cin.get();
	auto stop = std::chrono::steady_clock::now();

	auto duration = stop - start;
	std::cout << "Time waited:" << std::endl;
	std::cout << std::chrono::duration<double>(duration).count() << " s" << std::endl;
	std::cout << std::chrono::duration<double, std::milli>(duration).count() << " ms" << std::endl;
	std::cout << std::chrono::duration<double, std::micro>(duration).count() << " us" << std::endl;
	std::cout << std::chrono::duration<double, std::nano>(duration).count() << " ns" << std::endl;
}

int main()
{
	show_current_time();
	measure_elapsed_time();

	return 0;
}
