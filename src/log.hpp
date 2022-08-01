#pragma once

#include <boost/log/trivial.hpp>

namespace pika
{
    class Log
    {
    public:
        static void Setup(boost::log::trivial::severity_level level);
    };
}
