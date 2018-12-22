all: lbrk

lbrk: main.cc
	g++ -g -Wall -Wextra -std=c++1z $< -o $@

clean:
	rm -f lbrk *~

test:
	@echo "Test: read from stdin behaves similarly as reading from file."
	bash -c "diff <(./lbrk -tuw40 foo.txt) <(cat foo.txt | ./lbrk -tuw40)"
