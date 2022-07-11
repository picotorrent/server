#pragma once

#include <memory>

#include <boost/asio/io_context.hpp>

#include "middleware.hpp"

namespace libpika::http
{
    class HttpServer
    {
    public:
        struct Options
        {
            std::string host;
            uint16_t port;
        };

        explicit HttpServer(boost::asio::io_context& io, const Options& options);
        ~HttpServer();

        void Use(const Middleware& middleware);

    private:
        class State;
        std::shared_ptr<State> m_state;
    };
}
