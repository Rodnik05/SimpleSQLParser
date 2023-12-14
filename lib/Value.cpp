#include "Value.h"



bool Value::IsNull() const {
    return null_;
}

bool Value::operator==(const Value& other) const {
    return null_==other.null_ && value_==other.value_ ;
}

std::ostream& operator<<(std::ostream& tmp, const Value& element) {
    if (element.null_) {
        tmp << "NULL";
        return tmp;
    }
    tmp << element.value_;
    return tmp;
}

void Value::SetValue(const std::string& value) {
    if (value=="NULL"){
        null_ = true;
        return;
    } else {
        value_ = value;
        null_ = false;
    }
}

int Value::GetValueAsInt() const {
    return std::strtol(value_.c_str(), nullptr, 10);
}

float Value::GetValueAsFloat() const {
    return std::strtof(value_.c_str(), nullptr);
}

double Value::GetValueAsDouble() const {
    return std::strtod(value_.c_str(), nullptr);
}

bool Value::GetValueAsBool() const {
    if (value_ == "false" || value_ == "0") {
        return false;
    }
    if (value_ == "true" || value_ == "1") {
        return true;
    }
    throw std::invalid_argument("Invalid boolean value");
}

std::string Value::GetValueAsString() const {
    return value_.substr(1,value_.size() - 2);
}
