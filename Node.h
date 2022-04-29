//
// Created by zyz on 2022/4/29.
//

#ifndef SKIPLIST_NODE_H
#define SKIPLIST_NODE_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
template<typename K, typename V>
class Node{

public:
    Node(){}

    Node(K k ,V v ,int _level):key(k),value(v),node_level(_level){

        /// 注意  level为0~level
        forward = new Node<K,V> *[_level +1];

        memset(this->forward,0,sizeof(Node<K,V>*) *(_level +1));
    }

    ~Node(){
        delete []forward;
    }

    K get_key() const{
        return key;
    }

    V get_value() const{
        return value;
    }

    void set_value(V _value){
        value = _value;
    }

    Node<K,V> **forward;

    int node_level;

private:
    K key;
    V value;
};

#endif //SKIPLIST_NODE_H
