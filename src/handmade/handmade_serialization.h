#pragma once
#include <string>
#include <cstdint>
#include <cstring>
#include <cctype>
#include <iomanip>

class writeData {
  private:
    std::vector<char> buffer;
    size_t cf{0}; //current offset
 
  public:
    writeData(){
      buffer.reserve(2048);
    }

     /**
       * @brief Aligns a given value up to the next multiple of a specified alignment.
     * * This function uses an efficient bitwise algorithm that only works when the
     * alignment is a power of two, which is always true for memory alignment.
     * * @param value The value to align.
     * @param alignment The alignment boundary (must be a power of two).
     * @return The aligned value.
     */
    inline size_t align_up(size_t value, size_t alignment) {
        return (value + alignment - 1) & ~(alignment - 1);
    }
    
    // helper function for align_up
    template<typename T>
    inline size_t align_for(size_t value) {
        return align_up(value, alignof(T));
    }

    //simply writes a single char tothe buffer
    //no need to align since char align is 1 byte, just slapit there an    //d increment cf.
    void
    write_char(const char& ch)
    {
      cf += 1;
      buffer.resize(cf); 
      std::memcpy(buffer.data() + cf, &ch, 1);
    }

    void
    bufferToDataBin(std::ofstream file){
      file.write(buffer[0]*, cf);
    }

    /**
     * @brief sums up lenght type and one byte for each char in the string

     * * @param the cf.
     * @param the string to be aligned.
     * @return The cf after alignment.
     */
    void
    write_str(const std::string& str)
    {
      size_t helper = cf;
      helper = align_for<size_t>(cf);
      size_t padding = helper - cf;

      if (padding > 0) {
        buffer.insert(buffer.begin() + cf, padding, 0);
      }
      cf += padding;
      
      size_t len = str.size();

      buffer.resize(cf + sizeof(size_t) + len);

      std::memcpy(buffer.data() + cf, &len, sizeof(size_t));

      cf += sizeof(size_t);
      std::memcpy(buffer.data() + cf, str.data(), str.size());

      cf += str.size();
      std::cout << "final current_offset: " << cf << '\n';
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

};
