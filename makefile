G++ = gcc
NAME = go
SRCS = main.c
SRCS_TEST_GENERATE = generating.java
TEST_GENERATE = generating
SRCS_TEST_TESTING = testing.java
TEST_TESTING = testing
FILE_TIME = time.txt

.PHONY: all test generate fclean report

all:
	@$(G++) -std=c99 -L/usr/lib64 -lpapi $(SRCS) -o $(NAME)

generate: all
	@for type in f ; do \
	for size in 10 30 ; do \
	javac $(SRCS_TEST_GENERATE) ; \
	java $(TEST_GENERATE) $$type $$size $$size "test/$$type $$size*$$size A" "test/$$type $$size*$$size B" ; \
	./$(NAME) "test/$$type $$size*$$size A" "test/$$type $$size*$$size B" "test/$$type $$size*$$size C" ikj 72; \
	done ; done

test: all
	@for type in f ; do \
	for size in 10 30 50 100 150 ; do \
	for index in ijk ikj ; do \
	./$(NAME) "test/$$type $$size*$$size A" "test/$$type $$size*$$size B" "test/$$type $$size*$$size C test" $$index 72 ; \
	javac $(SRCS_TEST_TESTING) ; \
	if !(java -ea $(TEST_TESTING) "test/$$type $$size*$$size C test" "test/$$type $$size*$$size C" $$index) ; then \
		rm -f "test/$$type $$size*$$size C test" ; \
		exit 1 ; \
	fi ; \
	rm -f "test/$$type $$size*$$size C test" ; \
	done ; done ; done

report: report11
	python3 plot.py < $(FILE_TIME)

report11: all
	@echo "One minute pls ..."
	@for size in 10 30 50 100 150 ; do \
	./$(NAME) "test/f $$size*$$size A" "test/f $$size*$$size B" file ijk 32 plot >> $(FILE_TIME); \
	done ;
	@for size in 10 30 50 100 150 ; do \
	./$(NAME) "test/f $$size*$$size A" "test/f $$size*$$size B" file ikj 32 plot >> $(FILE_TIME); \
	done ;
	@for size in 10 30 50 100 150 ; do \
	./$(NAME) "test/f $$size*$$size A" "test/f $$size*$$size B" file ikj 72 plot >> $(FILE_TIME); \
	done ;
	@rm  file
	@Echo "Tested"

fclean:
	@rm -f $(TEST_GENERATE).class $(TEST_TESTING).class $(NAME)
	@Echo "Cleaned"

re: fclean all
	
