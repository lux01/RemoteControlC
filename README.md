# C/HTML 5 Canvas Remote Control Mouse #

A lightweight (and highlighy experimental) remote mouse application for Microsoft Windows systems written in C and HTML/JavaScript.

## Getting Started ##

1. Download and compile the source-code. It was tested using Visual Studio 2012 on Windows, and clang 3.0 on Linux so I recommend these for the best results. The Makefile assumes clang is available, though the code should compile in your favourite C compiler.
2. Run the `RemoteControlC.exe`(Windows) or `rrc` (Linux) application. It expects that the working directory is the application directory.
3. Enter a password when prompted. There is currently a 50 character maximum for such passwords, any characters past the 50th will be ignored.
4. Navigate your favourite HTML 5 Canvas enabled browser on a secondary computer to a suitable  IP:Port combination from those listed.
5. Enter the username shown in the program output, and the password you specified earlier when prompted.
6. Move your mouse, or swipe your finger, across the canvas area in the centre of the screen to move the mouse on the first computer! You can use the "Left Click" and "Right Click" buttons to emulate mouse clicks.
7. To stop the server just hit &lt;ENTER&gt; (in the server application).

If the mouse isn't moving fast enough, or is moving too fast for you, you can change the mouse speed by either right-clicking the canvas or touching it with four fingers simultaneously. This will bring up the Mouse speed prompt dialogue where you can enter a new speed. Larger numbers make the mouse move faster.

_NB_: Since this code directly performs actions with the mouse, it is recommended that you do not run both the client and server on the same machine or crazy things might happen.

## Technologies Used ##

### Server-Side ###
The server-side code is written in C and uses the [Mongoose][1] lightweight web server. Mongoose is available under the [MIT License][2]. For Linux systems, the uinput kernel module is required, the running user should have write permission to the uinput device which is assumed to be located at `/dev/uinput`.

 [1]: http://code.google.com/p/mongoose/
 [2]: http://opensource.org/licenses/mit-license.php

### Client-Side ###
The client-side code is basic HTML 5, JavaScript and CSS making extensive use of the HTML 5 canvas and touch events.

## Changelog ##

### v0.1 -> v0.1.1 ###
  * Added Linux support.
  * Improved touch to clip support on touch devices:
    * Press with two fingers to left click
    * Press with three fingers to right click
    * Press with four fingers to bring up the mouse sensitivity option.
   Note that these are temporary changes and will be udpated when the updated UI is implemented.

## Roadmap ##

### v0.1 ###
  * Basic functionality: 
    * Mouse movement: _DONE_
    * Left and Right click: _DONE_
  * Windows only for server application. _DONE_
  * Minimalistic interface. _DONE_

### v0.2 ###
  * Touch canvas to left-click.
  * Better looking interface.
    * No more using JavaScript prompts to change values.
    * Icons to replace buttons.
  * Send keyboard keys support.

### v0.3 ###
  * Linux support. _DONE_

### The Distant Future
  * More advanced features (screen sharing?)
