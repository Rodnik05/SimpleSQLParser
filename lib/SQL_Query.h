#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <regex>

static std::vector<std::string> split_by_character(const std::string& tmp, char ch) {
    std::vector<std::string> result;
    std::stringstream stream(tmp);
    std::string string;
    while (std::getline(stream, string, ch)) {
        result.push_back(string);
    }
    return result;
}


class SQL_Query {
public:
    explicit SQL_Query(const std::string& req);
    struct Condition {
        enum Type {
            AND,
            OR,
            WHERE,
            ON
        };
        enum Operator {
            EQUALS,
            NOT_EQUALS,
            MORE,
            MORE_OR_EQUALS,
            LESS,
            LESS_OR_EQUALS,
            IS_NULL,
            IS_NOT_NULL
        };
    private:
        Type condition_;
        std::string lhs_;
        Operator operation_;
        std::string rhs_;
    public:
        template<typename T>
        bool Compare(const T& lhs, const T& rhs) const {
            bool result = false;
            if (operation_ == EQUALS){
                result = lhs == rhs;
            }
            if (operation_ == NOT_EQUALS){
                result = lhs != rhs;
            }
            if (operation_ == NOT_EQUALS){
                result = lhs != rhs;
            }
            if (operation_ == MORE){
                result = lhs > rhs;
            }
            if (operation_ == LESS){
                result = lhs < rhs;
            }
            if (operation_ == MORE_OR_EQUALS){
                result = (lhs > rhs || lhs == rhs);
            }
            if (operation_ == LESS_OR_EQUALS){
                result = (lhs < rhs || lhs == rhs);
            }
            if (operation_ == IS_NOT_NULL){
                result = true;
            }
            return result;
        }

        void SetCondition(const std::string& tmp);
        void SetLhs(const std::string& lhs);
        void SetOperation(const std::string& tmp);
        void SetRhs (const std::string& rhs);

        [[nodiscard]] Type GetCondition() const;
        [[nodiscard]] std::string GetLhs() const;
        [[nodiscard]] Operator GetOperation() const;
        [[nodiscard]] std::string GetRhs() const;
    };

    enum RequestType {
        SELECT,
        CREATE,
        DROP,
        INSERT,
        UPDATE,
        DELETE,
        JOIN,
        NONE
    };

    [[nodiscard]] RequestType GetType() const;
    [[nodiscard]] std::unordered_map<std::string, std::string> GetData() const;
    [[nodiscard]] std::vector<Condition> GetConditions() const;
    [[nodiscard]] std::string GetTableName() const;

    friend class Table;
    struct JoinDescriptionType {
        std::string join_type;
        std::string table_to_join;
    };
    const JoinDescriptionType& GetJoinDescription() const {
        return join_description_;
    }

private:
    void SetCondition(const std::string& tmp);
    static void SetColumnValues(const std::string& tmp, std::vector<std::string>& vector);
    void SetType(const std::string& req);

    bool ValidSelect(const std::string& req);
    bool ValidDelete(const std::string& req);
    bool ValidInsert(const std::string& req);
    bool ValidUpdate(const std::string& req);
    bool ValidCreate(const std::string& req);
    bool ValidDrop(const std::string& req);
    bool ValidJoin(const std::string& req);


    RequestType type_ = RequestType::NONE;
    std::string table_name;
    std::unordered_map<std::string, std::string> columns_values;
    JoinDescriptionType join_description_;
    std::vector<Condition> conditions;
};
