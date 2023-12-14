#include "Column.h"

const std::string& Column::GetName() const {
    return name_column;
}

Column::ValueType Column::GetType() const {
    return type_;
}

void Column::SetPrimaryKeyFlag(bool value) {
    is_primary_key = value;
}


std::string Column::GetValueAsStr(std::size_t ind) const {
    if (values[ind].IsNull()) {
        return "NULL";
    }
    return values[ind].GetValueAsString();
}

std::size_t Column::Size() const {
    return values.size();
}

void Column::AddValue(const std::string& value) {
    if (value == "NULL") {
        values.emplace_back();
    } else if (ValidValue(value)) {
        values.emplace_back(value);
    } else {
        return;
    }
}

void Column::AddDefault() {
    values.emplace_back();
}

void Column::SetValue(const std::string& value, std::size_t ind) {
    if (!ValidValue(value)) {
        return;
    }
    values[ind].SetValue(value);
}

void Column::DeleteValue(std::size_t ind) {
    values.erase(values.begin() + ind);
}

void Column::CopyValueFrom(Column* tmp, std::size_t ind) {
    values.push_back(tmp->values[ind]);
}

bool Column::Compare(const Column& other,
                     const SQL_Query::Condition& operation,
                     std::size_t ind_1, std::size_t ind_2) const {
    if (type_ == INT) {
        return operation.Compare(values[ind_1].GetValueAsInt(), other.values[ind_2].GetValueAsInt());
    }
    if (type_ == DOUBLE) {
        return operation.Compare(values[ind_1].GetValueAsDouble(), other.values[ind_2].GetValueAsDouble());
    }
    if (type_ == BOOL) {
        return operation.Compare(values[ind_1].GetValueAsBool(), other.values[ind_2].GetValueAsBool());
    }
    if (type_ == FLOAT) {
        return operation.Compare(values[ind_1].GetValueAsFloat(), other.values[ind_2].GetValueAsFloat());
    }
    if (type_ == VARCHAR) {
        return operation.Compare(values[ind_1].GetValueAsString(), other.values[ind_2].GetValueAsString());
    }
    return false;
}

bool Column::ValidValue(const std::string& value) const {
    AvaliableValue(value);
    if (type_ == VARCHAR) {
        if (!(value.front() == '\'' && value.back() == '\'')){
            return false;
        }
    }
    if (type_ == BOOL) {
        if(value != "true" && value != "false" && value != "0" && value != "1"){
            return false;
        }
    }
    return true;
}

bool Column::Compare(const std::string& other, const SQL_Query::Condition& operation, std::size_t ind) const {
    Value tmp = values[ind];
    if (operation.GetOperation() == SQL_Query::Condition::IS_NOT_NULL) {
        return !tmp.IsNull();
    }
    if (operation.GetOperation() == SQL_Query::Condition::IS_NULL) {
        return tmp.IsNull();
    }
    if (tmp.IsNull()) {
        return false;
    }
    Value new_tmp(other);
    if (type_ == INT) {
        return operation.Compare(tmp.GetValueAsInt(),new_tmp.GetValueAsInt());
    }
    if (type_ == DOUBLE) {
        return operation.Compare(tmp.GetValueAsDouble(),new_tmp.GetValueAsDouble());
    }
    if (type_ == BOOL) {
        return operation.Compare(tmp.GetValueAsBool(),new_tmp.GetValueAsBool());
    }
    if (type_ == FLOAT) {
        return operation.Compare(tmp.GetValueAsFloat(),new_tmp.GetValueAsFloat());
    }
    if (type_ == VARCHAR) {
        return operation.Compare(tmp.GetValueAsString(),new_tmp.GetValueAsString());
    }
    return false;
}

void Column::SetForeignKey(const std::string& table, Column* tmp) {
    foreign_key = {table, tmp};
}

std::string Column::GetForeignKeyAsStr() const {
    if (foreign_key.second == nullptr) {
        return "";
    }
    return foreign_key.first + '(' + foreign_key.second->name_column + ')';
}

bool Column::ValidForeignKey(const Value& value) const {
    if (foreign_key.second == nullptr) {
        return true;
    }
    auto tmp = std::find_if(foreign_key.second->values.begin(),
                            foreign_key.second->values.end(),
                            [value](const Value& tmp) {return tmp == value;});
    if (tmp == foreign_key.second->values.end()) {
        return false;
    }
    return true;
}

bool Column::ValidPrimaryKey(const std::string& value) const {
    if (!is_primary_key) {
        return true;
    }
    auto tmp = std::find_if(values.begin(),
                            values.end(),
                            [value](const Value& tmp) {return value == tmp.GetValueAsString();});
    if (tmp != values.end()) {
        return false;
    }
    return true;
}

void Column::AvaliableValue(const std::string& value) const {
    if (!ValidForeignKey(Value(value)) || !ValidPrimaryKey(value)) {
        if (!ValidForeignKey(Value(value))) {
            throw std::invalid_argument("Invalid foreign key");
        } else {
            throw std::invalid_argument("Invalid primary key");
        }
    }
}

Value Column::GetValueFromIndex(std::size_t ind) const {
    return values[ind];
}




