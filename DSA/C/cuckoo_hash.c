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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TABLE_SIZE 11 // Prime number for better distribution
#define MAX_LOOPS  50 // Max loops to avoid infinite loop

// Hash functions
int hash1(int key) {
    return key % TABLE_SIZE;
}

int hash2(int key) {
    return (key / TABLE_SIZE) % TABLE_SIZE;
}

typedef struct {
    int* table1;
    int* table2;
} CuckooHashTable;

// Initialize the hash table
CuckooHashTable* createHashTable() {
    CuckooHashTable* ht = (CuckooHashTable*) malloc(sizeof(CuckooHashTable));
    ht->table1 = (int*) calloc(TABLE_SIZE, sizeof(int));
    ht->table2 = (int*) calloc(TABLE_SIZE, sizeof(int));
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht->table1[i] = -1; // -1 indicates empty slot
        ht->table2[i] = -1;
    }
    return ht;
}

// Cuckoo Hash insert function
bool cuckooInsert(CuckooHashTable* ht, int key) {
    if (key == -1) {
        return false; // -1 is used to indicate empty slot, cannot insert -1
    }
    int loopCounter = 0;
    int pos1 = hash1(key), pos2 = hash2(key);
    int tempKey = key, temp;

    while (loopCounter++ < MAX_LOOPS) {
        // Try to place in table 1
        if (ht->table1[pos1] == -1) {
            ht->table1[pos1] = tempKey;
            return true;
        }

        // Swap with existing element and try to place it in table 2
        temp = ht->table1[pos1];
        ht->table1[pos1] = tempKey;
        tempKey = temp;

        pos2 = hash2(tempKey);

        if (ht->table2[pos2] == -1) {
            ht->table2[pos2] = tempKey;
            return true;
        }

        // Swap with existing element and try to place it in table 1
        temp = ht->table2[pos2];
        ht->table2[pos2] = tempKey;
        tempKey = temp;

        pos1 = hash1(tempKey);
    }

    // If loopCounter exceeds MAX_LOOPS, rehashing is needed (not implemented here)
    printf("Rehashing needed\n");
    return false;
}

// Search in Cuckoo Hash Table
bool cuckooSearch(CuckooHashTable* ht, int key) {
    int pos1 = hash1(key);
    int pos2 = hash2(key);

    if (ht->table1[pos1] == key || ht->table2[pos2] == key) {
        return true;
    }
    return false;
}


// Cuckoo Hash delete function
bool cuckooDelete(CuckooHashTable* ht, int key) {
    int pos1 = hash1(key);
    int pos2 = hash2(key);

    // Check if the key is in table 1
    if (ht->table1[pos1] == key) {
        ht->table1[pos1] = -1; // Mark the slot as empty
        return true;
    }

    // Check if the key is in table 2
    if (ht->table2[pos2] == key) {
        ht->table2[pos2] = -1; // Mark the slot as empty
        return true;
    }

    // Key not found
    return false;
}


// Main function to test Cuckoo Hashing
int main() {
    CuckooHashTable* ht = createHashTable();

    cuckooInsert(ht, 20);
    cuckooInsert(ht, 50);
    cuckooInsert(ht, 53); // This may cause displacement

    printf("Deleting 50: %s\n", cuckooDelete(ht, 50) ? "Success" : "Failed");
    printf("Searching for 50 after deletion: %s\n", cuckooSearch(ht, 50) ? "Found" : "Not Found");

    // Free allocated memory and cleanup (not shown)

    return 0;
}

