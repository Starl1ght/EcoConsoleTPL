#include <tuple>
#include "Command.h"
#include "Utils.h"
#include "Converter.h"
#include <iostream>

void void0() {
	std::cout << "CALLED 'void'\n";
}

void int1(const int& i) {
	std::cout << "CALLED 'int1' ARGS: " << i << '\n';
}

void float2(const float& f1, const float& f2) {
	std::cout << "CALLED 'float2' ARGS: " << f1 << " " << f2 << '\n';
}

template<size_t COUNT, typename ARGT1, typename ARGT2>
struct Invoke {};

template <typename ARGT1, typename ARGT2>
struct Invoke<0, ARGT1, ARGT2> {
	template <typename CALLABLE>
	static void Do(CALLABLE func, const std::vector<std::string>&) {
		func();
	}
};

template <typename ARGT1, typename ARGT2>
struct Invoke<1, ARGT1, ARGT2> {
	template <typename CALLABLE>
	static void Do(CALLABLE func, const std::vector<std::string>& tokens) {
		const auto& arg1 = Converter<ARGT1>(tokens[1]);
		func(arg1);
	}
};

template <typename ARGT1, typename ARGT2>
struct Invoke<2, ARGT1, ARGT2> {
	template <typename CALLABLE>
	static void Do(CALLABLE func, const std::vector<std::string>& tokens) {
		const auto& arg1 = Converter<ARGT2>(tokens[1]);
		const auto& arg2 = Converter<ARGT2>(tokens[2]);
		func(arg1, arg2);
	}
};

template<size_t C, typename CMD>
struct HelpHelper {};


template <typename CMD> 
struct HelpHelper<2, CMD> {
	static void Do(std::stringstream& ss) {
		ss << " TYPES: " << typeid(CMD::ArgType1).name() << ' ' << typeid(CMD::ArgType1).name();
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
			ss << T::ArgCount::value;
			HelpHelper<T::ArgCount::value, T>::Do(ss);
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
			if (tokens.size() != T::ArgCount() + 1) {
				Throw("Excepted ", T::ArgCount::value, " arguments, got ", tokens.size() - 1);
			}
			auto fn = command.GetFunc();
			Invoke<T::ArgCount::value, T::ArgType1, T::ArgType2>::Do(fn, tokens);
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
