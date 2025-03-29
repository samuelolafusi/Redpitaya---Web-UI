// Define an immediately invoked function expression (IIFE) to create an APP object and avoid global scope pollution
(function(APP, $, undefined) {
    // Set up configuration for the app
    APP.config = {};                           // Create an empty config object
    APP.config.app_id = 'ledv5_7';             // Unique ID for the app (version 5 with 7+ LEDs)
    APP.config.app_url = '/bazaar?start=' + APP.config.app_id + '?' + location.search.substr(1); // URL to start the app, including query parameters
    APP.config.socket_url = 'ws://' + window.location.hostname + ':9002'; // WebSocket URL using the current hostname

    // Initialize WebSocket variable (will hold the connection)
    APP.ws = null;

    // Track the state of 8 LEDs (true = on, false = off)
    APP.led_states = [false, false, false, false, false, false, false, false]; // Array for 8 LEDs, all off by default

    // Function to start the app by making a request to the server
    APP.startApp = function() {
        $.get(APP.config.app_url)              // Send a GET request to the app URL
            .done(function(dresult) {          // If the request succeeds
                if (dresult.status === 'OK') { // Check if the server says everything is good
                    APP.connectWebSocket();    // Connect to the WebSocket
                } else if (dresult.status === 'ERROR') { // If the server returns an error
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
                console.log('WebSocket error: ', ev); // Log the error details
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
    // Dynamically create controls for 8 LEDs and add them to the LED grid
    for (var i = 0; i < 8; i++) {
        var ledControl = $('<div class="led-control"></div>'); // Create a container for each LED
        var ledLabel = $('<h3>LED_' + i + '</h3>'); // Create a heading with the LED number
        var switchLabel = $('<p>Switch</p>'); // Create a label for the toggle switch
        var ledToggle = $('<label class="led-toggle"><input type="checkbox" data-led="' + i + '"><span class="slider"></span></label>'); // Create a checkbox toggle with LED index
        var bulb = $('<span class="bulb"></span>'); // Create a span for the bulb visual
        var lightIcon = $('<span class="light-icon"> O </span>'); // Create a simple text-based light icon (could be replaced with an emoji like 💡)

        // Add all elements to the LED control container
        ledControl.append(ledLabel).append(switchLabel).append(ledToggle).append(lightIcon).append(bulb);
        $('.led-grid').append(ledControl); // Add the LED control to the grid in HTML
    }

    // Add an event handler for when any LED toggle checkbox changes
    $('.led-toggle input[type="checkbox"]').on('change', function() {
        var ledIndex = $(this).data('led'); // Get the LED index from the data attribute
        var isChecked = $(this).is(':checked'); // Check if the toggle is on or off
        var bulb = $(this).closest('.led-control').find('.bulb'); // Find the bulb span in the same LED control

        // Update the bulb’s color based on the toggle state
        bulb.css('background-color', isChecked ? 'yellow' : 'white'); // Yellow for on, white for off

        // Update the LED state in the array
        APP.led_states[ledIndex] = isChecked;

        // Send the state change to the server via WebSocket
        if (APP.ws && APP.ws.readyState === WebSocket.OPEN) { // Check if WebSocket is connected
            var local = {}; // Create an object to hold the data
            local['LED' + ledIndex + '_STATE'] = { value: isChecked }; // Add the LED state with its number (matches C++ parameter names)
            APP.ws.send(JSON.stringify({ parameters: local })); // Send the data as JSON
        } else {
            console.log('WebSocket is not open'); // Log an error if the connection isn’t ready
        }
    });

    // Start the application when the page loads
    APP.startApp();
});