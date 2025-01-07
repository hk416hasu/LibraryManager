const express = require('express');
const router = express.Router();
const mysql2 = require('mysql2');

const pool = mysql2.createPool({
   host: 'localhost',
   user: 'guoli',
   password: 'uneasilycaption',
   database: 'test_zhao',
});

router.get('/getAll', (req, res) => {

   const query = `SELECT * FROM dingdan`;

   pool.execute(query, (err, results) => {
      if (err) {
         console.error('Database query error: ', err);
         return res.status(500).json({ error: 'Database error' });
      }

      res.json(results);
   });
});

module.exports = router;
