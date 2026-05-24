#pragma once
#include <vector> 
#include <fstream> 
#include <sstream> 
#include <iostream> 
#include <unordered_map> 
#include <set> 
#include <map>
#include <array>
#include <cctype> 
#include <ctime>
#include <algorithm>
#include <strings.h>
#include "helpers.h"

using namespace std;

static constexpr char min_allowed_level = 'A';
static constexpr char max_allowed_level = 'F';

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

		void add_new_song(const Song& song);
		void delete_song(const string& title);
		void replace_song(const string& old_song, const Song& new_song);
		bool does_it_exist(const string& title);		
		const Song lookup(const string& title); 

	private:
		string filename;
		
		set<Song,Song_Comp> database;
		unordered_map<string, set<Song>::const_iterator> catalogue;
		// tally[stars][levels] = count of songs
		array<array<int, 6>, 10> tally;

		void save(const string& file_name);
};