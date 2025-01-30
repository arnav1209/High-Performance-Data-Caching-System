#include <unordered_map>
#include <mutex>
#include <stdexcept>

template <typename Key, typename Value>
class LRUCache {
private:
    struct Node {
        Key key;
        Value value;
        Node* prev;
        Node* next;
        Node(Key k, Value v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };

    size_t capacity;
    Node* head;
    Node* tail;
    std::unordered_map<Key, Node*> cache_map;
    std::mutex mutex;

    void add_to_front(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    void remove_node(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void move_to_front(Node* node) {
        remove_node(node);
        add_to_front(node);
    }

    Node* remove_last() {
        Node* last = tail->prev;
        remove_node(last);
        return last;
    }

public:
    LRUCache(size_t cap) : capacity(cap) {
        head = new Node(Key(), Value());
        tail = new Node(Key(), Value());
        head->next = tail;
        tail->prev = head;
    }

    ~LRUCache() {
        Node* current = head->next;
        while (current != tail) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        delete head;
        delete tail;
    }

    Value get(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = cache_map.find(key);
        if (it == cache_map.end()) {
            throw std::out_of_range("Key not found");
        }
        Node* node = it->second;
        move_to_front(node);
        return node->value;
    }

    void put(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = cache_map.find(key);
        if (it != cache_map.end()) {
            Node* node = it->second;
            node->value = value;
            move_to_front(node);
        } else {
            if (cache_map.size() >= capacity) {
                Node* last = remove_last();
                cache_map.erase(last->key);
                delete last;
            }
            Node* new_node = new Node(key, value);
            cache_map[key] = new_node;
            add_to_front(new_node);
        }
    }
};