#include "handmade.h"
#include <fstream>
#include <chrono>
#include <thread>
#include <shared_mutex>
#include "json.hpp"



Status
KeyValueStore::put(const std::string& key,const std::string& data)
{ 
  std::unique_lock<std::shared_mutex> lock(mutex);
  if (key.empty() || data.empty()){
    std::cout << "Error: not valid group or data";
    return Status::ServerError();
  }

  auto res = db.emplace(key, data);

  if (res.second) {
    std::cout << "\nSuccess: " << data << " inserted into " << key << '\n';
    return Status::OK();
  }
  return Status::ServerError();
}

Status
KeyValueStore::get(const std::string& key) const
{
  std::shared_lock<std::shared_mutex> lock(mutex);
  auto it = db.find(key);
  if (it == db.end()){
    return Status::NotFound(key);
  } else {
    return Status::OK();
  }
}

Status
KeyValueStore::remove(const std::string&  key)
{
  std::unique_lock<std::shared_mutex> lock(mutex);
  auto erase = db.erase(key);
  if (erase > 0) {
    std::cout << key << " completely erased from the database\n ";
    return Status::OK();
  }
  if(db.find(key) == db.end()){
    return Status::NotFound(key);
  } else {
    return Status::ServerError();
  }
}

Status
KeyValueStore::update(const std::string& key,const std::string& value)
{
  std::unique_lock<std::shared_mutex> lock(mutex);
  if (db.find(key) == db.end()){
    return Status::NotFound(key);
  } else {
    db[key] = value;
    return Status::OK();
  }
}

size_t
KeyValueStore::size()
{
  std::unique_lock<std::shared_mutex> lock(mutex);
  return db.size();
}

Status
KeyValueStore::save()
{
  std::unique_lock<std::shared_mutex> lock(mutex);
  std::ofstream storage("data.json");
  
  if(!storage.is_open()) {
    return Status::IOError();
  } 

  std::cout << "saving information...\n"; 
  auto it = db.begin();

  storage << "{\n";

  while (it != db.end()) {
    storage << "  \"" << it->first << "\": \"" << it->second << "\"";

    ++it;

    if (it != db.end()) {
      storage << ",\n";
    }
  }    

  storage << "\n}\n";
  storage.close();

  std::cout << "save complete.\n";
  return Status::OK();
}

Status
KeyValueStore::load()
{
  std::unique_lock<std::shared_mutex> lock(mutex);
  using json = nlohmann::json;
  std::ifstream storage("data.json");
  
  if (!storage.is_open()){
    std::cerr << "Error: Could not open file, verify if file exists\n";
    return Status::IOError();
  }

  try {
    json data = json::parse(storage);

    db.clear();

    db = data.get<std::unordered_map<std::string, std::string>>();
    
    std::cout << "Data loaded successfully.\n";

  } catch (json::parse_error& e){
    std::cerr << "Error parsing JSON: " << e.what() << '\n';
    return Status::ParseError();
  }
  return Status::OK();
}
