#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#define STORE_FILE "store/dumpFile"
std::mutex mtx;     // mutex for critical section
std::string delimiter = ":";
//节点模板类
template<typename Key,typename Value>
class Node{
public:
    Node(){}
    Node(Key,Value, int);
    ~Node();
    Key get_key() const;
    Value get_value() const;
    void set_value(Value);
    Node ** forward;
    int node_level;
private:
    Key mKey;
    Value mValue;
};
template<typename Key,typename Value> 
Node<Key,Value>::Node(const Key key,const Value value,int level){
    this->mKey=key;
    this->mValue=value;
    this->node_level=level;
    this ->forward= new Node *[level+1];
    memset(this->forward, 0, sizeof(Node<Key,Value>*)*(level+1));
};
template<typename Key,typename Value> 
Node<Key,Value>::~Node(){
    delete []forward;
};
template<typename Key,typename Value> 
Key Node<Key,Value>::get_key()const{
    return mKey;
};
template<typename Key,typename Value> 
Value Node<Key,Value>::get_value()const{
    return mValue;
};
template<typename Key,typename Value> 
void Node<Key,Value>::set_value(Value value){
    this->mValue =value;
};
template<typename Key,typename Value>
class SkipList{
public:
    SkipList(int);//
    ~SkipList();//
    int insert_element(Key, Value);//
    int get_random_level();//
    Node<Key,Value>* create_node(Key, Value, int);//
    void display_list();//
    bool search_element(Key);//
    void delete_element(Key);//
    void dump_file();//
    void load_file();//
    int size()const;//
private:
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);//
    bool is_valid_string(const std::string& str);//
    int max_level;
    int skip_list_level;
    Node<Key, Value> *header;
    std::ofstream _file_writer;
    std::ifstream _file_reader;
    int element_count;
};
template<typename Key,typename Value> 
Node<Key,Value>* SkipList<Key,Value>::create_node(Key key,Value value,int level){
    Node<Key,Value>* n=new Node<Key,Value>(key ,value,level);
    return n;
};
template<typename Key,typename Value> 
int SkipList<Key,Value>::insert_element(Key key,Value value){
    mtx.lock();
    Node<Key,Value> *current =this->header;
    Node<Key,Value> *update[max_level+1];
    for (int i = 0; i <= max_level; i++)
            update[i] = NULL;
    for(int i=skip_list_level;i>=0;i--){//从最高等级往下
        while(current->forward[i]!=nullptr&&current->forward[i]->get_key()<key){//当key<target时候 current右移
            current=current->forward[i];
        }
        update[i]=current;
    }
    current = current->forward[0];//找到插入的点
    if(current!=nullptr&&current->get_key()==key){//元素已经存在
        
        std::cout<<key<<"元素已经存在"<<std::endl;
        mtx.unlock();
        return 1;}
    if(current == NULL || current->get_key() != key )//元素不存在
    {
        int randomlevel =get_random_level();
        if(randomlevel>skip_list_level)
        //random is greater than now level to make the updata[] bigger
        {
            for(int i=skip_list_level+1;i<randomlevel+1;i++){
                update[i]=header;
            }
            skip_list_level=randomlevel;
        }
        Node<Key, Value>* inserted_node = create_node(key, value, randomlevel);
        for(int i=0;i<randomlevel;i++){
            inserted_node->forward[i]=update[i]->forward[i];
            update[i]->forward[i]=inserted_node;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        element_count ++;
    }
    mtx.unlock();
    return 0;
};
template<typename Key, typename Value> 
void SkipList<Key,Value>::display_list() {

    std::cout << "\n*****Skip List*****"<<"\n"; 
    for (int i = 0; i < skip_list_level+1; i++) {
        Node<Key,Value> *node = this->header->forward[i]; 
        std::cout << "Level " << i << ": ";
        while (node != NULL) {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

// Get current SkipList size 
template<typename Key, typename Value> 
int SkipList<Key, Value>::size() const{ 
    return element_count;
}
template<typename Key, typename Value> 
int SkipList<Key, Value>::get_random_level(){
    int k=1;
    while (rand()%3)
    {
        k++;
    }
    k=(k<max_level)?k:max_level;
    return k;
};
template<typename Key,typename Value>
bool SkipList<Key, Value>::search_element(Key key){
    std::cout << "search_element-----------------" << std::endl;
    Node<Key,Value>* current =header;
    for(int i=skip_list_level;i>=0;i--){
        while (current->forward[i]&&current->forward[i]->get_key()<key)
        {
            current=current->forward[i];
        }
    }
    current=current->forward[0];
    if(current!=nullptr&&current->get_key()==key){
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }
    std::cout << "Not Found Key:" << key << std::endl;
    return false;
};
template<typename Key,typename Value>
SkipList<Key, Value>::SkipList(int maxlevel){
    this->max_level=maxlevel;
    this->skip_list_level=0;
    this->element_count=0;
    Key key;
    Value value;
    this->header=new Node<Key,Value>(key,value,maxlevel);
};
template<typename Key,typename Value>
void SkipList<Key, Value>::delete_element(Key key){
    std::cout<<"Deleting"<<std::endl;
    mtx.lock();
    Node<Key,Value>*current =header;
    Node<Key,Value>*deleter =header;
    Node<Key,Value> *update[max_level+1];
    memset(update, 0, sizeof(Node<Key, Value>*)*(max_level+1));
    for(int i=skip_list_level;i>=0;i--){
        while (current->forward[i]!=nullptr&&current->forward[i]->get_key()<key)
        {
            current=current->forward[i];
        }
        update[i]=current;
    }
    current=current->forward[0];
    if(current!=nullptr&current->get_key()==key){
        deleter=current;
        for(int i=0;i<skip_list_level+1;i++){
            if (update[i]->forward[i]!=current)
            {
                break;
            }
            update[i]->forward[i]=current->forward[i];
            
        }
        while(skip_list_level>0&&header->forward[skip_list_level]==nullptr)
            skip_list_level--;
        delete(deleter);
        std::cout << "Successfully deleted key "<< key << std::endl;
        element_count --;
    }
    mtx.unlock();
    return ;
};
template<typename Key,typename Value>
SkipList<Key, Value>::~SkipList(){

    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
    delete header;
};
template<typename Key,typename Value>
bool SkipList<Key, Value>::is_valid_string(const std::string& str) {

    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
};

template<typename Key,typename Value>
void SkipList<Key, Value>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {

    if(!is_valid_string(str)) {
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
};
template<typename Key,typename Value>
void SkipList<Key, Value>::dump_file() {

    std::cout << "dump_file-----------------" << std::endl;
    _file_writer.open(STORE_FILE);
    Node<Key, Value> *node = this->header->forward[0]; 

    while (node != NULL) {
        _file_writer << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
    return ;
};

template<typename Key,typename Value>
void SkipList<Key, Value>::load_file() {

    _file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();
    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insert_element(*key, *value);
        std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    _file_reader.close();
};

