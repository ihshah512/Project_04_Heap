// CMSC 341 - Spring 2025 - Project 3
#include "squeue.h"

// Constructor of Squeue
SQueue::SQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure)
{

    m_heap = nullptr;
    m_size = 0;
    m_priorFunc = priFn;
    m_heapType = heapType;
    m_structure = structure;
}
// DESTRUCTOR wil call clear function
SQueue::~SQueue()
{
    clear();
}
// clear funciton will deaallocate the memory and set the m_heap to nullptr
void SQueue::clear()
{
    while (m_heap != nullptr)
    {
        Post *temp = m_heap;
        m_heap = m_heap->m_right;
        delete temp;
    }
    m_size = 0;
    m_heap = nullptr;
}

// Copy constructor
SQueue::SQueue(const SQueue &rhs)
{
    m_heap = nullptr; // Initialize heap pointer to null
    m_size = 0;       // Initialize size to 0
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;

    // Perform deep copy of the heap
    m_heap = copySubtree(rhs.m_heap);
    m_size = rhs.m_size; // Set size to match the original
}

// Recursive helper to copy the subtree
Post *SQueue::copySubtree(Post *node)
{
    if (node == nullptr)
        return nullptr;

    // Create a new node with the same data
    Post *newNode = new Post(*node);
    // Recursively copy left and right subtrees
    newNode->m_left = copySubtree(node->m_left);
    newNode->m_right = copySubtree(node->m_right);
    newNode->m_npl = node->m_npl; // Preserve NPL for leftist heaps

    return newNode;
}

// deep copy using operator
// Assignment operator
SQueue &SQueue::operator=(const SQueue &rhs)
{
    if (this != &rhs)
    {            // Prevent self-assignment
        clear(); // Delete old content

        m_priorFunc = rhs.m_priorFunc;
        m_heapType = rhs.m_heapType;
        m_structure = rhs.m_structure;

        // Deep copy the heap
        m_heap = copySubtree(rhs.m_heap);
        m_size = rhs.m_size; // Set size to match the original
    }
    return *this;
}
// merging two ques
void SQueue::mergeWithQueue(SQueue &rhs)
{
    // error checking in case of merging with same que
    if (this == &rhs)
    {
        throw domain_error("Cannot merge a queue with itself");
    }
    // error checking in case of using non pertinent priority func or structuire
    if (m_priorFunc != rhs.m_priorFunc || m_structure != rhs.m_structure)
    {
        throw domain_error("Cannot merge queues with different priority functions or structures");
    }
    // using healper function for merge
    m_heap = merge(m_heap, rhs.m_heap);
    m_size += rhs.m_size;
    rhs.m_heap = nullptr;
    rhs.m_size = 0;
}
// insert a post in the que
bool SQueue::insertPost(const Post &post)
{
    if (m_priorFunc(post) == 0)
    {
        return false; // Invalid priority value
    }

    Post *newPost = new Post(post);
    m_heap = merge(m_heap, newPost);
    m_size++;
    return true;
}

// return the number of posts in que
int SQueue::numPosts() const
{
    return m_size;
}

// gettting currnt prioty function
prifn_t SQueue::getPriorityFn() const
{
    return m_priorFunc;
}
// extract and return the highest post
Post SQueue::getNextPost()
{
    if (m_size == 0)
    {
        throw out_of_range("Queue is empty");
    }

    Post *highest = m_heap;
    m_heap = merge(m_heap->m_left, m_heap->m_right);
    m_size--;

    Post result = *highest;
    delete highest;
    return result;
}

// set new priorty functon and rebuild heap
void SQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType)
{
    m_priorFunc = priFn;
    m_heapType = heapType;

    // Collect all nodes
    Post *current = m_heap;
    Post *nodes = nullptr;
    while (current != nullptr)
    {
        Post *temp = current;
        current = current->m_right;
        temp->m_left = temp->m_right = nullptr;
        temp->m_npl = 0;
        temp->m_right = nodes;
        nodes = temp;
    }
    m_heap = nullptr;

    // Rebuild heap
    current = nodes;
    while (current != nullptr)
    {
        Post *temp = current;
        current = current->m_right;
        temp->m_right = nullptr;
        m_heap = merge(m_heap, temp);
    }
}

// set new structre and rebulid hep
void SQueue::setStructure(STRUCTURE structure)
{
    m_structure = structure;
    setPriorityFn(m_priorFunc, m_heapType); // Rebuild using current priority function
}

// getter of stucture
STRUCTURE SQueue::getStructure() const
{
    return m_structure;
}

// getter of heap type
HEAPTYPE SQueue::getHeapType() const
{
    return m_heapType;
}
void SQueue::printPostsQueue() const
{
    cout << "Contents of the queue:" << endl;
    printPreorder(m_heap);
}

void SQueue::dump() const
{
    if (m_size == 0)
    {
        cout << "Empty heap.\n";
    }
    else
    {
        dump(m_heap);
    }
    cout << endl;
}
void SQueue::dump(Post *pos) const
{
    if (pos != nullptr)
    {
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

ostream &operator<<(ostream &sout, const Post &post)
{
    sout << "Post#: " << post.getPostID() << ", likes#: " << post.getNumLikes()
         << ", connect level: " << post.getConnectLevel();
    return sout;
}

// Private helper function to merge two heaps
Post *SQueue::merge(Post *h1, Post *h2)
{
    if (h1 == nullptr)
        return h2;
    if (h2 == nullptr)
        return h1;

    // Ensure h1 has higher priority
    if ((m_heapType == MAXHEAP && m_priorFunc(*h1) < m_priorFunc(*h2)) ||
        (m_heapType == MINHEAP && m_priorFunc(*h1) > m_priorFunc(*h2)))
    {
        Post *temp = h1;
        h1 = h2;
        h2 = temp;
    }

    if (m_structure == SKEW)
    {
        // Skew heap: swap children
        Post *temp = h1->m_left;
        h1->m_left = h1->m_right;
        h1->m_right = merge(h2, temp);
    }
    else
    { // LEFTIST
        h1->m_right = merge(h1->m_right, h2);
        // Update NPL and swap if necessary
        int leftNPL = (h1->m_left) ? h1->m_left->m_npl : -1;
        int rightNPL = (h1->m_right) ? h1->m_right->m_npl : -1;
        h1->m_npl = min(leftNPL, rightNPL) + 1;

        if (leftNPL < rightNPL)
        {
            Post *temp = h1->m_left;
            h1->m_left = h1->m_right;
            h1->m_right = temp;
        }
    }
    return h1;
}

// Helper function for preorder printing
void SQueue::printPreorder(Post *node) const
{
    if (node != nullptr)
    {
        cout << "[" << m_priorFunc(*node) << "] " << *node << endl;
        printPreorder(node->m_left);
        printPreorder(node->m_right);
    }
}