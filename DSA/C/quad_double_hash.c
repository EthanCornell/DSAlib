/*
 * Copyright (c) Cornell University.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: I-Hsuan (Ethan) Huang
 * Email: ih246@cornell.edu
 */
//  Quadratic Probing and Double Hashing
typedef struct HashTable {
    HashEntry* entries;
    int size;
    int numItems; // Track the number of occupied items
} HashTable;

typedef enum EntryStatus {
    EMPTY, OCCUPIED, DELETED
} EntryStatus;

typedef struct HashEntry {
    int key;
    EntryStatus status;
} HashEntry;


#define LINEAR_PROBING 0
#define QUADRATIC_PROBING 1
#define DOUBLE_HASHING 2


int hashFunction(int key, int size) {
    return key % size;
}

int linearProbing(int key, int i, int size) {
    return (hashFunction(key, size) + i) % size;
}

// Quadratic Probing
int quadraticProbing(int key, int i, int size, int c1, int c2) {
    return (hashFunction(key, size) + c1 * i + c2 * i * i) % size;
}

// Double Hashing
int hashFunction2(int key, int size) {
    // Using a prime number less than table size as a simple example
    int prime = size - 1; // Ensure this is prime and less than table size
    return prime - (key % prime);
}

int doubleHashing(int key, int i, int size) {
    int hash1 = hashFunction(key, size);
    int hash2 = hashFunction2(key, size);
    return (hash1 + i * hash2) % size;
}


void hashTableInsert(HashTable* table, int key, int method) {
    if (table->numItems == table->size) {
        // Table is full
        return;
    }

    int i = 0, index;
    do {
        // Select probing method
        if (method == QUADRATIC_PROBING) {
            index = quadraticProbing(key, i, table->size, 1, 3); // Example constants
        } else if (method == DOUBLE_HASHING) {
            index = doubleHashing(key, i, table->size);
        } else {
            // Default to linear if method is unknown
            index = linearProbing(key, i, table->size);
        }

        if (table->entries[index].status != OCCUPIED) {
            table->entries[index].key = key;
            table->entries[index].status = OCCUPIED;
            table->numItems++;
            return;
        }
        i++;
    } while (i < table->size);
}


int search(HashTable* table, int key, int (*probingFunction)(int, int, int), int c1, int c2) {
    for (int i = 0; i < table->size; i++) {
        int index = probingFunction(key, i, table->size, c1, c2);
        if (table->entries[index].status == EMPTY) {
            return -1; // Key not found
        }
        if (table->entries[index].status == OCCUPIED && table->entries[index].key == key) {
            return index; // Key found
        }
        // Note: If status is DELETED, we keep probing
    }
    return -1; // Key not found after full table scan
}

void delete(HashTable* table, int key, int (*probingFunction)(int, int, int), int c1, int c2) {
    for (int i = 0; i < table->size; i++) {
        int index = probingFunction(key, i, table->size, c1, c2);
        if (table->entries[index].status == EMPTY) {
            return; // Key not found, end search
        }
        if (table->entries[index].status == OCCUPIED && table->entries[index].key == key) {
            table->entries[index].status = DELETED; // Mark as deleted
            return;
        }
        // Note: If status is DELETED, we keep probing
    }
}
