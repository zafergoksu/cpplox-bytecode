#pragma once

#include <cstddef>
#include <iterator>
namespace ds {

template<typename T>
struct Node {
    T* item;
    Node* next;
};

template<typename T, bool Owned = true>
class List {
public:
    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Node<T>;
        using difference_type = std::ptrdiff_t;
        using pointer = Node<T>*;
        using reference = Node<T>&;

        explicit iterator(Node<T>* current) noexcept : m_current{current} {}

        operator Node<T>*() noexcept { return m_current; }
        Node<T>* operator->() noexcept { return m_current; }

        iterator& operator++() noexcept {
            if (m_current != nullptr) {
                m_current = m_current->next;
            }

            return *this;
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs) {
            return lhs.m_current != rhs.m_current;
        }

    private:
        Node<T>* m_current;
    };

    List() noexcept
        : m_head{nullptr}, m_size{0} {}

    List(const List&) = delete;
    List(List&&) = delete;
    List& operator=(const List&) = delete;
    List& operator=(List&&) = delete;

    ~List() {
        if constexpr (Owned) {
            while (m_head) {
                auto erase_node = m_head;
                m_head = m_head->next;
                delete erase_node->item;
                delete erase_node;
            }
        }
    }

    iterator begin() const noexcept {
        return iterator{m_head};
    }

    iterator end() const noexcept {
        return iterator{nullptr};
    }

    bool empty() const noexcept {
        return m_head == nullptr;
    }

    std::size_t size() const noexcept {
        return m_size;
    }

    void insert(T* item) noexcept {
        auto new_node = new Node{item, m_head};
        m_head = new_node;
        ++m_size;
    }

    template<typename Pred>
    void erase_if(Pred pred) {
        Node<T>* previous = nullptr;
        Node<T>* node = m_head;

        while (node != nullptr) {
            if (pred(node)) {
                auto erase_node = node;
                node = node->next;
                if (previous != nullptr) {
                    previous->next = node;
                } else {
                    m_head = node;
                }
                delete erase_node->item;
                delete erase_node;
                --m_size;
            } else {
                previous = node;
                node = node->next;
            }
        }
    }

private:
    Node<T>* m_head;
    std::size_t m_size;
};

} // namespace ds
