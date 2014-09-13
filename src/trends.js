// Configuration
var DEVICE_ID = '53ff78065075535147131587';
var ACCESS_TOKEN = '4a5ad03cc220cd0e50c81dffff7fa64ed667c6b0';

// App Keys
var
	KEY_GET_TRENDS = 'KEY_GET_TRENDS';

// Trend global storage
var trends;
var trendIndex = 0;

/**
 * Import jQuery to the document
 */
var importjQuery = function() {
    var script = document.createElement('script');
    script.src = 'http://code.jquery.com/jquery-latest.min.js';
    script.type = 'text/javascript';
    document.getElementsByTagName('head')[0].appendChild(script);
};

/**
 * Use jQuery to query the Spark API
 */
function sparkRequest(deviceId, accessToken, functionName, args, callback) {
  // Construct URL
  var URL = 'https://api.spark.io/v1/devices/' + deviceId + '/' + functionName + '?access_token=' + ACCESS_TOKEN;

  console.log('ajax: URL=' + URL);

  //Send with jQuery
  $.ajax({
    type: 'POST',
    url: URL,
    data: {
			'args': args
		},
    success: callback,
		error: function(error, e1, e2) {
			console.log('error in ajax');
		},
    dataType: 'json'
  });
}

var sendIndex = 0;

function sendNextTrend() {
	var dict = {
		sendIndex: trends[sendIndex].name
	};
	
	Pebble.sendAppMessage(dict,
		function(e) {
			sendIndex += 1;
			
			if(sendIndex < trends.length) {
				sendNextTrend();
			} else {
				console.log('Send complete!');
			}
		},
		function(e) {
			console.log('Error in sendNextTrend()');
		}
	);
}

/**
 * When the trends array has been written
 */
function getTrendsSuccess() {
	// Print all trends
	console.log();
	for(var i = 0; i < trends.length; i++) {
		console.log('Trend #' + i + ': ' + trends[i].name);
	}
	
	// Start sending to Pebble
	sendNextTrend();

	// Loop display on Spark LCD
	/* setInterval(function() {
		trendIndex += 1;
		if(trendIndex >= trends.length) {
			trendIndex = 0;
		}

		sparkRequest(DEVICE_ID, ACCESS_TOKEN, 'set', trends[trendIndex].name, function() {
			console.log('Send index ' + trendIndex);
		});
	}, 1500); */
}

/**
 * Download the trends and execute getTrendsSuccess if that
 */
function getTrends() {
	// Get the JSON
	$.ajax({
    type: 'GET',
    url: 'http://api.whatthetrend.com/api/v2/trends.json',
		success: function(response) {
			// Save the results
			trends = response.trends;
			getTrendsSuccess();
		},
		error: function(error, e1, e2) {
			console.log('error in ajax');
		},
    dataType: 'json'
  });
}

/****************************** Pebble App Events ****************************/

Pebble.addEventListener('ready', 
  function(e) {
    // Import jQuery
		importjQuery();
		
    console.log('PebbleKit JS ready!');
  }
);

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('appmessage');

// 		if(e.payload[KEY_GET_TRENDS] !== null) {
// 			// Pebble wants to get the current trends
// 			getTrends();
// 		}
  }
);

console.log('everything set up');