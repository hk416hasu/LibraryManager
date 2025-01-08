const express = require('express');
const router = express.Router();
const mysql2 = require('mysql2');
const { exec } = require('child_process');
const path = require('path');

// 数据库连接池配置
const pool = mysql2.createPool({
   host: 'localhost',
   user: 'root',
   password: 'Calamari=/+*&(Yy6',
});

router.get('', (req, res) => {

   // 定义所有查询
   const queries = {
      collectionCount: `SELECT SUM(totalNum) AS collectionCount FROM test_zhang.book_circulation;`, // 馆藏数量
      userCount: `SELECT COUNT(*) AS userCount FROM test_liguan.user_info;`, // 用户数量
      borrowCount: `SELECT COUNT(*) AS borrowCount FROM test_zhang.borrow_list;`, // 假设借阅记录表为 borrow_records
   };

   // 执行数据库查询
   const dbQueries = Object.entries(queries).map(([key, query]) =>
      new Promise((resolve, reject) => {
         pool.execute(query, (err, results) => {
            if (err) {
               return reject(err);
            }
            resolve({ [key]: results[0][key] });
         });
      })
   );

   // 执行 Linux 命令
   const logFilePath = './config_info/server.log'; // 日志文件路径
   const requestCountPromise = new Promise((resolve, reject) => {
      exec(`wc -l ${logFilePath}`, (err, stdout) => {
         if (err) {
            return reject(err);
         }
         const count = parseInt(stdout.split(' ')[0], 10); // 提取行数
         resolve({ requestCount: Math.round(count / 8) });
      });
   });

   // 合并数据库查询和 Linux 命令的结果
   Promise.all([...dbQueries, requestCountPromise])
      .then(results => {
         const stats = results.reduce((acc, curr) => ({ ...acc, ...curr }), {});
         res.json(stats);
      })
      .catch(error => {
         console.error('Error fetching statistics:', error);
         res.status(500).json({ error: 'Failed to fetch statistics' });
      });
});

module.exports = router;

