#pragma once

#include <memory>

#include <Type.h>

namespace myredis
{

const int TYPE_BITS = 4;
const int ENCODING_BITS = 4;
const int LRU_BITS = 24;

class RedisObject
{
public:
    RedisObject(int type, void *ptr);
    ~RedisObject();

private:
    void freeString();
    void freeList();
    void freeHash();
    void freeSet();
    void freeZSet();
    unsigned type_:TYPE_BITS;
    unsigned encoding_:ENCODING_BITS;
    unsigned lru_:LRU_BITS;
    void *ptr_;
};

using RedisObjectPtr = std::shared_ptr<RedisObject>;
    
} // namespace myredis

