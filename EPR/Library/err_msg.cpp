#include "err_msg.hpp"


void FileError::printError(){
    std::string errorString = "";
    if (_errorCode  == 1){
        errorString += "Error Opening File: ";
    }
    else if (_errorCode  == 2){
        errorString += "Error reading File: ";
    }
    else if (_errorCode  == 3){
        errorString += "Error number of elements <= 0 File: ";
    }
    else if (_errorCode  == 4){
        errorString += "Malloc error File: ";
    }
    else if (_errorCode  == 99){
        errorString += "Unknown Error File: ";
    }
    else {
        errorString += "Warning File: ";
    }
    errorString += _fileName;
    std::cout << errorString << std::endl;
}

int FileError::getErrorCode(){
    return this->_errorCode;
}

std::string FileError::getFileName(){
    return this->_fileName;
}