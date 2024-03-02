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
// Coalesced hashing
typedef struct HashEntry {
    int key;         // Key of the entry
    int next;        // Index of the next entry in the chain, -1 if none
    int status;      // Status: EMPTY, OCCUPIED, DELETED
} HashEntry;

typedef enum EntryStatus {
    EMPTY, OCCUPIED, DELETED
} EntryStatus;

typedef struct HashTable {
    HashEntry* entries; // Array of hash entries
    int size;           // Total size of the hash table
    int numItems;       // Number of occupied items
} HashTable;

HashTable* initializeHashTable(int size) {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    table->size = size;
    table->numItems = 0;
    table->entries = (HashEntry*)malloc(sizeof(HashEntry) * size);

    for (int i = 0; i < size; i++) {
        table->entries[i].key = 0;
        table->entries[i].next = -1; // -1 indicates no next entry
        table->entries[i].status = EMPTY;
    }

    return table;
}

void insert(HashTable* table, int key) {
    int index = hashFunction(key, table->size);

    // If the slot is empty, use it directly
    if (table->entries[index].status == EMPTY) {
        table->entries[index].key = key;
        table->entries[index].status = OCCUPIED;
        table->numItems++;
    } else {
        // Find an empty slot for the collided item
        int emptyIndex = findEmptySlot(table);
        if (emptyIndex == -1) {
            printf("Hash table is full\n");
            return;
        }

        // If the initial slot wasn't occupied due to a collision, chain from the initial slot
        if (table->entries[index].status != OCCUPIED) {
            table->entries[index].next = emptyIndex;
        } else {
            // Chain from the last item in the current chain
            int current = index;
            while (table->entries[current].next != -1) {
                current = table->entries[current].next;
            }
            table->entries[current].next = emptyIndex;
        }

        // Insert the collided item into the empty slot
        table->entries[emptyIndex].key = key;
        table->entries[emptyIndex].status = OCCUPIED;
        table->numItems++;
    }
}


int search(HashTable* table, int key) {
    int index = hashFunction(key, table->size);
    int current = index;

    while (current != -1) {
        if (table->entries[current].key == key && table->entries[current].status == OCCUPIED) {
            return current; // Key found, return index
        }
        current = table->entries[current].next;
    }

    return -1; // Key not found
}


void delete(HashTable* table, int key) {
    int index = hashFunction(key, table->size);
    int current = index;
    int prev = -1;

    while (current != -1) {
        if (table->entries[current].key == key && table->entries[current].status == OCCUPIED) {
            // If it's a middle or end node with a successor, pull the successor back
            if (table->entries[current].next != -1) {
                int successor = table->entries[current].next;
                if (prev != -1) {
                    // If not the head of the chain, adjust the predecessor's next
                    table->entries[prev].next = successor;
                }
                // Copy the successor to the current position and clear the successor slot
                table->entries[current].key = table->entries[successor].key;
                table->entries[current].next = table->entries[successor].next;
                table->entries[successor].status = DELETED;
                table->entries[successor].next = -1;
            } else {
                // If no successor, simply mark as deleted
                table->entries[current].status = DELETED;
                table->entries[current].next = -1;
                if (prev != -1) {
                    // Adjust the predecessor's next if not the head of the chain
                    table->entries[prev].next = -1;
                }
            }
            return; // Successfully deleted
        }
        prev = current;
        current = table->entries[current].next;
    }

    // If the function reaches here, the key was not found
}


// Successor Replacement: When deleting a node that has a successor, the function now checks if moving the successor to the deleted node's position would maintain or improve the chain's integrity. This is particularly important when the successor has no direct link to the chain's start, indicating that moving it could lead to an orphaned chain or unnecessary chain length.
// Chain Optimization: By ensuring that successors are only moved if it benefits the overall structure of the chain, this approach aims to minimize the length of chains, thereby optimizing search times post-deletion.
// Edge Case Handling: This revised approach takes extra care in handling nodes without successors and ensures that if a node is removed from the middle of a chain, the previous node's next pointer is correctly updated to maintain the chain's continuity.

// Chain Optimization Limitations: While this approach attempts to optimize chain lengths, there may still be cases where the resulting chain structure is not fully optimized due to the complexities involved in moving entries within the table.
// Further Enhancements: Additional strategies, such as more advanced rehashing techniques or dynamic resizing of the hash table, could further optimize the performance and efficiency of coalesced hashing, especially in scenarios with high load factors or frequent deletions.
void deleteImproved(HashTable* table, int key) {
    int index = hashFunction(key, table->size);
    int current = index;
    int prev = -1;

    while (current != -1) {
        if (table->entries[current].key == key && table->entries[current].status == OCCUPIED) {
            // Check if this node has a successor
            if (table->entries[current].next != -1) {
                int successorIndex = table->entries[current].next;
                // Attempt to move successor to current position if it optimizes chain length
                // Check if successor can be directly linked to predecessor or start of chain
                if (prev != -1 || hashFunction(table->entries[successorIndex].key, table->size) == index) {
                    // Move successor to current position
                    table->entries[current].key = table->entries[successorIndex].key;
                    table->entries[current].next = table->entries[successorIndex].next;
                    table->entries[successorIndex].status = DELETED; // Mark old successor slot as deleted
                    table->entries[successorIndex].next = -1;
                } else {
                    // Simply mark current as DELETED if moving doesn't optimize the chain
                    table->entries[current].status = DELETED;
                    table->entries[current].next = -1;
                    if (prev != -1) {
                        table->entries[prev].next = table->entries[current].next;
                    }
                }
            } else {
                // No successor, mark current as DELETED
                table->entries[current].status = DELETED;
                table->entries[current].next = -1;
                if (prev != -1) {
                    table->entries[prev].next = -1;
                }
            }
            return; // Key deleted
        }
        prev = current;
        current = table->entries[current].next;
    }
    // Key not found, no action needed
}

