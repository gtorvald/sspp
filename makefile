G++ = g++
NAME = go
SRCS = main.cpp
SRCS_TEST_GENERATE = generating.java
TEST_GENERATE = generating
SRCS_TEST_TESTING = testing.java
TEST_TESTING = testing
FILE_TIME = time.txt

.PHONY: all test generate fclean report

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

report: all
	@echo "Testing time for float matrix 300x300 ..."
	@touch $(FILE_TIME) ; \
	for index in ijk ikj jik jki kij kji ; do \
	for ((i = 0; i < 10; i++)) ; do \
	./$(NAME) "test/f 300*300 A" "test/f 300*300 B" file $$index plot >> $(FILE_TIME) ; \
	done ; done;
	@rm  file
	@python3 plot.py < $(FILE_TIME)
	@rm $(FILE_TIME)
	@Echo "Tested"

fclean:
	@rm -f $(TEST_GENERATE).class $(TEST_TESTING).class $(NAME)
	@Echo "Cleaned"

re: fclean all
	