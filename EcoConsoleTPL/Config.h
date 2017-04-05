#pragma once

template <typename T>
class Field_t {
	template <typename VALTYPE>
	friend Field_t<VALTYPE> MakeField(std::string&& name, VALTYPE&& val);
public:
	const std::string& GetName() const {
		return m_name;
	}

	const T& Get() const {
		return m_val;
	}

	void Set(const T& val) {
		m_val = val;
	}

private:
	Field_t(std::string&& name, T&& val) :
		m_name(name), m_val(std::forward<T>(val)) {}
	Field_t(std::string&& name, const T& val) :
		m_name(name), m_val(val) {}

	const std::string m_name;
	T m_val;
};

template <typename VALTYPE>
Field_t<VALTYPE> MakeField(std::string&& name, VALTYPE&& val) {
	return Field_t<VALTYPE>(move(name), std::forward<VALTYPE>(val));
}

template <typename...ARGS>
class Section_t {
	template <typename...FIELDS>
	friend Section_t<FIELDS...> MakeSection(std::string&& name, FIELDS&&...fields);

public:
	std::tuple<ARGS...>& GetChildren() {
		return m_children;
	}

	const std::tuple<ARGS...>& GetChildren() const {
		return m_children;
	}

	const std::string& GetName() const {
		return m_name;
	}

private:
	Section_t(std::string&& name, std::tuple<ARGS...>&& tpl) :
		m_name(name), m_children(std::forward<std::tuple<ARGS...>>(tpl)) {}

	const std::string m_name;
	std::tuple<ARGS...> m_children;
};


template<typename...FIELDS>
Section_t<FIELDS...> MakeSection(std::string&& name, FIELDS&&...fields) {
	return Section_t<FIELDS...>(move(name), std::forward_as_tuple(fields...));
}

template <typename T>
bool EditConfTree(const Field_t<T>& field) {
	Throw("New value is missing for '", field.GetName(), "'");
	return false;
}

template <typename T>
typename std::enable_if<!std::is_same<T, std::string>::value, bool>::type
EditConfTree(Field_t<T>& field, const T& val) {
	field.Set(val);
	return true;
}

template <typename T>
bool EditConfTree(Field_t<T>& field, const std::string& toConvert) {
	field.Set(Converter<T>(toConvert));
	return true;
}

template <typename T, typename...DUMMY>
bool EditConfTree(const Field_t<T>&, const std::string&, DUMMY&&...) {
	Throw("Extra arguments supplied");
	return false;
}

template <typename T, typename...DUMMY>
typename std::enable_if<!std::is_same<T,std::string>::value, bool>::type
EditConfTree(const Field_t<T>&, const T&, DUMMY&&...) {
	Throw("Extra arguments supplied");
	return false;
}

template <typename...FIELDS>
bool EditConfTree(Section_t<FIELDS...>& sect) {
	Throw("Field inside section '", sect.GetName(), "' and new value required");
	return false;
}

template <typename...FIELDS, typename...STRINGS>
bool EditConfTree(Section_t<FIELDS...>& config, const std::string& cmd, const STRINGS&...rest) {
	bool found = false;
	ForEach(config.GetChildren(), [&](auto&& elem) {
		if (elem.GetName() == cmd) {
			found |= EditConfTree(elem, rest...);
		}
	});
	return found;
}

template <typename...FIELDS, typename...STRINGS>
void StartEdit(std::tuple<FIELDS...>& config, const std::string& cmd, const STRINGS&...rest) {
	bool found{ false };
	ForEach(config, [&](auto& elem) {
		if (elem.GetName() == cmd) {
			found |= EditConfTree(elem, rest...);
		}
	});
	if (!found) {
		Throw("No such field '", cmd, "'");
	}
}

inline std::string GenSpaces(size_t sp) {
	return std::string(sp, ' ');
}

template <typename T>
void DumpTree(std::ostream& os, const Field_t<T>& field, size_t spaces = 0) {
	os << GenSpaces(spaces) << field.GetName() << " : " << field.Get() << '\n';
}

template <typename...ARGS>
void DumpTree(std::ostream& os, const Section_t<ARGS...>& sect, size_t spaces = 0) {
	const auto spc = GenSpaces(spaces);
	os << spc << sect.GetName() << " {\n";

	ForEach(sect.GetChildren(), [&](auto& elem) {
		DumpTree(os, elem, spaces + 4);
	});
	os << spc << "}\n";
}

template <typename...ARGS>
void DumpTree(std::ostream& os, const std::tuple<ARGS...>& tpl) {
	ForEach(tpl, [&](auto& elem) {
		DumpTree(os, elem);
	});
}

template <typename T>
bool ViewConfTree(const Field_t<T>& field) {
	DumpTree(std::cout, field);
	return true;
}

template <typename T, typename...STRINGS>
bool ViewConfTree(Field_t<T>& field, const STRINGS&...dummy) {
	if (sizeof...(dummy) != 0) {
		Throw("Additional useless arguments in 'view'");
	}
	DumpTree(std::cout, field);
	return true;
}

template <typename...FIELDS>
bool ViewConfTree(Section_t<FIELDS...>& config) {
	DumpTree(std::cout, config);
	return true;
}

template <typename...FIELDS, typename...STRINGS>
bool ViewConfTree(Section_t<FIELDS...>& config, const std::string& cmd, const STRINGS&...rest) {
	bool found = false;
	ForEach(config.GetChildren(), [&, sz = sizeof...(rest)](auto& elem) {
		if (elem.GetName() == cmd) {
			if (sz != 0) {
				found |= ViewConfTree(elem, rest...);
			}
			else {
				DumpTree(std::cout, elem);
				found = true;
				return;
			}
		}
	});
	return found;
}

template <typename...FIELDS, typename...STRINGS>
void StartView(std::tuple<FIELDS...>& config, const std::string& cmd, const STRINGS&...rest) {
	bool found{ false };
	ForEach(config, [&, sz = sizeof...(rest)](auto& elem) {
		if (elem.GetName() == cmd) {
			if (sz != 0) {
				found |= ViewConfTree(elem, rest...);
			}
			else {
				DumpTree(std::cout, elem);
				found = true;
				return;
			}
		}
		if (found)
			return;
	});
	if (!found) {
		Throw("No such field '", cmd, "'");
	}
}
template <typename...FIELDS>
void StartView(std::tuple<FIELDS...>& config) {
	DumpTree(std::cout, config);
}

static auto ConfigTree = std::make_tuple(
	MakeField("moo", true),
	MakeField("purr", 2),
	MakeSection("sect",
		MakeField("cat", std::string{ "very little!" }), 
		MakeField("doggo", std::string{ "very big!" }),
		MakeSection("deep",
			MakeSection("deeper",
				MakeField("^_^", 1)
			)
		)
	),
	MakeField("boop", 2.2222)
);
