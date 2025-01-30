#include <unordered_map>
#include <mutex>
#include <stdexcept>

template <typename Key, typename Value>
class LFUCache {
private:
    struct Node {
        Key key;
        Value value;
        int freq;
        Node* prev;
        Node* next;
        Node(Key k, Value v) : key(k), value(v), freq(1), prev(nullptr), next(nullptr) {}
    };

    size_t capacity;
    int min_freq;
    std::unordered_map<Key, Node*> key_map;
    std::unordered_map<int, std::pair<Node*, Node*>> freq_map;
    std::mutex mutex;

    void add_to_freq_list(Node* node, int freq) {
        if (freq_map.find(freq) == freq_map.end()) {
            Node* head = new Node(Key(), Value());
            Node* tail = new Node(Key(), Value());
            head->next = tail;
            tail->prev = head;
            freq_map[freq] = {head, tail};
        }
        Node* head = freq_map[freq].first;
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    void remove_from_freq_list(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void increment_freq(Node* node) {
        int old_freq = node->freq;
        remove_from_freq_list(node);
        if (freq_map[old_freq].first->next == freq_map[old_freq].second) {
            freq_map.erase(old_freq);
            if (min_freq == old_freq) min_freq++;
        }
        node->freq++;
        add_to_freq_list(node, node->freq);
    }

public:
    LFUCache(size_t cap) : capacity(cap), min_freq(1) {}

    ~LFUCache() {
        for (auto& pair : freq_map) {
            Node* head = pair.second.first;
            Node* tail = pair.second.second;
            Node* current = head->next;
            while (current != tail) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
            delete head;
            delete tail;
        }
        for (auto& pair : key_map) {
            delete pair.second;
        }
    }

    Value get(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = key_map.find(key);
        if (it == key_map.end()) {
            throw std::out_of_range("Key not found");
        }
        Node* node = it->second;
        increment_freq(node);
        return node->value;
    }

    void put(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(mutex);
        if (capacity == 0) return;

        auto it = key_map.find(key);
        if (it != key_map.end()) {
            Node* node = it->second;
            node->value = value;
            increment_freq(node);
        } else {
            if (key_map.size() >= capacity) {
                Node* head = freq_map[min_freq].first;
                Node* tail = freq_map[min_freq].second;
                Node* to_remove = tail->prev;
                if (to_remove != head) {
                    key_map.erase(to_remove->key);
                    remove_from_freq_list(to_remove);
                    delete to_remove;
                    if (head->next == tail) {
                        freq_map.erase(min_freq);
                    }
                }
            }
            Node* new_node = new Node(key, value);
            key_map[key] = new_node;
            add_to_freq_list(new_node, 1);
            min_freq = 1;
        }
    }
};