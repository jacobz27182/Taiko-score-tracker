#pragma once
#include<sstream>

enum class UR{
	Undo,
	Redo
};

void console_loop(TaikoDatabase& don_chan);
void search_interface(TaikoDatabase &don_chan, stringstream& search_term);
void song_menu(TaikoDatabase &don_chan, string title);
void new_song_menu(TaikoDatabase& don_chan);
void undo_redo(TaikoDatabase& don_chan, UR action, stringstream& term);