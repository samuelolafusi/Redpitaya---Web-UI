// Define an immediately invoked function expression (IIFE) to create an APP object
(function(APP, $, undefined) {
    
    // App configuration
    APP.config = {};
    APP.config.app_id = 'rev3_0_5v2'; // Unique ID for this app version
    APP.config.app_url = '/bazaar?start=' + APP.config.app_id + '?' + location.search.substr(1); // URL to start the app on the server
    APP.config.socket_url = 'ws://' + window.location.hostname + ':9002'; // WebSocket connection address

    // WebSocket
    APP.ws = null; // Variable to hold WebSocket connection

    // Plot
    APP.plot = {}; // Object to manage the signal graph

    // Signal stack
    APP.signalStack = []; // Array to store incoming signal data

    // Parameter functions with initial values
    APP.gain = 1; // Default gain value (starts at 1)

    APP.frequency = 1; // Default frequency for Channel 1 (1 Hz)
    APP.amplitude = 0; // Default amplitude for Channel 1 (0 V)
    APP.waveform = 0;  // Default waveform for Channel 1 (0 = sine)
    APP.phase = 0;     // Default phase for Channel 1 (0 degrees)
    APP.offset = 0;    // Default offset for Channel 1 (0 V)
    APP.enableCH1 = true; // Channel 1 starts enabled

    APP.frequency1 = 1; // Default frequency for Channel 2 (1 Hz)
    APP.amplitude1 = 0; // Default amplitude for Channel 2 (0 V)
    APP.waveform1 = 0;  // Default waveform for Channel 2 (0 = sine)
    APP.phase1 = 0;     // Default phase for Channel 2 (0 degrees)
    APP.offset1 = 0;    // Default offset for Channel 2 (0 V)
    APP.enableCH2 = true; // Channel 2 starts enabled

    APP.plotBufferCH1 = []; // Buffer to store Channel 1 plot data
    APP.plotBufferCH2 = []; // Buffer to store Channel 2 plot data
    APP.processing = false; // Flag to track if data is being processed

    // Starts template application on server
    APP.startApp = function() { // Start the app by contacting the server
        $.get(APP.config.app_url) // Send request to start app on server
            .done(function(dresult) { // If request succeeds
                if (dresult.status == 'OK') {
                    APP.connectWebSocket(); // Connect to WebSocket if successful
                } else if (dresult.status == 'ERROR') {
                    console.log(dresult.reason ? dresult.reason : 'Could not start the application (ERR1)'); // Log error reason
                    APP.startApp(); // Retry starting
                } else {
                    console.log('Could not start the application (ERR2)'); // Log unknown error
                    APP.startApp(); // Retry starting
                }
            })
            .fail(function() { // If request fails
                console.log('Could not start the application (ERR3)'); // Log failure
                APP.startApp(); // Retry starting
            });
    };

    APP.connectWebSocket = function() { // Set up WebSocket connection
        //Create WebSocket
        if (window.WebSocket) { // Check for standard WebSocket support
            APP.ws = new WebSocket(APP.config.socket_url); // Create WebSocket
            APP.ws.binaryType = "arraybuffer"; // Set to receive binary data
        } else if (window.MozWebSocket) { // Check for older Mozilla WebSocket
            APP.ws = new MozWebSocket(APP.config.socket_url); // Create Mozilla WebSocket
            APP.ws.binaryType = "arraybuffer";
        } else {
            console.log('Browser does not support WebSocket'); // Log if unsupported
        }

        // Define WebSocket event listeners
        APP.ws.onopen = function() { // When WebSocket connects
            $('#hello_message').text("you are connected!"); // Update UI message
            console.log('Socket opened'); // Log success
            APP.setGain(); // Send initial gain
            APP.setPhase(); // Send initial phase for Channel 1
            APP.setFrequency(); // Send initial frequency for Channel 1
            APP.setAmplitude(); // Send initial amplitude for Channel 1
            APP.setWaveform(); // Send initial waveform for Channel 1
            APP.setOffset(); // Send initial offset for Channel 1
            APP.setPhase1(); // Send initial phase for Channel 2
            APP.setFrequency1(); // Send initial frequency for Channel 2
            APP.setAmplitude1(); // Send initial amplitude for Channel 2
            APP.setWaveform1(); // Send initial waveform for Channel 2
            APP.setOffset1(); // Send initial offset for Channel 2

            for (var i = 0; i < 1024; i++) { // Fill plot buffers with initial offset values
                APP.plotBufferCH1.push(parseFloat(APP.offset)); // Buffer for Channel 1
                APP.plotBufferCH2.push(parseFloat(APP.offset1)); // Buffer for Channel 2
            }
            APP.updatePlot(); // Draw the initial plot
        };

        APP.ws.onclose = function() { console.log('Socket closed'); }; // Log when connection closes
        APP.ws.onerror = function(ev) { // Handle WebSocket errors
            $('#hello_message').text("Connection error"); // Show error on UI
            console.log('Websocket error: ', ev); // Log error details
        };

        APP.ws.onmessage = function(ev) { // Handle incoming messages
            if (APP.processing) return; // Skip if already processing
            APP.processing = true; // Mark as processing
            try {
                var receive = JSON.parse(pako.inflate(new Uint8Array(ev.data), { to: 'string' })); // Decode binary message to text
                if (receive.signals) { // If message has signal data
                    APP.signalStack.push(receive.signals); // Add to signal stack
                }
            } catch (e) {
                console.log("Error:", e); // Log any errors
            }
            APP.processing = false; // Done processing
        };
    };

    //Set gain
    APP.setGain = function() { // Update and send gain value
        APP.gain = $('#gain_set').val(); // Get gain from input
        var local = {};
        local['GAIN'] = { value: APP.gain }; // Prepare parameter object
        APP.ws.send(JSON.stringify({ parameters: local })); // Send to server
        $('#gain_value').text(APP.gain); // Update UI display
    };

    //Set Phase and send value to Cpp, note the parameter name in BLOCK letters
    APP.setPhase = function() { // Update and send phase for Channel 1
        APP.phase = $('#phase_set').val();
        var local = {};
        local['PHASE'] = { value: APP.phase };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#phase_value').text(APP.phase);
    };

    //Set offset and send value to Cpp, note the parameter name in BLOCK letters
    APP.setOffset = function() { // Update and send offset for Channel 1
        APP.offset = $('#offset_set').val();
        var local = {};
        local['OFFSET'] = { value: APP.offset };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#offset_value').text(APP.offset);
    };

    // Set frequency and send value to Cpp, note the parameter name in BLOCK letters
    APP.setFrequency = function() { // Update and send frequency for Channel 1
        APP.frequency = $('#frequency_set').val();
        var local = {};
        local['FREQUENCY'] = { value: APP.frequency };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#frequency_value').text(APP.frequency);
    };

    // Set amplitude and send value to Cpp, note the parameter name in BLOCK letters
    APP.setAmplitude = function() { // Update and send amplitude for Channel 1
        APP.amplitude = $('#amplitude_set').val();
        var local = {};
        local['AMPLITUDE'] = { value: APP.amplitude };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#amplitude_value').text(APP.amplitude);
    };

    // Set waveform and send value to Cpp, note the parameter name in BLOCK letters
    APP.setWaveform = function() { // Update and send waveform for Channel 1
        APP.waveform = $('#waveform_set').val();
        console.log('Set to ' + APP.waveform); // Log the selection
        var local = {};
        local['WAVEFORM'] = { value: APP.waveform };
        APP.ws.send(JSON.stringify({ parameters: local }));
    };

    //Set Phase1 and send value to Cpp, note the parameter name in BLOCK letters
    APP.setPhase1 = function() { // Update and send phase for Channel 2
        APP.phase1 = $('#phase_set1').val();
        var local = {};
        local['PHASE1'] = { value: APP.phase1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#phase_value1').text(APP.phase1);
    };

    //Set offset1 and send value to Cpp, note the parameter name in BLOCK letters
    APP.setOffset1 = function() { // Update and send offset for Channel 2
        APP.offset1 = $('#offset_set1').val();
        var local = {};
        local['OFFSET1'] = { value: APP.offset1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#offset_value1').text(APP.offset1);
    };

    // Set frequency1 and send value to Cpp, note the parameter name in BLOCK letters
    APP.setFrequency1 = function() { // Update and send frequency for Channel 2
        APP.frequency1 = $('#frequency_set1').val();
        var local = {};
        local['FREQUENCY1'] = { value: APP.frequency1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#frequency_value1').text(APP.frequency1);
    };

    // Set amplitude1 and send value to Cpp, note the parameter name in BLOCK letters
    APP.setAmplitude1 = function() { // Update and send amplitude for Channel 2
        APP.amplitude1 = $('#amplitude_set1').val();
        var local = {};
        local['AMPLITUDE1'] = { value: APP.amplitude1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#amplitude_value1').text(APP.amplitude1);
    };

    // Set waveform1 and send value to Cpp, note the parameter name in BLOCK letters
    APP.setWaveform1 = function() { // Update and send waveform for Channel 2
        APP.waveform1 = $('#waveform_set1').val();
        console.log('Set to ' + APP.waveform1);
        var local = {};
        local['WAVEFORM1'] = { value: APP.waveform1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
    };    

    APP.updatePlot = function() { // Update the graph with current data
        var pointArr = []; // Array to hold plot data
        var timeWindow = 1.0; // Show 1 second of data
        if (APP.enableCH1) { // If Channel 1 is enabled
            var pointsCH1 = [];
            for (var i = 0; i < APP.plotBufferCH1.length; i++) {
                var t = i / 1000.0; // Convert index to seconds (0 to 1)
                pointsCH1.push([t, APP.plotBufferCH1[i]]); // Add time and value
            }
            pointArr.push(pointsCH1); // Add Channel 1 data to plot
        }
        if (APP.enableCH2) { // If Channel 2 is enabled
            var pointsCH2 = [];
            for (var i = 0; i < APP.plotBufferCH2.length; i++) {
                var t = i / 1000.0;
                pointsCH2.push([t, APP.plotBufferCH2[i]]);
            }
            pointArr.push(pointsCH2); // Add Channel 2 data to plot
        }
        APP.plot.setData(pointArr); // Update plot with new data
        APP.plot.setupGrid(); // Refresh the grid
        APP.plot.draw(); // Draw the updated plot
    };

    // Processes newly received data for signals
    APP.processSignals = function(new_signals) { // Handle new signal data
        for (var sig_name in new_signals) { // Loop through signals
            if (new_signals[sig_name].size == 0) continue; // Skip empty signals
            var newData = new_signals[sig_name].value; // Get signal values
            if (sig_name === "VOLTAGE_CH1" && APP.enableCH1) { // If Channel 1 data
                APP.plotBufferCH1 = APP.plotBufferCH1.slice(newData.length).concat(newData); // Update buffer
            } else if (sig_name === "VOLTAGE_CH2" && APP.enableCH2) { // If Channel 2 data
                APP.plotBufferCH2 = APP.plotBufferCH2.slice(newData.length).concat(newData);
            }
        }
        APP.updatePlot(); // Refresh the plot with new data
    };

    //Handler
    APP.signalHandler = function() { // Regularly check for new signals
        if (APP.signalStack.length > 0) { // If there are signals to process
            APP.processSignals(APP.signalStack[0]); // Process the first one
            APP.signalStack.splice(0, 1); // Remove it from the stack
        }
        if (APP.signalStack.length > 2) // If stack grows too large
            // APP.signalStack.shift(); ////////////////////////////// // Original comment preserved
            APP.signalStack = []; // Fixed: Clear stack instead of shifting
    }
    setInterval(APP.signalHandler, 10); // Run every 10 milliseconds

// Attach APP to the global window object with jQuery
}(window.APP = window.APP || {}, jQuery));

// Page onload event handler, this receives the event when a switch is toggled, 
// it sends the value through the paramter (in BLOCK letters) to CPP
// the value when toggled off is 0, vale whne toggled on is 1
$(function() { // When the page loads
    APP.setChannelState = function(channel, state) { // Send channel on/off state
        var local = {};
        local['CHANNEL_STATE_' + channel] = { value: state }; // e.g., CHANNEL_STATE_1
        if (APP.ws && APP.ws.readyState === WebSocket.OPEN) { // If WebSocket is connected
            APP.ws.send(JSON.stringify({ parameters: local })); // Send state to server
        } else {
            console.log("WebSocket not connected. Cannot send channel state."); // Log error
        }
    };

    $("#toggleChannel1").on("change", function() { // When Channel 1 toggle changes
        APP.enableCH1 = this.checked; // Update enable flag
        APP.setChannelState(1, APP.enableCH1 ? 1 : 0); // Send 1 (on) or 0 (off)
        APP.updatePlot(); // Refresh the plot
    });

    $("#toggleChannel2").on("change", function() { // When Channel 2 toggle changes
        APP.enableCH2 = this.checked;
        APP.setChannelState(2, APP.enableCH2 ? 1 : 0);
        APP.updatePlot();
    });
    
    //Init plot, plotting the waveform, not in use at the moment
    APP.plot = $.plot($("#placeholder"), [], { // Set up the graph
        series: { shadowSize: 0 }, // No shadows for faster drawing
        yaxis: { min: -5, max: 5, show: true }, // Y-axis range (-5V to 5V)
        xaxis: { min: 0, max: 1, show: true, tickFormatter: function(v) { return v.toFixed(2) + "s"; } } // X-axis (0-1s) with seconds label
    });

    // this part updates the parameters when the user inputs a new value 
    $("#gain_set").on("change input", function() { // Update gain on change
        APP.setGain(); 
    });

    $("#phase_set").on("change input", function() { // Update phase for Channel 1
        APP.setPhase(); 
    });

    $("#offset_set").on("change input", function() { // Update offset for Channel 1
        APP.setOffset(); 
    });
    
    $("#frequency_set").on("change input", function() { // Update frequency for Channel 1
        APP.setFrequency(); 
    });

    $("#amplitude_set").on("change input", function() { // Update amplitude for Channel 1
        APP.setAmplitude(); 
    });

    $("#waveform_set").on("change", function() { // Update waveform for Channel 1
        APP.setWaveform(); 
    });

    $("#phase_set1").on("change input", function() { // Update phase for Channel 2
        APP.setPhase1(); 
    });

    $("#offset_set1").on("change input", function() { // Update offset for Channel 2
        APP.setOffset1(); 
    });
    
    $("#frequency_set1").on("change input", function() { // Update frequency for Channel 2
        APP.setFrequency1(); 
    });

    $("#amplitude_set1").on("change input", function() { // Update amplitude for Channel 2
        APP.setAmplitude1(); 
    });

    $("#waveform_set1").on("change", function() { // Update waveform for Channel 2
        APP.setWaveform1(); 
    });

    // Start application
    APP.startApp(); // Launch the app when page loads
});