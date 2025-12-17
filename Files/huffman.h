#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>

/*
  HUFFMAN - DSA LOGIC SUMMARY (highlighted):
  - Frequency counting: unordered_map<uint8_t, uint64_t>  (O(n) over file bytes)
  - Huffman tree: min-priority queue (min-heap) built over nodes (O(m log m), m = distinct symbols)
  - Tree traversal to generate codes: DFS (O(m * avg_code_len))
  - Time overall: O(n + m log m)
  - Memory: O(m) for map and tree; O(n) only when reading file into memory (we read once)
*/

namespace Huffman {

// Public API
bool compressBytes(const std::vector<uint8_t>& input, std::vector<uint8_t>& outBinary);
bool decompressBytes(const std::vector<uint8_t>& inBinary, std::vector<uint8_t>& outBytes);

} // namespace Huffman

#endif // HUFFMAN_H
