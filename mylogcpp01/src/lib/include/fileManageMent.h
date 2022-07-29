#ifndef MYLOGCPP_FILEMANAGEMENT_H
#define MYLOGCPP_FILEMANAGEMENT_H

#include <iostream>
#include <string>
#include <fstream>
#include <time.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>

#define ONEDAY 24 * 60 * 60 //秒

using namespace std;

class FileManagement {
public:
    FileManagement();

    ~FileManagement();

    //创建文件路径
    bool createFilePath(string fileName);

    //创建文件
    bool createFile(string fileName);

    //验证文件存在
    bool verifyFileExistence(string fileName);

    //改名
    bool fileRename(const string oldFile, string newFile);

    //验证长度
    long verifyFileSize(const string fileName);

    //当前时间
    long getCurrentTIme();


};

#endif //MYLOGCPP_FILEMANAGEMENT_H
