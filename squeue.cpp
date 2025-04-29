// CMSC 341 - Spring 2025 - Project 3
#include "squeue.h"
#include <iostream>
using namespace std;

// Constructor
SQueue::SQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure)
{
  m_heap = nullptr;
  m_size = 0;
  m_priorFunc = priFn;
  m_heapType = heapType;
  m_structure = structure;
}

// Destructor
SQueue::~SQueue()
{
  clear();
}

// Clear function to deallocate all memory
void SQueue::clear()
{
  deleteSubtree(m_heap);
  m_heap = nullptr;
  m_size = 0;
}

// Recursive helper to delete the subtree
void SQueue::deleteSubtree(Post *node)
{
  if (node != nullptr)
  {
    deleteSubtree(node->m_left);
    deleteSubtree(node->m_right);
    delete node;
  }
}

// Copy constructor
SQueue::SQueue(const SQueue &rhs)
{
  m_heap = nullptr;
  m_size = 0;
  m_priorFunc = rhs.m_priorFunc;
  m_heapType = rhs.m_heapType;
  m_structure = rhs.m_structure;
  m_heap = copySubtree(rhs.m_heap);
  m_size = rhs.m_size;
}

// Recursive helper to copy the subtree
Post *SQueue::copySubtree(Post *node)
{
  if (node == nullptr)
  {
    return nullptr;
  }
  Post *newNode = new Post(*node);
  newNode->m_left = copySubtree(node->m_left);
  newNode->m_right = copySubtree(node->m_right);
  newNode->m_npl = node->m_npl;
  return newNode;
}

// Assignment operator
SQueue &SQueue::operator=(const SQueue &rhs)
{
  if (this != &rhs)
  {
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
void SQueue::mergeWithQueue(SQueue &rhs)
{
  if (this == &rhs)
  {
    throw domain_error("ERROR: Cannot merge a queue with itself");
  }
  if (m_priorFunc != rhs.m_priorFunc || m_structure != rhs.m_structure)
  {
    throw domain_error("ERROR: Cannot merge queues with different priority functions or structures");
  }
  m_heap = merge(m_heap, rhs.m_heap);
  m_size += rhs.m_size;
  rhs.m_heap = nullptr;
  rhs.m_size = 0;
}

// Insert a post
bool SQueue::insertPost(const Post &post)
{
  if (m_priorFunc(post) == 0 || post.getPostID() == DEFAULTPOSTID ||
      post.getPostTime() == MAXTIME || post.getConnectLevel() == MAXCONLEVEL)
  {
    return false;
  }
  Post *newPost = new Post(post);
  newPost->m_npl = 0;
  m_heap = merge(m_heap, newPost);
  m_size++;
  return true;
}

// Number of posts
int SQueue::numPosts() const
{
  return m_size;
}

// Get priority function
prifn_t SQueue::getPriorityFn() const
{
  return m_priorFunc;
}

// Extract highest priority post
Post SQueue::getNextPost()
{
  if (m_size == 0)
  {
    throw out_of_range("Queue is empty");
  }
  Post *highest = m_heap;
  m_heap = merge(highest->m_left, highest->m_right);
  m_size--;
  Post result = *highest;
  delete highest;
  return result;
}

// Helper to count nodes in the heap
int SQueue::countNodes(Post *node) const
{
  if (node == nullptr)
    return 0;
  return 1 + countNodes(node->m_left) + countNodes(node->m_right);
}

// Helper to gather nodes into an array
void SQueue::gatherNodes(Post *node, Post **nodes, int &index)
{
  if (node != nullptr)
  {
    nodes[index++] = node;
    gatherNodes(node->m_left, nodes, index);
    gatherNodes(node->m_right, nodes, index);
  }
}

// Set new priority function and rebuild heap
void SQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType)
{
  m_priorFunc = priFn;
  m_heapType = heapType;
  if (m_size == 0)
    return;
  int nodeCount = countNodes(m_heap);
  Post **nodes = new Post *[nodeCount];
  int index = 0;
  gatherNodes(m_heap, nodes, index);
  m_heap = nullptr;
  for (int i = 0; i < nodeCount; i++)
  {
    nodes[i]->m_left = nullptr;
    nodes[i]->m_right = nullptr;
    nodes[i]->m_npl = 0;
    m_heap = merge(m_heap, nodes[i]);
  }
  delete[] nodes;
}

// Set new structure and rebuild heap
void SQueue::setStructure(STRUCTURE structure)
{
  m_structure = structure;
  setPriorityFn(m_priorFunc, m_heapType);
}

// Getters
STRUCTURE SQueue::getStructure() const
{
  return m_structure;
}

HEAPTYPE SQueue::getHeapType() const
{
  return m_heapType;
}

// Print queue in preorder
void SQueue::printPostsQueue() const
{
  cout << "Contents of the queue:" << endl;
  printPreorder(m_heap);
}

// Dump heap structure
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

// Overloaded output operator for Post
ostream &operator<<(ostream &sout, const Post &post)
{
  sout << "Post#: " << post.getPostID()
       << ", likes#: " << post.getNumLikes()
       << ", connect level: " << post.getConnectLevel();
  return sout;
}

// Private helper to merge two heaps
Post *SQueue::merge(Post *h1, Post *h2)
{
  if (h1 == nullptr)
    return h2;
  if (h2 == nullptr)
    return h1;

  // Compare priorities, with postID as tie-breaker
  bool swapNeeded = false;
  int pri1 = m_priorFunc(*h1);
  int pri2 = m_priorFunc(*h2);
  if (m_heapType == MAXHEAP)
  {
    if (pri1 < pri2 || (pri1 == pri2 && h1->m_postID > h2->m_postID))
      swapNeeded = true;
  }
  else // MINHEAP
  {
    // Ensure smaller priority or smaller postID is root
    if (pri1 > pri2 || (pri1 == pri2 && h1->m_postID > h2->m_postID))
      swapNeeded = true;
  }

  if (swapNeeded)
  {
    Post *temp = h1;
    h1 = h2;
    h2 = temp;
  }

  if (m_structure == SKEW)
  {
    // Swap children and merge right subtree
    Post *temp = h1->m_left;
    h1->m_left = h1->m_right;
    h1->m_right = merge(h2, temp);
  }
  else // LEFTIST
  {
    h1->m_right = merge(h1->m_right, h2);
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
// Helper for preorder printing
void SQueue::printPreorder(Post *node) const
{
  if (node != nullptr)
  {
    cout << "[" << m_priorFunc(*node) << "] " << *node << endl;
    printPreorder(node->m_left);
    printPreorder(node->m_right);
  }
}