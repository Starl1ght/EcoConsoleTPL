#pragma once
#include <iostream>

inline void void0() {
	std::cout << "CALLED 'void'\n";
}

inline void int1(const int& i) {
	std::cout << "CALLED 'int1' ARGS: " << i << '\n';
}

inline void float2(const float& f1, const float& f2) {
	std::cout << "CALLED 'float2' ARGS: " << f1 << " " << f2 << '\n';
}