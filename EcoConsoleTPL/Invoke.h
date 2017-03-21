#pragma once
#include "Converter.h"

using iter = std::vector<std::string>::const_iterator;

template<size_t COUNT, typename ARGT1, typename ARGT2>
struct Invoke {
	template <typename CALLABLE>
	static void Do(CALLABLE&& func, const iter&) {
		static_assert(std::is_same<CALLABLE, void>::value, "Specialize for more args");
	}
};

template <typename ARGT1, typename ARGT2>
struct Invoke<0, ARGT1, ARGT2> {
	template <typename CALLABLE>
	static void Do(CALLABLE&& func, const iter&) {
		func();
	}
};

template <typename ARGT1, typename ARGT2>
struct Invoke<1, ARGT1, ARGT2> {
	template <typename CALLABLE>
	static void Do(CALLABLE&& func, const iter& curr) {
		const auto& arg1 = Converter<ARGT1>(*(curr + 1));
		func(arg1);
	}
};

template <typename ARGT1, typename ARGT2>
struct Invoke<2, ARGT1, ARGT2> {
	template <typename CALLABLE>
	static void Do(CALLABLE&& func, const iter& curr) {
		const auto& arg1 = Converter<ARGT1>(*(curr + 1));
		const auto& arg2 = Converter<ARGT2>(*(curr + 2));
		func(arg1, arg2);
	}
};