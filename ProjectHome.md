
# Introduction #

Bridge Graphics Library or bridgeGL is an easy to use graphics library,
powered by cairo and GTK+. It works with GTK+2 and GTK+3. By itself,
cairo is a very powerful vector based graphics library with
support for multiple output devices. However, using cairo to draw
graphics for any program will require some understanding of how
cairo works, and although it's fairly easy once you look into it;
the initial learning curve might be steep. Fortunately, BridgeGL provides an
easy-to-use application layer between your program and the cairo
backend. The functions in BGL enable you to draw shapes and
texts, set colors, linewidths and much more. The output is
generally rendered on a system window, but can also be saved to a
.pdf, .png, .svg or .ps files. The GUI is based on GTK+ library.

I still need to stress test the software, but based on some pre-eliminary
results, I am guessing that more than 20,000 shapes drawn and updated regularly
should slow it down a lot. But, theres a lot that could be done within
20,000 shapes. Also, I will udpate the results here when I have them.


# Features #
  1. **Simplicity**: It acts as a simple and generic interface between the user and the Cairo backend, enabling user to draw primitive shapes and texts (and change their attributes) in his own coordinate system, without (initially) bothering about 2D graphics jargon.
  1. **Usability**: The UI supports zooming and panning operations. Also, using the cairo backend, it enables the user to save the graphics to a .pdf, .png, .svg or .ps file.
  1. **Callbacks**: It provides a framework to enable function callbacks for key presses, mouse movement and mouse button press on the drawing area. The user can setup his own routines as desired to handle the events.
  1. **Customization**: The user can create custom buttons to do application specific tasks and add them to BridgeGL UI.


# How to Proceed #
The main documentation for this project can be found [here](http://www.eecg.toronto.edu/~chatte45/bgl_doc/index.html). After downloading the package, the best way to start would be to directly read the [FAQ](http://www.eecg.toronto.edu/~chatte45/bgl_doc/FAQ.html) page. It gives the simplest code that can get you started with BridgeGL. Also, it might be worthwhile to familiarize yourself with the the UI as detailed in [FAQ](http://www.eecg.toronto.edu/~chatte45/bgl_doc/FAQ.html) and [User Interface](http://www.eecg.toronto.edu/~chatte45/bgl_doc/user_interface.html) tabs in the documentation. Please note that the documentation is incomplete, and is a work in progress. Still, the basic drawing functions are described there and should be enough to get you started.


# Some Important Questions Always Asked #

### What libraries do I need to compile and run the downloaded source code? ###
<p>To use bridgeGL for rendering graphics in your application, you will need GTK+2.0 development kit. Many machines may already have GTK+ runtime libraries installed, but to compile the code, you will need the development kit. GTK+, or the GIMP Toolkit, is a multi-platform toolkit for creating graphical user interfaces. It's a cross platform and easy-to-use API and should be easy to install. Visit <a href='http://www.gtk.org/download/index.php'>GTK+ download page</a> to download and install the development kit on your machine. A few handy tips:<br>
<ol>
<li>For linux users, don't waste a lot of time in trying to compile the source packages and dependencies separately. Search for a debian package in your software repository. The command that works on Ubuntu and Debian is:<br>
<pre><code>sudo apt-get install libgtk2.0-dev</code></pre>
Its hassle free and neat. If you want a list of all GTK-related packages installed on your system, try:<br>
<pre><code>dpkg -l | grep libgtk</code></pre>
</li>
<li>For window machines, download and untar the <b>all-in-one-bundle</b> rather than the individual packages. Also, install GNU compiler for windows, known as <a href='http://www.mingw.org/'>MinGW</a>. It is a handy tool to have if you are interested in programming. For more details, click <a href='http://www.gtk.org/download/win32.php'>here</a>.</li>
<li>I don't have any experience of GTK+ on Mac OS X, so you are on your own.</li>
</ol>
</p>

### Is there a minimal code example? ###
<p>To use bridgeGL in your application to render the graphics, you will need two files:<br>
<ul>
<li>A header file <em>bgl_graphics.h</em>, containing all class and function declarations.</li>
<li>A source file <em>bgl_graphics.cpp</em> containig all the class and function definitions.</li>
</ul>
Suppose, a dummy application intends to draw a simple rectangle outline in blue colored lines of width 2px in the canvas (drawing area).<br>
Then, the minimalistic code that would do the same is:<br>
<pre><code><br>
/* Include the header file */<br>
#include "bgl_graphics.h"<br>
<br>
/* This is the main drawing function, all your drawing has to be done<br>
here. It can have any name, but the input arguments to the function<br>
needs to be a Gtkwidget and a cairo context (cairo_t), and in that order. The arguments<br>
are passed automatically as the function is called to do the required drawing.<br>
As of now, we only declare the function, and define it later.*/<br>
static void drawscreen (GtkWidget *widget, cairo_t *cr);<br>
<br>
/* Declare an instance of the class gtk_win. This is the main class that renders<br>
the graphics, handles the panning &amp; zooming and passes control to callback functions<br>
if required. It's constructor requires three arguments, a drawscreen function, and<br>
the width and height of window to be displayed. */<br>
gtk_win gtkw (drawscreen, 800, 800);<br>
<br>
/* The main function*/<br>
int main()<br>
{<br>
/* We use the init_world() function to initialize your canvas. The main purpose<br>
of this function is to set the user-coordinate system. In the following line, we<br>
set the user coordinates to vary from (0,0) on top-left to (710, 710) on bottom-right.<br>
It is not necessary to start from 0 or to maintain 1:1 aspect ratio.*/<br>
gtkw.init_world( 0,0, 710, 710 );<br>
<br>
/* Now that the coordinate system is set, its time to initialize and display the graphics.<br>
We do that by calling the init_graphics() function, which requires a single argument<br>
specifying the window name to be displayed at the window title bar. */<br>
gtkw.init_graphics( "minimalistic code" );<br>
<br>
/* And that's it. Once you have initialized the graphics, the control is passed to<br>
the main event loop that waits for user interaction. Once the user quits the application<br>
by pressing 'Alt+F4' or the 'close' button on title bar, we also exit the main loop.*/<br>
return 0;<br>
}<br>
<br>
/* We still have one more very important thing to do, and it is to write the drawscreen function<br>
to do the required drawing, which in this case is a rectangle outline in blue colored lines of<br>
width 2px.*/<br>
static void drawscreen (GtkWidget *widget, cairo_t *cr)<br>
{<br>
/* The first function to be called before any drawing is performed. */<br>
gtkw.clearscreen( cr );<br>
<br>
/* Set the current color to blue, see documentation for more details */<br>
gtkw.setcolor(cr, 0, 0, 1, 1);<br>
<br>
/* Set the current linewidth to 2px, see documentation for more details */<br>
gtkw.setlinewidth(cr, 2);<br>
<br>
/* Now we draw the rectangle */<br>
gtkw.drawrect(cr, 100, 100, 600, 600 );<br>
}<br>
</code></pre></p>

### How can I compile this example code? ###
<p>You are already provided with a makefile to build the code. To compile it with GTK+3 development libraries, use:<br>
<pre><code> make GV=GTK+3 </code></pre>
and for GTK+2, simply type:<br>
<pre><code> make </code></pre>
As of now, expect for UI differences, there is no functional difference in using either of them.<br>
</p>

# Known Bugs #
<ul>
<li> The gradient rendering using native cairo APIs fails under certain canvas transformations. </li>
<li> On some GTK versions, an offset might be added to the y-coordinate that effects the location of a mouse click. Note that the drawing is unaffected. </li>
<li> CSS style-sheets are not consistent across platforms and are experimental </li>
<li> The 'Translate <code>*</code>' options under view menu don't work. However, the view-port can be easily translated using the arrow keys on keyboard. </li>
<li> The bounding box estimates are really bad when using cairo toy text API to draw text. Use the function drawpangocairo() instead.</li>
</ul>

# To Do #
<ul>
<li>Provide a second function pointer to user for drawing, where he/she can pass a (void<code>*</code>) pointer with parameter values to used while drawing, instead of using global variables. </li>
<li> As of now, user has to redrawn the whole canvas for every update. Provide a mechanism by which he/she can redraw only a part of the canvas. </li>
<li> A searchbox, that displays a pop-up menu with list of clickable options matching the description typed by the user, so that the implemented functionality can be easily found and used.</li>
</ul>