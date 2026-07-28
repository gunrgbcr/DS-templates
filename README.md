# Custom C++ Data Structures Library

Three header-only C++ data structures written from scratch as course assignments: an AVL tree, a double-hashing hash table, and a union-find built on top of them.
Keys are `int`; stored values are a template parameter. No standard-library containers are used: the only standard headers included are `<memory>`, `<stdexcept>`, `<math.h>` and `<cassert>`.

---

## Structures Included

### 1. AVL Tree (`Avl.h`)
`Avl<T>`, a self-balancing binary search tree keyed by `int id`.
*   **Operations:** `insert`, `remove`, `find`, and `merge`, which combines two trees by flattening both in order, merging the sorted lists and rebuilding a balanced tree.
*   **Balancing:** tracks node heights and balance factors and rebalances with single and double rotations after every insert and remove, keeping these operations O(log n).

### 2. Hash Table (`hash_table.h`)
`hash_table<V>`, open addressing with double hashing over `int` keys.
*   **Probing:** slot `(h(x) + k*r(x)) % capacity`, with `h(x) = x % capacity` and `r(x) = 1 + x % (capacity - 2)`.
*   **Resizing:** capacities come from a fixed table of primes; when occupancy exceeds a load factor of 0.75 the table moves to the next prime and re-inserts every entry.
*   **Ownership:** each value is held in a `std::unique_ptr<V>`; copying the table is disabled.

### 3. Union-Find (`unionFind.h`)
A disjoint-set structure built on the two containers above.
*   **Union by size**, and **path compression** when finding a set's root.
*   One AVL tree maps public set IDs to internal IDs, another stores the sets, and members are stored in the hash table.
*   Operations: add a set, add a member, merge two sets, find a member's set, and remove a set (marked inactive).

---

## Memory Management

Tree nodes, the hash table's slot array and union-find records are allocated with `new` and released with `delete` / `delete[]` in the destructors.