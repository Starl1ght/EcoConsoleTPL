#pragma once
#include <string>

template <typename CALLABLE, typename...TYPES>
class Command_t {
	template <typename...ARGS>
	friend auto MakeCommand(std::string&& name, void(*func)(ARGS...));
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

template <typename...ARGS>
auto MakeCommand(std::string&& name, void(*func)(ARGS...)) {
	return Command_t<decltype(func), std::decay_t<ARGS>...>(move(name), func);
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
