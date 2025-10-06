#include "handmade.h"
#include <fstream>
#include "json.hpp"

bool
KeyValueStore::put(std::string key, std::string data)
{ 
  if (key.empty() || data.empty()){
    std::cout << "Error: not valid group or data";
    return {};
  }

  auto res = db.emplace(key, data);

  if (res.second) {
    std::cout << "\nSuccess: " << data << " inserted into " << key << '\n';
    return res.second;
  }
  return {};
}

std::optional<std::string>
KeyValueStore::get(std::string& key)
{
  if (db.find(key) == db.end()){
    return {};
  } else {
    return db[key];
  }
}

bool
KeyValueStore::remove(std::string key)
{
  auto data = get(key);
  if (data) {
    auto erased = db.erase(key);
    if (erased > 0) {
      std::cout << key << " completely erased from the database\n ";
      return true;
    }
  }
  return false;
}

bool
KeyValueStore::update(std::string key, std::string value)
{
  if (db.find(key) == db.end()){
    return false;
  } else {
    db[key] = value;
    return true;
  }
  return false;
}

size_t
KeyValueStore::size()
{
  return db.size();
}

bool
KeyValueStore::save()
{
  std::ofstream storage("data.json");
  
  if(storage.is_open()){
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
    return true;
  } else {
    std::cerr << "Error in oppening file\n";
    return false;
  }

  return false;
}

bool
KeyValueStore::load()
{
  using json = nlohmann::json;
  std::ifstream storage("data.json");
  
  if (!storage.is_open()){
    std::cerr << "Error: Could not open file, verify if file exists\n";
    return false;
  }

  try {
    json data = json::parse(storage);

    db.clear();

    db = data.get<std::unordered_map<std::string, std::string>>();
    
    std::cout << "Data loaded successfully.\n";

  } catch (json::parse_error& e){
    std::cerr << "Error parsing JSON: " << e.what() << '\n';
    return false;
  }
  return true;
}
