#pragma once
#include <string>
#include <iterator>
#include <vector>
#include <sstream>

inline std::vector<std::string> Split(std::string&& input) {
	std::stringstream ss{ input };
	return std::vector<std::string> {std::istream_iterator<std::string>(ss), std::istream_iterator<std::string>{} };
}

template<typename T, typename...ARGS>
void ThrowInternal(std::stringstream& ss, const T& elem, const ARGS&...rest) {
	ss << elem;
	ThrowInternal(ss, rest...);
}

template<typename T>
void ThrowInternal(std::stringstream& ss, const T& elem) {
	ss << elem;
	throw std::logic_error(ss.str());
}

template <typename...ARGS>
void Throw(const ARGS&...args) {
	std::stringstream ss;
	ThrowInternal(ss, args...);
}

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
