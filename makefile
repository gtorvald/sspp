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
	@$(G++) -std=c99 $(SRCS) -o $(NAME)

generate: all
	@for type in f ; do \
	for size in 1000 2000 3000 4000 5000; do \
	javac $(SRCS_TEST_GENERATE) ; \
	java $(TEST_GENERATE) $$type $$size $$size "test/$$type $$size*$$size A" "test/$$type $$size*$$size B" ; \
	./$(NAME) "test/$$type $$size*$$size A" "test/$$type $$size*$$size B" "test/$$type $$size*$$size C" ikj ; \
	done ; done

test: all
	@for type in f ; do \
	for size in 1000 2000 ; do \
	for index in ijk ikj ; do \
	./$(NAME) "test/$$type $$size*$$size A" "test/$$type $$size*$$size B" "test/$$type $$size*$$size C test" $$index 72 ; \
	javac $(SRCS_TEST_TESTING) ; \
	if !(java -ea $(TEST_TESTING) "test/$$type $$size*$$size C test" "test/$$type $$size*$$size C" $$index) ; then \
		rm -f "test/$$type $$size*$$size C test" ; \
		exit 1 ; \
	fi ; \
	rm -f "test/$$type $$size*$$size C test" ; \
	done ; done ; done

report:
	python3 plot.py < $(FILE_TIME)

report11: all
	@echo "One minute pls ..."
	@touch $(FILE_TIME) ;
	@for size in 1000 ; do \
	./$(NAME) "test/f $$size*$$size A" "test/f $$size*$$size B" file ijk 32 plot >> $(FILE_TIME) ; \
	done ;
	@for size in 1000 ; do \
	./$(NAME) "test/f $$size*$$size A" "test/f $$size*$$size B" file ikj 32 plot >> $(FILE_TIME) ; \
	done ;
	@for size in 1000 ; do \
	./$(NAME) "test/f $$size*$$size A" "test/f $$size*$$size B" file ikj 72 plot >> $(FILE_TIME) ; \
	done ;
	@rm  file
	@python3 plot.py < $(FILE_TIME)
	@rm $(FILE_TIME)
	@Echo "Tested"

fclean:
	@rm -f $(TEST_GENERATE).class $(TEST_TESTING).class $(NAME)
	@Echo "Cleaned"

re: fclean all
	