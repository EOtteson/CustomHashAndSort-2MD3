#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // for the swap function

/*  
    For this assignment many design decisions had to be made which are explained here. First the f1.txt 
    file is completely read to determine the number of words, which along with the load factor, is used 
    to determine the size of the hash table. This allows for varying input file sizes without wasting 
    memory or increasing collisions. The chosen load factor means that the hash table will be double the 
    size of the number of words, this was done as it minimized collisions without wasting memory. In 
    order to handle any collisions that do happen a linked list is used at that index. This could have 
    the risk of just creating a long linked list at a single index, however, this is avoided as the chosen 
    hash function spreads the values out very well and avoids clumping. This hash function, djb2, is also 
    very quick as it uses bitwise operation instead of multiplication to reduce runtime. Finally, a simple 
    quick sort algorithm is implemented to sort the data at the end as it is fast and easy to implement. 
    The runtimes were compared using the chrono library and some std sorting algorithms (qsort(), sort(), 
    and sort_heap()) which all rendered nearly identical average runtimes. Thank you! 
*/

// node sturcture used for collision handling by creating a linked list at that index in the hash table
struct Node {
    std::string key;
    Node* next;
    Node(const std::string& k) : key(k), next(NULL) {}
    Node() : key(NULL), next(NULL) {}
};

// implemenation of the hash table
class HashTable {
    private:
        Node** table;   // list of node pointers
        int size;       // table size to minimize the chance of collisions

        // djb2 hash function
        int hash(const std::string& key) {
            unsigned long hash = 5381;
            for (int c : key) {
                hash = ((hash << 5) + hash) + c; // bitwise operation equivalent to hash * 33 + c as it is faster than the multiplication
            }
            return int(hash % size);             // compression function
        }

    public:
        // constructor
        HashTable(const int DATA_SIZE, const float LOAD_FACTOR) {
            size = int(DATA_SIZE/LOAD_FACTOR);
            table = new Node*[size];
            for (int i = 0; i < size; i++) table[i] = NULL;
        }

        // destructor
        ~HashTable() {
            for (int i = 0; i < size; i++) {
                Node* cur = table[i];
                while (cur != NULL) {
                    auto temp = cur;
                    cur = cur->next;
                    delete temp;
                }
            }
            delete[] table;
        }

        // inserts a key/word into the hash table
        void insert(const std::string& key) {
            int i = hash(key);
            Node* newNode = new Node(key);
            if (table[i] == NULL) {
                table[i] = newNode;
            } else {    // collision handling
                newNode->next = table[i];
                table[i] = newNode;
            }
        }

        // searches for a key/word in the hash table
        bool search(const std::string& key) {
            int i = hash(key);
            Node* cur = table[i];
            while (cur != NULL) {
                if (cur->key == key) return true;
                cur = cur->next;
            }
            return false;
        }
};

// helper function for quick sort which partitions the vector
int partition(int l, int h, std::vector<std::string>& vec) {
    auto p = vec[l];
    auto i = l + 1;
    auto j = h;
    while (i <= j) {
        if (vec[i] > p && vec[j] < p) std::swap(vec[i], vec[j]);
        if (vec[i] <= p) i++;
        if (vec[j] >= p) j--;
    }
    std::swap(vec[l],vec[j]);
    return j;
}

// quick sort implementation
void quickSort(int l, int h, std::vector<std::string>& vec) {
    if(l < h) {
        auto j = partition(l, h, vec);
        quickSort(l, j, vec);
        quickSort(j+1, h, vec);
    }
}

int main() {
    // open the required files
    std::ifstream file1("f1.txt");
    std::ifstream file2("f2.txt");
    std::ofstream out1("out.txt");

    // check that the files opened
    if (!file1.is_open() || !file2.is_open() || !out1.is_open()) {
        std::cout << "Error opening one of the files" << std::endl;
        return 1; // return 1 to indicate failure
    }

    // counts the number of words in the file to determine the size of the hash table
    int numWords = 0;
    std::string line;
    while (std::getline(file1, line)) numWords++;
    file1.clear();
    file1.seekg(0, std::ios::beg);  // used to return to the start of the list

    const int DATA_SIZE = numWords;
    const float LOAD_FACTOR = 0.5;  // load factor chosen so that the hash table is double the length of the number of words

    HashTable hashTable(DATA_SIZE, LOAD_FACTOR);
    
    // read file1 into the hash table
    std::string word;
    while (file1 >> word) {
        hashTable.insert(word);
    }
    file1.close();

    // read file2 and check to see if any duplicate words are present
    std::vector<std::string> duplicateWords;
    while (file2 >> word) {
        if (hashTable.search(word)) {
            duplicateWords.push_back(word);
        }
    }
    file2.close();

    // sort and print repeat words
    quickSort(0, duplicateWords.size() - 1, duplicateWords);

    for (const auto& w : duplicateWords) {
        out1 << w << std::endl;
    }

    out1.close();
    return 0;
}