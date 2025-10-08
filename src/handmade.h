#pragma once
#include "./thread_management/handmande_threadpool.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <optional>
#include <shared_mutex>

class KeyValueStore {
  protected:
    std::unordered_map<std::string, std::string> db;

  public:
    // public variables
    mutable std::shared_mutex mutex;


    //CRUD
    bool
    put(std::string key, std::string data);

    std::optional<std::string>
    get(std::string& key) const;

    bool
    update(std::string key, std::string data);

    bool
    remove(std::string key);

    bool
    exists(std::string key);

    size_t
    size();


    // I/0 ops
    bool
    save();
    
    bool
    load();

};
