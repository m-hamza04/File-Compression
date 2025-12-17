#include "huffman.h"
#include <queue>
#include <memory>
#include <cstring>
#include <array>
#include <cassert>

// --- Helpers for binary read/write in memory ---
static void writeUint32(std::vector<uint8_t>& out, uint32_t v) {
    for (int i = 0; i < 4; ++i) out.push_back((v >> (i*8)) & 0xFF);
}
static uint32_t readUint32(const std::vector<uint8_t>& in, size_t& pos) {
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i) v |= uint32_t(in[pos++]) << (i*8);
    return v;
}
static void writeUint64(std::vector<uint8_t>& out, uint64_t v) {
    for (int i = 0; i < 8; ++i) out.push_back((v >> (i*8)) & 0xFF);
}
static uint64_t readUint64(const std::vector<uint8_t>& in, size_t& pos) {
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) v |= uint64_t(in[pos++]) << (i*8);
    return v;
}

// --- DSA: Huffman tree structures ---
struct Node {
    uint8_t byteVal;
    uint64_t freq;
    std::shared_ptr<Node> left, right;
    Node(uint8_t b, uint64_t f): byteVal(b), freq(f), left(nullptr), right(nullptr) {}
    Node(std::shared_ptr<Node> l, std::shared_ptr<Node> r): byteVal(0), freq(l->freq + r->freq), left(l), right(r) {}
};

struct Cmp {
    bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const {
        return a->freq > b->freq; // min-heap
    }
};

// Generate codes by DFS (DSA logic)
static void buildCodes(const std::shared_ptr<Node>& node, std::string& cur, std::array<std::string, 256>& codes) {
    if (!node) return;
    if (!node->left && !node->right) {
        // leaf
        codes[node->byteVal] = cur.empty() ? "0" : cur; // single-symbol file => code "0"
        return;
    }
    cur.push_back('0');
    buildCodes(node->left, cur, codes);
    cur.pop_back();
    cur.push_back('1');
    buildCodes(node->right, cur, codes);
    cur.pop_back();
}

// pack bits to bytes
class BitWriter {
    uint8_t cur = 0;
    int used = 0;
    std::vector<uint8_t>& out;
public:
    BitWriter(std::vector<uint8_t>& o): out(o) {}
    void writeBit(int b) {
        if (b) cur |= (1 << used);
        used++;
        if (used == 8) {
            out.push_back(cur);
            cur = 0; used = 0;
        }
    }
    void writeBitsFromString(const std::string& s) {
        for (char c : s) writeBit(c == '1');
    }
    void flush() {
        if (used) {
            out.push_back(cur);
            cur = 0; used = 0;
        }
    }
};

// read bits
class BitReader {
    const std::vector<uint8_t>& in;
    size_t pos;
    uint8_t cur;
    int used;
public:
    BitReader(const std::vector<uint8_t>& data, size_t startPos): in(data), pos(startPos), cur(0), used(8) {}
    // returns -1 on EOF
    int readBit() {
        if (used == 8) {
            if (pos >= in.size()) return -1;
            cur = in[pos++];
            used = 0;
        }
        int b = (cur >> used) & 1;
        used++;
        return b;
    }
    size_t bytePos() const { return pos; }
};

// Public API implementations
namespace Huffman {

bool compressBytes(const std::vector<uint8_t>& input, std::vector<uint8_t>& outBinary) {
    if (input.empty()) return false;

    // Frequency counting (DSA): O(n)
    std::array<uint64_t, 256> freq{};
    for (uint8_t b : input) freq[b]++;

    // Create nodes for bytes with non-zero freq
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, Cmp> pq;
    uint32_t distinct = 0;
    for (int i = 0; i < 256; ++i) {
        if (freq[i]) {
            pq.push(std::make_shared<Node>(static_cast<uint8_t>(i), freq[i]));
            distinct++;
        }
    }

    // Edge: if only one distinct symbol, still create tree
    if (pq.size() == 1) {
        auto only = pq.top(); pq.pop();
        // create a dummy sibling so tree traversal yields a code
        auto dummy = std::make_shared<Node>(0, 0);
        pq.push(std::make_shared<Node>(only, dummy));
    }

    // Build Huffman tree (DSA: min-heap merging) O(m log m)
    while (pq.size() > 1) {
        auto a = pq.top(); pq.pop();
        auto b = pq.top(); pq.pop();
        pq.push(std::make_shared<Node>(a, b));
    }
    auto root = pq.top();

    // Build codes (DSA: DFS)
    std::array<std::string, 256> codes;
    std::string cur;
    buildCodes(root, cur, codes);

    // Output header:
    // magic "HUF1" (4 bytes), original size (8 bytes), distinct count (4 bytes)
    outBinary.clear();
    outBinary.reserve(input.size()/2);
    outBinary.push_back('H'); outBinary.push_back('U'); outBinary.push_back('F'); outBinary.push_back('1');
    writeUint64(outBinary, (uint64_t)input.size());
    writeUint32(outBinary, distinct);

    // For each distinct byte: 1 byte value + 8 bytes frequency
    for (int i = 0; i < 256; ++i) {
        if (freq[i]) {
            outBinary.push_back((uint8_t)i);
            writeUint64(outBinary, freq[i]);
        }
    }

    // Now write the bitstream using codes
    BitWriter bw(outBinary);
    for (uint8_t b : input) {
        bw.writeBitsFromString(codes[b]);
    }
    bw.flush();
    return true;
}

bool decompressBytes(const std::vector<uint8_t>& inBinary, std::vector<uint8_t>& outBytes) {
    size_t pos = 0;
    if (inBinary.size() < 4) return false;
    if (inBinary[pos] != 'H' || inBinary[pos+1] != 'U' || inBinary[pos+2] != 'F' || inBinary[pos+3] != '1') return false;
    pos += 4;
    uint64_t originalSize = readUint64(inBinary, pos);
    uint32_t distinct = readUint32(inBinary, pos);

    std::array<uint64_t, 256> freq{};
    for (uint32_t i = 0; i < distinct; ++i) {
        if (pos + 1 + 8 > inBinary.size()) return false;
        uint8_t val = inBinary[pos++];
        uint64_t f = readUint64(inBinary, pos);
        freq[val] = f;
    }

    // Reconstruct tree as in compression: create pq of nodes
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, Cmp> pq;
    for (int i = 0; i < 256; ++i) if (freq[i]) pq.push(std::make_shared<Node>(static_cast<uint8_t>(i), freq[i]));

    if (pq.empty()) return false;
    if (pq.size() == 1) {
        auto only = pq.top(); pq.pop();
        auto dummy = std::make_shared<Node>(0, 0);
        pq.push(std::make_shared<Node>(only, dummy));
    }
    while (pq.size() > 1) {
        auto a = pq.top(); pq.pop();
        auto b = pq.top(); pq.pop();
        pq.push(std::make_shared<Node>(a, b));
    }
    auto root = pq.top();

    // Now read bits and traverse tree to produce bytes
    BitReader br(inBinary, pos);
    outBytes.clear();
    outBytes.reserve((size_t)originalSize);

    std::shared_ptr<Node> cur = root;
    while (outBytes.size() < originalSize) {
        int bit = br.readBit();
        if (bit < 0) return false; // incomplete
        cur = (bit == 0) ? cur->left : cur->right;
        if (!cur->left && !cur->right) {
            // leaf
            outBytes.push_back(cur->byteVal);
            cur = root;
        }
    }
    return true;
}

} // namespace Huffman
