#pragma once
#include <string>

template <typename T>
T Converter(const std::string&) {
	static_assert(false, "Converter for this type is not specialized");
	return T{};
}

template<>
inline int Converter<int>(const std::string& str) {
	try {
		return stoi(str);
	} catch (...) {
		Throw("Unable to convert '", str, "' to int");
	}
}

template <>
inline float Converter<float>(const std::string& str) {
	try {
		return stof(str);
	} catch (...) {
		Throw("Unable to convert '", str, "' to float");
	}
}
