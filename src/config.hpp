#pragma once

#include <toml++/toml.h>

namespace pika
{
    class Config
    {
    public:
        static toml::table Load(int argc, char** argv);
    };
}
