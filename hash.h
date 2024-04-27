#ifndef HASH_H
#define HASH_H

#include <iostream>
#include <cmath>
#include <random>
#include <chrono>

typedef std::size_t HASH_INDEX_T;

struct MyStringHash {
    HASH_INDEX_T rValues[5] { 983132572, 1468777056, 552714139, 984953261, 261934300 };
    MyStringHash(bool debug = true)
    {
        if(false == debug){
            generateRValues();
        }
    }

    /*
        for (unsigned int t = 0; t < substring.size(); ++t){
            std::cout << substring.at(t); 
        }
    */
    // hash function entry point (i.e. this is h(k))
    HASH_INDEX_T operator()(const std::string& k) const
    {
        // Add your code here
        HASH_INDEX_T h = 0;
        HASH_INDEX_T w[5] = {0,0,0,0,0};
        std::string allSubstrings[6]; 

        std::string substring;
        unsigned int size = k.size(); 

        // Creating substrings a1, ..., a6
        for (unsigned int i = 0; i < 6; ++i){
            if (size == 0){
                substring = ""; 
            }

            else if (size <= 6){
                substring = k.substr(0, size);
                size = 0; 
            }

            else if (size > 6){
                substring = k.substr(k.size()-(6 * (i+1)), 6);
                size -= 6; 
            }

            allSubstrings[5-i] = substring; 

        }

        // Testing substrings
        // for (unsigned int j = 0; j < 6; ++j){
        //     // std::cout << j << std::endl; 
        //     std::cout << allSubstrings[j] << std::endl; 
        // }

        // size -= substring.size(); 
        // Printing out substring
        // for (unsigned int i = 0; i < substring.size(); ++i){
        //     std::cout << substring.at(i); 
        // }
        
        // Creating w[0], ... , w[4] 
        for (unsigned int i = 0; i < 5; ++i){

            for (unsigned int j = 0; j < allSubstrings[5-i].size(); ++j){
                //allSubstrings[5-i].size()-1-j
                // std::cout << "Base: " << j << " " << "Val: " <<  allSubstrings[5-i].at(allSubstrings[5-i].size()-1-j) << std::endl;
                // std::cout << pow(36, j) << " " << letterDigitToNumber(allSubstrings[5-i].at(allSubstrings[5-i].size()-1-j)) << std::endl;
                // std::cout << letterDigitToNumber(allSubstrings[5-i].at(allSubstrings[5-i].size()-1-j)) << std::endl;
                // // std::cout << (pow(36,j) * letterDigitToNumber(allSubstrings[5-i].at(allSubstrings[5-i].size()-1-j))) << std::endl;
                HASH_INDEX_T val;
                if (j <= 4){
                    val = (pow(36,j) * letterDigitToNumber(allSubstrings[5-i].at(allSubstrings[5-i].size()-1-j)));
                }

                else{
                    val = 60466176 * letterDigitToNumber(allSubstrings[5-i].at(allSubstrings[5-i].size()-1-j));
                }
                
                // std::cout << " Val" << val << std::endl;
                // std::cout << pow(36,j) << " " << letterDigitToNumber(allSubstrings[5-i].at(allSubstrings[5-i].size()-1-j)) << std::endl;
                w[4-i] += val; //(pow(36,j) * letterDigitToNumber(allSubstrings[5-i].at(allSubstrings[5-i].size()-1-j)));
                // std::cout << w[4-i] << std::endl;
                // std::cout << 5-i << std::endl; 
                // std::cout << w[4-i] << std::endl;
            }

            if (allSubstrings[5-i].size() == 0){
                w[4-i] = 0;
    
            }
        }

        // Printing out w[i], .., w[6]
        // for (unsigned int i = 0; i < 5; ++i){
        //     std::cout << "w[" << i << "] = " << w[i] << std::endl;
        // }

        // Creating h[k]
        // std::cout << w[4] << std::endl; 
        for (unsigned int i = 0; i < 5; ++i){
            // std::cout << "w[" << i << "]" << w[i] << std::endl; 
            h += rValues[i] * w[i]; 
        }

        // std::cout << std::endl << std::to_string(size) << std::endl;

        return h; 
    }

    // A likely helper function is to convert a-z,0-9 to an integral value 0-35
    HASH_INDEX_T letterDigitToNumber(char letter) const
    {
        // Add code here or delete this helper function if you do not want it

        HASH_INDEX_T result = 0; 

        // if lowercase
        if (letter >= 'a' && letter <= 'z'){
            result = letter - 'a';  
            
        }


        // if uppercase
        else if (letter >= 'A' && letter <= 'Z'){
            // Find difference between 'a' and 'A' and shift
            // this gives the distance between 0 and 'A'
            result = letter - 'A';  
        }

        // if integer
        else if (letter >= '0' && letter <= '9'){
            result = letter - '0' + 26; 
        }

        // std::cout << result << std::endl; 
        return result;

    }

    // Code to generate the random R values
    void generateRValues()
    {
        // obtain a seed from the system clock:
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 generator (seed);  // mt19937 is a standard random number generator

        // Simply call generator() [it has an operator()] to get another random number
        for(int i{ 0 }; i < 5; ++i)
        {
            rValues[i] = generator();
        }
    }
};

#endif
