FLAGS = -Wall -Wextra -Wpedantic
DIR = src/
JUNK = junk/

ass: $(JUNK)taiko.o $(JUNK)helpers.o $(JUNK)main.o $(JUNK)ui.o
	g++ $(FLAGS) $(JUNK)taiko.o $(JUNK)helpers.o $(JUNK)main.o $(JUNK)ui.o -o ass

$(JUNK)main.o: $(DIR)main.cpp $(DIR)taiko.h $(DIR)helpers.h $(DIR)constants.h
	g++ $(FLAGS) -c $(DIR)main.cpp -o $(JUNK)main.o

$(JUNK)ui.o: $(DIR)ui.h $(DIR)ui.cpp $(DIR)taiko.cpp $(DIR)taiko.h $(DIR)constants.h
	g++ $(FLAGS) -c $(DIR)ui.cpp -o $(JUNK)ui.o
	
$(JUNK)taiko.o: $(DIR)taiko.cpp $(DIR)taiko.h $(DIR)helpers.h $(DIR)constants.h
	g++ $(FLAGS) -c $(DIR)taiko.cpp -o $(JUNK)taiko.o

$(JUNK)helpers.o: $(DIR)helpers.cpp $(DIR)helpers.h $(DIR)constants.h
	g++ $(FLAGS) -c $(DIR)helpers.cpp -o $(JUNK)helpers.o

clean:
	rm -f $(JUNK)helpers.o $(JUNK)main.o $(JUNK)taiko.o $(JUNK)ui.o ass