#pragma once

namespace libpika::data
{
    class ITransaction
    {
    public:
        virtual ~ITransaction() = default;

        virtual void Commit() = 0;
        virtual void Rollback() = 0;
    };
}
