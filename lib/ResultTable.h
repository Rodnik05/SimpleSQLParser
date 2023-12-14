#pragma once

#include "Table.h"

class ResultTable : public Table {
public:
    ResultTable() = default;
    explicit ResultTable(Table& table, const SQL_Query& sql)
            : Table(table, sql)
    {}
    static ResultTable JoinTables(Table& table1, Table& table2, SQL_Query& request);
    bool Next();
    void Reset();
    Value Get(const std::string& columnName);

private:
    std::size_t current_ = 0;
};

