## Library Management System

### Environment Configuration

1. **Operating System**  
   Ubuntu 22.04 LTS (GNU/Linux 5.15.0-116-generic x86_64)

2. **Software Dependencies**

- **Node.js and Dependencies**  

```bash
VM: ~/web > node --version
v12.22.9
VM: ~/web > cat package.json
{
  "dependencies": {
    "body-parser": "^2.0.0",
    "cookie-parser": "^1.4.7",
    "crypto-js": "^4.2.0",
    "express": "^4.21.2",
    "fs": "^0.0.1-security",
    "multer": "^1.4.5-lts.1",
    "mysql2": "^3.11.5",
    "node-fetch": "^2.7.0",
    "nodemailer": "^6.9.16",
    "pdfkit": "^0.8.0"
  }
}
```

- **C++ Compiler Version and Standard**  

```bash
# C++ compiler version and standard
VM: ~/web > g++ --version
g++ (Ubuntu 11.2.0-19ubuntu1) 11.2.0
VM: ~/web > g++ -dM -E -x c++ /dev/null | grep __cplusplus
#define __cplusplus 201703L # C++17
```

- **Database Version**

```bash
# MySQL Database
VM: ~/web > mysql --version
mysql  Ver 8.0.39-0ubuntu0.22.04.1 for Linux on x86_64 ((Ubuntu))
```

- **Other Tools**  
  1. `nlohmann/json` v3.11.3  
  2. `cpp-httplib` (no specific version, download from [cpp-httplib repository](https://github.com/yhirose/cpp-httplib))  
  3. `utfcpp` v4.0.6  

### Installation

Run the installation script to configure the environment. Note that the script’s robustness may be limited.  

```bash
sudo bash init.sh
```

