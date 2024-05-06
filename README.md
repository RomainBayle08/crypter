# how to build : 
## with cmake 
```bash
mkdir build
cd build
cmake ..
make
```

## with g++  
```bash
g++ -std=c++17  main.cpp crypter.cpp AES_256.cpp -o crypter.exe

```

# usage : 
```bash
./crypter crypt/decrypt AES256_Key Targeted_directory
```
## key exemple ( for testing purposes ):  
```plaintext
3p9KbR7NqGkTz8h1WdVx5Fy2PcEoXmJl
