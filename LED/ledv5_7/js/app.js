(function(APP, $, undefined) {
    // App configuration
    APP.config = {};
    APP.config.app_id = 'ledv5_7';
    APP.config.app_url = '/bazaar?start=' + APP.config.app_id + '?' + location.search.substr(1);
    APP.config.socket_url = 'ws://' + window.location.hostname + ':9002';

    // WebSocket
    APP.ws = null;

    // All LED initail states (values) are set here
    APP.led_states = [false, false, false, false, false, false, false, false];

    // Function to start the application by fetching the app URL
    APP.startApp = function() {
        $.get(APP.config.app_url)
            .done(function(dresult) {
                if (dresult.status === 'OK') {
                    APP.connectWebSocket();
                } else if (dresult.status === 'ERROR') {
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

    // Connect to WebSocket
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
                console.log('WebSocket error: ', ev);
            };

            APP.ws.onmessage = function(ev) {
                console.log('Message received');
            };
        }
    };

}(window.APP = window.APP || {}, jQuery));

// Page onload event handler
// this part fills up the led grid in html dymically
$(function() {
    for (var i = 0; i < 8; i++) {
        var ledControl = $('<div class="led-control"></div>');
        var ledLabel = $('<h3>LED_' + i + '</h3>');
        var switchLabel = $('<p>Switch</p>');
        var ledToggle = $('<label class="led-toggle"><input type="checkbox" data-led="' + i + '"><span class="slider"></span></label>');
        var bulb = $('<span class="bulb"></span>');
        var lightIcon = $('<span class="light-icon"> O </span>'); //💡

        ledControl.append(ledLabel).append(switchLabel).append(ledToggle).append(lightIcon).append(bulb);
        $('.led-grid').append(ledControl);
    }

    // Change bulb color and send WebSocket messages on toggle
    $('.led-toggle input[type="checkbox"]').on('change', function() {
        var ledIndex = $(this).data('led');
        var isChecked = $(this).is(':checked');
        var bulb = $(this).closest('.led-control').find('.bulb');

        // Update bulb color
        bulb.css('background-color', isChecked ? 'yellow' : 'white');

        // Update LED state
        APP.led_states[ledIndex] = isChecked;

        // Send the state change to the server
        // note the parameter name here in BLOCK letters,
        // same name is used in CPP
        if (APP.ws && APP.ws.readyState === WebSocket.OPEN) {
            var local = {};
            local['LED' + ledIndex + '_STATE'] = { value: isChecked };
            APP.ws.send(JSON.stringify({ parameters: local }));
        } else {
            console.log('WebSocket is not open');
        }
    });

    // Start the application
    APP.startApp();
});
