// LINE AND BARS CHARTS
$(document).ready(function() {
	var result_x, result_temp, result_hum;
	var initial_temp = new Array();
	var initial_hum = new Array();
	Highcharts.setOptions({
        global: {
            useUTC: false
        }
    });
	//ajax로 app.js에서 db값 받아오기.
	$.ajax({ 
		url:"http://35.185.138.8:3000/initial/",
		type:"POST",
		dataType: "json",
		async: false,
		success: function(result) {
			if (result) { 
				var result_str = JSON.stringify(result);
				console.log(result_str);
				var result_final = JSON.parse(result_str);
				for (var i =0;i<7;i++){
					var result_elem = result_final[i];
					var result_time = "01/01/2000 " + result_elem['hour'] +':'+result_elem['min'] + ':00';
					var temp_time = Date.parse(result_time);
					result_x = (new Date(temp_time)).getTime();
					result_temp = parseInt(result_elem['temp'],10);
					result_hum = parseInt(result_elem['hum'],10);
					initial_temp.push({
						x: result_x,
						y: result_temp
					});
					initial_hum.push({
						x: result_x,
						y: result_hum
					});
					console.log("success");
				}
			} 
		},
		error: function (result) {
			//alert('error: ' + ress);
			console.log('err: '+result);
		}
	});
    var chart = new Highcharts.Chart({
      chart: {
		renderTo: 'importantchart',
        type: 'spline',
        backgroundColor: 'transparent',
        height: 200,
		marginLeft: 50,
        marginRight: 50,
        marginBottom: 45,
        marginTop: 45,
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
		width : 450,
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
            text: '<span style="color: #EEEEEE">Sensor</span><br/><span style="font-size: 9px; color: #EEEEEE; font-weight: normal">(Click to hide)</span>',
        },
        enabled: true, 
		align : 'right',
		verticalAlign: 'middle',
		layout : 'vertical',
		itemStyle: {
            color: '#EEEEEE'
        }
      },	
      tooltip: { 
        enabled: true
      }
    }); 
});
