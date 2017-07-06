#pragma once

#include <array>
#include <vector>
#include <memory>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/style/function/type.hpp>
#include <mbgl/util/feature.hpp>


namespace mbgl {

class GeometryTileFeature;


namespace style {
namespace expression {

using OutputValue = variant<
    float,
    std::string,
    mbgl::Color,
    std::array<float, 2>,
    std::array<float, 4>>;

class Error {
public:
    Error() {}
    Error(std::string message_) : message(message_) {}
    std::string message;
};

class Expression {
public:
    Expression(std::string key_, type::Type type_) : key(key_), type(type_) {}
    virtual ~Expression() {}
    
    virtual optional<OutputValue> evaluate(float, const GeometryTileFeature&, Error& e) const = 0;
    
    // Exposed for use with pure Feature objects (e.g. beyond the context of tiled map data).
    optional<OutputValue> evaluate(float, const Feature&, Error&) const;
    
    type::Type getType() {
        return type;
    }
    
    bool isFeatureConstant() {
        return true;
    }
    
    bool isZoomConstant() {
        return true;
    }
    
private:
    std::string key;
    type::Type type;
};

class LiteralExpression : public Expression {
public:
    LiteralExpression(std::string key, OutputValue value_) :
        Expression(key, value_.match(
            [&] (const float&) -> type::Type { return type::Primitive::Number; },
            [&] (const std::string&) { return type::Primitive::String; },
            [&] (const mbgl::Color) { return type::Primitive::Color; },
            [&] (const auto&) { return type::Primitive::Null; } // TODO (remaining output types)
        )),
        value(value_)
    {}
    
    optional<OutputValue> evaluate(float, const GeometryTileFeature&, Error&) const override {
        return {value};
    }
    
private:
    OutputValue value;
};

class LambdaExpression : public Expression {
public:
    LambdaExpression(std::string key,
                    type::Lambda type,
                    std::string name_,
                    std::vector<std::unique_ptr<Expression>> args_) :
        Expression(key, type),
        name(name_),
        args(std::move(args_))
    {}
    
private:
    std::string name;
    std::vector<std::unique_ptr<Expression>> args;
};



} // namespace expression
} // namespace style
} // namespace mbgl
