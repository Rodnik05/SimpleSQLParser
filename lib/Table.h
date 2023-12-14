#pragma once

#include "SQL_Query.h"
#include "Column.h"

class Table {
protected:
    std::map<std::string, std::unique_ptr<Column>> columns_;
    Table() = default;
    Table(Table& table, const SQL_Query& sql);

private:
    bool CheckConditions(const std::vector<SQL_Query::Condition>& conditions, size_t ind_row);

    bool CheckOneCondition(const SQL_Query::Condition& condition, size_t ind_row);

    void CreateForeignKey(const std::string& column,
                          const std::string& foreign,
                          const std::map<std::string,
                          std::unique_ptr<Table>>& other_tables);

    void AddColumn(const std::string& name, const Column::ValueType& type);

    Column::ValueType GetType(const std::string& type);

public:
    explicit Table(const SQL_Query& request, const std::map<std::string, std::unique_ptr<Table>>& tables);

    std::size_t Count() const;

    void AddRow(const SQL_Query& sql);

    void UpdateRowsValues(const SQL_Query& request);

    void DeleteRow(const SQL_Query& request);

    std::vector<std::string> GetColumnNames() const;


    friend class ResultTable;
    friend class MyCoolDB;
};
