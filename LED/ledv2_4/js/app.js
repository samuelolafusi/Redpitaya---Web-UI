(function(APP, $, undefined) {
    
    // App configuration
    APP.config = {};
    APP.config.app_id = 'ledv2_4';
    APP.config.app_url = '/bazaar?start=' + APP.config.app_id + '?' + location.search.substr(1);
    APP.config.socket_url = 'ws://' + window.location.hostname + ':9002';

    // WebSocket
    APP.ws = null;

    // LED states 
    APP.led_states = [false, false, false, false];

    APP.startApp = function() {
        $.get(APP.config.app_url)
            .done(function(dresult) {
                if (dresult.status == 'OK') {
                    APP.connectWebSocket();
                } else if (dresult.status == 'ERROR') {
                    console.log(dresult.reason ? dresult.reason : 'Could not start the application (ERR1)');
                    APP.startApp();
                } else {
                    console.log('Could not start the application (ERR2)');
                    APP.startApp();
                }
            })
            .fail(function() {
                console.log('Could not start the application (ERR3)');
                APP.startApp();
            });
    };

    APP.connectWebSocket = function() {
        //Create WebSocket
        if (window.WebSocket) {
            APP.ws = new WebSocket(APP.config.socket_url);
            APP.ws.binaryType = "arraybuffer";
        } else if (window.MozWebSocket) {
            APP.ws = new MozWebSocket(APP.config.socket_url);
            APP.ws.binaryType = "arraybuffer";
        } else {
            console.log('Browser does not support WebSocket');
        }

        // Define WebSocket event listeners
        if (APP.ws) {
            APP.ws.onopen = function() {
                $('#hello_message').text("Hello User!");
                console.log('Socket opened');   
            };

            APP.ws.onclose = function() {
                console.log('Socket closed');
            };

            APP.ws.onerror = function(ev) {
                $('#hello_message').text("Connection error");
                console.log('Websocket error: ', ev);         
            };

            APP.ws.onmessage = function(ev) {
                console.log('Message received');
            };
        }
    };

}(window.APP = window.APP || {}, jQuery));

// Page onload event handler
$(function() {
    // Create buttons for each LED
    for (var i = 1; i <= 4; i++) {
        var ledControl = $('<div class="led-control"></div>');
        var stateButton = $('<button class="led_state" data-led="' + i + '">LED ' + i + '</button>');
        var status = $('<span class="led_status">LED ' + i + ' is off</span>');
        ledControl.append(stateButton).append(status);
        $('#content').append(ledControl);
    }

    // Click handler for all LED state buttons
    $('.led_state').click(function() {
        var ledIndex = $(this).data('led') - 1;
        var statusSpan = $(this).next('.led_status');
        if (APP.led_states[ledIndex]) {
            statusSpan.text('LED ' + (ledIndex + 1) + ' is off').removeClass('led_on').addClass('led_off');
            APP.led_states[ledIndex] = false;
        } else {
            statusSpan.text('LED ' + (ledIndex + 1) + ' is on').removeClass('led_off').addClass('led_on');
            APP.led_states[ledIndex] = true;
        }
        var local = {};
        local['LED' + (ledIndex + 1) + '_STATE'] = { value: APP.led_states[ledIndex] };
        APP.ws.send(JSON.stringify({ parameters: local }));
    });

    // Start application
    APP.startApp();
});