#pragma once
#include <string>
#include <cstdint>
#include <cstring>
#include <fstream>

// print function requirements
#include <cctype>
#include <iomanip>

class serializer {
  private:
    std::vector<char> buffer;
    size_t write_pos{0}; //write current offset
    size_t read_pos{0}; //read current offset

    static constexpr uint32_t MAGIC = 0x4B564442;  // "KVDB" in hex
    static constexpr uint32_t VERSION = 1;
 
  public:
    serializer(){
      buffer.reserve(4096);
    }
  
    // WRITING ... 

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
      buffer.resize(write_pos + sizeof(T));
      memcpy(buffer.data() + write_pos, &data, sizeof(T)); 
      write_pos += sizeof(T);
    }

    void
    write_str(const std::string& str)
    {
      size_t len = str.size();

      buffer.resize(write_pos + sizeof(size_t) + len);

      std::memcpy(buffer.data() + write_pos, &len, sizeof(size_t));

      write_pos += sizeof(size_t);
      std::memcpy(buffer.data() + write_pos, str.data(), str.size());

      write_pos += str.size();
    }

    // READING ...

    template<typename T>
    bool
    read_POD(T& data)
    {  
      if(read_pos + sizeof(T) > buffer.size()) {
        return false;
      }

      std::memcpy(&data, buffer.data() + read_pos, sizeof(T));
      read_pos += sizeof(T);
      return true;
    }

    bool
    read_str(std::string& str)
    {
      if (read_pos + sizeof(size_t) > buffer.size()){
        return false;
      }
      size_t len;

      std::memcpy(&len, buffer.data() + read_pos, sizeof(size_t));
      read_pos += sizeof(size_t);

      if(read_pos + len > buffer.size()){ 
        return false;
      }
    
      str.assign(buffer.data() + read_pos, len);
      read_pos += len;

      return true;
    }

    bool
    read_header(uint64_t& entries)
    {
      uint32_t magic, version;
      if (!read_POD<uint32_t>(magic)) return false;
      if (magic != MAGIC) {
        std::cerr << "Error: invalid magic number" << '\n';
        return false;
      }
      if (!read_POD<uint32_t>(version)) {
        std::cerr << "Unsupported version";
        return false;
      }
      if (!read_POD<uint64_t>(entries)) return false;
      return true;
    }

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

    bool
    load_from_file(const std::string& filename)
    {
      std::ifstream file(filename, std::ios::binary);

      if (!file) {
        std::cerr << "Error: unable to open file." << '\n';
        return false;
      }

      // get file size.
      file.seekg(0, std::ios::end);
      size_t filesize = file.tellg();
      file.seekg(0, std::ios::beg);

      buffer.resize(filesize);
      file.read(buffer.data(), filesize);

      if (!file.good()) {
        std::cerr << "Error: problem with file integrity." << '\n';
        return false;
      }

      write_pos = filesize;
      read_pos = 0;

      return true;
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
