
$(document).ready(function() {
	var socket = io.connect('35.185.138.8:3000');
	var result_x,result_temp, result_hum,result_lux,result_power;
	var initial_temp = new Array();
	var initial_hum = new Array();
	var initial_lux = new Array(); //텍스트로 띄울 것
	var initial_power = new Array();
	var electricity = new Array(); //그래프로 그릴 것
	var chart_line, chart_gauge;
	Highcharts.setOptions({
        global: {
            useUTC: false
        }
    });
	socket.on('connect', function() {
	});	
	socket.emit('initial_data',{data:'.'});
	socket.on('init_1', function (result_line) {
		if (result_line) { 
			var result_str_line = JSON.stringify(result_line);
			var result_final_line = JSON.parse(result_str_line);
			var for_time = new Date();
			//전력 그래프 셋팅 : 최근 30분간의 전력 (5분단위.)
			for (var i =5;i>=0;i--){
				var result_elem_line = result_final_line[i];
				var result_time = result_elem_line['month']+"/"+result_elem_line['date'] +"/"+ result_elem_line['year']+ " " +result_elem_line['hour'] +':'+result_elem_line['minute'] + ':00';
				console.log(result_time);
				var temp_time = Date.parse(result_time);
				result_x = (new Date(temp_time)).getTime();
				result_power = parseFloat(result_elem_line['power']); //전력
				initial_power.push({
					x: result_x,
					y: result_power
				});
				if (i==5){
					initial_hum[0]=result_elem_line['hum']; //습도
					initial_temp[0]=result_elem_line['temp']; //온도
					initial_lux[0]=result_elem_line['lux']; //조도
				}
			}
			var result_elem_bar = result_final_line[6];
			electricity.push(result_elem_bar['month']); //월별로 limit 조정!
			electricity.push(result_elem_bar['amount']);
			$( "#temperture" ).html(initial_temp[0]);
			$( "#humidity" ).html(initial_hum[0]);
			$( "#bright" ).html(initial_lux[0]);
			//실시간 전력 그래프
			chart_line = new Highcharts.Chart({
				chart: {
					renderTo: 'linegraph',
					type: 'spline',
					backgroundColor: 'transparent',
					height: 180,
					marginLeft: 40,
					marginRight: 30,
					style: {
						fontFamily: 'Ubuntu, sans-serif'
					}	
				},
				title: {
					text: null,
					verticalAlign : 'top',
					color : '#666'
				},
				xAxis: {
					type: 'datetime',
					lineWidth: 0,
					tickWidth: 1,
					align: 'middle',
				},
				yAxis: {
					height: 180,
					tickWidth: 1,
					verticalAlign: 'middle',
					gridLineWidth: 0,
					title: {
					  text: 'power (W)',
					},
					labels: {
					  align: 'left',
					  x: 3,
					  y: 16,
					  format: '{value}'
					},
					minRange:30,
				},
				series: [{
					name: 'Power',
					color: '#ff7473',
					yAxis:0,
					marker: {
						enabled: false
					},
					data: initial_power
				}],
				credits: { 
					enabled: false 
				},
				legend: { 
					enabled: false
				},	
				tooltip: { 
					enabled: true
				}
			}); 
			//월별 전력량 그래프
			chart_gauge = new Highcharts.chart({
				chart: {
					renderTo: 'gaugegraph',
					type: 'solidgauge',
					backgroundColor:'transparent',
					height: 180,
					marginBottom: 30,
					style: {
						fontFamily: 'Ubuntu, sans-serif'
					}			
				},
				title: null,
				pane: {
					center: ['50%', '50%'],
					size: '100%',
					startAngle: 0,
					endAngle: 360,
					background: {
						backgroundColor: Highcharts.Color('#ffc952').setOpacity(0.3).get(),
						innerRadius: '60%',
						outerRadius: '100%',
						borderWidth: 0
					}
				},
				tooltip: {
					enabled: false
				},
				plotOptions: {
					solidgauge: {
						dataLabels: {
							y: 0,
							borderWidth: 0,
							useHTML: true
						},
					}
				},
				yAxis: {
					min: 0,
					max: 1000,
					lineWidth: 0,
					tickPositions: []
				},
				credits: {
					enabled: false
				},
				series: [{
					name: 'Amount of electricity',
					data: [{
						color: '#ffc952',
						y: electricity[1],
					}],
					dataLabels: {
						verticalAlign: 'middle',
						format: '<div style="text-align:center"><span style="font-size:12px;color:{point.color};">{y}</span><br/>' +
							   '<span style="font-size:9px;color:silver">kWh</span></div>'		
					}
				}]
			});
		} 	
    });

	setInterval(function() {
		socket.emit('data',{data:'.'});
	}, 300000); //5분에 한번
	
	socket.on('data_1', function (result){
		var result_time = result['month']+"/"+result['date'] +"/"+ result['year']+ " " + + result['hour'] +':'+result['minute'] + ':00';
		
		var temp_time = Date.parse(result_time);
		result_x = (new Date(temp_time)).getTime();
		result_power = parseFloat(result['power']);
		result_temp = parseInt(result['temp'],10);
		result_hum = parseInt(result['hum'],10);
		result_lux = parseInt(result['lux'],10);
		var chart0 = chart_line;
		chart0.series[0].addPoint([result_x, result_power], true, true);
		//텍스트 띄우기 
		$( "#temperture" ).html(result_temp);
		$( "#humidity" ).html(result_hum);
		$( "#bright" ).html(result_lux);
	});
});
