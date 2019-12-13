// created a variable for http module
var http = require('http');

// created a variable for file server module 
var fs = require('fs');

// created a variable for index page
var index = fs.readFileSync( 'index.html');

// created a variable for serial port module
var SerialPort = require('serialport');

// created a constant for serial port parsers
const parsers = SerialPort.parsers;

// created a constant for parsers
// helpful: https://serialport.io/docs/api-parser-readline
const parser = new parsers.Readline({
    delimiter: '\r\n'
});

// created variable for ports
var port = new SerialPort('COM5',{ 
    baudRate: 9600,
    dataBits: 8,
    parity: 'none',
    stopBits: 1,
    flowControl: false
});

// 
port.pipe(parser);

// created variable for server that passes request & response
var app = http.createServer(function(req, res) {
    
    // write header 
    res.writeHead(200, {'Content-Type': 'text/html'});
    
    // end response with index
    res.end(index);
});

// created a variable to socket.io that requires http server
var io = require('socket.io').listen(app);

// when socket.io connects
io.on('connection', function(socket) {
    
    // display message on CLI
    console.log('Node is listening to port');
    
    // when receiving data from the browser
    socket.on("userInput", function(data) {
        
        // display message on CLI
        console.log("User selected: " + data); 
        
        // write data
        port.write(data.status); 
    
    });    
    
});

// when receive info from arduino
parser.on('data', function(data) {
    
    // display message on CLI
    console.log('Received data from port: ' + data);
    
    // sending to all connected clients
    io.emit('data', data);
    
});

// listen on port 3000
app.listen(3000);