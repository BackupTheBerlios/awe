ifeq "$(DEBUGMODE)" "1"
FLAGS=-Wall -g
DEFS=-DDEBUGMODE
else
FLAGS=-Wall -O2 -ffast-math -fomit-frame-pointer -funroll-loops
endif

ifdef DJDIR
include makefile.dj
else
ifdef MINGDIR
include makefile.mgw
else
include makefile.unx
endif
endif

all: $(ALL)
ex01.o: ex01.c
	$(CC) $(FLAGS) -c ex01.c -o ex01.o $(DEFS)
$(EX01): ex01.o
	$(CC) $(FLAGS) ex01.o -o $(EX01) $(AL_LINK)
ex02.o: ex02.c
	$(CC) $(FLAGS) -c ex02.c -o ex02.o $(DEFS)
$(EX02): ex02.o
	$(CC) $(FLAGS) ex02.o -o $(EX02) $(AL_LINK)
ex03.o: ex03.c
	$(CC) $(FLAGS) -c ex03.c -o ex03.o $(DEFS)
$(EX03): ex03.o
	$(CC) $(FLAGS) ex03.o -o $(EX03) $(AL_LINK)
ex04.o: ex04.c
	$(CC) $(FLAGS) -c ex04.c -o ex04.o $(DEFS)
$(EX04): ex04.o
	$(CC) $(FLAGS) ex04.o -o $(EX04) $(AL_LINK)
ex05.o: ex05.c
	$(CC) $(FLAGS) -c ex05.c -o ex05.o $(DEFS)
$(EX05): ex05.o
	$(CC) $(FLAGS) ex05.o -o $(EX05) $(AL_LINK)
clean:
	rm -f *.o
veryclean:
	rm -f *.o
	rm -f $(EX01)
	rm -f $(EX02)
	rm -f $(EX03)
	rm -f $(EX04)
	rm -f $(EX05)
	rm -f *.log
compress:
	upx -9 $(EX01)
	upx -9 $(EX02)
	upx -9 $(EX03)
	upx -9 $(EX04)
	upx -9 $(EX05)