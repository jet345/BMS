// LINE CHARTS
$(document).ready(function() {
	var result_x, result_temp, result_hum;
	var initial_temp = new Array();
	var initial_hum = new Array();
	var electricity = new Array();
	var initial_gauge = new Array();
	Highcharts.setOptions({
        global: {
            useUTC: false
        }
    });
	
	//ajax로 app.js에서 db값 받아오기.
	$.ajax({ 
		url:"http://35.185.138.8:3000/initial_dht/",
		type:"POST",
		dataType: "json",
		async: false,
		success: function(result_line) {
			if (result_line) { 
				var result_str_line = JSON.stringify(result_line);
				var result_final_line = JSON.parse(result_str_line);
				var for_time = new Date();
				for (var i =0;i<7;i++){
					var result_elem_line = result_final_line[i];
					var result_time = result_elem_line['month']+"/"+result_elem_line['date'] +"/"+ result_elem_line['year']+ " " +result_elem_line['hour'] +':'+result_elem_line['min'] + ':00';
					console.log(result_time);
					var temp_time = Date.parse(result_time);
					result_x = (new Date(temp_time)).getTime();
					result_temp = parseInt(result_elem_line['temp'],10);
					result_hum = parseInt(result_elem_line['hum'],10);
					initial_temp.push({
						x: result_x,
						y: result_temp
					});
					initial_hum.push({
						x: result_x,
						y: result_hum
					});
					initial_gauge[0] = parseInt(result_elem_line['lux'],10);
				}
				for (var i =7;i<9;i++){
					var result_elem_bar = result_final_line[i];
					var result_bar = result_elem_bar['amount'];
					electricity.push(result_bar);
				}
				console.log("success");
			} 
		},
		error: function (result_line) {
			console.log('err: '+result_line);
		}
	});
	var chart_bar = Highcharts.chart({
		chart: { 
			renderTo: 'bargraph',
			height: 200,
			backgroundColor :'transparent',
		},
		title: {
			text: null
		},
		xAxis: {
			categories: ['2016', '2017']
		},
		credits: { 
			enabled: false 
		},	
		series: [{
			type: 'column',
			colorByPoint: true,
			data: electricity,
			showInLegend: false
		}]

	});
    var chart_line = new Highcharts.Chart({
      chart: {
		renderTo: 'linegraph',
        type: 'spline',
        backgroundColor: 'transparent',
        height: 200,
		marginLeft: 50,
        marginRight: 50,
        marginBottom: 0,
        marginTop: 40,
        /*events: {
            load: function () {
                // set up the updating of the chart each second
                var serie1 = this.series[0];
				var serie2 = this.series[1];
                setInterval(function () {
					//ajax로 app.js에서 db값 받아오기.
					$.ajax({ 
						url:"http://35.185.138.8:3000/dht/",
						type:"POST",
						dataType: "json",
						success: function(result) {
							if (result) { 
								var result_time = "01/01/2000 " + result['hour'] +':'+result['min'] + ':00';
								var temp_time = Date.parse(result_time);
								result_x = (new Date(temp_time)).getTime();
								result_temp = parseInt(result['temp'],10);
								result_hum = parseInt(result['hum'],10);
							} 
						}
						
					});
					serie1.addPoint([result_x, result_temp], true, true);
					serie2.addPoint([result_x, result_hum], true, true);
				}, 5000);
			}
		}*/   
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
		width : 550,
		align: 'middle',
      },
      yAxis: {
		height: 100,
		tickWidth: 1,
		verticalAlign: 'middle',
        gridLineWidth: 0,
        title: {
          text: 'temperture (°C)',
		  color: '#FFFFFF'
        },
		labels: {
          align: 'left',
          x: 3,
          y: 16,
          format: '{value:.,0f}'
        },
		showFirstLabel: false
      },
      series: [{
            name: 'Temperture',
			color: '#FF3333',
			marker: {
				enabled: false
			},
            data: initial_temp
        },
		{
            name: 'Humidity',
			color: '#33CCFF',
			marker: {
				enabled: false
			},
            data: initial_hum
        }],
      credits: { 
        enabled: false 
      },
      legend: { 
		title: {
            text: '<span style="color: #EEEEEE">      Sensor</span><span style="font-size: 9px; color: #EEEEEE; font-weight: normal"> (Click to hide)</span>',
        },
        enabled: true, 
		align : 'center',
		verticalAlign: 'bottom',
		layout : 'horizontal',
		y: 20,
		itemStyle: {
            color: '#EEEEEE'
        }
      },	
      tooltip: { 
        enabled: true
      }
    }); 
	var chart_gauge = Highcharts.chart({
		chart: {
			renderTo: 'gaugegraph',
			type: 'solidgauge',
			backgroundColor:'transparent',
			height: 250,
			marginBottom: 30,
			events: {
				load: function () {
					var series = this.series[0];
					var point = series.points[0];
					setInterval(function () {
						//ajax로 app.js에서 db값 받아오기.
						/*$.ajax({ 
							url:"http://35.185.138.8:3000/dht/",
							type:"POST",
							dataType: "json",
							success: function(result) {
								if (result) { 
									var result_time = "01/01/2000 " + result['hour'] +':'+result['min'] + ':00';
									var temp_time = Date.parse(result_time);
									result_x = (new Date(temp_time)).getTime();
									result_temp = parseInt(result['temp'],10);
									result_hum = parseInt(result['hum'],10);
								} 
							}						
						});*/
						inc = Math.round((Math.random() - 0.5) * 100);
						newVal = point.y + inc;

						if (newVal < 0 || newVal > 200) {
							newVal = point.y - inc;
						}
						point.update(newVal);
					}, 5000);
				}
			}   
		},
		
		title: null,

		pane: {
			center: ['50%', '50%'],
			size: '100%',
			startAngle: 0,
			endAngle: 360,
			background: {
				backgroundColor: Highcharts.Color(Highcharts.getOptions().colors[0]).setOpacity(0.3).get(),
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
				linecap: 'round',
				rounded: true,
			}
		},
		
		yAxis: {
			min: 0,
			max: 500,
			lineWidth: 0,
			tickPositions: []
		},

		credits: {
			enabled: false
		},

		series: [{
			name: 'Light',
			data: initial_gauge,
			dataLabels: {
				verticalAlign: 'middle',
				format: '<div style="text-align:center"><span style="font-size:25px;color:{point.color};">{y}</span><br/>' +
					   '<span style="font-size:12px;color:silver">lx</span></div>'		
			}
		}]

	});
	
});
