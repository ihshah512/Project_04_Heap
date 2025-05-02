// CMSC 341 - Spring 2025 - Project 3
#include "squeue.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

enum RANDOM
{
    UNIFORMINT,
    UNIFORMREAL,
    NORMAL,
    SHUFFLE
};
class Random
{
public:
    Random() {}
    Random(int min, int max, RANDOM type = UNIFORMINT, int mean = 50, int stdev = 20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL)
        {
            // the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            // the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            // the mean and standard deviation can change by passing new values to constructor
            m_normdist = std::normal_distribution<>(mean, stdev);
        }
        else if (type == UNIFORMINT)
        {
            // the case of UNIFORMINT to generate integer numbers
            //  Using a fixed seed value generates always the same sequence
            //  of pseudorandom numbers, e.g. reproducing scientific experiments
            //  here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10); // 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min, max);
        }
        else if (type == UNIFORMREAL)
        {                                   // the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10); // 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min, (double)max);
        }
        else
        { // the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum)
    {
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }
    void init(int min, int max)
    {
        m_min = min;
        m_max = max;
        m_type = UNIFORMINT;
        m_generator = std::mt19937(10); // 10 is the fixed seed value
        m_unidist = std::uniform_int_distribution<>(min, max);
    }
    void getShuffle(vector<int> &array)
    {
        // this function provides a list of all values between min and max
        // in a random order, this function guarantees the uniqueness
        // of every value in the list
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i <= m_max; i++)
        {
            array.push_back(i);
        }
        shuffle(array.begin(), array.end(), m_generator);
    }

    void getShuffle(int array[])
    {
        // this function provides a list of all values between min and max
        // in a random order, this function guarantees the uniqueness
        // of every value in the list
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i <= m_max; i++)
        {
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it = temp.begin(); it != temp.end(); it++)
        {
            array[i] = *it;
            i++;
        }
    }

    int getRandNum()
    {
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if (m_type == NORMAL)
        {
            // returns a random number in a set with normal distribution
            // we limit random numbers by the min and max values
            result = m_min - 1;
            while (result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT)
        {
            // this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum()
    {
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result * 100.0) / 100.0;
        return result;
    }

    string getRandString(int size)
    {
        // the parameter size specifies the length of string we ask for
        // to use ASCII char the number range in constructor must be set to 97 - 122
        // and the Random type must be UNIFORMINT (it is default in constructor)
        string output = "";
        for (int i = 0; i < size; i++)
        {
            output = output + (char)getRandNum();
        }
        return output;
    }

    int getMin() { return m_min; }
    int getMax() { return m_max; }

private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;            // normal distribution
    std::uniform_int_distribution<> m_unidist;        // integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal; // real uniform distribution
};

int priorityFn1(const Post &post); // works with a MAXHEAP
int priorityFn2(const Post &post); // works with a MINHEAP

class Tester
{
public:
    //THIS FUNCTION will be utilzied as a helper function
    bool checkHeapProperty(Post *node, prifn_t priFn, HEAPTYPE type)
    {
        if (!node)
            return true;

        int checkRootPrioity = priFn(*node);//checking the prioty of parent rather its min or max
        if (node->m_left)
        {
            int leftPri = priFn(*node->m_left);
            if ((type == MAXHEAP && checkRootPrioity < leftPri) ||
                (type == MINHEAP && checkRootPrioity > leftPri))
                return false;
        }
        if (node->m_right)
        {
            int rightPri = priFn(*node->m_right);
            if ((type == MAXHEAP && checkRootPrioity < rightPri) ||
                (type == MINHEAP && checkRootPrioity > rightPri))
                return false;
        }
        return checkHeapProperty(node->m_left, priFn, type) && //if both left and right side meets heap property the return true
               checkHeapProperty(node->m_right, priFn, type);
    }

    bool checkNPL(Post *node)
    {
        if (!node)
            return true;
        int leftNPL = node->m_left ? node->m_left->m_npl : -1;//if left node is nullptr then set its npl value to -1
        int rightNPL = node->m_right ? node->m_right->m_npl : -1;//if right node is nullptr then set its npl value to -1
        return (node->m_npl == std::min(leftNPL, rightNPL) + 1) &&
               (leftNPL >= rightNPL) &&//in case of skew heap left size npl must be greater than right side npl
               checkNPL(node->m_left) &&
               checkNPL(node->m_right);
    }
//in this function we would test min heap insertion after inserting posts we would check the heap property
    bool testMinHeapInsertion()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 300; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        return checkHeapProperty(q.m_heap, priorityFn2, MINHEAP);
    }

    bool testMaxHeapInsertion()
    {
        SQueue q(priorityFn1, MAXHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 300; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        return checkHeapProperty(q.m_heap, priorityFn1, MAXHEAP);
    }

    bool testMinHeapRemoval()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        std::vector<int> priorities;
        for (int i = 0; i < 10; i++)
        {
            Post p(r.getRandNum(), 100, 3, 25, 5);
            q.insertPost(p);
            priorities.push_back(priorityFn2(p));
        }
        std::sort(priorities.begin(), priorities.end());
        for (int i = 0; i < 10; i++)
        {
            try
            {
                if (priorityFn2(q.getNextPost()) != priorities[i])
                    return false;
            }
            catch (...)
            {
                return false;
            }
        }
        return true;
    }

    bool testMaxHeapRemoval()
    {
        SQueue q(priorityFn1, MAXHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        std::vector<int> priorities;
        for (int i = 0; i < 300; i++)
        {
            Post p(r.getRandNum(), i, 3, 25, 5);
            q.insertPost(p);
            priorities.push_back(priorityFn1(p));
        }
        std::sort(priorities.begin(), priorities.end(), std::greater<int>());
        for (int i = 0; i < 300; i++)
        {
            try
            {
                if (priorityFn1(q.getNextPost()) != priorities[i])
                    return false;
            }
            catch (...)
            {
                return false;
            }
        }
        return true;
    }

    bool testLeftistNPL()
    {
        SQueue q(priorityFn2, MINHEAP, LEFTIST);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        return checkNPL(q.m_heap);
    }

    bool testLeftistProperty()
    {
        SQueue q(priorityFn2, MINHEAP, LEFTIST);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        return checkHeapProperty(q.m_heap, priorityFn2, MINHEAP) && checkNPL(q.m_heap);
    }

    bool testPriorityChange()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        q.setPriorityFn(priorityFn1, MAXHEAP);
        return checkHeapProperty(q.m_heap, priorityFn1, MAXHEAP);
    }

    bool testEmptyMerge()
    {
        SQueue q1(priorityFn2, MINHEAP, SKEW);
        SQueue q2(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q2.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        try
        {
            q1.mergeWithQueue(q2);
            return q2.numPosts() == 0 && checkHeapProperty(q1.m_heap, priorityFn2, MINHEAP);
        }
        catch (...)
        {
            return false;
        }
    }

    bool testCopyConstructorNormal()
    {
        SQueue q1(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q1.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        SQueue q2(q1);
        return q1.numPosts() == q2.numPosts() && checkHeapProperty(q2.m_heap, priorityFn2, MINHEAP);
    }

    bool testCopyConstructorEmpty()
    {
        SQueue q1(priorityFn2, MINHEAP, SKEW);
        SQueue q2(q1);
        return q2.numPosts() == 0;
    }

    bool testAssignmentNormal()
    {
        SQueue q1(priorityFn2, MINHEAP, SKEW);
        SQueue q2(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q1.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        q2 = q1;
        return q1.numPosts() == q2.numPosts() && checkHeapProperty(q2.m_heap, priorityFn2, MINHEAP);
    }

    bool testAssignmentEmpty()
    {
        SQueue q1(priorityFn2, MINHEAP, SKEW);
        SQueue q2(priorityFn2, MINHEAP, SKEW);
        q2 = q1;
        return q2.numPosts() == 0;
    }

    bool testEmptyDequeue()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        try
        {
            q.getNextPost();
            return false;
        }
        catch (const std::out_of_range &)
        {
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool testDifferentPriorityMerge()
    {
        SQueue q1(priorityFn1, MAXHEAP, SKEW);
        SQueue q2(priorityFn2, MINHEAP, SKEW);
        try
        {
            q1.mergeWithQueue(q2);
            return false;
        }
        catch (const std::domain_error &)
        {
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool testNonEmptyMerge()
    {
        SQueue q1(priorityFn2, MINHEAP, SKEW);
        SQueue q2(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 50; i++)
        {
            q1.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        for (int i = 0; i < 50; i++)
        {
            q2.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        int totalPosts = q1.numPosts() + q2.numPosts();
        try
        {
            q1.mergeWithQueue(q2);
            return q1.numPosts() == totalPosts && q2.numPosts() == 0 && checkHeapProperty(q1.m_heap, priorityFn2, MINHEAP);
        }
        catch (...)
        {
            return false;
        }
    }

    bool testInvalidPostInsertion()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        Post invalidPost(MINPOSTID - 1, 100, 3, 25, 5);
        bool result = q.insertPost(invalidPost);
        return !result && q.numPosts() == 0;
    }

    bool testComplexOperations()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 50; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        for (int i = 0; i < 25; i++)
        {
            try
            {
                q.getNextPost();
            }
            catch (...)
            {
                return false;
            }
        }
        for (int i = 0; i < 25; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        q.setPriorityFn(priorityFn1, MAXHEAP);
        return checkHeapProperty(q.m_heap, priorityFn1, MAXHEAP) && q.numPosts() == 50;
    }

    bool testDeepCopyIntegrity()
    {
        SQueue q1(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 50; i++)
        {
            q1.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        SQueue q2(q1);
        try
        {
            q2.getNextPost();
        }
        catch (...)
        {
            return false;
        }
        return q1.numPosts() == 50 && q2.numPosts() == 49 && checkHeapProperty(q1.m_heap, priorityFn2, MINHEAP) && checkHeapProperty(q2.m_heap, priorityFn2, MINHEAP);
    }

    bool testLeftistSingleNodeNPL()
    {
        SQueue q(priorityFn2, MINHEAP, LEFTIST);
        Random r(MINPOSTID, MAXPOSTID);
        q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        return q.m_heap->m_npl == 0 && checkNPL(q.m_heap);
    }

    // New test functions for failed and passing unit tests
    bool testInsertMinSkewHeap()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 200; i++)
        {
            Post p(r.getRandNum(), 50 + i % 100, 2, 10 + i % 40, 3);
            if (!q.insertPost(p))
                return false;
        }
        return checkHeapProperty(q.m_heap, priorityFn2, MINHEAP) && q.numPosts() == 200;
    }

    bool testInsertMaxSkewHeap()
    {
        SQueue q(priorityFn1, MAXHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 200; i++)
        {
            Post p(r.getRandNum(), 50 + i % 100, 2, 10 + i % 40, 3);
            if (!q.insertPost(p))
                return false;
        }
        return checkHeapProperty(q.m_heap, priorityFn1, MAXHEAP) && q.numPosts() == 200;
    }

    bool testInsertMaxLeftistHeap()
    {
        SQueue q(priorityFn1, MAXHEAP, LEFTIST);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 200; i++)
        {
            Post p(r.getRandNum(), 50 + i % 100, 2, 10 + i % 40, 3);
            if (!q.insertPost(p))
                return false;
        }
        return checkHeapProperty(q.m_heap, priorityFn1, MAXHEAP) && checkNPL(q.m_heap) && q.numPosts() == 200;
    }

    bool testGetNextMinHeap()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        vector<pair<int, int>> priorities; // Store (priority, postID)
        for (int i = 0; i < 20; i++)
        {
            int connect = 1;  // Fixed connectLevel
            int time = 1 + i; // Sequential postTime: 1 to 20
            Post p(r.getRandNum(), 100, connect, time, 5);
            q.insertPost(p);
            priorities.push_back({priorityFn2(p), p.getPostID()});
        }
        // Sort by priority ascending, then by postID ascending for ties
        sort(priorities.begin(), priorities.end(),
             [](const pair<int, int> &a, const pair<int, int> &b)
             {
                 return a.first < b.first || (a.first == b.first && a.second < b.second);
             });
        for (int i = 0; i < 20; i++)
        {
            try
            {
                Post next = q.getNextPost();
                if (priorityFn2(next) != priorities[i].first || next.getPostID() != priorities[i].second)
                    return false;
            }
            catch (...)
            {
                return false;
            }
        }
        return q.numPosts() == 0 && q.m_heap == nullptr;
    }
    bool testGetNextMaxHeap()
    {
        SQueue q(priorityFn1, MAXHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        vector<pair<int, int>> priorities; // Store (priority, postID)
        for (int i = 0; i < 50; i++)
        {
            int likes = 100 + (i * 5); // 100 to 345
            Post p(r.getRandNum(), likes, 1, 1, 5);
            q.insertPost(p);
            priorities.push_back({priorityFn1(p), p.getPostID()});
        }
        // Sort by priority descending, then by postID ascending for ties
        sort(priorities.begin(), priorities.end(),
             [](const pair<int, int> &a, const pair<int, int> &b)
             {
                 return a.first > b.first || (a.first == b.first && a.second < b.second);
             });
        for (int i = 0; i < 50; i++)
        {
            try
            {
                Post next = q.getNextPost();
                if (priorityFn1(next) != priorities[i].first || next.getPostID() != priorities[i].second)
                    return false;
            }
            catch (...)
            {
                return false;
            }
        }
        return q.numPosts() == 0 && q.m_heap == nullptr;
    }

    bool testMergeWithQueue()
    {
        SQueue q1(priorityFn2, MINHEAP, LEFTIST);
        SQueue q2(priorityFn2, MINHEAP, LEFTIST);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q1.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
            q2.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        int totalPosts = q1.numPosts() + q2.numPosts();
        try
        {
            q1.mergeWithQueue(q2);
            return q1.numPosts() == totalPosts && q2.numPosts() == 0 && checkHeapProperty(q1.m_heap, priorityFn2, MINHEAP) && checkNPL(q1.m_heap);
        }
        catch (...)
        {
            return false;
        }
    }

    bool testEmptyMergeWithQueue()
    {
        SQueue q1(priorityFn2, MINHEAP, LEFTIST);
        SQueue q2(priorityFn2, MINHEAP, LEFTIST);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q2.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        try
        {
            q1.mergeWithQueue(q2);
            return q1.numPosts() == 100 && q2.numPosts() == 0 && checkHeapProperty(q1.m_heap, priorityFn2, MINHEAP) && checkNPL(q1.m_heap);
        }
        catch (...)
        {
            return false;
        }
    }

    bool testEmptyMergeWithEmpty()
    {
        SQueue q1(priorityFn2, MINHEAP, SKEW);
        SQueue q2(priorityFn2, MINHEAP, SKEW);
        try
        {
            q1.mergeWithQueue(q2);
            return q1.numPosts() == 0 && q2.numPosts() == 0;
        }
        catch (...)
        {
            return false;
        }
    }

    bool testSelfMergeWithQueue()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 50; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        try
        {
            q.mergeWithQueue(q);
            return false;
        }
        catch (const std::domain_error &)
        {
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool testSetPriorityFn()
    {
        SQueue q(priorityFn2, MINHEAP, LEFTIST);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100 + i % 50, 3, 25, 5));
        }
        q.setPriorityFn(priorityFn1, MAXHEAP);
        return checkHeapProperty(q.m_heap, priorityFn1, MAXHEAP) && checkNPL(q.m_heap);
    }

    bool testSetStructure()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        q.setStructure(LEFTIST);
        return checkHeapProperty(q.m_heap, priorityFn2, MINHEAP) && checkNPL(q.m_heap);
    }

    bool testGetNextException()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        try
        {
            q.getNextPost();
            return false;
        }
        catch (const std::out_of_range &)
        {
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool testMergeException()
    {
        SQueue q1(priorityFn1, MAXHEAP, LEFTIST);
        SQueue q2(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 50; i++)
        {
            q1.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
            q2.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        try
        {
            q1.mergeWithQueue(q2);
            return false;
        }
        catch (const std::domain_error &)
        {
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool testCopyConstructor()
    {
        SQueue q1(priorityFn2, MINHEAP, LEFTIST);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q1.insertPost(Post(r.getRandNum(), 100 + i % 50, 3, 25, 5));
        }
        SQueue q2(q1);
        return q1.numPosts() == q2.numPosts() && checkHeapProperty(q2.m_heap, priorityFn2, MINHEAP) && checkNPL(q2.m_heap);
    }

    bool testCopyConstructorEdge()
    {
        SQueue q1(priorityFn2, MINHEAP, LEFTIST);
        SQueue q2(q1);
        return q2.numPosts() == 0 && q2.m_heap == nullptr;
    }

    bool testAssignmentOp()
    {
        SQueue q1(priorityFn2, MINHEAP, LEFTIST);
        SQueue q2(priorityFn2, MINHEAP, LEFTIST);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q1.insertPost(Post(r.getRandNum(), 100 + i % 50, 3, 25, 5));
        }
        q2 = q1;
        return q1.numPosts() == q2.numPosts() && checkHeapProperty(q2.m_heap, priorityFn2, MINHEAP) && checkNPL(q2.m_heap);
    }

    bool testAssignmentOpEdge()
    {
        SQueue q1(priorityFn2, MINHEAP, LEFTIST);
        SQueue q2(priorityFn2, MINHEAP, LEFTIST);
        for (int i = 0; i < 50; i++)
        {
            q2.insertPost(Post(MINPOSTID + i, 100, 3, 25, 5));
        }
        q2 = q1;
        return q2.numPosts() == 0 && q2.m_heap == nullptr;
    }

    bool testSetSamePriorityFn()
    {
        SQueue q(priorityFn2, MINHEAP, SKEW);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        q.setPriorityFn(priorityFn2, MINHEAP);
        return checkHeapProperty(q.m_heap, priorityFn2, MINHEAP);
    }

    bool testSetSameStructure()
    {
        SQueue q(priorityFn2, MINHEAP, LEFTIST);
        Random r(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 100; i++)
        {
            q.insertPost(Post(r.getRandNum(), 100, 3, 25, 5));
        }
        q.setStructure(LEFTIST);
        return checkHeapProperty(q.m_heap, priorityFn2, MINHEAP) && checkNPL(q.m_heap);
    }
};

int main()
{
    Random idGen(MINPOSTID, MAXPOSTID);
    Random likesGen(MINLIKES, MAXLIKES);
    Random timeGen(MINTIME, MAXTIME);
    Random conLevelGen(MINCONLEVEL, MAXCONLEVEL);
    Random interestGen(MININTERESTLEVEL, MAXINTERESTLEVEL);
    SQueue queue1(priorityFn2, MINHEAP, LEFTIST);
    for (int i = 0; i < 10; i++)
    {
        Post aPost(idGen.getRandNum(),
                   likesGen.getRandNum(),
                   conLevelGen.getRandNum(),
                   timeGen.getRandNum(),
                   interestGen.getRandNum());
        queue1.insertPost(aPost);
    }
    cout << "\nDump of the leftist heap queue with priorityFn2 (MINHEAP):\n";
    queue1.dump();
    queue1.setStructure(SKEW);
    cout << "\nDump of the skew heap queue with priorityFn2 (MINHEAP):\n";
    queue1.dump();
    cout << "\nPreorder traversal of the nodes in the queue with priorityFn2 (MINHEAP):\n";
    queue1.printPostsQueue();
    queue1.setPriorityFn(priorityFn1, MAXHEAP);
    cout << "\nDump of the skew queue with priorityFn1 (MAXHEAP):\n";
    queue1.dump();
    cout << endl;
    cout << "****Calling TEST functions as follow ****" << endl;

    // Calling tst functions here

    Tester t;
    cout << "MinHeap Insertion: " << (t.testMinHeapInsertion() ? "PASS" : "FAIL") << endl;
    cout << "MaxHeap Insertion: " << (t.testMaxHeapInsertion() ? "PASS" : "FAIL") << endl;
    cout << "MinHeap Removal: " << (t.testMinHeapRemoval() ? "PASS" : "FAIL") << endl;
    cout << "MaxHeap Removal: " << (t.testMaxHeapRemoval() ? "PASS" : "FAIL") << endl;
    cout << "Leftist NPL: " << (t.testLeftistNPL() ? "PASS" : "FAIL") << endl;
    cout << "Leftist Property: " << (t.testLeftistProperty() ? "PASS" : "FAIL") << endl;
    cout << "Priority Change: " << (t.testPriorityChange() ? "PASS" : "FAIL") << endl;
    cout << "Empty Merge: " << (t.testEmptyMerge() ? "PASS" : "FAIL") << endl;
    cout << "Copy Constructor Normal: " << (t.testCopyConstructorNormal() ? "PASS" : "FAIL") << endl;
    cout << "Copy Constructor Empty: " << (t.testCopyConstructorEmpty() ? "PASS" : "FAIL") << endl;
    cout << "Assignment Normal: " << (t.testAssignmentNormal() ? "PASS" : "FAIL") << endl;
    cout << "Assignment Empty: " << (t.testAssignmentEmpty() ? "PASS" : "FAIL") << endl;
    cout << "Empty Dequeue: " << (t.testEmptyDequeue() ? "PASS" : "FAIL") << endl;
    cout << "Different Priority Merge: " << (t.testDifferentPriorityMerge() ? "PASS" : "FAIL") << endl;
    cout << "Non-Empty Merge: " << (t.testNonEmptyMerge() ? "PASS" : "FAIL") << endl;
    cout << "Invalid Post Insertion: " << (t.testInvalidPostInsertion() ? "PASS" : "FAIL") << endl;
    cout << "Complex Operations: " << (t.testComplexOperations() ? "PASS" : "FAIL") << endl;
    cout << "Deep Copy Integrity: " << (t.testDeepCopyIntegrity() ? "PASS" : "FAIL") << endl;
    cout << "Leftist Single Node NPL: " << (t.testLeftistSingleNodeNPL() ? "PASS" : "FAIL") << endl;
    cout << "Insert Min Skew Heap: " << (t.testInsertMinSkewHeap() ? "PASS" : "FAIL") << endl;
    cout << "Insert Max Skew Heap: " << (t.testInsertMaxSkewHeap() ? "PASS" : "FAIL") << endl;
    cout << "Insert Max Leftist Heap: " << (t.testInsertMaxLeftistHeap() ? "PASS" : "FAIL") << endl;
    cout << "Get Next Min Heap: " << (t.testGetNextMinHeap() ? "PASS" : "FAIL") << endl;
    cout << "Get Next Max Heap: " << (t.testGetNextMaxHeap() ? "PASS" : "FAIL") << endl;
    cout << "Merge With Queue: " << (t.testMergeWithQueue() ? "PASS" : "FAIL") << endl;
    cout << "Empty Merge With Queue: " << (t.testEmptyMergeWithQueue() ? "PASS" : "FAIL") << endl;
    cout << "Empty Merge With Empty: " << (t.testEmptyMergeWithEmpty() ? "PASS" : "FAIL") << endl;
    cout << "Self Merge With Queue: " << (t.testSelfMergeWithQueue() ? "PASS" : "FAIL") << endl;
    cout << "Set Priority Function: " << (t.testSetPriorityFn() ? "PASS" : "FAIL") << endl;
    cout << "Set Structure: " << (t.testSetStructure() ? "PASS" : "FAIL") << endl;
    cout << "Get Next Exception: " << (t.testGetNextException() ? "PASS" : "FAIL") << endl;
    cout << "Merge Exception: " << (t.testMergeException() ? "PASS" : "FAIL") << endl;
    cout << "Copy Constructor: " << (t.testCopyConstructor() ? "PASS" : "FAIL") << endl;
    cout << "Copy Constructor Edge: " << (t.testCopyConstructorEdge() ? "PASS" : "FAIL") << endl;
    cout << "Assignment Operator: " << (t.testAssignmentOp() ? "PASS" : "FAIL") << endl;
    cout << "Assignment Operator Edge: " << (t.testAssignmentOpEdge() ? "PASS" : "FAIL") << endl;
    cout << "Set Same Priority Function: " << (t.testSetSamePriorityFn() ? "PASS" : "FAIL") << endl;
    cout << "Set Same Structure: " << (t.testSetSameStructure() ? "PASS" : "FAIL") << endl;
    return 0;
}

/* Priority functions */
int priorityFn1(const Post &post)
{
    // needs MAXHEAP
    // priority value is determined based on some criteria
    // priority value falls in the range [1-510]
    // the highest priority would be 500+10 = 510
    // the lowest priority would be 0+1 = 1
    // the larger value means the higher priority
    int priority = post.getNumLikes() + post.getInterestLevel();
    int minValue = 1;
    int maxValue = 510;
    if (priority >= minValue && priority <= maxValue)
        return priority;
    else
        return 0; // this is an invalid order object
}

int priorityFn2(const Post &post)
{
    // needs MINHEAP
    // priority value is determined based on some criteria
    // priority value falls in the range [2-55]
    // the highest priority would be 1+1 = 2
    // the lowest priority would be 5+50 = 55
    // the smaller value means the higher priority
    int minValue = 2;
    int maxValue = 55;
    int priority = post.getPostTime() + post.getConnectLevel();
    if (priority >= minValue && priority <= maxValue)
        return priority;
    else
        return 0; // this is an invalid order object
}
