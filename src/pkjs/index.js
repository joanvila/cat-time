var apiKey = 'YOUR API KEY HERE';

function getWeatherMapping(weatherId) {
    var weatherCode = 'n';

    if (weatherId >= 200 && weatherId < 300) weatherCode = 'U';
    else if (weatherId >= 300 && weatherId < 600) weatherCode = 'S';
    else if (weatherId >= 600 && weatherId < 700) weatherCode = 'W';
    else if (weatherId >= 700 && weatherId < 800) weatherCode = 'G';
    else if (weatherId == 800) weatherCode = 'A';
    else if (weatherId >= 801 && weatherId < 804) weatherCode = 'C';
    else if (weatherId == 804) weatherCode = 'P';
    else if (weatherId >= 900 && weatherId < 950) weatherCode = 'p';

    return weatherCode;
};

var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

function locationSuccess(pos) {
    var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
        pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + apiKey;

    xhrRequest(url, 'GET', function(responseText) {
        var json = JSON.parse(responseText);

        // Temperature in Kelvin requires adjustment
        var temperature = Math.round(json.main.temp - 273.15);
        console.log('Temperature is ' + temperature);

        // Weather id
        var weatherId = json.weather[0].id;
        console.log('Weather is ' + weatherId + ' ' + json.weather[0].main);

        var responseObject = {
            'TEMPERATURE': temperature,
            'WEATHER': getWeatherMapping(weatherId)
        };
        Pebble.sendAppMessage(responseObject, function(e) {
            console.log('Weather info sent to Pebble successfully!');
        }, function(e) {
            console.log('Error sending weather info to Pebble!');
        });
    });
}

function locationError(err) {
    console.log('Error requesting location!');
}

function getWeather() {
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {timeout: 15000, maximumAge: 60000}
    );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', function(e) {
    console.log('PebbleKit JS ready!');
    getWeather();
});

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage', function(e) {
    console.log('AppMessage received!');
    getWeather();
});
