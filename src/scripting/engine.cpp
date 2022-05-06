#include "engine.hpp"

#include <fstream>
#include <sstream>

#include <boost/log/trivial.hpp>

using pika::Scripting::Engine;

static duk_ret_t Pika_On(duk_context* ctx)
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

    BOOST_LOG_TRIVIAL(info) << "Event handlers: " << duk_get_length(ctx, -1);

    duk_pop(ctx);
    duk_pop(ctx);
    duk_pop(ctx);

    return 0;
}

void Engine::Run()
{
    duk_context *ctx = duk_create_heap_default();

    // Set up stash
    duk_push_global_stash(ctx);
    duk_push_object(ctx); // callbacks {}

    // set up callbacks
    duk_push_array(ctx);
    duk_put_prop_string(ctx, -2, "session.configure");

    duk_put_prop_string(ctx, -2, "callbacks");
    duk_pop(ctx);

    std::ifstream js("/home/viktor/dev/picotorrent/server/scripts/pika.js", std::ios::binary);
    std::stringstream ss;
    ss << js.rdbuf();
    std::string script = ss.str();

    duk_push_string(ctx, script.c_str());

    if (duk_peval(ctx) != 0)
    {
        BOOST_LOG_TRIVIAL(warning) << "Error when eval: " << duk_safe_to_string(ctx, -1);
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "Script evaluated";

        if (duk_get_global_string(ctx, "plugin"))
        {
            BOOST_LOG_TRIVIAL(info) << "Found plugin prop";

            if (duk_is_callable(ctx, -1))
            {
                BOOST_LOG_TRIVIAL(info) << "Plugin is callable";

                // Create an object to load our plugin with
                duk_push_object(ctx);

                // Put a C function on the pika object
                duk_push_c_function(ctx, Pika_On, 2);
                duk_put_prop_string(ctx, -2, "on");

                // call function which is now at index -2
                if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS)
                {
                    BOOST_LOG_TRIVIAL(warning) << "Failed to load plugin: " << duk_safe_to_string(ctx, -1);
                }

                // push stash
                duk_push_global_stash(ctx);
                duk_dup(ctx, -2);
                duk_put_prop_string(ctx, -2, "pika");
                duk_pop(ctx);

                BOOST_LOG_TRIVIAL(info) << "State stored in stash";

                duk_pop(ctx);
            }

            duk_pop(ctx);
        }
    }

    duk_destroy_heap(ctx);
}
