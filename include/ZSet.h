#pragma once

#include <unordered_map>

#include "Skiplist.h"
#include "RedisObject.h"

namespace myredis
{

class ZSet
{
public:
    ZSet() {}

    bool insert(RedisObjectPtr key, float val);
    bool remove(RedisObjectPtr key);
    bool isInRange(float l, float r);
    RedisObjectPtr firstInRange(float l, float r);
    RedisObjectPtr lastInRange(float l, float r);
private:
    Skiplist<RedisObjectPtr, float> l_;
    std::unordered_map<RedisObjectPtr, float> m_;
};

} // namespace myredis