#pragma once

#include <memory>
#include <sstream>
#include <mbgl/style/function/expression.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace conversion {

using namespace mbgl::style::expression;

template<> struct Converter<std::unique_ptr<Expression>> {
    template <class V>
    std::string getJSType(const V& value) const {
        if (isUndefined(value)) {
            return "undefined";
        }
        if (isArray(value) || isObject(value)) {
            return "object";
        }
        optional<mbgl::Value> v = toValue(value);
        assert(v);
        return v->match(
            [&] (std::string) { return "string"; },
            [&] (bool) { return "boolean"; },
            [&] (auto) { return "number"; }
        );
    }

    template <class V>
    optional<std::unique_ptr<Expression>> operator()(const V& value, Error& error) const {
        if (isArray(value)) {
            if (arrayLength(value) == 0) {
                error = { "Expected an array with at least one element. If you wanted a literal array, use [\"literal\", []]." };
                return {};
            }
            
            const optional<std::string>& op = toString(arrayMember(value, 0));
            if (!op) {
                std::ostringstream ss;
                ss << "Expression name must be a string, but found "
                    << getJSType(arrayMember(value, 0))
                    << " instead. If you wanted a literal array, use [\"literal\", [...]].";
                error = { ss.str() };
                return {};
            }
        }
        return {std::make_unique<LiteralExpression>("", 1.0f)};
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
