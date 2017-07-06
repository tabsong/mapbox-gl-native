#include "node_expression.hpp"
#include "node_conversion.hpp"

#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/conversion/expression.hpp>
#include <mbgl/util/geojson.hpp>
#include <nan.h>

using namespace mbgl::style;
using namespace mbgl::style::expression;

namespace node_mbgl {

Nan::Persistent<v8::Function> NodeExpression::constructor;

void NodeExpression::Init(v8::Local<v8::Object> target) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("Expression").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1); // what is this doing?

    Nan::SetPrototypeMethod(tpl, "evaluate", Evaluate);
    Nan::SetPrototypeMethod(tpl, "getType", GetType);
    Nan::SetPrototypeMethod(tpl, "isFeatureConstant", IsFeatureConstant);
    Nan::SetPrototypeMethod(tpl, "isZoomConstant", IsZoomConstant);

    constructor.Reset(tpl->GetFunction()); // what is this doing?
    Nan::Set(target, Nan::New("Expression").ToLocalChecked(), tpl->GetFunction());
}

void NodeExpression::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowTypeError("Use the new operator to create new Expression objects");
    }

    if (info.Length() < 1 || info[0]->IsUndefined()) {
        return Nan::ThrowTypeError("Requires a JSON style expression argument.");
    }

    auto expr = info[0];

    try {
        conversion::Error error;
        auto expression = conversion::convert<std::unique_ptr<Expression>>(expr, error);
        if (!expression) {
            return Nan::ThrowTypeError(error.message.c_str());
        }
        auto nodeExpr = new NodeExpression(std::move(*expression));
        nodeExpr->Wrap(info.This());
    } catch(std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }

    info.GetReturnValue().Set(info.This());
}

void NodeExpression::Evaluate(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    NodeExpression* nodeExpr = ObjectWrap::Unwrap<NodeExpression>(info.Holder());

    if (info.Length() < 2) {
        return Nan::ThrowTypeError("Requires arguments zoom and feature arguments.");
    }

    float zoom = info[0]->NumberValue();

    // Pending https://github.com/mapbox/mapbox-gl-native/issues/5623,
    // stringify the geojson feature in order to use convert<GeoJSON, string>()
    Nan::JSON NanJSON;
    Nan::MaybeLocal<v8::String> geojsonString = NanJSON.Stringify(Nan::To<v8::Object>(info[1]).ToLocalChecked());
    if (geojsonString.IsEmpty()) {
        return Nan::ThrowTypeError("couldn't stringify JSON");
    }
    conversion::Error conversionError;
    mbgl::optional<mbgl::GeoJSON> geoJSON = conversion::convert<mbgl::GeoJSON, std::string>(*Nan::Utf8String(geojsonString.ToLocalChecked()), conversionError);
    if (!geoJSON) {
        Nan::ThrowTypeError(conversionError.message.c_str());
        return;
    }
    
    try {
        mapbox::geojson::feature feature = geoJSON->get<mapbox::geojson::feature>();
        Error error;
        auto result = nodeExpr->expression->evaluate(zoom, feature, error);
        if (result) {
            result->match(
                [&] (const std::array<float, 2>&) {},
                [&] (const std::array<float, 4>&) {},
                [&] (const std::string s) {
                    info.GetReturnValue().Set(Nan::New(s.c_str()).ToLocalChecked());
                },
                [&] (const mbgl::Color& c) {
                    info.GetReturnValue().Set(Nan::New(c.stringify().c_str()).ToLocalChecked());
                },
                [&] (const auto& v) {
                    info.GetReturnValue().Set(Nan::New(v));
                }
            );
        } else {
            v8::Local<v8::Object> res = Nan::New<v8::Object>();
            Nan::Set(res,
                    Nan::New("error").ToLocalChecked(),
                    Nan::New(error.message.c_str()).ToLocalChecked());
            info.GetReturnValue().Set(res);
        }
    } catch(std::exception &ex) {
        return Nan::ThrowTypeError(ex.what());
    }
}

void NodeExpression::GetType(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    NodeExpression* nodeExpr = ObjectWrap::Unwrap<NodeExpression>(info.Holder());
    const auto& type = nodeExpr->expression->getType();
    const auto& name = type.match([&] (const auto& t) { return t.getName(); });
    info.GetReturnValue().Set(Nan::New(name.c_str()).ToLocalChecked());
}

void NodeExpression::IsFeatureConstant(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    NodeExpression* nodeExpr = ObjectWrap::Unwrap<NodeExpression>(info.Holder());
    info.GetReturnValue().Set(Nan::New(nodeExpr->expression->isFeatureConstant()));
}

void NodeExpression::IsZoomConstant(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    NodeExpression* nodeExpr = ObjectWrap::Unwrap<NodeExpression>(info.Holder());
    info.GetReturnValue().Set(Nan::New(nodeExpr->expression->isZoomConstant()));
}

} // namespace node_mbgl
