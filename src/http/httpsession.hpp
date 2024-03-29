#pragma once

#include <map>
#include <memory>
#include <vector>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <sqlite3.h>

namespace pt::Server { class SessionManager; }
namespace pt::Server::RPC { class Command; }

namespace pt::Server::Http
{
    class HttpRequestHandler;

    class HttpSession : public std::enable_shared_from_this<HttpSession>
    {
        class DefaultContext;

        class Queue
        {
            enum
            {
                // Maximum number of responses we will queue
                Limit = 8
            };

            // The type-erased, saved work item
            struct Work
            {
                virtual ~Work() = default;
                virtual void operator()() = 0;
            };

            HttpSession& m_self;
            std::vector<std::unique_ptr<Work>> m_items;

        public:
            explicit Queue(HttpSession& self)
                : m_self(self)
            {
                static_assert(Limit > 0, "queue limit must be positive");
                m_items.reserve(Limit);
            }

            // Returns `true` if we have reached the queue limit
            bool IsFull() const
            {
                return m_items.size() >= Limit;
            }

            // Called when a message finishes sending
            // Returns `true` if the caller should initiate a read
            bool OnWrite()
            {
                BOOST_ASSERT(!m_items.empty());
                auto const wasFull = IsFull();
                m_items.erase(m_items.begin());
                if(! m_items.empty())
                    (*m_items.front())();
                return wasFull;
            }

            template<bool isRequest, class Body, class Fields>
            void operator()(boost::beast::http::message<isRequest, Body, Fields>&& msg)
            {
                // This holds a work item
                struct WorkImpl : Work
                {
                    HttpSession& m_self;
                    boost::beast::http::message<isRequest, Body, Fields> m_msg;

                    WorkImpl(
                        HttpSession& self,
                        boost::beast::http::message<isRequest, Body, Fields>&& msg)
                        : m_self(self)
                        , m_msg(std::move(msg))
                    {
                    }

                    void operator()()
                    {
                        boost::beast::http::async_write(
                            m_self.m_stream,
                            m_msg,
                            boost::beast::bind_front_handler(
                                &HttpSession::EndWrite,
                                m_self.shared_from_this(),
                                m_msg.need_eof()));
                    }
                };

                // Allocate and store the work
                m_items.push_back(std::make_unique<WorkImpl>(m_self, std::move(msg)));

                // If there was no previous work, start this one
                if(m_items.size() == 1)
                    (*m_items.front())();
            }
        };

    public:
        HttpSession(
            boost::asio::ip::tcp::socket&& socket,
            std::shared_ptr<std::map<std::tuple<std::string, std::string>, std::shared_ptr<HttpRequestHandler>>>  handlers,
            std::shared_ptr<std::string const>  docroot);

        void Run();

    private:
        void BeginRead();
        void EndRead(boost::beast::error_code ec, std::size_t bytes_transferred);
        void EndWrite(bool close, boost::beast::error_code ec, std::size_t bytes_transferred);
        void BeginClose();

        boost::beast::tcp_stream m_stream;
        boost::beast::flat_buffer m_buffer;
        std::shared_ptr<std::string const> m_docroot;
        std::shared_ptr<std::map<std::tuple<std::string, std::string>, std::shared_ptr<HttpRequestHandler>>> m_handlers;

        Queue m_queue;

        // The parser is stored in an optional container so we can
        // construct it from scratch it at the beginning of each new message.
        boost::optional<boost::beast::http::request_parser<boost::beast::http::string_body>> m_parser;
    };
}
