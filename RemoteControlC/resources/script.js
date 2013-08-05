var canvas = document.getElementById("remoteCanvas");
var ctx = canvas.getContext('2d');

var latestPos = null;
var prevPos = null;
var touchMax = 0;

var server = "/"

var sensitivity = 1;

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

function _handleTouchMove(event) {
    if (event.targetTouches.length == 1) {
        var touch = event.targetTouches[0];
        ctx.clearRect(0,0,canvas.width, canvas.height);
        ctx.textBaseline = 'top';
        ctx.textAlign = 'left';
        ctx.font = 'bold 14px Arial';
        ctx.fillText("(" + Math.round(touch.pageX) + ", " + Math.round(touch.pageY) + ")", 0, 0);
        
        // Attempt to remove some jitter from the movement.
        //if( Math.abs(prevPos.pageX - latestPos.x) <= 3 && Math.abs(prevPos.pageY - latestPos.y) <= 3)
        //    return;
        
        latestPos = {x: touch.pageX, y: touch.pageY};
        
        if(prevPos === null) {
            prevPos = latestPos;
        }
    }
    
}

function _handleTouchEnd(event) {
	if(event.targetTouches.length > 1) return;
	if(touchMax == 2) leftClick(); 
	if(touchMax == 3) rightClick(); 
    if(touchMax >= 4) changeSensitivity(); 
    prevPos = null;
    latestPos = null;
	touchMax = event.targetTouches.length;
}

function sendMousePosition() {
    if(latestPos !== null && prevPos !== null && latestPos != prevPos) {
        console.log("Sending mouse position", latestPos);
        
        var x = sensitivity*(latestPos.x - prevPos.x);
        var y = sensitivity*(latestPos.y - prevPos.y);
        
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

function changeSensitivity() {
    var sense = prompt("Mouse speed:", sensitivity);
    if(isNaN(sense)) {
        alert("Mouse speed must be a number.");
    } else if( sense <= 0) {
        alert("Mouse speed must be a non-negative number.");
    } else if(!isFinite(sense)) {
        alert("Mouse speed must be finite.");
    } else if(sense == null) {
        return;
    } else {
        sensitivity = sense;
        return;
    }
    changeSensitivity();
}

// Prevent overscrolling
document.body.addEventListener('touchmove', function(event) {
    event.preventDefault();
});
document.body.addEventListener('touchstart', function(event) {
    event.preventDefault();
});

window.onresize = resizeCanvas;
resizeCanvas();

canvas.addEventListener('mousemove', _handleMouseMove);
canvas.addEventListener('touchmove', _handleTouchMove);
canvas.addEventListener('touchend', _handleTouchEnd);
canvas.addEventListener('contextmenu', changeSensitivity);
canvas.addEventListener('touchstart', function(event) {
   if(event.targetTouches.length > touchMax) touchMax = event.targetTouches.length;
});
canvas.addEventListener('click', leftClick, false);			// Does this ever work?

var leftButton = document.getElementById("leftClick");
leftButton.addEventListener('click', leftClick);
leftButton.addEventListener('touchend', leftClick);

var rightButton = document.getElementById("rightClick");
rightButton.addEventListener('click', rightClick);
rightButton.addEventListener('touchend', rightClick);

setInterval(sendMousePosition, 10);
