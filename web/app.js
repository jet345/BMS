//app.js
var net = require('net'); //tcp
var express = require('express');
var app = express(),
  server = require('http').createServer(app),
  io = require('socket.io').listen(server);
var mysql = require('mysql');
var router = require('./router/main')(app); //router
var bodyparser = require('body-parser');
var path = require("path")

server.listen(3000);

//ajax를 위한 middleware 추가
app.use(bodyparser.urlencoded({ extended: false }));
app.use(bodyparser.json());

//css, javascript 사용
app.use(express.static(path.join(__dirname, '/public'))); 

var pool = mysql.createPool({
	host: '127.0.0.1',
	port: 3306,
	user: 'root',
	password: 'rudal45',
	database: 'buildingmanager'
});

var resultObject = {};
var result;

// routing
app.set('views', __dirname + '/views');
app.set('view engine', 'ejs');
app.engine('html', require('ejs').renderFile);


// ajax응답
app.post('/initial/',function(req,res) {	
	console.log('req');
	
	//connect DB
	pool.getConnection(function(err, connection) {
		console.log("success");
		var for_time = new Date();

		resultObject.year = parseInt(for_time.getFullYear());
		console.log(resultObject.year);
		resultObject.month = parseInt(for_time.getMonth()+1);
		console.log(resultObject.month);
		resultObject.date = parseInt(for_time.getDate());
		console.log(resultObject.date);
		resultObject.hour = for_time.getHours();
		resultObject.minute = for_time.getMinutes();
		
		if (err) {
			return console.log("디비커넥션에러 %s \n%s", err.message, err.stack);
		}
		var selectDML = "SELECT temp, hum, hour, min FROM dht WHERE hour=? and min<=? and min>=?";

		var placeHolers = [21, 38, 32];

		connection.query(selectDML, placeHolers, function(err, rows) {
			if (err) {
				return console.log("SELECT error %s \n%s", err.message, err.stack);
			} else {
				result = rows;
				//console.log(result);
			}
			connection.release();
		});
		
	});
	res.json(result);
	console.log(result);
});


app.post('/dht/',function(req,res) {
	pool.getConnection(function(err, connection) {
		console.log("success");
		var for_time = new Date();

		resultObject.year = for_time.year;
		resultObject.mon = for_time.month;
		resultObject.date = for_time.day;
		resultObject.hour = for_time.hour;
		resultObject.min = for_time.min;
		if (err) {
			return console.log("디비커넥션에러 %s \n%s", err.message, err.stack);
		}
		var selectDML = "SELECT temp, hum, hour, min FROM dht WHERE year=? and mon=? and date=? and hour=? and min=?";

		var placeHolers = [resultObject.year,resultObject.mon,resultObject.date,'00','00'];

		connection.query(selectDML, placeHolers, function(err, rows) {
			if (err) {
				return console.log("SELECT error %s \n%s", err.message, err.stack);
			} else {
				result = rows[0];
				
			}
			connection.release();
		});
		
	});
	console.log("req");
	res.json(result);
	console.log(result);
});


