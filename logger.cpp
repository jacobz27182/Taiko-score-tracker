#define ARE_WE_HERE_YET cout << "here" << endl 
#include <vector> 
#include <fstream> 
#include <sstream> 
#include <iostream> 
#include <unordered_map> 
#include <set> 
#include <map>
#include <cctype> 
#include <ctime>
#include <algorithm>
#include <strings.h>
using namespace std; 

/* 
LEVELS: 
A = Fail/Unplayed
B = Pass 
C = gngr >= 3 
D = gngr >= 5 
E = gngr >= 10, bads <= 3 
F = ???? 
*/


/*
TODO LIST:
- undo and redo
- stats
*/
struct Song{ 
    string title; 
    int stars; 
    char level; 
};

struct Comp {
    bool operator()(const Song& a, const Song& b) const {
        if (a.level != b.level){
            return a.level < b.level; 
        }

        if (a.stars != b.stars){
            return a.stars < b.stars;
        }
        
        return a.title < b.title;
    }
};

enum class SearchState{
    Operator,
    Number
};

set<Song, Comp> load_database(string filename);
unordered_map<string, const Song*> get_catalogue(const set<Song, Comp>& database);
void help();
void quit(set<Song, Comp>& database, string filename);
void search(stringstream& word, set<Song, Comp>& database, unordered_map<string, const Song*>& catalogue);
void save(const set<Song, Comp>& database, string filename);
void backup(const set<Song, Comp>& database);

void add_song(set<Song, Comp>& database, unordered_map<string, const Song*>& catalogue);
void update_song(set<Song, Comp>& database, unordered_map<string, const Song*>& catalogue, string title);
bool apply_search_mask(vector<bool>& mask, string op, int num);
void clean_up_whitespace(string &message);

bool ask_int(string message, int& dest);
void ask_char(string message, char& dest);
void ask_yn(string message, bool& dest);


const string HLINE = "___________________________________";
const string YOU = "zzykrkv: ";
const set<char> ALLOWED_LEVELS = {'A','B','C','D','E','F'};

int main(){
    const string filename = "database.tsv"; 
    
    cout << "loading database..." << endl;
    set<Song, Comp> database = load_database(filename);
    unordered_map<string, const Song*> catalogue = get_catalogue(database);
    string you_2 = YOU;
    you_2.erase(YOU.length()-2);
    cout << "Welcome, " +  you_2 << endl;

    while(1){        //CONSOLE LOOP
        cout << HLINE << endl << "           How can I help today?" << endl << YOU; 
        string command;
        getline(cin, command); 
        stringstream ss(command);

        string function;
        ss >> function;

        if (function == "help"){
            help();
        } else if (function == "quit"){
            quit(database,filename);
            return 0;
        } else if (function == "search"){
            search(ss,database,catalogue);
        } else if (function == "save") {
            save(database,filename);
            cout << "Saved" << endl;
        }else if (function == "backup") {
            backup(database);
        } else if (function == "add"){
            add_song(database,catalogue);
        } else {
            cout << "Invalid command" << endl;
        }
    }
}

set<Song, Comp> load_database(string filename){
    set<Song, Comp> database;
    ifstream data_file;
    
    data_file.open(filename,ios::in);
    if (!data_file.is_open()){
        return database;
    }

    string line;
    // TODO: FIX
    while(getline(data_file,line)){
        stringstream ss(line);
        string title;
        string stars_s;
        int stars;
        char level;
        getline(ss,title,'\t');
        getline(ss,stars_s,'\t');
        ss.get(level);
        transform(title.begin(), title.end(), title.begin(), [](unsigned char c) {
            return tolower(c);
        });
        stars = stoi(stars_s);
        Song song = {title,stars,level};
        database.insert(song);
    }

    data_file.close();

    return database;

}

unordered_map<string, const Song*> get_catalogue(const set<Song, Comp>& database){
    unordered_map<string, const Song*> catalogue;
    for (const Song& song : database){
        catalogue[song.title] = &song;
    } 
    return catalogue;
}

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

void help(){
    cout << HLINE << endl;
    cout << "Help Menu" << endl;
    cout << "quit: saves and quits" << endl;
    cout << "search [SPECIFIERS] [TERM]: searches for songs with a given term " << endl;
    cout << "\t Specifiers: Filter form stars or level. Allowed comparison operators are >,<,==,<=,>=,!" << endl;
    cout << "\t Example usage: search stars>4 level<=E rose" << endl;
    cout << "\t Example usage with escape character: search \\stars!!" << endl;
    cout << "save: saves the current database" << endl;
    cout << "backup: backs up the current database (including unsaved changes)" << endl;
}

void quit(set<Song, Comp>& database, string filename){
    cout << "Saving and quitting" << endl;
    save(database,filename);
}

bool apply_search_mask(vector<bool>& mask, string op, int num){
    /*
    NUM MUST BE ZERO INDEXED
    */
    int flip_count = 0;
    bool flip = false;
    while (op[flip_count] == '!'){ //just to fuck around lmao
        flip ^= true;
        flip_count++;
    }

    string sub_operator = op.substr(flip_count); 
    for (int i=0; i<mask.size(); i++){
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

void clean_up_whitespace(string &message){
    replace(message.begin(),message.end(),'\t',' ');

    message.erase(message.find_last_not_of(' ') + 1);

    message.erase(0, message.find_first_not_of(' '));

    bool charged = false;
    for (int i=0; i<message.size(); i++){
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

void search(stringstream& term, set<Song, Comp>& database, unordered_map<string, const Song*>& catalogue){
    /*
    Allowed operators:
    >
    <
    >=
    <=
    ==
    !=
    */

    // vector<string> results;
    vector<bool> star_mask(10,true);
    vector<bool> level_mask(ALLOWED_LEVELS.size(),true);

    string specifier;
    bool no_term = false;
    while (1){ //read specifier by specifier
        if (!(term >> specifier)){
            //in this situation, there is no title
            no_term = true;
            break;
        }

        if (specifier[0] == '\\'){//escape character
            specifier.erase(0,1);
            break;
        }

        if ((specifier == "stars")||(specifier=="level")){
            cout << "Note: treating \"" + specifier + " \" as a literal search term. To suppress this warning use \\" + specifier << endl;
            break;
        }
        
        transform(specifier.begin(), specifier.end(), specifier.begin(), [](unsigned char c) {
            return tolower(c);
        });

        size_t found_star = specifier.find("stars");
        if (found_star == 0){//read star specifier
            bool valid = true;
            string star_op;
            string star_num;
            SearchState state = SearchState::Operator;
            int i=5;
            while (i<specifier.size()){ 
                if (state==SearchState::Operator){ //Read until we hit a digit
                    char op_char = specifier[i];
                    
                    if ((48 <= op_char)&&(op_char<= 57)){//digit
                        star_num += op_char;
                        state = SearchState::Number;
                        i++;
                        continue;
                    }

                    star_op += op_char;
                    i++;
                }

                if (state == SearchState::Number){//Read a star count
                    char num_char = specifier[i];

                    if ((48 <= num_char)&&(num_char<= 57)){//digit
                        star_num += num_char;
                        i++;
                        continue;
                    }

                    cout << "Warning: Ignored specifier: \"" << specifier << "\" as it is invalid. (Use \\stars if your song name begins with \"stars\")" << endl;
                    valid = false;
                    break;
                }
            }    
            
            if (star_num.empty()){
                valid = false;
                cout << "Warning: Ignored specifier: \"" << specifier << "\" as it is invalid. (Use \\stars if your song name begins with \"stars\")" << endl;
            }
            if (valid){
                valid = apply_search_mask(star_mask, star_op, stoi(star_num)-1);
                if (!valid){
                    cout << "Warning: Ignored specifier: \"" << specifier << "\" as it is invalid. (Use \\stars if your song name begins with \"stars\")" << endl;   
                }
            }
            continue;   //read next specifier
        }

        size_t found_level = specifier.find("level");
        if (found_level == 0){ //read level specifier
            bool valid = true;
            string level_op;
            char level_num = '_';
            SearchState state = SearchState::Operator;
            int i=5;
            while (i<specifier.size()){ 
                //Read until we hit a digit
                char op_char = toupper(specifier[i]);
                
                if (ALLOWED_LEVELS.find(op_char) != ALLOWED_LEVELS.end()){//level
                    level_num = op_char;
                    break;
                }

                level_op += op_char;
                i++;
            }

            if (level_num == '_'){
                valid = false;
            }

            if (valid){
                valid = apply_search_mask(level_mask, level_op, level_num-'A');
            }
            if (!valid){
                cout << "Warning: Ignored specifier: \"" << specifier << "\" as it is invalid. (Use \\level if your song name begins with \"level\")" << endl;   
            }

            continue;   //read next specifier
        }

        //in this situation, specifier holds the first word of the title
        break; 
    }
    string rest_of_term;
    getline(term,rest_of_term);

    string target;
    if (!no_term){
        target = specifier + rest_of_term;
    }

    if (!target.empty()){
        clean_up_whitespace(target);
    }

    //gather results
    transform(target.begin(), target.end(), target.begin(), [](unsigned char c) {
        return tolower(c);
    });

    map<size_t,vector<string>> results;

    cout << HLINE << endl;
    int cnt = 0;
    for (const Song& song : database){
        if (!star_mask[song.stars-1]){
            continue;
        }
        if (!level_mask[song.level-'A']){
            continue;
        }

        size_t pos = song.title.find(target);
        if (pos != string::npos){ // find
            results[pos].push_back(song.title);
        }
    }

    vector<string> results_index;
    for (const auto& [position, list_of_results] : results){
        for (string ttl : list_of_results){
            results_index.push_back(ttl);
            cout << cnt++ << ": " << catalogue[ttl]->level 
            << ": (" << catalogue[ttl]->stars << "☆) " << catalogue[ttl]->title << endl;
        } 
    }

    //post search interface
    cout << "R: search again" << endl;
    cout << "B: back to main menu" << endl;
    cout << "N: add new entry" << endl;
    cout << YOU;

    while (1){
        string command;
        getline(cin, command);

        if (toupper(command[0]) == 'R'){
            string new_term;
            cout << "please enter new search term: ";
            getline(cin,new_term);
            stringstream ss(new_term);
            search(ss,database,catalogue);
            break;

        } else if (toupper(command[0]) == 'B'){
            break;

        } else if (toupper(command[0]) == 'N'){

            add_song(database,catalogue);
            break;

        } else {
            try {
                int idx = stoi(command);
                if (idx >= 0 && idx < results_index.size()){
                    update_song(database, catalogue, results_index[idx]);
                } else {
                    cout << "Invalid index" << endl << YOU;
                    continue;
                }
                break;
            } catch (invalid_argument& e){
                cout << "Invalid command" << endl;
                cout << YOU;
            }
        }
    }
}

void add_song(set<Song, Comp>& database, unordered_map<string, const Song*>& catalogue){
    string title;
    int stars;
    char level;

    cout << HLINE << endl;
    cout << "Enter song name" << endl << YOU;
    while(1){
        getline(cin,title);
        clean_up_whitespace(title);
        if (title.empty()){
            cout << "You gave an empty title.. come on man" << endl << YOU;
            continue;
        }
        transform(title.begin(), title.end(), title.begin(), [](unsigned char c) {
            return tolower(c);
        });
        
        if (catalogue.find(title)!=catalogue.end()){
            cout << "A song with the same title already exists!" << endl << YOU;
            continue;
        }
        break;
    }
    
    bool repeat_int = false;
    while (1){
        bool outcome = ask_int("Enter star count", stars, repeat_int);
        if (!outcome){
            cout << "Invalid integer" << endl << YOU;
            continue;
        }
        if ((1<=stars) && (stars<=10)){
            break;
        }
        cout << "Enter a star count from 1 to 10" << endl << YOU;
    }

    bool repeat_char = false;
    while (1){
        ask_char("Enter a level",level,repeat_char);
        level = toupper(level);
        if (ALLOWED_LEVELS.find(level) != ALLOWED_LEVELS.end()){
            break;
        }
        cout << "Enter a valid level (" << *ALLOWED_LEVELS.begin() << "-" <<
        *prev(ALLOWED_LEVELS.end()) << ")" << endl << YOU;
    }


    string level_s = {level};
    string message = "Add: " + title + " (" + to_string(stars) 
    + "*) " + "at level " + level_s + "? (Y/n)";
    
    bool decision;
    ask_yn(message, decision);
    if (decision){
        Song new_song = {title,stars,level};
        auto [it, ok] = database.insert(new_song);
        if (ok){
            catalogue[it->title] = &(*it);
        }
        cout << "Added" << endl;
    } else {
        cout << "Cancelled" << endl;
    }
}

void update_song(set<Song, Comp>& database, unordered_map<string, const Song*>& catalogue, string title){
    cout << HLINE << endl;
    cout << title << " (" << catalogue[title]->stars << "☆" << ")" << endl;
    cout << "Current level: " << catalogue[title]->level << endl;
    
    char decision;
    bool modification_repeat = false;
    while (1){
        ask_char("Would you like to modify level (L), stars (S), title (T), delete (D), or go back (B)?", decision, modification_repeat);
        decision = toupper(decision);
        if (decision == 'L'){

            char old_level = toupper(catalogue[title]->level);
            char new_lv;

            bool repeat_char = false;
            while(1){
                ask_char("Enter new level",new_lv,repeat_char);
                new_lv = toupper(new_lv);
                if (ALLOWED_LEVELS.find(new_lv) != ALLOWED_LEVELS.end()){
                    break;
                }
                cout << "Enter a valid level (" << *ALLOWED_LEVELS.begin() << "-" <<
                *prev(ALLOWED_LEVELS.end()) << ")" << endl << YOU;
            }

            
            Song song_to_update = *catalogue[title];
            database.erase(song_to_update);  // erase from set by value
            song_to_update.level = new_lv;
            auto [new_song_it,ok] = database.insert(song_to_update);
            catalogue[title] = &(*new_song_it); //update catalogue

            if (old_level < new_lv) {
                cout << "promoted " << title << " (" << catalogue[title]->stars << "☆)" << " from level " << old_level << " to level " << new_lv << endl;
                cout << "congratulations!" << endl;
            } else {
                cout << "updated " << title << " (" << catalogue[title]->stars << "☆) " << " from level " << old_level << " to level " << new_lv << endl;
            }
            break;

        } else if (decision == 'S'){
            
            int old_stars = catalogue[title]->stars;
            int new_stars;

            bool repeat_int = false;
            while (1){
                bool outcome = ask_int("Enter new star count",new_stars,repeat_int);
                if (!outcome){
                    cout << "Invalid integer" << endl << YOU;
                    continue;
                }
                if ((1<=new_stars) && (new_stars<=10)){
                    break;
                }
                cout << "Enter a star count from 1 to 10" << endl << YOU;
            }
            
            Song song_to_update = *catalogue[title];
            database.erase(song_to_update);  // erase from set by value
            song_to_update.stars = new_stars;
            auto [new_song_it,ok] = database.insert(song_to_update);
            catalogue[title] = &(*new_song_it); //update catalogue

            cout << "updated " << title << " from " << old_stars << "☆ to " << new_stars << "☆" << endl;
            
            break;
        } else if (decision == 'T'){
            while (1){
                cout << "Enter new song title" << endl << YOU;
                string new_title;
                while(1){
                    getline(cin,title);
                    clean_up_whitespace(title);
                    if (title.empty()){
                        cout << "You gave an empty title.. come on man" << endl << YOU;
                        continue;
                    }
                    transform(title.begin(), title.end(), title.begin(), [](unsigned char c) {
                        return tolower(c);
                    });
                    
                    if (catalogue.find(title)!=catalogue.end()){
                        cout << "A song with the same title already exists!" << endl << YOU;
                        continue;
                    }
                    break;
                }

                string message = "Confirm title as: " + new_title + "? (Y/n)";
                bool yes;
                ask_yn(message,yes);
                
                if (yes){
                    Song song_to_update = *catalogue[title];
                    database.erase(song_to_update);  // erase from set by value
                    catalogue.erase(title); //erase from map by key

                    song_to_update.title = new_title;
                    auto [new_song_it,ok] = database.insert(song_to_update);
                    catalogue[title] = &(*new_song_it); //update catalogue
                    break;

                } else {
                    string re_message =  "Re-enter new title? (Y/n)";
                    bool re_yes;
                    ask_yn(re_message,re_yes);
                    if (re_yes){
                        continue;
                    } else {
                        cout << "Returning to home page" << endl;
                        break;
                    }
                }
            }
            break;
        } else if (decision == 'D'){
            string message = "Are you sure you want to delete: " + title + "? (Y/n)";
            bool decision;
            ask_yn(message,decision);
            if (decision){
                database.erase(*catalogue[title]);
                catalogue.erase(title);

                cout << "Deleted!" << endl;
            } 
            break;
        } else if (decision == 'B'){
            break;
        } else {
            cout << "Invalid command" << endl << YOU;
        }
    }
}

void save(const set<Song, Comp>& database, string filename){
    ofstream output_file;
    output_file.open(filename,ios::out);
    for (const Song& song : database){
        output_file << song.title << "\t" << song.stars << "\t" << song.level << endl;
    } 
    output_file.close();

}

void backup(const set<Song, Comp>& database){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int year = 1900 + ltm->tm_year;
    int month =  1 + ltm->tm_mon;
    int day = ltm->tm_mday;

    string backup_filename = "database_backup_" + to_string(year) + "_" 
        + to_string(month) + "_" + to_string(day) + ".tsv";
    
    save(database,backup_filename);
    cout << "Database has been backed up to: " + backup_filename << endl;
}