const fs = require('fs');
const { exec } = require('child_process');

// 示例 ISBN 列表
const isbns = [
   "9787115441249",
   "9787115599438",
   "9787115565198"
];

// 查询单个 ISBN 的信息
function fetchBookInfo(isbn) {
   return new Promise((resolve, reject) => {
      exec(`node isbn2.js ${isbn}`, (error, stdout, stderr) => {
         if (error) {
            console.error(`执行错误: ${error.message}`);
            return reject(error);
         }
         if (stderr) {
            console.error(`标准错误输出: ${stderr}`);
            return reject(new Error(stderr));
         }
         try {
            const result = JSON.parse(stdout);
            if (result.success && result.data && result.data.details && result.data.details.length > 0) {
               resolve(result.data.details[0]);
            } else {
               reject(new Error(`未找到 ISBN ${isbn} 的信息`));
            }
         } catch (err) {
            reject(new Error(`解析错误: ${err.message}`));
         }
      });
   });
}

// 生成 MySQL 插入语句的函数
function generateInsertStatements(books) {
   const statements = books.map(book => {
      return `INSERT INTO book_circulation (
        ISBN, callNum, CLCNum, bookTitle, author, press, pressDate, introduction, leftNum, totalNum
    ) VALUES (
        '${book.isbn}',
        '${book.genus}',
        '${book.genus}',
        '${book.title}',
        '${book.author}',
        '${book.publisher}',
        '${book.pubDate}',
        ${book.gist ? `'${book.gist.replace(/'/g, "''")}'` : 'NULL'},
        10,
        10
    );`;
        });
  return statements.join('\n\n');
}

// 主函数
async function main() {
  const books = [];
  for (const isbn of isbns) {
    try {
      console.log(`查询 ISBN: ${isbn}`);
           const bookInfo = await fetchBookInfo(isbn);
           books.push(bookInfo);
        } catch (err) {
           console.error(`错误: ${err.message}`);
        }
   }

      if (books.length > 0) {
         const sqlStatements = generateInsertStatements(books);
         console.log(sqlStatements);
      } else {
         console.log('未生成任何 SQL 插入语句。');
      }
}

main();

