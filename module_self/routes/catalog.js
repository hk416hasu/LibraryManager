const express = require('express');
const router = express.Router();
const { execFile } = require('child_process');
const path = require('path');
const fs = require('fs');
const PDFDocument = require('pdfkit');
const doc = new PDFDocument();

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

            // 检查目标目录是否存在
            const pdfDir = '/home/ubuntu/web/public/doc';
            if (!fs.existsSync(pdfDir)) {
               fs.mkdirSync(pdfDir, { recursive: true });
            }

            // 创建 PDF 并保存到服务器或内存
            const pdfFilename = `trans_${Date.now()}.pdf`;
            const pdfPath = path.join(pdfDir, pdfFilename);

            const writeStream = fs.createWriteStream(pdfPath);
            writeStream.on('finish', () => {
               console.log('PDF 文件写入成功:', pdfPath);
            });
            writeStream.on('error', (err) => {
               console.error('PDF 写入错误:', err.message);
               throw err;
            });

            // 写入内容并结束
            doc.pipe(writeStream);
            doc.font('/home/ubuntu/ftp/font.otf').fontSize(20).text('移送清单:').moveDown();
            jsonData.array_key.forEach((item, index) => {
               const bookDetails = `
         ${index + 1}. 书名: ${item.bookTitle || 'Unknown'}
                       ISBN: ${item.ISBN || 'Unknown'}
                       索书号: ${item.CLCNum || 'Unknown'}/${item.callNum || 'Unknown'}
                       出版社: ${item.press || 'Unknown'}
            `;
               doc.fontSize(12).text(bookDetails).moveDown();
            });
            doc.end();

            // 验证文件是否成功创建
            if (!fs.existsSync(pdfPath)) {
               return res.status(500).json({ error: 'PDF file was not created' });
            }

            // 返回 JSON 数据，附加 PDF 文件链接
            res.status(200).json({
               ...jsonData,
               pdfURL: `/doc/${pdfFilename}`,
            });
         } catch (err) {
            console.error('生成 PDF 失败:', err.message);
            res.status(500).json({
               error: 'Failed to generate PDF',
               message: err.message,
            });
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
