#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>
#include <functional>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;

        // Constructor for sentinel node (no data)
        node() : data(nullptr), prev(nullptr), next(nullptr) {}

        // Constructor with data
        node(const T &value) : data(new T(value)), prev(nullptr), next(nullptr) {}

        // Destructor
        ~node() {
            if (data != nullptr) {
                delete data;
            }
        }
    };

protected:
    node *head;  // sentinel node at the beginning
    node *tail;  // sentinel node at the end
    size_t count;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        return cur;
    }

    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        return pos;
    }

    void init() {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
        count = 0;
    }

public:
    class const_iterator;
    class iterator {
    private:
        node *ptr;
        const list *list_ptr;

    public:
        friend class list;
        friend class const_iterator;

        iterator(node *p = nullptr, const list *l = nullptr) : ptr(p), list_ptr(l) {}

        /**
         * iter++
         */
        iterator operator++(int) {
            if (ptr == nullptr || ptr->next == nullptr || list_ptr == nullptr) {
                throw invalid_iterator();
            }
            // Check if we're at end(), can't go forward from there
            if (ptr == list_ptr->tail) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        /**
         * ++iter
         */
        iterator & operator++() {
            if (ptr == nullptr || ptr->next == nullptr || list_ptr == nullptr) {
                throw invalid_iterator();
            }
            // Check if we're at end(), can't go forward from there
            if (ptr == list_ptr->tail) {
                throw invalid_iterator();
            }
            ptr = ptr->next;
            return *this;
        }

        /**
         * iter--
         */
        iterator operator--(int) {
            if (ptr == nullptr || ptr->prev == nullptr || list_ptr == nullptr) {
                throw invalid_iterator();
            }
            // Check if we're at begin(), can't go back from there
            if (ptr == list_ptr->head->next) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        /**
         * --iter
         */
        iterator & operator--() {
            if (ptr == nullptr || ptr->prev == nullptr || list_ptr == nullptr) {
                throw invalid_iterator();
            }
            // Check if we're at begin(), can't go back from there
            if (ptr == list_ptr->head->next) {
                throw invalid_iterator();
            }
            ptr = ptr->prev;
            return *this;
        }

        /**
         * *it
         * remember to throw if iterator is invalid
         */
        T & operator *() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }

        /**
         * it->field
         * remember to throw if iterator is invalid
         */
        T * operator ->() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return ptr->data;
        }

        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }

        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
    };

    /**
     * has same function as iterator, just for a const object.
     * should be able to construct from an iterator.
     */
    class const_iterator {
    private:
        const node *ptr;
        const list *list_ptr;

    public:
        friend class list;

        const_iterator(const node *p = nullptr, const list *l = nullptr) : ptr(p), list_ptr(l) {}

        const_iterator(const iterator &it) : ptr(it.ptr), list_ptr(it.list_ptr) {}

        const_iterator operator++(int) {
            if (ptr == nullptr || ptr->next == nullptr || list_ptr == nullptr) {
                throw invalid_iterator();
            }
            if (ptr == list_ptr->tail) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        const_iterator & operator++() {
            if (ptr == nullptr || ptr->next == nullptr || list_ptr == nullptr) {
                throw invalid_iterator();
            }
            if (ptr == list_ptr->tail) {
                throw invalid_iterator();
            }
            ptr = ptr->next;
            return *this;
        }

        const_iterator operator--(int) {
            if (ptr == nullptr || ptr->prev == nullptr || list_ptr == nullptr) {
                throw invalid_iterator();
            }
            if (ptr == list_ptr->head->next) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        const_iterator & operator--() {
            if (ptr == nullptr || ptr->prev == nullptr || list_ptr == nullptr) {
                throw invalid_iterator();
            }
            if (ptr == list_ptr->head->next) {
                throw invalid_iterator();
            }
            ptr = ptr->prev;
            return *this;
        }

        const T & operator *() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }

        const T * operator ->() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return ptr->data;
        }

        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }

        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
    };

    /**
     * Constructs
     * Atleast two: default constructor, copy constructor
     */
    list() {
        init();
    }

    list(const list &other) {
        init();
        for (node *p = other.head->next; p != other.tail; p = p->next) {
            push_back(*(p->data));
        }
    }

    /**
     * Destructor
     */
    virtual ~list() {
        clear();
        delete head;
        delete tail;
    }

    /**
     * Assignment operator
     */
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (node *p = other.head->next; p != other.tail; p = p->next) {
            push_back(*(p->data));
        }
        return *this;
    }

    /**
     * access the first / last element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const {
        if (empty()) {
            throw container_is_empty();
        }
        return *(head->next->data);
    }

    const T & back() const {
        if (empty()) {
            throw container_is_empty();
        }
        return *(tail->prev->data);
    }

    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        return iterator(head->next, this);
    }

    const_iterator cbegin() const {
        return const_iterator(head->next, this);
    }

    /**
     * returns an iterator to the end.
     */
    iterator end() {
        return iterator(tail, this);
    }

    const_iterator cend() const {
        return const_iterator(tail, this);
    }

    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const {
        return count == 0;
    }

    /**
     * returns the number of elements
     */
    virtual size_t size() const {
        return count;
    }

    /**
     * clears the contents
     */
    virtual void clear() {
        node *p = head->next;
        while (p != tail) {
            node *tmp = p;
            p = p->next;
            delete tmp;
        }
        head->next = tail;
        tail->prev = head;
        count = 0;
    }

    /**
     * insert value before pos (pos may be the end() iterator)
     * return an iterator pointing to the inserted value
     * throw if the iterator is invalid
     */
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.ptr == nullptr || pos.list_ptr != this) {
            throw invalid_iterator();
        }
        node *new_node = new node(value);
        insert(pos.ptr, new_node);
        count++;
        return iterator(new_node, this);
    }

    /**
     * remove the element at pos (the end() iterator is invalid)
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid
     */
    virtual iterator erase(iterator pos) {
        if (empty()) {
            throw container_is_empty();
        }
        if (pos.ptr == nullptr || pos.ptr->data == nullptr || pos.list_ptr != this) {
            throw invalid_iterator();
        }
        node *tmp = pos.ptr;
        node *next_node = tmp->next;
        erase(tmp);
        delete tmp;
        count--;
        return iterator(next_node, this);
    }

    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        node *new_node = new node(value);
        insert(tail, new_node);
        count++;
    }

    /**
     * removes the last element
     * throw when the container is empty.
     */
    void pop_back() {
        if (empty()) {
            throw container_is_empty();
        }
        node *tmp = tail->prev;
        erase(tmp);
        delete tmp;
        count--;
    }

    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        node *new_node = new node(value);
        insert(head->next, new_node);
        count++;
    }

    /**
     * removes the first element.
     * throw when the container is empty.
     */
    void pop_front() {
        if (empty()) {
            throw container_is_empty();
        }
        node *tmp = head->next;
        erase(tmp);
        delete tmp;
        count--;
    }

    /**
     * sort the values in ascending order with operator< of T
     */
    void sort() {
        if (count <= 1) return;

        // Copy data to array
        T **arr = new T*[count];
        size_t idx = 0;
        for (node *p = head->next; p != tail; p = p->next) {
            arr[idx++] = p->data;
        }

        // Sort using provided algorithm
        sjtu::sort<T*>(arr, arr + count, [](const T *a, const T *b) {
            return *a < *b;
        });

        // Copy back to list
        idx = 0;
        for (node *p = head->next; p != tail; p = p->next) {
            p->data = arr[idx++];
        }

        delete[] arr;
    }

    /**
     * merge two sorted lists into one (both in ascending order)
     * compare with operator< of T
     * container other becomes empty after the operation
     * for equivalent elements in the two lists, the elements from *this shall always precede the elements from other
     * the order of equivalent elements of *this and other does not change.
     * no elements are copied or moved
     */
    void merge(list &other) {
        if (this == &other) return;

        node *p1 = head->next;
        node *p2 = other.head->next;

        while (p2 != other.tail) {
            // Find position in this list to insert p2
            while (p1 != tail && !(*(p2->data) < *(p1->data))) {
                p1 = p1->next;
            }

            // Remove p2 from other list
            node *next_p2 = p2->next;
            other.erase(p2);

            // Insert p2 before p1 in this list
            insert(p1, p2);

            p2 = next_p2;
        }

        count += other.count;
        other.count = 0;
    }

    /**
     * reverse the order of the elements
     * no elements are copied or moved
     */
    void reverse() {
        if (count <= 1) return;

        node *p = head->next;
        while (p != tail) {
            node *tmp = p->prev;
            p->prev = p->next;
            p->next = tmp;
            p = p->prev;  // p->prev is actually the next node now
        }

        // Swap head and tail connections
        node *tmp = head->next;
        head->next = tail->prev;
        tail->prev = tmp;

        // Fix head and tail
        head->next->prev = head;
        tail->prev->next = tail;
    }

    /**
     * remove all consecutive duplicate elements from the container
     * only the first element in each group of equal elements is left
     * use operator== of T to compare the elements.
     */
    void unique() {
        if (count <= 1) return;

        node *p = head->next;
        while (p != tail && p->next != tail) {
            if (*(p->data) == *(p->next->data)) {
                node *tmp = p->next;
                erase(tmp);
                delete tmp;
                count--;
            } else {
                p = p->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
