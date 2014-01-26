/************************************************************************
 BridgeGL is an easy-to-use 2D graphics package powered by cairo and GTK+.
Copyright (c) 2013 Sandeep Chatterjee [chatte45@eecg.utoronto.ca]

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.


You should have received a copy of the GNU General Public License
along with this program. If not, get it here: "http://www.gnu.org/licenses/".
*************************************************************************/

#include "bgl_graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <time.h>
#include <string.h>
#include <map>
#include <stdio.h>
#include <math.h>
#include <sstream>

static void drawscreen (GtkWidget *widget, cairo_t *cr);
//static void on_key_press(GdkEventKey *event);
//static void on_mouse_move(GdkEventMotion *event);
static void on_mouse_button_press(GdkEventButton *event);

gtk_win gtkw (drawscreen, 950, 950);

//the extent of user coordinates
double user_extent_x = 1010;
double user_extent_y = 910;

//text balloon variables
bool to_show_warning_about_text_balloon = true;
ifstream myfile;

// for timer
vector <string> time_str;
bool first_call = true;

// for month and day stamp
map<string,char*> month;
map<string,char*> day;

//color index 
int c_index;

/*template<typename T>
bool isinf(T value)
{
	return numeric_limits<T>::has_infinity() && 
	       value == numeric_limits<T>::infinity();
}*/

void line2arr (char* str, vector<string>* arr, char *tokenizer)
{	
	string ts;
	char* tok;
	(*arr).clear();
	tok = strtok(str,tokenizer);
	while ( tok != NULL )
	{
		ts.assign(tok);
		(*arr).push_back(ts);
		tok = strtok(NULL,tokenizer);
	}
}	

// the widget passed here is the button, and normally we wont need to use it.
// global variables to draw polos, rectangles and sectors
vector<double> xcen(25, 0);
vector<double> ycen(25, 0);
vector<double> red(25,0);
vector<double> green(25,0);
vector<double> blue(25,0);
vector<double> alpha(25,0);
static void change_configuration(GtkWidget *widget, gpointer data)
{
	// randomly decide xcen and ycen 
	// between rectangle (50, 100) to (350, 400).
	for ( int i = 0; i < 25; i++ )
	{
		xcen[i] =  50 + rand()%300;
		ycen[i] = 100 + rand()%300;
		red[i] = 1.0*rand()/RAND_MAX;
		green[i] = 1.0*rand()/RAND_MAX;
		blue[i] = 1.0*rand()/RAND_MAX;
		alpha[i] = 1 - 0.3*rand()/RAND_MAX;
	}
	c_index = rand() % 25;
	gtkw.redraw();
}

void get_image_map_text_from_file( string name, vector<string> *info )
{
	if ( !myfile.is_open() )
	{
		fprintf(stderr, "Could not open the file to display balloon texts\n");
		return;
	}
	
	string line;
	string whitespaces (" \t\f\v\n\r");
	string start_tag = "/* "+name;
	string end_tag = "/* END "+name;
	myfile.seekg (0, ifstream::beg);
	bool to_push = false;
	int line_no = 0;
	stringstream ss;
	while ( myfile.good() )
	{
		getline (myfile,line);
		line_no++;
		if ( line.length() == 0 )
			continue;
		else if ( line.find( end_tag ) != string::npos )
			break;
		else if ( line.find( start_tag ) != string::npos )
			to_push = true;
		else if ( to_push )
		{
			size_t start = line.find_first_not_of(whitespaces);
			if ( start != string::npos )
			{
				ss.str(std::string());
				ss<<line_no;
				size_t end = line.find_last_not_of(whitespaces);
				(*info).push_back( ss.str() + ": " + 
				line.substr( start, end-start+1));
			}
		}
	}
}

static gboolean time_handler(GtkWidget *widget)
{
	time_t rawtime;
  	struct tm * timeinfo;
  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
  	char* str = asctime(timeinfo);
  	//printf ( "Current local time and date: %s", str);
  	
  	time_str.clear();
  	line2arr(str, &time_str, " :");
  	
  	if (!first_call)
		gtkw.redraw();
	return TRUE;
}

int main()
{
	gtkw.init_world( 0,0, user_extent_x, user_extent_y );
	//gtkw.activate_user_key_press_input( on_key_press );
	//gtkw.activate_user_mouse_pointer_position_input( on_mouse_move );
	gtkw.activate_user_mouse_button_press_input( on_mouse_button_press );
	
	style_button change_button( "Change", change_configuration, &gtkw );
	change_button.set_desc("Changes color Configuration for various shapes");
	gtkw.add_button_to_sidepane( &change_button);
	
	
	// Initialize configurations
	for ( int i = 0; i < 25; i++ )
	{
		xcen[i] =  50 + rand()%300;
		ycen[i] = 100 + rand()%300;
		red[i] = 1.0*rand()/RAND_MAX;
		green[i] = 1.0*rand()/RAND_MAX;
		blue[i] = 1.0*rand()/RAND_MAX;
		alpha[i] = 1 - 0.3*rand()/RAND_MAX;
	}
	c_index = rand() % 25;
	
	//open *THIS* file, as it has all the balloon texts
	myfile.open(__FILE__, ifstream::in);
	
	// tell gtk_win that we will use pango markups
	
	// intilialize the maps
	month["Jan"] = "JANUARY";
	month["Feb"] = "FEBRUARY";
	month["Mar"] = "MARCH";
	month["Apr"] = "APRIL";
	month["May"] = "MAY";
	month["Jun"] = "JUNE";
	month["Jul"] = "JULY";
	month["Aug"] = "AUGUST";
	month["Sep"] = "SEPTEMBER";
	month["Oct"] = "OCTOBER";
	month["Nov"] = "NOVEMBER";
	month["Dec"] = "DECEMBER";
	day["Mon"] = "MONDAY";
	day["Tue"] = "TUESDAY";
	day["Wed"] = "WEDNESDAY";
	day["Thu"] = "THURSDAY";
	day["Fri"] = "FRIDAY";
	day["Sat"] = "SATURDAY";
	day["Sun"] = "SUNDAY";
	
	//Initialize image maps
	image_map polo( "POLO", 50, 100, 350, 400, 5, get_image_map_text_from_file);
	image_map donut( "DONUT", 430, 50, 670, 290, 5, get_image_map_text_from_file);
	image_map pattern( "PATTERN", 40, 630, 40+256, 630+256, 5, get_image_map_text_from_file);
	image_map shadowtext( "CAIROTEXT", 30, 15, 365, 60, 5, get_image_map_text_from_file);
	image_map lines( "LINES", 35, 445, 345, 580, 5, get_image_map_text_from_file);
	image_map fonts( "PANGOTEXT", 680, 25, 990, 285, 5, get_image_map_text_from_file);
	image_map rectangles( "RECTANGLES", 430, 340, 730, 640, 5, get_image_map_text_from_file);
	image_map sectors( "SECTORS", 890-110, 460-110, 890+110, 460+110, 5, get_image_map_text_from_file);
	image_map clock( "CLOCK", 738, 638, 962, 862, 5, get_image_map_text_from_file);
	image_map date( "MARKUP", 340, 685, 700, 880, 5, get_image_map_text_from_file);
	gtkw.image_maps.push_back( &polo );
	gtkw.image_maps.push_back( &donut );
	gtkw.image_maps.push_back( &pattern );
	gtkw.image_maps.push_back( &shadowtext );
	gtkw.image_maps.push_back( &lines );
	gtkw.image_maps.push_back( &fonts );
	gtkw.image_maps.push_back( &rectangles );
	gtkw.image_maps.push_back( &sectors );
	gtkw.image_maps.push_back( &clock );
	gtkw.image_maps.push_back( &date );
	
	// for timer
	time_handler(NULL);
	first_call = false;
	g_timeout_add(1000, (GSourceFunc) time_handler, NULL);
	
	gtkw.init_graphics( "Demonstration" );
	
	myfile.close();
}

static void drawscreen (GtkWidget *widget, cairo_t *cr)
{
	gtkw.clearscreen( cr );

	/* CAIROTEXT */
	// Creation of Shadow text using Cairo Toy Text API. 
	// ------------------------------------------------------
	gtkw.setfontface (cr, "Tahoma", CAIRO_FONT_SLANT_NORMAL,
	                  CAIRO_FONT_WEIGHT_NORMAL);
	gtkw.setfontsize (cr, 60.0);
	gtkw.setcolor (cr, 0.4, 0.4, 0.4, 1);
	gtkw.drawtext( cr, 40, 60., "Shadow", 1000); 
	gtkw.setcolor (cr, 0.1, 0.65, 0.77, 1);
	gtkw.drawtext( cr, 37, 57, "Shadow", 1000);
	// ------------------------------------------------------
	/* END CAIROTEXT */

	/* POLO */
	// PRIMITIVES USED: circles
	// A random assortment of hollow circles. Press the
	// 'Change' button to alter their arrangement and colors. 
	// The line width is scaled as per the current context.
	// ------------------------------------------------------
	gtkw.setlinewidth(cr, gtkw.user2context_x(15) );
	for ( int i = 0; i < 25; i++ )
	{
		gtkw.setcolor(cr, 0.4, 0.4, 0.4, 1);
		gtkw.drawarc( cr, xcen[i]+2, ycen[i]+2, 25, 0, 360 );
		
		gtkw.setcolor(cr, red[i], green[i], blue[i], 1 );
		gtkw.drawarc( cr, xcen[i], ycen[i], 25, 0, 360 );
	}
	// ------------------------------------------------------
	/* END POLO */
	
	/* PATTERN */
	// Creates a gradient pattern with a combination of cairo API and BGL.
	// Conversion to user_context is necessary to ensure that drawing is
	// displayed correctly under all transformations.
	// NOTE: This part might not render properly under ceratin transformations
	// ------------------------------------------------------
    gtkw.translate_coordinates(cr, 40, 630 );
    cairo_pattern_t *pat;
    pat = cairo_pattern_create_linear (gtkw.user2context_x(0.0), gtkw.user2context_y(0.0),  
                                       gtkw.user2context_x(256), gtkw.user2context_y(256) );
    cairo_pattern_add_color_stop_rgba (pat, 1, 1, 1, 1, 1);
    cairo_pattern_add_color_stop_rgba (pat, 0.5, red[c_index], green[c_index], blue[c_index], 1);
    cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
    cairo_rectangle (cr, gtkw.user2context_x(0.0), gtkw.user2context_y(0.0),  
                         gtkw.user2context_x(256), gtkw.user2context_y(256) ) ;
    cairo_set_source (cr, pat);
    cairo_fill (cr);
    cairo_pattern_destroy (pat);
    pat = cairo_pattern_create_radial ( gtkw.user2context_x(115.2), gtkw.user2context_y(102.4),
    									gtkw.user2context_x(25.6), gtkw.user2context_x(102.4),
    									gtkw.user2context_y(102.4), gtkw.user2context_x(128.0) );
    cairo_pattern_add_color_stop_rgba (pat, 0, red[c_index], green[c_index], blue[c_index], 1);
    cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
    cairo_set_source (cr, pat);
    gtkw.fillarc (cr, 128.0, 128.0, 76.8, 0, 360);
    cairo_pattern_destroy (pat);
    gtkw.translate_coordinates(cr, -40, -630 );
    
    // ------------------------------------------------------
    /* END PATTERN */
	
	/* LINES */
	// PRIMITIVES USED: lines
	// Drawing lines of different styles and widths
	// ------------------------------------------------------
	gtkw.setcolor(cr, 0,0,0,1);
	gtkw.setlinestyle(cr, 0);
	gtkw.setlinewidth(cr,2);
	gtkw.drawline(cr, 40, 450, 340, 450);
	gtkw.setlinewidth(cr,4);
	gtkw.drawline(cr, 40, 470, 340, 470);
	gtkw.setlinewidth(cr,8);
	gtkw.drawline(cr, 40, 490, 340, 490);
	gtkw.setlinewidth(cr,16);
	gtkw.drawline(cr, 40, 510, 340, 510);
	gtkw.setlinewidth(cr,4);
	double dashes[4] = {5., 2., 9, 2};
	gtkw.setlinestyle(cr, 4, dashes);
	gtkw.drawline(cr, 40, 530, 340, 530);
	double dashes2[2] = {4., 1.};
	gtkw.setlinestyle(cr, 2, dashes2);
	gtkw.drawline(cr, 40, 545, 340, 545);
	double dashes3[4] = {7., 3., 2., 3.};
	gtkw.setlinestyle(cr, 4, dashes3);
	gtkw.drawline(cr, 40, 560, 340, 560);
	double dashes4[2] = {5, 5};
	gtkw.setlinestyle(cr, 2, dashes4);
	gtkw.drawline(cr, 40, 575, 340, 575);
	// ------------------------------------------------------
	/* END LINES */
	
	/* DONUT */
	// PRIMITIVES USED: ellipses
	// A donut shape drawn to demonstrate the use of cairo_rotate()
	// and drawellipticarc(). Note that we first translate the origin to
	// the centre of the shape, then rotate by 10 degrees for each new
	// ellipse, and finally translate the origin back to (0,0).  
	// ------------------------------------------------------
	gint i;
	gtkw.translate_coordinates(cr, 550., 170.);
	gtkw.setcolor(cr, blue[c_index], red[c_index], green[c_index], 1);
	gtkw.setlinewidth(cr, 1);
	gtkw.setlinestyle(cr, 0);
	for (i = 0; i < 36; i++) 
	{
		cairo_save(cr);
		cairo_rotate(cr, i*M_PI/36);
		gtkw.drawellipticarc(cr, 0, 0, 120, 36, 0, 360);
		cairo_restore(cr);
	}
	gtkw.translate_coordinates(cr, -550., -170.);
	// ------------------------------------------------------
	/* END DONUT */	
	
	/* PANGOTEXT */
	// Use PangoCairo layout to render text. This is the
	// recommended way of drawing text on canvas. Note that
	// we explicitly disable the makup_enable flag for faster 
	// processing. Also, different text styles can be set as
	// - Normal or Bold: using 'Sans Normal 16' or 'Sans Bold 26'
	//   in setpangofontdesc().
	// - Italic: using 'Helvetica Normal 18' in setpangofontdesc()
	//   or PANGO_STYLE_ITALIC while calling drawpangotext().
	// - Bold+Italic:  use 'Sans Bold 26' in  setpangofontdesc()
	//  and PANGO_STYLE_ITALIC while calling drawpangotext().
	// Note that PANGO_STYLE_* takes precedence over 'Sans * 26'
	// in the code. 
	// ------------------------------------------------------
	gtkw.pango_markup_enable(0);
	double fw = 0, fh = 0;
	gtkw.setcolor(cr, 0, 0, 0, 1);
	char text[][25] = {"Sans Normal 8","Sans Normal 16","Sans Bold 26",
	"Verdana Normal 20","Georgia Normal 8","Serif Normal 20",
	"Monospace Normal 20","Tahoma Normal 20", "Helvetica Normal 18"} ;
	double c = 40;
	for( int i = 0; i < 9; i++ )
	{
		c = c + fh;
		gtkw.setpangofontdesc(text[i]);
		gtkw.drawpangotext(cr, 690., c, text[i], fw, fh,
		(PangoStyle)(i%3), 5000);
	}
	// ------------------------------------------------------
	/* END PANGOTEXT */
	
	/* RECTANGLES */
	// PRIMITIVES USED: rectangles
	// A random assortment of hollow rectangles. Press the
	// 'Change' button to alter their arrangement and colors.
	// ------------------------------------------------------
	gtkw.setlinewidth(cr, gtkw.user2context_x(10) );
	for( int i = 0; i < 16; i++ )
	{
		cairo_save(cr);
		gtkw.translate_coordinates(cr, ycen[i]+330, xcen[i]+290  );
		cairo_rotate(cr, i*M_PI/16);
		gtkw.setcolor(cr, 0.2, 0.2, 0.2, 1);
		gtkw.drawrect(cr, -23, -23, 27, 27);
		gtkw.setcolor(cr, 1-red[i], 1-green[i], 1-blue[i], 1);
		gtkw.drawrect(cr, -25, -25, 25, 25);
		cairo_restore(cr);
	}
	// ------------------------------------------------------
	/* END RECTANGLES */
	
	/* SECTORS */
	// PRIMITIVES USED: arcs
	// Using fillarc() with different random colors to draw
	// a complete circles. The colors change when 'Change'
	// button is pressed. 
	// ------------------------------------------------------
	gtkw.setcolor(cr, 0.2, 0.2, 0.2, 1);
	gtkw.fillarc(cr, 892, 462, 110, 0, 360);
	for( int i = 0; i < 8; i++ )
	{
		gtkw.setcolor(cr, red[i*3], green[i*3], blue[i*3], 1);
		gtkw.fillarc(cr, 890, 460, 110, i*45, (i+1)*45);
	}
	// ------------------------------------------------------
	/* END SECTORS */
	
	/* CLOCK */
	// A clock to display present time. Demonstrates calling a function
	// repeatedly, and showing progress by redrawing the canvas. Repeated
	// calling is done using g_timeout_add() in main()
	double seconds_angle = atoi(time_str[5].c_str())*6.0;
	gtkw.setlinewidth(cr, 3);
	if (seconds_angle != 0)
	{
		gtkw.setcolor(cr, red[c_index], blue[c_index], green[c_index], 0.4);
		gtkw.drawarc(cr, 850, 750., 110, seconds_angle-90, 360-90);
	}
	else
		seconds_angle = 360;
	gtkw.setcolor(cr, red[c_index], blue[c_index], green[c_index], 1);
	gtkw.drawarc(cr, 850, 750., 110, 0-90, seconds_angle-90);
	c = 750; gtkw.setpangofontdesc("Helvetica Bold 60");
	gtkw.drawpangotext(cr, c, 695., (char*)time_str[3].c_str(), fw, fh, PANGO_STYLE_NORMAL, 300);
	c += fw; gtkw.setpangofontdesc("Helvetica Normal 60");
	gtkw.drawpangotext(cr, c, 695., ":", fw, fh, PANGO_STYLE_NORMAL, 300);
	c += fw; 
	gtkw.drawpangotext(cr, c, 695., (char*)time_str[4].c_str(), fw, fh, PANGO_STYLE_NORMAL, 300);
	/* END CLOCK */
	
	/* MARKUP */
	// In order for zooming and other transforms to work,
	// function setpangofontdesc() should always be used to
	// set font description instead of 'font' attribute of <span>.
	// Note that when pango-markups are enabled, the argument
	// PANGO_STYLE_* is ignored. Also, <span> attributes have the
	// last say whether a font will be Normal, Bold, Italic or Oblique.
	// So, please dont use the 'size' or 'font' attribute in <span>, use
	// setpangofontdesc() insatead.
	gtkw.pango_markup_enable(1);
	c = 685.; 
	double u = 345;
	
	char markup_text[][300] = {
	"Font_desc = 'Serif Normal 15'",
	"<span underline='double' fgcolor='#e47821'>underline='double' fgcolor='#e47821'</span>",
	"<span bgcolor='#147861' fgcolor='white'>bgcolor='#147861' fgcolor='white'</span>",
	"Now Font_desc = 'Georgia Normal 15'",
	"<span font_weight='800'>font_weight='800'</span>",
	"<span font_weight='200'>Some sup:<tt>(a+b)<sup>2</sup></tt> Sub: x<sub>1</sub></span>",
	"<span strikethrough='true' underline='error' underline_color='red'>strikethrough text</span>",
	"<span letter_spacing='1000'>5 standard entities: &amp; &lt; &gt; &quot; &apos;</span>"};
	
	for ( int i = 0; i < 8; i++ )
	{
		if ( i == 0 )
			gtkw.setpangofontdesc("Serif Normal 15");
		else if ( i == 3 )
			gtkw.setpangofontdesc("Georgia Normal 15");
		gtkw.drawpangotext(cr, u, c, markup_text[i], fw, fh, PANGO_STYLE_NORMAL, 5000);
		c += fh;
	} 
	/* END MARKUP */
	
	// disable markups
	gtkw.pango_markup_enable(0);
}

/*
static void on_key_press(GdkEventKey *event)
{
	printf("key pressed: %d\n", event->keyval);
}

static void on_mouse_move(GdkEventMotion *event)
{
	printf("position: x: %.2f, y: %.2f\n", event->x, event->y);
}*/
static void on_mouse_button_press(GdkEventButton *event)
{
	printf("Clicked on, 1) Window coord: x: %.2f, y: %.2f, 2) User coord: x: %.2f, y: %.2f,\n",
	event->x, event->y, gtkw.user2context_x(event->x), gtkw.user2context_y(event->y) );
}
