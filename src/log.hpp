#pragma once

#include <boost/log/trivial.hpp>

namespace pt::Server
{
    class Log
    {
    public:
        static void Setup(boost::log::trivial::severity_level level);
    };
}
