#define _CRT_SECURE_NO_WARNINGS
#include "Hamming.h"
#include<fstream>

void Hamming::encodingApp(const int argc, const char argv[7][15])
{
    char ch;
    char input[52];
    char output[56];
    int wordLen;
    int count = 0;
    int k = 0;
    char curNum;
    try
    {
        readArgs(argc, argv, input, output, wordLen);
    }
    catch (const std::exception&)
    {
        std::cerr << "wrong command line arguments" << std::endl;
        return;
    }
    std::ifstream fin(input);
    std::ofstream fout(output);
    if (!fin.is_open())
    {
        std::cerr << "input file can't be opened";
        return;
    }
    if (!fout.is_open())
    {
        std::cerr << "output file can't be opened";
        return;
    }

    int codeSize = wordLen + controlBits(wordLen) + 1;
    char word[256]; 
    char code[256]; 
    memset(word, 0, 256);
    memset(code, 0, 256);
    
    while ((ch = fin.get()) != EOF)
    {
        if (isspace(ch))
            continue;
        k += (count / 8);
        count %= 8;
        curNum = (1 & (ch - '0')) << (7 - count);
        word[k] |= curNum; // записываем очередной бит в слово
        count++;
        if (k * 8 + count == wordLen)
        {
            encode(word, code, wordLen);
            fout << getBinaryString(code, wordLen + controlBits(wordLen) + 1);
            memset(word, 0, 256);
            memset(code, 0, 256);
            k = 0;
            count = 0;
        }
    }
    if (k != 0 || count != 0) // кодируем оставшийся кусок
    {
        encode(word, code, count + k*8);
        fout << getBinaryString(code, (count + k * 8) + controlBits(count + k * 8) + 1);
    }
    fin.close();
    fout.close();
    /// TODO: определяем размер слова
    /// выделяем необходимую память под source и dest
    /// заполняем нулями оба массива с помощью memset
    /// открываем один файл на чтение, а второй на запись
    /// Создаём цикл посимвольного считывания, каждое очередное слово кодируем и сразу записываем в выходной файл результат
    /// функцию кодировки оборачиваем в try чтоб отловить ошибку в случае чего.

}

void Hamming::decodingApp(const int argc, const char** argv)
{
}

int Hamming::encode(char* source, char* dest, int num)
{
    int deg = 0;
    int k1 = 0, k2 = 0;
    int dest_size = num + controlBits(num);
    int j = 0, i = 0;
    int counter = 0;


    while (i + k1 * 8 < dest_size)
    {
        k1 += (i / 8);
        k2 += (j / 8);
        i %= 8;
        j %= 8;

        if (i + 1 + k1 * 8 != pow(2, deg))
        {
            setBit(source + k2, dest + k1, j + 1, i + 1);
            j++;
        }
        else
            deg++;
        i++;
    }

    showBinary(dest, num + controlBits(num));
    std::cout << std::endl;
    for (i = 0; i < controlBits(num); i++)
    {
        setControlBit(dest, i, dest_size);
        //showBinary(dest, num + controlBits(num));
    }
    k1 = dest_size / 8;
    dest[k1] |= sumAllBits(dest, dest_size) << (7 - (dest_size % 8)); // устанавливаем доп бита для подсчета четности числа
    showBinary(dest, dest_size+1);
    return 0;
}

int Hamming::decode(char* source, char* dest, int num)
{
    int deg = 0;
    int k1 = 0, k2 = 0;
    int full_size = num + controlBits(num);
    //int dest_size = num + controlBits(num);
    int j = 0, i = 0;

    char* tmp = new char[(full_size - 1) / 8 + 1];
    for (; i < (full_size - 1) / 8 + 1; i++)
        tmp[i] = 0;
    i = 0;

    while (i + k1 * 8 < full_size)
    {
        k1 += (i / 8);
        k2 += (j / 8);
        i %= 8;
        j %= 8;

        if (i + 1 + k1 * 8 != pow(2, deg))
        {
            setBit(source + k1, dest + k2, i + 1, j + 1);
            setBit(source + k1, tmp + k1, i + 1, i + 1);
            j++;
        }
        else
            deg++;
        i++;
    }
    // TODO: set control bits of tmp and compare it with source by xor, if 0 - it's ok
    showBinary(source, full_size);
    showBinary(tmp, full_size);
    showBinary(dest, num);

    k1 = 0;
    for (i = 0; i < controlBits(num); i++)
    {
        setControlBit(tmp, i, full_size);
        //showBinary(dest, num + controlBits(num));
    }
    if (compareControlBits(source, tmp, controlBits(num)))
        std::cout << "uncorrect" << std::endl;
    delete[] tmp;
    return 0;
}

void Hamming::showBinary(char* arr, int num)
{
    int move;
    for (int i = 1; i <= num; i++)
    {
        move = 8 - (i / 9 + i % 9);
        std::cout << ((arr[i / 9] >> move) & 1);
    }
    std::cout << std::endl;
}

int Hamming::controlBits(int bitNum)
{
    int a = logb(bitNum) + 1;
    if (bitNum + a >= pow(2, a))
        a++;
    return a;
}

void Hamming::setBit(char* source, char* dest, int pos_s, int pos_d)
{
    char moved = *source >> (8 - pos_s) & 1;
    *dest = *dest | (moved << (8 - pos_d));
}

void Hamming::setControlBit(char* source, int deg, int size)
{
    int bits = 0;
    int count = 0;
    int k = 0;
    int i = pow(2, deg) - 1;

    while (i + k * 8 < size)
    {
        k += (i / 8);
        i %= 8;

        for (int j = 0; j < pow(2, deg); j++)
        {
            if (source[k] >> (8 - i - 1) & 1)
                bits = (bits + 1) % 2;
            i++;
            k += (i / 8);
            i %= 8;
        }
        i += pow(2, deg);

    }
    if (bits)
    {
        k = ((int)pow(2, deg) - 1) / 8;
        std::cout << std::endl;
        int move = 1 << (8 - (((int)pow(2, deg) - 1) % 8) - 1);
        source[k] |= move;
    }
}

int Hamming::compareControlBits(char* source, char* correct, int cb)
{
    int error_bit = 0;
    int move;
    int k;
    for (int i = 0; i < cb; i++)
    {
        move = pow(2, i);
        k = (move - 1) / 8;
        if ((source[k] >> move & 1) ^ (correct[k] >> move & 1))
            error_bit += move;
    }
    if (error_bit)
    {
        k = (error_bit - 1) / 8;
        move = 8 - ((error_bit - 1) % 8) - 1;
        source[k] ^= ((source[k] >> move & 1) << move); // setting correct bit
        return 1;
    }
    return 0;
}

int Hamming::sumAllBits(char* arr, int size)
{
    int res = 0;
    int k = 0;
    int i = 0;
    int num;
    while (i + k * 8 < size)
    {
        num = ((arr[k] >> (7 - i)) & 1);
        res ^= num;
        i++;
        k += (i / 8);
        i %= 8;
    }
    return res;
}

void Hamming::readArgs(const int argc, const char argv[7][15], char* input, char* output, int& wordlen)
{
    if (argc != 7)
        throw std::exception();
    int i = 1;
    while (i < argc)
    {
        if (!strcmp(argv[i], "-n")) // word len
        {
            wordlen = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-i"))
        {
            strcpy(input, argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-o"))
        {
            strcpy(output, argv[i + 1]);
        }
        else throw std::exception();
        i += 2;
    }
    
}

std::string Hamming::getBinaryString(char* code, int size)
{
    int i = 0, k = 0;
    std::string res = "";
    while (i + k * 8 < size)
    {
        k += (i / 8);
        i %= 8;
        res += std::to_string((code[k] >> (7-i)) & 1);
        i++;
    }
    return res;
}
