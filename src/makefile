KYROLAB_HOME = .

INCD = $(KYROLAB_HOME)
SRCD = $(KYROLAB_HOME)
OBJD = $(KYROLAB_HOME)

OBJECTS = $(OBJD)/kyrolab.o $(OBJD)/record.o

CPPFLAGS = -g -lstdc++ -std=c++14 -pthread

all: kyrolab

kyrolab: $(OBJECTS)
	g++ $(CPPFLAGS) -o kyrolab $(OBJECTS)

$(OBJD)/record.o: $(INCD)/record.h $(INCD)/tensor.h $(INCD)/config.h $(SRCD)/record.cpp
	g++ $(CPPFLAGS) -I$(INCD) -c $(SRCD)/record.cpp -o $(OBJD)/record.o

$(OBJD)/kyrolab.o: $(INCD)/record.h $(INCD)/tensor.h $(INCD)/config.h $(SRCD)/kyrolab.cpp
	g++ $(CPPFLAGS) -I$(INCD) -c $(SRCD)/kyrolab.cpp -o $(OBJD)/kyrolab.o

clean: 
	rm $(KYROLAB_HOME)/*.o
	rm kyrolab

