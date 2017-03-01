#pragma once
#include <typeinfo>
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
inline void ThrowInternal(std::stringstream& ss) {
	throw std::logic_error(ss.str());
}

template<typename T, typename...ARGS>
void ThrowInternal(std::stringstream& ss, const T& elem, const ARGS&...rest) {
	ss << elem;
	ThrowInternal(ss, rest...);
}

template <typename...ARGS>
void Throw(const ARGS&...args) {
	std::stringstream ss;
	ThrowInternal(ss, args...);
}

// Calc depth string
std::string DepthToString(std::vector<std::string>& depth) {
	std::stringstream ss;
	for (const auto& str : depth) {
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

// Help Helper
#ifdef _WIN32
#define DEMANGLE(symbols) symbols
#else
#include <cxxabi.h>
std::string DEMANGLE(const std::string& mangled) {
	int status;
	char* demangled = abi::__cxa_demangle(mangled.c_str(), 0, 0, &status);
	const std::string ret { demangled };
	free(demangled);
	return ret;
}
#endif

template<size_t C, typename CMD>
struct HelpHelper {};

template <typename CMD>
struct HelpHelper<2, CMD> {
	static void Do(std::stringstream& ss) {
		const std::string tname1 = DEMANGLE(typeid(typename CMD::ArgType1).name());
		const std::string tname2 = DEMANGLE(typeid(typename CMD::ArgType2).name());
		ss << " TYPES: " << tname1 << ' ' << tname2;
	}
};

template <typename CMD>
struct HelpHelper<1, CMD> {
	static void Do(std::stringstream& ss) {
		ss << " TYPES: " << DEMANGLE(typeid(typename CMD::ArgType1).name());
	}
};

template <typename CMD>
struct HelpHelper<0, CMD> {
	static void Do(std::stringstream& ss) {}
};

// Show Help

template <typename CALLABLE, size_t ARGC, typename T1, typename T2>
void HelpIterateTuple(std::vector<std::string>& out, std::vector<std::string>& depth, const Command_t<CALLABLE, ARGC, T1, T2>& command) {
	std::stringstream ss;
	ss << DepthToString(depth) << "-> " << command.GetName();
	HelpHelper<ARGC, Command_t<CALLABLE, ARGC, T1, T2>>::Do(ss);
	out.push_back(ss.str());
};

template <typename...ARGS>
void HelpIterateTuple(std::vector<std::string>& out, std::vector<std::string>& depth, const Branch_t<ARGS...>& branch) {
	depth.push_back(branch.GetName());
	ForEach(branch.GetChildren(), [&out, &depth] (const auto& elem) {
		HelpIterateTuple(out, depth, elem);
	});
	depth.pop_back();
}


template <typename...ARGS>
void ShowHelp(const std::tuple<ARGS...>& cmds) {
	std::vector<std::string> out;
	std::vector<std::string> depth;

	ForEach(cmds, [&out, &depth] (const auto& elem) {
		HelpIterateTuple(out, depth, elem);
	});
	for (const auto& str : out) {
		std::cout << str << '\n';
	}
}
