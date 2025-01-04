const express = require('express');
const app = express();

app.use(express.json()); // analyze json

app.use((req, res, next) => {
   console.log(`Request received: ${req.method} ${req.url} `);
   console.log(req.body);
   res.status(200).json({ message: 'test ok' });
});

app.use((req, res, next) => {
   res.status(404).sendFile(path.join('/home/ubuntu/web/', 'public', '404.html'));
});

app.use((err, req, res, next) => {
   res.status(500).send('Internel Server Error');
});

app.listen(8080, '0.0.0.0', () => {
   console.log('Zhao_server is running');
});
