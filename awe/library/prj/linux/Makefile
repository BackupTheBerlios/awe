.PHONY: all clean veryclean

CC=gcc
LIB=libawe.a
INCLUDE=../../include/awe/
AWEDIR=./
OBJ=./obj/
SRC=../../src/
OBJECTS=linkedlist.o symbintree.o font.o mouse.o skin.o gdi.o com.o input.o event.o widget.o dataobjects.o draganddrop.o geomman.o control.o pushbutton.o togglebutton.o label.o checkbox.o radio.o

ifeq "$(DEBUGMODE)" "1"
FLAGS=-Wall -g
DEFS=-DDEBUGMODE
else
FLAGS=-Wall -O2 -ffast-math -fomit-frame-pointer -funroll-loops
endif

ifeq "$(ALFONT)" "1"
DEFS+=-DTTFONT
endif

ifeq "$(LOADPNG)" "1"
DEFS+=-DLOADPNG
endif

LIBDIR=./
UNIXDIR_D=/usr/local
OBJECTS += input_linux.o

./unix/libawe.a: $(addprefix $(OBJ),$(OBJECTS))
	#ar cr $(LIBDIR)libawe.a $(OBJ)*.o
	#ranlib $(LIBDIR)libawe.a
	gcc -shared $(OBJ)*.o -o $(LIBDIR)libawe.a `allegro-config --libs` -lpthread
	@echo Make Completed Successfully
	@echo Run Make Install
install:
	-mkdir $(UNIXDIR_D)/include/awe
	cp ../../include/awe.h $(UNIXDIR_D)/include
	cp ../../include/awe/*.* $(UNIXDIR_D)/include/awe
	cp ./libawe.a $(UNIXDIR_D)/lib
uninstall:
	rm -f $(UNIXDIR_D)/include/awe.h
	rm -r $(UNIXDIR_D)/include/awe/*.*
	rm -f $(UNIXDIR_D)/lib/libawe.a


$(OBJ)linkedlist.o: $(SRC)linkedlist.c $(INCLUDE)$(AWEDIR)linkedlist.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)linkedlist.c -o $(OBJ)linkedlist.o $(DEFS)
$(OBJ)symbintree.o: $(SRC)symbintree.c $(INCLUDE)$(AWEDIR)symbintree.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)symbintree.c -o $(OBJ)symbintree.o $(DEFS)
$(OBJ)font.o: $(SRC)font.c $(INCLUDE)$(AWEDIR)font.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)font.c -o $(OBJ)font.o $(DEFS)
$(OBJ)mouse.o: $(SRC)mouse.c $(INCLUDE)$(AWEDIR)mouse.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)mouse.c -o $(OBJ)mouse.o $(DEFS)
$(OBJ)skin.o: $(SRC)skin.c $(INCLUDE)$(AWEDIR)skin.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)skin.c -o $(OBJ)skin.o $(DEFS)
$(OBJ)gdi.o: $(SRC)gdi.c $(INCLUDE)$(AWEDIR)gdi.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)gdi.c -o $(OBJ)gdi.o $(DEFS)
$(OBJ)com.o: $(SRC)com.c $(INCLUDE)$(AWEDIR)com.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)com.c -o $(OBJ)com.o $(DEFS)
$(OBJ)input.o: $(SRC)input.c $(INCLUDE)$(AWEDIR)input.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)input.c -o $(OBJ)input.o $(DEFS)
$(OBJ)event.o: $(SRC)event.c $(INCLUDE)$(AWEDIR)event.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)event.c -o $(OBJ)event.o $(DEFS)
$(OBJ)widget.o: $(SRC)widget.c $(INCLUDE)$(AWEDIR)widget.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)widget.c -o $(OBJ)widget.o $(DEFS)
$(OBJ)dataobjects.o: $(SRC)dataobjects.c $(INCLUDE)$(AWEDIR)dataobjects.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)dataobjects.c -o $(OBJ)dataobjects.o $(DEFS)
$(OBJ)draganddrop.o: $(SRC)draganddrop.c $(INCLUDE)$(AWEDIR)draganddrop.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)draganddrop.c -o $(OBJ)draganddrop.o $(DEFS)
$(OBJ)geomman.o: $(SRC)geomman.c $(INCLUDE)$(AWEDIR)geomman.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)geomman.c -o $(OBJ)geomman.o $(DEFS)
$(OBJ)control.o: $(SRC)control.c $(INCLUDE)$(AWEDIR)control.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)control.c -o $(OBJ)control.o $(DEFS)
$(OBJ)pushbutton.o: $(SRC)pushbutton.c $(INCLUDE)$(AWEDIR)pushbutton.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)pushbutton.c -o $(OBJ)pushbutton.o $(DEFS)
$(OBJ)togglebutton.o: $(SRC)togglebutton.c $(INCLUDE)$(AWEDIR)togglebutton.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)togglebutton.c -o $(OBJ)togglebutton.o $(DEFS)
$(OBJ)label.o: $(SRC)label.c $(INCLUDE)$(AWEDIR)label.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)label.c -o $(OBJ)label.o $(DEFS)
$(OBJ)checkbox.o: $(SRC)checkbox.c $(INCLUDE)$(AWEDIR)checkbox.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)checkbox.c -o $(OBJ)checkbox.o $(DEFS)
$(OBJ)radio.o: $(SRC)radio.c $(INCLUDE)$(AWEDIR)radio.h
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)radio.c -o $(OBJ)radio.o $(DEFS)
$(OBJ)input_win32.o: $(SRC)input_win32.c
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)input_win32.c -o $(OBJ)input_win32.o $(DEFS)
$(OBJ)input_linux.o: $(SRC)input_linux.c
	$(CC) $(FLAGS) -I$(INCLUDE)$(AWEDIR) -c $(SRC)input_linux.c -o $(OBJ)input_linux.o $(DEFS) -lpthread
clean:
	rm -f $(OBJ)*.o
veryclean:
	rm -f $(OBJ)*.o
	rm -f $(LIBDIR)*.a
