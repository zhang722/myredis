## Redis数据类型
[](figures/redis-data.png)
redis的五种数据类型，其底层根据数据量的不同，可能采用不同实现。为了简单：
1. 每种类型不考虑数据量大小，仅仅有一种实现
2. 可以用STL容器的，尽量用容器
   
每种类型的底层实现如下：
* string-> std::string
* list  -> std::list
* hash  -> std::unordered_map
* set   -> Intset,std::unordered_map
* zset  -> Skiplist

因此，只需要实现Intset和Skiplist两个数据类型；而对于hash，由于redis中实现了渐进hash，因此我们需要手动实现这一部分

* Hash
* Intset
* Skiplist
