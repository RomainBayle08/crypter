
#include "crypter.h"

#define BUFFER_SIZE 1024*1024


int main()
{

    char* init_key = "0123456789ABCDEFFEDCBA9876543210";
    char* in_file = "C:\\Users\\romain\\CLionProjects\\crypter\\test.txt";
    char* exit = "C:\\Users\\romain\\CLionProjects\\crypter\\test2.txt";
    char* dec = "C:\\Users\\romain\\CLionProjects\\crypter\\test3.txt";

    Crypter crypter("C:\\Users\\romain\\CLionProjects\\crypter\\",init_key);
    crypter.encrypt(init_key,in_file,exit);
    crypter.decrypt(init_key,exit,dec);


    return 0;
}