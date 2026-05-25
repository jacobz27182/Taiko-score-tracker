CXXFLAGS = -Wall -Wextra -Wpedantic
DIR = src/
JUNK = junk/

.PHONY: clean

ass: $(JUNK)taiko.o $(JUNK)helpers.o $(JUNK)main.o $(JUNK)ui.o
	g++ $(CXXFLAGS) $^ -o $@

$(JUNK):
	mkdir -p $(JUNK)

$(JUNK)main.o: $(DIR)main.cpp $(DIR)taiko.h $(DIR)helpers.h $(DIR)constants.h $(DIR)ui.h | $(JUNK)
	g++ $(CXXFLAGS) -c $(DIR)main.cpp -o $@

$(JUNK)ui.o: $(DIR)ui.cpp $(DIR)ui.h $(DIR)taiko.h $(DIR)constants.h $(DIR)helpers.h | $(JUNK)
	g++ $(CXXFLAGS) -c $(DIR)ui.cpp -o $@

$(JUNK)taiko.o: $(DIR)taiko.cpp $(DIR)taiko.h $(DIR)helpers.h $(DIR)constants.h | $(JUNK)
	g++ $(CXXFLAGS) -c $(DIR)taiko.cpp -o $@

$(JUNK)helpers.o: $(DIR)helpers.cpp $(DIR)helpers.h $(DIR)constants.h | $(JUNK)
	g++ $(CXXFLAGS) -c $(DIR)helpers.cpp -o $@

clean:
	rm -f $(JUNK)*.o ass