#ifndef LIST_H
#define LIST_H
/* List.h
 *
 * doubly-linked, double-ended list with Iterator interface
 * Project UID c1f28c309e55405daf00c565d57ff9ad
 * EECS 280 Project 4
 */

#include <iostream>
#include <cassert> //assert
#include <cstddef> //NULL

using namespace std;

template <typename T>
class List {
    //OVERVIEW: a doubly-linked, double-ended list with Iterator interface
public:
    
    //default construct
    List() : first(nullptr), last(nullptr), amt(0) {}
    
    //copy construct
    List(const List<T> &other) : first(nullptr), last(nullptr), amt(0) {
        copy_all(other);
    }
    
    //deconstruct
    ~List() {
        clear();
    }
    
    //assignment op
    List<T> &operator=(const List<T> &other) {
        if (this != &other) {
            clear();
            copy_all(other);
        }
        return *this;
    }
    
    //EFFECTS:  returns true if the list is empty
    bool empty() const {
        if (amt == 0) {
            return true;
        }
        return false;
    }
    
    //EFFECTS: returns the number of elements in this List
    //HINT:    Traversing a list is really slow.  Instead, keep track of the size
    //         with a private member variable.  That's how std::list does it.
    int size() const {
        return amt;
    }
    
    //REQUIRES: list is not empty
    //EFFECTS: Returns the first element in the list by reference
    T & front() {
        return first->datum;
    }
    
    //REQUIRES: list is not empty
    //EFFECTS: Returns the last element in the list by reference
    T & back() {
        return last->datum;
    }
    
    //EFFECTS:  inserts datum into the front of the list
    void push_front(const T &datum) {
        Node *first_node = new Node {first, nullptr, datum};
        if (empty()) {
            last = first_node;
            first = first_node;
        }
        else {
            first->prev = first_node;
        }
        first = first_node;
        amt++;
    }
    
    //EFFECTS:  inserts datum into the back of the list
    void push_back(const T &datum) {
        Node *last_node = new Node {nullptr, last, datum};
        if (empty()) {
            first = last_node;
            last = last_node;
        }
        else {
            last->next = last_node;
        }
        last = last_node;
        amt++;
    }
    
    //REQUIRES: list is not empty
    //MODIFIES: may invalidate list iterators
    //EFFECTS:  removes the item at the front of the list
    void pop_front() {
        Node *first_old = first;
        first = first_old->next;
        if (size() != 1) {
            first->prev = nullptr;
        }
        else {
            first = nullptr;
            last = nullptr;
        }
        delete first_old;
        --amt;
    }
    
    //REQUIRES: list is not empty
    //MODIFIES: may invalidate list iterators
    //EFFECTS:  removes the item at the back of the list
    void pop_back() {
        Node *last_old = last;
        last = last_old->prev;
        if (size() != 1) {
            last->next = nullptr;
        }
        else {
            last = nullptr;
            first = nullptr;
        }
        delete last_old;
        --amt;
    }
    
    //MODIFIES: may invalidate list iterators
    //EFFECTS:  removes all items from the list
    void clear() {
        while(!empty()) {
            pop_front();
        }
    }
    
    // You should add in a default constructor, destructor, copy constructor,
    // and overloaded assignment operator, if appropriate. If these operations
    // will work correctly without defining these, you can omit them. A user
    // of the class must be able to create, copy, assign, and destroy Lists
    
private:
    //a private type
    struct Node {
        Node *next;
        Node *prev;
        T datum;
    };
    
    //REQUIRES: list is empty
    //EFFECTS:  copies all nodes from other to this
    void copy_all(const List<T> &other) {
        Node *pos = other.first;
        while (pos != nullptr) {
            push_back(pos->datum);
            pos = pos->next;
        }
    }
    
    Node *first;   // points to first Node in list, or nullptr if list is empty
    Node *last;    // points to last Node in list, or nullptr if list is empty
    int amt = 0;
    
public:
    ////////////////////////////////////////
    class Iterator {
        //OVERVIEW: Iterator interface to List
        
        // You should add in a default constructor, destructor, copy constructor,
        // and overloaded assignment operator, if appropriate. If these operations
        // will work correctly without defining these, you can omit them. A user
        // of the class must be able to create, copy, assign, and destroy Iterators.
        
        // Your iterator should implement the following public operators: *,
        // ++ (prefix), default constructor, == and !=.
        
    public:
        // This operator will be used to test your code. Do not modify it.
        // Requires that the current element is dereferenceable.
        Iterator& operator--() {
            assert(node_ptr);
            node_ptr = node_ptr->prev;
            return *this;
        }
        
        
        
        // overload the equality operator
        bool operator==(const Iterator& other) const {
            return node_ptr == other.node_ptr;
        }
        
        // overload the inequality operator
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
        
        // overload the dereference operator to get the
        //value of the node pointed to by the iterator
        T& operator*() const {
            assert(node_ptr);
            return node_ptr->datum;
        }
        
        // overload the pre-increment operator to move the iterator to the next node
        Iterator& operator++() {
            if (node_ptr) {
                node_ptr = node_ptr->next;
            }
            return *this;
        }
        
    private:
        Node* node_ptr; // current Iterator position is a List node
        
        // construct an Iterator at a specific position
        Iterator(Node* p) : node_ptr(p) {}
        
        // add any additional necessary member variables here
        
        // add any friend declarations here
        friend class List;
        
    };
    
    //return iterator pointing at first element
    typename List<T>::Iterator begin() const {
      return Iterator(first);
    }
    
    // return an Iterator pointing to "past the end"
    typename List<T>::Iterator end() const {
        return Iterator(nullptr);
    }
    
    // REQUIRES: i is a valid, dereferenceable iterator associated with this list
    // MODIFIES: may invalidate other list iterators
    // EFFECTS: Removes a single element from the list container
    void erase(Iterator i) {
        if (i == end()) {
            return;
        }
        Node* node_to_remove = i.node_ptr;
        if (node_to_remove == first) {
            pop_front();
        } else if (node_to_remove == last) {
            pop_back();
        } else {
            node_to_remove->prev->next = node_to_remove->next;
            node_to_remove->next->prev = node_to_remove->prev;
            delete node_to_remove;
            --amt;
        }
    }
    
    // REQUIRES: i is a valid iterator associated with this list
    // EFFECTS: inserts datum before the element at the specified position.
    void insert(Iterator i, const T& datum) {
        Node* new_node = new Node{ nullptr, nullptr, datum };
        if (i == begin()) {
            push_front(datum);
        } else {
            Node* current_node = i.node_ptr;
            --i;
            Node* prev_node = i.node_ptr;
            new_node->next = current_node;
            new_node->prev = prev_node;
            prev_node->next = new_node;
            current_node->prev = new_node;
            ++amt;
        }
    }
};//List

////////////////////////////////////////////////////////////////////////////////
// Add your member function implementations below or in the class above
// (your choice). Do not change the public interface of List, although you
// may add the Big Three if needed.  Do add the public member functions for
// Iterator.

#endif // Do not remove this. Write all your code above this line.
