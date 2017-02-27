#include <tuple>
#include "Command.h"
#include "Utils.h"
#include "Invoke.h"
#include <iostream>
#include "Callbacks.h"

template <typename...ARGS>
void MagicStartsHere(const std::tuple<ARGS...>& cmds, const std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		return;
	}

	if (tokens[0] == "help") {
		ShowHelp(cmds);
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
