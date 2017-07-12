var net = require('net'); //tcp
var tcp_client = net.connect({port:5000}, function(){
  console.log('client - server success');
});

tcp_client.write('hello');

tcp_client.on('data',function(data){
	console.log(data);
});

tcp_client.on('end',function(){
	console.log('connection free');
});