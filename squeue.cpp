// CMSC 341 - Spring 2025 - Project 3
#include "squeue.h"
#include <stdexcept>
#include <iostream>
#include <vector>
using namespace std;

// Constructor
SQueue::SQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
    m_heap = nullptr;
    m_size = 0;
    m_priorFunc = priFn;
    m_heapType = heapType;
    m_structure = structure;
}

// Destructor
SQueue::~SQueue() {
    clear();
}

// Clear function to deallocate all memory
void SQueue::clear() {
    deleteSubtree(m_heap);
    m_heap = nullptr;
    m_size = 0;
}

// Recursive helper to delete the subtree
void SQueue::deleteSubtree(Post* node) {
    if (node != nullptr) {
        deleteSubtree(node->m_left);
        deleteSubtree(node->m_right);
        delete node;
    }
}

// Copy constructor
SQueue::SQueue(const SQueue& rhs) {
    m_heap = nullptr;
    m_size = 0;
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
    m_heap = copySubtree(rhs.m_heap);
    m_size = rhs.m_size;
}

// Recursive helper to copy the subtree
Post* SQueue::copySubtree(Post* node) {
    if (node == nullptr) return nullptr;
    Post* newNode = new Post(*node);
    newNode->m_left = copySubtree(node->m_left);
    newNode->m_right = copySubtree(node->m_right);
    newNode->m_npl = node->m_npl;
    return newNode;
}

// Assignment operator
SQueue& SQueue::operator=(const SQueue& rhs) {
    if (this != &rhs) {
        clear();
        m_priorFunc = rhs.m_priorFunc;
        m_heapType = rhs.m_heapType;
        m_structure = rhs.m_structure;
        m_heap = copySubtree(rhs.m_heap);
        m_size = rhs.m_size;
    }
    return *this;
}

// Merge two queues
void SQueue::mergeWithQueue(SQueue& rhs) {
    if (this == &rhs) {
        throw domain_error("Cannot merge a queue with itself");
    }
    if (m_priorFunc != rhs.m_priorFunc || m_structure != rhs.m_structure) {
        throw domain_error("Cannot merge queues with different priority functions or structures");
    }
    m_heap = merge(m_heap, rhs.m_heap);
    m_size += rhs.m_size;
    rhs.m_heap = nullptr;
    rhs.m_size = 0;
}

// Insert a post
bool SQueue::insertPost(const Post& post) {
    if (m_priorFunc(post) == 0) {
        return false;
    }
    Post* newPost = new Post(post);
    newPost->m_npl = 0;
    m_heap = merge(m_heap, newPost);
    m_size++;
    return true;
}

// Number of posts
int SQueue::numPosts() const {
    return m_size;
}

// Get priority function
prifn_t SQueue::getPriorityFn() const {
    return m_priorFunc;
}

// Extract highest priority post
Post SQueue::getNextPost() {
    if (m_size == 0) {
        throw out_of_range("Queue is empty");
    }
    Post* highest = m_heap;
    m_heap = merge(highest->m_left, highest->m_right);
    m_size--;
    Post result = *highest;
    delete highest;
    return result;
}

// Set new priority function and rebuild heap
void SQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
    m_priorFunc = priFn;
    m_heapType = heapType;
    vector<Post*> nodes;
    collectNodes(m_heap, nodes);
    m_heap = nullptr;
    for (Post* node : nodes) {
        node->m_left = nullptr;
        node->m_right = nullptr;
        node->m_npl = 0;
        m_heap = merge(m_heap, node);
    }
}

// Helper to collect nodes
void SQueue::collectNodes(Post* node, vector<Post*>& nodes) {
    if (node != nullptr) {
        nodes.push_back(node);
        collectNodes(node->m_left, nodes);
        collectNodes(node->m_right, nodes);
    }
}

// Set new structure and rebuild heap
void SQueue::setStructure(STRUCTURE structure) {
    m_structure = structure;
    setPriorityFn(m_priorFunc, m_heapType);
}

// Getters
STRUCTURE SQueue::getStructure() const {
    return m_structure;
}

HEAPTYPE SQueue::getHeapType() const {
    return m_heapType;
}

// Print queue in preorder
void SQueue::printPostsQueue() const {
    cout << "Contents of the queue:" << endl;
    printPreorder(m_heap);
}

// Dump heap structure
void SQueue::dump() const {
    if (m_size == 0) {
        cout << "Empty heap.\n";
    } else {
        dump(m_heap);
    }
    cout << endl;
}

void SQueue::dump(Post* pos) const {
    if (pos != nullptr) {
        cout << "(";
        dump(pos->m_left);
        if (m_structure == SKEW)
            cout << m_priorFunc(*pos) << ":" << pos->m_postID;
        else
            cout << m_priorFunc(*pos) << ":" << pos->m_postID << ":" << pos->m_npl;
        dump(pos->m_right);
        cout << ")";
    }
}

// Overloaded output operator for Post
ostream& operator<<(ostream& sout, const Post& post) {
    sout << "Post#: " << post.getPostID()
         << ", likes#: " << post.getNumLikes()
         << ", connect level: " << post.getConnectLevel();
    return sout;
}

// Private helper to merge two heaps
Post* SQueue::merge(Post* h1, Post* h2) {
    if (h1 == nullptr) return h2;
    if (h2 == nullptr) return h1;

    if ((m_heapType == MAXHEAP && m_priorFunc(*h1) < m_priorFunc(*h2)) ||
        (m_heapType == MINHEAP && m_priorFunc(*h1) > m_priorFunc(*h2))) {
        swap(h1, h2);
    }

    if (m_structure == SKEW) {
        Post* temp = h1->m_left;
        h1->m_left = h1->m_right;
        h1->m_right = merge(h2, temp);
    } else { // LEFTIST
        h1->m_right = merge(h1->m_right, h2);
        int leftNPL = (h1->m_left) ? h1->m_left->m_npl : -1;
        int rightNPL = (h1->m_right) ? h1->m_right->m_npl : -1;
        h1->m_npl = min(leftNPL, rightNPL) + 1;
        if (leftNPL < rightNPL) {
            swap(h1->m_left, h1->m_right);
        }
    }
    return h1;
}

// Helper for preorder printing
void SQueue::printPreorder(Post* node) const {
    if (node != nullptr) {
        cout << "[" << m_priorFunc(*node) << "] " << *node << endl;
        printPreorder(node->m_left);
        printPreorder(node->m_right);
    }
}
