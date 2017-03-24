#pragma once
#include "Converter.h"

using iter = std::vector<std::string>::const_iterator;

template<size_t COUNT, typename...ARGS>
struct Invoke {
	template <typename CALLABLE>
	static void Do(CALLABLE&& func, const iter&) {
		static_assert(std::is_same<CALLABLE, void>::value, "Specialize for more args");
	}
};

template <typename...ARGS>
struct Invoke<0, ARGS...> {
	template <typename CALLABLE>
	static void Do(CALLABLE&& func, const iter&) {
		func();
	}
};

template <typename...ARGS>
struct Invoke<1, ARGS...> {
	template <typename CALLABLE>
	static void Do(CALLABLE&& func, const iter& curr) {
		const auto& arg1 = Converter<typename std::tuple_element<0, std::tuple<ARGS...>>::type>(*(curr + 1));
		func(arg1);
	}
};

template <typename...ARGS>
struct Invoke<2, ARGS...> {
	template <typename CALLABLE>
	static void Do(CALLABLE&& func, const iter& curr) {
		const auto& arg1 = Converter<typename std::tuple_element<0, std::tuple<ARGS...>>::type>(*(curr + 1));
		const auto& arg2 = Converter<typename std::tuple_element<1, std::tuple<ARGS...>>::type>(*(curr + 2));
		func(arg1, arg2);
	}
};