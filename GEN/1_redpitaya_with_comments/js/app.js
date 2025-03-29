// Define an immediately invoked function expression (IIFE) to create an APP object and avoid global scope pollution
(function(APP, $, undefined) {
    
    // Set up configuration for the app
    APP.config = {};                           // Create an empty config object
    APP.config.app_id = '1_redpitaya';         // Unique ID for the app
    APP.config.app_url = '/bazaar?start=' + APP.config.app_id + '?' + location.search.substr(1); // URL to start the app, including query parameters
    APP.config.socket_url = 'ws://' + window.location.hostname + ':9002'; // WebSocket URL using the current hostname

    // Initialize WebSocket variable (will hold the connection)
    APP.ws = null;

    // Object to hold the plot (for graphing signals)
    APP.plot = {};

    // Array to store incoming signal data
    APP.signalStack = [];

    // Define initial parameter values
    APP.frequency = 1;    // Default frequency (1 Hz)
    APP.amplitude = 0;    // Default amplitude (0 V)
    APP.waveform = 0;     // Default waveform (0, likely a code for a specific type)
    APP.processing = false; // Flag to prevent overlapping signal processing
    APP.gain = 1;         // Default gain (1x)

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
                $('#hello_message').text("Hello User!, Welcome to the Gen!"); // Update welcome message
                console.log('Socket opened');  // Log that the connection is open
                // Set initial parameters when the connection opens
                APP.setGain();
                APP.setFrequency();
                APP.setAmplitude();
                APP.setWaveform();
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

                // Skip processing if already handling a message
                if (APP.processing) {
                    return;
                }
                APP.processing = true;         // Set flag to indicate processing

                try {
                    var data = new Uint8Array(ev.data); // Convert received data to Uint8Array
                    var inflate = pako.inflate(data);   // Decompress data using pako (assumes pako.js is included)
                    var text = String.fromCharCode.apply(null, new Uint8Array(inflate)); // Convert to string
                    var receive = JSON.parse(text);     // Parse the JSON string

                    if (receive.parameters) {  // If parameters are received (not processed here yet)
                        // Placeholder for parameter handling
                    }

                    if (receive.signals) {     // If signals are received
                        APP.signalStack.push(receive.signals); // Add signals to the stack for processing
                    }
                    APP.processing = false;    // Reset processing flag
                } catch (e) {
                    APP.processing = false;    // Reset flag on error
                    console.log(e);            // Log any errors
                } finally {
                    APP.processing = false;    // Ensure flag is reset
                }
            };
        }
    };

    // Function to set the gain value and send it to the server
    APP.setGain = function() {
        APP.gain = $('#gain_set').val();       // Get the gain value from the input field
        var local = {};                        // Create an object for the parameter
        local['GAIN'] = { value: APP.gain };   // Add gain to the object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send as JSON to the server
        $('#gain_value').text(APP.gain);       // Update the displayed gain value
    };

    // Function to set the frequency value and send it to the server
    APP.setFrequency = function() {
        APP.frequency = $('#frequency_set').val(); // Get the frequency value from the input field
        var local = {};                        // Create an object for the parameter
        local['FREQUENCY'] = { value: APP.frequency }; // Add frequency to the object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send as JSON to the server
        $('#frequency_value').text(APP.frequency); // Update the displayed frequency value
    };

    // Function to set the amplitude value and send it to the server
    APP.setAmplitude = function() {
        APP.amplitude = $('#amplitude_set').val(); // Get the amplitude value from the input field
        var local = {};                        // Create an object for the parameter
        local['AMPLITUDE'] = { value: APP.amplitude }; // Add amplitude to the object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send as JSON to the server
        $('#amplitude_value').text(APP.amplitude); // Update the displayed amplitude value
    };

    // Function to set the waveform value and send it to the server
    APP.setWaveform = function() {
        APP.waveform = $('#waveform_set').val(); // Get the waveform value from the input field
        console.log('Set to ' + APP.waveform);   // Log the selected waveform
        var local = {};                          // Create an object for the parameter
        local['WAVEFORM'] = { value: APP.waveform }; // Add waveform to the object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send as JSON to the server
    };

    // Function to process new signal data and update the plot
    APP.processSignals = function(new_signals) {
        var pointArr = [];  // Array to hold plot data
        var voltage;        // Variable to store the latest voltage value

        // Loop through each signal in the received data
        for (sig_name in new_signals) {
            if (new_signals[sig_name].size == 0) continue; // Skip empty signals

            var points = [];   // Array to hold signal points
            for (var i = 0; i < new_signals[sig_name].size; i++) {
                points.push([i, new_signals[sig_name].value[i]]); // Add x (index), y (value) pairs
            }
            pointArr.push(points); // Add points to the plot array
            voltage = new_signals[sig_name].value[new_signals[sig_name].size - 1]; // Get the last voltage value
        }

        // Update the displayed voltage value with 2 decimal places
        $('#value').text(parseFloat(voltage).toFixed(2) + "V");

        // Update the plot with new data
        APP.plot.setData(pointArr); // Set the new data
        APP.plot.resize();          // Resize the plot
        APP.plot.setupGrid();       // Redraw the grid
        APP.plot.draw();            // Render the plot
    };

    // Function to handle signals from the stack
    APP.signalHandler = function() {
        if (APP.signalStack.length > 0) {      // If there are signals to process
            APP.processSignals(APP.signalStack[0]); // Process the first signal
            APP.signalStack.splice(0, 1);       // Remove the processed signal
        }
        if (APP.signalStack.length > 2)        // If the stack grows too large
            APP.signalStack.length = [];       // Clear the stack to avoid backlog
    };
    setInterval(APP.signalHandler, 15);        // Run signal handler every 15ms

// Pass the APP object to the global window scope and include jQuery
}(window.APP = window.APP || {}, jQuery));

// When the page finishes loading, run this code
$(function() {
    // Initialize the plot using Flot (assumes jquery.flot.js is included)
    APP.plot = $.plot($("#placeholder"), [], { 
        series: {
            shadowSize: 0, // Disable shadows for faster drawing
        },
        yaxis: {
            min: 0,        // Minimum y-axis value (0V)
            max: 5         // Maximum y-axis value (5V)
        },
        xaxis: {
            min: 0,        // Minimum x-axis value (0)
            max: 1024,     // Maximum x-axis value (1024 samples)
            show: false    // Hide x-axis labels
        }
    });

    // Event handler for gain input changes
    $("#gain_set").on("change input", function() {
        APP.setGain(); // Update gain when the input changes
    });
    
    // Event handler for frequency input changes
    $("#frequency_set").on("change input", function() {
        APP.setFrequency(); // Update frequency when the input changes
    });

    // Event handler for amplitude input changes
    $("#amplitude_set").on("change input", function() {
        APP.setAmplitude(); // Update amplitude when the input changes
    });

    // Event handler for waveform input changes
    $("#waveform_set").on("change", function() {
        APP.setWaveform(); // Update waveform when the input changes
    });

    // Start the application when the page loads
    APP.startApp();
});