#pragma once
#include <string>

template <typename CALLABLE, size_t ARGCOUNT, typename ARG1 = void, typename ARG2 = void>
class Command_t {
	friend auto MakeCommand(std::string&& name, void(*func)());
	template <typename T1>
	friend auto MakeCommand(std::string&& name, void(*func)(const T1&));
	template <typename T1, typename T2>
	friend auto MakeCommand(std::string&& name, void(*func)(const T1&, const T2&));
public:
	const std::string& GetName() const {
		return m_name;
	}
	const CALLABLE& GetFunc() const {
		return m_func;
	}
private:
	Command_t(std::string&& name, CALLABLE func) :
		m_name(std::forward<std::string>(name)), m_func(func) {}

	const std::string m_name;
	const CALLABLE m_func;
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

template <typename...CMDS>
class Branch_t {
	template <typename...ARGS>
	friend Branch_t<ARGS...> MakeBranch(std::string&& name, ARGS&&...args);
public:
	const std::string& GetName() const {
		return m_name;
	}
	const std::tuple<CMDS...>& GetChildren() const {
		return m_cmds;
	}
private:
	Branch_t(std::string&& name, CMDS&&...cmds) :
			m_name(std::forward<std::string>(name)),
            m_cmds(std::forward_as_tuple(cmds...)) {};

	const std::string m_name;
	const std::tuple<CMDS...> m_cmds;
};

template <typename...ARGS>
Branch_t<ARGS...> MakeBranch(std::string&& name, ARGS&&...args) {
	return Branch_t<ARGS...>(std::forward<std::string>(name), std::forward<ARGS>(args)...);
}