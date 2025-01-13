#pragma once

#include <memory>

template<typename T>
struct Node {
    T item;
    std::unique_ptr<Node> next;
};

template<typename T>
class List {
public:
    List() noexcept
        : m_head{nullptr} {}

    List(const List&) = delete;
    List(List&&) = delete;
    List& operator==(const List&) = delete;
    List& operator==(List&&) = delete;

    bool empty() const noexcept {
        return m_head == nullptr;
    }

    void insert(std::unique_ptr<T> node) noexcept {
        if (node == nullptr) {
            return;
        }

        auto new_node = std::make_unique<Node<T>>(std::move(*node), nullptr);
        new_node->next = std::move(m_head);
        m_head = new_node;
    }

    template<typename Pred>
    void erase_if(Pred pred) {
        Node<T>* previous = nullptr;
        Node<T>* node = m_head.get();

        while (node != nullptr) {
            if (pred(node)) {
                node = node->next;
                if (previous != nullptr) {
                    previous->next = node;
                } else {
                    m_head = node;
                }
            } else {
                previous = node;
                node = node->next;
            }
        }
    }

private:
    std::unique_ptr<Node<T>> m_head;
};
