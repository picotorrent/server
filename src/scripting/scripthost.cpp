#include "scripthost.hpp"

#include <boost/log/trivial.hpp>

#include "../io/file.hpp"
#include "../../vendor/quickjs/quickjs.h"

#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

using pt::Server::IO::File;
using pt::Server::Scripting::ScriptHost;

struct ScriptHost::Context
{
    JSContext* ctx;
};

struct ScriptHost::Runtime
{
    JSRuntime* rt;
};

static JSValue ConsoleLog(JSContext* ctx, JSValue self, int argc, JSValue* args)
{
    BOOST_LOG_TRIVIAL(info) << JS_ToCString(ctx, args[0]);
    return JS_NewInt32(ctx, 1);
}

static JSValue PicoTorrentSessionOn(JSContext* ctx, JSValue self, int argc, JSValue* args)
{
    JS_Call(ctx, args[1], self, 0, nullptr);
    return JS_NewInt32(ctx, 1);
}

static const JSCFunctionListEntry PicoTorrentSessionProps[] =
{
    JS_CFUNC_DEF("on", 2, PicoTorrentSessionOn)
};

static const JSCFunctionListEntry PicoTorrentFuncs[] =
{
    JS_OBJECT_DEF("session", PicoTorrentSessionProps, countof(PicoTorrentSessionProps), JS_PROP_CONFIGURABLE)
};

static int InitPicoTorrentModule(JSContext* ctx, JSModuleDef* def)
{
    return JS_SetModuleExportList(ctx, def, PicoTorrentFuncs, countof(PicoTorrentFuncs));
}

ScriptHost::ScriptHost()
{
    m_runtime = std::make_unique<Runtime>();
    m_runtime->rt = JS_NewRuntime();
}

ScriptHost::~ScriptHost()
{
}

void ScriptHost::Load(const fs::path &path)
{
    fs::path main = path / "main.js";

    if (!fs::exists(main))
    {
        BOOST_LOG_TRIVIAL(warning) << "No 'main.js' found at " << path;
        return;
    }

    std::vector<char> data;
    File::ReadAllBytes(main, data);

    auto context = std::make_unique<Context>();
    context->ctx = JS_NewContext(m_runtime->rt);

    // Set a reference back to our container context
    JS_SetContextOpaque(context->ctx, context.get());

    JSValue console = JS_NewObject(context->ctx);
    JS_SetProperty(context->ctx, console, JS_NewAtom(context->ctx, "log"), JS_NewCFunction(context->ctx, ConsoleLog, "log", 3));
    JS_SetProperty(context->ctx, JS_GetGlobalObject(context->ctx), JS_NewAtom(context->ctx, "console"), console);

    // Set up 'picotorrent' module
    JSModuleDef* mod = JS_NewCModule(context->ctx, "picotorrent", InitPicoTorrentModule);
    JS_AddModuleExportList(context->ctx, mod, PicoTorrentFuncs, countof(PicoTorrentFuncs));

    // Run module
    JSValue result = JS_Eval(context->ctx, data.data(), data.size(), "main.js", JS_EVAL_TYPE_MODULE);

    if (JS_IsException(result))
    {
        JSValue exc = JS_GetException(context->ctx);
        BOOST_LOG_TRIVIAL(error) << "Error when loading JS plugin: " << JS_ToCString(context->ctx, exc);
        JS_FreeValue(context->ctx, exc);
    }
}
