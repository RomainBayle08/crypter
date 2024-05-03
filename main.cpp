
#include "crypter.h"

#define BUFFER_SIZE 1024*1024


int main()
{

    char* init_key = "0123456789ABCDEFFEDCBA9876543210";
    char* in_file = "C:\\Users\\romain\\CLionProjects\\crypter\\test.txt";
    char* exit = "C:\\Users\\romain\\CLionProjects\\crypter\\test2.txt";
    char* dec = "C:\\Users\\romain\\CLionProjects\\crypter\\test3.txt";

   /*Crypter crypter("C:\\Users\\romain\\CLionProjects\\crypter\\",init_key);
    crypter.encrypt(init_key,in_file,exit);
    crypter.decrypt(init_key,exit,dec);*/

    ByteArray key, enc;
    size_t file_len;

    FILE *input, *output;

    srand(time(0));



    size_t key_len = 0;
    while(key_init[key_len] != 0)
        key.push_back(key_init[key_len++]);

    input = fopen(input_file, "rb");
    if (input == 0) {
        fprintf(stderr, "Cannot read file '%s'\n", input_file);
        exit(1);
    }

    output = fopen(output_file, "wb");
    if (output == 0) {
        fprintf(stderr, "Cannot write file '%s'\n", output_file);
        exit(1);
    }

    AES_256 aes(key);

    fseeko64(input, 0, SEEK_END);
    file_len = ftell(input);
    fseeko64(input, 0, SEEK_SET);
    printf("File is %zd bytes\n", file_len);
    if(file_len > 1 ) {

        enc.clear();
        aes.encrypt_start(file_len, enc);
        fwrite(enc.data(), enc.size(), 1, output);

        while (!feof(input)) {
            unsigned char buffer[BUFFER_SIZE];
            size_t buffer_len;

            buffer_len = fread(buffer, 1, BUFFER_SIZE, input);
            printf("Read %zd bytes\n", buffer_len);
            if (buffer_len > 0) {
                enc.clear();
                aes.encrypt_continue(buffer, buffer_len, enc);
                fwrite(enc.data(), enc.size(), 1, output);
            }
        }

        enc.clear();
        aes.encrypt_end(enc);
        fwrite(enc.data(), enc.size(), 1, output);
        printf(enc.data());
        fclose(input);
        fclose(output);


    return 0;
}