#include "taiko.h"
#include "helpers.h"
#include "ui.h"

int main(){
    const string filename = "database.tsv"; 
    
    cout << "loading database..." << endl;
    
	TaikoDatabase don_chan(filename);

    cout << "Welcome, zzykrkv" << endl;

    console_loop(don_chan);
    cout << "See you next time, zzkyrkv" << endl;    
}

