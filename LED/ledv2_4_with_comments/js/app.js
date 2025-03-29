// Define an immediately invoked function expression (IIFE) to create an APP object and avoid global scope pollution
(function(APP, $, undefined) {
    
    // Set up configuration for the app
    APP.config = {};                           // Create an empty config object
    APP.config.app_id = 'ledv2_4';             // Unique ID for the app (version 2 with 4 LEDs)
    APP.config.app_url = '/bazaar?start=' + APP.config.app_id + '?' + location.search.substr(1); // URL to start the app, including query parameters
    APP.config.socket_url = 'ws://' + window.location.hostname + ':9002'; // WebSocket URL using the current hostname

    // Initialize WebSocket variable (will hold the connection)
    APP.ws = null;

    // Track the state of 4 LEDs (true = on, false = off)
    APP.led_states = [false, false, false, false]; // Array to store the state of each LED

    // Function to start the app by making a request to the server
    APP.startApp = function() {
        $.get(APP.config.app_url)              // Send a GET request to the app URL
            .done(function(dresult) {          // If the request succeeds
                if (dresult.status == 'OK') {  // Check if the server says everything is good
                    APP.connectWebSocket();    // Connect to the WebSocket
                } else if (dresult.status == 'ERROR') { // If the server returns an error
                    console.log(dresult.reason ? dresult.reason : 'Could not start the application (ERR1)'); // Log the reason or a default message
                    APP.startApp();            // Retry starting the app
                } else {                       // If the status is unexpected
                    console.log('Could not start the application (ERR2)'); // Log an error
                    APP.startApp();            // Retry starting the app
                }
            })
            .fail(function() {                 // If the request fails completely
                console.log('Could not start the application (ERR3)'); // Log an error
                APP.startApp();                // Retry starting the app
            });
    };

    // Function to connect to the WebSocket server
    APP.connectWebSocket = function() {
        // Create a WebSocket connection based on browser support
        if (window.WebSocket) {                // If the browser supports standard WebSocket
            APP.ws = new WebSocket(APP.config.socket_url); // Create a new WebSocket connection
            APP.ws.binaryType = "arraybuffer"; // Set the data type to arraybuffer for binary data
        } else if (window.MozWebSocket) {      // If the browser uses Mozilla's WebSocket (older browsers)
            APP.ws = new MozWebSocket(APP.config.socket_url); // Create a Mozilla WebSocket connection
            APP.ws.binaryType = "arraybuffer"; // Set the data type to arraybuffer
        } else {                               // If WebSocket is not supported
            console.log('Browser does not support WebSocket'); // Log an error message
        }

        // Set up event listeners for the WebSocket if it exists
        if (APP.ws) {
            APP.ws.onopen = function() {       // When the WebSocket connection opens
                $('#hello_message').text("Hello User!"); // Update the hello message on the page
                console.log('Socket opened');  // Log that the connection is open
            };

            APP.ws.onclose = function() {      // When the WebSocket connection closes
                console.log('Socket closed');  // Log that the connection is closed
            };

            APP.ws.onerror = function(ev) {    // When there’s an error with the WebSocket
                $('#hello_message').text("Connection error"); // Show an error message on the page
                console.log('Websocket error: ', ev); // Log the error details
            };

            APP.ws.onmessage = function(ev) {  // When a message is received from the server
                console.log('Message received'); // Log that a message came in
            };
        }
    };

// Pass the APP object to the global window scope and include jQuery
}(window.APP = window.APP || {}, jQuery));

// When the page finishes loading, run this code
$(function() {
    // Create buttons and status text for each of the 4 LEDs
    for (var i = 1; i <= 4; i++) {
        var ledControl = $('<div class="led-control"></div>'); // Create a container div for each LED
        var stateButton = $('<button class="led_state" data-led="' + i + '">LED ' + i + '</button>'); // Create a button for toggling the LED
        var status = $('<span class="led_status">LED ' + i + ' is off</span>'); // Create a span to show the LED status
        ledControl.append(stateButton).append(status); // Add the button and status to the container
        $('#content').append(ledControl); // Add the container to the content div
    }

    // Add a click event handler for all LED state buttons
    $('.led_state').click(function() {
        var ledIndex = $(this).data('led') - 1; // Get the LED number (1-4) and adjust to array index (0-3)
        var statusSpan = $(this).next('.led_status'); // Find the status span next to the clicked button
        if (APP.led_states[ledIndex]) {       // If the LED is currently on
            statusSpan.text('LED ' + (ledIndex + 1) + ' is off') // Update status text to "off"
                .removeClass('led_on').addClass('led_off'); // Switch CSS class to show off state
            APP.led_states[ledIndex] = false;  // Update the LED state to off
        } else {                               // If the LED is currently off
            statusSpan.text('LED ' + (ledIndex + 1) + ' is on') // Update status text to "on"
                .removeClass('led_off').addClass('led_on'); // Switch CSS class to show on state
            APP.led_states[ledIndex] = true;   // Update the LED state to on
        }
        var local = {};                        // Create an object to hold data to send
        local['LED' + (ledIndex + 1) + '_STATE'] = { value: APP.led_states[ledIndex] }; // Add the LED state with its number
        APP.ws.send(JSON.stringify({ parameters: local })); // Send the state to the server as JSON
    });

    // Start the application when the page loads
    APP.startApp();
});