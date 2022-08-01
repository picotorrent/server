#pragma once

namespace libpika::data
{
    class IRow;

    class IStatement
    {
    public:
        virtual ~IStatement() = default;

        virtual void Bind(int pos, int value);
        virtual void Bind(int pos, const std::optional<std::string_view>& value) = 0;
        virtual void Bind(int pos, const std::vector<char>& value) = 0;

        virtual void Reset() = 0;
        virtual std::unique_ptr<IRow> Step() = 0;
    };
}
