#pragma once

#include <boost/asio.hpp>
#include <duktape.h>

#include "scriptengine.hpp"

namespace pika::Scripting
{
    class Engine : public IScriptEngine
    {
    public:
        explicit Engine(boost::asio::io_context &io);

        // overridables
        void Emit(const std::string &name, Wrapper *wrapper) override;

        void Run();

    private:
        boost::asio::io_context &m_io;
        duk_context *m_ctx;
    };
}
