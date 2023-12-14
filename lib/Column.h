#pragma once

#include "SQL_Query.h"
#include "Value.h"

#include <string>
#include <vector>

class Column {
public:
    enum ValueType {
        INT,
        VARCHAR,
        BOOL,
        DOUBLE,
        FLOAT
    };

protected:
    std::string name_column;
    ValueType type_;
    std::vector<Value> values;
    std::pair<std::string, Column*> foreign_key = {"", nullptr};
    bool is_primary_key = false;

public:

    explicit Column(const std::string& name, ValueType type){
        name_column = name;
        type_ = type;
    }

    ~Column() = default;

    const std::string& GetName() const;
    ValueType GetType() const;
    std::string GetValueAsStr(std::size_t ind) const;

    std::size_t Size() const;

    void AddValue(const std::string& value);
    void AddDefault();
    void SetValue(const std::string& value, std::size_t ind);
    bool ValidValue(const std::string& value) const;
    Value GetValueFromIndex(std::size_t ind) const;

    void DeleteValue(std::size_t ind);
    void CopyValueFrom(Column* tmp, std::size_t ind);

    bool Compare(const Column& other,
                 const SQL_Query::Condition& operation,
                 std::size_t ind_1, std::size_t ind_2 = SIZE_MAX) const;

    bool Compare(const std::string& other, const SQL_Query::Condition& operation, std::size_t ind) const;


    void SetForeignKey(const std::string& table, Column* tmp);
    std::string GetForeignKeyAsStr() const;
    void SetPrimaryKeyFlag(bool value);
    bool ValidForeignKey(const Value& value) const;
    bool ValidPrimaryKey(const std::string & value) const;
    void AvaliableValue(const std::string& value) const;

    friend class ResultSet;
};
