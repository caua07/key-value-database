#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <optional>

class KeyValueStore {
  private:
    std::unordered_map<std::string, std::string> db;

  public:

    //CRUD
    bool
    put(std::string key, std::string data);

    std::optional<std::string>
    get(std::string& key);

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
