// CMSC 341 - Spring 2025 - Project 3
#include "squeue.h"
SQueue::SQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
  
}
SQueue::~SQueue() {
  
}
void SQueue::clear() {
    
}
SQueue::SQueue(const SQueue& rhs) {
  
}

SQueue& SQueue::operator=(const SQueue& rhs) {
  
}
void SQueue::mergeWithQueue(SQueue& rhs) {

}

bool SQueue::insertPost(const Post& post) {
    
}

int SQueue::numPosts() const {
  
}
prifn_t SQueue::getPriorityFn() const {
  
}
Post SQueue::getNextPost() {
    
}
void SQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
                      
}
void SQueue::setStructure(STRUCTURE structure){
    
}
STRUCTURE SQueue::getStructure() const {
  
}
HEAPTYPE SQueue::getHeapType() const {

}
void SQueue::printPostsQueue() const {
  
}

void SQueue::dump() const {
  if (m_size == 0) {
    cout << "Empty heap.\n" ;
  } else {
      dump(m_heap);
  }
  cout << endl;
}
void SQueue::dump(Post *pos) const {
  if ( pos != nullptr ) {
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

ostream& operator<<(ostream& sout, const Post& post) {
  sout  << "Post#: " << post.getPostID()
        << ", likes#: " << post.getNumLikes() 
        << ", connect level: " << post.getConnectLevel();
  return sout;
}
