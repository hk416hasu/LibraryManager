#include <mysql/mysql.h>
#include <bits/stdc++.h>
#include "json.hpp"
#include "guanxin.h"

using namespace std;
using json = nlohmann::json;
struct user // 用户信息表
{
    int uID;
    string uCode;
    string userName;
    string POFNum;
    int authorityNum;
    string shenFenZheng;
    string gender;
    string phoneNumber;
    string email;
    int credit;
};
struct authority // 权限表
{
    int authorityNum;
    int borrowNum;
    int reservationNum;
    int reservationDay;
    int borrowDay;
};
struct POF // 单位信息表
{
    string POFNum;
    string POFName;
    string POFAddr;
    string POFPhoneNum;
};
struct manageRecord // 管理信息表
{
    int recordNum;
    int uID;
    string record;
    string recordTime;
};
struct feedback
{
    string message;
    bool status;
};
static const char *host = "82.156.207.47";
static const char *user = "guanxin";
static const char *passwd = "oilglitch";
static const char *db = "test_liguan";
static MYSQL *conn;                                                                      // 设为全局变量以避免出现Segmentation fault (core dumped)的问题
static MYSQL_RES *res;                                                                   // 同理
static MYSQL_ROW row;                                                                    // 同理
feedback signUp(struct user newUser);                                             // 1注册
feedback signIn(string uID, string uCode);                                        // 1登录，查找到用户id后检验是否与uCode匹配
authority getAuthority(string uID);                                               // 1获得对应用户的权限信息
struct user dispInfo(string uID1, string uID2);                                   // 1web将uID1的信息显示在屏幕上。uID2为请求者
void createTable();                                                               // 1创建用户管理子系统所需的四张表
void setCredit(string uID);                              // 1设置用户信任位
void writeRecord(manageRecord mr);                                                // 1管理记录留痕
int getCredit(string uID);                                                        // 1获得用户信任位
string getCurrentDate();                                                          // 1获取当前日期，返回格式为 "YYYY-MM-DD" 的字符串
feedback updateUserInfo(string uID1, string uID2, struct user u, string change_type); // 1增删改uID1的信息，请求者为uID2，记得把u的uid写上
void updateAuthorityTable(string managerID, string change_type, authority au);    // 1维护权限表
vector<manageRecord> getManageRecord(string ID);                                  // 1返回某账号的管理记录
vector<manageRecord> getManageRecord();                                           // 1返回所有记录
void updatePOFTable(string managerID, POF pof, string change_type);               // 1维护单位表
void jsonSignUp(json &j);
void jsonSignIn(json &j);
json jsGetAuthority(json &j);
void jsonGetAuthority(json &j);
void jsonDispInfo(json &j);
void jsonUpdateUserInfo(json &j);
void jsonGetManageRecord(json &j);
void jsonSetAuthorityNum(json &j);
void executeSQL(MYSQL *conn, const char *sql) // 调用api执行sql语句
{
    if (mysql_query(conn, sql) != 0)
    {
        cerr << "Error executing SQL: " << mysql_error(conn) << endl;
    }
}
string stdStrings(string str) // 标准化为八位字符串
{
    int targetLength = 8;
    while (str.size() < targetLength)
    {
        str = "0" + str;
    }
    return str;
}
MYSQL_RES *store_result() // 储存和获取询问的结果
{
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        // 没有结果集或者获取结果集失败的处理
        if (mysql_field_count(conn) == 0)
        {
            // 这可能是因为执行的是一个不返回结果集的语句，如INSERT、UPDATE等
            // 可以根据具体情况进行适当处理
            printf("No result set returned\n");
        }
        else
        {
            // 真正的获取结果集失败的情况
            fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        }
        return NULL;
    }
    return result;
}
void jsonSignUp(json &j)
{
    conn = mysql_init(NULL);
    mysql_real_connect(conn, host, user, passwd, db, 0, NULL, 0);
    struct user newUser;

    newUser.uCode = j["uCode"];
    newUser.userName = j["userName"];

    if (j["POFNum"].is_null())
    {
        newUser.POFNum = "NULL";
    }
    else
    {
        newUser.POFNum = j["POFNum"];
    }

    newUser.shenFenZheng = j["shenFenZheng"];

    if (j["gender"].is_null())
    {
        newUser.gender = "NULL";
    }
    else
    {
        newUser.gender = j["gender"];
    }

    newUser.phoneNumber = j["phoneNumber"];

    if (j["email"].is_null())
    {
        newUser.email = "NULL";
    }
    else
    {
        newUser.email = j["email"];
    }

    feedback f = signUp(newUser);

    json output;
    if (f.status)
    {
        output = {
            {"message", "success"},
            {"status", "success"},
            {"uID", f.message}};
    }
    else
    {
        output = {
            {"message", f.message},
            {"status", "failure"}};
    }
    mysql_close(conn);
    cout << output << endl;
}
void createTable()
{
    char sql[1024];
    sprintf(sql, "CREATE TABLE IF NOT EXISTS user_info (" // 用户信息表
                 "uID INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,"
                 "uCode VARCHAR(64) NOT NULL,"
                 "userName VARCHAR(30) NOT NULL,"
                 "POFNum VARCHAR(50) REFERENCES POF_info(POFNum),"
                 "authorityNum INTEGER NOT NULL DEFAULT 1 REFERENCES authority_info (authorityNum),"
                 "shenFenZheng CHARACTER(18) NOT NULL,"
                 "gender VARCHAR(50),"
                 "phoneNumber CHARACTER(11) NOT NULL,"
                 "email VARCHAR(50),"
                 "credit INTEGER(1) NOT NULL DEFAULT 1)");
    executeSQL(conn, sql);
    sprintf(sql, "CREATE TABLE IF NOT EXISTS authority_info(" // 用户权限表
                 "authorityNum INTEGER NOT NULL PRIMARY KEY,"
                 "borrowNum INTEGER NOT NULL,"
                 "reservationNum INTEGER NOT NULL,"
                 "reservationDay INTEGER NOT NULL,"
                 "borrowDay INTEGER NOT NULL)");
    executeSQL(conn, sql);
    sprintf(sql, "CREATE TABLE IF NOT EXISTS POF_info(" // 用户单位表
                 "POFNum VARCHAR(50) NOT NULL PRIMARY KEY,"
                 "POFName VARCHAR(300) NOT NULL,"
                 "POFAddr VARCHAR(300) NOT NULL,"
                 "POFPhoneNum CHARACTER(11) NOT NULL);");
    executeSQL(conn, sql);
    sprintf(sql, "CREATE TABLE IF NOT EXISTS manage_record(" // 管理记录表
                 "uID INTEGER NOT NULL REFERENCES user_info (uID),"
                 "record CHARACTER(200) NOT NULL,"
                 "recordTime DATE NOT NULL,"
                 "recordNum INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY)");
    executeSQL(conn, sql);
}
feedback signUp(struct user newUser)
{
    feedback f;
    char sql[1024];
    sprintf(sql, "SELECT uID FROM user_info WHERE shenFenZheng='%s'", newUser.shenFenZheng.c_str());
    executeSQL(conn, sql);
    res = store_result();
    if ((row = mysql_fetch_row(res)) != NULL)
    {
        f.message = "The uid have been existed";
        f.status = false;
        return f;
    }
    sprintf(sql, "INSERT INTO user_info(uCode,userName,POFNum,shenFenZheng,gender,phoneNumber,email)"
                 " values('%s','%s','%s','%s','%s','%s','%s')",
            newUser.uCode.c_str(), newUser.userName.c_str(), newUser.POFNum.c_str(), newUser.shenFenZheng.c_str(), newUser.gender.c_str(), newUser.phoneNumber.c_str(), newUser.email.c_str());
    executeSQL(conn, sql);
    sprintf(sql, "SELECT uID FROM user_info WHERE shenFenZheng='%s'", newUser.shenFenZheng.c_str());
    executeSQL(conn, sql);
    res = store_result();
    row = mysql_fetch_row(res);
    string str = row[0]; // ÏÈ·Åµ½strÀï£¬Ö±½Ó×ªint»áÉÙ×î¸ßÎ»
    // ÔÚÕâÀï¼ì²â¸÷×Ö¶Î£¬²»Îª¿ÕµÄ²åÈë½øÈ¥
    str = stdStrings(str);
    f.message = str;
    f.status = true;
    return f;
}

feedback signIn(string uID, string uCode) // 登录，如果有这个id则匹配密码，匹配成功则登录
{
    feedback f;
    char sql[1024];
    int sign_in_id = atoi(uID.c_str());
    sprintf(sql, "SELECT uID FROM user_info where uID=%d", sign_in_id);
    executeSQL(conn, sql);
    res = store_result();

    if (res == NULL)
    {
        f.message = "Invalid ID";
        f.status = false;
        return f;
    }
    else
    {
        sprintf(sql, "SELECT uCode FROM user_info where uID=%d", sign_in_id);
        executeSQL(conn, sql);
        res = store_result();
        row = mysql_fetch_row(res);

        if (row[0] == uCode)
        {
            f.message = "Succeed to sign in";
            f.status = true;
            return f;
        }
        else
        {
            f.message = "Wrong code";
            f.status = false;
            return f;
        }
    }
}
authority getAuthority(string uID) // 获得对应用户的权限信息，如果用户账号不存在，则authorityNum返回为-1
{
    authority tmp = {-1, 0, 0, 0, 0};
    char sql[1024];
    int id = atoi(uID.c_str()); // 转成整型
    sprintf(sql, "SELECT authorityNum FROM user_info WHERE uID=%d", id);
    executeSQL(conn, sql); // 查询权限
    res = store_result();

    if ((row = mysql_fetch_row(res)) == NULL)
    {
        return tmp;
    }

    int athNum = atoi(row[0]);
    sprintf(sql, "SELECT * FROM authority_info WHERE authorityNum=%d", athNum);
    executeSQL(conn, sql);
    res = store_result();
    row = mysql_fetch_row(res);
    tmp.authorityNum = atoi(row[0]);
    tmp.borrowNum = atoi(row[1]);
    tmp.reservationNum = atoi(row[2]);
    tmp.reservationDay = atoi(row[3]);
    tmp.borrowDay = atoi(row[4]);
    // 将结果保存至变量中
    return tmp;
}
void jsonGetAuthority(json &j)
{
    conn = mysql_init(NULL);
    mysql_real_connect(conn, host, user, passwd, db, 0, NULL, 0);
    string uID = j["uID"];
    authority au = getAuthority(uID);
    json output;
    if (au.authorityNum == -1)
    {
        output = {
            {"message", "The uid does not exist"},
            {"status", "failure"}};
    }
    else
    {
        output = {
            {"message", "founded"},
            {"status", "success"},
            {"borrowDay", au.borrowDay},
            {"borrowNum", au.borrowNum},
            {"reservationDay", au.reservationDay},
            {"reservationNum", au.reservationNum}};
    }

    mysql_close(conn);
    cout << output << endl;
}
json jsGetAuthority(json &j)
{
    conn = mysql_init(NULL);
    mysql_real_connect(conn, host, user, passwd, db, 0, NULL, 0);
    string uID = j["uID"];
    authority au = getAuthority(uID);
    json output;
    if (au.authorityNum == -1)
    {
        output = {
            {"message", "The uid does not exist"},
            {"status", "failure"}};
    }
    else
    {
        output = {
            {"borrowDay", au.borrowDay},
            {"borrowNum", au.borrowNum},
            {"reservationDay", au.reservationDay},
            {"reservationNum", au.reservationNum}};
    }

    mysql_close(conn);
    return output;
}
void jsonSignIn(json &j)
{
    conn = mysql_init(NULL);
    mysql_real_connect(conn, host, user, passwd, db, 0, NULL, 0);
    string uID = j["uID"], uCode = j["uCode"];
    feedback f = signIn(uID, uCode);
    json output;
    if (f.status)
    {
        struct user u = dispInfo(uID, uID);
        output = {
            {"message", f.message},
            {"status", "success"},
            {"uID", uID},
            {"username", u.userName},
            {"authority_num", u.authorityNum}};
    }
    else
    {
        output = {
            {"message", f.message},
            {"status", "failure"}};
    }
    mysql_close(conn);
    cout << output << endl;
}
struct user giveValue(MYSQL_ROW row) // 赋值为-1表示空
{
    struct user u1;
    u1.uID = atoi(row[0]);
    u1.uCode = row[1];
    u1.userName = row[2];
    if (row[3] == NULL)
        u1.POFNum = "NULL";
    else
        u1.POFNum = row[3];
    u1.authorityNum = atoi(row[4]);
    u1.shenFenZheng = row[5];
    if (row[6] == NULL)
        u1.gender = "NULL";
    else
        u1.gender = row[6];
    u1.phoneNumber = row[7];
    if (row[8] == NULL)
        u1.email = "NULL"; // 空串
    else
        u1.email = row[8];
    u1.credit = atoi(row[9]);
    return u1;
}
struct user dispInfo(string uID1, string uID2)
{
    char sql[1024];
    int id = atoi(uID1.c_str());
    struct user user1;
    authority ath = getAuthority(uID2); // 获得请求者的权限
    sprintf(sql, "SELECT * FROM user_info where uID=%d", id);
    if (ath.authorityNum == -1)
    {
        //cout << "Invalid uID" << endl;
        user1.uID=-1;
        return user1;
    }
    else if (ath.authorityNum == 0)
    {
        executeSQL(conn, sql);
        res = store_result();
        row = mysql_fetch_row(res);
        return giveValue(row);
    }
    else
    {
        if (uID1 == uID2)
        {
            executeSQL(conn, sql);
            res = store_result();
            row = mysql_fetch_row(res);
            return giveValue(row);
        }
        else
        {
         //   cout << "Unpermited" << endl;
           user1.uID=-2;
           return user1;
        }
    }
    return user1;
}
void jsonDispInfo(json &j)
{
    conn = mysql_init(NULL);
    mysql_real_connect(conn, host, user, passwd, db, 0, NULL, 0);
    string uID1 = j["uID1"], uID2 = j["uID2"];
    struct user u = dispInfo(uID1, uID2);

    json output;
    if (u.uID == -1)
    {
        output = {
            {"message", "Invalid ID"},
            {"status", "failure"}};
    }
    else if (u.uID == -2)
    {
        output = {
            {"message", "Unpermiteed"},
            {"status", "failure"}};
    }
    else
    {
        output = {
            {"message", "success to search"},
            {"status", "success"},
            {"uID", uID1},
            {"ucode", u.uCode},
            {"userName", u.userName},
            {"POFNum", u.POFNum},
            {"authorityNum", u.authorityNum},
            {"shenFenZheng", u.shenFenZheng},
            {"gender", u.gender},
            {"phoneNumber", u.phoneNumber},
            {"email", u.email},
            {"credit", u.credit}};
    }

    cout << output << endl;
    mysql_close(conn);
}
void jsonSetAuthorityNum(json &j)
{
  json output;
  string uID1=j["uID1"],uID2=j["uID2"];
  char sql[1024];
  int num=j["authorityNum"];
  conn = mysql_init(NULL);
  mysql_real_connect(conn, host, user, passwd, db, 0, NULL, 0);
  sprintf(sql, "UPDATE user_info SET authorityNum=%d WHERE uID=%d",num,atoi(uID1.c_str()));
  executeSQL(conn,sql);
  sprintf(sql, "user %s updated the authorityNum of user %s to %d", uID2.c_str(), uID1.c_str(),num);
  manageRecord mr;
  mr.recordTime = getCurrentDate();
  mr.record.assign(sql);
  mr.uID = atoi(uID2.c_str());
  writeRecord(mr);
  output={
    {"message","success"},
    {"status","success"}
  };
  cout<<output<<endl;
  mysql_close(conn);
}
int getCredit(string uID) // 获得用户信任位
{
    char sql[1024];
    int id = atoi(uID.c_str());
    sprintf(sql, "SELECT credit FROM user_info WHERE uID=%d", id);
    executeSQL(conn, sql);
    res = store_result();
    row = mysql_fetch_row(res);
    return atoi(row[0]);
}
void setCredit(string uID)
{
    char sql[1024];
    /*authority au = getAuthority(managerID);
    if (au.authorityNum != 0)
    {
        cout << "You have no right" << endl;
        return;
    }*/
    int id = atoi(uID.c_str());//, mid = atoi(managerID.c_str());
    sprintf(sql, "UPDATE user_info SET credit=%d WHERE uID=%d", 0, id);
    executeSQL(conn, sql);
    manageRecord mr;
    sprintf(sql, "The credit of user %s has been set", uID.c_str());
    mr.recordTime = getCurrentDate();
    mr.record.assign(sql);
    mr.uID = atoi(uID.c_str());
    writeRecord(mr);
}
void writeRecord(manageRecord mr)
{
    char sql[1024];
    sprintf(sql, "INSERT INTO manage_record(uID,record,recordTime)"
                 " VALUES(%d,'%s','%s')",
            mr.uID, mr.record.c_str(), mr.recordTime.c_str());
    executeSQL(conn, sql);
}
string getCurrentDate()
{
    auto now = std::chrono::system_clock::now();
    std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
    struct tm *local_time = std::localtime(&timestamp);
    std::string year = std::to_string(local_time->tm_year + 1900);
    std::string month = std::to_string(local_time->tm_mon + 1);
    std::string day = std::to_string(local_time->tm_mday);
    std::stringstream ss;
    ss << std::setw(4) << std::setfill('0') << year << "-"
       << std::setw(2) << std::setfill('0') << month << "-"
       << std::setw(2) << std::setfill('0') << day;
    return ss.str();
}
feedback updateUserInfo(string uID1, string uID2, struct user u, string change_type) // 请求者为uID2
{
    feedback f = {"false", false};
    char sql[1024];
    authority au = getAuthority(uID2);
    if (change_type == "update") // 更新信息
    {
        if (au.authorityNum == 0) // 请求者为管理员
        {
            sprintf(sql, "UPDATE user_info SET uCode='%s',userName='%s',POFNum='%s',authorityNum=%d,shenFenZheng='%s',gender='%s',phoneNumber='%s',email='%s',credit=%d WHERE uID=%d", u.uCode.c_str(), u.userName.c_str(), u.POFNum.c_str(), u.authorityNum, u.shenFenZheng.c_str(), u.gender.c_str(), u.phoneNumber.c_str(), u.email.c_str(), u.credit, u.uID);
            f.message = "manager";
            f.status = true;
        }
        else if (uID1 == uID2) // 有的信息本人不能随意修改
        {
            sprintf(sql, "UPDATE user_info SET uCode='%s',userName='%s',POFNum='%s',gender='%s',phoneNumber='%s',email='%s WHERE uID=%d'", u.uCode.c_str(), u.userName.c_str(), u.POFNum.c_str(), u.gender.c_str(), u.phoneNumber.c_str(), u.email.c_str(), u.uID);
            f.message = "user";
            f.status = true;
        }
        executeSQL(conn, sql);
        sprintf(sql, "user %s updated the infomation of user %s", uID2.c_str(), uID1.c_str());
        manageRecord mr;
        mr.recordTime = getCurrentDate();
        mr.record.assign(sql);
        mr.uID = atoi(uID2.c_str());
        writeRecord(mr);
    }
    else if (change_type == "delete")
    {
        if (au.authorityNum == 0)
        {
            sprintf(sql, "DELETE FROM user_info WHERE uID=%d", u.uID);
            executeSQL(conn, sql);
            sprintf(sql, "manager %s deleted user %s", uID2.c_str(), uID1.c_str());
            manageRecord mr;
            mr.recordTime = getCurrentDate();
            mr.record.assign(sql);
            mr.uID = atoi(uID2.c_str());
            writeRecord(mr);
            f.message = "manager";
            f.status = true;
        }
    }
    return f;
}
void jsonUpdateUserInfo(json &j)
{
    json output;
    feedback f;
    conn = mysql_init(NULL);
    mysql_real_connect(conn, host, user, passwd, db, 0, NULL, 0);
    string uID1 = j["uID1"], uID2 = j["uID2"], change_type = j["change_type"];
    struct user u;
    u.uCode = j["uCode"];
    u.userName = j["userName"];
    u.authorityNum = j["authorityNum"];
    if (j["POFNum"].is_null())
    {
        u.POFNum = "NULL";
    }
    else
    {
        u.POFNum = j["POFNum"];
    }

    u.shenFenZheng = j["shenFenZheng"];

    if (j["gender"].is_null())
    {
        u.gender = "NULL";
    }
    else
    {
        u.gender = j["gender"];
    }

    u.phoneNumber = j["phoneNumber"];

    if (j["email"].is_null())
    {
        u.email = "NULL";
    }
    else
    {
        u.email = j["email"];
    }
    u.credit = j["credit"];
    f = updateUserInfo(uID1, uID2, u, change_type);

    if (f.status)
    {
        output = {
            {"message", f.message},
            {"status", "success"}};
    }
    else
    {
        output = {
            {"message", f.message},
            {"status", "failure"}};
        cout << output << endl;
        mysql_close(conn);
    }
}
void updateAuthorityTable(string managerID, string change_type, authority au)
{
    char sql[1024];
    if (change_type == "insert") // 新增权限
    {
        sprintf(sql, "INSERT INTO authority_info"
                     " VALUES(%d,%d,%d,%d,%d)",
                au.authorityNum, au.borrowNum, au.reservationNum, au.reservationDay, au.borrowDay);
        executeSQL(conn, sql);
        sprintf(sql, "manager %s added authority %d", managerID.c_str(), au.authorityNum);
        manageRecord mr;
        mr.recordTime = getCurrentDate();
        mr.record.assign(sql);
        mr.uID = atoi(managerID.c_str());
        writeRecord(mr);
    }
    else if (change_type == "update") // 更新权限细则
    {
        sprintf(sql, "UPDATE authority_info SET borrowNum=%d,reservationNum=%d,reservationDay=%d,borrowDay=%d"
                     " WHERE authorityNum=%d",
                au.borrowNum, au.reservationNum, au.reservationDay, au.borrowDay, au.authorityNum);
        executeSQL(conn, sql);
        sprintf(sql, "manager %s updated authority %d", managerID.c_str(), au.authorityNum);
        manageRecord mr;
        mr.recordTime = getCurrentDate();
        mr.record.assign(sql);
        mr.uID = atoi(managerID.c_str());
        writeRecord(mr);
    }
    else if (change_type == "delete") // 删除权限
    {
        sprintf(sql, "DELETE FROM authority_info WHERE authorityNum=%d", au.authorityNum);
        executeSQL(conn, sql);
        sprintf(sql, "manager %s deleted authority %d", managerID.c_str(), au.authorityNum);
        manageRecord mr;
        mr.recordTime = getCurrentDate();
        mr.record.assign(sql);
        mr.uID = atoi(managerID.c_str());
        writeRecord(mr);
    }
}
void updatePOFTable(string managerID, POF pof, string change_type)
{
    char sql[1024];
    if (change_type == "insert") // 新增单位
    {
        sprintf(sql, "INSERT INTO POF_info"
                     " VALUES('%s','%s','%s','%s')",
                pof.POFNum.c_str(), pof.POFName.c_str(), pof.POFAddr.c_str(), pof.POFPhoneNum.c_str());
        executeSQL(conn, sql);
        sprintf(sql, "manager %s added POFNum %s", managerID.c_str(), pof.POFNum.c_str());
        manageRecord mr;
        mr.recordTime = getCurrentDate();
        mr.record.assign(sql);
        mr.uID = atoi(managerID.c_str());
        writeRecord(mr);
    }
    else if (change_type == "update") // 更新单位信息
    {
        sprintf(sql, "UPDATE POF_info SET POFNum='%s',POFName='%s',POFAddr='%s',POFPhoneNum='%s'"
                     " WHERE POFNum='%s'",
                pof.POFNum.c_str(), pof.POFName.c_str(), pof.POFAddr.c_str(), pof.POFPhoneNum.c_str(), pof.POFNum.c_str());
        executeSQL(conn, sql);
        sprintf(sql, "manager %s updated POFNum %s", managerID.c_str(), pof.POFNum.c_str());
        manageRecord mr;
        mr.recordTime = getCurrentDate();
        mr.record.assign(sql);
        mr.uID = atoi(managerID.c_str());
        writeRecord(mr);
    }
    else if (change_type == "delete") // 删除单位
    {
        sprintf(sql, "DELETE FROM POF_info WHERE POFNum='%s'", pof.POFNum.c_str());
        executeSQL(conn, sql);
        sprintf(sql, "manager %s deleted POFNum %s", managerID.c_str(), pof.POFNum.c_str());
        manageRecord mr;
        mr.recordTime = getCurrentDate();
        mr.record.assign(sql);
        mr.uID = atoi(managerID.c_str());
        writeRecord(mr);
    }
}
vector<manageRecord> getManageRecord()
{
    char sql[1024];
    sprintf(sql, "SELECT * FROM manage_record");
    executeSQL(conn, sql);
    res = store_result();
    vector<manageRecord> manare;
    manageRecord mr;
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        mr.uID = atoi(row[0]);
        mr.record = row[1];
        mr.recordTime = row[2];
        mr.recordNum = atoi(row[3]);
        manare.push_back(mr);
    }
    return manare;
}
vector<manageRecord> getManageRecord(string ID)
{
    char sql[1024];
    sprintf(sql, "SELECT * FROM manage_record WHERE uID=%d", atoi(ID.c_str()));
    executeSQL(conn, sql);
    res = store_result();
    vector<manageRecord> manare;
    manageRecord mr;
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        mr.uID = atoi(row[0]);
        mr.record = row[1];
        mr.recordTime = row[2];
        mr.recordNum = atoi(row[3]);
        manare.push_back(mr);
    }
    return manare;
}
/*void jsonGetManageRecord(json &j)
{
    json output;
    conn = mysql_init(NULL);
    mysql_real_connect(conn, host, user, passwd, db, 0, NULL, 0);
    vector<managerRecord> ma;
    vector<managerRecord>::iterator it;

    if (j["uID"].is_null())
    {
        ma = getManageRecord();
    }
    else
    {
        ma = getManageRecord(j["uID"]);
    }

    if (ma.begin() == ma.end())
    {
        output = {
            {"message", "NO record"},
            {"status", "failure"}};
    }
    for (it=ma.begin(); it < ma.end; it++)
    {
        output = {
            {"uID", stdStrings(to_string((*it).uID))},
            {"record", (*it).record},
            {"recordTime",(*it).recordTime},
            {"recordNum", (*it).recordNum}};
    }
    cout<<output<<endl;
    mysql_close(conn);
}*/
