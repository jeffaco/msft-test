all:
	time -f "%e" compile test.cpp

deps:
	depends test.cpp > depends.out
	depstats depends.out | awk '{ print $$2 }' > files
	compile `cat files` > compile.out
	grep "@@@.*[0-9]\.[0-9][0-9]" compile.out > deps.out
	cat deps.out | awk '{ print $$3 " " $$2; }' | sort
