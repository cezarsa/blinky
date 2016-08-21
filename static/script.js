var camera = (function() {
	var config = {
		el: null,
		video: null,
		canvas: null
	};
	var onColorCallback = null;

	navigator.getUserMedia = (navigator.getUserMedia ||
      navigator.webkitGetUserMedia ||
      navigator.mozGetUserMedia);
	window.webkitURL = (window.URL || window.webkitURL);

	var init = function(sel) {
		config.el = $(sel);
		bindEvents();
	};

	var bindEvents = function() {
		$('body').on('click', '.camera', enableCamera);
	};

	var enableCamera = function() {
		if (!config.video) {
			config.video = $('<video autoplay></video>')
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
		var videoSpec = {video: { facingMode: "environment" }};
		navigator.getUserMedia(videoSpec, function(mediaStream) {
			config.video.css('display', 'block');
			config.video[0].src = window.webkitURL.createObjectURL(mediaStream);
			config.video[0].play();
			config.playing = true;
			config.timeout = setTimeout(snap, 1000);
		}, function(error) {
			console.log("error getting media: " + error)
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
				r = 0, g = 0, b = 0, count = 0;

		for (var i = 0; i < dataLen; i += 4) {
			r += data.data[i];
			g += data.data[i+1];
			b += data.data[i+2];
			count++;
		}
    r = ~~(r / count);
    g = ~~(g / count);
    b = ~~(b / count);
		if (onColorCallback) {
			onColorCallback([r, g, b])
		}
		config.timeout = setTimeout(snap, 200);
	};

	var setCallback = function(cb) {
		onColorCallback = cb;
	}

	return {
		init: init,
		setCallback: setCallback
	};
}());

camera.init('.camera-area');

var colorPicker = (function() {

    var config = {
        baseColors: [
            [46, 204, 113],
            [52, 152, 219],
            [155, 89, 182],
            [52, 73, 94],
            [241, 196, 15],
            [230, 126, 34],
            [231, 76, 60],
            [180, 180, 180]
        ],
        lightModifier: 20,
        darkModifier: 0,
        transitionDuration: 200,
        transitionDelay: 25,
        variationTotal: 10,
        cachedVariations: []
    };

    var state = {
        activeColor: [0, 0, 0]
    };

    var onColorCallback = null;

    function init() {
        createColorPicker(function() {
            appendBaseColors();
        });
        cacheVariations();

        addEventListeners();
    }

    function cacheVariations() {
        for (var i = 0; i < config.baseColors.length; i++) {
            var color = config.baseColors[i];
            var variations = [];
            for (var j = 0; j < config.variationTotal; j++) {
                var newColor = [];
                for (var x = 0; x < color.length; x++) {
                    var modifiedColor = (Number(color[x]) - 100) + (config.lightModifier * j);

                    if (modifiedColor <= 0) {
                        modifiedColor = 0;
                    } else if (modifiedColor >= 255) {
                        modifiedColor = 255;
                    }

                    newColor.push(modifiedColor);
                }
                variations.push(newColor);
            }
            config.cachedVariations[i] = variations;
        }
    }

    function setActiveBaseColor(el) {
        $('.color.active').removeClass('active');
        el.addClass('active');
    }

    function setActiveColor(el) {
        $('.color-var.active').removeClass('active');
        el.addClass('active');
        state.activeColor = el.data('color').split(',');
        if (onColorCallback) {
            onColorCallback(state.activeColor);
        }
    }

    function addEventListeners() {
        $('body').on('click', '.color', function() {
            var colorIdx = $(this).index();
            setActiveBaseColor($(this));

            hideVariations(function() {
                createVariations(colorIdx, function() {
                    setDelays(function() {
                        showVariations();
                    });
                });
            });
        });

        $('body').on('click', '.color-var', function() {
            setActiveColor($(this));
            setBackgroundColor();
        });
    }

    function createColorPicker(callback) {
        $('.color-picker').append('<div class="base-colors"></div>');
        $('.color-picker').append('<div class="varied-colors"></div>');

        callback();
    }

    function appendBaseColors() {
        for (i = 0; i < config.baseColors.length; i++) {
            $('.base-colors').append('<div class="color" data-color="' + config.baseColors[i].join() + '" style="background-color: rgb(' + config.baseColors[i].join() + ');"></div>');
        }
    };

    function setBackgroundColor() {
        $('body').css({
            'background-color': 'rgb(' + state.activeColor + ')'
        });
    }

    function createVariations(colorIdx, callback) {
        $('.varied-colors').html('');
        var variations = config.cachedVariations[colorIdx];

        for (var i = 0; i < variations.length; i++) {
            var newColor = variations[i];
            var extraClass = "";
            if (newColor[0] > 220 && newColor[1] > 220 && newColor[2] > 220) {
                extraClass = "light";
            }
            $('.varied-colors').append('<div data-color="' + newColor + '" class="color-var ' + extraClass + '" style="background-color: rgb(' + newColor + ');"></div>');
        }

        callback();
    }

    function setDelays(callback) {
        $('.color-var').each(function(x) {
            $(this).css({
                'transition': 'transform ' + (config.transitionDuration / 1000) + 's ' + ((config.transitionDelay / 1000) * x) + 's'
            });
        });

        callback();
    }

    function showVariations() {
        setTimeout(function() {
            $('.color-var').addClass('visible');
        }, (config.transitionDelay * config.variationTotal));
    }

    function hideVariations(callback) {
        $('.color-var').removeClass('visible').removeClass('active');

        setTimeout(function() {
            callback();
        }, (config.transitionDelay * config.variationTotal));
    }

    function setCallback(cb) {
        onColorCallback = cb;
    }

    function setColor(color) {
				for (var i = 0; i < config.baseColors.length; i++) {
					for (var j = 0; j < config.cachedVariations[i].length; j++) {
						var curColor = config.cachedVariations[i][j];
						if (curColor[0] == color[0] && curColor[1] == color[1] && curColor[2] == color[2]) {
							$('.color').eq(i).trigger('click');
							setTimeout(function() {
								$('.color-var').eq(j).trigger('click');
							}, 500);
							return;
						}
					}
				}
    }

    return {
        init: init,
        setCallback: setCallback,
        setColor: setColor
    };

}());

colorPicker.init();
