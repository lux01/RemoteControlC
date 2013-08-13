// Canvas
var canvas = document.getElementById("remoteCanvas");
var ctx = canvas.getContext('2d');

// Position
var latestPos = null;
var prevPos = null;

// Server root
var server = "/"

// Mouse speed multiplier
var sensitivity = 1;

var optionsVisible = false;

// Touch start time
var startTime = null;
var numTouches = 0;
var skipEnds = 0;

// Num tap event handlers
// The function at index i will be called if a (i+1) finger tap is detected.
var numTapEvents = [
    leftClick,
    rightClick
];

function resizeCanvas() {
    canvas.width = document.documentElement.clientWidth - 20;
    canvas.height = document.documentElement.clientHeight - 70;
}

function _handleMouseMove(event) {
    ctx.clearRect(0,0,canvas.width, canvas.height);
    ctx.textBaseline = 'top';
    ctx.textAlign = 'left';
    ctx.font = 'bold 14px Arial';
    ctx.fillText("(" + Math.round(event.pageX) + ", " + Math.round(event.pageY) + ")", 0, 0);
    
    latestPos = {x: event.pageX, y: event.pageY};
    
    if(prevPos === null) {
        prevPos = latestPos;
    }
}

function _handleTouchStart(event) {
    numTouches++;
    startTime = new Date();
    startPos = {x: event.touches[0].pageX, y: event.touches[0].pageY};

    ctx.fillStyle = 'yellow';
    ctx.fillRect(0,0,1000,1000);

    if(numTouches == 1) {
        latestPos = null;
        prevPos = null;
    }
}

function _handleTouchMove(event) {
    ctx.fillStyle = 'SkyBlue';
    ctx.fillRect(0,0,1000,1000);
    ctx.fillStyle = 'black';

    if (event.targetTouches.length == 1) {
        var touch = event.targetTouches[0];
        latestPos = {x: touch.pageX, y: touch.pageY};
        
        if(prevPos === null) {
            prevPos = latestPos;
        }
    }
    
}

function _handleTouchEnd(event) {
    numTouches--;
    console.log(numTouches, skipEnds);
    if(skipEnds > 0) {
        skipEnds--;
        return;
    }

    ctx.clearRect(0,0,1000,1000);
    // Check to see if this is a tap event
    var endTime = new Date();
    var endPos = {x: event.changedTouches[0].pageX, y: event.changedTouches[0].pageY};
    var dt = endTime - startTime;
    var dx = endPos.x - startPos.x;
    var dy = endPos.y - startPos.y;
    var ds = Math.sqrt( Math.pow(dx, 2) + Math.pow(dy, 2) );
    console.log(sprintf("dt: %d, dx: %d, dy: %d, ds: %f, ctl: %d", dt, dx, dy, ds, event.changedTouches.length));

    if( dt < 100 || ds < 3) {
        console.log(sprintf("%d tap", numTouches+1));
        ctx.fillStyle = 'HotPink';
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        skipEnds = numTouches;

        if(typeof numTapEvents[numTouches] !== 'undefined') {
            numTapEvents[numTouches]();
        }
    }

    latestPos = null;
    startTime = null;
}

function sendMousePosition() {
    if(latestPos !== null && prevPos !== null && latestPos != prevPos) {
        var x = sensitivity*(latestPos.x - prevPos.x);
        var y = sensitivity*(latestPos.y - prevPos.y);
        
        console.log(sprintf("Sending change in position (%d, %d)", x, y));

        var xhReq = new XMLHttpRequest();
        xhReq.open("POST", server + "move?x="+ x + "&y=" + y, true);
        xhReq.send(null);
        prevPos = latestPos;
    }
}

function leftClick() {
    var xhReq = new XMLHttpRequest();
    xhReq.open("POST", server + "left_click", true);
    xhReq.send(null);
}

function rightClick() {
    var xhReq = new XMLHttpRequest();
    xhReq.open("POST", server + "right_click", true);
    xhReq.send(null);
}

function toggleConfig() {
    document.getElementById('mainContent').style.left = (optionsVisible) ? '0px' : '150px';
    optionsVisible = !optionsVisible;
}

function increaseSensitivity() {
    sensitivity++;
    document.getElementById('mouseSpeedInput').value = sensitivity;
    document.getElementById('decreaseSpeedButton').disabled = false;
}

function decreaseSensitivity() {
    sensitivity = Math.max(1, sensitivity - 1);
    document.getElementById('mouseSpeedInput').value = sensitivity;
    document.getElementById('decreaseSpeedButton').disabled = (sensitivity == 1);
}

// Prevent overscrolling
document.body.addEventListener('touchmove', function(event) {
    event.preventDefault();
});
document.body.addEventListener('touchstart', function(event) {
    event.preventDefault();
});

// Resize canvas on window resize
window.onresize = resizeCanvas;
resizeCanvas();

// Add config button event handlers
document.getElementById('optionsLink').addEventListener('click', toggleConfig);
document.getElementById('optionsLink').addEventListener('touchend', toggleConfig);

// Add increase/decrease speed button handlers
document.getElementById('increaseSpeedButton').addEventListener('click', increaseSensitivity);
document.getElementById('increaseSpeedButton').addEventListener('touchend', increaseSensitivity);
document.getElementById('decreaseSpeedButton').addEventListener('click', decreaseSensitivity);
document.getElementById('decreaseSpeedButton').addEventListener('touchend', decreaseSensitivity);

// Add left/right click buttons
document.getElementById('forceLeftClick').addEventListener('click', leftClick);
document.getElementById('forceLeftClick').addEventListener('touchend', leftClick);
document.getElementById('forceRightClick').addEventListener('click', rightClick);
document.getElementById('forceRightClick').addEventListener('touchend', rightClick);

// Add canvas event handlers
canvas.addEventListener('mousemove', _handleMouseMove);
canvas.addEventListener('touchmove', _handleTouchMove);

canvas.addEventListener('touchstart', _handleTouchStart);
canvas.addEventListener('touchend', _handleTouchEnd);

setInterval(sendMousePosition, 10);


