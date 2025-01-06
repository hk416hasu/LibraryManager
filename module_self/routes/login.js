const express = require('express');
const router = express.Router();
const { execFile } = require('child_process');
const path = require('path');
const fs = require('fs');

router.post('', (req, res) => {
   console.log(req.body);

   const elfPath = './build/main';
   const jsonPath = './build/data.json';

   const modifiedBody = {
      ...req.body,
      method: 'login'  // 添加方法字段
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

module.exports = router;
