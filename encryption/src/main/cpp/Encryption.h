//
// Created by xfding on 24-3-28.
//

#ifndef WENET_ENCRYPTION_H
#define WENET_ENCRYPTION_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <chrono>

namespace vwm {
    using namespace std;

#define MAGIC_NUMBER 1985
#ifndef ENCRYPTION_KEY
#define ENCRYPTION_KEY "TEST"  // 默认值
#endif

    static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const char* VWM_KEY = ENCRYPTION_KEY;

    class Encryption {
    private:
        static inline bool is_base64(unsigned char c) {
            return (isalnum(c) || (c == '+') || (c == '/'));
        }

    public:
        static std::string base64_decode(const std::string &encoded_string) {
            int in_len = encoded_string.size();
            int i = 0;
            int j;
            int in_ = 0;
            unsigned char char_array_4[4], char_array_3[3];
            std::string ret;

            while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
                char_array_4[i++] = encoded_string[in_];
                in_++;
                if (i == 4) {
                    for (i = 0; i < 4; i++)
                        char_array_4[i] = base64_chars.find(char_array_4[i]);

                    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                    char_array_3[1] =
                            ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                    for (i = 0; (i < 3); i++)
                        ret += char_array_3[i];
                    i = 0;
                }
            }

            if (i) {
                for (j = i; j < 4; j++)
                    char_array_4[j] = 0;

                for (j = 0; j < 4; j++)
                    char_array_4[j] = base64_chars.find(char_array_4[j]);

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

                for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
            }

            return ret;
        }

        static std::string base64_encode(const unsigned char *data, size_t len) {
            std::string encoded;
            int i = 0;
            int j;
            unsigned char char_array_3[3];
            unsigned char char_array_4[4];

            while (len--) {
                char_array_3[i++] = *(data++);
                if (i == 3) {
                    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                    char_array_4[1] =
                            ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                    char_array_4[2] =
                            ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                    char_array_4[3] = char_array_3[2] & 0x3f;

                    for (i = 0; (i < 4); i++)
                        encoded += base64_chars[char_array_4[i]];
                    i = 0;
                }
            }

            if (i) {
                for (j = i; j < 3; j++)
                    char_array_3[j] = '\0';

                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

                for (j = 0; (j < i + 1); j++)
                    encoded += base64_chars[char_array_4[j]];

                while ((i++ < 3))
                    encoded += '=';
            }

            return encoded;
        }

        // 定义用于标识加密文件的结构体
        struct EncryptionHeader {
            int magic_number;
            int version;
        };

// 解密方法
        static bool decrypt(std::istream &inputStream,
                            std::ostream *outputStream,
                            long &size,
                            int &version) {
            if (!inputStream) {
                cerr << "Error: inputStream error." << endl;
                return false;
            }

            // 读取加密文件标识结构体
            EncryptionHeader header{};
            inputStream.read(reinterpret_cast<char *>(&header), sizeof(EncryptionHeader));

            if (header.magic_number != MAGIC_NUMBER) {
                // 获取文件大小
                inputStream.seekg(0, ios::end);
                size = inputStream.tellg();

                inputStream.seekg(0, ios::beg);
                unique_ptr<char[]> buffer(new char[size]);
                inputStream.read(buffer.get(), size);
                outputStream->write(buffer.get(), size);
                return false;
            } else {
                version = header.version;
                // 获取文件大小
                inputStream.seekg(0, ios::end);
                size = (long) inputStream.tellg() - (long) sizeof(EncryptionHeader);
                inputStream.seekg(sizeof(EncryptionHeader), ios::beg);

                char ch;
                int keyLength = (int) strlen(VWM_KEY);
                int keyIndex = 0;
                while (inputStream.get(ch)) {
                    // 使用XOR解密算法
                    ch = ch ^ VWM_KEY[keyIndex];
                    outputStream->put(ch);

                    // 循环使用密钥中的字符
                    keyIndex = (keyIndex + 1) % keyLength;
                }
                return true;
            }
        }

// 加密方法
        static bool encrypt(std::istream &inputStream,
                            std::ostream *outputStream) {
            if (!inputStream) {
                cerr << "Error: inputStream error." << endl;
                return false;
            }

            // 写入加密文件标识结构体
            EncryptionHeader header{};
            header.magic_number = MAGIC_NUMBER;  // 假设一个魔数用于标识加密文件
            header.version = GetDate();
            outputStream->write(reinterpret_cast<char *>(&header),
                                sizeof(EncryptionHeader));

            // 使用的密钥
            const char *key = VWM_KEY;
            int keyLength = (int) strlen(key);
            int keyIndex = 0;

            char ch;
            while (inputStream.get(ch)) {
                // 使用XOR加密算法
                ch = ch ^ key[keyIndex];
                outputStream->put(ch);

                // 循环使用密钥中的字符
                keyIndex = (keyIndex + 1) % keyLength;
            }

            cout << "encrypted successfully." << endl;
            return true;
        }

    private:
        static int GetDate() {
            // 获取当前系统时间点
            auto now = std::chrono::system_clock::now();

            // 将时间点转换为 time_t
            std::time_t now_time = std::chrono::system_clock::to_time_t(now);

            // 将 time_t 转换为本地时间结构体 tm
            struct tm *timeinfo = std::localtime(&now_time);

            // 获取年份、月份和日期
            int year = timeinfo->tm_year + 1900; // tm_year 表示从 1900 开始的年份偏移量
            int month = timeinfo->tm_mon + 1;    // tm_mon 表示月份偏移量（0 - 11）
            int day = timeinfo->tm_mday;         // 当月的日期（1 - 31）

            // 将年份、月份和日期转换为整数值
            return year * 10000 + month * 100 + day;
        }
    };

}  // namespace vwm

#endif  // WENET_ENCRYPTION_H
