//
// Created by omer on 04/06/2026.
//

#pragma once
#include <math.h>
#include <stdexcept>
using namespace std;
#include <memory>
using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;
using std::make_shared;
using std::weak_ptr;

#ifndef HW1WET_AVL_H
#define HW1WET_AVL_H
template<typename T>
class Avl {
public: struct node {
        T data;
        int id;
        node* parent;
        node* left;
        node* right;
        int height;
        node *next;
        node *prev;
        node() = default;
        explicit node(T data, int id, node* parent=nullptr, node* left=nullptr,node* right=nullptr)
         :data(move(data)),id(id),parent(parent),left(left),right(right),  height(0), next(nullptr),prev(nullptr) {}
        ~node() {
            this->prev = nullptr;
            this->next = nullptr;
            delete this->right;
            delete this->left;
        }
        bool isLeaf() const{
            if (this->left || this->right){
                return false;
            }
            return true;
        }

        T get_data() const {
            return data;
        }
        int getBalanceFactor() const {
            int leftH= this->left ? left->height : -1;
            int rightH= this->right ? right->height : -1;
            return leftH-rightH;
        }
        void updateHeight() {
            int leftH = this->left ? this->left->height : -1;
            int rightH = this->right ? this->right->height : -1;
            this->height = max(leftH, rightH) + 1;
        }
    };
private:
        node* root=nullptr;

        void roll_RR(node* rolled) {
            if (!rolled || !rolled->left) {
                throw std::invalid_argument("node not found");
            }

            if (rolled->parent) {
                if (rolled->parent->left == rolled) {
                    rolled->parent->left = rolled->left;
                }
                else {
                    rolled->parent->right = rolled->left;
                }
            }
            else {
                this->root=rolled->left;
            }
            rolled->left->parent = rolled->parent;
            node* temp = rolled->left;
            if (rolled->left->right) {
                rolled->left->right->parent = rolled;
            }
            rolled->left = rolled->left->right;
            temp->right = rolled;
            rolled->parent = temp;

            rolled->updateHeight();
            temp->updateHeight();
            if (this->root) {
                this->root->parent = nullptr;
            }
        }
        void roll_LL(node* rolled) {
            if (!rolled || !rolled->right) {
                throw std::invalid_argument("node not found");
            }
            if (rolled->parent) {
                if (rolled->parent->left == rolled) {
                    rolled->parent->left = rolled->right;
                } else {
                    rolled->parent->right = rolled->right;
                }
            } else {
                this->root = rolled->right;
            }
            rolled->right->parent = rolled->parent;
            node *temp = rolled->right;
            if (rolled->right->left) {
                rolled->right->left->parent = rolled;
            }
            rolled->right = rolled->right->left;
            temp->left = rolled;
            rolled->parent = temp;

            rolled->updateHeight();
            temp->updateHeight();
            if (this->root){
                this->root->parent = nullptr;
            }
        }
        void roll_LR(node* rolled) {
            if (!rolled || !rolled->left) {
                return;
            }
            roll_LL(rolled->left);
            roll_RR(rolled);
        }
        void roll_RL(node* unbalanced) {
            if (!unbalanced || !unbalanced->right) {
                return;
            }
            roll_RR(unbalanced->right);
            roll_LL(unbalanced);
        }
        static node* seekBottom(node* start) {
            //finds and returns the given node if it has <2 children or next inorder otherwise
            if(!start->left || !start->right){
                return start;
            }
            start = start->right;
            while (start->left){
                start = start->left;
            }
            return start;
        }
        void treeToNode(node* climber, node*& last, node*& parent) {
            if (!climber) {
                return;
            }
            treeToNode(climber->left,last,parent);
            if (last) {
                last->right=climber;
                climber->left=last;
            }
            else {
                parent=climber;
            }
            last=climber;
            treeToNode(climber->right,last,parent);
        }

        node* mergeNodes(node* flat1, node* flat2) {
            if (!flat1) {
                return flat2;
            }
            if (!flat2) {
                return flat1;
            }

            node temp;
            node* runner=&temp;

            while (flat1 && flat2) {
                if (flat1->id < flat2->id) {
                    runner->right=flat1;
                    runner->next=flat1;
                    flat1->left=runner;
                    flat1->prev=runner;
                    flat1=flat1->right;
                } else {
                    runner->right=flat2;
                    runner->next=flat2;
                    flat2->left=runner;
                    flat2->prev=runner;
                    flat2=flat2->right;
                }
                runner=runner->right;
            }

            node* leftOvers=flat1 ? flat1 : flat2;
            runner->right=leftOvers;
            runner->next=leftOvers;
            if (leftOvers) {
                leftOvers->left=runner;
                leftOvers->prev=runner;
            }

            node* head=temp.right;
            if (head) {
                head->left=nullptr;
                head->prev=nullptr;
            }
            temp.right = nullptr;
            temp.left = nullptr;
            return head;
        }

        node* nodeToTree(node*& climber,int n,node* parent) {
           if (!climber) {
               return nullptr;
           }
            if (n<=0) {
                return nullptr;
            }
            node* left=nodeToTree(climber,n/2,nullptr);
            node* rootParent=climber;
            climber=climber->right;
            rootParent->parent=parent;
            rootParent->left=left;
            if (left) {
                left->parent=rootParent;
            }
            rootParent->right=nodeToTree(climber,n-(n/2)-1,rootParent);
            rootParent->updateHeight();
            return rootParent;

        }

    public:
        Avl()=default; //init
        ~Avl() {
            delete this->root;
        }
        node* getListHead()const{
            if(!root){
                return nullptr;
            }
            auto diver = root;
            while (diver->left){
                diver = diver->left;
            }
            return diver;
        }
        node* findNode(int id){
            node* climber=this->root;
            while (climber && climber->id!=id) {
                if (climber->id<id) {
                    climber=climber->right;
                }
                else {
                    climber=climber->left;
                }
            }
            return climber;
        }
        void insert(int id, T data) {

            if (!this->root) {
                this->root=new node(move(data),id);
                return;
            }
            if (findNode(id)) {
                return;
            }

            node* climber=this->root;
            while (true) {
                if (id<climber->id) {
                    if (!climber->left ) {
                        climber->left=new node(move(data),id,climber);
                        node* newNode = climber->left;
                        newNode->next = climber;
                        newNode->prev = climber->prev;
                        if (climber->prev) {
                            climber->prev->next = newNode;
                        }
                        climber->prev = newNode;
                        climber=climber->left;
                        break;
                    }
                    climber=climber->left;
                }
                else {
                    if (!climber->right) {
                        climber->right=new node(move(data),id,climber);
                        node* newNode = climber->right;
                        newNode->prev = climber;
                        newNode->next = climber->next;
                        if (climber->next) {
                            climber->next->prev = newNode;
                        }
                        climber->next = newNode;
                        climber=climber->right;
                        break;
                    }
                    climber=climber->right;
                }
            }

            while (climber->parent) {
                if (climber->height<climber->parent->height) {
                    return;
                }
                climber->parent->height=climber->height+1;

                int bf=climber->parent->getBalanceFactor();
                if (abs(bf)<2) {
                    climber=climber->parent;
                    continue;
                }
                int bfc;
                if (bf==2) {
                    bfc=climber->parent->left->getBalanceFactor();
                    if (bfc==-1) {
                        roll_LR(climber->parent);
                        return;
                    }
                    if (bfc>=0) {
                        roll_RR(climber->parent);
                        return;
                    }
                    throw runtime_error("error 117");
                }
                if (bf==-2) {
                    bfc=climber->parent->right->getBalanceFactor();
                    if (bfc==1) {
                        roll_RL(climber->parent);
                        return;
                    }
                    if (bfc<=0) {
                        roll_LL(climber->parent);
                        return;
                    }
                    throw runtime_error("error 128");
                }
                climber=climber->parent;
            }

        }

        void remove(int id) {
            node *target = findNode(id);
            if (!target) {
                return;
            }

            node *newPosition = seekBottom(target);
            if (newPosition->prev) {
                newPosition->prev->next = newPosition->next;
            }
            if (newPosition->next) {
                newPosition->next->prev = newPosition->prev;
            }

            if (newPosition != target) {    //swap
                target->id = newPosition->id;
                target->data = move(newPosition->data);
            }
            if (!newPosition->parent) {
                this->root = newPosition->left ? newPosition->left : newPosition->right;
                if (this->root) {
                    this->root->parent = nullptr;
                }
            }
            else {
                bool divergeRight = (newPosition->parent->right == newPosition);
                if(newPosition->isLeaf()){
                    if(divergeRight){
                        newPosition->parent->right = nullptr;
                    }
                    else{
                        newPosition->parent->left = nullptr;
                    }
                }else if(newPosition->right){
                    if(divergeRight){
                        newPosition->parent->right = newPosition->right;
                        newPosition->right->parent = newPosition->parent;
                    }
                    else{
                        newPosition->parent->left = newPosition->right;
                        newPosition->right->parent = newPosition->parent;
                    }
                }else{
                    if(divergeRight){
                        newPosition->parent->right = newPosition->left;
                        newPosition->left->parent = newPosition->parent;
                    }
                    else{
                        newPosition->parent->left = newPosition->left;
                        newPosition->left->parent = newPosition->parent;
                    }

                }
            }
            node* climber = newPosition->parent;
            newPosition->left = nullptr;    //disconnect to work with dtor
            newPosition->right = nullptr;
            newPosition->next = nullptr;
            newPosition->prev = nullptr;
            delete newPosition;
            while(climber){
                int leftHeight = -1, rightHeight = -1;
                if (climber->left){
                    leftHeight = climber->left->height;
                }
                if(climber->right){
                    rightHeight = climber->right->height;
                }
                climber->height = max(leftHeight, rightHeight) + 1;
                int currentBalance = climber->getBalanceFactor();
                if(currentBalance == 2){
                    if(climber->left->getBalanceFactor() >= 0){
                        roll_RR(climber);
                    }
                    else if (climber->left->getBalanceFactor()== -1){
                        roll_LR(climber);
                    }
                    else {
                        throw std::logic_error("Wrong roll classification");
                    }
                }
                else if(currentBalance == -2){
                    if(climber->right->getBalanceFactor() <= 0 ){
                        roll_LL(climber);
                    }
                    else if (climber->right->getBalanceFactor() ==1){
                        roll_RL(climber);
                    }
                    else {
                        throw std::logic_error("Wrong roll classification");
                    }
                }
                climber = climber->parent;
            }

        }

        T* find(int id) {
            node* target=findNode(id);
            if (!target){
                return nullptr;
            }
            return &(target->data);
        }
        void merge(Avl& other) {        // doesnt include prev,next treatment
            if (!other.root) {
                return;
            }
            if (!this->root) {
                this->root=other.root;
                other.root=nullptr;
                return;
            }

            node* head1=nullptr;
            node* prev1=nullptr;
            treeToNode(this->root,prev1,head1);

            node* head2=nullptr;
            node* prev2=nullptr;
            treeToNode(other.root,prev2,head2);

            node* merger=mergeNodes(head1,head2);

            int nTotal=0;
            node* temp=merger;
            while (temp) {
                nTotal++;
                temp=temp->right;
            }
            this->root=nodeToTree(merger,nTotal,nullptr);
            other.root=nullptr;
        }

    };
#endif //HW1WET_AVL_H