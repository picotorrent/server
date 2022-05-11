#pragma once

#include <boost/beast.hpp>

namespace pika::Http
{
    class HttpRequestHandler
    {
    public:
        class Context : public std::enable_shared_from_this<Context>
        {
        public:
            virtual boost::beast::http::request<boost::beast::http::string_body>& Request() = 0;
            virtual boost::beast::tcp_stream& Stream() = 0;

            virtual void Write(boost::beast::http::response<boost::beast::http::file_body> res) = 0;
            virtual void Write(boost::beast::http::response<boost::beast::http::string_body> res) = 0;
        };

        virtual void Execute(std::shared_ptr<Context> context) = 0;
    };
}
