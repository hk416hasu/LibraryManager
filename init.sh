#!/bin/env bash

# 1. install nodejs and npm
sudo apt install -y nodejs npm
npm config set registry http://mirrors.cloud.tencent.com/npm # 设置npm镜像地址

# 2. install some modules for nodejs
npm install node-fetch@2 mysql2 express pdfkit 

# 3. install mysql and configurate it
sudo apt install mysql-server
sudo systemctl start mysql
sudo systemctl enable mysql
sudo mysql_secure_installation # secure configuration of mysql
if [ -f /etc/mysql/mysql.conf.d/mysqld.cnf ]; then
   mv /etc/mysql/mysql.conf.d/mysqld.cnf /etc/mysql/mysql.conf.d/mysqld.cnf_bak
fi
sudo cp ./config_info/mysqld.cnf /etc/mysql/mysql.conf.d/mysqld.cnf
sudo systemctl stop mysql
sudo systemctl restart mysql # reload the mysql configuration
sudo systemctl reload mysql # reload the mysql configuration, twice
sudo ufw allow 3306 # open the server port
# don't forget modify your server firewall supported by server providers

# 4. make the C++ program
make -C ./build

exit 0
