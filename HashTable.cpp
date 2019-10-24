#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "HashTable.hpp"
#include <iomanip>

unsigned int HashTable::getHash(std::string word){
  unsigned int hashValue = 5381;
  int length = word.length();
  for (int i=0;i<length;i++){
    hashValue=((hashValue<<5)+hashValue) + word[i];
  }
  hashValue %= hashTableSize;
  return hashValue;
}

HashTable::HashTable(int newHashTableSize){
    hashTableSize = newHashTableSize;
    this->hashTable = new wordItem*[hashTableSize];
    numCollisions = 0;
    numItems = 0;
    for(int i=0; i<hashTableSize; i++){
        hashTable[i] = nullptr;
    }
}



HashTable::~HashTable(){
  wordItem *temp;
  wordItem *next;
  for(int i = 0; i < hashTableSize; i++){
    if(hashTable[i] != nullptr){
      temp = hashTable[i];
      while(temp != nullptr){
        next = temp->next;
        delete temp;
        temp = next;
      }
    }
  }
  delete[] hashTable;
}

void HashTable::addWord(std::string word){
  if(!isInTable(word)){
    int index = getHash(word);
    wordItem *node = new wordItem;
    node->word = word;
    node->next = nullptr;
    numItems++;
    if(hashTable[index] == nullptr){
      hashTable[index] = node;
      node->count = 1;
    }
    else{
      node->next = hashTable[index];
      node->count = 1;
      hashTable[index] = node;
      numCollisions++;
    }
  }
}
void getStopWords(char *ignoreWordFileName, HashTable &stopWordsTable){
  std::ifstream stopWords(ignoreWordFileName);
  std::string str;
  while(stopWords >> str){
    stopWordsTable.addWord(str);
  }
}

bool isStopWord(std::string word, HashTable &stopWordsTable){
  return stopWordsTable.isInTable(word);
}

wordItem* HashTable::searchTable(std::string word){
  int index = getHash(word);
  wordItem *temp = hashTable[index];
  while(temp != nullptr){
    if(temp->word == word){
      return temp;
    }
    temp = temp->next;
  }
  return nullptr;
}

void HashTable::incrementCount(std::string word){
  wordItem *word1 = searchTable(word);
  word1->count++;
}

bool HashTable::isInTable(std::string word){
  if(searchTable(word)){
    return true;
  }
  return false;
}

void HashTable::printTopN(int n){
  int topWords[n];
  std::string strArr[n];
  int index;
  bool isGreater = false;
  for(int i = 0; i < n; i++){
    topWords[i] = 0;
  }
  for (int i = 0; i < hashTableSize; i++) {
    if(hashTable[i] != nullptr){
      wordItem *temp = hashTable[i];
      while(temp != nullptr){
        for(int j = 0; j < n; j++){
          if(temp->count > topWords[j]){
            isGreater = true;
            index = j;
            break;
          }
        }
        if(isGreater){
          for(int k = n-1; k > index; k--){
            topWords[k] = topWords[k-1];
            strArr[k] = strArr[k-1];
          }
        topWords[index] = temp->count;
        strArr[index] = temp->word;
        isGreater = false;
        }
        temp = temp->next;
      }
    }
  }
  for(int l = 0; l < n; l++){
    std::cout << std::fixed << std::setprecision(4) << (float) topWords[l]/getTotalNumWords() << " - " << strArr[l] << std::endl;
  }
}

int HashTable::getNumCollisions(){
  return numCollisions;

  /*
  int sum = 0;
  for (int i = 0; i < hashTableSize; i++) {
    if(hashTable[i] != nullptr){
      wordItem *temp = hashTable[i]->next;
      while(temp != nullptr){
        sum++;
        temp = temp->next;
      }
    }
  }
  return sum;
  */
}


int HashTable::getNumItems(){
  return numItems;
  /*
  int sum = 0;
  for (int i = 0; i < hashTableSize; i++) {
    if(hashTable[i] != nullptr){
      wordItem *temp = hashTable[i];
      while(temp != nullptr){
        sum++;
        temp = temp->next;
      }
    }
  }
  return sum;
  */
}


int HashTable::getTotalNumWords(){
  int sum = 0;
  for (int i = 0; i < hashTableSize; i++) {
    if(hashTable[i] != nullptr){
      wordItem *temp = hashTable[i];
      while(temp != nullptr){
        sum += temp->count;
        temp = temp->next;
      }
    }
  }
  return sum;
}

int main(int argc, char* argv[]){
  HashTable stopTable(STOPWORD_TABLE_SIZE);
  HashTable wordTable(std::stoi(argv[4]));
  getStopWords(argv[3], stopTable);

  std::ifstream words(argv[2]);
  std::string word;

  while(words >> word){
    if(!isStopWord(word, stopTable)){
      if(!wordTable.isInTable(word)){
        wordTable.addWord(word);
      }
      else{
        wordTable.incrementCount(word);
      }
    }
  }

  wordTable.printTopN(std::stoi(argv[1]));

  std::cout << "#" << std::endl;
  std::cout << "Number of collisions: "  << wordTable.getNumCollisions() << std::endl;
  std::cout << "#" << std::endl;
  std::cout << "Unique non-stop words: " << wordTable.getNumItems() << std::endl;
  std::cout << "#" << std::endl;
  std::cout << "Total non-stop words: " << wordTable.getTotalNumWords() << std::endl;

}
