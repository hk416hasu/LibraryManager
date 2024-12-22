const express = require('express');
const { execFile }  = require('child_process');
const path = require('path');
const app = express();

function startServer(rootPath) {

    app.use((req, res, next) => {
        console.log(`Request received: ${req.method} ${req.url}`);
        next();
    });

    app.use(express.static(path.join(rootPath, 'public')));

    app.post('/executeHello', (req, res) => {
        //const elfPath = '/home/ubuntu/nodejs/exe/hello';
        const elfPath = './build/hello';
        const args = ['yixuan', 'liguan', 'tianci', 'guoli'];

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
            console.log(stdout.trim());
        });
    });

    app.use((req, res, next) => {
        res.status(404).sendFile(path.join(rootPath, 'public', '404.html'));
    });

    app.use((err, req, res, next) => {
        res.status(500).send('Internel Server Error');
    });

    app.listen(80, '0.0.0.0', () => {
        console.log('Server is running');
    });
}

module.exports = { startServer };
