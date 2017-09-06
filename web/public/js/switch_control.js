$(document).ready(function() {
	var temp = document.getElementById("tempCounter");
	var onoff = document.getElementById("ap1");
	var socket = io.connect('35.185.138.8:3000');
	
	//임시로 초기값 설정.
	temp.innerHTML = "26";
	temp.style.color = "#666666";
	var tempValue = 26;
	
	onoff.innerHTML= 'ON';
	onoff.style.color='#888888';
	
	socket.on('connect', function() {
	});
	
	socket.emit('setreq_1',{data:'1층 현재 상태를 보내주쎄오~~~'}); //초기세팅을 위한 시그널 전송.
	socket.on('setres_1', function (data) {
    //여기서 초기상태 setting.
    });
	
	//다른 클라이언트가 조명을 제어했을 때 받아오는 부분.
	socket.on('lightreceive_1', function (data) {
        console.log(data.msg);
		if(data.msg=='a'){
			document.getElementById('off').checked=true;
		}
		else if(data.msg=='b'){
			document.getElementById('light1').checked=true;
		}
		else if(data.msg=='c'){
			document.getElementById('light2').checked=true;
		}
		else {
			document.getElementById('light3').checked=true;
		}
    });
	//다른 클라이언트가 에어컨을 제어했을 때 받아오는 부분.
	socket.on('airconreceive', function (data) {
        console.log(data.msg);
		if(data.msg=='11'){
			onoff.innerHTML= 'OFF';
			onoff.style.color='#47b8e0';
		}
		else if(data.msg=='10'){
			onoff.innerHTML= 'ON';
			onoff.style.color='#888888';
		}
		else {
			tempValue=parseInt(data.msg);
			temp.innerHTML=tempValue;
		}
    });
	
	//조명 제어
	$('#off_1').mouseup(function(){
		console.log("a");
		socket.emit('lightcontrol_1', { power: 'a' }); //서버로 신호 전달.
	});
	$('#light_1').mouseup(function(){
		console.log("b");
		socket.emit('lightcontrol_1', { power: 'b' }); //서버로 신호 전달.
	});
	$('#light_2').mouseup(function(){
		console.log("c");
		socket.emit('lightcontrol_1', { power: 'c' }); //서버로 신호 전달.
	});
	$('#light_3').mouseup(function(){
		console.log("d");
		socket.emit('lightcontrol_1', { power: 'd' }); //서버로 신호 전달.
	});
	
	//에어컨 제어
	$('#upButton').click(function(){
		if (parseInt(temp.innerHTML)<29 && onoff.innerHTML=='OFF'){
			tempValue++;
			socket.emit('aircontrol', { value: tempValue }); //서버로 신호 전달.
			temp.innerHTML = tempValue;
			console.log(temp.innerHTML);
		}
	})

	$('#downButton').click(function(){
		if (parseInt(temp.innerHTML)>18 && onoff.innerHTML=='OFF'){
			tempValue--;
			socket.emit('aircontrol', { value: tempValue }); //서버로 신호 전달.
			temp.innerHTML = tempValue;
			console.log(temp.innerHTML);
		}
	})
	$('#ap1').mousedown(function(){

		if (onoff.innerHTML == 'ON'){
			onoff.innerHTML= 'OFF';
			onoff.style.color='#47b8e0';
			console.log("11");
			socket.emit('aircontrol', { value: '11' }); //서버로 신호 전달.
		}
		else if (onoff.innerHTML == 'OFF'){
			onoff.innerHTML= 'ON';
			onoff.style.color='#888888';
			console.log("10");
			socket.emit('aircontrol', { value: '10' }); //서버로 신호 전달.
		}	
		
	});
	
});