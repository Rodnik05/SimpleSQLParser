#include "ResultTable.h"


bool ResultTable::Next() {
    return ++current_ < Count();
}
void ResultTable::Reset() {
    current_ = 0;
}


ResultTable ResultTable::JoinTables(Table& table1, Table& table2, SQL_Query& request) {
    std::unordered_map<std::string, std::string> rows = request.GetData();
    std::string left_table_name = request.GetTableName() + '.';
    std::string right_table_name = request.GetJoinDescription().table_to_join + '.';
    Table joined_table;
    for (auto& [name, column] : table1.columns_) {
        std::string table_column_name = left_table_name + name;
        if (rows.contains(table_column_name) || rows.contains("*")) {
            joined_table.AddColumn(table_column_name, column->GetType());
        }
    }
    for (auto& [name, column] : table2.columns_) {
        std::string table_column_name = right_table_name + name;
        if (rows.contains(table_column_name) || rows.contains("*")) {
            joined_table.AddColumn(table_column_name, column->GetType());
        }
    }
    Table* left_table = &table1;
    Table* right_table = &table2;
    if (request.GetJoinDescription().join_type== "RIGHT") {
        std::swap(left_table, right_table);
        std::swap(left_table_name, right_table_name);
    }
    bool Inner = (request.GetJoinDescription().join_type == "INNER");
    std::vector<std::pair<Column*, Column*>> ON_conditions;
    ON_conditions.reserve(request.GetConditions().size());
    for (auto& condition : request.GetConditions()) {
        if (condition.GetCondition() == SQL_Query::Condition::Type::WHERE) {
            break;
        }

        std::string left_column = split_by_character(condition.GetLhs(), '.')[1];
        std::string right_column = split_by_character(condition.GetRhs(), '.')[1];

        if (left_table->columns_.contains(left_column) && right_table->columns_.contains(right_column)) {
            ON_conditions.emplace_back(left_table->columns_[left_column].get(),
                                       right_table->columns_[right_column].get());
        } else if (right_table->columns_.contains(left_column) && left_table->columns_.contains(right_column)) {
            ON_conditions.emplace_back(left_table->columns_[right_column].get(),
                                       right_table->columns_[left_column].get());
        } else {
            throw std::invalid_argument("Invalid join request");
        }
    }
    SQL_Query::Condition equals_obj{};
    equals_obj.SetOperation("=");
    for (std::size_t i = 0; i < left_table->Count(); ++i) {
        bool joined_flag = false;
        for (auto& [column1, column2] : ON_conditions) {
            for (std::size_t j = 0; j < right_table->Count(); ++j) {
                bool equals = true;
                for (auto& [lhs, rhs] : ON_conditions) {
                    if (!lhs->Compare(*rhs, equals_obj, i, j)) {
                        equals = false;
                        break;
                    }
                }
                if (equals) {
                    joined_flag = true;
                    for (auto& [name, column]: left_table->columns_) {
                        if (joined_table.columns_.contains(left_table_name + name)) {
                            joined_table.columns_[left_table_name + name]->CopyValueFrom(column.get(), i);
                        }
                    }
                    for (auto& [name, column]: right_table->columns_) {
                        if (joined_table.columns_.contains(right_table_name + name)) {
                            joined_table.columns_[right_table_name + name]->CopyValueFrom(column.get(), j);
                        }
                    }
                }
            }
        }
        if (!joined_flag && !Inner) {
            for (auto& [name, column]: left_table->columns_) {
                if (joined_table.columns_.contains(left_table_name + name)) {
                    joined_table.columns_[left_table_name + name]->CopyValueFrom(column.get(), i);
                }
            }
            for (auto& [name, _]: right_table->columns_) {
                joined_table.columns_[right_table_name + name]->AddValue("NULL");
            }
        }
    }
    return ResultTable(joined_table, request);
}

Value ResultTable::Get(const std::string& columnName) {
    if (current_ == SIZE_MAX) {
        ++current_;
    }
    if (current_ >= Count()) {
        return {};
    }
    for (const auto& [name, column] : columns_) {
        if (name == columnName) {
            return column.get()->GetValueFromIndex(current_);
        }
    }
    return {};
}
