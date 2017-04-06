#pragma once
#include <iostream>
#include "Config.h"
#include "Utils.h"

namespace fn {
	inline void sum(int a, int b) {
		std::cout << a << " + " << b << " = " << a + b << '\n';
	}
	inline void div(const double& a, const double& b) {
		std::cout << a << " / " << b << " = " << a / b << '\n';
	}
	inline void square(const unsigned long& a) {
		std::cout << a << " ^ 2 = " << a * a << '\n';
	}
	inline void reverse(const std::string& str) {
		std::cout << str << " -> " << std::string{ str.rbegin(), str.rend() } << '\n';
	}
	inline void con_exit() {
		exit(0);
	}

	inline void edit() {
		Throw("You cannot edit nothing");
	}
	template<typename...ARGS>
	void edit(const ARGS&...args) {
		StartEdit(ConfigTree, args...);
	}

	template<typename...ARGS>
	void view(const ARGS&...args) {
		StartView(ConfigTree, args...);
	}

	inline void view() {
		StartView(ConfigTree);
	}
}
