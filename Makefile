make:
	gcc -o test test.c && g++ -o  ipfw_test ipfw_test.cc ipfw.cc -std=c++20

