// Define an immediately invoked function expression (IIFE) to create an APP object
(function(APP, $, undefined) {
    
    // Set up the main application configuration
    APP.config = {};                           // Empty object for config settings
    APP.config.app_id = 'rev3_0_4';            // Unique ID for this app version (3.0.4)
    APP.config.app_url = '/bazaar?start=' + APP.config.app_id + '?' + location.search.substr(1); // URL to start the app on the server
    APP.config.socket_url = 'ws://' + window.location.hostname + ':9002'; // WebSocket address for communication

    // Variable to hold the WebSocket connection
    APP.ws = null;

    // Object to manage the signal graph
    APP.plot = {};

    // Array to store incoming signal data
    APP.signalStack = [];

    // Initial settings for Channel 1 signal generator
    APP.frequency = 1;    // Default frequency (1 Hz)
    APP.amplitude = 0;    // Default amplitude (0 V)
    APP.waveform = 0;     // Default waveform type (0 = sine)
    APP.processing = false; // Flag to track if data is being processed
    APP.phase = 0;        // Default phase (wave starting position)
    APP.offset = 0;       // Default offset (vertical shift of wave)

    // Initial settings for Channel 2 signal generator
    APP.frequency1 = 1;   // Default frequency (1 Hz)
    APP.amplitude1 = 0;   // Default amplitude (0 V)
    APP.waveform1 = 0;    // Default waveform type (0 = sine)
    APP.processing1 = false; // Flag for Channel 2 (unused here)
    APP.phase1 = 0;       // Default phase
    APP.offset1 = 0;      // Default offset

    // Function to start the app by contacting the server
    APP.startApp = function() {
        $.get(APP.config.app_url)              // Send a request to the server
            .done(function(dresult) {          // If the request succeeds
                if (dresult.status == 'OK') {  // If server says it started
                    APP.connectWebSocket();    // Set up WebSocket connection
                } else if (dresult.status == 'ERROR') { // If server reports an error
                    console.log(dresult.reason ? dresult.reason : 'Could not start the application (ERR1)'); // Show error reason
                    APP.startApp();            // Retry starting
                } else {                       // If response is unexpected
                    console.log('Could not start the application (ERR2)'); // Log unknown error
                    APP.startApp();            // Retry starting
                }
            })
            .fail(function() {                 // If request fails completely
                console.log('Could not start the application (ERR3)'); // Log failure
                APP.startApp();                // Retry starting
            });
    };

    // Function to connect to the WebSocket server
    APP.connectWebSocket = function() {
        // Create WebSocket depending on browser support
        if (window.WebSocket) {
            APP.ws = new WebSocket(APP.config.socket_url); // Standard WebSocket
            APP.ws.binaryType = "arraybuffer"; // Expect binary data
        } else if (window.MozWebSocket) {
            APP.ws = new MozWebSocket(APP.config.socket_url); // Older Mozilla WebSocket
            APP.ws.binaryType = "arraybuffer";
        } else {
            console.log('Browser does not support WebSocket'); // No support message
        }

        // Set up WebSocket event listeners
        if (APP.ws) {
            APP.ws.onopen = function() {       // When connection opens
                $('#hello_message').text("you are connected!"); // Update UI message
                console.log('Socket opened');  // Log success
                // Send initial settings for both channels
                APP.setPhase();
                APP.setFrequency();
                APP.setAmplitude();
                APP.setWaveform();
                APP.setOffset();
                APP.setPhase1();
                APP.setFrequency1();
                APP.setAmplitude1();
                APP.setWaveform1();
                APP.setOffset1();
            };

            APP.ws.onclose = function() {      // When connection closes
                console.log('Socket closed');  // Log closure
            };

            APP.ws.onerror = function(ev) {    // When an error occurs
                $('#hello_message').text("Connection error"); // Show error on UI
                console.log('Websocket error: ', ev); // Log error details
            };

            APP.ws.onmessage = function(ev) {  // When a message arrives
                console.log('Message received'); // Log message arrival
                if (APP.processing) return;    // Skip if already busy
                APP.processing = true;         // Mark as processing

                try {
                    // Decode the binary message into text
                    var data = new Uint8Array(ev.data); // Convert to byte array
                    var inflate = pako.inflate(data);   // Decompress data
                    var text = String.fromCharCode.apply(null, new Uint8Array(inflate)); // Convert to string
                    var receive = JSON.parse(text);     // Parse JSON

                    if (receive.signals) {     // If message contains signals
                        APP.signalStack.push(receive.signals); // Add to signal stack
                    }
                    APP.processing = false;    // Done processing
                } catch (e) {
                    console.log(e);            // Log any errors
                    APP.processing = false;    // Reset on error
                } finally {
                    APP.processing = false;    // Ensure reset
                }
            };
        }
    };

    // Function to set and send phase for Channel 1
    APP.setPhase = function() {
        APP.phase = $('#phase_set').val();     // Get phase from input
        var local = {};                        // Create parameter object
        local['PHASE'] = { value: APP.phase }; // Add phase value
        APP.ws.send(JSON.stringify({ parameters: local })); // Send to server
        $('#phase_value').text(APP.phase);     // Update display
    };

    // Function to set and send offset for Channel 1
    APP.setOffset = function() {
        APP.offset = $('#offset_set').val();   // Get offset from input
        var local = {};
        local['OFFSET'] = { value: APP.offset };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#offset_value').text(APP.offset);   // Update display
    };

    // Function to set and send frequency for Channel 1
    APP.setFrequency = function() {
        APP.frequency = $('#frequency_set').val();
        var local = {};
        local['FREQUENCY'] = { value: APP.frequency };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#frequency_value').text(APP.frequency);
    };

    // Function to set and send amplitude for Channel 1
    APP.setAmplitude = function() {
        APP.amplitude = $('#amplitude_set').val();
        var local = {};
        local['AMPLITUDE'] = { value: APP.amplitude };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#amplitude_value').text(APP.amplitude);
    };

    // Function to set and send waveform for Channel 1
    APP.setWaveform = function() {
        APP.waveform = $('#waveform_set').val();
        console.log('Set to ' + APP.waveform); // Log selection
        var local = {};
        local['WAVEFORM'] = { value: APP.waveform };
        APP.ws.send(JSON.stringify({ parameters: local }));
    };

    // Functions for Channel 2 (same pattern as Channel 1)
    APP.setPhase1 = function() {
        APP.phase1 = $('#phase_set1').val();
        var local = {};
        local['PHASE1'] = { value: APP.phase1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#phase_value1').text(APP.phase1);
    };

    APP.setOffset1 = function() {
        APP.offset1 = $('#offset_set1').val();
        var local = {};
        local['OFFSET1'] = { value: APP.offset1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#offset_value1').text(APP.offset1);
    };

    APP.setFrequency1 = function() {
        APP.frequency1 = $('#frequency_set1').val();
        var local = {};
        local['FREQUENCY1'] = { value: APP.frequency1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#frequency_value1').text(APP.frequency1);
    };

    APP.setAmplitude1 = function() {
        APP.amplitude1 = $('#amplitude_set1').val();
        var local = {};
        local['AMPLITUDE1'] = { value: APP.amplitude1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#amplitude_value1').text(APP.amplitude1);
    };

    APP.setWaveform1 = function() {
        APP.waveform1 = $('#waveform_set1').val();
        console.log('Set to ' + APP.waveform1);
        var local = {};
        local['WAVEFORM1'] = { value: APP.waveform1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
    };

    // Function to process and plot signal data
    APP.processSignals = function(new_signals) {
        var pointArr = [];  // Array for graph points
        var voltage;        // Store latest voltage

        // Loop through received signals
        for (sig_name in new_signals) {
            if (new_signals[sig_name].size == 0) continue; // Skip empty signals
            var points = [];
            for (var i = 0; i < new_signals[sig_name].size; i++) {
                points.push([i, new_signals[sig_name].value[i]]); // Add x,y points
            }
            pointArr.push(points);
            voltage = new_signals[sig_name].value[new_signals[sig_name].size - 1]; // Get last value
        }

        $('#value').text(parseFloat(voltage).toFixed(2) + "V"); // Show voltage on UI
        // Update the graph
        APP.plot.setData(pointArr);
        APP.plot.resize();
        APP.plot.setupGrid();
        APP.plot.draw();
    };

    // Function to handle signals periodically
    APP.signalHandler = function() {
        if (APP.signalStack.length > 0) {      // If there are signals to process
            APP.processSignals(APP.signalStack[0]); // Process the first one
            APP.signalStack.splice(0, 1);       // Remove it from stack
        }
        if (APP.signalStack.length > 2)        // If stack gets too big
            APP.signalStack.shift();           // Remove oldest signal
    };
    setInterval(APP.signalHandler, 15);        // Run every 15ms

// Attach APP to the global window object and use jQuery
}(window.APP = window.APP || {}, jQuery));

// When the page finishes loading
$(function() {
    // Function to send channel on/off state to server
    APP.setChannelState = function(channel, state) {
        var local = {};
        local['CHANNEL_STATE_' + channel] = { value: state }; // e.g., CHANNEL_STATE_1
        if (APP.ws && APP.ws.readyState === WebSocket.OPEN) { // Check if connected
            APP.ws.send(JSON.stringify({ parameters: local })); // Send state
        } else {
            console.log("WebSocket not connected. Cannot send channel state."); // Log error
        }
    };

    // Handle toggle switch changes for channels
    $("#toggleChannel1, #toggleChannel2").on("change", function() {
        var channel = $(this).attr('id') === "toggleChannel1" ? 1 : 2; // Identify channel
        var state = this.checked ? 1 : 0; // 1 = on, 0 = off
        APP.setChannelState(channel, state); // Send state to server
    });
    
    // Initialize the graph (currently unused in UI)
    APP.plot = $.plot($("#placeholder"), [], { 
        series: { shadowSize: 0 }, // No shadows for speed
        yaxis: { min: -5, max: 5 }, // Voltage range
        xaxis: { min: 0, max: 1024, show: false } // Sample range, hidden
    });

    // Update Channel 1 settings when inputs change
    $("#phase_set").on("change input", function() {
        APP.setPhase(); // Update phase
    });

    $("#offset_set").on("change input", function() {
        APP.setOffset(); // Update offset
    });
    
    $("#frequency_set").on("change input", function() {
        APP.setFrequency(); // Update frequency
    });

    $("#amplitude_set").on("change input", function() {
        APP.setAmplitude(); // Update amplitude
    });

    $("#waveform_set").on("change", function() {
        APP.setWaveform(); // Update waveform
    });

    // Update Channel 2 settings when inputs change
    $("#phase_set1").on("change input", function() {
        APP.setPhase1();
    });

    $("#offset_set1").on("change input", function() {
        APP.setOffset1();
    });
    
    $("#frequency_set1").on("change input", function() {
        APP.setFrequency1();
    });

    $("#amplitude_set1").on("change input", function() {
        APP.setAmplitude1();
    });

    $("#waveform_set1").on("change", function() {
        APP.setWaveform1();
    });

    // Start the app when the page loads
    APP.startApp();
});