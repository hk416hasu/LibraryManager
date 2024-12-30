const express = require('express');
const router = express.Router();
const { execFile } = require('child_process'); // the two 
const path = require('path');                  // are paired

router.post('/Hello', (req, res) => {
   const elfPath = './build/hello'; // relate to the Node-Working-dir, NOT current js
   const args = ['yixuan', 'guanxin', 'tianci', 'guoli', 'Nana'];

   execFile(elfPath, args, (error, stdout, stderr) => {
      if (error) {
         console.error(`执行错误: ${error.message}`);
         return res.status(500).send('执行错误');
      }
      if (stderr) {
         console.error(`标准错误: ${stderr}`);
         return res.status(500).send(`标准错误: ${stderr}`);
      }
      res.send(stdout.trim()); // 返回 ELF 文件的输出
   });
});

module.exports = router;
