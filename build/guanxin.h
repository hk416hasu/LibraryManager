#ifndef GUANXIN_H
#define GUANXIN_H

void jsonSignUp(nlohmann::json &j);
void jsonSignIn(nlohmann::json &j);
void jsonDispInfo(nlohmann::json &j);
void jsonSetAuthorityNum(nlohmann::json &j);
int getCredit(std::string uID);
void setCredit(std::string uID);
nlohmann::json jsGetAuthority(const nlohmann::json &j);

#endif
