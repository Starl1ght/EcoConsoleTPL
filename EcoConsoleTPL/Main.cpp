#include <tuple>
#include "Command.h"
#include "Utils.h"
#include "Invoke.h"
#include "Callbacks.h"

template <typename CALLABLE, typename...TYPES>
bool ProcessTupleElem(std::vector<std::string>::const_iterator curr, std::vector<std::string>::const_iterator last, const Command_t<CALLABLE, TYPES...>& command) {
	if (command.GetName() == *curr) {
		if (std::distance(curr, last) != sizeof...(TYPES) + 1) {
			Throw("Excepted ", sizeof...(TYPES), " arguments, got ", std::distance(curr, last) - 1);
		}
		Invoke(command.GetFunc(), curr);
		return true;
	}
	return false;
}

template <typename...ARGS>
bool ProcessTupleElem(std::vector<std::string>::const_iterator curr, std::vector<std::string>::const_iterator last, const Branch_t<ARGS...>& branch) {
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
