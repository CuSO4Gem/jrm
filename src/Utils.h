#ifndef UTILS_H
#define UTILS_H

#include <memory>

#include "date.h"
#include "Journal.h"

#include <string>

using namespace::std;
using namespace::ec;

/**
 * @brief remove multiple space from string
 */
void removeMultipleSpaces(string &str);

/**
 * @brief Get the Value From Config object
 * 
 * @param config 
 * @param key 
 * @param value 
 * @return true find
 * @return false not find
 */
bool getValueFromConfig(const string &config, const string &key, string &value);

/**
 * @brief Set the Value To Config object
 * 
 * @param config 
 * @param key 
 * @param value 
 * @return true 
 * @return false 
 */
bool setValueToConfig(string &config, const string &key,const string value);

/**
 * @brief Get the Stamp Form Config string
 * 
 * @param config 
 * @return time_t 
 */
time_t getStampFormConfig(const string &config);

/**
 * @brief parse string to journal
 * 
 * @param inStr 
 * @return shared_ptr<Journal> 
 */
shared_ptr<Journal> strToJournal(const string &inStr);

/**
 * @brief conver rawPath to a valid path
 *  remove spaces at begin, conver ~ to home dir
 * 
 * @param rawPath 
 * @return string 
 */
string validPath(string rawPath);

/**
 * @brief get path from user, conver ~ to home dir
 * 
 * @return string 
 */
string getFilePath();
#endif