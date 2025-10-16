#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>

enum class Operation : uint8_t {
  PUT,
  UPDATE,
  DELETE,
  GET
}

enum logStatus {
  PENDING,
  COMMITED,
  FLUSHED 
};

struct logEntry {
  Operation op;
  std::string key;
  std::string value;
  uint64_t txnID;
}

class WalManager {
  private:
    std::vector<char> walBuffer;
    uint64_t write_pos;
    uint64_t read_pos;

    uint64_t LSN{0};
    uint64_t flushedLsn{0};
    uint64_t totalOperations{0};
  public:
    //what must be added
    // LSN : uint64_t | TIMESTAMP : uint64_t | txnID : uint64_t  | OP : std::string | KEY: std::string | VALUE : std::string | STATUS : logStatus | CHECKSUM : std::ios::bytes

    uint64_t currentTxnID{0};

    WalManager()
    {
      serializer walWriter; 
      walBuffer.reserve(4096);
    }

    ~WalManager()
    {

    }


    Status
    writeToBuffer(const logEntry& entry)
    {
      if (key.empty()){
        std::cout << "Key invalid for this operation\n";
        return Status::ServerError();
      }
      std::stringstream ss;
      auto now = std::chrono::system_clock::now();
      uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();     
      
      uint32_t len0 = time_stamp.size();
      uint32_t len1 = entry.key.size();
      uint32_t len2 = entry.value.size();
      uint8_t operation = static_cast<uint8_t>(logEntry::op);

      walBuffer.resize(sizeof(uint64_t) + sizeof(uint32_t)*3 + len1 + len2 + opLen);

      //LSN
      buffercopy(&LSN, sizeof(uint64_t));
      write_pos += sizeof(uint64_t);

      //timestamp
      buffercopy(&timestamp, sizeof(uint64_t));
      write_pos += sizeof(uint64_t);

      //operation
      buffercopy(&operation, sizeof(uint8_t));
      write_pos += sizeof(uint8_pos);

      //key len
      buffercopy(&len1, sizeof(uint32_t));
      write_pos += sizeof(uint32_t);

      //key
      buffercopy(entry.key.data(), len1);
      write_pos += len1;

      //value len
      buffercopy(&len2, sizeof(uint32_t));
      write_pos += sizeof(uint32_t);

      //value
      buffercopy(entry.value.data(), len2);
      write_pos += len2;
      ++LSN;
      ++totalOperations;
    }

    Status
    update(const std::string& key, const std::string& value)
    {

    }
    
    Status
    flush()
    {
      
    }

    void
    beginTxn()
    {
      ++currentTxnID{0};
    }
    
    bool
    commit()
    {
      
    }

    void
    buffercopy(const void* src, uint32_t count)
    {
      std::memcpy(walBuffer.data() + write_pos, src, count);
    }
    
};
