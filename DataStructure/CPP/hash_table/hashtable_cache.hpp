#ifndef CACHE_FRIENDLY_HASH_TABLE_HPP
#define CACHE_FRIENDLY_HASH_TABLE_HPP

#include <iostream>
#include <vector>
#include <optional>
#include <functional>

// This class implements a cache-friendly hash table using open addressing
// with linear probing. It's designed to be efficient in terms of cache usage
// by keeping the data compact and accessing memory sequentially.
template <typename KeyType, typename ValueType>
class CacheFriendlyHashTable
{
public:
    explicit CacheFriendlyHashTable(size_t size) : table(size), occupied(size, false) {}

    bool insert(const KeyType &key, const ValueType &value)
    {
        size_t index = hash(key) % table.size();
        for (size_t i = 0; i < table.size(); ++i)
        {
            size_t currentIndex = (index + i) % table.size();
            if (!occupied[currentIndex])
            {
                table[currentIndex] = std::make_pair(key, value);
                occupied[currentIndex] = true;
                return true;
            }
            if (occupied[currentIndex] && table[currentIndex].first == key)
            {
                table[currentIndex].second = value;
                return true;
            }
        }
        return false;
    }

    // std::optional<ValueType> search(const KeyType &key)
    // {
    //     size_t index = hash(key) % table.size();
    //     for (size_t i = 0; i < table.size(); ++i)
    //     {
    //         size_t currentIndex = (index + i) % table.size();
    //         if (occupied[currentIndex] && table[currentIndex].first == key)
    //         {
    //             return table[currentIndex].second;
    //         }
    //         if (!occupied[currentIndex])
    //         {
    //             break;
    //         }
    //     }
    //     return std::nullopt;
    // }

    std::optional<ValueType> search(const KeyType &key)
    {
        size_t index = hash(key) % table.size();
        for (size_t i = 0; i < table.size(); ++i)
        {
            size_t currentIndex = (index + i) % table.size();
            if (!occupied[currentIndex])
            {
                // If the slot is not occupied, we know the key is not present.
                return std::nullopt;
            }
            else if (table[currentIndex].first == key)
            {
                // Slot is occupied and key matches.
                return table[currentIndex].second;
            }
            // Continue probing if occupied but key does not match.
        }
        // Full table has been probed without finding the key.
        return std::nullopt;
    }

    bool remove(const KeyType &key)
    {
        size_t index = hash(key) % table.size();
        for (size_t i = 0; i < table.size(); ++i)
        {
            size_t currentIndex = (index + i) % table.size();
            if (occupied[currentIndex] && table[currentIndex].first == key)
            {
                occupied[currentIndex] = false;
                // Optionally, handle the removal in a way that does not break the probing sequence
                return true;
            }
            if (!occupied[currentIndex])
            {
                return false;
            }
        }
        return false;
    }

private:
    std::vector<std::pair<KeyType, ValueType>> table;
    std::vector<bool> occupied;
    std::hash<KeyType> hash;
};

#endif // CACHE_FRIENDLY_HASH_TABLE_HPP
