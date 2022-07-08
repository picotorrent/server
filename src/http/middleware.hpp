#pragma once

#include <functional>
#include <memory>

#include "context.hpp"

namespace pika::Http
{
    typedef std::function<void(std::shared_ptr<Context> ctx)> Middleware;
}
