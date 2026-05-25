#include "helpers.h"
#include "constants.h"


using namespace std;

bool ask_int(string message, int& dest, bool& repeat){
    if (!repeat){
        cout << message << endl << YOU;
    }
    repeat = true;
    
    string dest_s;
    getline(cin, dest_s);

    try {
        dest = stoi(dest_s);
        return true;
    } catch (invalid_argument& e){
        return false;
    }
    
}

void ask_char(string message, char& dest, bool& repeat){
    if (!repeat){
        cout << message << endl << YOU;
    }
    repeat = true;

    string char_s;
    getline(cin,char_s);
    dest = char_s[0];
    return;
}

void ask_yn(string message, bool& dest){
    cout << message << endl << YOU;
    char decision;
    bool repeat = true;
    while (1){
        ask_char("",decision,repeat);
        decision = toupper(decision);
        if (decision == 'Y'){
            dest = true;
            return;
        } else if (decision == 'N'){
            dest = false;
            return;
        } else {
            cout << endl << YOU;
        }
    }
}

void clean_up_whitespace(string &message){
    replace(message.begin(),message.end(),'\t',' ');

    message.erase(message.find_last_not_of(' ') + 1);

    message.erase(0, message.find_first_not_of(' '));

    bool charged = false;
    for (unsigned int i=0; i<message.size(); i++){
        if(message[i] == ' '){
            if (!charged){
                charged = true;
            } else {
                message.erase(i,1);
                i--;
            }
        } else {
            charged = false;
        }
    }
}
