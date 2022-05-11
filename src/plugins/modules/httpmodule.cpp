#include "httpmodule.hpp"

#include <boost/beast.hpp>
#include <boost/log/trivial.hpp>

#include "../../http/httplistener.hpp"
#include "../../http/httprequesthandler.hpp"

using pika::Plugins::Modules::HttpModule;

class HttpResponseState
{
public:
    explicit HttpResponseState(std::shared_ptr<pika::Http::HttpRequestHandler::Context> ctx)
        : m_context(std::move(ctx))
    {}

    static duk_ret_t Func_Send(duk_context* ctx)
    {
        std::string body = duk_get_string(ctx, 0);

        duk_push_this(ctx);
        duk_get_prop_string(ctx, -1, "\xff" "res");
        auto* state = static_cast<HttpResponseState*>(duk_get_pointer(ctx, -1));
        duk_pop_2(ctx);

        state->m_response.body() = body;
        state->m_response.prepare_payload();
        state->m_context->Write(state->m_response);

        return 0;
    }

    static duk_ret_t Func_SendText(duk_context* ctx)
    {
        std::string body = duk_get_string(ctx, 0);

        duk_push_this(ctx);
        duk_get_prop_string(ctx, -1, "\xff" "res");
        auto* state = static_cast<HttpResponseState*>(duk_get_pointer(ctx, -1));
        duk_pop_2(ctx);

        state->m_response.set(boost::beast::http::field::content_type, "text/plain");
        state->m_response.body() = body;
        state->m_response.prepare_payload();
        state->m_context->Write(state->m_response);

        return 0;
    }

    static duk_ret_t Func_ContentType(duk_context* ctx)
    {
        std::string contentType = duk_get_string(ctx, 0);

        duk_push_this(ctx);
        duk_get_prop_string(ctx, -1, "\xff" "res");
        auto* state = static_cast<HttpResponseState*>(duk_get_pointer(ctx, -1));
        duk_pop(ctx);

        state->m_response.set(boost::beast::http::field::content_type, contentType);

        return 1;
    }

    static duk_ret_t Func_Status(duk_context* ctx)
    {
        int status = duk_get_int(ctx, 0);

        duk_push_this(ctx);
        duk_get_prop_string(ctx, -1, "\xff" "res");
        auto* state = static_cast<HttpResponseState*>(duk_get_pointer(ctx, -1));
        duk_pop(ctx);

        state->m_response.result(status);

        return 1;
    }

private:
    std::shared_ptr<pika::Http::HttpRequestHandler::Context> m_context;
    boost::beast::http::response<boost::beast::http::string_body> m_response;
};

class ScriptedHttpRequestHandler : public pika::Http::HttpRequestHandler
{
public:
    explicit ScriptedHttpRequestHandler(duk_context* ctx, duk_idx_t idx)
        : m_ctx(ctx)
        , m_idx(idx)
    {
    }

    void Execute(std::shared_ptr<HttpRequestHandler::Context> context) override
    {
        // Call function in http-callbacks array from stash
        duk_push_global_stash(m_ctx);
        duk_get_prop_string(m_ctx, -1, "http-callbacks");
        duk_get_prop_index(m_ctx, -1, m_idx);

        // Set up callback
        // req
        duk_push_object(m_ctx);

        // res
        duk_push_object(m_ctx);
        duk_push_pointer(m_ctx, new HttpResponseState(context));
        duk_put_prop_string(m_ctx, -2, "\xff" "res");
        duk_push_c_function(m_ctx, HttpResponseState::Func_ContentType, 1);
        duk_put_prop_string(m_ctx, -2, "contentType");
        duk_push_c_function(m_ctx, HttpResponseState::Func_Send, 1);
        duk_put_prop_string(m_ctx, -2, "send");
        duk_push_c_function(m_ctx, HttpResponseState::Func_SendText, 1);
        duk_put_prop_string(m_ctx, -2, "sendText");
        duk_push_c_function(m_ctx, HttpResponseState::Func_Status, 1);
        duk_put_prop_string(m_ctx, -2, "status");

        if (duk_pcall(m_ctx, 2) != 0)
        {
            BOOST_LOG_TRIVIAL(error) << "HTTP callback error: " << duk_safe_to_string(m_ctx, -1);
        }

        duk_pop_3(m_ctx);
    }

private:
    duk_context* m_ctx;
    duk_idx_t m_idx;
};

class HttpModuleState
{
public:
    explicit HttpModuleState(std::shared_ptr<pika::Http::HttpListener> http)
        : http(std::move(http))
    {}

    std::shared_ptr<pika::Http::HttpListener> http{};
};

void HttpModule::Push(duk_context *ctx, const std::shared_ptr<pika::Http::HttpListener>& http)
{
    duk_push_global_stash(ctx);
    duk_push_array(ctx);
    duk_put_prop_string(ctx, -2, "http-callbacks");
    duk_pop(ctx);

    duk_push_object(ctx);
    duk_push_c_function(ctx, Func_Get, 2);
    duk_put_prop_string(ctx, -2, "get");

    duk_push_pointer(ctx, new HttpModuleState(http));
    duk_put_prop_string(ctx, -2, "\xff" "http");
}

duk_ret_t HttpModule::Func_Get(duk_context *ctx)
{
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "\xff" "http");
    auto* state = static_cast<HttpModuleState*>(duk_get_pointer(ctx, -1));
    duk_pop_2(ctx);

    std::string path = duk_get_string(ctx, 0);

    // Put callback function in global stash array of http-callbacks
    duk_push_global_stash(ctx);
    duk_get_prop_string(ctx, -1, "http-callbacks");
    auto idx = duk_get_length(ctx, -1);
    duk_dup(ctx, 1);
    duk_put_prop_index(ctx, -2, idx);
    duk_pop(ctx);
    duk_pop(ctx);

    state->http->AddHandler("GET", path, std::make_shared<ScriptedHttpRequestHandler>(ctx, idx));

    return 0;
}
