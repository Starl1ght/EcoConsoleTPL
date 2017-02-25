#pragma once
#include <string>

template <typename CALLABLE, int ARGCOUNT, typename ARG1 = void, typename ARG2 = void>
class Command_t {
public:
	const std::string& GetName() const {
		return m_name;
	}
	CALLABLE GetFunc() const {
		return m_func;
	}

	using ArgCount = std::integral_constant<size_t, ARGCOUNT>;
	using ArgType1 = ARG1;
	using ArgType2 = ARG2;
private:
	Command_t(std::string&& name, CALLABLE func) :
		m_name(name), m_func(func) {}

	const std::string m_name;
	const CALLABLE m_func;

	// Factory methods
	friend auto MakeCommand(std::string&& name, void(*func)());
	template <typename T1>
	friend auto MakeCommand(std::string&& name, void(*func)(const T1&));
	template <typename T1, typename T2>
	friend auto MakeCommand(std::string&& name, void(*func)(const T1&, const T2&));
};

inline auto MakeCommand(std::string&& name, void(*func)()) {
	return Command_t<decltype(func), 0>(move(name), func);
}

template <typename T1>
auto MakeCommand(std::string&& name, void(*func)(const T1&)) {
	return Command_t<decltype(func), 1, T1>(move(name), func);
}

template <typename T1, typename T2>
auto MakeCommand(std::string&& name, void(*func)(const T1&, const T2&)) {
	return Command_t<decltype(func), 2, T1, T2>(move(name), func);
}
