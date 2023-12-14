#include "MyCoolDB.h"

std::vector<std::string> MyCoolDB::GetTableNames() const {
    std::vector<std::string> table_names;
    table_names.reserve(tables_.size());
    for (auto& [table_name, _] : tables_) {
        table_names.push_back(table_name);
    }

    return table_names;
}


ResultTable MyCoolDB::Query(const std::string& request) {
    SQL_Query sql_request(request);

    if (tables_.contains(sql_request.GetTableName()) && sql_request.GetType() == SQL_Query::RequestType::CREATE) {
        throw std::invalid_argument("Table already exists");
    }

    if (sql_request.GetType() != SQL_Query::RequestType::CREATE && !tables_.contains(sql_request.GetTableName())) {
        throw std::invalid_argument("No such table");
    }

    switch (sql_request.GetType()) {
        case SQL_Query::RequestType::CREATE:
            tables_[sql_request.GetTableName()] = std::make_unique<Table>(sql_request, tables_);
            break;

        case SQL_Query::RequestType::DROP:
            tables_[sql_request.GetTableName()].reset();
            tables_.erase(sql_request.GetTableName());
            break;

        case SQL_Query::RequestType::UPDATE:
            tables_[sql_request.GetTableName()]->UpdateRowsValues(sql_request);
            break;

        case SQL_Query::RequestType::INSERT:
            tables_[sql_request.GetTableName()]->AddRow(sql_request);
            break;

        case SQL_Query::RequestType::DELETE:
            tables_[sql_request.GetTableName()]->DeleteRow(sql_request);
            break;

        case SQL_Query::RequestType::SELECT:
            return ResultTable(*tables_[sql_request.GetTableName()], sql_request);

        case SQL_Query::RequestType::JOIN:
            return JoinTables(sql_request);

        default:
            return {};
    }
    return {};
}



ResultTable MyCoolDB::JoinTables(SQL_Query& sql) {
    if (sql.GetType() != SQL_Query::RequestType::JOIN) {
        return {};
    }

    ResultTable first_table = Query("SELECT * FROM " + sql.GetTableName());
    ResultTable second_table = Query("SELECT * FROM " + sql.GetJoinDescription().table_to_join);

    return ResultTable::JoinTables(first_table, second_table, sql);
}


