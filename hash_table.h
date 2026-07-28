//
// Created by omer on 13/07/2026.
// V2: Uses shared_ptr<V> to avoid copies on resize,
//     removes default-constructibility requirement on V,
//     and eliminates dangling-reference risk.
//

#ifndef HW2WET_HASH_TABLE_H
#define HW2WET_HASH_TABLE_H
#include <stdexcept>
#include <memory>
#include <cassert>

using std::out_of_range;
using std::unique_ptr;

constexpr static int primes[] = {
    13,         29,         53,         97,
    193,        389,        769,        1543,
    3079,       6151,       12289,      24593,
    49157,      98317,      196613,     393241,
    786433,     1572869,    3145739,    6291469,
    12582917,   25165843,   50331653,   100663319,
    201326611,  402653189,  805306457,  1610612741
};

template <class V>
class hash_table {
    struct node {
        int secondId;
        int key;
        unique_ptr<V> value;
        bool occupied;

        node():secondId(0),key(0),value(nullptr),occupied(false){};
        ~node() = default;
        node(int key, unique_ptr<V> value) :secondId(0),key(key), value(move(value)), occupied(false) {}

        int getSecondId() const {
            return secondId;
        }
        void setSecondId(int newId) {
            secondId=newId;
        }
    };

    node* arr;
    int prime_counter = 0;
    int capacity;
    int occupiedSpace;
    constexpr static float load_factor = 0.75;
    constexpr static int step_mod = 2;

    void resize() {
        int temp_cap = capacity;
        prime_counter++;
        capacity = primes[prime_counter];
        node* arr2 = new node[capacity];
        node* temp = arr;
        arr = arr2;
        occupiedSpace = 0;
        for (int i = 0; i < temp_cap; i++) {
            if (temp[i].occupied) {
                this->insert(temp[i].key,temp[i].secondId,move( temp[i].value));
            }
        }
        delete[] temp;
}
    int h(int x) const {
        return x % capacity;
    }

    int r(int x) const {
        return 1 + (x % (capacity - step_mod));
    }

    int insertHashed(int x, int k) const {
        if (k == capacity) {
            throw out_of_range("table full");
        }
        int target = (h(x) + k * r(x)) % capacity;
        assert(target >= 0);
        if (!arr[target].occupied) {
            return target;
        }
        return insertHashed(x, k + 1);
    }

    int insert_hash(int key) const {
        return insertHashed(key, 0);
    }

    int searchHashed(int x, int k) const {
        if (k == capacity) {
            throw out_of_range("not found");
        }
        int target = (h(x) + k * r(x)) % capacity;
        if (!arr[target].occupied) {
            throw out_of_range("not found");
        }
        if (arr[target].key == x) {
            return target;
        }
        return searchHashed(x, k + 1);
    }

    int search_hash(int key) const {
        return searchHashed(key, 0);
    }

public:
    hash_table(const hash_table&) = delete;
    hash_table& operator=(const hash_table&) = delete;
    
    hash_table() : capacity(primes[prime_counter]), occupiedSpace(0) {
        arr = new node[capacity];
    }

    ~hash_table() {
        delete[] arr;
    }

    void insert(int key, int secondId, unique_ptr<V> value) {
        int hashed = insert_hash(key);
        arr[hashed].key = key;
        arr[hashed].value = move(value);
        arr[hashed].secondId = secondId;
        arr[hashed].occupied = true;
        occupiedSpace++;
        if (load_factor * capacity < occupiedSpace) {
            resize();
        }
    }
    V* get_value(int key) const {
        return arr[search_hash(key)].value.get();
    }

    int find_key(int id) const {
        return arr[search_hash(id)].secondId;
    }



};

#endif //HW2WET_HASH_TABLE_H
