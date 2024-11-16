#pragma 

namespace myredis
{

enum Type {
    STRING,
    LIST,
    HASH,
    SET,
    ZSET,
};

enum Encoding {
    INT,
    STRING,
    HT,
    LINKEDLIST,
    INTSET,
    SKIPLIST, 
};
    
} // namespace myredis
