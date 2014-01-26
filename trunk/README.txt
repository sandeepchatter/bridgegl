About
======

Bridge Graphics Library or bridgeGL is an easy to use graphics library,
powered by cairo and GTK+. It works with GTK+2 and GTK+3. By itself,
cairo is a very powerful vector based graphics library with
support for multiple output devices. However, using cairo to draw
graphics for any program will require some understanding of how
cairo works, and although it's fairly easy once you grasp it;
initially it might seem quite daunting. BGL provides an
easy-to-use application layer between your program and the cairo
backend. The functions in BGL enable you to draw shapes and
texts, set colors, linewidths and much more. The output is
generally rendered on a system window, but can also be saved to a
.pdf, .png, .svg or .ps files.

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

Softwares Required
==================

To use bridgeGL for rendering graphics in your application, you will
need GTK+2.0 development kit. Many machines may already have GTK+
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

    -bgl_graphics.h, the header file of the graphics package

    -bgl_graphics.cpp, the source file of the graphics package

    -application.cpp, an example file to demostrate the graphics package. 

    -makefile, a sample makefile, that can be used to complile the
    demonstration.

    -logo.png, a .png. file, which is the logo for BridgeGL.

    -gpl.txt, the GNU public license.

    -icons, a folder of icon sets used in the UI.

Compiling the code
==================

After installing GTK+2 (GTK+3) development kit, you can compile the code as

> make GV=GTK+3

to compile it with GTK+3 develompenyt libraries and 

> make 

to compile it with GTK+2. As of now, expect for UI differences, there is
no functinal difference in using both the version.   

