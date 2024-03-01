#include <atomic>
#include <vector>
#include <thread>
#include <cassert>


template <typename T>
class HazardPointer
{
public:
    std::atomic<std::thread::id> owner;
    std::atomic<void *> pointer;

    HazardPointer() : owner(std::thread::id()), pointer(nullptr) {}
};

template <typename T>
class HPManager
{
private:
    static const int MAX_HAZARD_POINTERS = 100;
    HazardPointer<T> hazardPointers[MAX_HAZARD_POINTERS];

public:
    HPManager() {}

    HazardPointer<T> *acquireHazardPointer()
    {
        std::thread::id nullThreadId;
        for (int i = 0; i < MAX_HAZARD_POINTERS; ++i)
        {
            std::thread::id oldId = nullThreadId;
            if (hazardPointers[i].owner.compare_exchange_strong(oldId, std::this_thread::get_id(),
                                                                std::memory_order_acquire, std::memory_order_relaxed))
            {
                return &hazardPointers[i];
            }
        }
        assert(false); // No available hazard pointers, should handle this case better in production code
        return nullptr;
    }

    void releaseHazardPointer(HazardPointer<T> *hp)
    {
        hp->pointer.store(nullptr, std::memory_order_release);
        hp->owner.store(std::thread::id(), std::memory_order_release);
    }

    bool isPointerHazardous(void *ptr)
    {
        for (int i = 0; i < MAX_HAZARD_POINTERS; ++i)
        {
            if (hazardPointers[i].pointer.load(std::memory_order_acquire) == ptr)
            {
                return true;
            }
        }
        return false;
    }

    // In a complete implementation, you would also need methods to retire nodes and to reclaim retired nodes safely.
    bool search(T data)
    {
        Node *temp = nullptr;
        HPManager<T> &hpManager = getHPManager(); // Get the global HP manager instance
        HazardPointer<T> *myHP = hpManager.acquireHazardPointer();

        while (true)
        {
            Node *current = head.load(std::memory_order_acquire);
            while (current != nullptr)
            {
                myHP->pointer.store(current, std::memory_order_release); // Mark current node as hazardous
                if (current != head.load(std::memory_order_acquire))
                {
                    break; // The head changed, restart the search
                }

                // Verify the node has not been deleted after setting the hazard pointer
                if (current->data == data)
                {
                    hpManager.releaseHazardPointer(myHP);
                    return true; // Data found
                }
                current = current->next.load(std::memory_order_acquire);
            }
            if (current == nullptr)
            {
                hpManager.releaseHazardPointer(myHP);
                return false; // Data not found
            }
        }
    }

    bool remove(T data)
    {
        HPManager<T> &hpManager = getHPManager(); // Get the global HP manager instance
        std::vector<Node *> retiredNodes;         // Nodes to be deleted later

        while (true)
        {
            Node *prev = nullptr;
            Node *current = head.load(std::memory_order_acquire);

            while (current != nullptr)
            {
                Node *next = current->next.load(std::memory_order_acquire);

                if (current->data == data)
                {
                    // Check if the node is hazardous
                    if (!hpManager.isPointerHazardous(current))
                    {
                        // Attempt to unlink the node
                        if (prev == nullptr)
                        {
                            if (head.compare_exchange_strong(current, next, std::memory_order_release))
                            {
                                retiredNodes.push_back(current); // Schedule for deletion
                                break;
                            }
                        }
                        else if (prev->next.compare_exchange_strong(current, next, std::memory_order_release))
                        {
                            retiredNodes.push_back(current); // Schedule for deletion
                            break;
                        }
                    }
                    else
                    {
                        // Node is hazardous, defer deletion
                        // Note: You would typically add this node to a list of nodes to retry deleting later
                    }
                }
                prev = current;
                current = next;
            }

            // Try to reclaim memory for retired nodes that are no longer hazardous
            for (Node *node : retiredNodes)
            {
                if (!hpManager.isPointerHazardous(node))
                {
                    delete node; // Safely delete the node
                }
                else
                {
                    // Node is still hazardous, defer deletion
                    // Add back to the list of nodes to retry deleting later
                }
            }
            retiredNodes.clear();

            return current != nullptr; // Return true if a node was removed
        }
    }

    
};
