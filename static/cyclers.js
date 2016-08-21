var sinRainbow = (function() {
    var pi2 = 2 * Math.PI,
        pi2by3 = pi2 / 3;
    var rFreq = 1,
        gFreq = 1,
        bFreq = 1,
        inc = 0.01,
        html = $('html')[0],
        toHandler = null;
    var ri = 0,
        bi = 0,
        gi = 0;
    var rOffset = 0,
        gOffset = pi2by3,
        bOffset = 2 * pi2by3;
    var width = 127,
        center = 128;
    var timeoutWait = 2;
    var cycler = function() {
        var r = Math.sin(rFreq * ri + rOffset) * width + center;
        var g = Math.sin(gFreq * gi + gOffset) * width + center;
        var b = Math.sin(bFreq * bi + bOffset) * width + center;
        ri += inc;
        gi += inc;
        bi += inc;
        var rDiff = (rFreq * ri + rOffset) - pi2;
        if (rDiff >= 0) {
            ri = -(rDiff + rOffset) / rFreq;
        }
        var gDiff = (gFreq * gi + gOffset) - pi2;
        if (gDiff >= 0) {
            gi = -(gDiff + gOffset) / gFreq;
        }
        var bDiff = (bFreq * bi + bOffset) - pi2;
        if (bDiff >= 0) {
            bi = -(bDiff + bOffset) / bFreq;
        }
        html.style.backgroundColor = 'rgb(' + Math.round(r) + ',' + Math.round(g) + ',' + Math.round(b) + ')';
        toHandler = setTimeout(cycler, timeoutWait);
    };
    return {
        speed: function(speed) {
            timeoutWait = speed;
        },
        toggle: function(speed) {
            timeoutWait = speed;
            if (toHandler) {
                clearTimeout(toHandler);
                toHandler = null;
            } else {
                cycler();
            }
            return !!toHandler;
        },
        start: function(speed) {
            timeoutWait = speed;
            if (!toHandler) {
                cycler();
            }
        },
        stop: function() {
            clearTimeout(toHandler);
            toHandler = null;
        }
    };
}());

var linearGrad = (function() {
    var colors = [
            [255, 0, 0],
            [0, 255, 0],
            [255, 0, 0],
        ],
        ci = 0,
        inc = 0.004,
        transition = 0,
        func = 'linear';
    var html = $('html')[0],
        toHandler = null;
    var timeoutWait = 2;
    var cycler = function() {
        if (transition == colors.length - 1) {
            transition = 0;
        }
        var color = [],
            base = colors[transition],
            dest = colors[transition + 1];
        for (i = 0; i < 3; ++i) {
            color[i] = (base[i] * (1 - ci)) + (dest[i] * ci);
            color[i] = Math.max(0, Math.min(255, Math.round(color[i])));
        }
        ci += inc;
        if (color[0] == dest[0] && color[1] == dest[1] && color[2] == dest[2]) {
            transition++;
            ci = 0;
        }
        html.style.backgroundColor = 'rgb(' + color[0] + ',' + color[1] + ',' + color[2] + ')';
        toHandler = setTimeout(cycler, timeoutWait);
    };
    return {
        speed: function(speed) {
            timeoutWait = speed;
        },
        toggle: function(speed) {
            timeoutWait = speed;
            if (toHandler) {
                clearTimeout(toHandler);
                toHandler = null;
            } else {
                cycler();
            }
            return !!toHandler;
        },
        start: function(speed) {
            timeoutWait = speed;
            if (!toHandler) {
                cycler();
            }
        },
        stop: function() {
            clearTimeout(toHandler);
            toHandler = null;
        }
    };
}());
