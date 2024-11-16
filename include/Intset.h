#pragma once

// #include <cstdint>

namespace myredis {

class Intset {
public:
    Intset();
    ~Intset();

    bool insert(int64_t value);
    bool remove(int64_t value);
    bool exists(int64_t value) const;
    void print() const;

private:
    enum class Encoding { INT16, INT32, INT64 };
    
    Encoding encoding;
    size_t size;     // 当前元素数量
    size_t capacity; // 当前容量
    void* data;      // 存储元素的连续内存块

    bool canHold(int64_t value) const;
    size_t findInsertPosition(int64_t value) const;
    void insertAtPosition(int64_t value, size_t pos);
    void removeAtPosition(size_t pos);
    void ensureCapacity();
    void upgradeType(int64_t value);

    size_t elementSize() const;
    size_t elementSize(Encoding enc) const;
};

} // namespace myredis