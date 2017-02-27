#pragma once
#include "Converter.h"
template<size_t COUNT, typename ARGT1, typename ARGT2>
struct Invoke {};

template <typename ARGT1, typename ARGT2>
struct Invoke<0, ARGT1, ARGT2> {
	template <typename CALLABLE>
	static void Do(CALLABLE func, const std::vector<std::string>&) {
		func();
	}
};

template <typename ARGT1, typename ARGT2>
struct Invoke<1, ARGT1, ARGT2> {
	template <typename CALLABLE>
	static void Do(CALLABLE func, const std::vector<std::string>& tokens) {
		const auto& arg1 = Converter<ARGT1>(tokens[1]);
		func(arg1);
	}
};

template <typename ARGT1, typename ARGT2>
struct Invoke<2, ARGT1, ARGT2> {
	template <typename CALLABLE>
	static void Do(CALLABLE func, const std::vector<std::string>& tokens) {
		const auto& arg1 = Converter<ARGT1>(tokens[1]);
		const auto& arg2 = Converter<ARGT2>(tokens[2]);
		func(arg1, arg2);
	}
};