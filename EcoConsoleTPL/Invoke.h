#pragma once
#include "Converter.h"

template<typename...ARGS>
void Invoke(void(*func)(ARGS...args), std::vector<std::string>::const_iterator curr) {
	std::tuple<std::decay_t<ARGS>...> tempTpl;
	ForEach(tempTpl, [&curr](auto& elem) {
		elem = Converter<std::decay_t<decltype(elem)>>(*(++curr));
	});
	ExpandAndCall(tempTpl, func);
}
