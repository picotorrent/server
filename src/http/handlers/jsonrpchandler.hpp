#pragma once

#include <libpika/http/context.hpp>

namespace libpika::jsonrpc
{
    class JsonRpcServer;
}

namespace pika::Http::Handlers
{
    class JsonRpcHandler
    {
    public:
        explicit JsonRpcHandler(libpika::jsonrpc::JsonRpcServer& rpcServer);

        void operator()(const std::shared_ptr<libpika::http::Context>& context);

    private:
        libpika::jsonrpc::JsonRpcServer& m_rpc;
    };
}
