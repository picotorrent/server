#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <duktape.h>

#include "../sessioneventhandler.hpp"

namespace pika::Http
{
    class HttpListener;
}

namespace pika::Plugins
{
    class IPlugin;

    class PluginFactory : public ISessionEventHandler
    {
    public:
        explicit PluginFactory(boost::asio::io_context& io, std::shared_ptr<pika::Http::HttpListener> http);
        ~PluginFactory();

        std::shared_ptr<IPlugin> Load(const std::filesystem::path &path);

        // Session events
        void OnSessionStats(const std::map<std::string, int64_t>& metrics) override;

    private:
        static duk_context* CreateDefaultContext();
        static duk_ret_t Func_Pika_On(duk_context* ctx);
        static duk_ret_t Func_Use(duk_context* ctx);

        struct PluginMetadata;

        boost::asio::io_context& m_io;
        std::shared_ptr<pika::Http::HttpListener> m_http;
        std::vector<std::unique_ptr<PluginMetadata>> m_plugins;
    };
}
