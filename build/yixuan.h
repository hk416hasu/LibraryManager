#ifndef YIXUAN_H
#define YIXUAN_H

void jsonBorrowBook(nlohmann::json Borrow);
void jsonReturnBook(nlohmann::json Borrow);
std::string ifExitIncBook(const nlohmann::json& book);
std::string InsertRecord(const nlohmann::json& book);
std::string DecBookNum(const nlohmann::json& book);

#endif
