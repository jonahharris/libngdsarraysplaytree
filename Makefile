GCOV_OUTPUT = *.gcda *.gcno *.gcov 
GCOV_CCFLAGS = -fprofile-arcs -ftest-coverage
CC     = gcc
#CCFLAGS = -I. -Itests -g -Werror -W -fno-omit-frame-pointer -fno-common -fsigned-char $(GCOV_CCFLAGS)
CCFLAGS = -I. -Itests -g -W -fno-omit-frame-pointer -fno-common -fsigned-char $(GCOV_CCFLAGS)


all: test

main.c:
	sh tests/make-tests.sh tests/test_*.c > main.c

test: main.c ngds_array_splay_tree.o tests/test_ngds_array_splay_tree.c tests/CuTest.c main.c
	$(CC) $(CCFLAGS) -o $@ $^
	./test
	gcov main.c tests/test_ngds_array_splay_tree.c ngds_array_splay_tree.c

ngds_array_splay_tree.o: ngds_array_splay_tree.c
	$(CC) $(CCFLAGS) -c -o $@ $^

clean:
	rm -f main.c ngds_array_splay_tree.o test $(GCOV_OUTPUT)
