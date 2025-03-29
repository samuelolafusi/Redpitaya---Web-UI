(function(APP, $, undefined) {
    
    // App configuration
    APP.config = {};
    APP.config.app_id = 'rev3_0_5v2';
    APP.config.app_url = '/bazaar?start=' + APP.config.app_id + '?' + location.search.substr(1);
    APP.config.socket_url = 'ws://' + window.location.hostname + ':9002';

    // WebSocket
    APP.ws = null;

    // Plot
    APP.plot = {};

    // Signal stack
    APP.signalStack = [];

    // Parameter functions with initial values

    APP.gain = 1;

    APP.frequency = 1;
    APP.amplitude = 0;
    APP.waveform = 0;
    APP.phase = 0;
    APP.offset = 0;
    APP.enableCH1 = true;

    APP.frequency1 = 1;
    APP.amplitude1 = 0;
    APP.waveform1 = 0;
    APP.phase1 = 0;
    APP.offset1 = 0;
    APP.enableCH2 = true;

    APP.plotBufferCH1 = [];
    APP.plotBufferCH2 = [];
    APP.processing = false;



    // Starts template application on server
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
        APP.ws.onopen = function() {
            $('#hello_message').text("you are connected!");
            console.log('Socket opened');
            APP.setGain();
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

            for (var i = 0; i < 1024; i++) {
                APP.plotBufferCH1.push(parseFloat(APP.offset));
                APP.plotBufferCH2.push(parseFloat(APP.offset1));
            }
            APP.updatePlot();
        };

        APP.ws.onclose = function() { console.log('Socket closed'); };
        APP.ws.onerror = function(ev) {
            $('#hello_message').text("Connection error");
            console.log('Websocket error: ', ev);
        };

        APP.ws.onmessage = function(ev) {
            if (APP.processing) return;
            APP.processing = true;
            try {
                var receive = JSON.parse(pako.inflate(new Uint8Array(ev.data), { to: 'string' }));
                if (receive.signals) {
                    APP.signalStack.push(receive.signals);
                }
            } catch (e) {
                console.log("Error:", e);
            }
            APP.processing = false;
        };
    };


    //Set gain
    APP.setGain = function() {

        APP.gain = $('#gain_set').val();

        var local = {};
        local['GAIN'] = { value: APP.gain };
        APP.ws.send(JSON.stringify({ parameters: local }));

        $('#gain_value').text(APP.gain);

    };

    //Set Phase and send value to Cpp, note the parameter name in BLOCK letters
    APP.setPhase = function() {

        APP.phase = $('#phase_set').val();

        var local = {};
        local['PHASE'] = { value: APP.phase };
        APP.ws.send(JSON.stringify({ parameters: local }));

        $('#phase_value').text(APP.phase);

    };

    //Set offset and send value to Cpp, note the parameter name in BLOCK letters
    APP.setOffset = function() {

        APP.offset = $('#offset_set').val();

        var local = {};
        local['OFFSET'] = { value: APP.offset };
        APP.ws.send(JSON.stringify({ parameters: local }));

        $('#offset_value').text(APP.offset);

    };

    // Set frequency and send value to Cpp, note the parameter name in BLOCK letters
    APP.setFrequency = function() {

        APP.frequency = $('#frequency_set').val();

        var local = {};
        local['FREQUENCY'] = { value: APP.frequency };
        APP.ws.send(JSON.stringify({ parameters: local }));

        $('#frequency_value').text(APP.frequency);

    };

    // Set amplitude and send value to Cpp, note the parameter name in BLOCK letters
    APP.setAmplitude = function() {

        APP.amplitude = $('#amplitude_set').val();

        var local = {};
        local['AMPLITUDE'] = { value: APP.amplitude };
        APP.ws.send(JSON.stringify({ parameters: local }));

        $('#amplitude_value').text(APP.amplitude);

    };

    // Set waveform and send value to Cpp, note the parameter name in BLOCK letters
    APP.setWaveform = function() {

        APP.waveform = $('#waveform_set').val();

        console.log('Set to ' + APP.waveform);

        var local = {};
        local['WAVEFORM'] = { value: APP.waveform };
        APP.ws.send(JSON.stringify({ parameters: local }));
    };

    //Set Phase1 and send value to Cpp, note the parameter name in BLOCK letters
    APP.setPhase1 = function() {

        APP.phase1 = $('#phase_set1').val();

        var local = {};
        local['PHASE1'] = { value: APP.phase1 };
        APP.ws.send(JSON.stringify({ parameters: local }));

        $('#phase_value1').text(APP.phase1);

    };

    //Set offset1 and send value to Cpp, note the parameter name in BLOCK letters
    APP.setOffset1 = function() {

        APP.offset1 = $('#offset_set1').val();

        var local = {};
        local['OFFSET1'] = { value: APP.offset1 };
        APP.ws.send(JSON.stringify({ parameters: local }));

        $('#offset_value1').text(APP.offset1);

    };

    // Set frequency1 and send value to Cpp, note the parameter name in BLOCK letters
    APP.setFrequency1 = function() {

        APP.frequency1 = $('#frequency_set1').val();

        var local = {};
        local['FREQUENCY1'] = { value: APP.frequency1 };
        APP.ws.send(JSON.stringify({ parameters: local }));

        $('#frequency_value1').text(APP.frequency1);

    };

    // Set amplitude1 and send value to Cpp, note the parameter name in BLOCK letters
    APP.setAmplitude1 = function() {

        APP.amplitude1 = $('#amplitude_set1').val();

        var local = {};
        local['AMPLITUDE1'] = { value: APP.amplitude1 };
        APP.ws.send(JSON.stringify({ parameters: local }));

        $('#amplitude_value1').text(APP.amplitude1);

    };

    // Set waveform1 and send value to Cpp, note the parameter name in BLOCK letters
    APP.setWaveform1 = function() {

        APP.waveform1= $('#waveform_set1').val();

        console.log('Set to ' + APP.waveform1);

        var local = {};
        local['WAVEFORM1'] = { value: APP.waveform1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
    };    

    APP.updatePlot = function() {
        var pointArr = [];
        var timeWindow = 1.0; // 1 sec
        if (APP.enableCH1) {
            var pointsCH1 = [];
            for (var i = 0; i < APP.plotBufferCH1.length; i++) {
                var t = i / 1000.0; // 0 to 1 sec
                pointsCH1.push([t, APP.plotBufferCH1[i]]);
            }
            pointArr.push(pointsCH1);
        }
        if (APP.enableCH2) {
            var pointsCH2 = [];
            for (var i = 0; i < APP.plotBufferCH2.length; i++) {
                var t = i / 1000.0;
                pointsCH2.push([t, APP.plotBufferCH2[i]]);
            }
            pointArr.push(pointsCH2);
        }
        APP.plot.setData(pointArr);
        APP.plot.setupGrid();
        APP.plot.draw();
    };


    // Processes newly received data for signals
    APP.processSignals = function(new_signals) {
        for (var sig_name in new_signals) {
            if (new_signals[sig_name].size == 0) continue;
            var newData = new_signals[sig_name].value;
            if (sig_name === "VOLTAGE_CH1" && APP.enableCH1) {
                APP.plotBufferCH1 = APP.plotBufferCH1.slice(newData.length).concat(newData);
            } else if (sig_name === "VOLTAGE_CH2" && APP.enableCH2) {
                APP.plotBufferCH2 = APP.plotBufferCH2.slice(newData.length).concat(newData);
            }
        }
        APP.updatePlot();
    };




    //Handler
    APP.signalHandler = function() {
        if (APP.signalStack.length > 0) {
            APP.processSignals(APP.signalStack[0]);
            APP.signalStack.splice(0, 1);
        }
        if (APP.signalStack.length > 2)
            // APP.signalStack.shift(); //////////////////////////////
            APP.signalStack = []; // Fixed
    }
    setInterval(APP.signalHandler, 10);


}(window.APP = window.APP || {}, jQuery));




// Page onload event handler, this receives the event when a switch is toggled, 
// it sends the value through the paramter (in BLOCK letters) to CPP
// the value when toggled off is 0, vale whne toggled on is 1
$(function() {
    
    APP.setChannelState = function(channel, state) {
        var local = {};
        local['CHANNEL_STATE_' + channel] = { value: state };
    
        if (APP.ws && APP.ws.readyState === WebSocket.OPEN) {
            APP.ws.send(JSON.stringify({ parameters: local }));
        } else {
            console.log("WebSocket not connected. Cannot send channel state.");
        }
    };

    $("#toggleChannel1").on("change", function() {
        APP.enableCH1 = this.checked;
        APP.setChannelState(1, APP.enableCH1 ? 1 : 0);
        APP.updatePlot();
    });

    $("#toggleChannel2").on("change", function() {
        APP.enableCH2 = this.checked;
        APP.setChannelState(2, APP.enableCH2 ? 1 : 0);
        APP.updatePlot();
    });
    
    
    //Init plot, plotting the waveform, not in use at the moment
    APP.plot = $.plot($("#placeholder"), [], { 
        series: { shadowSize: 0 },
        yaxis: { min: -5, max: 5, show: true },
        xaxis: { min: 0, max: 1, show: true, tickFormatter: function(v) { return v.toFixed(2) + "s"; } }
    });


    // this part updates the parameters when the user inputs a new value 

    // Input change
    $("#gain_set").on("change input", function() {

        APP.setGain(); 
    });

    // Input change
    $("#phase_set").on("change input", function() {

        APP.setPhase(); 
    });

    // Input change
    $("#offset_set").on("change input", function() {

        APP.setOffset(); 
    });
    
    // Input change
    $("#frequency_set").on("change input", function() {

        APP.setFrequency(); 
    });

    $("#amplitude_set").on("change input", function() {

        APP.setAmplitude(); 
    });

    $("#waveform_set").on("change", function() {

        APP.setWaveform(); 
    });


    $("#phase_set1").on("change input", function() {

        APP.setPhase1(); 
    });

    // Input change
    $("#offset_set1").on("change input", function() {

        APP.setOffset1(); 
    });
    
    // Input change
    $("#frequency_set1").on("change input", function() {

        APP.setFrequency1(); 
    });

    $("#amplitude_set1").on("change input", function() {

        APP.setAmplitude1(); 
    });

    $("#waveform_set1").on("change", function() {

        APP.setWaveform1(); 
    });


    // Start application
    APP.startApp();
});
