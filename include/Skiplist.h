#pragma once

#include <vector>
#include <algorithm>
#include <climits>
#include <unordered_map>

namespace myredis
{

const int MAX_SKIPLIST_LEVEL = 32;

template <typename K, typename V>
class SkiplistNode {
public:
    using Node = SkiplistNode<K, V>;
    SkiplistNode(const K& key = K(), const V& val = V(), int level = MAX_SKIPLIST_LEVEL) : key_(key), val_(val), forward_(level, nullptr) {}

    K key_;
    V val_;
    std::vector<Node*> forward_;
};

template <typename K, typename V>
class Skiplist
{
public:
    using Node = SkiplistNode<K, V>;
    Skiplist(int maxLevel = MAX_SKIPLIST_LEVEL);
    ~Skiplist();

    bool insert(const K& key, const V& val);
    bool remove(const K& key, const V& val);
    bool isInRange(const V& l, const V& r);
    Node* firstInRange(const V& l, const V& r);
    Node* lastInRange(const V& l, const V& r);
    // bool removeRangeByScore(const T& target);
    // bool removeRangeByRank(int l, int r);

    int size() { return size_; }
    bool empty() { return size_ == 0; }
    void print();

private:
    // std::vector<Node*> getPres(const V& val);
    std::vector<Node*> getPres(const K& key, const V& val);
    int randomLevel();

    Node *head_;
    Node *tail_;
    int size_;
    int maxLevel_;

};

template<typename K, typename V>
inline Skiplist<K, V>::Skiplist(int maxLevel) {
    maxLevel_ = std::min(maxLevel, MAX_SKIPLIST_LEVEL);
    head_ = new Node(maxLevel_);
    size_ = 0;
}

template<typename K, typename V>
inline Skiplist<K, V>::~Skiplist() {
    Node *cur = head_;
    while (cur) {
        Node *next = cur->forward_[0];
        delete cur;
        cur = next;
    }
}

template<typename K, typename V>
inline bool Skiplist<K, V>::insert(const K & key, const V & val) {
    std::vector<Node*> pre = getPres(key, val);

    int level = randomLevel();
    Node *toInsert = new Node(key, val, level);
    for (int i = 0; i < level; ++i) {
        Node *next = pre[i]->forward_[i];
        pre[i]->forward_[i] = toInsert;
        toInsert->forward_[i] = next;
    }
    size_++;
    return true;
}

template<typename K, typename V>
inline bool Skiplist<K, V>::remove(const K & key, const V & val) {
    std::vector<Node*> pre = getPres(key, val);

    if (pre[0]->forward_[0] && (pre[0]->forward_[0]->key_ != key || pre[0]->forward_[0]->val_ != val)) {
        return false;
    }

    Node *toRemove = pre[0]->forward_[0];
    for (int i = 0; i < maxLevel_ && pre[i]->forward_[i]; ++i) {
        pre[i]->forward_[i] = pre[i]->forward_[i]->forward_[i];
    }
    size_--;
    delete toRemove;
    return true;
}

template<typename K, typename V>
inline bool Skiplist<K, V>::isInRange(const V & l, const V & r) {
    Node *cur = head_->forward_[0];
    while (cur && cur->val_ < l) {
        cur = cur->forward_[0];
    }
    return cur && cur->val_ <= r;
}

template<typename K, typename V>
inline typename Skiplist<K, V>::Node * Skiplist<K, V>::firstInRange(const V & l, const V & r) {
    Node *cur = head_->forward_[0];
    while (cur && cur->val_ < l) {
        cur = cur->forward_[0];
    }
    if (!cur || cur->val_ > r) {
        return nullptr;
    }
    return cur;
}

template<typename K, typename V>
inline typename Skiplist<K, V>::Node * Skiplist<K, V>::lastInRange(const V & l, const V & r) {
    Node *cur = head_;
    for (int i = maxLevel_ - 1; i >= 0; --i) {
        while (cur->forward_[i] && cur->forward_[i]->val_ <= r) {
            cur = cur->forward_[i];
        }
    } 

    return (cur != head_ && cur->val_ >= l) ? cur : nullptr;
}


template<typename K, typename V>
inline void Skiplist<K, V>::print() {
    Node *cur = head_->forward_[0];
    std::cout << "Skiplist: ";
    while (cur) {
        std::cout << cur->val_ << ',';
        cur = cur->forward_[0];
    }
    std::cout << std::endl;
}

// template<typename K, typename V>
// inline std::vector<SkiplistNode<K, V>*> Skiplist<K, V>::getPres(const V & val) {
//     std::vector<Node*> pre(maxLevel_, nullptr);

//     Node *cur = head_;
//     for (int i = maxLevel_ - 1; i >= 0; --i) {
//         while (cur->forward_[i] && cur->forward_[i]->val_ < val) {
//             cur = cur->forward_[i];
//         }
//         pre[i] = cur;
//     }
//     return pre;
// }

template<typename K, typename V>
inline std::vector<typename  Skiplist<K, V>::Node*> Skiplist<K, V>::getPres(const K & key, const V & val) {
    std::vector<Node*> pre(maxLevel_, nullptr);

    Node *cur = head_;
    for (int i = maxLevel_ - 1; i >= 0; --i) {
        while (cur->forward_[i] && (cur->forward_[i]->val_ < val ||
            (cur->forward_[i]->val_ == val && cur->forward_[i]->key_ < key))) {
            cur = cur->forward_[i];
        }
        pre[i] = cur;
    }
    return pre;
}

template<typename K, typename V>
inline int Skiplist<K, V>::randomLevel() {
    int level = 1;
    while (rand() % 2 && level < maxLevel_) {
        level++;
    }
    return level;
}

} // namespace myredis