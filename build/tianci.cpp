#include <iostream>
#include <mysql/mysql.h>
#include "json.hpp"
#include <fstream>
#include <string>
#include <ctime>
#include "global.h"

// TODO: should connect to the real book_circulation table!!

// 为了简化代码，使用 nlohmann::json 别名
using json = nlohmann::json;

nlohmann::json output; // 返回结果为json对象
// 数据库连接配置
// 服务器地址
static const char *server = "82.156.207.47";
// 用户名
static const char *user = "tianci";
// 密码
static const char *password = "doozyexact";
// 数据库名
static const char *database = "test_zhu";

// 函数声明
MYSQL *connectToDatabase();
bool createTable(MYSQL *conn);
std::string updateBooktotalNum(const nlohmann::json &book);
void writeToCancellationRecord(MYSQL *conn, const std::string &ISBN, const std::string &reason, const std::string &adminName);
std::string getCurrentTime();
bool readJsonFile(const std::string &filePath, json &jsonData);

/**
 * @brief 注销报损入口
 *
 * 该函数负责连接数据库，创建表，读取JSON文件，更新图书数量，写入注销记录，并关闭数据库连接。
 * 需要流通库表的API
 * @return int 返回0表示程序正常结束，返回1表示程序异常结束。
 */
int jsonCancel(json jsonData)
{
    // 连接数据库
    MYSQL *conn = connectToDatabase();
    if (!conn)
    {
        std::string Message = "无法连接到数据库" + std::string(mysql_error(conn));
        output["status"] = "failure";
        output["message"] = Message;
        std::cout << output << std::endl;
        // std::cerr << "无法连接到数据库" << std::endl;
        return 1;
    }

    // 创建表
    createTable(conn);

    // std::cout << jsonData << std::endl;
    // 从 JSON 数据中获取 ISBN 号、注销理由和管理员名称
    std::string ISBN = jsonData["ISBN"];
    std::string reason = jsonData["reason"];
    std::string adminName = jsonData["adminname"];

    json book;
    book["ISBN"] = ISBN;

    // 写入注销记录
    std::string resultapi = DecBookNum(book);
    if (resultapi.find("success") != std::string::npos)
    {
        writeToCancellationRecord(conn, ISBN, reason, adminName);
        output["status"] = "success";
        output["message"] = "注销报损成功";
        std::cout << output << std::endl;
    }
    else
    {
        output["status"] = "failture";
        output["message"] =resultapi;
        std::cout << output << std::endl;
    }

    // 关闭数据库连接
    mysql_close(conn);
    return 0;
}

/**
 * @brief 连接到 MySQL 数据库
 *
 * 该函数初始化一个 MySQL 连接对象，并尝试连接到指定的数据库。
 *
 * @return MYSQL* 返回一个指向 MySQL 连接对象的指针，如果连接失败则返回 nullptr。
 */
MYSQL *connectToDatabase()
{
    MYSQL *conn = mysql_init(nullptr);
    if (!conn)
    {
        std::string Message = "mysql_init() failed" + std::string(mysql_error(conn));
        output["status"] = "failure";
        output["message"] = Message;
        // std::cout << output << std::endl;
        // std::cerr << "mysql_init() failed" << std::endl;
        return nullptr;
    }

    if (!mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0))
    {
        std::string Message = "mysql_real_connect() failed: " + std::string(mysql_error(conn));
        output["status"] = "failure";
        output["message"] = Message;
        // std::cout << output << std::endl;
        // std::cerr << "mysql_real_connect() failed: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return nullptr;
    }

    return conn;
}

/**
 * @brief 创建注销记录表
 *
 * 该函数在数据库中创建一个名为 CancellRecordTable 的表，用于存储注销记录。
 *
 * @param conn 指向 MySQL 连接对象的指针。
 */
bool createTable(MYSQL *conn)
{
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS CancellRecordTable ("
                                   "ISBN VARCHAR(255),"
                                   "reason VARCHAR(255),"
                                   "adminname VARCHAR(255),"
                                   "time VARCHAR(255))";
    if (mysql_query(conn, createTableQuery) != 0)
    {
        std::string Message = "表不存在，而且创建表失败: " + std::string(mysql_error(conn));
        output["status"] = "failure";
        output["message"] = Message;
        return false;
    }
    else
    {
        // std::cout << "表创建成功" << std::endl;
        return true;
    }
}

/**
 * @brief 更新图书数量
 *
 * 该函数根据提供的 ISBN 号，在 book_circulation 表中减少对应图书的数量。
 *
 * @param conn 指向 MySQL 连接对象的指针。
 * @param ISBN 图书的 ISBN 号。
 */
/*
std::string updateBooktotalNum(const nlohmann::json &book)
{

    // 初始化MySQL连接
    MYSQL *conn = mysql_init(nullptr);
    if (!conn)
    {
        // 如果连接初始化失败，返回错误信息
        return "mysql_init() failed";
    }

    // 连接到MySQL数据库
    if (!mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0))
    {
        // 如果连接数据库失败，构造错误信息
        std::string error = "mysql_real_connect() failed: ";
        // 获取 MySQL 错误信息并添加到错误信息中
        error += mysql_error(conn);
        // 关闭 MySQL 连接
        mysql_close(conn);
        // 返回错误信息
        return error;
    }

    // 获取ISBN号
    std::string ISBN = book["ISBN"];
    // 检查图书是否存在
    std::string query = "SELECT totalNum FROM book_circulation WHERE ISBN = '" + ISBN + "'";
    if (mysql_query(conn, query.c_str()) != 0)
    {
        // std::cerr << "查询图书失败: " << mysql_error(conn) << std::endl;
        return "查询图书失败: " + std::string(mysql_error(conn));
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (!result)
    {

        // std::cerr << "获取查询结果失败: " << mysql_error(conn) << std::endl;
        return "获取查询结果失败: " + std::string(mysql_error(conn));
    }

    if (mysql_num_rows(result) == 0)
    {
        // std::cerr << "不存在该图书" << std::endl;
        return "不存在该图书" + std::string(mysql_error(conn));
        mysql_free_result(result);
    }
    else
    {
        // 更新图书数量
        query = "UPDATE book_circulation SET totalNum = totalNum - 1, leftNum = leftNum -1 WHERE ISBN = '" + ISBN + "'";
        if (mysql_query(conn, query.c_str()) != 0)
        {

            // std::cerr << "更新图书数量失败: " << mysql_error(conn) << std::endl;
            return "更新图书数量失败: " + std::string(mysql_error(conn));
        }
        else
        {
            std::string Message = "图书数量更新成功";
            output["status"] = "success";
            output["message"] = Message;
            // std::cout << output << std::endl;
            //  std::cout << "图书数量更新成功" << std::endl;
        }
    }

    mysql_free_result(result);
    return "success";
}
*/

/**
 * @brief 写入注销记录
 *
 * 该函数将注销记录插入到 CancellRecordTable 表中。
 *
 * @param conn 指向 MySQL 连接对象的指针。
 * @param ISBN 图书的 ISBN 号。
 * @param reason 注销理由。
 * @param adminName 管理员名称。
 */
void writeToCancellationRecord(MYSQL *conn, const std::string &ISBN, const std::string &reason, const std::string &adminName)
{
    std::string query = "INSERT INTO CancellRecordTable (ISBN,reason,adminname,time) VALUES"
                        "('" +
                        ISBN + "', '" + reason + "', '" + adminName + "', '" + getCurrentTime() + "')";
    if (mysql_query(conn, query.c_str()) != 0)
    {
        std::string Message = "写入注销记录失败: " + std::string(mysql_error(conn));
        output["status"] = "failure";
        output["message"] = Message;
        // std::cout << output << std::endl;
        // std::cerr << "写入注销记录失败: " << mysql_error(conn) << std::endl;
    }
    else
    {
        std::string Message = "写入注销记录成功";
        output["status"] = "success";
        output["message"] = Message;

        // std::cout << "写入注销记录成功" << std::endl;
    }
}

/**
 * @brief 获取当前时间
 *
 * 该函数返回当前的日期和时间，格式为 "YYYY-MM-DD HH:MM:SS"。
 *
 * @return std::string 当前的日期和时间。
 */
std::string getCurrentTime()
{
    std::time_t now = std::time(nullptr);
    std::tm *currentTime = std::localtime(&now);

    char timeStr[26];
    std::strftime(timeStr, 26, "%Y-%m-%d %H:%M:%S", currentTime);
    // char timeStr[11]; // 因为 "yy - mm - dd" 格式最多需要 10 个字符，加上字符串结束符 '\0'
    // std::strftime(timeStr, 11, "%Y-%m-%d", currentTime);
    return std::string(timeStr);
}

/**
 * @brief 读取 JSON 文件
 *
 * 该函数从指定的文件路径中读取 JSON 数据，并将其存储在提供的 json 对象中。
 *
 * @param filePath JSON 文件的路径。
 * @param jsonData 用于存储 JSON 数据的 json 对象。
 * @return bool 如果读取成功则返回 true，否则返回 false。
 */
// bool readJsonFile(const std::string &filePath, json &jsonData)
// {
//     std::ifstream file(filePath);
//     if (!file.is_open())
//     {
//         std::string Message = "无法打开文件: " + filePath;
//         output["status"] = "failure";
//         output["message"] = Message;
//         // std::cout << output << std::endl;
//         // std::cerr << "无法打开文件: " << filePath << std::endl;
//         return false;
//     }

//     file >> jsonData;
//     file.close();
//     return true;
// }
bool readJsonFile(const std::string &filePath, json &jsonData)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::string message = "无法打开文件: " + filePath;
        // json output; // 定义 output
        output["status"] = "failure";
        output["message"] = message;
        std::cerr << output << std::endl;
        return false;
    }

    try
    {
        file >> jsonData;
        file.close();
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "读取 JSON 文件时出错: " << e.what() << std::endl;
        return false;
    }
}
