#pragma once
#include <string>
#include <algorithm>
#include <locale>

template <typename T>
T Converter(const std::string&) {
	static_assert(std::is_same<T, void>::value, "Converter for this type is not specialized");
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
inline double Converter<double>(const std::string& str) {
	try {
		return stod(str);
	} catch (...) {
		Throw("Unable to convert '", str, "' to double");
	}
}

template<>
inline unsigned long Converter<unsigned long>(const std::string& str) {
	try {
		if (str.front() == '-') throw std::exception{};
		return stoul(str);
	} catch (...) {
		Throw("Unable to convert '", str, "' to unsigned long");
	}
}

template<>
inline std::string Converter<std::string>(const std::string& str) {
	return str;
}

template<>
inline bool Converter<bool>(const std::string& str) {
	std::string lowered;
	transform(str.begin(), str.end(), std::back_inserter(lowered), ::tolower);
	if (lowered == "true" || lowered == "1" || lowered == "enable" || lowered == "enabled") {
		return true;
	}
	if (lowered == "false" || lowered == "0" || lowered == "disable" || lowered == "disabled") {
		return false;
	}
	Throw("Unable to convert '", str, "' to bool");
}