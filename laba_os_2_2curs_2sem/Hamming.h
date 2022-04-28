#pragma once
#include <iostream>
#include <cmath>
#include<string>

class Hamming
{
public:
	void encodingApp(const int argc, const char argv[7][15]);
	void decodingApp(const int argc, const char argv[7][15]);
private:
	int encode(char* source, char* dest, int num);
	int decode(char* source, char* dest, int num);
	void showBinary(char* arr, int num);
	int controlBits(int bitNum); // return number of control bits
	void setBit(char* source, char* dest, int pos_s, int pos_d);
	void setControlBit(char* source, int deg, int size);
	int compareControlBits(char* source, char* correct, int cb);
	int sumAllBits(char* arr, int size);
	void readArgs(const int argc, const char argv[7][15], char* input, char* output, int& wordlen);
	std::string getBinaryString(char* code, int size);

};

