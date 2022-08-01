#pragma once

#include <functional>
#include <memory>

#include <libpika/http/context.hpp>

namespace libpika::http
{
    typedef std::function<void(std::shared_ptr<Context> ctx)> Middleware;
}
