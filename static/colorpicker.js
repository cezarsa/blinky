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
    }

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
        var setSecondLevel = function(j){
            setTimeout(function() {
                $('.color-var').eq(j).trigger('click');
            }, 500);
        };
        state.activeColor = color;
        setBackgroundColor();
        for (var i = 0; i < config.baseColors.length; i++) {
            for (var j = 0; j < config.cachedVariations[i].length; j++) {
                var curColor = config.cachedVariations[i][j];
                if (curColor[0] == color[0] && curColor[1] == color[1] && curColor[2] == color[2]) {
                    $('.color').eq(i).trigger('click');
                    setSecondLevel(j);
                    return true;
                }
            }
        }
        return false;
    }

    return {
        init: init,
        setCallback: setCallback,
        setColor: setColor
    };

}());

colorPicker.init();
