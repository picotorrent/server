#include "log.hpp"

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

using pt::Server::Log;

namespace expr = boost::log::expressions;

void Log::Setup(boost::log::trivial::severity_level level)
{
    auto psink = boost::log::add_console_log(
        std::cout,
        boost::log::keywords::format = expr::stream
            << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") << " "
            << "[" << expr::attr<boost::log::attributes::timer::value_type>("Uptime") << "] "
            << "[" << expr::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID") << "] "
            << std::setw(7) << boost::log::trivial::severity << ": "
            << expr::message);

    psink->locked_backend()->auto_flush(true);

    boost::log::add_common_attributes();
    boost::log::core::get()->add_global_attribute("Uptime", boost::log::attributes::timer());
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= level);
}
