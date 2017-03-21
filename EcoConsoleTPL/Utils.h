#pragma once
#include <typeinfo>
#include <string>
#include <iterator>
#include <vector>
#include <iostream>
#include <sstream>

template <typename T>
std::vector<std::string> Split(T&& input) {
	std::stringstream ss{ std::forward<T>(input) };
	return std::vector<std::string> {std::istream_iterator<std::string>(ss), std::istream_iterator<std::string>{} };
}

// Throw format
inline void ThrowInternal(std::stringstream& ss) {
	throw std::logic_error(ss.str());
}

template<typename T, typename...ARGS>
void ThrowInternal(std::stringstream& ss, T&& elem, ARGS&&...rest) {
	ss << std::forward<T>(elem);
	ThrowInternal(ss, std::forward<ARGS>(rest)...);
}

template <typename...ARGS>
void Throw(ARGS&&...args) {
	std::stringstream ss;
	ThrowInternal(ss, std::forward<ARGS>(args)...);
}

// Calc depth string
inline std::string DepthToString(const std::vector<std::string>& depth) {
	std::stringstream ss;
	for (auto&& str : depth) {
		ss << str << ' ';
	}
	return ss.str();
}

// ForEach tuple
template <size_t CUR, size_t END>
struct ForEachIter_t {
	template <typename CALLABLE, typename...TYPES>
	static void Do(const std::tuple<TYPES...>& tpl, CALLABLE&& func) {
		auto& tplItem = std::get<CUR>(tpl);
		func(tplItem);
		ForEachIter_t<CUR + 1, END>::Do(tpl, func);
	}
};

template <size_t CUR>
struct ForEachIter_t<CUR, CUR> {
	template <typename CALLABLE, typename...TYPES>
	static void Do(const std::tuple<TYPES...>&, CALLABLE&&) {};
};

template <typename CALLABLE, typename...TYPES>
void ForEach(const std::tuple<TYPES...>& tpl, CALLABLE&& func) {
	constexpr size_t tplSize = std::tuple_size<std::tuple<TYPES...>>::value;
	ForEachIter_t<0, tplSize>::Do(tpl, func);
}
