#include <tuple>
#include "Command.h"
#include "Utils.h"
#include "Invoke.h"
#include <iostream>
#include "Callbacks.h"

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

template <typename...ARGS>
void MagicStartsHere(const std::tuple<ARGS...>& cmds, const std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		return;
	}

	if (tokens[0] == "help") {
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
		return;
	}

	bool found = false;
	auto fn = [&tokens, &found](const auto& command) {
		if (command.GetName() == tokens.front()) {
			found = true;
			using T = typename std::remove_const_t<std::remove_reference_t<decltype(command)>>;
			if (tokens.size() != T::ArgCount + 1) {
				Throw("Excepted ", T::ArgCount, " arguments, got ", tokens.size() - 1);
			}
			auto fn = command.GetFunc();
			Invoke<T::ArgCount, T::ArgType1, T::ArgType2>::Do(fn, tokens);
		}
	};

	ForEach(cmds, fn);
	if (!found) {
		Throw("Command '", tokens[0], "' is not found");
	}
}

void main() {
	const auto commands = std::make_tuple(
		MakeCommand("void", void0),
		MakeCommand("int1", int1),
		MakeCommand("float2", float2)
	);

	while (true) {
		std::cout << "> ";
		std::string inp;
		getline(std::cin, inp);
		const auto tokens = Split(move(inp));
		try {
			MagicStartsHere(commands, tokens);
		} catch (const std::exception& e) {
			std::cout << "ERROR: " << e.what() << '\n';
		}
	}
}
