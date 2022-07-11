#pragma once

#include <utility>

#include <libpika/http/middleware.hpp>

namespace libpika::http
{
    class HttpMethod
    {
    public:
        explicit HttpMethod(boost::beast::http::verb verb, std::string route, Middleware middleware)
            : m_verb(verb)
            , m_route(std::move(route))
            , m_middleware(std::move(middleware))
        {
        }

        void operator()(const std::shared_ptr<libpika::http::Context> &ctx)
        {
            if (ctx->Request().target() == m_route
                && ctx->Request().method() == m_verb)
            {
                m_middleware(ctx);
            }
            else
            {
                ctx->Next();
            }
        }

    private:
        boost::beast::http::verb m_verb;
        std::string m_route;
        Middleware m_middleware;
    };

    class HttpGet : public HttpMethod
    {
    public:
        explicit HttpGet(std::string route, Middleware middleware)
            : HttpMethod(
                boost::beast::http::verb::get,
                std::move(route),
                std::move(middleware))
        {
        }
    };

    class HttpPost : public HttpMethod
    {
    public:
        explicit HttpPost(std::string route, Middleware middleware)
            : HttpMethod(
            boost::beast::http::verb::post,
            std::move(route),
            std::move(middleware))
        {
        }
    };
}
