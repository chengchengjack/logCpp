#ifndef MYLOGCPP_LOGGER_H
#define MYLOGCPP_LOGGER_H

#include <fileManageMent.h>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <mutex>
#include <queue>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>

/**
ERROR： 此信息输出后，主体系统核心模块不能正常工作，需要修复才能正常工作。
WARN：   此信息输出后，系统一般模块存在问题，不影响系统运行。
INFO：     此信息输出后，主要是记录系统运行状态等关联信息。
DEBUG： 最细粒度的输出，除去上面各种情况后，你希望输出的相关信息，都可以在这里输出。
TRACE：  最细粒度的输出，除去上面各种情况后，你希望输出的相关信息，都可以在这里输出。
 */

namespace log {
#define Error1 __LOGNAME__(Error)
#define Warn1 __LOGNAME__(Warn)
#define Info1 __LOGNAME__(Info)
#define Debug1 __LOGNAME__(Debug)
#define Trace1 __LOGNAME__(Trace)

#define __LOGTIME__   Logger::getInstance()->getLogCoutTime()       //时间宏
#define __LOGPID__    Logger::getInstance()->getLogCoutProcessId()  //进程宏
#define __LOGTID__    Logger::getInstance()->getLogCoutThreadId()   //线程宏
#define __USERNAME__  Logger::getInstance()->getLogCoutUserName()   //获取调用用户名字

#define __LOGFILE__  __FILE__          //文件名宏
#define __LOGFUNC__  __func__          //函数名宏
#define __LOGLINE__  __LINE__          //行数宏
#define __LOGNAME__(name) #name        //名字宏

    const int MEGABYTES = 1048576;
    const std::string  SQUARE_BRACKETS_LEFT  = " [";
    const std::string  SQUARE_BRACKETS_RIGHT = "] ";
    const std::string SPACE = " ";
    const std::string LINE_FEED = "\n";
    const std::string COLON  = ":";
    const std::string SLASH = "/";
    const std::string DEFA = "\e[0m";
    const std::string SWITCH_OFF = "off";
    const std::string SWITCH_ON = "on";
    const std::string RED = "\e[1;31m";
    const std::string BLUE = "\e[1;34m";
    const std::string GREEN = "\e[1;32m";
    const std::string WHITE = "\e[1;37m";
    const std::string PURPLE = "\e[1;35m";


    enum class coutType :int {
        Error,
        Warn,
        Info,
        Debug,
        Trace
    };

    using std::string;

    struct Log {
        string logSwitch;               //日志开关
        string logFileSwitch;           //是否写入文件
        string logTerminalSwitch;       //是否打印终端
        string logFileQueueSwitch;      //是否开启队列策略
        string logName;                 //日志文件名称
        string logFilePath;             //日志保存路径
        string logMaxSize;              //日志最大文件大小
        string logBehavior;             //日志文件达到最大大小行为
        string logOutputLevelFile;      //日志输出等级文件
        string logOutputLevelTerminal;  //日子输出等级终端
    };

    class Logger {
    public:
        static std::mutex *terminal_mutex;

        void initLogConfig(); //初始化配置

        void releaseConfig();

        void configInfoPrint();

        static Logger *getInstance(); //获取实例

        string getLogCoutTime();

        string getLogNameTime();

        string getSourceFilePath();

        string getFilePath();

        string getFilePathAndName();

        string getFilePathAndNameTime();

        string getLogCoutThreadId(); //获取线程id

        string getLogCoutProcessId();

        string getLogCoutUserName();

        string getLogSwitch();

        string getLogFileSwitch();

        string getLogTerminalSwitch();

        string getCoutType(coutType ctype);

        string getCoutTypeColor(string colorType);

        bool getFileType(coutType ctype);

        bool getTerminalType(coutType ctype);

        bool logFileWrite(string messages, string message, string line);

        bool insertQueue(string messages, string filePathName);

        bool bindFileCoutMap(string value1, coutType value2);

        bool bindTerminalCoutMap(string value1, coutType value2);

        class Recycle {
        public:
            ~Recycle() {
                if (getInstance() != nullptr) {
                    delete singleObject;
                    singleObject == nullptr;
                    delete terminal_mutex;
                    terminal_mutex = nullptr;
                    delete file_mutex;
                    file_mutex = nullptr;
                    delete log_mutex;
                    log_mutex = nullptr;
                    delete queue_mutex;
                    queue_mutex = nullptr;

                }
            }
        };

    private:
        char sourceFilePath[128]; //输出路径
        Log m_log;
        FileManagement fileManagement;
        static Logger *singleObject; //单例对象
        static std::mutex *log_mutex;
        static std::mutex *file_mutex;
        static std::mutex *queue_mutex;
        std::map<coutType, std::string> coutTypeMap;
        std::map<coutType, bool> fileCOutMap;
        std::map<coutType, bool> terminalCoutMap;
        std::map<string, string> coutColor;
        std::queue<std::string> messageQueue;

        //构造函数
        Logger();

        ~Logger();
    };

#define  KV(value) " " << #value << "=" << value

    //rdbuf(), 可以实现哪一个流对象指向的内容用另一个流对象输出
    //cout, cin 支持重定向，cout 只能输出屏幕，cinjianpan，重定向指定文件
    // 还原标准流，从键盘输入std::cout.rdbuf(pOldBuf);
    //放入ret  std::string ret = oss.str();
#define COMBINATION_INFO_FILE(coutTypeInfo, message) \
    do{\
        std::ostringstream oss;\
        std::streambuf* pOldBuf = std::cout.rdbuf(oss.rdbuf());\
        std::cout << message;\
        std::string ret = oss.str();\
        std::cout.rdbuf(pOldBuf);\
        std::string messagesAll = __LOGTIME__ + coutTypeInfo + __USERNAME__ + __LOGTID__ + SQUARE_BRACKETS_LEFT + \
        __LOGFILE__  + SPACE +__LOGFUNC__ + COLON + std::to_string(__LOGLINE__) + SQUARE_BRACKETS_RIGHT;\
        Logger::getInstance()->logFileWrite(messagesAll, ret, LINE_FEED); \
    }while(0);

    //no
#define COMBINATION_INFO_TERMINAL(coutTypeInfo, message) \
    do{\
        std::string color = Logger::getInstance()->getCoutTypeColor(coutTypeInfo);\
        std::string logFormatCout = __LOGTIME__ + color + coutTypeInfo + DEFA + __USERNAME__ + __LOGTID__ + SQUARE_BRACKETS_LEFT + \
        __LOGFILE__  + SPACE +__LOGFUNC__ + COLON + std::to_string(__LOGLINE__) + SQUARE_BRACKETS_RIGHT;\
        Logger::terminal_mutex->lock(); \
        std::cout << logFormatCout << message << LINE_FEED;\
        fflush(stdout);\
        Logger::terminal_mutex->unlock(); \
    }while(0);


/**
 * @brief 获取类型
 * 是否开启日志
 * 是否写入文件
 * 
 * 
 * 是否写入终端
 */

#define LoggerCout(coutTyle, coutTypeInfo, fileCoutBool, terminalCoutBool, message) \
    do {\
        std::string coutType = Logger::getInstance()->getCoutType(coutTyle);\
        if( SWITCH_ON == Logger::getInstance()->getLogSwitch()){ \       
            if (SWITCH_OFF != Logger::getInstance()->getLogFileSwitch()){\
                if (Logger::getInstance()->getFileType(fileCoutBool)) {\
                    COMBINATION_INFO_FILE(coutTypeInfo, message)\
                }\
            }\
            if (SWITCH_OFF != Logger::getInstance()->getLogTerminalSwitch()){\
                if (Logger::getInstance()->getTerminalType(terminalCoutBool)) {\
                    COMBINATION_INFO_TERMINAL(coutTypeInfo, message)\
                }\
            }\
        }\
    }while(0);

#define logError(...) \
    do{\
        LoggerCout(log::coutType::Error, Error1, log::coutType::Error, log::coutType::Error, __VA_ARGS__)\
    }while(0);

#define logWarn(...)  \
    do{\
        LoggerCout(log::coutType::Warn, Warn1, log::coutType::Warn, log::coutType::Warn, __VA_ARGS__)\
    }while(0);

#define logInfo(...)  \
    do{\
        LoggerCout(log::coutType::Info, Info1, log::coutType::Info, log::coutType::Info, __VA_ARGS__)\
    }while(0);

#define logDebug(...) \
    do{\
        LoggerCout(log::coutType::Debug, Debug1, log::coutType::Debug, log::coutType::Debug, __VA_ARGS__)\
    }while(0);

#define logTrace(...) \
    do{\
        LoggerCout(log::coutType::Trace, Trace1, log::coutType::Trace, log::coutType::Trace, __VA_ARGS__)\
    }while(0);
}


#endif //MYLOGCPP_LOGGER_H
