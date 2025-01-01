const express = require('express');
const router = express.Router();
const mysql2 = require('mysql2');

const pool = mysql2.createPool({
   host: 'localhost',
   user: 'hk416hasu',
   password: '12345678hhh',
   database: 'test_db',
});

router.post('', (req, res) => {
   const { username, hashed_passwd } = req.body;

   console.log("username is : " + username);
   console.log("hashed_passwd is : " + hashed_passwd);

   const query = `SELECT * FROM users WHERE username = ? AND hashed_passwd = ?`;

   // 执行参数化查询, 防治SQL注入攻击
   pool.execute(query, [username, hashed_passwd], (err, results) => {
      if (err) {
         console.error('Database query error: ', err);
         return res.status(500).json({ error: 'Database error' });
      }

      if (results.length > 0) {
         res.status(200).json({
            message: 'User authenticated',
            authority_num: results[0].authority_num,
         });
      } else {
         res.status(401).send('Invalid username or password');
      }
   });
});

module.exports = router;
