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
    std::ifstream file(argv[1]); // 打开文件

    std::string input((std::istreambuf_iterator<char>(file)),
                      (std::istreambuf_iterator<char>())); // 从文件中读取内容到字符串
    file.close(); // 关闭文件

    std::string decoded = vwm::Encryption::base64_decode(input);
    std::istringstream iss(decoded);
    std::ofstream oss(argv[2]);
    long size;
    int version;
    vwm::Encryption::decrypt(iss, &oss, size, version);
    oss.close();
    return 0;
}
