var http = require('http');
var fs = require('fs');
var url = require('url');
var path = require('path'); // resolve the extend-name of files

// create a server
const server = http.createServer(function (request, response) {
    // resolve the request and file
    var pathname = url.parse(request.url).pathname;
    console.log("Request for " + pathname + " received.");

    // set default file (index.html)
    var filePath = pathname === "/" ? "./index.html" : "." + pathname;

    // read the expect file from fs
    fs.readFile(filePath, function (err, data) {
        if (err) {
            console.log(err);
            // if error, reture 404 not found
            response.writeHead(404, { 'Content-Type': 'text/html' });
            fs.readFile("./404.html", function (error, errorData) {
                if (error) {
                    console.log("Error reading 404.html");
                } else {
                    response.end(errorData); // return 404.html content
                }
            });
        } else {
            // get extend-name of file, and get Content-Type
            var extname = path.extname(filePath);
            var contentType = getContentType(extname);

            // HTTP status: 200 : OK
            response.writeHead(200, { 'Content-Type': contentType });

            // response with file
            response.end(data);
        }
    });
});

// mapping of Content-Type and extend-name
function getContentType(extname) {
    var mimeTypes = {
        '.html': 'text/html',
        '.js': 'application/javascript',
        '.css': 'text/css',
        '.json': 'application/json',
        '.png': 'image/png',
        '.jpg': 'image/jpg',
        '.ico': 'image/x-con',
        '.gif': 'image/gif',
        '.mp3': 'audio/mpeg',
        '.mp4': 'video/mp4',
        '.wav': 'audio/wav',
        '.svg': 'image/svg+xml',
        '.txt': 'text/plain'
    };
    // return mimeTypes[extname] || 'application/octet-stream';
    return mimeTypes[extname];
}

const startServer = (port) => {
    server.listen(port, '0.0.0.0', () => {
        console.log('server is running');
    }); 
};

module.exports = { startServer };
