#include <tuple>
#include "Command.h"
#include "Config.h"
#include "Invoke.h"
#include "Callbacks.h"

using namespace std::string_literals;

enum class IterResult {
	NotFound,
	WrongArgCount,
	Ok
};

IterResult NewGlobalResult(IterResult global, IterResult local) {
	if (local == IterResult::Ok) {
		return IterResult::Ok;
	}
	if (global == IterResult::NotFound && local == IterResult::WrongArgCount) {
		return IterResult::WrongArgCount;
	}
	return global;
}

template <typename CALLABLE, typename...TYPES>
IterResult ProcessTupleElem(std::vector<std::string>::const_iterator curr, std::vector<std::string>::const_iterator last, const Command_t<CALLABLE, TYPES...>& command) {
	if (command.GetName() == *curr) {
		if (std::distance(curr, last) != sizeof...(TYPES) + 1) {
			return IterResult::WrongArgCount;
		}
		Invoke(command.GetFunc(), curr);
		return IterResult::Ok;
	}
	return IterResult::NotFound;
}

template <typename...ARGS>
IterResult ProcessTupleElem(std::vector<std::string>::const_iterator curr, std::vector<std::string>::const_iterator last, const Branch_t<ARGS...>& branch) {
	if (branch.GetName() == *curr) {
		if (std::distance(curr, last) == 1) {
			Throw("Excepted command after branch, got none");
		}
		IterResult global{ IterResult::NotFound };
		ForEach(branch.GetChildren(), [&global, &curr, &last] (const auto &elem) {
			const auto local = ProcessTupleElem(curr + 1, last, elem);
			global = NewGlobalResult(global, local);
		});

		switch(global) {
			case IterResult::NotFound:
				Throw("command '", *(curr + 1), "' not found in branch '", *curr, "'");
				break;
			case IterResult::WrongArgCount:
				Throw("incorrect argument count");
				break;
			case IterResult::Ok:
				return IterResult::Ok;
		}
	}
	return IterResult::NotFound;
}

template <typename...ARGS>
void MagicStartsHere(const std::tuple<ARGS...>& cmds, std::vector<std::string>&& tokens) {
	if (tokens.empty()) return;
	IterResult global{ IterResult::NotFound };
	ForEach(cmds, [&global, &tokens] (auto&& elem) {
		const auto local = ProcessTupleElem(tokens.cbegin(), tokens.cend(), elem);
		global = NewGlobalResult(global, local);
	});

	switch(global) {
		case IterResult::NotFound:
			Throw("command '", tokens.front(), "' not found");
			break;
		case IterResult::WrongArgCount:
			Throw("incorrect argument count");
			break;
		case IterResult::Ok:
			break;
	}
}

int main() {
	const auto commands = std::make_tuple(
		MakeBranch("math",
			MakeCommand("sum", fn::sum),
			MakeCommand("div", fn::div),
			MakeCommand("square", fn::square)
		),
		MakeCommand("edit", fn::edit),
		MakeCommand("edit", fn::edit<std::string>),
		MakeCommand("edit", fn::edit<std::string, std::string>),
		MakeCommand("edit", fn::edit<std::string, std::string, std::string>),
		MakeCommand("edit", fn::edit<std::string, std::string, std::string, std::string>),
		MakeCommand("view", fn::view),
		MakeCommand("view", fn::view<std::string>),
		MakeCommand("view", fn::view<std::string, std::string>),
		MakeCommand("view", fn::view<std::string, std::string, std::string>),
		MakeCommand("view", fn::view<std::string, std::string, std::string, std::string>),
		MakeCommand("reverse", fn::reverse),
		MakeCommand("exit", fn::con_exit)
	);

	fn::edit("sect"s, "cat"s, "AMAZING GOSHMAN"s);
	//fn::edit("moo"s, true);

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
