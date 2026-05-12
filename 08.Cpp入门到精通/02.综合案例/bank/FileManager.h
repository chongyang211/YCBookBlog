//
// Created by 杨充 on 2025/7/16.
//

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <limits>
#include <fstream>
#include <sstream>
#include "Bank.h"
#ifndef FILEMANAGER_H
#define FILEMANAGER_H



class FileManager {
public:
    static void saveToFile(const Bank& bank, const std::string& filename);
    // 从文件加载账户数据
    static void loadFromFile(Bank& bank, const std::string& filename);
};


#endif //FILEMANAGER_H
