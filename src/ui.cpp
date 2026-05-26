#include <iostream>
#include <algorithm>
#include "constants.h"
#include "taiko.h"
#include "helpers.h"
#include "ui.h"

void console_loop(TaikoDatabase& don_chan){
	while(1){        
        cout << HLINE << endl << "           How can I help today?" << endl << YOU; 
        string command;
        getline(cin, command); 
        stringstream ss(command);

        string function;
        ss >> function;

        if (function == "help"){
            don_chan.help();

        } else if (function == "quit"){
            cout << "Saving and Quitting..." << endl;
			don_chan.save();
            return;

        } else if (function == "search"){
            search_interface(don_chan,ss);

        } else if (function == "save") {
            don_chan.save();
            cout << "Saved" << endl;

        }else if (function == "backup") {
            string backed_up_filename = don_chan.backup();
			cout << "Database has been backed up to: " + backed_up_filename << endl;

        } else if (function == "add"){
            new_song_menu(don_chan);

		} else if (function == "stats"){
			don_chan.display_stats();

		} else if (function == "undo"){
			undo_redo(don_chan, UR::Undo, ss);

		} else if (function == "redo"){
			undo_redo(don_chan, UR::Redo, ss);

        } else {
            cout << "Invalid command" << endl;
        }
    }
}

void search_interface(TaikoDatabase& don_chan, stringstream& search_term){
	vector<string> results_index = don_chan.search(search_term);

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
            search_interface(don_chan,ss);
            break;

        } else if (toupper(command[0]) == 'B'){
            break;

        } else if (toupper(command[0]) == 'N'){

            new_song_menu(don_chan);
            break;

        } else {
            try {
                unsigned int idx = stoi(command);
                //negatives will wrap around
                if (idx < results_index.size()){
                    song_menu(don_chan, results_index[idx]);
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

void song_menu(TaikoDatabase &don_chan, string title){ //keep the old title in case we delete the object
    const Song song = don_chan.lookup(title);
    if (song.title.empty()){
        //THIS SHOULD NEVER HAPPEN TO A USER
        cout << "WHAT THE FUCK. Lookup on nonexistent song" << endl;
        return;
    }
    cout << HLINE << endl;
    cout << song.title << " (" << song.stars << "☆" << ")" << endl;
    cout << "Current level: " << song.level << endl;
    
    char decision;
    bool modification_repeat = false;

    while (1){
        ask_char("Would you like to modify level (L), stars (S), title (T), delete (D), or go back (B)?", decision, modification_repeat);
        decision = toupper(decision);
        if (decision == 'L'){

            char old_level = toupper(song.level);
            char new_lv;

            bool repeat_char = false;
            while(1){
                ask_char("Enter new level",new_lv,repeat_char);
                new_lv = toupper(new_lv);
                if (don_chan.is_this_level_valid(new_lv)){
                    break;
                }
                cout << "Enter a valid level (" << don_chan.min_allowed_level << "-" <<
                don_chan.max_allowed_level << ")" << endl << YOU;
            }
            Song new_song = {song.title, song.stars, new_lv};
            // don_chan.delete_song(title);
            // don_chan.add_new_song(new_song);
            don_chan.replace_song(title,new_song);

            if (old_level < new_lv) {
                cout << "promoted " << title << " (" << song.stars << "☆)" << " from level " << old_level << " to level " << new_lv << endl;
                cout << "congratulations!" << endl;
            } else {
                cout << "updated " << title << " (" << song.stars << "☆) " << " from level " << old_level << " to level " << new_lv << endl;
            }
            break;

        } else if (decision == 'S'){
            
            int old_stars = song.stars;
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
            
            Song new_song = {song.title, new_stars, song.level};
            // don_chan.delete_song(title);
            // don_chan.add_new_song(new_song);
            don_chan.replace_song(title,new_song);

            cout << "updated " << title << " from " << old_stars << "☆ to " << new_stars << "☆" << endl;
            
            break;
        } else if (decision == 'T'){
            while (1){
                cout << "Enter new song title" << endl << YOU;
                string new_title;
                while(1){
                    getline(cin,new_title);
                    clean_up_whitespace(new_title);
                    if (new_title.empty()){
                        cout << "You gave an empty title.. come on man" << endl << YOU;
                        continue;
                    }
                    transform(new_title.begin(), new_title.end(), new_title.begin(), [](unsigned char c) {
                        return tolower(c);
                    });
                    
                    if (don_chan.does_it_exist(new_title)){
                        cout << "A song with the same title already exists!" << endl << YOU;
                        continue;
                    }
                    break;
                }

                string message = "Confirm title as: " + new_title + "? (Y/n)";
                bool yes;
                ask_yn(message,yes);
                
                if (yes){
                    Song new_song = {new_title, song.stars, song.level};
                    // don_chan.delete_song(title);
                    // don_chan.add_new_song(new_song);
                    don_chan.replace_song(title,new_song);
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
                don_chan.delete_song(title);

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

void new_song_menu(TaikoDatabase& don_chan){
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
        
        if (don_chan.does_it_exist(title)){
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
        if (don_chan.is_this_level_valid(level)){
            break;
        }
        cout << "Enter a valid level (" << don_chan.min_allowed_level << "-" <<
        don_chan.max_allowed_level << ")" << endl << YOU;
    }


    string level_s = {level};
    string message = "Add: " + title + " (" + to_string(stars) 
    + "☆) " + "at level " + level_s + "? (Y/n)";
    
    bool decision;
    ask_yn(message, decision);
    if (decision){
        Song new_song = {title,stars,level};
        don_chan.add_new_song(new_song);
        cout << "Added" << endl;
    } else {
        cout << "Cancelled" << endl;
    }
}

void undo_redo(TaikoDatabase &don_chan, UR action, stringstream& term){
	string potential_number;
	term >> potential_number;

	int amount = 1;
	if (!potential_number.empty()){
		try {
			amount = stoi(potential_number);
		} catch (invalid_argument& e) {
			cout << "Not an integer" << endl;
			return;
		} catch (out_of_range& e){
			cout << "I can only undo/redo between -2,147,483,648 and 2,147,483,647 times" << endl;
			return;
		}

		if (amount == 0){
			cout << "Ok bro????????" << endl;
			return;
		}

		if (amount < 0){ //because lol why the fuck not. This is a feature and if you dont like it maybe learn to use a keyboard properly
			amount *= -1;
			if (action == UR::Undo){
				action = UR::Redo;
			} else {
				action = UR::Undo;
			}
		}
	}
	
	if (action == UR::Undo){
		int total_changes = don_chan.undo(amount);
		cout << "Undid " << total_changes << " changes" << endl;
	} else {
		int total_changes = don_chan.redo(amount);
		cout << "Redid " << total_changes << " changes" << endl;
	}
}
