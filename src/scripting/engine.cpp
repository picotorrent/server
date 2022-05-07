#include "engine.hpp"

#include <fstream>
#include <sstream>

#include <boost/log/trivial.hpp>

using pika::Scripting::Engine;

class EngineState
{
public:
    explicit EngineState(boost::asio::io_context &io)
        : m_io(io)
    {
    }

    boost::asio::io_context& Io() { return m_io; }

private:
    boost::asio::io_context &m_io;
};

static duk_ret_t Pika_Log(duk_context* ctx)
{
    switch (duk_get_top(ctx))
    {
    case 1:
        BOOST_LOG_TRIVIAL(info) << duk_to_string(ctx, 0);
        break;
    case 2: {
        std::string level = duk_to_string(ctx, 0);
        std::string message = duk_to_string(ctx, 1);
        if (level == "debug") BOOST_LOG_TRIVIAL(debug) << message;
        if (level == "info") BOOST_LOG_TRIVIAL(info) << message;
        if (level == "warn") BOOST_LOG_TRIVIAL(warning) << message;
        if (level == "error") BOOST_LOG_TRIVIAL(error) << message;
        break;
    }
    }
    return 0;
}

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

    // Duplicate the callback function
    duk_dup(ctx, 1);
    duk_put_prop_index(ctx, -2, duk_get_length(ctx, -2));

    duk_pop_3(ctx);

    return 0;
}

static duk_ret_t Pika_TimerCancel(duk_context* ctx);

struct Timer
{
    explicit Timer(boost::asio::io_context &io, duk_context *ctx, duk_uarridx_t id, int interval)
        : timer(io)
        , m_ctx(ctx)
        , m_interval(interval)
        , id(id)
    {
        timer.expires_after(std::chrono::milliseconds(m_interval));
        timer.async_wait(
            [this](auto && PH1)
            {
                Complete(std::forward<decltype(PH1)>(PH1));
            });
    }

    void Cancel()
    {
        timer.cancel();
    }

private:
    void Complete(const boost::system::error_code &ec)
    {
        if (ec)
        {
            switch (ec.value())
            {
            case boost::asio::error::operation_aborted:
                break;
            default:
                BOOST_LOG_TRIVIAL(error) << "Timer error: " << ec.message();
            }
            return;
        }

        timer.expires_after(std::chrono::milliseconds(m_interval));
        timer.async_wait(
            [this](auto && PH1)
            {
                Complete(std::forward<decltype(PH1)>(PH1));
            });

        duk_push_global_stash(m_ctx);
        duk_get_prop_string(m_ctx, -1, "timers");
        duk_get_prop_index(m_ctx, -1, id);

        duk_push_object(m_ctx);
        duk_push_pointer(m_ctx, this);
        duk_put_prop_string(m_ctx, -2, "\xff" "Timer");
        duk_push_c_function(m_ctx, Pika_TimerCancel, 0);
        duk_put_prop_string(m_ctx, -2, "cancel");

        if (duk_pcall_method(m_ctx, 0) != 0)
        {
            BOOST_LOG_TRIVIAL(error) << "Timer callback error: " << duk_safe_to_string(m_ctx, -1);
        }

        duk_pop_3(m_ctx);
    }

    boost::asio::steady_timer timer;
    duk_context* m_ctx;
    int m_interval;
    duk_uarridx_t id;
};

static duk_ret_t Pika_TimerCancel(duk_context* ctx)
{
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "\xff" "Timer");

    auto* t = static_cast<Timer*>(duk_get_pointer(ctx, -1));
    t->Cancel();

    duk_pop_2(ctx);

    return 0;
}

static duk_ret_t Pika_Timer(duk_context* ctx)
{
    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, "\xff" "engine");
    auto engineState = static_cast<EngineState*>(duk_get_pointer(ctx, -1));
    duk_pop_2(ctx);

    // add callback to array of callbacks in stash
    duk_push_global_stash(ctx);
    duk_get_prop_string(ctx, -1, "timers");
    duk_dup(ctx, 0); // the callback is the first argument
    duk_size_t id = duk_get_length(ctx, -2);
    duk_put_prop_index(ctx, -2, id);
    duk_pop(ctx); // get prop string
    duk_pop(ctx); // push global stash

    auto t = new Timer(
        engineState->Io(),
        ctx,
        id,
        duk_get_int(ctx, 1));

    // create an object that contains the timer and lets us cancel it

    duk_push_object(ctx);
    duk_push_pointer(ctx, t);
    duk_put_prop_string(ctx, -2, "\xff" "Timer");
    duk_push_c_function(ctx, Pika_TimerCancel, 0);
    duk_put_prop_string(ctx, -2, "cancel");

    return 1;
}

Engine::Engine(boost::asio::io_context &io)
    : m_io(io)
    , m_ctx(duk_create_heap_default())
{
}

void Engine::Emit(const std::string &name, Wrapper *wrapper)
{
    duk_push_global_stash(m_ctx);
    duk_get_prop_string(m_ctx, -1, "callbacks");
    duk_get_prop_string(m_ctx, -1, name.c_str()); // event

    // top should now be an array of event handlers for this event

    for (duk_uarridx_t i = 0; i < duk_get_length(m_ctx, -1); i++)
    {
        duk_get_prop_index(m_ctx, -1, i);
        if (duk_pcall(m_ctx, (*wrapper)(m_ctx)) != 0)
        {
            BOOST_LOG_TRIVIAL(error) << "Emit error: " << duk_safe_to_string(m_ctx, -1);
        }
        duk_pop(m_ctx);
    }

    duk_pop_3(m_ctx);
}

void Engine::Run()
{
    // Set up stash
    duk_push_global_stash(m_ctx);
    duk_push_object(m_ctx); // callbacks {}

    // set up callbacks
    duk_push_array(m_ctx);
    duk_put_prop_string(m_ctx, -2, "session.configure");

    // Set callbacks string object on stash
    duk_put_prop_string(m_ctx, -2, "callbacks");

    // Add timer array
    duk_push_array(m_ctx);
    duk_put_prop_string(m_ctx, -2, "timers");

    // Pop stash
    duk_pop(m_ctx);

    // Set IO pointer on global
    duk_push_pointer(m_ctx, new EngineState(m_io));
    duk_put_global_string(m_ctx, "\xff" "engine");

    std::ifstream js("/Users/viktor/code/picotorrent/server/scripts/pika.js", std::ios::binary);
    std::stringstream ss;
    ss << js.rdbuf();
    std::string script = ss.str();

    duk_push_string(m_ctx, script.c_str());

    if (duk_peval(m_ctx) != 0)
    {
        BOOST_LOG_TRIVIAL(warning) << "Error when eval: " << duk_safe_to_string(m_ctx, -1);
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "Script evaluated";

        if (duk_get_global_string(m_ctx, "plugin"))
        {
            BOOST_LOG_TRIVIAL(info) << "Found plugin prop";

            if (duk_is_callable(m_ctx, -1))
            {
                BOOST_LOG_TRIVIAL(info) << "Plugin is callable";

                // Create an object to load our plugin with
                duk_push_object(m_ctx);

                // Log
                duk_push_c_function(m_ctx, Pika_Log, DUK_VARARGS);
                duk_put_prop_string(m_ctx, -2, "log");

                // Add function for subscriptions
                duk_push_c_function(m_ctx, Pika_On, 2);
                duk_put_prop_string(m_ctx, -2, "on");

                // Add function for timers
                duk_push_c_function(m_ctx, Pika_Timer, 2);
                duk_put_prop_string(m_ctx, -2, "timer");

                // call function which is now at index -2
                if (duk_pcall(m_ctx, 1) != DUK_EXEC_SUCCESS)
                {
                    BOOST_LOG_TRIVIAL(warning) << "Failed to load plugin: " << duk_safe_to_string(m_ctx, -1);
                }

                // push stash
                duk_push_global_stash(m_ctx);
                duk_dup(m_ctx, -2);
                duk_put_prop_string(m_ctx, -2, "pika");
                duk_pop(m_ctx);

                BOOST_LOG_TRIVIAL(info) << "State stored in stash";

                duk_pop(m_ctx);
            }

            duk_pop(m_ctx);
        }
    }
}
