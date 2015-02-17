all:
	gcc -fPIC -c libLab5.c -o libLab5.o
	gcc -shared -o libLab5.so libLab5.o -lrt
	gcc main.cpp -o lab5 -pthread -ldl
