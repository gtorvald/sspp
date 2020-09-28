G++ = g++
NAME = go
SRCS = main.cpp
SRCS_TEST_GENERATE = generating.java
TEST_GENERATE = generating
SRCS_TEST_TESTING = testing.java
TEST_TESTING = testing

.PHONY: all test generate fclean

all:
	@$(G++) $(SRCS) -o $(NAME)

generate: all
	@for type in f d ; do \
	for size in 10 50 200 300 500 1000; do \
	javac $(SRCS_TEST_GENERATE) ; \
	java $(TEST_GENERATE) $$type $$size $$size "test/$$type $$size*$$size A" "test/$$type $$size*$$size B" ; \
	./$(NAME) "test/$$type $$size*$$size A" "test/$$type $$size*$$size B" "test/$$type $$size*$$size C" ikj ; \
	done ; done

test: all
	@for type in f d ; do \
	for size in 10 50 200 ; do \
	for index in ijk ikj jik jki kij kji ; do \
	./$(NAME) "test/$$type $$size*$$size A" "test/$$type $$size*$$size B" "test/$$type $$size*$$size C test" $$index ; \
	javac $(SRCS_TEST_TESTING) ; \
	if !(java -ea $(TEST_TESTING) "test/$$type $$size*$$size C test" "test/$$type $$size*$$size C" $$index) ; then \
		rm -f "test/$$type $$size*$$size C test" ; \
		exit 1 ; \
	fi ; \
	rm -f "test/$$type $$size*$$size C test" ; \
	done ; done ; done

fclean:
	rm -f $(TEST_GENERATE).class $(TEST_TESTING).class $(NAME)