#pragma once
#include "./handmade/handmade_threadpool.h"
#include "./handmade/handmade_errorHandle.h"
#include "./handmade/handmade_serialization.h"
#include "./handmade/handmade_alignedSerialization.h"
#include <optional>
#include <iostream>
#include <shared_mutex>
#include <unordered_map>
#include <string>


class KeyValueStore {
  private:
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
