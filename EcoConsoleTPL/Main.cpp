#include <tuple>
#include "Command.h"
#include "Utils.h"
#include "Invoke.h"
#include "Callbacks.h"

template <typename CALLABLE, size_t ARGC, typename T1, typename T2>
bool ProcessTupleElem(const iter& curr, const iter& last, const Command_t<CALLABLE, ARGC, T1, T2>& command) {
	if (command.GetName() == *curr) {
		if (std::distance(curr, last) != ARGC + 1) {
			Throw("Excepted ", ARGC, " arguments, got ", std::distance(curr, last) - 1);
		}
		Invoke<ARGC, T1, T2>::Do(command.GetFunc(), curr);
		return true;
	}
	return false;
}

template <typename...ARGS>
bool ProcessTupleElem(const iter& curr, const iter& last, const Branch_t<ARGS...>& branch) {
	if (branch.GetName() == *curr) {
		if (std::distance(curr, last) == 1) {
			Throw("Excepted command after branch, got none");
		}
		bool found{ false };
		ForEach(branch.GetChildren(), [&found, &curr, &last] (const auto &elem) {
			found |= ProcessTupleElem(curr + 1, last, elem);
		});
		if (!found) {
			Throw("command '", *curr, "' not found in branch '", *(curr + 1), "'");
		}
		return true;
	}
	return false;
}

template <typename...ARGS>
void MagicStartsHere(const std::tuple<ARGS...>& cmds, std::vector<std::string>&& tokens) {
	if (tokens.empty()) return;
	bool found{ false };
	ForEach(cmds, [&found, &tokens] (auto&& elem) {
		found |= ProcessTupleElem(tokens.cbegin(), tokens.cend(), elem);
	});
	if (!found) {
		Throw("command '", tokens.front(), "' not found");
	}
}

int main() {
	const auto commands = std::make_tuple(
		MakeBranch("math",
			MakeCommand("sum", fn::sum),
			MakeCommand("div", fn::div),
			MakeCommand("square", fn::square)
		),
		MakeCommand("reverse", fn::reverse),
		MakeCommand("exit", fn::con_exit)
	);

	while (true) {
		std::cout << "> ";
		std::string inp;
		getline(std::cin, inp);
		try {
			MagicStartsHere(commands, Split(inp));
		} catch (const std::exception& e) {
			std::cout << "ERROR: " << e.what() << '\n';
		}
	}
}
