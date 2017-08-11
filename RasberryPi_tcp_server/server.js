//** server TO raspberry pi tcp connection **//
var net = require('net'); //tcp
var port = 5000;
var tcp_server = net.createServer();

var Gpio = require('onoff').Gpio;
var led = new Gpio(21,'out');
var led2 = new Gpio(20,'out');
var led3 = new Gpio(16,'out');
var led4 = new Gpio(19,'out');

tcp_server.on('connection',function(tcp_socket){
  console.log('tcp connection...');
  tcp_socket.on('end',function(){
    console.log('rasp_client socket close');
    setTimeout(function(){
      tcp_server.close();
    },5000);
  });
  tcp_socket.on('data',function(data){
   	var is_open = data.indexOf('open');
  	var is_close = data.indexOf('close');
  	console.log('is_close : '+is_close);
  	console.log('is_open : '+is_open);
    if(is_open==1){
  		var a = data.toString();
  		var valve_id = a.substring(0,1);
		  valve_id = Number(valve_id); 
  		switch(valve_id){
  			case 1:
  				led.writeSync(1);
  				break;
  			case 2:
  				led2.writeSync(1);
  				break;
  			case 3:
  				led3.writeSync(1);
  				break;
  			case 4:
  				led4.writeSync(1);
  				break;
  		}
  		is_open=-1;
  	}

  	else if(is_close==1){
  		var a = data.toString();
  		var valve_id = a.substring(0,1);
      valve_id = Number(valve_id);
      console.log('close'); 
  		switch(valve_id){
  			case 1:
  				led.writeSync(0);
  				break;
  			case 2:
  				led2.writeSync(0);
  				break;
  			case 3:
  				led3.writeSync(0);
  				break;
  			case 4:
  				led4.writeSync(0);
  				break;
  		}
  		is_close=-1;
  	}

  });
});

tcp_server.on('close',function(){
  console.log('tcp_server closed');
});

tcp_server.listen(port);
//** server TO raspberry pi tcp connection **//
