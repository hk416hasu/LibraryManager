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
   const { username, hashed_passwd, email, tel } = req.body;

   console.log("username is : " + username);
   console.log("hashed_passwd is : " + hashed_passwd);

   const query = `SELECT * FROM users WHERE username = ?`;

   // 查重
   pool.execute(query, [username], (err, results) => {
      if (err) {
         console.error('Database query error: ', err);
         return res.status(500).json({ error: 'Database error' });
      }

      console.log(err);
      console.log(results);
      console.log(results.length);

      if (results.length == 0) {
         const insertQuery = `INSERT INTO users (username, hashed_passwd) VALUES (?, ?)`;
         pool.execute(insertQuery, [username, hashed_passwd], (err, results) => {
            if (err) {
               console.error('Error inserting user: ', err);
               return res.status(500).json({ error: 'Failed to create user' });
            }

            console.log('User created successfully: ', results);
            res.status(201).json({ message: 'User created successfully' });
         });
      } else {
         res.status(409).json({ error: 'Username already exists' });
      }
   });
});

module.exports = router;
