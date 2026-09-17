//
// Created by 杨充 on 2026/5/27.
//

#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include "Account.h"
#include <vector>
#include <string>


class FileManager {
public:
    // 保存
    static bool save(const std::string& fileName, const std::vector<Account*> accounts);
    // 获取
    static std::vector<Account*> load(const std::string& fileName);
};



#endif //FILEMANAGER_H
