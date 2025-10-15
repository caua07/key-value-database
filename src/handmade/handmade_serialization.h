#pragma once
#include <string>
#include <cstdint>
#include <cstring>
#include <fstream>

// print function requirements
#include <cctype>
#include <iomanip>

class alignedSerializer {
  private:
    std::vector<char> buffer;
    size_t write_pos{0}; //write current offset
    size_t read_pos{0}; //read current offset

    static constexpr uint32_t MAGIC = 0x4B564442;  // "KVDB" in hex
    static constexpr uint32_t VERSION = 1;
 
  public:
    alignedSerializer(){
      buffer.reserve(4096);
    }
  
    // WRITING ... 

    // calculates alignment needs of data types
    inline size_t align_up(size_t value, size_t alignment) {
        return (value + alignment - 1) & ~(alignment - 1);
    }
    
    // helper function for align_up
    template<typename T>
    inline size_t align_for(size_t value) {
        return align_up(value, alignof(T));
    }

    void
    write_header(uint64_t entries)
    {
      write_POD<uint32_t>(MAGIC);
      write_POD<uint32_t>(VERSION);
      write_POD<uint64_t>(entries);
    }

    template<typename T>
    void write_POD(const T& data)
    {
      size_t aligned_offset = align_for<T>(write_pos);
      size_t padding = aligned_offset - write_pos;
      buffer.resize(write_pos + padding + sizeof(T));

      if (padding > 0) {
        buffer.insert(buffer.begin() + write_pos, padding, 0);
      }
      write_pos += padding;
      memcpy(buffer.data() + write_pos, &data, sizeof(T)); 
      write_pos += sizeof(T);
    }

    void
    write_str(const std::string& str)
    {
      size_t helper = align_for<size_t>(write_pos);
      size_t padding = helper - write_pos;

      if (padding > 0) {
        buffer.insert(buffer.begin() + write_pos, padding, 0);
      }
      write_pos += padding;
      
      size_t len = str.size();

      buffer.resize(write_pos + sizeof(size_t) + len);

      std::memcpy(buffer.data() + write_pos, &len, sizeof(size_t));

      write_pos += sizeof(size_t);
      std::memcpy(buffer.data() + write_pos, str.data(), str.size());

      write_pos += str.size();
      std::cout << "final current_offset: " << write_pos << '\n';
    } 

    // READING ...

    // I/O operaions ...

    bool
    save_in_file(const std::string& filename)
    {
      std::ofstream file(filename, std::ios::binary);
      if (!file) {
        return false;
      }

      file.write(reinterpret_cast<const char*>(buffer.data()), write_pos);
      return file.good();
    }

    // UTILITIES
    
    void
    reset()
    {
      buffer.clear();
      write_pos = 0;
      read_pos = 0;
    }

    size_t
    size() const
    {
      return write_pos;
    }

    void
    printBuffer() {
      std::cout << "Buffer content: '";
      for(const char& c : buffer) {
        // We must cast to unsigned char for isprint to work correctly with all byte values
        if (isprint(static_cast<unsigned char>(c))) {
          // It's a printable character, print it directly
          std::cout << c;
        } else {
          // It's not printable, print its hex code instead
          std::cout << "\\x" // A common prefix for hex values
                    << std::hex
                    << std::setw(2)
                    << std::setfill('0')
                    << static_cast<int>(static_cast<unsigned char>(c));
        }
      }
      std::cout << "'" << std::endl;
      // Reset cout to decimal for other parts of the program
      std::cout << std::dec;
    }

  private:
    void
    ensure_space(size_t bytes)
    {
      if (write_pos + bytes > buffer.size()) {
        buffer.resize(write_pos + bytes);
      }
    }

};
