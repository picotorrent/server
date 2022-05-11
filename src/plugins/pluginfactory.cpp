#include "pluginfactory.hpp"

#include <fstream>
#include <sstream>

#include <boost/log/trivial.hpp>
#include <duktape.h>

#include "modules/httpmodule.hpp"

namespace fs = std::filesystem;
using pika::Plugins::IPlugin;
using pika::Plugins::PluginFactory;

struct PluginFactory::PluginMetadata
{
    duk_context* ctx{};
    std::filesystem::path path;
    std::shared_ptr<IPlugin> plugin;
};

PluginFactory::PluginFactory(boost::asio::io_context &io, std::shared_ptr<pika::Http::HttpListener> http)
    : m_io(io)
    , m_http(std::move(http))
{}

PluginFactory::~PluginFactory() = default;

std::shared_ptr<pika::Plugins::IPlugin> PluginFactory::Load(const std::filesystem::path &path)
{
    fs::path actualPath = path.is_absolute()
        ? path
        : fs::absolute(fs::current_path() / path);

    BOOST_LOG_TRIVIAL(info) << "Loading plugin from " << actualPath;

    auto meta = std::make_unique<PluginMetadata>();
    meta->ctx = CreateDefaultContext();
    meta->path = actualPath;

    if (fs::is_regular_file(path)
        && path.extension() == ".js")
    {
        std::ifstream file(path, std::ios::binary);
        std::stringstream buf;
        buf << file.rdbuf();
        std::string contents = buf.str();

        duk_push_string(meta->ctx, contents.c_str());

        if (duk_peval(meta->ctx) != 0)
        {
            BOOST_LOG_TRIVIAL(error) << "Error when evaluating plugin: " << duk_safe_to_string(meta->ctx, -1);
        }
        else
        {
            if (duk_get_global_string(meta->ctx, "plugin"))
            {
                if (duk_is_callable(meta->ctx, -1))
                {
                    // Put ourself in the global stash
                    duk_push_global_stash(meta->ctx);
                    duk_push_pointer(meta->ctx, this);
                    duk_put_prop_string(meta->ctx, -2, "\xff" "PluginFactory");

                    duk_push_object(meta->ctx); // callbacks {}
                    // set up callbacks
                    duk_push_array(meta->ctx);
                    duk_put_prop_string(meta->ctx, -2, "session.stats");
                    // Set callbacks string object on stash
                    duk_put_prop_string(meta->ctx, -2, "callbacks");

                    duk_pop(meta->ctx); // pop global stash

                    // Create the 'pika' object
                    duk_push_object(meta->ctx);

                    // Set 'on' function
                    duk_push_c_function(meta->ctx, Func_Pika_On, 2);
                    duk_put_prop_string(meta->ctx, -2, "on");

                    // Set the 'use' function
                    duk_push_c_function(meta->ctx, Func_Use, 1);

                    if (duk_pcall(meta->ctx, 2) != DUK_EXEC_SUCCESS)
                    {
                        BOOST_LOG_TRIVIAL(error)
                            << "Error when calling plugin function: "
                            << duk_safe_to_string(meta->ctx, -1);
                    }
                }
                else
                {
                    BOOST_LOG_TRIVIAL(warning) << "'plugin' global is not callable";
                }

                duk_pop(meta->ctx);
            }
            BOOST_LOG_TRIVIAL(info) << "Plugin loaded";
        }
    }
    else if (fs::is_regular_file(path)
        && path.extension() == ".zip")
    {
        // Load plugin from zip file
    }
    else if (fs::is_directory(path))
    {
        // Load plugin from directory
    }

    m_plugins.push_back(std::move(meta));

    return nullptr;
}

void PluginFactory::OnSessionStats(const std::map<std::string, int64_t> &metrics)
{
    for (const auto& meta : m_plugins)
    {
        duk_push_global_stash(meta->ctx);
        duk_get_prop_string(meta->ctx, -1, "callbacks");
        duk_get_prop_string(meta->ctx, -1, "session.stats"); // event

        // top should now be an array of event handlers for this event

        for (duk_uarridx_t i = 0; i < duk_get_length(meta->ctx, -1); i++)
        {
            duk_get_prop_index(meta->ctx, -1, i);

            duk_push_object(meta->ctx);

            for (const auto& [key,value] : metrics)
            {
                duk_push_int(meta->ctx, value);
                duk_put_prop_string(meta->ctx, -2, key.c_str());
            }

            if (duk_pcall(meta->ctx, 1) != 0)
            {
                BOOST_LOG_TRIVIAL(error) << "Emit error: " << duk_safe_to_string(meta->ctx, -1);
            }
            duk_pop(meta->ctx);
        }

        duk_pop_3(meta->ctx);
    }
}

duk_context* PluginFactory::CreateDefaultContext()
{
    duk_context* ctx = duk_create_heap_default();
    return ctx;
}

duk_ret_t PluginFactory::Func_Pika_On(duk_context *ctx)
{
    BOOST_LOG_TRIVIAL(info) << "Registering handler for event " << duk_get_string(ctx, 0);

    // Push the stash and store the function from the 2nd param
    // the stash looks something like this
    // callbacks: {
    //   event: [ functions ]
    // }

    duk_push_global_stash(ctx);
    duk_get_prop_string(ctx, -1, "callbacks");
    duk_get_prop_string(ctx, -1, duk_get_string(ctx, 0)); // event

    // Duplicate the callback function
    duk_dup(ctx, 1);
    duk_put_prop_index(ctx, -2, duk_get_length(ctx, -2));

    duk_pop_3(ctx);

    return 0;
}

duk_ret_t PluginFactory::Func_Use(duk_context *ctx)
{
    std::string module = duk_require_string(ctx, 0);

    BOOST_LOG_TRIVIAL(info) << "Loading module " << module;

    // Get PluginFactory instance
    duk_push_global_stash(ctx);
    duk_get_prop_string(ctx, -1, "\xff" "PluginFactory");
    auto* pf = static_cast<PluginFactory*>(duk_get_pointer(ctx, -1));
    duk_pop_2(ctx);

    if (module == "http/server")
    {
        Modules::HttpModule::Push(ctx, pf->m_http);
        return 1;
    }

    return 0;
}
