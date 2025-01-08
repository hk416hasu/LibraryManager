#define _CRT_SECURE_NO_WARNINGS 1 //sprintf有警告
#include <mysql/mysql.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <string>
#include "json.hpp"
#include "global.h"
using namespace std;  

#define SQL_MAX 256		// sql语句字符数组最大值
static MYSQL* conn;
static MYSQL_RES* res;
static MYSQL_ROW row;

static const char* host = "82.156.207.47";
static const char* user = "yixuan";
static const char* passwd = "yarnimprovise";
static const char* db = "test_zhang";


MYSQL* connectToDB(const char* host, const char* user, const char* passwd, const char* db);
void releaseDB(MYSQL* conn, MYSQL_RES* res);
std::string extractString(const nlohmann::json& j, const std::string& key);
/*
string ifExit(std::string ISBN);
std::string ifExitIncBook(const nlohmann::json& Book);
std::string InsertRecord(const nlohmann::json& Book);
std::string DecBookNum(const nlohmann::json& Book);
*/
std::string ifExitIncBook(const nlohmann::json& book);
std::string InsertRecord(const nlohmann::json& book);
std::string DecBookNum(const nlohmann::json& book);


int GetLeftNum(const nlohmann::json Borrow);
//int GetTotalNum(const nlohmann::json Borrow);

void BorrowBook(const nlohmann::json Borrow);
int BorrowAuthority(const nlohmann::json Borrow);//ok
int AddBorrowList(const nlohmann::json Book);//ok

int DecreaseLeftNum(const nlohmann::json Borrow);//ok
int ifLeft(const nlohmann::json Borrow);//ok
int ifCredit(const nlohmann::json Borrow);//ok
int ifBelowLimit(const nlohmann::json Borrow);

void ReturnBook(const nlohmann::json Borrow);
int ifOverDue(const nlohmann::json Return);
int DelBorrowList(const nlohmann::json Borrow);
int IncreaseNum(const nlohmann::json Return);
int IncreaseLeftNum(const nlohmann::json Return);

void jsonBorrowBook(nlohmann::json Borrow);
void jsonReturnBook(nlohmann::json Borrow);



/*int main() {
    nlohmann::json Borrow;
    Borrow["ISBN"] = "0000020183760";
    Borrow["callNum"] = "Y7891";
    Borrow["CLCNum"] = "W5371";
    Borrow["bookTitle"] = "InsertBookTest";
    Borrow["author"] = "zyx";
    Borrow["press"] = "zyx press";
    Borrow["pressDate"] = "2025-01-02";
    Borrow["introduction"] = " ";
    Borrow["uID"] = "00002201";
    Borrow["userName"] = "ztc";
    Borrow["credit"] = 1;//用户子系统
    Borrow["borrowDate"] = "2025-01-08";
    Borrow["ifOver"] = "0";
    Borrow["dueDate"] = "2025-02-07";
    Borrow["borrowNum"] = 2;
    Borrow["borrowLimit"] = 5;//用户子系统
    // std::string exit= ifExitIncBook(Borrow);
     //std::cout << exit << endl;

    std::string insert = DecBookNum(Borrow);
    std::cout << insert << endl;

    return 0;
}
*/

MYSQL* connectToDB(const char* host, const char* user, const char* passwd, const char* db) {// 封装数据库连接
    MYSQL* conn = mysql_init(NULL);
    if (conn == NULL) {
        std::cerr << "MySQL initialization failed" << std::endl;
        return NULL;
    }
    if (mysql_real_connect(conn, host, user, passwd, db, 0, NULL, 0) == NULL) {
        std::cerr << "MySQL connection failed: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return NULL;
    }
    return conn;
}
void releaseDB(MYSQL* conn, MYSQL_RES* res) {// 封装数据库资源释放
    if (res != NULL) {
        mysql_free_result(res);
    }
    if (conn != NULL) {
        mysql_close(conn);
    }
}

// 辅助函数，从json对象中提取字符串
std::string extractString(const nlohmann::json& j, const std::string& key) {

    if (j.contains(key)) {
        return j[key];
    }
    return "";
}

std::string ifExitIncBook(const nlohmann::json& book)
{
    // 初始化MySQL连接
    MYSQL* conn = mysql_init(nullptr);
    if (!conn)
    {
        // 如果初始化失败，返回相应信息
        std::string Message = std::string("初始化MySQL连接失败");
       // output["status"] = "failure";
        //output["message"] = Message;
        //std::cout << output << std::endl;
        // std::cerr << "mysql_init() failed" << std::endl;
        return Message;
    }

    // 连接到MySQL数据库
    if (!mysql_real_connect(conn, host, user, passwd, db, 0, nullptr, 0))
    {
       // std::string Message = "mysql_real_connect() failed: " + std::string(mysql_error(conn));
        //output["status"] = "failure";
       // output["message"] = Message;
        //std::cout << output << std::endl;
        // std::cerr << "mysql_real_connect() failed: " << mysql_error(conn) << std::endl;
        std::string error = "mysql_real_connect() failed: ";
        error += mysql_error(conn);
        // 关闭MySQL连接
        mysql_close(conn);
        return error;
    }

    // 获取ISBN号
    std::string isbn = book["ISBN"];

    // 检查是否存在该ISBN号的记录并获取数量
    std::string query = "SELECT totalNum FROM book_circulation WHERE ISBN = '" + isbn + "'";
    if (mysql_query(conn, query.c_str()) != 0)
    {
        //std::string Message = "查询数据库流通库表失败: " + std::string(mysql_error(conn));
        //output["status"] = "failure";
        //output["message"] = Message;
       // std::cout << output << std::endl;
        // std::cerr << "查询数据库流通库表失败: " << mysql_error(conn) << std::endl;
        std::string error = "查询数据库流通库表失败: ";
        error += mysql_error(conn);
        // 关闭MySQL连接
        mysql_close(conn);
        return error;
    }

    // 获取查询结果
    MYSQL_RES* result = mysql_store_result(conn);
    if (!result)
    {
        //std::string Message = "获取查询结果失败: " + std::string(mysql_error(conn));
        //output["status"] = "failure";
        //output["message"] = Message;
       // std::cout << output << std::endl;
        // std::cerr << "获取查询结果失败: " << mysql_error(conn) << std::endl;
        std::string error = "获取查询结果失败: ";
        error += mysql_error(conn);
        // 关闭MySQL连接
        mysql_close(conn);
        return error;
    }

    if (mysql_num_rows(result) > 0)
    {
        // 获取查询结果的第一行
        MYSQL_ROW row = mysql_fetch_row(result);
        // 将查询结果转换为整数
        int totalNum = atoi(row[0]);

        // 释放查询结果
        mysql_free_result(result);

        // 构建更新记录的SQL语句
        query = "UPDATE book_circulation SET totalNum = totalNum + 1,leftNum = leftNum + 1 WHERE ISBN = '" + isbn + "'";
        if (mysql_query(conn, query.c_str()) != 0)
        {
           // std::string Message = "更新记录失败: " + std::string(mysql_error(conn));
            //output["status"] = "failure";
           // output["message"] = Message;
           // std::cout << output << std::endl;
            // std::cerr << "更新记录失败: " << mysql_error(conn) << std::endl;
            std::string error = "更新记录失败: ";
            error += mysql_error(conn);
            // 关闭MySQL连接
            mysql_close(conn);
            return error;
        }
        else
        {
            // 如果更新成功，返回成功信息
            std::string successMessage = "成功更新记录数量，新的数量为 " + std::to_string(totalNum + 1);
            mysql_close(conn);
            return "success";
        }
    }
    else
    {
        // 如果未找到记录，返回相应信息
        mysql_free_result(result);
        mysql_close(conn);
        return "Nofind";
    }
}


std::string InsertRecord(const nlohmann::json& book)
{
    // 初始化MySQL连接
    MYSQL* conn = mysql_init(nullptr);
    if (!conn)
    {
        // 如果连接初始化失败，返回错误信息
        return "mysql_init() failed";
    }

    // 连接到MySQL数据库
    if (!mysql_real_connect(conn, host, user, passwd, db, 0, nullptr, 0))
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
    std::string isbn = book["ISBN"];

    // 插入新记录
    std::stringstream ss;
    ss << "INSERT INTO book_circulation (ISBN, callNum, CLCNum, bookTitle, author, press, pressDate, introduction, leftNum, totalNum) VALUES ('"
        << isbn << "', '" << book["callNum"].get<std::string>() << "', '" << book["CLCNum"].get<std::string>() << "', '" << book["bookTitle"].get<std::string>() << "', '" << book["author"].get<std::string>() << "', '"
        << book["press"].get<std::string>() << "', " << book["pressDate"].get<std::string>() << ", '" << book["introduction"].get<std::string>() << "', 1, 1)";
    std::string query = ss.str();

    //std::cout << query << std::endl;

    if (mysql_query(conn, query.c_str()) != 0)
    {
        // 如果插入新记录失败，构造错误信息
        std::string error = "插入新记录失败: ";
        // 获取 MySQL 错误信息并添加到错误信息中
        error += mysql_error(conn);
        // 关闭 MySQL 连接
        mysql_close(conn);
        // 返回错误信息
        return error;
    }
    else
    {
        // 关闭 MySQL 连接
        mysql_close(conn);
        // 返回成功插入新记录的信息
        return "success";
    }
}


std::string DecBookNum(const nlohmann::json& book)
{

    // 初始化MySQL连接
    MYSQL* conn = mysql_init(nullptr);
    if (!conn)
    {
        // 如果连接初始化失败，返回错误信息
        return "mysql_init() failed";
    }

    // 连接到MySQL数据库
    if (!mysql_real_connect(conn, host, user, passwd, db, 0, nullptr, 0))
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

    MYSQL_RES* result = mysql_store_result(conn);
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
        query = "UPDATE book_circulation SET totalNum = totalNum - 1,leftNum = leftNum - 1 WHERE ISBN = '" + ISBN + "'";
        if (mysql_query(conn, query.c_str()) != 0)
        {

            // std::cerr << "更新图书数量失败: " << mysql_error(conn) << std::endl;
            return "更新图书数量失败: " + std::string(mysql_error(conn));
        }
        else
        {
            // std::string Message = "图书数量更新成功";
            // output["status"] = "success";
            // output["message"] = Message;
            // std::cout << output << std::endl;
            //  std::cout << "图书数量更新成功" << std::endl;
        }
    }

    mysql_free_result(result);
    return "success";
}


/*string ifExit(std::string ISBN) {

    std::string query = "SELECT totalNum FROM book_circulation WHERE ISBN = '" + ISBN + "'";

    if (mysql_query(conn, query.c_str()) != 0) {
        std::string error = "查询数据库失败: ";        
        error += mysql_error(conn);        
        releaseDB(conn, res);
        return error;
    }
    res = mysql_store_result(conn);
    if (!res) {        
        std::string error = "获取查询结果失败: ";        
        error += mysql_error(conn);
        releaseDB(conn, res);
        return error;
    }
    int total = 0;
    row = mysql_fetch_row(res);
    if (row != NULL) {
        total = atoi(row[0]);    
        query = "UPDATE book_circulation SET totalNum = " + std::to_string(total + 1) + " WHERE ISBN = '" + ISBN + "'";

        if (mysql_query(conn, query.c_str()) != 0) {
            std::string error = "更新记录失败: ";
            error += mysql_error(conn);
            return error;
        }       
    }

    query = "SELECT leftNum FROM book_circulation WHERE ISBN = '" + ISBN + "'";

    if (mysql_query(conn, query.c_str()) != 0) {
        std::string error = "查询数据库失败: ";
        error += mysql_error(conn);
        releaseDB(conn, res);
        return error;
    }
    res = mysql_store_result(conn);
    if (!res) {
        std::string error = "获取查询结果失败: ";
        error += mysql_error(conn);
        releaseDB(conn, res);
        return error;
    }
    int left = 0;
    row = mysql_fetch_row(res);
    if (row != NULL) {
        left = atoi(row[0]);
        query = "UPDATE book_circulation SET leftNum = " + std::to_string(left + 1) + " WHERE ISBN = '" + ISBN + "'";

        if (mysql_query(conn, query.c_str()) != 0) {
            std::string error = "更新记录失败: ";
            error += mysql_error(conn);
            return error;
        }
        else {
            return "成功更新图书数量";
        }
    }
    else {
        std::string error = "Nofind";
        return error;
    }  
   
}

std::string ifExitIncBook(const nlohmann::json& Book) {

    conn = connectToDB(host, user, passwd, db);
    if (conn==NULL) {
        return "mysql connect failed";
    }
    // 获取ISBN号
    std::string ISBN = extractString(Book, "ISBN");

    if (ISBN.empty()) {
        
        return "Missing required fields in JSON!";
    }
    std::string exit = ifExit(ISBN);

    releaseDB(conn, res);
    return exit;    
}
*/
/*
std::string InsertRecord(const nlohmann::json& Book) {
    conn = connectToDB(host, user, passwd, db);
    if (conn == NULL) {
        return "mysql connect failed";
    }

    std::string ISBN = extractString(Book, "ISBN");
    std::string callNum = extractString(Book, "callNum");
    std::string CLCNum = extractString(Book, "CLCNum");
    std::string bookTitle = extractString(Book, "bookTitle");
    std::string author = extractString(Book, "author");
    std::string press = extractString(Book, "press");
    std::string pressDate = extractString(Book, "pressDate");
    std::string introduction = extractString(Book, "introduction");

    if (ISBN.empty() || callNum.empty() || CLCNum.empty() || bookTitle.empty() || author.empty() || press.empty() || pressDate.empty() || introduction.empty()) {

        mysql_close(conn);
        return "Missing required fields in JSON!";
    }

    char sql[SQL_MAX];
    std::snprintf(sql, SQL_MAX, "INSERT INTO book_circulation ( ISBN,callNum,CLCNum,bookTitle,author,press,pressDate,introduction,leftNum,totalNum ) VALUES('%s', '%s','%s', '%s','%s', '%s','%s', '%s',%d,%d);", ISBN.c_str(), callNum.c_str(), CLCNum.c_str(), bookTitle.c_str(), author.c_str(), press.c_str(), pressDate.c_str(), introduction.c_str(), 1, 1);
   // std::cout << "Insert statement: " << sql << std::endl;

    if (mysql_real_query(conn, sql, (unsigned long)strlen(sql)) != 0) {

        std::string error = "插入新记录失败: ";
        error += mysql_error(conn);
        mysql_close(conn);
        return error;
    }
    else {
        mysql_close(conn);
        return "success";
    }

}
*/
/*
std::string DecBookNum(const nlohmann::json& Book)
{
    conn = connectToDB(host, user, passwd, db);   
    if (conn == NULL) {
        return "mysql connect failed";
    }
    std::string ISBN = extractString(Book, "ISBN");
    
    std::string query = "SELECT totalNum FROM book_circulation WHERE ISBN = '" + ISBN + "'";
    if (mysql_query(conn, query.c_str()) != 0)
    {
        // std::cerr << "查询图书失败: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return "查询图书失败: " + std::string(mysql_error(conn));
    }

    res = mysql_store_result(conn);
    if (!res)
    {
        // std::cerr << "获取查询结果失败: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return "获取查询结果失败: " + std::string(mysql_error(conn));
    }

    if (mysql_num_rows(res) == 0)
    {
        // std::cerr << "不存在该图书" << std::endl;        
        mysql_free_result(res);
        mysql_close(conn);
        return "不存在该图书" + std::string(mysql_error(conn));        
    }
    else
    {
        // 更新图书数量
        query = "UPDATE book_circulation SET totalNum = totalNum - 1,leftNum = leftNum - 1 WHERE ISBN = '" + ISBN + "'";
        if (mysql_query(conn, query.c_str()) != 0)
        {
            // std::cerr << "更新图书数量失败: " << mysql_error(conn) << std::endl;
            mysql_free_result(res);
            mysql_close(conn);
            return "更新图书数量失败: " + std::string(mysql_error(conn));
        }
        else
        {
            std::string Message = "图书数量更新成功";          
            // std::cout << output << std::endl;
            std::cout << "图书数量更新成功" << std::endl;
        }
    }
    mysql_free_result(res);
    mysql_close(conn);
    return "success";
}
*/

void jsonBorrowBook(nlohmann::json Borrow) {
    conn = connectToDB(host, user, passwd, db);
    assert(conn != NULL);

    //Borrow["credit"]= 1 ;//用户子系统
    //Borrow["borrowLimit"] = 5;//用户子系统
    
    BorrowBook(Borrow);

    releaseDB(conn, res);
}

void jsonReturnBook(nlohmann::json Borrow) {
    conn = connectToDB(host, user, passwd, db);
    assert(conn != NULL);
       
    //Borrow["credit"]= 1 ;//用户子系统
    //Borrow["borrowLimit"] = 5;//用户子系统
    
    ReturnBook(Borrow);

    releaseDB(conn, res);
}

int GetLeftNum(const nlohmann::json Borrow) {
    int left = 0;
    std::string ISBN;
    if (Borrow.contains("ISBN")) {
        ISBN = Borrow["ISBN"];
    }
    else {
        cout << "Missing required 'ISBN 'in JSON" << endl;
        return -1;
    }
    std::string sql = "SELECT leftNum FROM book_circulation WHERE ISBN = '" + ISBN + "'";
    int query_result = mysql_query(conn, sql.c_str());
    if (query_result != 0) {
        std::cerr << "mysql_query() failed: " << mysql_error(conn) << std::endl;
        return -1;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        std::cerr << "mysql_store_result() failed: " << mysql_error(conn) << std::endl;
        return -1;
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row != NULL) {
        left = atoi(row[0]);
        mysql_free_result(res);
        return left;
    }
    else {
        std::cout << "No record found" << std::endl;
        mysql_free_result(res);
        return -1;
    }
}

//------------------------------------------------------借书----------------------------------------------------------------------------------------

int ifLeft(const nlohmann::json Borrow) {//查流通库表，剩余数量大于0返回1，等于0返回0
    int left = GetLeftNum(Borrow);
    
    if (left > 0) {
        return 1;
    }
    return 0;
}

//调用用户子系统提供的方法得到信任位，可信返回1，不可信返回0，出错返回2
int ifCredit(const nlohmann::json Borrow) {

    int credit = 0;
    std::string uID = extractString(Borrow, "uID");
    if (uID.empty()) {
        credit = 2;
        //cout << "Missing required 'credit'in JSON" << endl;
    }
    else {
        credit = getCredit(uID);
    }     
    return credit;
}

//用户借书量达上限返回0，未达上限返回1，出错返回2
int ifBelowLimit(const nlohmann::json Borrow) {//调用用户子系统的方法得到借书上限，查借阅记录表valid值为1的行数为在借数量，比较判断借书量是否达上限
    int num = 0;
    int limit = 0;
    if (Borrow.contains("borrowNum")) {
        num = Borrow["borrowNum"];
    }
    else {
       // cout << "Missing required 'borrowNum'in JSON" << endl;
        return 2;
    }
    nlohmann::json user = jsGetAuthority(Borrow);
    if (user.contains("borrowNum")) {
        
        limit = user["borrowNum"];
    }
    else {
        //cout << "Missing required 'borrowLimit'in JSON" << endl;
        return 2;
    }
    if (num >= limit) {
        return 0;
    }
    return 1;//未达上限返回1
}

int BorrowAuthority(const nlohmann::json Borrow) {
    nlohmann::json result;
    int leftnumber = ifLeft(Borrow);
    if (leftnumber == 2) {   
       result["status"] = "failure";
       result["message"] = "Error getting left number";
       std::cout << result.dump() << std::endl;        
        return 0;
    }
    if (leftnumber == 0) {
       result["status"] = "failure";
       result["message"] = "error：借书失败！当前无余量";
       std::cout << result.dump() << std::endl;
        return 0;
    }
  
    int credit = ifCredit(Borrow);
    if (credit == 2) {    
       result["status"] = "failure";
       result["message"] = "Error getting credit";
       std::cout << result.dump() << std::endl;
        return 0;
    }
    if (credit == 0) {
       result["status"] = "failure";
       result["message"] = "error：借书失败！信誉度不足无法借书";
       std::cout << result.dump() << std::endl;
        return 0;
    }

    int limit = ifBelowLimit(Borrow);
    if (limit == 2) {      
       result["status"] = "failure";
       result["message"] = "Error getting borrow limit status";
       std::cout << result.dump() << std::endl;
        return 0;
    }
    if (limit == 0) {
       result["status"] = "failure";
       result["message"] = "error：借书失败！借书量已达上限";
       std::cout << result.dump() << std::endl;
        return 0;
    }
    return 1;
}

//向借阅记录表中加项，添加借书信息
int AddBorrowList(const nlohmann::json Borrow) {//成功返回1，失败0

    std::string ISBN = extractString(Borrow, "ISBN");
    std::string callNum = extractString(Borrow, "callNum");
    std::string bookTitle = extractString(Borrow, "bookTitle");
    std::string uID = extractString(Borrow, "uID");
    std::string userName = extractString(Borrow, "userName");
    std::string borrowDate = extractString(Borrow, "borrowDate");
    std::string dueDate = extractString(Borrow, "dueDate");

    nlohmann::json result;
   
    if (ISBN.empty() || callNum.empty() || bookTitle.empty() || uID.empty() || userName.empty() || borrowDate.empty() || dueDate.empty()) {        
       result["status"] = "failure";
       result["message"] = "Missing required fields in JSON!";
       std::cout << result.dump() << std::endl;
        return 0;
    }
    int valid = 1;
    
    char sql[SQL_MAX];    
    std::snprintf(sql, SQL_MAX, "INSERT INTO borrow_list ( ISBN,callNum,bookTitle,uID,userName,borrowDate,dueDate,valid ) VALUES('%s', '%s','%s', '%s','%s', '%s','%s', %d);", ISBN.c_str(), callNum.c_str(), bookTitle.c_str(), uID.c_str(), userName.c_str(), borrowDate.c_str(), dueDate.c_str(), valid);//.c_str()将string转换为const char*类型
    //std::cout << "Insert statement: " << sql << std::endl;    

    int ifsuccess = mysql_real_query(conn, sql,(unsigned long) strlen(sql));
    if (ifsuccess == 1) {      
        const char* error_msg = mysql_error(conn);
        std::cerr << "MySQL Error: " << error_msg << std::endl;        
       result["status"] = "failure";
       result["message"] = "Add to BorrowList Failed: " + std::string(error_msg);
       std::cout << result.dump() << std::endl;
        return 0;
    }
    return 1;
}

//流通库表中的在馆数量减一
int DecreaseLeftNum(const nlohmann::json Borrow) {//成功返回1，失败返回0

    std::string ISBN = extractString(Borrow, "ISBN");
    int left = GetLeftNum(Borrow);
    left--;
    char sql[SQL_MAX];
    std::snprintf(sql, SQL_MAX, "UPDATE book_circulation SET leftNum= %d WHERE ISBN ='%s';", left,ISBN.c_str());
    // std::cout << "UPDATE statement: " << sql << std::endl;

    nlohmann::json result;
    int ifsuccess = mysql_real_query(conn, sql, (unsigned long)strlen(sql));
    if (ifsuccess == 1) {
        const char* error_msg = mysql_error(conn);
        std::cerr << "MySQL Error: " << error_msg << std::endl;
        result["status"] = "failure";
        result["message"] = "Add to BorrowList Failed: " + std::string(error_msg);
        std::cout << result.dump() << std::endl;
        return 0;
    }
    return 1;
}
 
void BorrowBook(const nlohmann::json Borrow){ //借书
    
    nlohmann::json result;
    int authority = BorrowAuthority(Borrow);
    if (authority == 0) {        
        return ;
    }
    else if (authority == 1) {
        if (AddBorrowList(Borrow) == 0) {
            return;
        }
        if (DecreaseLeftNum(Borrow) == 0) {
            return;
        }
        result["status"] = "success";
        result["message"] = "Borrow book success";
        std::cout << result.dump() << std::endl;
       
        return;
    }
}


//-------------------------------------------------------还书-----------------------------------------------------------

void ReturnBook(const nlohmann::json Return) {

    nlohmann::json result;

    std::string uID = extractString(Return, "uID");
    if (uID.empty()) {
        result["status"] = "failure";
        result["message"] = "Missing required uID in JSON!";
        std::cout << result.dump() << std::endl;
        return;
    }

    int over = ifOverDue(Return);
    if (over == -1) {
        return;
    }
    else if (over == 1) {
        setCredit(uID);
    }

    if (DelBorrowList(Return) == 0) {
        return;
    }
    if (IncreaseLeftNum(Return) == 0) {
        return;
    }
    result["status"] = "success";
    result["message"] = "Reserve book success";
    std::cout << result.dump() << std::endl;
    // cout << "Success";
    return;
}
int ifOverDue(const nlohmann::json Return) {
    int over = 0;                //  string转int
    nlohmann::json result;
    if (Return.contains("isOver")) {

        std::string str_over = Return["isOver"].get<std::string>();

        if (str_over.empty()) {

            result["status"] = "failure";
            result["message"] = "Missing required fields in JSON!";
            std::cout << result.dump() << std::endl;
            std::cout << "Missing required fields(ifOver) in JSON!" << std::endl;
            return -1;
        }
        else {
            over = std::stoi(str_over);
        }        
    }
    if (over == 1) {
        return 1;
    }
    return 0;
}
int DelBorrowList(const nlohmann::json Return) {

    std::string ISBN = extractString(Return, "ISBN");   
    std::string uID = extractString(Return, "uID");
    std::string borrowDate = extractString(Return, "borrowDate");

    nlohmann::json result;

    if (ISBN.empty() || uID.empty() || borrowDate.empty()) {
        result["status"] = "failure";
        result["message"] = "Missing required ISBN in JSON!";
       std::cout << result.dump() << std::endl;
       //std::cout << "Missing required fields(ISBN) in JSON!" << std::endl;
        return 0;
    }
    char sql[SQL_MAX];
   
    std::snprintf(sql, sizeof(sql),"UPDATE borrow_list SET valid = 0 WHERE (ISBN = '%s' AND uID = '%s' AND valid = 1)",ISBN.c_str(), uID.c_str());
    // std::cout << "Update statement: " << sql << std::endl;

    // 执行UPDATE语句
    int ifsuccess = mysql_real_query(conn, sql, (unsigned long)strlen(sql));
    if (ifsuccess==1) {
        const char* error_msg = mysql_error(conn);
        
        result["status"] = "failure";
        result["message"] = "Delete BorrowList Failed: " + std::string(error_msg);
        std::cout << result.dump() << std::endl;
        //std::cout << "Failed更新借阅记录 :" << std::string(error_msg) << std::endl;
        return 0;
    }
    return 1;
}
int IncreaseLeftNum(const nlohmann::json Return) {

    std::string ISBN = extractString(Return, "ISBN");

    int leftNum = GetLeftNum(Return);
    leftNum++;

    char sql[SQL_MAX];
    std::snprintf(sql, sizeof(sql), "UPDATE book_circulation SET leftNum = %d WHERE ISBN = '%s' ", leftNum, ISBN.c_str());
    // std::cout << "Update statement: " << sql << std::endl;

    nlohmann::json result;
    // 执行UPDATE语句
    if (mysql_real_query(conn, sql, (unsigned long)strlen(sql)) != 0) {
        const char* error_msg = mysql_error(conn);        
        result["status"] = "failure";
        result["message"] = "Increase Left Number Failed: " + std::string(error_msg);
        std::cout << result.dump() << std::endl;
       // std::cout << "Increase Left Number Failed:" << std::string(error_msg)<< std::endl;
        return 0;
    }
    return 1;
}
