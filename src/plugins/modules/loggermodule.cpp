#include "loggermodule.hpp"

#include <sstream>

#include <boost/log/trivial.hpp>

using pika::Plugins::Modules::LoggerModule;

void LoggerModule::Push(duk_context *ctx)
{
    duk_push_object(ctx);
    duk_push_c_function(ctx, Func_Info, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "info");
}

duk_ret_t LoggerModule::Func_Info(duk_context *ctx)
{
    duk_push_global_stash(ctx);
    duk_get_prop_string(ctx, -1, "\xff" "Name");
    std::string name = duk_get_string(ctx, -1);
    duk_pop_2(ctx);

    BOOST_LOG_TRIVIAL(info) << "(" << name << ") " << duk_to_string(ctx, 0);

    return 0;
}
