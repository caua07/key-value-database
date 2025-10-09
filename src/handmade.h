#pragma once
#include "./handmade/handmande_threadpool.h"
#include <iostream>
#include <shared_mutex>
#include <unordered_map>
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

class KeyValueStore {
  protected:
    std::unordered_map<std::string, std::string> db;

  public:
    // public variables
    mutable std::shared_mutex mutex;

    //CRUD
    Status
    put(const std::string& key,const std::string& data);

    Status
    putBatch(const std::vector<std::pair<std::string, std::string>>& vec);

    Status 
    get(const std::string& key) const;

    Status
    update(const std::string& key,const std::string& data);

    Status
    remove(const std::string& key);

    Status
    exists(const std::string& key);

    std::optional<std::vector<std::string>>
    listKeys();

    size_t
    size();


    // I/0 ops
    Status
    save();
    
    Status
    load();

};
