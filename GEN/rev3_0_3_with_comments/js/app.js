// Define an immediately invoked function expression (IIFE) to create an APP object and avoid global scope pollution
(function(APP, $, undefined) {
    
    // Set up configuration for the app
    APP.config = {};                           // Create an empty config object
    APP.config.app_id = 'rev3_0_3';            // Unique ID for the app (revision 3.0.3)
    APP.config.app_url = '/bazaar?start=' + APP.config.app_id + '?' + location.search.substr(1); // URL to start the app, including query parameters
    APP.config.socket_url = 'ws://' + window.location.hostname + ':9002'; // WebSocket URL using the current hostname

    // Initialize WebSocket variable (will hold the connection)
    APP.ws = null;

    // Object to hold the plot (for graphing signals)
    APP.plot = {};

    // Array to store incoming signal data
    APP.signalStack = [];

    // Define initial parameters for Channel 1
    APP.frequency = 1;    // Default frequency (1 Hz)
    APP.amplitude = 0;    // Default amplitude (0 V)
    APP.waveform = 0;     // Default waveform (0, likely sine)
    APP.processing = false; // Flag to prevent overlapping signal processing
    APP.gain = 1;         // Default gain (1x)

    // Define initial parameters for Channel 2
    APP.frequency1 = 1;   // Default frequency (1 Hz)
    APP.amplitude1 = 0;   // Default amplitude (0 V)
    APP.waveform1 = 0;    // Default waveform (0, likely sine)
    APP.processing1 = false; // Flag for Channel 2 processing (unused here)
    APP.gain1 = 1;        // Default gain (1x)

    // Function to start the app by making a request to the server
    APP.startApp = function() {
        $.get(APP.config.app_url)              // Send a GET request to the app URL
            .done(function(dresult) {          // If the request succeeds
                if (dresult.status == 'OK') {  // Check if the server says everything is good
                    APP.connectWebSocket();    // Connect to the WebSocket
                } else if (dresult.status == 'ERROR') { // If the server returns an error
                    console.log(dresult.reason ? dresult.reason : 'Could not start the application (ERR1)'); // Log reason or default message
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
            APP.ws.binaryType = "arraybuffer"; // Set data type to arraybuffer for binary data
        } else if (window.MozWebSocket) {      // If the browser uses Mozilla's WebSocket (older browsers)
            APP.ws = new MozWebSocket(APP.config.socket_url); // Create a Mozilla WebSocket connection
            APP.ws.binaryType = "arraybuffer"; // Set data type to arraybuffer
        } else {                               // If WebSocket is not supported
            console.log('Browser does not support WebSocket'); // Log an error message
        }

        // Set up event listeners for the WebSocket if it exists
        if (APP.ws) {
            APP.ws.onopen = function() {       // When the WebSocket connection opens
                $('#hello_message').text("you are connected!"); // Update connection message
                console.log('Socket opened');  // Log that the connection is open
                // Set initial parameters for both channels
                APP.setGain();
                APP.setFrequency();
                APP.setAmplitude();
                APP.setWaveform();
                APP.setGain1();
                APP.setFrequency1();
                APP.setAmplitude1();
                APP.setWaveform1();
            };

            APP.ws.onclose = function() {      // When the WebSocket connection closes
                console.log('Socket closed');  // Log that the connection is closed
            };

            APP.ws.onerror = function(ev) {    // When there’s an error with the WebSocket
                $('#hello_message').text("Connection error"); // Show error message on page
                console.log('Websocket error: ', ev); // Log error details
            };

            APP.ws.onmessage = function(ev) {  // When a message is received from the server
                console.log('Message received'); // Log that a message came in
                if (APP.processing) return;    // Skip if already processing
                APP.processing = true;         // Set flag to indicate processing

                try {
                    var data = new Uint8Array(ev.data); // Convert received data to Uint8Array
                    var inflate = pako.inflate(data);   // Decompress data using pako
                    var text = String.fromCharCode.apply(null, new Uint8Array(inflate)); // Convert to string
                    var receive = JSON.parse(text);     // Parse the JSON string

                    if (receive.parameters) {  // If parameters are received (not processed here)
                        // Placeholder for parameter handling
                    }
                    if (receive.signals) {     // If signals are received
                        APP.signalStack.push(receive.signals); // Add to signal stack
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

    // Function to set gain for Channel 1
    APP.setGain = function() {
        APP.gain = $('#gain_set').val();       // Get gain value from input
        var local = {};                        // Create object for parameter
        local['GAIN'] = { value: APP.gain };   // Add gain to object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send to server
        $('#gain_value').text(APP.gain);       // Update displayed gain value
    };

    // Function to set frequency for Channel 1
    APP.setFrequency = function() {
        APP.frequency = $('#frequency_set').val(); // Get frequency from input
        var local = {};                        // Create object for parameter
        local['FREQUENCY'] = { value: APP.frequency }; // Add frequency to object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send to server
        $('#frequency_value').text(APP.frequency); // Update displayed frequency
    };

    // Function to set amplitude for Channel 1
    APP.setAmplitude = function() {
        APP.amplitude = $('#amplitude_set').val(); // Get amplitude from input
        var local = {};                        // Create object for parameter
        local['AMPLITUDE'] = { value: APP.amplitude }; // Add amplitude to object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send to server
        $('#amplitude_value').text(APP.amplitude); // Update displayed amplitude
    };

    // Function to set waveform for Channel 1
    APP.setWaveform = function() {
        APP.waveform = $('#waveform_set').val(); // Get waveform from input
        console.log('Set to ' + APP.waveform);   // Log the selected waveform
        var local = {};                          // Create object for parameter
        local['WAVEFORM'] = { value: APP.waveform }; // Add waveform to object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send to server
    };

    // Function to set gain for Channel 2
    APP.setGain1 = function() {
        APP.gain1 = $('#gain_set1').val();     // Get gain value from input
        var local = {};                        // Create object for parameter
        local['GAIN1'] = { value: APP.gain1 }; // Add gain to object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send to server
        $('#gain_value1').text(APP.gain1);     // Update displayed gain value
    };

    // Function to set frequency for Channel 2
    APP.setFrequency1 = function() {
        APP.frequency1 = $('#frequency_set1').val(); // Get frequency from input
        var local = {};                        // Create object for parameter
        local['FREQUENCY1'] = { value: APP.frequency1 }; // Add frequency to object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send to server
        $('#frequency_value1').text(APP.frequency1); // Update displayed frequency
    };

    // Function to set amplitude for Channel 2
    APP.setAmplitude1 = function() {
        APP.amplitude1 = $('#amplitude_set1').val(); // Get amplitude from input
        var local = {};                        // Create object for parameter
        local['AMPLITUDE1'] = { value: APP.amplitude1 }; // Add amplitude to object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send to server
        $('#amplitude_value1').text(APP.amplitude1); // Update displayed amplitude
    };

    // Function to set waveform for Channel 2
    APP.setWaveform1 = function() {
        APP.waveform1 = $('#waveform_set1').val(); // Get waveform from input
        console.log('Set to ' + APP.waveform1);    // Log the selected waveform
        var local = {};                            // Create object for parameter
        local['WAVEFORM1'] = { value: APP.waveform1 }; // Add waveform to object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send to server
    };

    // Function to process new signal data and update the plot
    APP.processSignals = function(new_signals) {
        var pointArr = [];  // Array to hold plot data
        var voltage;        // Variable to store the latest voltage

        // Loop through each signal in the received data
        for (sig_name in new_signals) {
            if (new_signals[sig_name].size == 0) continue; // Skip empty signals
            var points = [];   // Array for signal points
            for (var i = 0; i < new_signals[sig_name].size; i++) {
                points.push([i, new_signals[sig_name].value[i]]); // Add x (index), y (value) pairs
            }
            pointArr.push(points); // Add points to plot array
            voltage = new_signals[sig_name].value[new_signals[sig_name].size - 1]; // Get last value
        }

        // Update the displayed voltage with 2 decimal places
        $('#value').text(parseFloat(voltage).toFixed(2) + "V");

        // Update the plot with new data
        APP.plot.setData(pointArr); // Set new data
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
            APP.signalStack.shift();           // Remove the oldest signal
    };
    setInterval(APP.signalHandler, 15);        // Run signal handler every 15ms

// Pass the APP object to the global window scope and include jQuery
}(window.APP = window.APP || {}, jQuery));

// When the page finishes loading, run this code
$(function() {
    // Function to send channel state (on/off) to the server
    APP.setChannelState = function(channel, state) {
        var local = {};                        // Create object for parameter
        local['CHANNEL_STATE_' + channel] = { value: state }; // Add channel state (e.g., CHANNEL_STATE_1)
        if (APP.ws && APP.ws.readyState === WebSocket.OPEN) { // Check if WebSocket is open
            APP.ws.send(JSON.stringify({ parameters: local })); // Send to server
        } else {
            console.log("WebSocket not connected. Cannot send channel state."); // Log error if not connected
        }
    };

    // Event handler for channel toggle switches
    $("#toggleChannel1, #toggleChannel2").on("change", function() {
        var channel = $(this).attr('id') === "toggleChannel1" ? 1 : 2; // Determine channel (1 or 2)
        var state = this.checked ? 1 : 0; // 1 for on, 0 for off
        APP.setChannelState(channel, state); // Send state to server
    });

    // Initialize the plot using Flot
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

    // Event handlers for Channel 1 inputs
    $("#gain_set").on("change input", function() {
        APP.setGain(); // Update gain when input changes
    });
    $("#frequency_set").on("change input", function() {
        APP.setFrequency(); // Update frequency when input changes
    });
    $("#amplitude_set").on("change input", function() {
        APP.setAmplitude(); // Update amplitude when input changes
    });
    $("#waveform_set").on("change", function() {
        APP.setWaveform(); // Update waveform when input changes
    });

    // Event handlers for Channel 2 inputs
    $("#gain_set1").on("change input", function() {
        APP.setGain1(); // Update gain when input changes
    });
    $("#frequency_set1").on("change input", function() {
        APP.setFrequency1(); // Update frequency when input changes
    });
    $("#amplitude_set1").on("change input", function() {
        APP.setAmplitude1(); // Update amplitude when input changes
    });
    $("#waveform_set1").on("change", function() {
        APP.setWaveform1(); // Update waveform when input changes
    });

    // Start the application when the page loads
    APP.startApp();
});