// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure
            // to initialize it here.
            explicit Node(const TYPE &val)
        : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to the element at that Node's
            // position.  There are two versions, getElt() and a dereference
            // operator, use whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data
            // members of this Node class from within the PairingPQ class.
            // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
            // function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;
    }; // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root{ nullptr }, total{ 0 } {
        // The heap starts empty. No further initialization is required.
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
        PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root{ nullptr }, total{ 0 } {
        // Add elements in the range to the heap.
        for (auto it = start; it != end; ++it) {
            addNode(*it);
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare } {
        // Implementing deep copy
        root = copy(other.root);
        total = other.total;
    }


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        if (this != &rhs) {
            PairingPQ temp(rhs);
            std::swap(root, temp.root);
            std::swap(total, temp.total);
        }
        return *this;
    }


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        clear(root);
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        std::deque<Node*> treeDeque;
        
        Node* curr = root;
        root = nullptr;
        treeDeque.push_back(curr);
        
        while (!treeDeque.empty()) {
            curr = treeDeque.front();
            treeDeque.pop_front();
            if (curr->child != nullptr) treeDeque.push_back(curr->child);
            if (curr->sibling != nullptr) treeDeque.push_back(curr->sibling);
            
            curr->parent = nullptr;
            curr->child = nullptr;
            curr->sibling = nullptr;
            
            if (root == nullptr) {
                root = curr;
            }
            else {
                root = meld(root, curr);
            }
        }
    }


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        if (root == nullptr) {
            // Handle the case where the heap is empty (no elements to pop)
            return;
        }

        Node* removed = root;
        Node* children = removed->child;
        
        delete removed; // Delete the removed node
        
        root = meldPairs(children); // Meld the children to reconstruct the heap
        total--;
    }


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function
        return root->getElt();
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return total;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        if (root != nullptr) {
            return false;
        }
        return true;
    } // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
            node->elt = new_value;
            if (node != root) {
                if (node->sibling) {
                    node->sibling->parent = node->parent;
                }
                if (node->parent->child == node) {
                    node->parent->child = node->sibling;
                } else {
                    node->parent->sibling = node->sibling;
                }
                node->parent = nullptr;
                node->sibling = nullptr;
                root = meld(root, node);
            }
        }


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node* addNode(const TYPE &val) {
        Node* newNode = new Node(val);
        if (root == nullptr) {
            root = newNode;
        }
        else {
            root = meld(root, newNode);
        }
        total++;
        return newNode;
    }


private:
    // TODO: Add any additional member variables or member functions you
    // require here.
    
    //root of our pairing heap
    Node *root = nullptr;
    
    //size of the PQ
    std::size_t total = 0;
    
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap
    // papers).
    Node* meld(Node* node1, Node* node2) {
        if (this->compare(node1->getElt(), node2->getElt())) {
            if (node2->child != nullptr) node1->sibling = node2->child;
            node1->parent = node2;
            node2->child = node1;
            
            return node2;
        }
        else {
            if (node1->child != nullptr) node2->sibling = node1->child;
            node2->parent = node1;
            node1->child = node2;
            
            return node1;
        }
    }
    
    Node* copy(Node* node) {
        if (!node) return nullptr;
        Node* newNode = new Node(node->getElt());
        newNode->child = copy(node->child);
        if (newNode->child) newNode->child->parent = newNode;
        newNode->sibling = copy(node->sibling);
        if (newNode->sibling) newNode->sibling->parent = newNode;
        return newNode;
    }
    
    void clear(Node* node) {
        if (!node) return;
        clear(node->child);
        clear(node->sibling);
        delete node;
    }
    
    Node* meldPairs(Node* first) {
        if (first == nullptr) return nullptr;
        if (first->sibling == nullptr) return first;
        
        std::deque<Node*> siblings;
        Node* curr = first;
        while (curr != nullptr) {
            siblings.push_back(curr);
            curr = curr->sibling;
        }
        while (siblings.size() > 1) {
            Node* A = siblings.front();
            A->parent = nullptr;
            A->sibling = nullptr;
            
            siblings.pop_front();
            Node* B = siblings.front();
            B->parent = nullptr;
            B->sibling = nullptr;
            
            siblings.pop_front();
            
            siblings.push_back(meld(A, B));
        }
     
        return siblings.front();
    }
    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
};


#endif // PAIRINGPQ_H
