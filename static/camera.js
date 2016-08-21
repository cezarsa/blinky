var camera = (function() {
    var config = {
        el: null,
        video: null,
        canvas: null
    };
    var onColorCallback = null;
    var body = $('body');

    navigator.getUserMedia = (navigator.getUserMedia ||
        navigator.webkitGetUserMedia ||
        navigator.mozGetUserMedia);
    window.webkitURL = (window.URL || window.webkitURL);

    var init = function(sel) {
        config.el = $(sel);
        bindEvents();
    };

    var bindEvents = function() {
        body.on('click', '.camera', enableCamera);
    };

    var enableCamera = function() {
        if (!config.video) {
            config.video = $('<video autoplay></video>');
            config.canvas = $('<canvas id="canvas" width=200 height=200></canvas>')[0];
            config.ctx = config.canvas.getContext("2d");
            config.el.append(config.video);
            config.el.append(config.canvas);
        }
        if (config.playing) {
            config.playing = false;
            config.video[0].pause();
            config.video[0].src = "";
            config.video.css('display', 'none');
            if (config.timeout) {
                clearTimeout(config.timeout);
            }
            return;
        }
        var videoSpec = {
            video: {
                facingMode: "environment"
            }
        };
        navigator.getUserMedia(videoSpec, function(mediaStream) {
            config.video.css('display', 'block');
            config.video[0].src = window.webkitURL.createObjectURL(mediaStream);
            config.video[0].play();
            config.playing = true;
            config.timeout = setTimeout(snap, 1000);
        }, function(error) {
            console.log("error getting media: " + error);
        });
    };

    var snap = function() {
        var video = config.video[0];
        var ctx = config.ctx;
        var sz = video.videoWidth / 4;
        config.canvas.width = config.canvas.height = sz;
        ctx.drawImage(video, (video.videoWidth / 2) - (sz / 2), (video.videoHeight / 2) - (sz / 2), sz, sz, 0, 0, config.canvas.width, config.canvas.height);
        var data = ctx.getImageData(0, 0, config.canvas.width, config.canvas.height),
            dataLen = data.data.length,
            r = 0,
            g = 0,
            b = 0,
            count = 0;

        for (var i = 0; i < dataLen; i += 4) {
            r += data.data[i];
            g += data.data[i + 1];
            b += data.data[i + 2];
            count++;
        }
        r = ~~(r / count);
        g = ~~(g / count);
        b = ~~(b / count);
        if (onColorCallback) {
            onColorCallback([r, g, b]);
        }
        body.css({
            'background-color': 'rgb(' + [r, g, b] + ')'
        });
        config.timeout = setTimeout(snap, 200);
    };

    var setCallback = function(cb) {
        onColorCallback = cb;
    };

    return {
        init: init,
        setCallback: setCallback
    };
}());

camera.init('.camera-area');
