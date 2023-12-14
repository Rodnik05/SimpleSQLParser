#include <stdexcept>

#include "SQL_Query.h"


SQL_Query::SQL_Query(const std::string& req) {
    SetType(req);
    if (type_ == RequestType::NONE) {
        throw std::invalid_argument("Invalid request");
    }
}

SQL_Query::RequestType SQL_Query::GetType() const {
    return type_;
}

std::unordered_map<std::string, std::string> SQL_Query::GetData() const {
    return columns_values;
}

std::vector<SQL_Query::Condition> SQL_Query::GetConditions() const {
    return conditions;
}

std::string SQL_Query::GetTableName() const {
    return table_name;
}


void SQL_Query::SetCondition(const std::string& tmp)  {
    if (tmp.empty()) {
        return;
    }

    std::regex regex(R"((WHERE|AND|OR|ON)\s+([^\s=]+)\s+([((IS)<>=(!=)])\s*((NOT NULL|NULL)|(?:'[^']*')|(?:[^,\s]+)+))");
    std::smatch matches;

    auto start = tmp.cbegin();
    while (std::regex_search(start, tmp.cend(), matches, regex)) {
        if (matches.size() >= 5) {

            Condition tmp_condition;
            tmp_condition.SetCondition(matches[1].str());
            tmp_condition.SetLhs(matches[2].str());
            tmp_condition.SetOperation(matches[3].str());
            tmp_condition.SetRhs(matches[4].str());

            conditions.push_back(tmp_condition);
        } else {
            throw std::invalid_argument("Invalid condition");
        }

        start = matches.suffix().first;
    }

    if (conditions.empty()) {
        throw std::invalid_argument("Invalid condition");
    }
}

void SQL_Query::SetColumnValues(const std::string& tmp, std::vector<std::string>& vector) {
    std::regex format(R"(('[^']*')|([^,\s]+))");

    std::sregex_iterator end;
    for (std::sregex_iterator begin(tmp.begin(), tmp.end(), format); begin != end; ++begin) {
        vector.push_back(begin->str());
    }
}

bool SQL_Query::ValidSelect(const std::string& req) {
    std::regex regex(R"(SELECT\s+(.+)\s+FROM\s+([^\s]+)\s*(?:(WHERE\s+[^;]+))?[\s;]*)");
    std::smatch tmp;

    if (std::regex_match(req, tmp, regex) && tmp.size() >= 3) {
        std::string columns_str = tmp[1].str();
        if (columns_str == "*") {
            columns_values["*"] = "";
        } else {
            std::vector<std::string> columns;
            SetColumnValues(columns_str, columns);

            for (auto& column : columns) {
                columns_values[column] = "";
            }
        }
        table_name = tmp[2].str();
        if (tmp.size() >= 4) {
            SetCondition(tmp[3].str());
        }
        return true;
    }
    return false;
}

bool SQL_Query::ValidDelete(const std::string& req)  {
    std::regex regex(R"(DELETE\s+FROM\s+([^\s]+)\s*(?:(WHERE\s+[^;]+))?;*)");
    std::smatch tmp;

    if (std::regex_match(req, tmp, regex) && tmp.size() >= 2) {
        table_name = tmp[1].str();
        if (tmp.size() >= 3) {
            SetCondition(tmp[2].str());
        }
        return true;
    }
    return false;
}

bool SQL_Query::ValidInsert(const std::string &req) {
    std::regex regex(R"(INSERT\s+INTO\s+([^\s]+)\s*\(([^)]+)\)\s*VALUES\s*\(([^)]+)\);?)");
    std::smatch matched_strings;

    if (std::regex_match(req, matched_strings, regex) && matched_strings.size() >= 4) {
        table_name = matched_strings[1].str();
        std::vector<std::string> columns;
        std::vector<std::string> values;
        SetColumnValues(matched_strings[2].str(), columns);
        SetColumnValues(matched_strings[3].str(), values);
        if (columns.size() != values.size()) {
            throw std::invalid_argument("Invalid insert request");
        }
        for (size_t i = 0; i < columns.size(); ++i) {
            columns_values[columns[i]] = values[i];
        }
        return true;
    }
    return false;

}

bool SQL_Query::ValidUpdate(const std::string &req)  {
    std::regex regex(R"(UPDATE\s+([^\s]+)\s+SET\s+([^(?:(WHERE))]+)\s*(WHERE\s+[^;]*)?[\s;]*)");
    std::smatch matched_strings;

    if (std::regex_match(req, matched_strings, regex) && matched_strings.size() >= 3) {
        table_name = matched_strings[1].str();
        std::string columns_str = matched_strings[2].str();

        std::regex columns_regex("[^\\s=,]+|'[^']*'");
        std::sregex_iterator end;

        std::string column;
        for (std::sregex_iterator begin(columns_str.begin(), columns_str.end(), columns_regex); begin != end; ++begin) {
            std::string element_str = begin->str();
            if (column.empty()) {
                column = element_str;
            } else {
                columns_values[column] = element_str;
                column = "";
            }
        }
        if (matched_strings.size() >= 4) {
            SetCondition(matched_strings[3].str());
        }
        return true;
    }
    return false;
}

bool SQL_Query::ValidCreate(const std::string &req) {
    std::regex regex(R"(CREATE\s+TABLE\s+([^\s]+)\s+\(([\sA-Za-z0-9-_,\(\)]*)\)[\s;]*)");
    std::smatch matched_strings;

    if (std::regex_match(req, matched_strings, regex) && matched_strings.size() >= 2) {
        table_name = matched_strings[1].str();

        std::string struct_ = matched_strings[2].str();
        std::regex struct_regex(R"(([^\s,\(]+)\s+([^\s,]+(?:\s+(?:PRIMARY KEY))*))");

        std::string::const_iterator start(struct_.cbegin());
        while (std::regex_search(start, struct_.cend(), matched_strings, struct_regex)) {
            if (matched_strings.size() >= 3 &&
                matched_strings[2].str() != "KEY" &&
                matched_strings[2].str() != "REFERENCES") {
                columns_values[matched_strings[1].str()] = matched_strings[2].str();
            }
            start = matched_strings.suffix().first;
        }

        if (columns_values.empty()) {
            return false;
        }

        std::regex foreign_key_regex(R"(FOREIGN\s+KEY\s+\(([^\s]+)\)\s+REFERENCES\s+([^\s\)]+)\(([^\s,]+)\))");

        start = struct_.cbegin();
        while (std::regex_search(start, struct_.cend(), matched_strings, foreign_key_regex)) {
            if (matched_strings.size() >= 4) {
                columns_values[matched_strings[1].str()] = matched_strings[2].str() + " " + matched_strings[3].str();
                if (!columns_values.contains(matched_strings[1].str())) {
                    return false;
                }
            }
            start = matched_strings.suffix().first;
        }
        return true;
    }
    return false;
}

bool SQL_Query::ValidDrop(const std::string &req) {
    std::regex regex(R"(DROP\s+TABLE\s+([^\s;]+)[\s;]*)");
    std::smatch matched_strings;

    if (std::regex_match(req, matched_strings, regex) && matched_strings.size() >= 2) {
        table_name = matched_strings[1].str();
        return true;
    }
    return false;
}

bool SQL_Query::ValidJoin(const std::string& req) {
    std::regex regex(R"(SELECT\s+(.+)\s+FROM\s+([^\s]+)\s+(LEFT|RIGHT|INNER)?\s+JOIN\s+([^\s]+)\s+(ON\s+[^(?:WHERE)]+)\s*(?:(WHERE\s+[^;]+))?[\s;]*)");
    std::smatch matched_strings;
    if (std::regex_match(req, matched_strings, regex) && matched_strings.size() >= 6) {
        std::string columns_str = matched_strings[1].str();
        if (columns_str == "*") {
            columns_values["*"] = "";
        } else {
            std::vector<std::string> columns;
            SetColumnValues(columns_str, columns);

            for (auto& column : columns) {
                columns_values[column] = "";
            }
        }

        table_name = matched_strings[2].str();
        join_description_.join_type = matched_strings[3].str();
        join_description_.table_to_join = matched_strings[4].str();
        SetCondition(matched_strings[5].str());

        return true;
    }
    return false;
}

void SQL_Query::SetType(const std::string& req) {
    if (ValidSelect(req)) {
        type_ = SELECT;
    } else if (ValidInsert(req)) {
        type_ = INSERT;
    } else if (ValidDelete(req)) {
        type_ = DELETE;
    } else if (ValidUpdate(req)) {
        type_ = UPDATE;
    } else if (ValidCreate(req)) {
        type_ = CREATE;
    } else if (ValidDrop(req)) {
        type_ = DROP;
    } else if (ValidJoin(req)) {
        type_ = JOIN;
    }
}

void SQL_Query::Condition::SetCondition(const std::string& tmp) {
    if (tmp == "AND") {
        condition_ = AND;
    } else if (tmp == "OR") {
        condition_ = OR;
    } else if (tmp == "WHERE") {
        condition_ = WHERE;
    }  else if (tmp == "ON") {
        condition_ = ON;
    } else {
        throw std::invalid_argument("Invalid condition");
    }
}

void SQL_Query::Condition::SetLhs(const std::string& lhs) {
    lhs_ = lhs;
}

void SQL_Query::Condition::SetOperation(const std::string& tmp) {
    if (tmp == "=" || tmp == "IS") {
        operation_ = EQUALS;
    } else if (tmp == "!=") {
        operation_ = NOT_EQUALS;
    } else if (tmp == ">") {
        operation_ = MORE;
    } else if (tmp == ">=") {
        operation_ = MORE_OR_EQUALS;
    } else if (tmp == "<") {
        operation_ = LESS;
    } else if (tmp == "<=") {
        operation_ = LESS_OR_EQUALS;
    }
};

void SQL_Query::Condition::SetRhs(const std::string& rhs) {
    if (rhs == "NULL" || rhs == "NOT NULL") {
        if (operation_ != EQUALS && operation_ != IS_NULL && operation_ != IS_NOT_NULL) {
            throw std::invalid_argument("Invalid condition");
        }
        if(rhs == "NULL"){
            operation_ = IS_NULL;
        } else {
            operation_ = IS_NOT_NULL;
        }
    }
    rhs_ = rhs;
}

SQL_Query::Condition::Type SQL_Query::Condition::GetCondition() const {
    return condition_;
}

std::string SQL_Query::Condition::GetLhs() const {
    return lhs_;
}

SQL_Query::Condition::Operator SQL_Query::Condition::GetOperation() const {
    return operation_;
}

std::string SQL_Query::Condition::GetRhs() const {
    return rhs_;
}
