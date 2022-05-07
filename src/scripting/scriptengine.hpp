#pragma once

#include <string>

#include "duktape.h"

namespace pika::Scripting
{
    class Wrapper
    {
    public:
        virtual duk_ret_t operator ()(duk_context* ctx) = 0;
    };

    class IScriptEngine
    {
    public:
        IScriptEngine() = default;
        virtual ~IScriptEngine() = default;

        virtual void Emit(const std::string &name, Wrapper *wrapper) = 0;
    };
}
