all: htrtrainer.c
	gcc -o htrtrainer htrtrainer.c
cleanall: clean cleantxt
clean:
	rm -f htrtrainer scramble.txt return.txt outputhtr.txt
cleantxt:
	rm -f *.txt
