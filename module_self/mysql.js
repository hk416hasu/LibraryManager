const mysql = require('mysql2');

const connection = mysql.createConnection({
    host: '82.156.207.47',
    user: 'hk416hasu',
    password: '12345678hhh'
});

connection.query(
    'show databases;',
    function (err, res, fields) {
        console.log(res);
        console.log(fields);
    }
);

connection.end();
