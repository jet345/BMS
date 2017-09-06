//app.js
var net = require('net'); //tcp
var express = require('express');
var app = express(),
  server = require('http').createServer(app),
  io = require('socket.io').listen(server);
var mysql = require('mysql');
var router = require('./router/main')(app); 
var router2 = require('./router/main2')(app); //router
var router3 = require('./router/home')(app); //router
var bodyparser = require('body-parser');
var path = require("path");
var FCM = require('fcm-node');
var fs = require("fs");
var PythonShell = require('python-shell');
var cors = require('cors');

var tcp_client = net.connect({port:5000, host:'222.97.239.226'}, function(){
  console.log('client - server success');
});

server.listen(3000);

//ajax를 위한 middleware 추가
app.use(bodyparser.urlencoded({ extended: false }));
app.use(bodyparser.json());
//크로스 도메인 해결.
app.use(cors());
//css, javascript 사용
app.use(express.static(path.join(__dirname, '/public'))); 

//mysql 셋팅
var pool = mysql.createPool({
	host: '35.185.138.8',
	port: 3306,
	user: 'root',
	password: 'rudal45',
	database: 'buildingmanager'
});

var resultObject = {};
var result;
var resultJson;


// routing
app.set('views', __dirname + '/views');
app.set('view engine', 'ejs');
app.engine('html', require('ejs').renderFile);

//fcm 셋팅.
var serverkey='AAAAICDYIgA:APA91bH_6aS10ZVTKwnyMKtzuwgfy8rvZHaEsGgylwHVHaHB-Xc8C98NB2nTyuEsCJUPgZCCCyDBDBVuvAEL_kRhui2v-IRdQWuwPsLZ6kWlbJE0JtO9K2dY4EvOPBYY98nWvwzchX3x';
var fcm = new FCM(serverkey);
var message_fire1 = {
	to: '/topics/subin',
	data:{
		'title':'buildingmanager',
		'body': '1층에서 화재발생이 감지되었습니다.'
	}	
}
var message_fire2 = {
	to: '/topics/subin',
	data:{
		'title':'buildingmanager',
		'body': '2층에서 화재발생이 감지되었습니다.'
	}	
}
var message_enter = {
	to: '/topics/subin',
	data:{
		'title':'buildingmanager',
		'body': '신원불명자 출입시도.'
	}	
}
io.sockets.on('connection',function(socket){
/****************그래프*****************/
	// 1층그래프 초기값
	socket.on('initial_data',function(req) {	
		//connect DB
		pool.getConnection(function(err, connection) {
			var selectDML_init;
			var for_time = new Date(); 
			resultObject.year = parseInt(for_time.getFullYear());
			resultObject.month = parseInt(for_time.getMonth()+1);
			if (err) {
				return console.log("디비커넥션에러 %s \n%s", err.message, err.stack);
			}
			selectDML_init = "SELECT temp, hum, lux, power, year, month, date, hour, minute FROM test_new order by year desc, month desc, date desc, hour desc, minute desc limit 6";
			connection.query(selectDML_init, function(err, rows) {
				if (err) {
					return console.log("SELECT error %s \n%s", err.message, err.stack);
				} else {				
					var strJson_init = JSON.stringify(rows);
					resultJson = JSON.parse(strJson_init);
					console.log(resultJson);
				}
			});
			
			var selectDML_elec_init = "SELECT month,amount FROM electricity WHERE year=? and month=?";
			var placeHolers_elec_init = [resultObject.year, resultObject.month];
			
			connection.query(selectDML_elec_init, placeHolers_elec_init, function(err, rows) {
				if (err) {
					return console.log("SELECT error %s \n%s", err.message, err.stack);
				} else {	
					var strJson_elec = JSON.stringify(rows);
					strJson_elec = JSON.parse(strJson_elec);					
					resultJson.push(strJson_elec[0]);
				}		
			});
			connection.release();
		});
		socket.emit('init_1',resultJson);
		console.log(resultJson);
		resultObject = [];
		resultJson = null;
	});
	// 2층 그래프 초기값.
	socket.on('initial_data2',function(req) {	
		//connect DB
		pool.getConnection(function(err, connection) {
			var selectDML_init;
			var for_time = new Date(); 
			resultObject.year = parseInt(for_time.getFullYear());
			resultObject.month = parseInt(for_time.getMonth()+1);
			if (err) {
				return console.log("디비커넥션에러 %s \n%s", err.message, err.stack);
			}
			selectDML_init = "SELECT temp, hum, lux, power, year, month, date, hour, minute FROM test_new order by year desc, month desc, date desc, hour desc, minute desc limit 6";
			connection.query(selectDML_init, function(err, rows) {
				if (err) {
					return console.log("SELECT error %s \n%s", err.message, err.stack);
				} else {				
					var strJson_init = JSON.stringify(rows);
					resultJson = JSON.parse(strJson_init);
					console.log(resultJson);
				}
			});
			
			var selectDML_elec_init = "SELECT month,amount FROM electricity WHERE year=? and month=?";
			var placeHolers_elec_init = [resultObject.year, resultObject.month];
			
			connection.query(selectDML_elec_init, placeHolers_elec_init, function(err, rows) {
				if (err) {
					return console.log("SELECT error %s \n%s", err.message, err.stack);
				} else {	
					var strJson_elec = JSON.stringify(rows);
					strJson_elec = JSON.parse(strJson_elec);					
					resultJson.push(strJson_elec[0]);
				}		
			});
			connection.release();
		});
		socket.emit('init_2',resultJson);
		console.log(resultJson);
		resultObject = [];
		resultJson = null;
	});
	//1층 실시간 그래프 그리는 데이터 가져오는 부분.
	socket.on('data',function(req) {
		pool.getConnection(function(err, connection) {
			if (err) {
				return console.log("디비커넥션에러 %s \n%s", err.message, err.stack);
			}
			var selectDML = "SELECT temp, hum, lux, power, year, month, date, hour, minute FROM test_new order by year desc, month desc, date desc, hour desc, minute desc limit 6";

			connection.query(selectDML, function(err, rows) {
				if (err) {
					return console.log("SELECT error %s \n%s", err.message, err.stack);
				} else {
					result = rows[0];
					console.log(rows[0]);
				}
				connection.release();
			});
		});
		
		socket.emit('data_1',result);
		console.log(result);
		result = null;
	});
	//2층 실시간 그래프 그리는 데이터 가져오는 부분.
	socket.on('data2',function(req) {
		pool.getConnection(function(err, connection) {
			if (err) {
				return console.log("디비커넥션에러 %s \n%s", err.message, err.stack);
			}
			var selectDML = "SELECT temp, hum, lux, power, year, month, date, hour, minute FROM test_new order by year desc, month desc, date desc, hour desc, minute desc limit 6";

			connection.query(selectDML, function(err, rows) {
				if (err) {
					return console.log("SELECT error %s \n%s", err.message, err.stack);
				} else {
					result = rows[0];
					console.log(rows[0]);
				}
				connection.release();
			});
		});
		
		socket.emit('data_2',result);
		console.log(result);
		result = null;
	});

/****************제어부*****************/
	//light& aircon 초기상태 받아오기!
	socket.on('setreq_1',function(req){
		console.log(req.data);
		tcp_client.write('y'); //1층 현재 조명상태와 에어컨 상태 보내달라는 신호.
		tcp_client.on('data', function(data){
			socket.emit('setres_1',{msg:data}); //data = '조명세기,에어컨전원,설정온도,설정바람세기'
		});
	});
	//light& aircon 초기상태 받아오기!
	socket.on('setreq_2',function(req){
		console.log(req.data);
		tcp_client.write('z');
		tcp_client.on('data', function(data){
			socket.emit('setres_2',{msg:data}); //data = '조명세기,에어컨전원,설정온도,설정바람세기'
		});
	});
	//1층 light 제어
	socket.on('lightcontrol_1',function(signal){
		console.log(signal.power);
		tcp_client.write(signal.power);//라즈베리에 신호 전송.
		io.sockets.emit('lightreceive_1',{msg : signal.power}); //다른 클라이언트에게도 전송
	});
	//2층 light 제어
	socket.on('lightcontrol_2',function(signal){
		console.log(signal.power);
		tcp_client.write(signal.power); //라즈베리에 신호전송.
		io.sockets.emit('lightreceive_2',{msg : signal.power}); //다른 클라이언트에게도 전송
		
	});
	//에어컨 제어
	socket.on('aircontrol',function(signal){
		console.log(signal.value);
		tcp_client.write((signal.value).toString()); //라즈베리에 신호전송.
		io.sockets.emit('airconreceive',{msg : signal.value}); //다른 클라이언트에게도 전송
	});
});


/***************출입부***************/
app.post('/photo/',function(req,res) {
	console.log("enter");
	var imgdata = req.body.imgBase64;
	var matches = imgdata.match(/^data:.+\/(.+);base64,(.*)$/);
	var ext = matches[1];
    var base64_data = matches[2];
	var imgfinal = new Buffer(base64_data, 'base64');
	fs.writeFile('./images/image.jpg', imgfinal, 'base64');
	console.log('done');
	var options = {
		  mode: 'text',
		  pythonPath: '',
		  pythonOptions: [],
		  scriptPath: './openface/demos/',
		  args: ['infer', './learning_pic/embeddings-pic/classifier.pkl', './images/image.jpg']
	};
	
	PythonShell.run('classifier.py', options, function (err, results) {
		if (err) throw err;
		console.log('results: %s', results[0]);
		var strArray=results[0].split(',');
		if(parseFloat(strArray[1])>=0.90){
			//여기에서 tcp로 open신호 보냄, ajax로 success return함.
			tcp_client.write('o');
			console.log('o');	
			res.json({msg:'문이 열렸습니다.'});
		}
		else {
			//여기에서는 fail return함.
			console.log('x');
			res.json({msg:'출입이 불가합니다.'});
			fcm.send(message_enter,function(err, response){
				if(err){
					console.log(err);
				}
				else {
					console.log("sent"+response);
				}
			});
		}
	});
});
var count =0;
// 1분에 한번씩 화재판단 후 fcm을 통해 사용자에게 푸시알람! app.post 에 넘길 것--> 아직 테스트 안함.
/*setInterval(function() {
	pool.getConnection(function(err, connection) {
		var for_time = new Date(); 
		
		resultObject_f.year = parseInt(for_time.getFullYear());
		resultObject_f.month = parseInt(for_time.getMonth()+1);
		resultObject_f.date = parseInt(for_time.getDate());
		resultObject_f.hour = for_time.getHours();
		resultObject_f.minute = for_time.getMinutes()-1;
		console.log(resultObject_f);
		
		if (err) {
			return console.log("디비커넥션에러 %s \n%s", err.message, err.stack);
		}
		var selectDML = "SELECT temp, hum, gas FROM sensor WHERE year=? and month=? and date=? and hour=? and min=?";
		var placeHolers = [resultObject_f.year,resultObject_f.month,resultObject_f.date,resultObject_f.hour,resultObject_f.minute];

		connection.query(selectDML, placeHolers, function(err, rows) {
			if (err) {
				return console.log("SELECT error %s \n%s", err.message, err.stack);
			} else {
				result_f_1 = rows[0];
				console.log(rows[0]);
			}
		});
		selectDML = "SELECT temp, hum, gas FROM sensor2 WHERE year=? and month=? and date=? and hour=? and min=?";
		connection.query(selectDML, placeHolers, function(err, rows) {
			if (err) {
				return console.log("SELECT error %s \n%s", err.message, err.stack);
			} else {
				result_f_2 = rows[0];
				console.log(rows[0]);
			}
			connection.release();
		});
	});
	resultObject = [];
	if(count==0){ //서버를 처음 켰을 때,
		lastsensor_1 = result_f_1;
		lastsensor_2 = result_f_2;
		count++;
	}	
	else {
		if(result_f_1.temp-lastsensor_1.temp>=5&&lastsensor_1.hum-result_f_1.hum>=3 && lastsensor_1.gas-result_f_1.gas>=10){
			
		}
	}
	
	fcm.send(message_fcm,function(err, response){
		if(err){
			console.log(err);
		}
		else {
			console.log("sent"+response);
		}
	});
	
}, 60000);*/



