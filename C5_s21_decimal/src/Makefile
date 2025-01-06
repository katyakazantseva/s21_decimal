CC = gcc
STDFLAGS = -Wall -Werror -Wextra -std=c11
GCOV_FLAGS=-fprofile-arcs -ftest-coverage 
LINUX_FLAGS=-lsubunit -lrt -lpthread -lm
TESTFLAGS = -lcheck -lm -lpthread -o 

SRC=$(wildcard *s21_*.c)
OBJ=$(patsubst %.c,%.o, ${SRC})

OS := $(shell uname -s)

ifeq ($(OS),Linux)
  OPEN_CMD = xdg-open
endif
ifeq ($(OS),Darwin)
	OPEN_CMD = open
endif

TEST_SRC=$(wildcard ./tests//s21_*.c)
LIBS=-lcheck

all : clean s21_decimal.a test

s21_decimal.a: ${SRC} 
	$(CC) -c $(STDFLAGS) $(SRC)
	ar rc $@ $(OBJ)
	ranlib $@
	cp $@ lib$@
	make clean_obj

test : s21_decimal.a
ifeq ($(OS), Darwin)
	${CC} $(STDFLAGS) ${TEST_SRC}  $< -o unit_test $(LIBS) -lm
else
	${CC} $(STDFLAGS) ${TEST_SRC}  $< -o unit_test $(LIBS) $(LINUX_FLAGS)
endif
	./unit_test
	
gcov_report: clean_test 
ifeq ($(OS), Darwin)
	$(CC) $(STDFLAGS)  $(GCOV_FLAGS) ${TEST_SRC} ${SRC}  -o test $(LIBS) -lm
else
	$(CC) $(STDFLAGS) $(GCOV_FLAGS) ${TEST_SRC} ${SRC} -o test $(LIBS) $(LINUX_FLAGS)
endif
	./test
	lcov -t "stest" -o s21_test.info --no-external --filter range -c -d .
	genhtml -o report s21_test.info
	$(OPEN_CMD) ./report/index.html

clean_obj :
	rm -f *.o

clean_test:
	rm -rf *.gcda
	rm -rf *.gcno
	rm -rf *.info
	rm -rf test
	rm -rf report
	rm -rf unit_test

 clean : clean_obj clean_test
	rm -f *.a
	rm -f check
