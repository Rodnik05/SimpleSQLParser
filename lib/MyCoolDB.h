#pragma once

#include "Table.h"
#include "SQL_Query.h"
#include "ResultTable.h"

#include <memory>
#include <filesystem>
#include <fstream>

class MyCoolDB {
public:

    ResultTable Query(const std::string& request);
    std::vector<std::string> GetTableNames() const;

private:

    ResultTable JoinTables(SQL_Query& sql);
    std::map<std::string, std::unique_ptr<Table>> tables_;

    friend class Table;
};

