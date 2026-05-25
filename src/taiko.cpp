#define ARE_WE_HERE_YET cout << "here" << endl 
#include <fstream> 
#include <sstream> 
#include <iostream> 
#include <array>
#include <cctype> 
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <strings.h>
#include "constants.h"
#include "helpers.h"
#include "taiko.h"

/*
TODO:
- ADD A REPLACE FUNCTION
*/

TaikoDatabase::TaikoDatabase(string file_name) : filename(file_name){
	/*
    Tally: tally[star][level]
    */

    ifstream data_file;
    
    data_file.open(filename,ios::in);
    if (!data_file.is_open()){
		cout << "file open error" << endl;
        return;
    }

    string line;
    int i=1;
    while(getline(data_file,line)){
        stringstream ss(line);
        string title;
        string stars_s;
        int stars;
        char level;
        getline(ss,title,'\t');
        getline(ss,stars_s,'\t');
        ss.get(level);

        level = toupper(level);
        if ( (level<min_allowed_level) || (level>max_allowed_level) ){
            cout << "Warning: Ignored reading line " << i << "as it was invalid" << endl;
            i++;
            continue;
        }

        clean_up_whitespace(title);
        transform(title.begin(), title.end(), title.begin(), [](unsigned char c) {
            return tolower(c);
        });

        try{
            stars = stoi(stars_s);
        }  catch (invalid_argument& e) {
            cout << "Warning: Ignored reading line " << i << "as it was invalid" << endl;
            i++;
            continue;
        }
        if (stars<1||stars>10){
            cout << "Warning: Ignored reading line " << i << "as it was invalid" << endl;
            i++;
            continue;
        }

        Song song = {title,stars,level};

        auto result = database.insert(song);
        catalogue[title] = result.first;
        tally[stars-1][level-'A']++;
        i++;

    }

    data_file.close();
    return;
}

//string vector of search results
vector<string> TaikoDatabase::search(stringstream& term){
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
    array<bool,10> star_mask;
    array<bool,6> level_mask;

	star_mask.fill(true);
	level_mask.fill(true);

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
            unsigned int i=5;
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
            unsigned int i=5;
            while (i<specifier.size()){ 
                //Read until we hit a level
                char op_char = toupper(specifier[i]);
                
                if ((op_char >= 'A') && (op_char <= 'F')){//level
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
        // cout << song.title << endl;
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

	return results_index;
}

void TaikoDatabase::save(){
    ofstream output_file;
    output_file.open(filename,ios::out);
    for (const Song& song : database){
        output_file << song.title << "\t" << song.stars << "\t" << song.level << endl;
    } 
    output_file.close();

}

void TaikoDatabase::save(const string& file_name){
    ofstream output_file;
    output_file.open(file_name,ios::out);
    for (const Song& song : database){
        output_file << song.title << "\t" << song.stars << "\t" << song.level << endl;
    } 
    output_file.close();

}

string TaikoDatabase::backup(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int year = 1900 + ltm->tm_year;
    int month =  1 + ltm->tm_mon;
    int day = ltm->tm_mday;

    string backup_filename = (string)"backups/" + filename +  "_backup_" + to_string(year) + "_" 
        + to_string(month) + "_" + to_string(day) + ".tsv";
    
    save(backup_filename);
	return backup_filename;
}

void TaikoDatabase::add_new_song(const Song& song){
    if (does_it_exist(song.title)){
        cout << "WHAT THE FUCK, WHY DOES THIS SONG ALREADY EXIST" << endl;
        return;
    }
    if (!is_this_level_valid(song.level)){
        cout << "WHAT THE FUCK, WHY DOES THIS SONG HAVE AN INVALID LEVEL" << endl;
        return;
    }
    if ((song.stars < 1) || (song.stars > 10)){
        cout << "WHAT THE FUCK, WHY DOES THIS SONG HAVE AN INVALID LEVEL" << endl;
        return;
    }

    auto result = database.insert(song);
    catalogue[song.title] = result.first;
    tally[song.stars-1][song.level-'A']++;
}

void TaikoDatabase::delete_song(const string& title){
    // cout << title << endl;
    if (!does_it_exist(title)){
        cout << "WHAT THE FUCK. TRIED TO DELETE INVALID TITLE" << endl;
        return;
    }
    set<Song>::const_iterator song_to_delete = catalogue[title];
    tally[song_to_delete->stars-1][song_to_delete->level-'A']--; 
    database.erase(*song_to_delete);
    catalogue.erase(title);
}

void TaikoDatabase::replace_song(const string& old_song, const Song& new_song){
    delete_song(old_song);
    add_new_song(new_song);
}

void TaikoDatabase::help(){
	cout << HLINE << endl;
    cout << "Help Menu" << endl;
    cout << "quit: saves and quits" << endl;
    cout << "search [SPECIFIERS] [TERM]: searches for songs with a given term " << endl;
    cout << "\t Specifiers: Filter form stars or level. Allowed comparison operators are >,<,==,<=,>=,!" << endl;
    cout << "\t Example usage: search stars>4 level!<=C rose" << endl;
    cout << "\t Example usage with escape character: search \\stars!!" << endl;
    cout << "save: saves the current database" << endl;
    cout << "backup: backs up the current database (including unsaved changes)" << endl;
    cout << "add: adds new song to database" << endl;
    cout << "stats: displays your current progress" << endl;
}

void TaikoDatabase::display_stats(){
    //tally[stars][level]
    vector<int> agg(NUMBER_OF_LEVELS);
    constexpr int chart_width = 100;

    //PER STAR STATS
    cout << HLINE << endl << HLINE << endl << "STAR BY STAR OVERVIEW" << endl;
    vector<int> per_star(NUMBER_OF_LEVELS);
    for (int stars = 1; stars <= 10; stars++){
        int total = 0;
        for (char lv = 'A'; lv<=max_allowed_level; lv++){
            int count = tally[stars-1][lv-'A'];
            per_star[lv-'A'] = count;
            total += count;
            agg[lv-'A'] += count;
        }
        total = total ? total : 1;
        cout << stars << "☆:" << endl;

        int bar_length; //print bars
        for (char lv = 'A'; lv<=max_allowed_level; lv++){
            cout << lv << "|";
            float portion = (float)per_star[lv-'A']/total;
            bar_length = (int)(portion*chart_width);
            for (int i=0; i<bar_length; i++){
                cout << "█";
            }
            cout << per_star[lv-'A'] << " (" << fixed << setprecision(1) << portion*100 << "%)";
            cout << endl;
        }
        cout << HLINE << endl;

    }

    //AGGREGATE STATS
    cout << HLINE << endl << "TOTAL OVERVIEW" << endl;

    for (char lv = 'A'; lv<=max_allowed_level; lv++){
        cout << lv << "|";
        float portion = (float)agg[lv-'A']/database.size();
        int bar_length = (int)(portion*chart_width);
        for (int i=0; i<bar_length; i++){
            cout << "█";
        }
        cout << agg[lv-'A'] << " (" << fixed << setprecision(1) << portion*100 << "%)";
        cout << endl;
    }
    cout << HLINE << endl;

    

}

bool TaikoDatabase::does_it_exist(const string& title){
    return (catalogue.find(title) != catalogue.end());
}

const Song TaikoDatabase::lookup(const string& title){
    Song ans;
    if (does_it_exist(title)){
        ans = *catalogue[title];    
    }
    return ans;
}

bool TaikoDatabase::is_this_level_valid(char lv){
    if ((lv < min_allowed_level) || (lv  > max_allowed_level)){
        return false;
    }
    return true;
}