#pragma once

#include <memory>
#include <string>

namespace libpika::data
{
    class IStatement;
    class ITransaction;

    class Database
    {
    public:
        explicit Database(const std::string_view& file);
        ~Database() = default;

        std::unique_ptr<ITransaction> BeginTransaction();
        std::unique_ptr<IStatement> PrepareStatement(const std::string_view& sql);

    private:
        struct State;
        std::unique_ptr<State> m_state;
    };
}
