#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "Intset.h"

namespace myredis {

Intset::Intset() : encoding(Encoding::INT16), size(0), capacity(4) {
    data = malloc(capacity * sizeof(int16_t));
}

Intset::~Intset() {
    free(data);
}

bool Intset::insert(int64_t value) {
    if (!canHold(value)) {
        upgradeType(value);
    }

    if (exists(value)) return false;

    size_t pos = findInsertPosition(value);
    insertAtPosition(value, pos);
    size++;
    return true;
}

bool Intset::remove(int64_t value) {
    if (!exists(value)) return false;

    size_t pos = findInsertPosition(value);
    removeAtPosition(pos);
    size--;
    return true;
}

bool Intset::exists(int64_t value) const {
    if (size == 0) return false;
    switch (encoding) {
        case Encoding::INT16:
            return std::binary_search(static_cast<int16_t*>(data),
                                      static_cast<int16_t*>(data) + size,
                                      static_cast<int16_t>(value));
        case Encoding::INT32:
            return std::binary_search(static_cast<int32_t*>(data),
                                      static_cast<int32_t*>(data) + size,
                                      static_cast<int32_t>(value));
        case Encoding::INT64:
            return std::binary_search(static_cast<int64_t*>(data),
                                      static_cast<int64_t*>(data) + size,
                                      value);
    }
    return false;
}

void Intset::print() const {
    std::cout << "Intset (encoding: ";
    switch (encoding) {
        case Encoding::INT16: std::cout << "INT16"; break;
        case Encoding::INT32: std::cout << "INT32"; break;
        case Encoding::INT64: std::cout << "INT64"; break;
    }
    std::cout << ", size: " << size << "): ";
    for (size_t i = 0; i < size; ++i) {
        switch (encoding) {
            case Encoding::INT16:
                std::cout << static_cast<int16_t*>(data)[i] << " ";
                break;
            case Encoding::INT32:
                std::cout << static_cast<int32_t*>(data)[i] << " ";
                break;
            case Encoding::INT64:
                std::cout << static_cast<int64_t*>(data)[i] << " ";
                break;
        }
    }
    std::cout << std::endl;
}

bool Intset::canHold(int64_t value) const {
    switch (encoding) {
        case Encoding::INT16: return value >= INT16_MIN && value <= INT16_MAX;
        case Encoding::INT32: return value >= INT32_MIN && value <= INT32_MAX;
        case Encoding::INT64: return true;
    }
    return false;
}

size_t Intset::findInsertPosition(int64_t value) const {
    switch (encoding) {
        case Encoding::INT16:
            return std::lower_bound(static_cast<int16_t*>(data), static_cast<int16_t*>(data) + size,
                                    static_cast<int16_t>(value)) - static_cast<int16_t*>(data);
        case Encoding::INT32:
            return std::lower_bound(static_cast<int32_t*>(data), static_cast<int32_t*>(data) + size,
                                    static_cast<int32_t>(value)) - static_cast<int32_t*>(data);
        case Encoding::INT64:
            return std::lower_bound(static_cast<int64_t*>(data), static_cast<int64_t*>(data) + size,
                                    value) - static_cast<int64_t*>(data);
    }
    return 0;
}

void Intset::insertAtPosition(int64_t value, size_t pos) {
    ensureCapacity();
    if (encoding == Encoding::INT16) {
        int16_t* ptr = static_cast<int16_t*>(data);
        std::move_backward(ptr + pos, ptr + size, ptr + size + 1);
        ptr[pos] = static_cast<int16_t>(value);
    } else if (encoding == Encoding::INT32) {
        int32_t* ptr = static_cast<int32_t*>(data);
        std::move_backward(ptr + pos, ptr + size, ptr + size + 1);
        ptr[pos] = static_cast<int32_t>(value);
    } else {
        int64_t* ptr = static_cast<int64_t*>(data);
        std::move_backward(ptr + pos, ptr + size, ptr + size + 1);
        ptr[pos] = value;
    }
}

void Intset::removeAtPosition(size_t pos) {
    if (encoding == Encoding::INT16) {
        int16_t* ptr = static_cast<int16_t*>(data);
        std::move(ptr + pos + 1, ptr + size, ptr + pos);
    } else if (encoding == Encoding::INT32) {
        int32_t* ptr = static_cast<int32_t*>(data);
        std::move(ptr + pos + 1, ptr + size, ptr + pos);
    } else {
        int64_t* ptr = static_cast<int64_t*>(data);
        std::move(ptr + pos + 1, ptr + size, ptr + pos);
    }
}

void Intset::ensureCapacity() {
    if (size >= capacity) {
        capacity *= 2;
        data = realloc(data, capacity * elementSize());
    }
}

void Intset::upgradeType(int64_t value) {
    Encoding newEncoding;
    if (encoding == Encoding::INT16 && (value < INT16_MIN || value > INT16_MAX)) {
        newEncoding = Encoding::INT32;
    } else if (encoding == Encoding::INT32 && (value < INT32_MIN || value > INT32_MAX)) {
        newEncoding = Encoding::INT64;
    } else {
        return;
    }

    void* newData = malloc(capacity * elementSize(newEncoding));

    if (encoding == Encoding::INT16) {
        int16_t* oldData = static_cast<int16_t*>(data);
        if (newEncoding == Encoding::INT32) {
            int32_t* newData32 = static_cast<int32_t*>(newData);
            for (size_t i = 0; i < size; ++i) newData32[i] = oldData[i];
        } else {
            int64_t* newData64 = static_cast<int64_t*>(newData);
            for (size_t i = 0; i < size; ++i) newData64[i] = oldData[i];
        }
    } else if (encoding == Encoding::INT32) {
        int32_t* oldData = static_cast<int32_t*>(data);
        int64_t* newData64 = static_cast<int64_t*>(newData);
        for (size_t i = 0; i < size; ++i) newData64[i] = oldData[i];
    }

    free(data);
    data = newData;
    encoding = newEncoding;
}

size_t Intset::elementSize() const {
    return elementSize(encoding);
}

size_t Intset::elementSize(Encoding enc) const {
    switch (enc) {
        case Encoding::INT16: return sizeof(int16_t);
        case Encoding::INT32: return sizeof(int32_t);
        case Encoding::INT64: return sizeof(int64_t);
    }
    return sizeof(int64_t);
}

} // namespace myredis