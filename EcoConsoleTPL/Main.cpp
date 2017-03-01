#include <tuple>
#include "Command.h"
#include "Utils.h"
#include "Invoke.h"
#include <iostream>
#include "Callbacks.h"

template <typename CALLABLE, size_t ARGC, typename T1, typename T2>
void IterateTuple(const iter& curr, const iter& last, const Command_t<CALLABLE, ARGC, T1, T2>& command) {
	if (command.GetName() == *curr) {
		if (std::distance(curr, last) != ARGC + 1) {
			Throw("Excepted ", ARGC, " arguments, got ", std::distance(curr, last) - 1);
		}
		auto fn = command.GetFunc();
		Invoke<ARGC, T1, T2>::Do(fn, curr);
	}
}

template <typename...ARGS>
void IterateTuple(const iter& curr, const iter& last, const Branch_t<ARGS...>& branch) {
	if (branch.GetName() == *curr) {
		if (std::distance(curr, last) == 1) {
			Throw("Excepted command after branch, got none");
		}
		ForEach(branch.GetChildren(), [&curr, &last](const auto &elem) {
			IterateTuple(curr + 1, last, elem);
		});
	}
}

template <typename...ARGS>
void MagicStartsHere(const std::tuple<ARGS...>& cmds, const std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		return;
	}

	if (tokens[0] == "help") {
		ShowHelp(cmds);
		return;
	}

	ForEach(cmds, [&tokens] (const auto& elem) {
		IterateTuple(tokens.cbegin(), tokens.cend(), elem);
	});
}

int main() {
	const auto commands = std::make_tuple(
		MakeBranch("branch",
			MakeCommand("cmd", br_cmd),
			MakeCommand("cmd2", br_cmd)
		),
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
