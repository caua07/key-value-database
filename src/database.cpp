#include "handmade.h"
#include <fstream>
#include <optional>
#include <chrono>
#include <thread>
#include <shared_mutex>


Status
KeyValueStore::put(const std::string& key,const std::string& data)
{ 
  std::unique_lock<std::shared_mutex> lock(mutex);
  if (key.empty()){
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
KeyValueStore::putBatch(const std::vector<std::pair<std::string, std::string>>& vec)
{
  std::unique_lock<std::shared_mutex> lock(mutex);
  if (vec.size() == 0){
    return Status::ServerError();
  }
  unsigned int errors{0};
  unsigned int successes{0};

  for (const auto& [key, data]: vec){
    if (key.empty()){
      ++errors;
      continue;
    }
    auto res = db.emplace(key, data);
    ++successes;
  }
  if (successes > 0 && errors > 0){
    std::cout << "\nWarning: not all data was inserted into database\n"; 
    return Status::OK();
  } else if (successes > 0 && errors == 0) {
    std::cout << "\nSuccess: all data inserted into database\n";
    return Status::OK();
  } else {
    std::cout << "\nError: data not successfully inserted into database\n";
    return Status::ServerError();
  }
}

std::optional<std::vector<std::string>>
KeyValueStore::listKeys()
{
  std::unique_lock<std::shared_mutex> lock(mutex);

  std::vector<std::string> vec;
  for (const auto& [key, value]: db){
    std::cout << key << '\n';
    vec.emplace_back(value);
  }
  
  return vec;
}

Status
KeyValueStore::get(const std::string& key) const
{
  std::shared_lock<std::shared_mutex> lock(mutex);
  auto it = db.find(key);
  if (it == db.end()){
    return Status::NotFound(key);
  } else {
    return Status::OK(it->second);
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
    std::cout << "Success: updated " << key << " to " << value << '\n';
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
  serializer writer;
  alignedSerializer alignedWriter;

  std::unique_lock<std::shared_mutex> lock(mutex);
  std::ofstream file;
  std::ofstream alignedFile;

  file.open("data.bin", std::ios::out | std::ios::binary);
  alignedFile.open("alignedData.bin", std::ios::out | std::ios::binary);
  
  if(!file.is_open() || !alignedFile.is_open()) {
    std::cerr << "Error: Could not open file for writing." << '\n';
    return Status::IOError();
  } 

  std::cout << "saving information...\n"; 

  writer.write_header(db.size());
  alignedWriter.write_header(db.size());

  for (const auto& [key, value]: db){
    writer.write_str(key);
    writer.write_str(value);
  }
  for (const auto& [key, value]: db){
    alignedWriter.write_str(key);
    alignedWriter.write_str(value);
  }

  if (!writer.save_in_file("data.bin") || !alignedWriter.save_in_file("alignedData.bin")) {
    std::cerr << "Error: could not save content in file." << '\n';
    return Status::IOError();
  }


  std::cout << "save complete. Saved " << db.size() << " entries.\n";
  return Status::OK();
}

Status
KeyValueStore::load()
{
  std::unique_lock<std::shared_mutex> lock(mutex);

  serializer reader;
  alignedSerializer alignedReader;

  if (!reader.load_from_file("data.bin")) {
    return Status::IOError();
  }
  if (!alignedReader.load_from_file("alignedData.bin")) {
    return Status::IOError();
  }

  uint64_t num_entries;
  if (!reader.read_header(num_entries)){
    std::cerr << "Error: could not read content from file.\n";
    return Status::ParseError();
  }
  if (!alignedReader.read_header(num_entries)){
    std::cerr << "Error: could not read content from file.\n";
    return Status::ParseError();
  }
  std::cout << "Loading " << num_entries << " entries...\n";

  db.clear();

  for (uint64_t i{0}; i < num_entries; ++i){
    std::string key, value;
    if(!reader.read_str(key)){
      std::cerr << "Error: Failed to read key at entry " << i << '\n';
      return Status::ParseError();
    }

    if (!reader.read_str(value)){
      std::cerr << "Error: Failed to read key at entry " << i << '\n';
      return Status::ParseError();
    }

    db.emplace(key, value);
  }
  for (uint64_t i{0}; i < num_entries; ++i){
    std::string key, value;
    if(!alignedReader.read_str(key)){
      std::cerr << "Error: Failed to read key at entry " << i << '\n';
      return Status::ParseError();
    }

    if (!alignedReader.read_str(value)){
      std::cerr << "Error: Failed to read key at entry " << i << '\n';
      return Status::ParseError();
    }

    db.emplace(key, value);
  }

  std::cout << "Load complete. Loaded " << num_entries << " entries.\n";
  reader.reset();
  alignedReader.reset();
  return Status::OK();
}
