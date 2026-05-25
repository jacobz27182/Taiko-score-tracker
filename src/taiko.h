#pragma once
#include <vector> 
#include <unordered_map> 
#include <set> 
#include <map>
#include <string>

#define NUMBER_OF_LEVELS 6

using namespace std;

enum class SearchState{
    Operator,
    Number
};

struct Song{ 
    string title; 
    int stars; 
    char level; 
};

struct Song_Comp {
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

class TaikoDatabase{
	public:
		TaikoDatabase(string file_name);

		//string vector of search results
		vector<string> search(stringstream& term);   
		void save();
		string backup();
		void help();
		void display_stats();

		void add_new_song(const Song& song);
		void delete_song(const string& title);
		void replace_song(const string& old_song, const Song& new_song);

		bool does_it_exist(const string& title);		
		const Song lookup(const string& title); 
		bool is_this_level_valid(char lv);

		static constexpr char min_allowed_level = 'A';
		static constexpr char max_allowed_level = 'F';

	private:
		string filename;
		
		set<Song,Song_Comp> database;
		unordered_map<string, set<Song>::const_iterator> catalogue;
		// tally[stars][levels] = count of songs
		array<array<int, NUMBER_OF_LEVELS>, 10> tally = {};

		void save(const string& file_name);
};