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
      // method: 'catalog'  // 添加方法字段
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
               // 创建 PDF 并保存到服务器或内存
               if (Array.isArray(jsonData.array_key)) {
                  const pdfFilename = `trans_${Date.now()}.pdf`;
                  const PDFDocument = require('pdfkit');
                  const doc = new PDFDocument();

                  doc.text('移送清单:', { underline: true }); // Title
                  jsonData.array_key.forEach((item, index) => {

                     const ISBN = item.ISBN || 'Unknown';
                     const bookTitle = item.bookTitle || 'Unknown';
                     const callNum = item.callNum || 'Unknown';
                     const CLCNum = item.CLCNum || 'Unknown';
                     const press = item.press || 'Unknown';

                     doc.text(`${index + 1}. 书名: ${bookTitle}, ISBN: ${ISBN}, 索书号: ${CLCNum/callNum}, 出版社: ${press}`);
                  });
                  const pdfPath = path.join(__dirname, 'public/doc', pdfFilename);
                  doc.pipe(fs.createWriteStream(pdfPath)); // 保存到服务器文件
                  doc.end();

                  // 返回 JSON，附加 PDF 文件的下载链接
                  res.status(200).json({
                     ...jsonData,
                     pdf_url: `/public/doc/${pdfFilename}`,
                  });
               } else {
                  res.status(400).json({ message: 'array_key is missing or not an array' });
               }
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

router.post('/destroy', (req, res) => {
   console.log(req.body);

   const elfPath = './build/main';
   const jsonPath = './build/data.json';

   const modifiedBody = {
      ...req.body,
      method: 'destroy'  // 添加方法字段
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
