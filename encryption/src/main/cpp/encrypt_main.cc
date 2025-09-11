//
// Created by xfding on 24-3-28.
//

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Encryption.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        printf(
                "encrypt [original module] [encrypted module]\n"
                "Example:\n"
                "encrypt ./test.onnx ./encrypted.onnx");
        return -1;
    }
    std::ifstream inputFile(argv[1], std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error: Unable to open input file." << std::endl;
        return -1;
    }
    std::ostringstream oss;
    vwm::Encryption::encrypt(inputFile, &oss);
    std::string encrypted = oss.str();
    std::string encoded = vwm::Encryption::base64_encode(
            reinterpret_cast<const unsigned char *>(encrypted.c_str()), encrypted.length());

    std::ofstream outfile(argv[2]);
    outfile << encoded;
    outfile.close();
    inputFile.close();
    return 0;
}