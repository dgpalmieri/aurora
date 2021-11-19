#include <string>
#include <iostream>

#ifndef ERR_MSG_HPP
#define ERR_MSG_HPP

class FileError{
    std::string _fileName;
    int _errorCode;
    public:
        FileError(std::string fileName, int errorCode):_fileName(fileName), _errorCode(errorCode) {};
        void printError();
        int getErrorCode();
        std::string getFileName();
};

#endif