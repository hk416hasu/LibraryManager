const express = require('express');
const router = express.Router();
const mysql2 = require('mysql2');
const { execFile } = require('child_process');
const path = require('path');
const fs = require('fs');

const pool = mysql2.createPool({
   host: 'localhost',
   user: 'yixuan',
   password: 'yarnimprovise',
   database: 'test_zhang',
});

router.get('/all', (req, res) => {

   const query = `SELECT * FROM book_circulation`;

   pool.execute(query, (err, results) => {
      if (err) {
         console.error('Database query error: ', err);
         return res.status(500).json({ error: 'Database error' });
      }

      if (results.length > 0) {
         res.json(results);
      } else {
         res.status(401).send('Invalid username or password');
      }
   });
});

router.post('/borrow', (req, res) => {
   console.log(req.body);

   const { uID } = req.body;
   const query = `SELECT * FROM borrow_list where uID = ? and valid = 1`;
   pool.execute(query, [uID], (err, results) => {
      if (err) {
         console.error('Database query error: ', err);
         return res.status(500).json({ error: 'Database error' });
      }

      borrowNum = results.length;

      const elfPath = './build/main';
      const jsonPath = './build/data.json';

      const modifiedBody = {
         ...req.body,
         method: 'borrow', // 添加方法字段
         borrowNum: borrowNum,
      };

      fs.writeFile(jsonPath, JSON.stringify(modifiedBody), 'utf8', (err) => {
         if (err) {
            console.error('写入文件时出错：', err);
            res.status(500).json({ err: 'Internal Server Error', message: err.message });
            return;
         }
         console.log('JSON 数据已成功保存');

         execFile(elfPath, [jsonPath], (error, stdout, stderr) => {
            if (error) {
               console.error(`Error: ${error.message}`);
               res.status(500).json({ error: 'Internal Server Error', message: error.message });
               return;
            }

            if (stderr) {
               console.error(`Stderr: ${stderr}`);
               res.status(500).json({ error: 'Internal Server Error', message: stderr });
               return;
            }

            try {
               const jsonData = JSON.parse(stdout); // 解析 stdout 为 JSON
               if (jsonData.status === "success") {
                  res.status(200).json(jsonData);
               } else {
                  res.status(400).json(jsonData);
               }
            } catch (parseError) {
               console.error(`JSON Parse Error: ${parseError.message}`);
               res.status(500).json({ error: 'Invalid JSON format', message: parseError.message });
            }
         });
      });
   });
});

module.exports = router;
