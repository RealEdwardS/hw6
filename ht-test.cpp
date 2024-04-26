#include "ht.h"
#include "hash.h"
#include <unordered_map>
#include <iostream>
#include <utility>
#include <string>
#include <sstream>
#include <functional>
using namespace std;
int main()
{
    // DoubleHashProber<std::string, MyStringHash > dh;
    // HashTable<
    //     std::string, 
    //     int, 
    //     DoubleHashProber<std::string, MyStringHash >, 
    //     std::hash<std::string>, 
    //     std::equal_to<std::string> > ht(0.7, dh);

    // This is just arbitrary code. Change it to test whatever you like about your 
    // hash table implementation.
    // for(size_t i = 0; i < 10; i++){
    //     std::stringstream ss;
    //     ss << "hi" << i;
    //     ht.insert({ss.str(), i});
    // }
    // if( ht.find("hi1") != nullptr ){
    //     cout << "Found hi1" << endl;
    //     ht["hi1"] += 1;
    //     cout << "Incremented hi1's value to: " << ht["hi1"] << endl;
    // }
    // if( ht.find("doesnotexist") == nullptr ){
    //     cout << "Did not find: doesnotexist" << endl;
    // }
    // cout << "HT size: " << ht.size() << endl;
    // ht.remove("hi7");
    // ht.remove("hi9");
    // cout << "HT size: " << ht.size() << endl;
    // if( ht.find("hi9") != nullptr ){
    //     cout << "Found hi9" << endl;
    // }
    // else {
    //     cout << "Did not find hi9" << endl;
    // }
    // ht.insert({"hi7",17});
    // cout << "size: " << ht.size() << endl;

    // ht.insert({"edward", 1});
    // ht.insert({"edward", 2});
    // ht.insert({"edward", 3});
    // ht.reportAll(cout); 

    // cout << endl; 

    // ht.insert({"1", 1});
    // ht.insert({"2", 1});
    // ht.insert({"3", 1});
    // ht.insert({"4", 1});
    // ht.insert({"5", 1});
    // ht.insert({"6", 1});
    // ht.insert({"7", 1});
    // ht.insert({"8", 1});


    // ht.insert({"9", 1});
    // ht.insert({"10", 1});
    // ht.insert({"11", 1});
    // ht.insert({"12", 1});
    // ht.insert({"13", 1});c
    // ht.reportAll(cout);

    // if (ht.empty()){
    //     cout << "empty" << endl;
    // }

    // else{
    //     cout << "not empty" << endl; 
    // }


    const vector<int> sizemap = {11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717, 51437, 102877, 205759};
    HashTable<int, int, LinearProber<int>, std::hash<int>, std::equal_to<int>> ht(1.0, LinearProber<int>(), std::hash<int>());

    int prevsize;
    for(size_t i = 0;i<sizemap.size()-1;i++){
        if(i == 0){
            prevsize = 0;
        }

        else{
            prevsize = sizemap[i-1]+1;
        }

        for(int j = prevsize; j <= sizemap[i]; j++){
            pair<int,int> pair(j,j);
            ht.insert(pair);

            if(j == sizemap[i]-1){
                cout << ht.size() << endl; 
            }

            if(j == sizemap[i]){
                //resize should happen.
                // cout << ht.size() << endl;              
            }
        }
    }

    return 0;
}