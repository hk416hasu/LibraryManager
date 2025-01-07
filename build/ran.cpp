#include "global.h"

void printJsonObject(json &j) {
   // 4. 输出 JSON 数据, 输出格式必须只能是json格式，不要有其他干扰
   std::cout << j.dump(2) << std::endl;

   // 5. 解析 JSON 对象的信息
   std::string userName = j["userName"];
   // std::cout << "userName: " << userName << "\n";
}

json jsonFileParse(int argc, char *argv[]) {
   if (argc == 1) {
      std::cerr << "PlZ give me the jsonPath" << std::endl;
      exit(1);
   }

   // 1. 打开 JSON 文件
   std::ifstream file(argv[1]);
   if (!file.is_open()) {
      std::cerr << "Failed to open json file!" << std::endl;
      exit(1);
   }

   // 2. 从文件中读取 JSON 数据
   json j;
   try {
      file >> j; // 解析 JSON 文件并创建 JSON 对象
   } catch (const json::parse_error& e) {
      std::cerr << "Parse error: " << e.what() << std::endl;
      exit(1);
   }

   // 3. 关闭文件
   file.close();

   return j;
}
