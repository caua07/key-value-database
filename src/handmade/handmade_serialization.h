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

template<typename T>
inline size_t align_for(size_t value) {
    return align_up(value, alignof(T));
}

size_t 
serialize_str(size_t current_offset, const std::string& str)
{
  current_offset = align_for<size_t>(current_offset);
  current_offset += str.size();
}
