



#ifndef HW2WET_UNIONFIND_H
#define HW2WET_UNIONFIND_H
#include "hash_table.h"
#include "Avl.h"

template<class T>
class record;

template<class T>
class set {
protected:
    set* parent=this;
    record<T>* groupInfo;
    int& getSize();
public:
    bool active = true;
    set()=default;
    set(int publicId);
    virtual set* union_sets(set* i,set* j);
    virtual ~set() {
        delete groupInfo;
    }

    virtual set* findHead() {
        set* current = this;
        while (current->parent && current->parent != current) {
            current = current->parent;
        }
        set* root = current;
        current = this;
        while (current->parent && current->parent != current) {
            set* next = current->parent;
            current->parent = root;
            current = next;
        }
        return root;
    }

    virtual set* getParent() {
        return parent;
    }
   virtual void setParent(set* p) {
        parent = p;
    }

  virtual void orderBySize(set*& max,set*& min) {
        if(max->getSize()<min->getSize()){
            set* temp=min;
            min=max;
            max=temp;
            int tempId = max->groupInfo->getId();
            max->groupInfo->getId() = min->groupInfo->getId();
            min->groupInfo->getId() = tempId;
        }
    }
};

template <class T>
class record {
    int id;
    int size;
    set<T>* ptr;
public:
    record()=default;
    ~record()=default;
    record(int id,set<T>*ptr):id(id),size(1),ptr(ptr){}
    set<T>* getGroup() {
        return ptr;
    }
    int& getSize()  {
        return size;
    }
    int& getId() {
    return id;
    }

};


template<class T>
set<T>::set(int publicId) {
    this->groupInfo=new record<T>(publicId,this);
}

template <class T>
int& set<T>::getSize() {
    return this->groupInfo->getSize();
}

template<class T>
set<T>* set<T>::union_sets(set* i,set* j) {
    set* max=i->findHead();
    set* min=j->findHead();
    if(min==max){
        return max;
    }
    if (!min || !max) {
        return nullptr; //TODO throw?
    }
   orderBySize(max,min);
   min->parent=max;
    max->getSize() +=min->getSize();
    return max;
}

template <class T, template<class> class set>
class uf {
    Avl<set<T>> internal_sets;
    Avl<int> public_set_ids;
    hash_table<T> members;
    int next_internal_id = 1;
public:
    uf()=default;
    ~uf()=default;
    T* get_member(int memberId) {
        if (memberId <= 0) {
            throw std::invalid_argument("INVALID_INPUT");
        }
        return members.get_value(memberId);
    }
    set<T>* get_set(int publicSetId) {
        if (publicSetId <= 0) {
            throw std::invalid_argument("INVALID_INPUT");
        }
        int* internal_id = public_set_ids.find(publicSetId);
        if(!internal_id){
            return nullptr;
        }
        return internal_sets.find(*internal_id);
    }
    
    set<T>* find_set_of_member(int member_id) {
        if (member_id <= 0) {
            throw std::invalid_argument("INVALID_INPUT");
        }
        int key;
        try {
            key = members.find_key(member_id);
        } catch (const std::out_of_range&) {
            return nullptr;
        }
        set<T>* temp = internal_sets.find(key);
        if (!temp) {
            return nullptr;
        }
        return temp->findHead();
    }
    
    set<T>* get_original_set_of_member(int member_id) {
        if (member_id <= 0) {
            throw std::invalid_argument("INVALID_INPUT");
        }
        int key;
        try {
            key = members.find_key(member_id);
        } catch (const std::out_of_range&) {
            return nullptr;
        }
        return internal_sets.find(key);
    }
    
    void add_set(int publicSetId) {
        if (publicSetId <= 0) {
            throw std::invalid_argument("INVALID_INPUT");
        }
        if(public_set_ids.find(publicSetId)){
            throw std::invalid_argument("Set already exists");
        }
        int internal_id = next_internal_id++;
        public_set_ids.insert(publicSetId, make_unique<int>(internal_id));
        internal_sets.insert(internal_id, make_unique<set<T>>(publicSetId));
    }
    
    void add_set(int publicSetId, unique_ptr<set<T>> customSet) {
        if (publicSetId <= 0) {
            throw std::invalid_argument("INVALID_INPUT");
        }
        if(public_set_ids.find(publicSetId)){
            throw std::invalid_argument("Set already exists");
        }
        int internal_id = next_internal_id++;
        public_set_ids.insert(publicSetId, make_unique<int>(internal_id));
        internal_sets.insert(internal_id, std::move(customSet));
    }
    
    void remove_set(int publicSetId) {
        if (publicSetId <= 0) {
            throw std::invalid_argument("INVALID_INPUT");
        }
        set<T>* target = get_set(publicSetId);
        if(target && target->active){
            target->findHead()->active = false;
        } else {
            throw std::invalid_argument("Set does not exist");
        }
        public_set_ids.remove(publicSetId);
    }
    
    void add_member(int member_id, int publicSetId, T* memeber) {
        if (member_id <= 0 || publicSetId <= 0) {
            throw std::invalid_argument("INVALID_INPUT");
        }
        set<T>* target = get_set(publicSetId);
        if(!target || !target->active){
            throw std::invalid_argument("Set does not exist");
        }
        int* internal_id = public_set_ids.find(publicSetId);
        insert(member_id, internal_id, memeber);
    }
    
    void insert(int member_id, int* internal_id, T* memeber) {
        try {
            members.find_key(member_id);
            // If it succeeds, the member already exists
            throw std::invalid_argument("Member already exists");
        } catch (const std::out_of_range&) {
            // Member doesn't exist, proceed
        }
        members.insert(member_id, *internal_id, unique_ptr<T>(memeber));
    }

    set<T>* get_member_leaf_set(int member_id) {
        if (member_id <= 0) throw std::invalid_argument("INVALID_INPUT");
        int internal_id = members.find_key(member_id);
        return internal_sets.find(internal_id);
    }
    set<T>* add_member_to_set(int member_id, int publicSetId) {
        if (member_id <= 0 || publicSetId <= 0) {
            throw std::invalid_argument("INVALID_INPUT");
        }
        set<T>* merging=get_set(publicSetId);
        set<T>* merged=find_set_of_member(member_id);
        if(merging && merging->active){
            return merging->union_sets(merging,merged);
        }
        return nullptr;
    }
    
     set<T>* merge_sets(int publicSetId1,int publicSetId2) {
        if (publicSetId1 <= 0 || publicSetId2 <= 0) {
            throw std::invalid_argument("INVALID_INPUT");
        }
        if(publicSetId1 == publicSetId2){
            return get_set(publicSetId1);
        }
        set<T>* s1=get_set(publicSetId1);
        set<T>* s2=get_set(publicSetId2);
        if(!s1 || !s1->active || !s2 || !s2->active){
            throw std::invalid_argument("set not found");
        }
        s1 = static_cast<set<T>*> (s1->union_sets(s1,s2));
        public_set_ids.remove(publicSetId2);
        if(s1){
            return s1;
        }
        return nullptr;
    }

};
#endif 
