const express = require('express'); // "require" to import a "factory function"
const app = express();              // get an instance of `express`
const path = require('path');

function startServer(rootPath) {

   app.use((req, res, next) => {
      console.log(`Request received: ${req.method} ${req.url}`);
      next();
   });

   // app.use(express.static(path.join(rootPath, 'public')));
   app.use(express.static('public'));

   const exeDemoRouter = require('./routes/executeHello.js');
   app.use('/api', exeDemoRouter);

   app.use((req, res, next) => {
      res.status(404).sendFile(path.join(rootPath, 'public', '404.html'));
   });

   app.use((err, req, res, next) => {
      res.status(500).send('Internel Server Error');
   });

   app.listen(3000, '0.0.0.0', () => {
      console.log('Server is running');
   });
}

module.exports = { startServer };
