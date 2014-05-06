
#pragma once

// engine includes
#include "Types.h"

// Compare and swap
#define CAS_(v, p, n)      __sync_bool_compare_and_swap(v, p, n)

// ================================================================================================
// A multi-producer, multi-consumer lock-free queue with a maximum size specified on construction
// ================================================================================================
template<typename T>
class LockFreeQueue {

public:

    // Constructor / destructor
    LockFreeQueue(size_t poolSize);
    ~LockFreeQueue();

    bool Push(const T& t);
    bool Pop(T& t);
    bool Empty();

protected:

    class CNode {
    public:
        T data;
        volatile U64 next;
    };

    CNode* nodes;
    volatile U64 freeHead;
    volatile U64 head;
    volatile U64 tail;

    // Allocating and freeing nodes from the pool
    U64 AllocNode();
    void FreeNode(CNode* n);
};

// ================================================================================================
// Some useful macros for dealing with the U64 tag + ticket and converting between tags and indices
// ================================================================================================
#define Idx2Tag_(i) (i + 1)
#define Tag2Idx_(t) (t - 1)
#define Pair2Tag_(v) ((v) & 0x00000000ffffffff)
#define Pair2Ticket_(v) ((v) >> 32)
#define BuildPair_(tag, ticket) (((ticket) << 32) | (tag))

// ================================================================================================
// Constructor
// ================================================================================================
template<typename T>
inline LockFreeQueue<T>::LockFreeQueue(size_t poolSize) {
    // Allocate the nodes
    nodes = new CNode[poolSize];

    // Set up the free list
    freeHead = Idx2Tag_(1);
    for(U32 i = 1; i < U32(poolSize-1); ++i)
        nodes[i].next = Idx2Tag_(i+1);
    nodes[poolSize-1].next = 0;

    // Set up the dummy node at idx 0 and set head and tail to point to it
    nodes[0].next = 0;
    head = Idx2Tag_(0);
    tail = Idx2Tag_(0);
}

// ================================================================================================
// Destructor
// ================================================================================================
template<typename T>
inline LockFreeQueue<T>::~LockFreeQueue() {
    delete [] nodes;
}

// ================================================================================================
// Push a value into the queue
// ================================================================================================
template<typename T>
inline bool LockFreeQueue<T>::Push(const T& t) {
    U64 tag = AllocNode();
    if(tag == 0)
        return false;

    CNode* n = nodes + Tag2Idx_(tag);
    n->data = t;

    U64 tmpTail;
    while(true) {
        // Cache things
        tmpTail = tail;
        CNode* tailNode = nodes + Tag2Idx_(Pair2Tag_(tmpTail));
        U64 tmpTailNext = tailNode->next;

        // If tail has changed then our cache is potentially inconsistent, so start over
        if(tmpTail != tail)
            continue;

        // If tail->next is NULL (meaning the tail is actually pointing at the tail)
        U64 tailNextTag = Pair2Tag_(tmpTailNext);
        if(tailNextTag == 0) {
            // Put our node just after the tail
            U64 newTicket = Pair2Ticket_(tmpTailNext) + 1;
            if(CAS_(&tailNode->next, tmpTailNext, BuildPair_(tag, newTicket)))
                break;
        }
        else {
            // Tail isn't actually at the end, so attempt to move it back one at a time
            U64 newTicket = Pair2Ticket_(tmpTail) + 1;
            CAS_(&tail, tmpTail, BuildPair_(tailNextTag, newTicket));
            continue;
        }
    }

    // Attempt to move the tail
    U64 newTailTicket = Pair2Ticket_(tmpTail) + 1;
    CAS_(&tail, tmpTail, BuildPair_(tag, newTailTicket));

    return true;
}

// ================================================================================================
// Pop a value from the queue
// ================================================================================================
template<typename T>
inline bool LockFreeQueue<T>::Pop(T& t) {
    CNode* headNode = NULL;

    while(true) {
        // First we cache head and we'll use this to verify our cached values
        U64 tmpHead = head;
        // We need to cache tail before getting head->next
        // If we got head->next first it might be null while tail points to the same
        // thing, but then tail could be updated before we cache tail which would
        // make those two pieces of information inconsistent
        U64 tmpTail = tail;
        // Now we can get the head node and cache its next value
        U64 tmpHeadTag = Pair2Tag_(tmpHead);
        headNode = nodes + Tag2Idx_(tmpHeadTag);
        U64 tmpHeadNext = headNode->next;
        
        // If the head has changed then our cache is potentially inconsistent, so start over
        if(tmpHead != head)
            continue;

        // If the head and tail point to the same node
        U64 tmpHeadNextTag = Pair2Tag_(tmpHeadNext);
        if(tmpHeadTag == Pair2Tag_(tmpTail)) {
            // If the head/tail pointers are actually pointing to the end of the list
            if(tmpHeadNextTag == 0)
                return false;

            // Otherwise the tail is just not pointing to the end of the queue, so try to
            // advance it
            U64 tailTicket = Pair2Ticket_(tmpTail);
            CAS_(&tail, tmpTail, BuildPair_(tmpHeadNextTag, tailTicket + 1));
        }
        else {
            // Read the node data before we do the compare-and-swap, because that will
            // "publish" this action at which point another Pop could free the next node
            CNode* nextNode = nodes + Tag2Idx_(tmpHeadNextTag);
            t = nextNode->data;
            U64 headTicket = Pair2Ticket_(tmpHead);
            if(CAS_(&head, tmpHead, BuildPair_(tmpHeadNextTag, headTicket + 1)))
                break;
        }
    }

    // We can now free the node that we just popped
    FreeNode(headNode);

    return true;
}

// ================================================================================================
// Check to see if the queue is empty
// ================================================================================================
template<typename T>
inline bool LockFreeQueue<T>::Empty() {
    U64 tmpHeadNext;

    while(true) {
        U64 tmpHead = head;
        U64 tmpHeadTag = Pair2Tag_(tmpHead);
        CNode* headNode = nodes + Tag2Idx_(tmpHeadTag);
        tmpHeadNext = headNode->next;
        
        // If tmpHead is still the same as head, we have an internally consistent cache,
        // so we can exit the loop
        if(tmpHead == head)
            break;
    }

    // If the head next tag is zero, the queue is empty
    return Pair2Tag_(tmpHeadNext) == 0;
}

// ================================================================================================
// Allocate a node from the free list
// ================================================================================================
template<typename T>
inline U64 LockFreeQueue<T>::AllocNode() {
    U64 result;
    CNode* n;
    while(true) {
        U64 tmpFreeHead = freeHead;

        result = Pair2Tag_(tmpFreeHead);
        if(result == 0)
            return 0;

        n = nodes + Tag2Idx_(result);
        U64 newTag = Pair2Tag_(n->next);

        // If freeHead has changed, our cached values might be inconsistent, so we
        // should try again from the beginning
        if(tmpFreeHead != freeHead)
            continue;

        U64 newTicket = Pair2Ticket_(tmpFreeHead) + 1;
        if(CAS_(&freeHead, tmpFreeHead, BuildPair_(newTag, newTicket)))
            break;
    }
    U64 newTicket = Pair2Ticket_(n->next) + 1;
    n->next = BuildPair_(0, newTicket);
    return result;
}

// ================================================================================================
// Return a node to the free list
// ================================================================================================
template<typename T>
inline void LockFreeQueue<T>::FreeNode(CNode* n) {
    U64 tag = Idx2Tag_(n - nodes);
    U64 newNextTicket = Pair2Ticket_(n->next) + 1;
    while(true) {
        U64 tmpFreeHead = freeHead;
        n->next = BuildPair_(Pair2Tag_(tmpFreeHead), newNextTicket);
        U64 newFreeTicket = Pair2Ticket_(tmpFreeHead) + 1;
        if(CAS_(&freeHead, tmpFreeHead, BuildPair_(tag, newFreeTicket)))
            break;
    }
}

// ================================================================================================
// Undefine the macros used in our implementation
// ================================================================================================
#undef Idx2Tag_
#undef Tag2Idx_
#undef Pair2Tag_
#undef Pair2Ticket_
#undef BuildPair_

