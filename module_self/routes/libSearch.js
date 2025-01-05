const express = require('express');
const router = express.Router();
const mysql2 = require('mysql2');

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

module.exports = router;
