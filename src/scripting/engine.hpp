#pragma once

#include <boost/asio.hpp>

#include "duktape.h"

namespace pika::Scripting
{
    class Engine
    {
    public:
        explicit Engine(boost::asio::io_context &io);

        void Run();

    private:
        boost::asio::io_context &m_io;
        duk_context *m_ctx;
    };
}
