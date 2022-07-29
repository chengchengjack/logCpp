#include "logger.h"
#include <iostream>

using namespace log;

int main() {
    time_t  begin, end;
    double ret;
    begin = clock();
    string name = "lmc";
    for (int i = 0; i < 10000; ++i) {
        logError("error" << name);
        logWarn("warn" << name);
        logInfo("info" << name);
        logDebug("debug" << name);
        logTrace("trace" << name);
    }
    end = clock();
    ret = double(end - begin)/CLOCKS_PER_SEC;
    cout << "runtimeL=" << ret << endl;

    return 0;

}
