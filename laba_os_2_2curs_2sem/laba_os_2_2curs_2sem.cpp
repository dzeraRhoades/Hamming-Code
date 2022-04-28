#define _CRT_SECURE_NO_WARNINGS
#include"Hamming.h"


int main()
{
    Hamming h;
    int argc = 7;
    char argv[7][15];
    strcpy(argv[1], "-i");
    strcpy(argv[2], "output.txt");
    strcpy(argv[3], "-o");
    strcpy(argv[4], "decoded.txt");
    strcpy(argv[5], "-n");
    strcpy(argv[6], "10");
    //h.encodingApp(argc, argv);
    h.decodingApp(argc, argv);
}

