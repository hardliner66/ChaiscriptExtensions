#ifndef _CHAI_EXTENSIONS_MAYBE_
#define _CHAI_EXTENSIONS_MAYBE_
#include <chaiscript/chaiscript.hpp>

class Maybe {
public:
    explicit Maybe(chaiscript::Boxed_Value v) : m_value(v) {}
    explicit Maybe() {}

    chaiscript::Boxed_Value operator>>(std::function<chaiscript::Boxed_Value(chaiscript::Boxed_Value)> other) const;

    std::string str() const
    {
        if (m_value.is_undef()) {
            return std::string("Nothing()");
        }
        std::stringstream ss;
        ss << "Just(<" << m_value.get_type_info().name() << ">)";
        return ss.str();
    }

private:
    const chaiscript::Boxed_Value m_value;
};

inline Maybe& nothing() {
    static Maybe instance;
    return instance;
}

inline chaiscript::Boxed_Value Maybe::operator>>(std::function<chaiscript::Boxed_Value(chaiscript::Boxed_Value)> other) const {
    if (m_value.is_undef()) {
        return chaiscript::Boxed_Value(this);
    }
    auto result = other(m_value);
    if (result.is_null() || result.is_undef()) {
        return chaiscript::Boxed_Value(nothing());
    }
    if (result.get_type_info().name() == "class Maybe")
    {
        return result;
    }
    return chaiscript::Boxed_Value(Maybe(result));
}

inline void register_maybe(chaiscript::ChaiScript& chai) {
    chai.add(chaiscript::user_type<Maybe>(), "Maybe");
    chai.add_global_const(chaiscript::const_var(nothing()), "nothing");
    chai.add(chaiscript::constructor<Maybe(chaiscript::Boxed_Value)>(), "Just");
    chai.add(chaiscript::base_class<Maybe, Maybe>());
    chai.add(chaiscript::base_class<Maybe, Maybe>());

    chai.add(chaiscript::fun(&Maybe::operator>>), ">>");
    chai.add(chaiscript::fun(&Maybe::str), "to_string");
}

#endif//_CHAI_EXTENSIONS_MAYBE_