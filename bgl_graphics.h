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

#ifndef _BGL_GRAPHICS_H_
#define _BGL_GRAPHICS_H_

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-ps.h>
#include <cairo-svg.h>
#include <math.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>

#ifdef GTK+3
#include <gdk/gdkkeysyms-compat.h>
#else
#include <gdk/gdkkeysyms.h>
#endif

#ifndef MAX
#define max(a,b) (((a) > (b))? (a) : (b))
#endif
#ifndef MIN
#define min(a,b) ((a) > (b)? (b) : (a))
#endif

/* debug mode, verbose output */
//#define DEBUG

/* draws a bouding box around any shape drawn with the help of gtk_win class
 functions  */
//#define BOUNDING_BOX

/* Stylize side pane buttons when compiled with GTK+2. CSS is used to 
stylize the UI in GTK+3.  
*/
#define STYLE_BUTTONS

/* Follows the mouse pointer and updates the statubar with its location*/
//#define FOLLOW_MOUSE_POINTER

/* Scales text as user zooms. Without it, the text behaviour is undefined
under transforms.*/
#define SCALE_TEXT

/* Detemines whether the toolbar should be attcahed to the UI or not. 
*/
//#define TOOLBAR

/* Detemines whether the toolbar should be attcahed to the UI or not. Note that
most of the functionality is defined through menus and toolbars, So define at
least one of macros in header file, preferably the MENU_BAR macro.  
*/
#define MENU_BAR

/* If GTK+3 is being used, should the CSS be used to customize UI. Uncomment for
native look and feel
*/ 
#define APPLY_CSS

/* All system dialogs (not the main window) are undecorated if the following
macro is defined 
*/
//#define UNDECORATED_DIALOG


#define CONV_DEG_TO_RAD 0.01745329252  // pi/180
#define MAX_SHEAR 3.73205080756        // tan(75)
#define PI 3.14159265

using namespace std;

typedef void (*draw_gtk)(GtkWidget *widget, cairo_t *cr);
typedef void (*button_fcn)(GtkWidget *widget, gpointer data);
typedef void (*user_key_press_fcn)( GdkEventKey *event );
typedef void (*user_mouse_pointer_position_fcn)( GdkEventMotion *event );
typedef void (*user_mouse_button_press_fcn)( GdkEventButton *event );
typedef void (*get_image_map_text)( string name, vector<string> *info );

enum file_extension{
	PDF,
	PNG,
	SVG,
	PS
};

class image_map;
class style_button;
class bgl_stats;

class gtk_win
{
	string name, version;
	
	cairo_surface_t* cs;
	cairo_t* cr;
	
	GtkWidget *mainwin;		// the new windows 
	GtkWidget *canvas;		// a new canvas
	GtkWidget *hbox;		// 
	GtkWidget *vbox;		// 
	GtkWidget *sidepane;
	GtkWidget *statusbar;
	GtkWidget *omnibox;
	GtkWidget *searchentry, *searchbox;
	#ifdef GTK+3
	//GtkWidget *overlay;
	//GtkWidget *query_grid;
	#endif
	vector<style_button*> sidepane_buttons;
	
	int win_current_width, win_current_height;
	int canvas_width, canvas_height;
	int stbar_height, sdp_width, top_gui_height;
	draw_gtk drawscreen;
	double tx, ty, scale;		//as of now are dummy variables, serve no special purpose
	double xleft, xright, ytop, ybottom;
	double saved_xleft, saved_xright, saved_ytop, saved_ybottom;
	double xmult, ymult;
	double angle, user_tx, user_ty;
	
	bool window_zoom_mode_on, in_window_zoom_mode;
	double z_xleft, z_xright, z_ytop, z_ybottom;
	int worldx, worldy;
	double zoom_in_factor, zoom_out_factor;
	double zoom_in_value, zoom_out_value;
	double scroll_zoom_in_multiplier, scroll_zoom_out_multiplier;
	double translate_u_factor, translate_d_factor, translate_l_factor, translate_r_factor;
	double translate_u_value, translate_d_value, translate_l_value, translate_r_value;
	
	//for font description using pangocairo
	char *font_desc;
	bool enable_pango_markup;
	
	// for text-balloon-coordinates of bounding box
	double bx1, bx2, by1, by2;
	
	// for canvas drag-loookup
	double start_x, start_y, start_xleft, start_xright, start_ytop, start_ybottom;
	bool canvas_drag_mode;
	
	// GtkSpin buttons for preferences
	GtkWidget *zoomin_spinner, *zoomout_spinner;
	GtkWidget *up_spinner, *down_spinner, *left_spinner, *right_spinner;
	GtkWidget *hshear_spinner, *vshear_spinner;
	GtkWidget *translate_lock_button, *shear_lock_button;
	GtkAdjustment *adj;
	int hshear_spinner_handler_id, vshear_spinner_handler_id; 
	int up_spinner_handler_id, down_spinner_handler_id, left_spinner_handler_id, right_spinner_handler_id;
	
	
	#ifdef TOOLBAR
	GtkWidget *toolbar;
	GtkToolItem *save;
	GtkToolItem *about;
	GtkToolItem *preferences;
	GtkToolItem *zoom_in;
	GtkToolItem *zoom_out;
	GtkToolItem *zoom_fit;
	GtkToolItem *zoom_window;
	GtkToolItem *refresh;
	GtkToolItem *ti_rotate_clockwise;
	GtkToolItem *ti_rotate_anticlockwise;
	GtkToolItem *ti_imagemaps;
	GtkToolItem *ti_fullscreen;
	GtkToolItem *ti_exitfullscreen;
	GtkToolItem *exit;
	GtkToolItem *ti_sep1, *ti_sep2, *ti_sep3, *ti_sep4, *ti_sep5;
	GtkToolItem *ti_hshearplus;
	GtkToolItem *ti_hshearminus;
	GtkToolItem *ti_vshearplus;
	GtkToolItem *ti_vshearminus;
	bool toolbar_created;
	#endif
	
	#ifdef MENU_BAR
	GtkWidget *menubar;
	
	// file menu : (refresh, save as, quit)
	GtkWidget *filemenu;
	GtkWidget *file;
	GtkWidget *mi_refresh;
	GtkWidget *mi_saveas;
	GtkWidget *mi_property;
	GtkWidget *mi_stats;
	GtkWidget *mi_quit;
	GtkWidget *mi_sep;
	
	// edit menu
	GtkWidget *editmenu;
	GtkWidget *edit;
	GtkWidget *mi_preferences;
	
	// view menu : (window, zoom-in, zoom-out, zoom-fit)
	GtkWidget *viewmenu;
	GtkWidget *view;
	GtkWidget *mi_window;
	GtkWidget *mi_zoomin;
	GtkWidget *mi_zoomout;
	GtkWidget *mi_zoomfit;
	GtkWidget *mi_sep2;
	GtkWidget *mi_fullscreen;
	GtkWidget *tog_maps;

	// Image menu : ( translate [up, down, left, right],
	// shear [horz+, horz-, vert+, vert-], rotate[+90, -90])
	GtkWidget *imagemenu;
	GtkWidget *image;
	GtkWidget *mi_translateUp;
	GtkWidget *mi_translateDown;
	GtkWidget *mi_translateLeft;
	GtkWidget *mi_translateRight;
	GtkWidget *mi_sep3;
	GtkWidget *mi_shearHplus;
	GtkWidget *mi_shearHminus;
	GtkWidget *mi_shearVplus;
	GtkWidget *mi_shearVminus;
	GtkWidget *mi_sep4;
	GtkWidget *mi_rotateClockwise;
	GtkWidget *mi_rotateantiClockwise;
	
	// Help menu
	GtkWidget *helpmenu;
	GtkWidget *help;
	GtkWidget *mi_about;
	
	GtkAccelGroup *accel_group;
	#endif
	
	void focus();
	
	void translate_up();
	void translate_down();
	void translate_left();
	void translate_right();
	
	void update_transform();
	
	//UI display
	void update_statusbar_msg();
	
	#ifdef TOOLBAR
	void create_toolbar();
	#endif
	
	#ifdef MENU_BAR
	void create_custom_menu();
	#endif
	
	//functions for saving operation
	file_extension type;
	void save_as_pdf( char* filename, bool appendtype );
	void save_as_png( char* filename, bool appendtype );
	void save_as_svg( char* filename, bool appendtype );
	void save_as_ps ( char* filename, bool appendtype );
	
	user_key_press_fcn user_key_press_method;
	bool user_key_press_active;
	user_mouse_pointer_position_fcn user_mouse_pos_method;
	bool user_mouse_position_active;
	user_mouse_button_press_fcn user_mouse_button_press_method;
	bool user_mouse_press_active;
	
	// the main function that paints the canvas
	void paint_canvas(cairo_t *cr);
	
	// for image maps
	bool highlight_image_maps;
	void image_maps_store_transformed_coordinates();
	void image_maps_highlight();
	void image_maps_show_text_balloon( cairo_t *cr );
	
	// for fullscreen
	bool make_fullscreen;
	
	// the maximum slope, beyond which the the slope is considerd to be infinity.
	// the minimum slope, beyond which the the slope is considerd to be zero.
	double max_slope, min_slope;

	// shear related variables
	double horizontal_shear, delta_horz_shear, hshear_angle;
	double vertical_shear, delta_vert_shear, vshear_angle;
	
	// variables and functions to implement rotation
	int angle_multiplier ;
	double rx_translate , ry_translate;
	void set_xy_translate_and_scale();
	
	//varibles for statistics
	bgl_stats *stats;
	
	// coordinates passed are in canvas coordinates, now only used as an internal function
	void draw_separator( cairo_t* cr, double sep_x1, double sep_y1, double sep_x2, double height, double *rgb_sep);
	
	/* accessor functions for internal values, 
	   n is the actual desired zoom, say 1.1, 0.98 etc */
	void set_zoom_in_factor(double n);
	void set_zoom_out_factor(double n);
	void set_scroll_zoom_in_factor(double n);
	void set_scroll_zoom_out_factor(double n);
	void set_translate_up_factor(double n);
	void set_translate_down_factor(double n);
	void set_translate_left_factor(double n);
	void set_translate_right_factor(double n);
	void set_horizontal_shear_delta(double angle);
	void set_vertical_shear_delta(double angle);
	
	/* ************************************************************************************************** */
	public:
	// image maps
	vector<image_map*> image_maps;
	void toggle_image_map_visibility( ); //requires restricted access in future
	
	// for fullscreen
	void toggle_fullscreen( ); //requires restricted access in future
	
	// to check for window state event
	void chk_mainwin_state_event(GdkEventWindowState *event); //requires restricted access in future
	
	//constructor
	gtk_win( draw_gtk _drawscreen, int onset_width, int onset_height );

	//graphics related functions
	void init_world( double x1, double y1, double x2, double y2 );
	void init_graphics( char* windowtitle );
	void redraw();
	void focus_on_area( double _xleft, double _ytop, double _xright, 
		                double _ybottom );
	void restore_to_onset_view( bool remove_transforms );
	
	// utility to check if the bounding box of current shape is in viewport, used
	// for clipping
	bool rect_off_screen( cairo_t* cr, double x1, double y1, double x2, double y2 );
	
	// coordinate transforms
	double user2win_x( double user_x );
	double user2win_y( double user_y );
	double win2user_x( double world_x );
	double win2user_y( double world_y );
	double user2context_x( double user_x );
	double user2context_y( double user_y );
	void translate_coordinates( cairo_t* cr, double _tx, double _ty );
	void transform_user_to_window(cairo_t* cr, double &x, double &y);
	
	//functions for UI display
	void update_statusbar_message_with( char* msg );
	void add_button_to_sidepane( style_button *new_button );
	void open_preferences_dialouge(); //requires restricted access in future

	double get_zoom_in_factor();
	double get_zoom_out_factor();
	double get_scroll_zoom_in_factor();
	double get_scroll_zoom_out_factor();
	double get_translate_up_factor();
	double get_translate_down_factor();
	double get_translate_left_factor();
	double get_translate_right_factor();
	
	double get_max_slope();
	double get_min_slope();
	double get_horizontal_shear();
	double get_vertical_shear();

	// The drawing functions: aids in drawing, could have been written
	// in a way that the cairo contetx need not be passed everytime, but
	// would have involved a lot of extra coding to ensure
	// consistency.
	void clearscreen(cairo_t* cr);
	void setbgcolor(cairo_t*cr, double red, double green, double blue);
	void setcolor(cairo_t* cr, double red, double green, double blue, double alpha);
	void setradialgradient(cairo_t* cr, double cx0, double cy0, double r0, double cx1, double cy1, double r1);
	void setlineargradient(cairo_t* cr, double cx0, double cy0, double cx1, double cy1);
	void setlinestyle(cairo_t* cr, int linestyle);
	void setlinestyle(cairo_t* cr, int num_dashes, double *dashes);
	void setlinewidth(cairo_t* cr, double linewidth);
	void drawline(cairo_t* cr, double x1, double y1, double x2, double y2);
	void pango_markup_enable(bool enable);
	void setfontsize(cairo_t* cr,int pointsize);
	void setfontface( cairo_t* cr, char* fontface, cairo_font_slant_t slant, cairo_font_weight_t weight);
	void drawtext(cairo_t* cr, double xc, double yc, char *text, double boundx);
	void setpangofontdesc( char *_font_desc );
	void drawpangotext(cairo_t* cr, double xc, double yc, char *text, double &width, double &height, double boundx);
	void drawarc(cairo_t* cr, double xcen, double ycen, double rad, double startang, double angextent);
	void fillarc(cairo_t* cr, double xcen, double ycen, double rad, double startang, double angextent);
	void drawellipticarc(cairo_t* cr, double xcen, double ycen, double radx, double rady, double startang, double angextent);
	void fillellipticarc(cairo_t* cr, double xcen, double ycen, double radx, double rady, double startang, double angextent);
	void drawrect(cairo_t* cr, double x1, double y1, double x2, double y2);
	void fillrect(cairo_t* cr, double x1, double y1, double x2, double y2);
	void drawpolygon(cairo_t* cr, vector<double> &x, vector<double> &y);
	void fillpolygon(cairo_t* cr, vector<double> &x, vector<double> &y);	
	bool drawpolypath(cairo_t* cr, vector<double> &x, vector<double> &y);
	void drawroundedrect(cairo_t* cr, double x1, double y1, double x2, double y2, double xarc, double yarc);
	void fillroundedrect(cairo_t* cr, double x1, double y1, double x2, double y2, double xarc, double yarc);
	bool drawroundedrectpath(cairo_t* cr, double x1, double y1, double x2, double y2, double xarc, double yarc);
	void drawtextballoon(cairo_t* cr, double user_x1, double user_y1,  double tolerance, vector<string>* arr, int fontsize, 
	                     char* fontface, double *rgb_border, double *rgba_fill,  double *rgb_text);
	
	// functions for saving in a particular format
	void save_as();
	void save_as_file_type(GtkWidget *widget); //requires restricted access in future
	
	// Public functions for registering user callbacks for mouse motion, mouse button press 
	// and keyboard press
	void activate_user_key_press_input( user_key_press_fcn method );
	void deactivate_user_key_press_input();
	void activate_user_mouse_pointer_position_input( user_mouse_pointer_position_fcn method );
	void deactivate_user_mouse_pointer_position_input();
	void activate_user_mouse_button_press_input( user_mouse_button_press_fcn method );
	void deactivate_user_mouse_button_press_input();
	
	// utility functions
	void line2arr (char* str, vector<string>* arr, char* tokenizer); 
	
	// public accessibility of restricted functions/variables by accessibility-key
	void turn_on_window_zoom( ); //requires restricted access in future
	void update_statusbar( ); //requires restricted access in future
	
	//signal/event callbacks, restricted access
	#ifdef GTK+3
		gboolean mainwin_draw_event (cairo_t *cr); //requires restricted access in future
		gboolean canvas_draw_event  (cairo_t *cr); //requires restricted access in future
	#else
		gboolean mainwin_expose_event (GdkEventExpose *event); //requires restricted access in future
		gboolean canvas_expose_event  (GdkEventExpose *event); //requires restricted access in future
	#endif
	void     mainwin_scroll_event (GdkEventScroll* event); //requires restricted access in future
	gboolean mainwin_configure_event (GdkEventConfigure *event); //requires restricted access in future
	gboolean canvas_configure_event  (GdkEventConfigure *event); //requires restricted access in future
	gboolean mainwin_key_press_event (GdkEventKey *event); //requires restricted access in future
	gboolean mainwin_mouse_move_event(GdkEventMotion *event); //requires restricted access in future
	gboolean mainwin_mouse_button_event (GdkEventButton *event); //requires restricted access in future
	
	//toolbar functionalities, restricted access
	void toolbar_zoom_in( ); //requires restricted access in future
	void toolbar_zoom_out( ); //requires restricted access in future
	void show_about_dialouge( ); //requires restricted access in future
	
	// shear and rotate transforms
	void increase_horizontal_shear( ); //requires restricted access in future
	void decrease_horizontal_shear( ); //requires restricted access in future
	void increase_vertical_shear( ); //requires restricted access in future
	void decrease_vertical_shear( ); //requires restricted access in future
	void rotate_clockwise( ); //requires restricted access in future
	void rotate_anticlockwise( ); //requires restricted access in future
	
	// show properties and stats
	void show_properties_dialog( ); //requires restricted access in future
	void show_statistics_dialog( ); //requires restricted access in future
	
	// update preferences
	void update_values_for_zoom_translate_shear( ); //requires restricted access in future
	void toggle_translate_lock_state( ); //requires restricted access in future
	void lock_translate_spinners( GtkSpinButton *spinbutton ); //requires restricted access in future
	void toggle_shear_lock_state( ); //requires restricted access in future
	void lock_shear_spinners( GtkSpinButton *spinbutton ); //requires restricted access in future
};

class image_map
{
	public:
	// name of the image map, used as a key to fetch text to show
	// from the filkec codes.txt 
	string name;
	
	// the coordinates in user coordinate system where the text 
	// balloon points
	double event_x, event_y;
	
	// the text balloon is shown only when the square between
	// [event_x-tolerance, event_y-tolerance] and 
	// [event_x+tolerance, event_y+tolerance] is in the viewport 
	double tolerance;
	
	// the actual un-transformed coordinates of bouding box of 
	// this image_map in user coordinates. Defines the extent 
	// of the image-map.
	double x1, y1, x2, y2;

	// The transfomred coordinates of the bounding box of this 
	// image map in window coordinate system under the current 
	// context of the canvas. Updated for each refresh.  
	double x1t, y1t, x2t, y2t, x3t, y3t, x4t, y4t;
	
	// whether to show the text balloon corresponding to this image
	// map or not.
	bool to_show_info;
	
	// the function to get image map text
	get_image_map_text get_image_map_text_fcn;
	
	//constructor
	image_map( string _name, double _x1, double _y1, double _x2, double _y2, double tol, 
	get_image_map_text _get_image_map_text_fcn );
	
	
	// accepts the window-coordinates to decide if the text balloon of this image
	// map has to be shown for the corresponding mouse button-press. 
	bool try_set_event_coordinates( double ex, double ey, gtk_win *_application );
	
	// find if the event point is inside the bouding box of this image map
	// using the ray propapgation algorithm.
	bool in_bouding_box( double ex, double ey, gtk_win *_application );
};

class style_button
{
	GtkWidget *button;	
	GtkWidget *button_label;
	string button_text;
	button_fcn act_on_button_press;
	string desc;
	
	public:
	gtk_win *application;
	
	style_button( string _button_text, button_fcn _act_on_button_press, gtk_win *_application );
	
	void set_desc(string _desc);
	void act_on_enter_button_signal( ); //requires restricted access in future
	void act_on_leave_button_signal( ); //requires restricted access in future
	GtkWidget* get_widget( ); //requires restricted access in future
};

class bgl_stats
{
	public:
	gtk_win *application;
	int count_f_ellipses, count_f_arcs, count_f_rects, count_f_poly, count_f_roundrect;
	int count_d_ellipses, count_d_arcs, count_d_rects, count_d_poly, count_d_roundrect;
	int count_lines, count_text, count_textballoons;
	
	int count_redraw, count_saved_to_file;
	int count_mousebuttonpress_event, count_keypress_event, count_mousemove_event;
	int count_scroll_events, count_configure_event, count_expose_event;
	
	clock_t draw_start, draw_end;
	double cpu_time_used;
	
	int VmPeak, VmSize, VmLck, VmHWM, VmRSS, VmData, VmStk, VmExe, VmLib, VmPTE, Threads;
	//string process_status;
	time_t start_time, current_time;
	double num_cores;
	
	bgl_stats( gtk_win *_application );
	void reset_counts();
	void draw_start_fcn();
	void draw_end_fcn();
	void get_shape_counts( char *buffer );
	void get_event_count( char *buffer );
	void get_resource_usage( char *buffer );
};
#endif

