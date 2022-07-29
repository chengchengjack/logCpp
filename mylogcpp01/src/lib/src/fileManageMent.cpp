//
// Created by cheng on 2022/7/27.
//

#include "../include/fileManageMent.h"

bool FileManagement::createFilePath(string fileName) { //创建文件夹
    int len = fileName.length();
    if (!len) { //长度不为0 ,为0执行
        fileName = "log";

        //F_OK， 判断文件是否存在， F ok指定作用 xok 1判断文件执行权限
        if (access(fileName.c_str(), F_OK) != 0) { //访问
            //创建目录 -1创建失败
            if (mkdir(fileName.c_str(), 0) == -1) { //c_str -> char *
                return false;
            }
        }
    }

    std::string fileName_cy(len, '\0'); //创建string对象, len长度的\0
    for(int i = 0; i < len; i++) {
        fileName_cy[i] = fileName[i];//复制
        if(fileName_cy[i] == '/') { //文件结尾 类似  test/
            if(access(fileName_cy.c_str(), F_OK) == -1) {
                //文件是否存在，失败
                if(mkdir(fileName_cy.c_str(), 0) != 0) { //创建文件夹
                    return false;
                }
            }
        }
    }
    return true;
}

bool FileManagement::createFile(string fileName) { //创建文件
    ofstream ofs;
    ofs.open(fileName, ::ios::app | ios::out); //追加
    if(!ofs) {
        cout << "Failed to create file" << endl;
        return false;
    }
    ofs.close();
    return true;
}

bool FileManagement::verifyFileExistence(string fileName) {
    return (access(fileName.data(), F_OK) != -1); //date（）转char*
}

bool FileManagement::fileRename(const string oldFile, string newFile) {
    if(!rename(oldFile.data(), newFile.data())) { //不成功
        cout << "file rename failed" << endl;
        return false;
    }
    return true;
}

long FileManagement::verifyFileSize(const string fileName) {
    struct stat statBuf; //通过文件名获取文件信息，并保存在buf所指的结构体中
    if(stat(fileName.data(), &statBuf) == 0) { //成功
        return statBuf.st_size;
    }
    return -1;
}

long FileManagement::getCurrentTIme() {
    time_t timep;
    struct tm *p;
    time(&timep); //获得秒数
    p = localtime(&timep); //转换struct tm
    timep = mktime(p); //在转回 秒数
    return timep;
}

FileManagement::FileManagement() {

}

FileManagement::~FileManagement() {

}
