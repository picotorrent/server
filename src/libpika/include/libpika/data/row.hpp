#pragma once

#include <string>
#include <vector>

namespace libpika::data
{
    class IRow
    {
    public:
        virtual ~IRow() = default;

        virtual std::vector<char> GetBuffer(int index) = 0;
        virtual int GetInt32(int index) = 0;
        virtual std::string GetStdString(int index) = 0;
    };
}