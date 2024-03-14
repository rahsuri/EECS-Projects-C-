// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

#ifndef BINARYPQ_H
#define BINARYPQ_H


#include <algorithm>
#include "Eecs281PQ.h"

// A specialized version of the priority queue ADT implemented as a binary heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty PQ with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        // TODO: Implement this function, or verify that it is already done
    } // BinaryPQ


    // Description: Construct a PQ out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        // TODO: Implement this function done
        for (InputIterator it = start; it != end; ++it) {
            data.push_back(*it);
        }
        updatePriorities();
    } // BinaryPQ


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automatically.
    virtual ~BinaryPQ() {
    } // ~BinaryPQ()

    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        for (size_t i = 0; i < data.size(); i++) {
            fixDown(data.size() - i - 1);
        }
    } // updatePriorities()


    // Description: Add a new element to the PQ.
    // Runtime: O(log(n))
    virtual void push(const TYPE &val) {
        // TODO: Implement this function. done
        data.push_back(val);
        fixUp(data.size() - 1);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the PQ.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the PQ is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    virtual void pop() {
        // TODO: Implement this function. done
        std::swap(getEl(1), getEl(data.size()));
        data.pop_back();
        fixDown(0);
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the PQ. This should be a reference for speed. It MUST
    //              be const because we cannot allow it to be modified, as
    //              that might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function. done
        return data[0];
    } // top()


    // Description: Get the number of elements in the PQ.
    // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function. done
        return data.size();
    } // size()


    // Description: Return true if the PQ is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function. done
        return data.empty();
    } // empty()


private:
    // Note: This vector *must* be used for your PQ implementation.
    std::vector<TYPE> data;
    // NOTE: You are not allowed to add any member variables. You don't need
    //       a "heapSize", since you can call your own size() member
    //       function, or check data.size().
    // Translate 1-based indexing into a 0-based vector
    TYPE &getEl(std::size_t i) {
      return data[i - 1];
    }  // getElement()

    const TYPE &getEl(std::size_t i) const {
      return data[i - 1];
    }  // getElement()
    
    //fix up is only for adding values / initializing a heap
    void fixUp(size_t idx) {
        idx++;
        while (idx > 1 && this->compare(getEl(idx / 2), getEl(idx))) {
            std::swap(getEl(idx), getEl(idx / 2));
            idx /= 2;
        }
    }
    
    //fix down is only for popping the top value / updating priorities
    void fixDown(size_t idx) {
        idx++;
        while (2 * idx <= data.size()) {
            size_t j = 2 * idx;
            if (j < data.size() && this->compare(getEl(j), getEl(j + 1))) {
                ++j;
            }
            if (!this->compare(getEl(idx), getEl(j))) {
                break;
            }
            std::swap(getEl(idx), getEl(j));
            idx = j;
        } // while
    } //fixDown
    
    // TODO: Add any additional member functions you require here. done
    //       For instance, you might add fixUp() and fixDown().
}; // BinaryPQ


#endif // BINARYPQ_H
