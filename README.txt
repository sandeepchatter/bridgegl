About
======

Bridge Graphics Library or bridgeGL is an easy to use graphics library,
powered by Cairo and GTK+. It works with GTK+2 and GTK+3. By itself,
Cairo is a very powerful vector based graphics library with
support for multiple output devices. However, using Cairo to draw
graphics for any program will require some understanding of how
Cairo works, and although it's fairly easy once you look into it;
the initial learning curve might be steep. Fortunately, BridgeGL provides an
easy-to-use application layer between your program and the Cairo
backend. The functions in BGL enable you to draw shapes and
texts, set colors, linewidths and much more. The output is
generally rendered on a system window, but can also be saved to a
.pdf, .png, .svg or .ps files. The GUI is based on GTK+ library.

I still need to stress test the software, but based on some pre-eliminary
results, I am guessing that more than 20,000 shapes drawn and updated regularly
should slow it down a lot. But, there's a lot that could be done within 
20,000 shapes. Also, I will update the results here when I have them.

License 
=======

Released under GNU license:
This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

Known Bugs
==========

Some known bugs
- The gradient rendering using native Cairo APIs fails under certain 
  canvas transformations
- On some GTK versions, an offset might be added to the y-coordinate
  that effects the location of a mouse click. Note that the drawing is
  unaffected.
- CSS style-sheets are not consistent across platforms and are
  experimental
- The 'Translate *' options under view menu don't work. However, the
  view-port can be easily translated using the arrow keys on keyboard.
- The bounding box estimates are really bad when using Cairo toy text
  API to draw text. Use the function drawpangocairo() instead.

If you find something other bugs, let me know. 

Software Required
==================

To use bridgeGL for rendering graphics in your application, you will need
either GTK+2.0 or GTK+3.0 development kit. Many machines may already have GTK+
runtime libraries installed, but to compile the code, you will need the
development kit. GTK+, or the GIMP Toolkit, is a multi-platform toolkit
for creating graphical user interfaces. It's a cross platform and
easy-to-use API and should be easy to install. Visit GTK+ download page
to download and install the development kit on your machine. A few handy
tips:

    - For linux users, don't waste a lot of time in trying to compile the
      source packages and dependencies separately. Search for a debian package
      in your software repository. The command that works on Ubuntu and Debian
      is:

      > sudo apt-get install libgtk2.0-dev (for GTK+2)
      > sudo apt-get install libgtk3.0-dev (for GTK+3)

      Its hassle free and neat. If you want a list of all GTK-related packages
      installed on your system, try:

      dpkg -l | grep libgtk

    - For window machines, download and untar the all-in-one-bundle rather
      than the individual packages. Also, install GNU compiler for windows,
      known as MinGW. It is a handy tool to have if you are interested in
      programming.


File List
=========

    - bgl_graphics.h, the header file of the graphics package

    - bgl_graphics.cpp, the source file of the graphics package

    - application.cpp, an example file to demonstrate the graphics package. 

    - makefile, a sample makefile, that can be used to compile the
      demonstration, see the 'Compiling the code' section below.

    - logo.png, a .png. file, which is the logo for BridgeGL.

    - gpl.txt, the GNU public license.

    - icons, a folder of icon sets used in the UI.
    
    - README.txt: this file
    
    - css/style.css, a light-themed CSS file to use with BridgeGL when compiled
      with GTK+3
    
    - css/style-dark.css, a dark-themed CSS file to use with BridgeGL when
      compiled with GTK+3

Compiling the code
==================

After installing GTK+2 (GTK+3) development kit, you can compile the code as

> make GV=GTK+3

to compile it with GTK+3 development libraries and 

> make 

to compile it with GTK+2. As of now, expect for UI differences, there is
no functional difference in using either of them.   


Documentation
=============

URL: http://www.eecg.toronto.edu/~chatte45/bgl_doc/index.html

The documentation is incomplete, and is a work in progress. Still, the basic 
drawing functions are described there. The FAQ page should get you started
quickly. The screenshots are old, and pertain to revision 4 of the software.

About the Author
================

chatte45[at]eecg[dot]utoronto[dot]ca
URL: http://www.eecg.toronto.edu/~chatte45

To do list
==========

There's still a lot to be done. Some of them are:
    - Provide a second function pointer to user for drawing, where he/she can 
      pass a (void*) pointer with parameter values to used while drawing,
      instead of using global variables.
    - As of now, user has to redraw the whole canvas every-time. Provide a
      mechanism by which he/she can redraw only a part of the canvas.
    - A searchbox, that displays a pop-up menu with list of clickable options
      matching the description typed by the user, so that the implemented
      functionality can be easily found and used.
    - Some functions from gtk_win require restrcited access in future, so that
      they are not accessibile from outside the file bgl_graphics.cpp.
