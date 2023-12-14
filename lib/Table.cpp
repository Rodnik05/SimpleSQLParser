#include "Table.h"
#include "SQL_Query.h"

Table::Table(const SQL_Query& request, const std::map<std::string, std::unique_ptr<Table>>& tables) {
    if (request.type_ != SQL_Query::RequestType::CREATE) {
        return;
    }

    for (auto& [column, tmp] : request.GetData()) {
        std::vector<std::string> param = split_by_character(tmp,' ');
        AddColumn(column, GetType(param[0]));
        if (std::find(param.begin(), param.end(), "PRIMARY") != param.end()) {
            columns_[column]->SetPrimaryKeyFlag(true);
        }
    }
    for (auto& [column, tmp] : request.GetData()) {
        if (column[0] == 'F' && column[1] == ' ') {
            CreateForeignKey(column.substr(2), tmp, tables);
            continue;
        }
    }
}

size_t Table::Count() const {
    return columns_.begin()->second->Size();
}


void Table::AddRow(const SQL_Query& sql) {
    if (sql.type_ != SQL_Query::RequestType::INSERT) {
        return;
    }
    auto row = sql.GetData();
    for (const auto& [tmp, column] : columns_) {
        if (row.contains(column->GetName())) {
            column->AddValue(row[column->GetName()]);
        } else {
            column->AddDefault();
        }
    }
}

void Table::UpdateRowsValues(const SQL_Query& request) {
    if (request.type_ != SQL_Query::RequestType::UPDATE) {
        return;
    }
    std::vector<SQL_Query::Condition> conditions = request.GetConditions();
    for (std::size_t i = 0; i < Count(); ++i) {
        if (CheckConditions(conditions, i)) {
            for (auto& [column, value]: request.columns_values) {
                if (!columns_.contains(column)) {
                    throw std::invalid_argument("Invalid update request");
                }
                columns_[column]->SetValue(value, i);
            }
        }
    }
}

void Table::DeleteRow(const SQL_Query& request) {
    if (request.type_ != SQL_Query::RequestType::DELETE) {
        return;
    }
    std::vector<SQL_Query::Condition> conditions = request.GetConditions();
    for (long long int i = Count() - 1; i >= 0; --i) {
        if (CheckConditions(conditions, i)) {
            for (auto& [name, column]: columns_) {
                column->DeleteValue(i);
            }
        }
    }
}


Table::Table(Table& table, const SQL_Query& request) {
    if (request.GetType() != SQL_Query::RequestType::SELECT && request.GetType() != SQL_Query::RequestType::JOIN) {
        return;
    }
    std::unordered_map<std::string, std::string> row = request.GetData();
    for (const auto& [name, column] : table.columns_) {
        if (row.contains(column->GetName()) || row.contains("*")) {
            AddColumn(column->GetName(), column->GetType());
        }
    }
    std::vector<SQL_Query::Condition> conditions = request.GetConditions();
    for (std::size_t i = 0; i < table.Count(); ++i) {
        if (table.CheckConditions(conditions, i)) {
            for (auto& [name, column]: columns_) {
                column->CopyValueFrom(table.columns_[name].get(), i);
            }
        }
    }
}

bool Table::CheckConditions(const std::vector<SQL_Query::Condition>& conditions, std::size_t ind_row) {
    if (conditions.empty()) {
        return true;
    }
    std::size_t index = 0;
    while (index < conditions.size() && conditions[index].GetCondition() != SQL_Query::Condition::Type::WHERE) {
        index++;
    }
    if (index >= conditions.size()) {
        return true;
    }
    bool result = CheckOneCondition(conditions[index], ind_row);
    index++;
    for (; index < conditions.size(); ++index) {
        bool tmp = CheckOneCondition(conditions[index], ind_row);
        if(conditions[index].GetCondition() == SQL_Query::Condition::AND){
            result = result && tmp;
        } else if (conditions[index].GetCondition() == SQL_Query::Condition::OR){
            result = result || tmp;
        } else if (conditions[index].GetCondition() == SQL_Query::Condition::ON){
            throw std::invalid_argument("Invalid condition");
        }
    }
    return result;
}

bool Table::CheckOneCondition(const SQL_Query::Condition& condition, size_t ind_row) {
    if (columns_.contains(condition.GetLhs()) && columns_.contains(condition.GetRhs())) {
        return columns_[condition.GetRhs()]->Compare(*columns_[condition.GetLhs()], condition, ind_row);
    }
    if (columns_.contains(condition.GetLhs())) {
        return columns_[condition.GetLhs()]->Compare(condition.GetRhs(), condition, ind_row);
    }
    if (columns_.contains(condition.GetRhs())) {
        return columns_[condition.GetRhs()]->Compare(condition.GetLhs(), condition, ind_row);
    }
    return true;
}

void Table::CreateForeignKey(const std::string& column,
                             const std::string& foreign,
                             const std::map<std::string,
                             std::unique_ptr<Table>>& other_tables) {
    std::vector<std::string> parameters = split_by_character(foreign, ' ');
    columns_[column].get()->SetForeignKey(parameters[0],other_tables.at(parameters[0])->columns_.at(parameters[1]).get());
}

void Table::AddColumn(const std::string& name, const Column::ValueType& type) {
    columns_[name] = std::make_unique<Column>(name, type);
}

std::vector<std::string> Table::GetColumnNames() const {
    std::vector<std::string> names;
    for (auto& [tmp_1, tmp_2] : columns_) {
        names.push_back(tmp_1);
    }
    return names;
}

Column::ValueType Table::GetType(const std::string& type) {
    if (type == "INT") {
        return Column::ValueType::INT;
    } else if (type == "VARCHAR") {
        return  Column::ValueType::VARCHAR;
    } else if (type == "BOOL") {
        return Column::ValueType::BOOL;
    } else if (type == "DOUBLE") {
        return Column::ValueType::DOUBLE;
    }
    return Column::ValueType::FLOAT;
}


