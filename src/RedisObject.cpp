#include <list>
#include <unordered_map>

#include "RedisObject.h"
#include "ZSet.h"
#include "Intset.h"

#include "Logger.h"

namespace myredis
{

RedisObject::RedisObject(int type, void * ptr)
  : type_(type),
    ptr_(ptr) 
{
    encoding_ = Encoding::STRING;
}


RedisObject::~RedisObject() {
    switch (type_)
    {
    case Type::STRING:
        freeString();
        break;
    case Type::LIST:
        freeList();
        break;
    case Type::HASH:
        freeHash();
        break;
    case Type::SET:
        freeSet();
        break;
    case Type::ZSET:
        freeZSet();
        break;
    
    default:
        break;
    }
}


void RedisObject::freeString() {
    delete static_cast<std::string*>(ptr_);
}

void RedisObject::freeList() {
    delete static_cast<std::list<std::string>*>(ptr_);
}

void RedisObject::freeHash() {
    delete static_cast<std::unordered_map<std::string, std::string>*>(ptr_);
}

void RedisObject::freeSet() {
    switch (encoding_)
    {
    case Encoding::INTSET:
        delete static_cast<Intset*>(ptr_);
        break;
    case Encoding::HT:
        delete static_cast<std::unordered_map<std::string, std::string>*>(ptr_);
        break;
    
    default:
        LOG_FATAL("Unknown Set encoding.");
        break;
    }
}

void RedisObject::freeZSet() {
    delete static_cast<ZSet*>(ptr_);
}


} // namespace myredis