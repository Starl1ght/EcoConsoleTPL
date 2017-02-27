#pragma once
#include <string>
#include <iterator>
#include <vector>
#include <iostream>
#include <sstream>

inline std::vector<std::string> Split(std::string&& input) {
	std::stringstream ss{ input };
	return std::vector<std::string> {std::istream_iterator<std::string>(ss), std::istream_iterator<std::string>{} };
}

// Throw format

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

// Help Helper

template<size_t C, typename CMD>
struct HelpHelper {};

template <typename CMD>
struct HelpHelper<2, CMD> {
	static void Do(std::stringstream& ss) {
		ss << " TYPES: " << typeid(CMD::ArgType1).name() << ' ' << typeid(CMD::ArgType2).name();
	}
};

template <typename CMD>
struct HelpHelper<1, CMD> {
	static void Do(std::stringstream& ss) {
		ss << " TYPES: " << typeid(CMD::ArgType1).name();
	}
};

template <typename CMD>
struct HelpHelper<0, CMD> {
	static void Do(std::stringstream& ss) {}
};

// Show Help
template <typename...ARGS>
void ShowHelp(const std::tuple<ARGS...>& cmds) {
	std::vector<std::string> out;
	ForEach(cmds, [&out](const auto& command) {
		using T = typename std::remove_const_t<std::remove_reference_t<decltype(command)>>;
		std::stringstream ss;
		ss << "Command '" << command.GetName() << "' ARGC: ";
		ss << T::ArgCount;
		HelpHelper<T::ArgCount, T>::Do(ss);
		out.push_back(ss.str());
	});
	for (const auto& str : out) {
		std::cout << str << '\n';
	}
}