#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <shared_mutex>
#include <sstream>
#include <iomanip>
#include <chrono>

enum class Operation : uint8_t {
  PUT,
  UPDATE,
  DELETE,
  GET,
  BEGIN_TRANSACTION,
  COMMIT_TRANSACTION
};

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
};

class WalManager {
  private:
    std::vector<char> bufferPool;
    uint64_t write_pos;
    uint64_t read_pos;

    uint64_t LSN{0};
    uint64_t txnID{0};
    uint64_t flushedLsn{0};
    uint64_t totalOperations{0};
    uint64_t active_txnID{0};
    bool implicitCommit{false};
    mutable std::shared_mutex mutex;
  public:
    //what must be added
    // LSN : uint64_t | TIMESTAMP : uint64_t | txnID : uint64_t  | OP : std::string | KEY: std::string | VALUE : std::string | STATUS : logStatus | CHECKSUM : std::ios::bytes

    uint64_t currentTxnID{0};

    WalManager()
    {
      bufferPool.reserve(4096);
    }

    ~WalManager()
    {

    }


    
    Status
    flush()
    {
      //flush all from transactioninit to commit comment 
      
    }

    Status
    push(const std::string& key, const std::string& value)
    {
      std::unique_lock<std::shared_mutex> lock(mutex);

      if (key.empty()) return Status::ServerError();
      writeToBuffer({Operation::PUT, key, value, txnID});
      return Status::OK();
    }

    Status
    get(const std::string& key)
    {
      std::shared_lock<std::shared_mutex> lock(mutex);

      if (key.empty()) return Status::ServerError();
      // to be done...
    }

    uint64_t
    beginTxn()
    {
      std::unique_lock<std::shared_mutex> lock(mutex);

      if (active_txnID != 0 ){
        std::cerr << "Error: Transaction aready in progress.\n";
        return 0;
      }
      ++active_txnID;
      ++txnID;
      writeToBuffer(Operation::BEGIN_TRANSACTION, "", "", txnID);
      return txnID;
    }
    
    Status
    commit()
    {
      std::unique_lock<std::shared_mutex> lock(mutex);

      if (active_txnID == 0) {
        std::cerr << "Error: can't commit if theres no transactin open";
        return Status::ServerError();
      }
      writeToBuffer(Operation::COMMIT_TRANSACTION, "", "", txnID);
      active_txnID = 0;
      return Status::OK();
    }

  private:

    Status
    writeToBuffer(const logEntry& entry)
    {
      if (active_txnID == 0) {
        implicitCommit = true;
        beginTxn();
      }

      auto now = std::chrono::system_clock::now();
      uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();     
      
      uint32_t len1 = entry.key.size();
      uint32_t len2 = entry.value.size();
      uint8_t operation = static_cast<uint8_t>(logEntry.op);

      size_t original_buffer_size = bufferPool.size();
      bufferPool.resize(original_buffer_size + sizeof(uint64_t)*2 + sizeof(uint32_t)*2 + sizeof(uint8_t) + len1 + len2 );
      char* ptr = bufferPool.data() + original_buffer_size;

      //LSN
      std::memcpy(ptr, &LSN, sizeof(uint64_t)); ptr += sizeof(uint64_t);
      //timestamp
      std::memcpy(ptr, &timestamp, sizeof(uint64_t)); ptr += sizeof(uint64_t);
      //id
      std::memcpy(ptr, &txnID, sizeof(uint64_t)); ptr += sizeof(uint64_t);
      //operation
      std::memcpy(ptr, &operation, sizeof(uint8_t)); ptr += sizeof(uint8_t);
      //key len
      std::memcpy(ptr, &len1, sizeof(uint32_t)); ptr += sizeof(uint32_t);
      //key
      std::memcpy(ptr, entry.key.data(), len1); ptr += len1;
      //value len
      std::memcpy(ptr, &len2, sizeof(uint32_t)); ptr += sizeof(uint32_t);
      //value
      std::memcpy(ptr, entry.value.data(), len2); ptr += len2;


      ++LSN;
      ++totalOperations;
      if (implicitCommit == true){
        commit();
      }
    }
    
};
