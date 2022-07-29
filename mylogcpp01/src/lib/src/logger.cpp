//
// Created by cheng on 2022/7/27.
//

#include "logger.h"
#include <string.h>


log::Logger *log::Logger::singleObject = nullptr;
std::mutex *log::Logger::terminal_mutex = new(mutex);
std::mutex *log::Logger::log_mutex = new(mutex);
std::mutex *log::Logger::file_mutex = new(mutex);
std::mutex *log::Logger::queue_mutex = new(mutex);

void log::Logger::initLogConfig() {
    coutColor["Error"] = "\e[1;31m"; //字体格式
    coutColor["Warn"] = "\e[1;35m";
    coutColor["Info"] = "\e[1;34m";
    coutColor["Debug"] = "\e[1;32m";
    coutColor["Trace"] = "\e[1;37m";

    map<std::string, std::string *> logConfigInfo;

    logConfigInfo["logSwitch"] = &this->m_log.logSwitch;
    logConfigInfo["logFileSwitch"] = &this->m_log.logFileSwitch;
    logConfigInfo["logTerminalSwitch"] = &this->m_log.logTerminalSwitch;
    logConfigInfo["logFileQueueSwitch"] = &this->m_log.logFileQueueSwitch;
    logConfigInfo["logName"] = &this->m_log.logName;
    logConfigInfo["logFilePath"] = &this->m_log.logFilePath;
    logConfigInfo["logMaxSize"] = &this->m_log.logMaxSize;
    logConfigInfo["logBehavior"] = &this->m_log.logBehavior;
    logConfigInfo["logOutputLevelFile"] = &this->m_log.logOutputLevelFile;
    logConfigInfo["logOutputLevelTerminal"] = &this->m_log.logOutputLevelTerminal;

    bool isOpen = true;
    string str;
    ifstream ifs;
    char str_c[100] = {0};
//    memset(&str_c, 0, sizeof(str_c));
    ifs.open("../../conf/logConfig.conf"); //配置文件

    if (!ifs.is_open()) { //打开失败
        isOpen = false;
        cout << "file open failed" << endl;
    }

    while (getline(ifs, str)) { //读取配置 从文件
        if (!str.size()) { //为0继续读取
            continue;
        }
        // string str_copy = str; 
        string str_copy(str.size(), '\0'); //临时
        int j = 0;
        for (int i = 0; i < str.size(); i++) {
            if (str[i] == ' ') continue; //遇到空格继续
            str_copy[j] = str[i]; //复制内容
            j++;
        }
        // str_copy.erase(j);
        if (str_copy[0] != '#') { //首位不为标识符 ,后面配置
            sscanf(str_copy.data(), "%[^=]", str_c); //从str_cop读取数据，格式化,获取=号前面的内容
            auto iter = logConfigInfo.find(str_c);
            if (iter != logConfigInfo.end()) {
                sscanf(str_copy.data(), "%*[^=]=%s", str_c); //获取等号后面， %*跳过数据
                *iter->second = str_c; //放到配置map ,同时struct log也赋值了
            }
        }
    }
    ifs.close();

    //初始化map
    bindFileCoutMap("5", coutType::Error);
    bindFileCoutMap("4", coutType::Warn);
    bindFileCoutMap("3", coutType::Info);
    bindFileCoutMap("2", coutType::Debug);
    bindFileCoutMap("1", coutType::Trace);

    bindTerminalCoutMap("5", coutType::Error);
    bindTerminalCoutMap("4", coutType::Warn);
    bindTerminalCoutMap("3", coutType::Info);
    bindTerminalCoutMap("2", coutType::Debug);
    bindTerminalCoutMap("1", coutType::Trace);

    string filePathAndName = getFilePathAndName();
    string filepath = getFilePath();
    cout << filePathAndName << ":" << filepath << endl;
    if (m_log.logFileSwitch == SWITCH_ON) { //是否写入文件
        //创建文件路径 文件夹
        fileManagement.createFilePath(filepath);
        //检测文件是否可以访问
        if (!fileManagement.verifyFileExistence(filePathAndName)) {
            fileManagement.createFile(filePathAndName); //文件路径不存在，创建文件路径
        } else {
            long fileSize = fileManagement.verifyFileSize(filePathAndName);
            if (fileSize > (long) atoi(m_log.logMaxSize.data()) * MEGABYTES && m_log.logBehavior == "1") { //文件容量不足
                string newFileName = getFilePathAndNameTime(); //再常见新的文件夹
                fileManagement.fileRename(filePathAndName, newFileName); //改名
                fileManagement.createFile(filePathAndName); //再创建文件
            }
        }
    }

    if (isOpen) {
        configInfoPrint();
    }
    return;
}

void log::Logger::releaseConfig() {

}

void log::Logger::configInfoPrint() { //输出配置信息
    for (int i = 0; i < m_log.logFilePath.size() + 15; i++) {
        cout << GREEN << "-";
        if (i == ((m_log.logFilePath.size() + 15) / 2)) {
            cout << "Logger";
        }
    }

    cout << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  日志开关　　" << m_log.logSwitch << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  文件输出　　" << m_log.logFileSwitch << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  终端输出开关" << m_log.logTerminalSwitch << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  文件输出等级" << m_log.logOutputLevelFile << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  终端输出等级" << m_log.logOutputLevelTerminal << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  日志队列策略" << m_log.logFileQueueSwitch << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  日志文件名称" << m_log.logName << ".log" << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  日志保存路径" << m_log.logFilePath << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  日志文件大小" << m_log.logMaxSize << "M" << DEFA << endl;
    for (int i = 0; i < m_log.logFilePath.size() + 25; i++) {
        cout << GREEN << "-" << DEFA;
    }
    cout << endl;
}

log::Logger *log::Logger::getInstance() {
    //call_once() once_flag g_flag
    if (singleObject == nullptr) {
        unique_lock<std::mutex> myM(*log_mutex);;
        if (singleObject == nullptr) {
            singleObject = new Logger();
            static Recycle re; //回收单例
        }
    }
    return singleObject;
}

string log::Logger::getLogCoutTime() {
    time_t time1;
    time(&time1);

    char temp[64];
//    memset(&temp, 0, sizeof(temp));
    strftime(temp, sizeof(temp), "%Y-%m_%d %H:%M:%s", localtime(&time1)); //输出格式日期
    string temp_str = temp;
    return SQUARE_BRACKETS_LEFT + temp_str + SQUARE_BRACKETS_RIGHT;
}


string log::Logger::getLogNameTime() {
    time_t time1;
    time(&time1);
    char temp[64];
    strftime(temp, sizeof(temp), "%Y-%m-%d-%H:%M:%S", localtime(&time1));
    return temp;
}


string log::Logger::getSourceFilePath() {
    cout << 1 << endl;
    getcwd(sourceFilePath, sizeof(sourceFilePath) - 1);//获取当前工作路径

    string sourceFilePath_str = sourceFilePath;
    return sourceFilePath_str + SLASH;
}

string log::Logger::getFilePath() {
    return m_log.logFilePath + SLASH;
}

string log::Logger::getFilePathAndName() {
    return m_log.logFilePath + SLASH + m_log.logName + ".log";
}

string log::Logger::getFilePathAndNameTime() { //创建爱你带有时间路径
    return m_log.logFilePath + m_log.logName + getLogNameTime() + ".log";
}

string log::Logger::getLogCoutThreadId() {
    return to_string(syscall(__NR_gettid));
}

string log::Logger::getLogCoutProcessId() {
    return to_string(getpid());
}

string log::Logger::getLogCoutUserName() {
    struct passwd *my_info;
    my_info = getpwuid(getuid());
    string name = my_info->pw_name;
    return SPACE + name + SPACE;
}

string log::Logger::getLogSwitch() {
    return m_log.logSwitch;
}

string log::Logger::getLogFileSwitch() {
    return m_log.logFileSwitch;
}

string log::Logger::getLogTerminalSwitch() {
    return m_log.logTerminalSwitch;
}

string log::Logger::getCoutType(log::coutType ctype) {
    return getInstance()->coutTypeMap[ctype];
}

string log::Logger::getCoutTypeColor(string colorType) {
    return coutColor[colorType];
}

bool log::Logger::getFileType(log::coutType ctype) {
    return getInstance()->fileCOutMap[ctype];
}

//no
bool log::Logger::getTerminalType(log::coutType ctype) {
    return getInstance()->terminalCoutMap[ctype];
}

bool log::Logger::logFileWrite(string messages, string message, string line) {
    string filePathAndName = getFilePathAndName();

    long fileSize = fileManagement.verifyFileSize(filePathAndName);
    //atoi字符串转为整形
    if (fileSize > (long) atoi(m_log.logMaxSize.data()) * MEGABYTES
        && m_log.logBehavior == "1") { //判断容量
        string newFileName = getFilePathAndNameTime();
        fileManagement.fileRename(filePathAndName, newFileName);
        fileManagement.createFile(filePathAndName);
    }
    if (m_log.logFileQueueSwitch == SWITCH_OFF) { //是否开启队列策略
        file_mutex->lock();
        ofstream file;
        file.open(filePathAndName, ::ios::app | ios::out);//追加
        file << messages << message << line;
        file.close();
        file_mutex->unlock();
    } else {
        insertQueue(messages + message + line, filePathAndName);
    }
    return true;
}

bool log::Logger::insertQueue(string messages, string filePathName) {
    queue_mutex->lock();
    messageQueue.push(messages);
    if (messageQueue.size() >= 5000) {//存满足够
        file_mutex->lock(); //
        ofstream ofs; //向文件写内容
        ofs.open(filePathName, ::ios::app | ios::out);
        if (!ofs.is_open()) {
            cout << "file to failed" << endl;
            return false;
        }
        while (!messageQueue.empty()) { //不为空
            ofs << messageQueue.front(); //写入文件
            messageQueue.pop();
        }
        ofs.close();
        file_mutex->unlock();
    }
    queue_mutex->unlock();
    return true;
}

bool log::Logger::bindFileCoutMap(string value1, coutType value2) {
    if (m_log.logOutputLevelFile.find(value1) != string::npos) { //接收否到末尾
        fileCOutMap[value2] = true;
    } else {
        fileCOutMap[value2] = false;
    }
    return true;
}

bool log::Logger::bindTerminalCoutMap(string value1, coutType value2) {
    //npos可以表示string的结束位子，是string::type_size 类型的，也就是find（）返回的类型。find函数在找不到指定值得情况下会返回string::npos。
    if (m_log.logOutputLevelTerminal.find(value1) != string::npos) { //npos是一个常数，表示size_t的最大值
        terminalCoutMap[value2] = true;
    } else {
        terminalCoutMap[value2] = false;
    }
    return true;
}

log::Logger::Logger() {
    initLogConfig();
}

log::Logger::~Logger() {

}
