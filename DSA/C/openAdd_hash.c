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
// Open Addressing
typedef enum EntryStatus {
    EMPTY, OCCUPIED, DELETED
} EntryStatus;

typedef struct HashEntry {
    int key;
    EntryStatus status;
} HashEntry;

typedef struct HashTable {
    HashEntry* entries;
    int size; // Total size of the hash table
    int numItems; // Number of occupied items
} HashTable;

int hashFunction(int key, int size) {
    return key % size;
}

int linearProbing(int key, int i, int size) {
    return (hashFunction(key, size) + i) % size;
}

void hashTableInsert(HashTable* table, int key) {
    if (((float)table->numItems / table->size) >= 0.7) {
        // Consider resizing the hash table here to maintain performance
        printf("Load factor exceeds 0.7, consider resizing the hash table.\n");
        return;
    }

    int i = 0, index, probeCount = 0;
    do {
        index = linearProbing(key, i, table->size);
 
        if (table->entries[index].status != OCCUPIED) {
            table->entries[index].key = key;
            table->entries[index].status = OCCUPIED;
            table->numItems++;
            return;
        }
        i++;
        probeCount++;
    } while (probeCount < table->size);

    printf("Failed to insert key: %d, table might be full.\n", key);
}


int hashTableSearch(HashTable* table, int key) {
    int i = 0;
    int index;
    do {
        index = linearProbing(key, i, table->size);
        if (table->entries[index].status == EMPTY) {
            return -1; // Key not found
        }
        if (table->entries[index].status == OCCUPIED && table->entries[index].key == key) {
            return index; // Key found
        }
        i++;
    } while (table->entries[index].status != EMPTY && i < table->size);

    return -1; // Key not found
}

void hashTableDelete(HashTable* table, int key) {
    int index = hashTableSearch(table, key);
    if (index != -1) { // If the key is found
        table->entries[index].status = DELETED;
        table->numItems--;
    }
}