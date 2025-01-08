#include "global.h"

int main(int argc, char *argv[]) {

   json j = jsonFileParse(argc, argv);

   // 根据json中的method字段将json对象传递给其他模块
   std::string method = j["method"];
   if (method == "just_print") {
      printJsonObject(j);
   } else if (method == "register") {
      jsonSignUp(j);
   } else if (method == "login") {
      jsonSignIn(j);
   } else if (method == "borrow") {
      jsonBorrowBook(j);
   } else if (method == "return") {
      jsonReturnBook(j);
   } else if (method == "catalog") {
      // jsonCatalog();
   } else if (method == "destroy") {
      jsonCancel(j);
   } else if (method == "getUserinfo") {
      jsonDispInfo(j);
   } else if (method == "updateAuthority") {
      jsonSetAuthorityNum(j);
   }

   return 0;
}
