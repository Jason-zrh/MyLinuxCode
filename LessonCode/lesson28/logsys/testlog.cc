#include "log.hpp"

using namespace std;


int main()
{
    log temp;
    temp.logmessage(Info, "error message: %s, exit code: %d", "success", 0);
    temp.logmessage(Debug, "error message: %s, exit code: %d", "debug", 1);
    temp.logmessage(Warning, "error message: %s, exit code: %d", "warning!", 2);
    temp.logmessage(Error, "error message: %s, exit code: %d", "error!", 3);
    temp.logmessage(Fatal, "error message: %s, exit code: %d", "break", 4);
    return 0;
}