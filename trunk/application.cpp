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
		fprintf(stderr, "Could not open the file: codes.txt to display balloon texts\n");
		return;
	}
	
	string line;
	string start_tag = "* "+name;
	string end_tag = "* END "+name;
	myfile.seekg (0, ifstream::beg);
	bool to_push = false;
	while ( myfile.good() )
	{
		getline (myfile,line);
		if ( line.length() == 0 )
			continue;
		else if ( line == end_tag )
			break;
		else if ( line == start_tag )
			to_push = true;
		else if ( to_push )
			(*info).push_back(line);
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
  	
  	/*for (int i = 0; i < time_str.size(); i++)
  	{
  		printf("[%d] = %s, ", i, time_str[i].c_str()); 
  	}
	printf("\n");*/
	
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
	change_button.set_desc("Changes Polo Configuration");
	gtkw.add_button_to_sidepane( &change_button);
	
	
	// initilaize random seed
 	//srand (time(NULL));
  
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
	
	//open codes.txt file, as it has all the balloon texts
	myfile.open("codes.txt", ifstream::in);
	
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
	image_map polo( "POLO", 50, 100, 350, 400, 10, get_image_map_text_from_file);
	image_map donut( "DONUT", 430, 50, 670, 290, 10, get_image_map_text_from_file);
	image_map pattern( "PATTERN", 40, 630, 40+256, 630+256, 10, get_image_map_text_from_file);
	image_map shadowtext( "SHADOWTEXT", 30, 15, 365, 60, 10, get_image_map_text_from_file);
	image_map lines( "LINES", 35, 445, 345, 580, 10, get_image_map_text_from_file);
	image_map fonts( "FONTS", 680, 25, 990, 285, 10, get_image_map_text_from_file);
	image_map rectangles( "RECTANGLES", 430, 340, 730, 640, 10, get_image_map_text_from_file);
	image_map sectors( "SECTORS", 890-110, 460-110, 890+110, 460+110, 10, get_image_map_text_from_file);
	image_map clock( "CLOCK", 738, 638, 962, 862, 10, get_image_map_text_from_file);
	image_map date( "DATE", 350, 710, 655, 840, 10, get_image_map_text_from_file);
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
	
	//gtkw.setbgcolor( cr, 0., 0., 0. );
	gtkw.setcolor(cr, 0, 0, 0, 1);
	gtkw.drawrect(cr, 0, 0, user_extent_x, user_extent_y );
	
	// create custom text
	gtkw.setfontface (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	gtkw.setfontsize (cr, 60.0);

	gtkw.setcolor (cr, 0.4, 0.4, 0.4, 1);
	gtkw.drawtext( cr, 40, 60., "Shadow", 1000); 

	gtkw.setcolor (cr, 0.1, 0.65, 0.77, 1);
	gtkw.drawtext( cr, 37, 57, "Shadow", 1000);
	//end custom text
	
	//random polo fills, from 50, 100 to 350, 400
	cairo_save(cr);
	gtkw.setlinewidth(cr, gtkw.user2context_x(15) );
	for ( int i = 0; i < 25; i++ )
	{
		gtkw.setcolor(cr, 0.4, 0.4, 0.4, 1);
		gtkw.drawarc( cr, xcen[i]+2, ycen[i]+2, 25, 0, 360 );
		
		gtkw.setcolor(cr, red[i], green[i], blue[i], 1 );
		gtkw.drawarc( cr, xcen[i], ycen[i], 25, 0, 360 );
	}
	cairo_restore(cr);
	// end random polo fills
	
	// create pattern, a combination of cairo API and BGL.
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

	pat = cairo_pattern_create_radial ( gtkw.user2context_x(115.2), gtkw.user2context_y(102.4), gtkw.user2context_x(25.6),
		                            gtkw.user2context_x(102.4), gtkw.user2context_y(102.4), gtkw.user2context_x(128.0) );
	cairo_pattern_add_color_stop_rgba (pat, 0, red[c_index], green[c_index], blue[c_index], 1);
	cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
	cairo_set_source (cr, pat);
	gtkw.fillarc (cr, 128.0, 128.0, 76.8, 0, 360);
	cairo_pattern_destroy (pat);
	gtkw.translate_coordinates(cr, -40, -630 );
	// end pattern
	
	// start lines
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
	// end lines
	
	// start donut
	gint i;
	gtkw.translate_coordinates(cr, 550., 170.);
	gtkw.setcolor(cr, 0.992157, 0.42353, 0, 1);
	//gtkw.setcolor(cr, blue[c_index], red[c_index], green[c_index], 1);
	gtkw.setlinewidth(cr, 1);
	gtkw.setlinestyle(cr, 0);
	for (i = 0; i < 36; i++) 
	{
		cairo_save(cr);
		cairo_rotate(cr, i*M_PI/36);
		gtkw.drawellipticarc(cr, 0, 0, 120, 36, 0, 360);
		cairo_restore(cr);
	}//*/
	gtkw.translate_coordinates(cr, -550., -170.);
	// end donut
	
	
	//start font
	gtkw.setcolor(cr, 0, 0, 0, 1);
	gtkw.setfontsize(cr, 8);
	gtkw.setfontface(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	gtkw.drawtext(cr, 690., 40., "Sans 8 point Normal", 500);
	gtkw.setfontsize(cr, 16);
	gtkw.drawtext(cr, 690., 60., "Sans 16 point Normal", 500);
	gtkw.setfontsize(cr, 24);
	gtkw.drawtext(cr, 690., 85., "Sans 24 point Normal", 500);
	gtkw.setfontsize(cr, 26);
	gtkw.setfontface(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	gtkw.drawtext(cr, 690., 115., "Sans 26 point Bold", 500);
	gtkw.setfontsize(cr, 20);
	gtkw.setfontface(cr, "Verdana", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	gtkw.drawtext(cr, 690., 140., "Verdana 20 point Normal", 500);
	gtkw.setfontface(cr, "Georgia", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
	gtkw.drawtext(cr, 690., 165., "Georgia 20 point Italic", 500);
	gtkw.setfontface(cr, "Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	gtkw.drawtext(cr, 690., 190., "Serif 20 point Normal", 500);
	gtkw.setfontface(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	gtkw.drawtext(cr, 690., 210., "Monospace 20 point Normal", 500);
	gtkw.setfontsize(cr, 40);
	gtkw.drawtext(cr, 690., 245., "Monospace 40", 500);
	gtkw.setfontface(cr, "Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	gtkw.drawtext(cr, 690., 280., "Serif 40 Bold", 500);
	//end font
	
	//start rectangles
	gtkw.setlinewidth(cr, gtkw.user2context_x(15) );
	for( int i = 0; i < 20; i++ )
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
	//end rectangles
	
	//start sectors
	gtkw.setcolor(cr, 0.2, 0.2, 0.2, 1);
	gtkw.fillarc(cr, 892, 462, 110, 0, 360);
	for( int i = 0; i < 8; i++ )
	{
		gtkw.setcolor(cr, red[i*3], green[i*3], blue[i*3], 1);
		gtkw.fillarc(cr, 890, 460, 110, i*45, (i+1)*45);
	}
	//end sectors
	
	// show time as a clock widget, 236/255., 75/255., 65/255.
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
	gtkw.setfontsize(cr, 78);
	gtkw.setfontface(cr, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	gtkw.drawtext(cr, 750., 775., (char*)time_str[3].c_str(), 300);
	gtkw.setfontsize(cr, 84);
	gtkw.setfontface(cr, "FreeMono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	gtkw.drawtext(cr, 855., 775., (char*)time_str[4].c_str(), 300);
	gtkw.setfontface(cr, "LMSans10", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	gtkw.drawtext(cr, 840., 770., ":", 300);
	
	//show date and year
	gtkw.setfontface(cr, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	gtkw.setfontsize(cr, 100);
	char buffer[2];
	sprintf (buffer, "%02d", atoi(time_str[2].c_str()));
	gtkw.drawtext(cr, 360., 840., buffer, 300);
	gtkw.setfontsize(cr, 56);
	gtkw.drawtext(cr, 474., 840., (char*)time_str[6].substr(0,4).c_str(), 700);
	gtkw.setfontface(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	gtkw.setfontsize(cr, 55);
	gtkw.drawtext(cr, 360., 765., month[time_str[1]], 700);
	//gtkw.setfontface(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	gtkw.setfontsize(cr, 32);
	gtkw.drawtext(cr, 474., 795., day[time_str[0]], 700);
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
