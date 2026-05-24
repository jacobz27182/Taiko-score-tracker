#pragma once
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <iostream>

template<size_t N>
bool apply_search_mask(std::array<bool,N>& mask, std::string op, int num){
    /*
    NUM MUST BE ZERO INDEXED
    */
    int flip_count = 0;
    bool flip = false;
    while (op[flip_count] == '!'){ //just to fuck around lmao
        flip ^= true;
        flip_count++;
    }

    std::string sub_operator = op.substr(flip_count); 
    for (unsigned int i=0; i<N; i++){
        if (sub_operator == ">"){
            mask[i] = (mask[i] && flip^(i > num));
        } else if (sub_operator == ">="){
            mask[i] = (mask[i] && flip^(i >= num));
        } else if (sub_operator == "<"){
            mask[i] = (mask[i] && flip^(i < num));
        } else if (sub_operator == "<="){
            mask[i] = (mask[i] && flip^(i <= num));
        } else if ((sub_operator == "==") || (sub_operator == "=")){
            mask[i] = (mask[i] && flip^(i == num));
        } else {
            return false; //invalid operator!
        }
    }
    return true;
}
void clean_up_whitespace(std::string &message);
bool ask_int(std::string message, int& dest, bool& repeat);
void ask_char(std::string message, char& dest, bool& repeat);
void ask_yn(std::string message, bool& dest);
