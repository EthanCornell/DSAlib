#include <iostream>
#include <vector>
#include <optional>
#include <functional>

//  cache-friendliness in a hash table
// A common approach to achieving cache-friendliness in a hash table implementation is to use open addressing for collision resolution and to keep the data compact.
// This example will focus on a simple fixed-size hash table using linear probing for collision resolution,
// which is known for its cache-friendly characteristics due to its sequential memory access pattern.

template <typename KeyType, typename ValueType>
class CacheFriendlyHashTable
{
public:
    CacheFriendlyHashTable(size_t size) : table(size), occupied(size, false) {}

    bool insert(const KeyType &key, const ValueType &value)
    {
        size_t index = hash(key) % table.size();
        for (size_t i = 0; i < table.size(); ++i)
        {
            // Linear probing to find the next available slot
            size_t currentIndex = (index + i) % table.size();
            if (!occupied[currentIndex])
            {
                table[currentIndex] = std::make_pair(key, value);
                occupied[currentIndex] = true;
                return true;
            }
            // If key already exists, update the value
            if (occupied[currentIndex] && table[currentIndex].first == key)
            {
                table[currentIndex].second = value;
                return true;
            }
        }
        // Table is full or cannot find a slot after probing the entire table
        return false;
    }

    std::optional<ValueType> search(const KeyType &key)
    {
        size_t index = hash(key) % table.size();
        for (size_t i = 0; i < table.size(); ++i)
        {
            size_t currentIndex = (index + i) % table.size();
            if (occupied[currentIndex] && table[currentIndex].first == key)
            {
                return table[currentIndex].second;
            }
            if (!occupied[currentIndex])
            {
                // Stop searching if an empty slot is found
                return std::nullopt;
            }
        }
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

int main()
{
    CacheFriendlyHashTable<int, std::string> hashtable(10);

    hashtable.insert(1, "Value for 1");
    hashtable.insert(2, "Value for 2");
    hashtable.insert(11, "Value for 11"); // This will cause a collision with key 1

    auto value = hashtable.search(11);
    if (value.has_value())
    {
        std::cout << "Found value: " << value.value() << std::endl;
    }
    else
    {
        std::cout << "Value not found" << std::endl;
    }

    return 0;
}

// g++ -pg -fsanitize=address -g -std=c++17 ./hashtable_cache.cpp -o ht_c -O3  && ./ht_c