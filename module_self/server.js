const express = require('express');
const path = require('path');
const app = express();

function startServer(rootPath) {

   app.use((req, res, next) => {
      console.log(`Request received: ${req.method} ${req.url}`);
      next();
   });

   app.use(express.static(path.join(rootPath, 'public')));

   const exeDemoRouter = require('./routes/executeHello.js');
   app.use('/api', exeDemoRouter);

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
