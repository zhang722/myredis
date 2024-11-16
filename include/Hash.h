#pragma once

#include <iostream>
#include <unordered_map>
#include <memory>
#include <optional>

namespace myredis
{

template <typename K, typename V>
class Hash {
public:
    Hash(size_t initialCapacity = 8)
        : migrating_(false), migrationIndex_(0) {
        old_map_ = std::make_unique<std::unordered_map<K, V>>();
        old_map_->reserve(initialCapacity);
    }

    // 插入或更新元素
    void insert(const K& key, const V& value) {
        if (migrating_) {
            migrateStep();
        }

        // 插入或更新数据
        if (new_map_) {
            (*new_map_)[key] = value;
        } else {
            (*old_map_)[key] = value;

            // 检查是否需要启动迁移
            if (old_map_->size() > old_map_->bucket_count() * maxLoadFactor_) {
                startMigration();
            }
        }
    }

    // 查找元素
    std::optional<V> find(const K& key) {
        if (migrating_) {
            migrateStep();
        }

        // 先查找新表，再查找旧表
        if (new_map_ && new_map_->count(key)) {
            return (*new_map_)[key];
        } else if (old_map_ && old_map_->count(key)) {
            return (*old_map_)[key];
        }
        return std::nullopt; // 未找到
    }

    // 获取当前 ProgressiveHashMap 的元素总数
    size_t size() const {
        size_t total_size = old_map_ ? old_map_->size() : 0;
        total_size += new_map_ ? new_map_->size() : 0;
        return total_size;
    }

    // 重载 [] 操作符
    V& operator[](const K& key) {
        if (migrating_) {
            migrateStep();
        }

        if (new_map_) {
            return (*new_map_)[key];  // 如果新表存在，优先插入到新表
        } else if (old_map_->count(key)) {
            startMigration();  // 如果旧表中存在，启动迁移
            (*new_map_)[key] = std::move((*old_map_)[key]);
            old_map_->erase(key);  // 将元素从旧表迁移到新表
            return (*new_map_)[key];
        } else {
            return (*old_map_)[key];  // 若 key 不存在，插入到 old_map_
        }
    }

private:
    // 触发迁移过程
    void startMigration() {
        new_map_ = std::make_unique<std::unordered_map<K, V>>();
        new_map_->reserve(old_map_->size() * 2); // 扩展为旧表的两倍
        migrating_ = true;
        migrationIndex_ = 0;
    }

    // 渐进式迁移的一步
    void migrateStep() {
        if (!migrating_ || !old_map_) return;

        // 一次迁移一个桶
        if (migrationIndex_ < old_map_->bucket_count()) {
            for (auto it = old_map_->begin(); it != old_map_->end(); ) {
                (*new_map_)[it->first] = it->second;
                it = old_map_->erase(it); // 从旧表中删除迁移过的元素
            }
            migrationIndex_++;
        } else {
            // 迁移完成，切换 old_map 和 new_map
            old_map_ = std::move(new_map_);
            migrating_ = false;
        }
    }

    std::unique_ptr<std::unordered_map<K, V>> old_map_; // 旧哈希表
    std::unique_ptr<std::unordered_map<K, V>> new_map_; // 新哈希表
    bool migrating_;              // 是否正在迁移
    size_t migrationIndex_;       // 当前迁移的桶索引
    const double maxLoadFactor_ = 0.75; // 最大负载因子
};

} // namespace myredis
