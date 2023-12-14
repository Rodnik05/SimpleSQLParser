#pragma once

#include <ostream>
#include <stdexcept>

class Value {
private:
    std::string value_;
    bool null_ = true;

public:
    Value()= default;

    explicit Value(const std::string& value){
        value_ = value;
        null_ = false;
    }

    [[nodiscard]] bool IsNull() const;

    bool operator==(const Value& other) const;

    friend std::ostream& operator<<(std::ostream& tmp, const Value& element);

    void SetValue(const std::string& value);

    int GetValueAsInt() const;
    double GetValueAsDouble() const;
    float GetValueAsFloat() const;
    bool GetValueAsBool() const;
    std::string GetValueAsString() const;
};

