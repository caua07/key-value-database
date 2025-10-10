#pragma once 
#include <string>

enum class StatusCode { OK, NotFound, KeyEmpty, ValueEmpty, IOError, ParseError, ServerError };

struct Status {
  StatusCode code;
  std::string message;

  bool ok() const { return code == StatusCode::OK; }
  static Status OK(const std::string& message = "")
  {
    return {StatusCode::OK, message};
  }
  static Status NotFound(const std::string& key) {
    return {StatusCode::NotFound, "Key not found: " + key};
  }
  static Status KeyEmpty() { return {StatusCode::KeyEmpty, ""};};
  static Status ValueEmpty(const std::string& key)
  {
    return {StatusCode::ValueEmpty, "key holding empty value: " + key};
  };
  static Status IOError() { return {StatusCode::IOError, ""};};
  static Status ParseError() { return {StatusCode::ParseError, ""};};
  static Status ServerError() { return {StatusCode::ServerError, ""};};
};
