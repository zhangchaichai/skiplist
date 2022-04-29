//
// Created by zyz on 2022/4/29.
//

#ifndef SKIPLIST_SKIPLIST_H
#define SKIPLIST_SKIPLIST_H
#include <cstddef>
#include <cassert>
#include <ctime>
#include "Node.h"

#define STORE_FILE "D:\\projects\\skiplist\\File\\skiplistFile.txt"

std::mutex mtx;     // mutex for critical section
template<typename K, typename V>
class SkipList{

public:
    SkipList(int max_level)
    :m_max_level(max_level),m_skip_list_level(0),
    m_element_count(0){
        K k;
        V v;
        m_header = new Node<K, V>(k, v, m_max_level);
    }
    ~SkipList(){
        if (m_file_writer.is_open()) {
            m_file_writer.close();
        }
        if (m_file_reader.is_open()) {
           m_file_reader.close();
        }
        delete m_header;
    }
    int get_random_level(){
        int k = 0;
        while (rand() % 2) {
            k++;
        }
        k = (k < m_max_level) ? k : m_max_level;
        return k;
    }
    Node<K, V>* create_node(K, V, int);
    int insert_element(K, V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    void dump_file();
    void load_file();
    int size(){
        return m_element_count;
    }

private:
    // 在str中获取 key value
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value){
        if(!is_valid_string(str)) {
            return;
        }
        *key = str.substr(0, str.find(':'));
        *value = str.substr(str.find(':')+1, str.length());
    }
    // 判断读入的字符串是否合法：即是否存在key 和 value
    bool is_valid_string(const std::string& str){
        if (str.empty()) {
            return false;
        }
        if (str.find(':') == std::string::npos) {
            return false;
        }
        return true;
    }

private:
    // 跳表的最大的level
    int m_max_level;

    //跳表当前的level
    int m_skip_list_level;

    //跳表的头节点指针
    Node<K,V> *m_header;

    // file operator
    std::ofstream m_file_writer;
    std::ifstream m_file_reader;

    // skiplist current element count
    int m_element_count;

};

// create new node
template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V> *now = new Node<K, V>(k, v, level);
    return now;
}

template<typename K,typename V>
int SkipList<K,V>::insert_element(const K key, const V value) {
    mtx.lock();
    Node<K,V> *current = m_header;

    Node<K,V> *update[m_max_level+1];
    memset(update,0,sizeof(Node<K, V>*)*(m_max_level+1));

    for(int i = m_skip_list_level;i >= 0; i--){
        while(current->forward[i] != NULL && current->forward[i]->get_key() < key){
            current = current->forward[i];
        }
        update[i] = current;
    }

    // 第0层
    current = current->forward[0];

    if(current != NULL && current->get_key() == key){
        std::cout << "key: " << key << ", exists" << std::endl;
        mtx.unlock();
        return 1;
    }

    if(current == NULL || current->get_key() != key){

        int random_level = get_random_level();
        //需要增加层数
        if(random_level > m_skip_list_level){
            for(int i = m_skip_list_level + 1 ; i < random_level +1 ;i++){
                update[i] = m_header;
            }
            m_skip_list_level = random_level;
        }

        Node<K, V>* inserted_node = create_node(key, value, random_level);

        for(int i = 0 ;i <= random_level ; i++){
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        m_element_count ++;
    }
    mtx.unlock();
    return 0;
}

//展示整个跳表
template<typename  K,typename V>
void SkipList<K,V>::display_list() {
    std::cout << "\n*****Skip List*****"<<"\n";
    for (int i = m_skip_list_level; i>= 0; i--) {
        Node<K, V> *node = m_header->forward[i];
        std::cout << "Level " << i << ": ";
        while (node != NULL) {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

// 写入文件
template<typename K, typename V>
void SkipList<K, V>::dump_file() {

    std::cout << "dump_file-----------------" << std::endl;
    m_file_writer.open(STORE_FILE);
    Node<K, V> *node = m_header->forward[0];

    while (node != NULL) {
        m_file_writer << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    m_file_writer.flush();
    m_file_writer.close();
    return ;
}

// 读文件
// Load data from disk
template<typename K, typename V>
void SkipList<K, V>::load_file() {

    m_file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();
    while (getline(m_file_reader, line)) {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insert_element(*key, *value);
        std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    m_file_reader.close();
}

template<typename K ,typename  V>
void SkipList<K,V>::delete_element(K key) {
    mtx.lock();
    Node<K, V> *current = m_header;
    Node<K, V> *update[m_max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(m_max_level+1));
// 和插入一样 寻找每一层比key小的第一个节点
    for (int i = m_skip_list_level; i >= 0; i--) {
        while (current->forward[i] !=NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    //只有current->get_key()==key  才会删除
    if (current != NULL && current->get_key() == key){
        for(int i = 0 ;i < m_skip_list_level + 1 ; i++){
            // 有一层没有key，则往上均不存在key
            if(update[i]->forward[i] != current){
                break;
            }
            update[i]->forward[i] = current->forward[i];
        }
        // 删除节点后判断 层数会不会减少

        while (m_skip_list_level > 0 && m_header->forward[m_skip_list_level] == 0) {
            m_skip_list_level --;
        }
        std::cout << "Successfully deleted key "<< key << std::endl;
        m_element_count --;
    }
    mtx.unlock();
    return;
}

template<typename K, typename V>
bool SkipList<K, V>::search_element(K key){
    std::cout << "search_element-----------------" << std::endl;
    Node<K,V> *current = m_header;

    //同插入 和 删除
    for (int i = m_skip_list_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }
    current = current->forward[0];

    if (current and current->get_key() == key) {
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }

    std::cout << "Not Found Key:" << key << std::endl;
    return false;
}


#endif //SKIPLIST_SKIPLIST_H
