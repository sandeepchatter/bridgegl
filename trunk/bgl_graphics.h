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

/*! \file bgl_graphics.h
	The header file for the main class.
*/

/*! debug mode, verbose output */
//#define DEBUG

/*! Display a bouding box around any shape drawn with the help of gtk_win class
 functions  */
//#define BOUNDING_BOX

/*! Stylize side pane buttons when compiled with GTK+2. CSS is used to 
stylize the UI in GTK+3.  
*/
#define STYLE_BUTTONS

/*! Follows the mouse pointer and updates the statubar with its location*/
//#define FOLLOW_MOUSE_POINTER

/*!Scales text as user zooms. Without it, the text behaviour is undefined
under transforms.*/
#define SCALE_TEXT

/*! Detemines whether the toolbar should be attcahed to the UI or not. Note that
toolbar support is experimental
*/
//#define TOOLBAR

/*! Detemines whether the toolbar should be attcahed to the UI or not. Note that
most of the functionality is defined through menus and toolbars, So define at
least one of macros in header file, preferably the MENU_BAR macro.  
*/
#define MENU_BAR

/*! If GTK+3 is being used, this pre-processor decides if CSS will be used to
customize UI. For native look and feel, don't use CSS.
*/ 
//#define APPLY_CSS

/*! All system dialogs (not the main window) are undecorated if the following
macro is defined
*/
//#define UNDECORATED_DIALOG


#define CONV_DEG_TO_RAD 0.01745329252  // pi/180
#define MAX_SHEAR 3.73205080756        // tan(75)
#define PI 3.14159265

using namespace std;

/** @name Function pointers
*   These function pointer definitions enable the main class gtk_win to carry
    out a number of user operations, such as registering callbacks, generating
    image maps texts on the fly, performing the drawing as instructed by the 
    user etc.
*/
//@{
/*! 
	This is the function pointer for the main drawing function, that the user implements
	to render all his/her drawings on the output device. A function of this has two input argumnets, the widget
	on which the output is rendered (usually the canvas) and a cairo context through which all the drawing is done
	(this context is subsequently needed by all the drawing functions).
*/
typedef void (*draw_gtk)(GtkWidget *widget, cairo_t *cr);
/*! 
	Recall that the user can alter the UI and add custom buttons to the side pane to do the application specific tasks.
	This function pointer is used for callbacks corresponsing to the side-pane buttons. 
*/
typedef void (*button_fcn)(GtkWidget *widget, gpointer data);
/*!
	This function pointer is used as user-callback for key press events. 
*/
typedef void (*user_key_press_fcn)( GdkEventKey *event );
/*!
	This function pointer is used as user-callback for mouse move events. Note that tracking mouse movement
	continuously through the application can affect its response time.
*/
typedef void (*user_mouse_pointer_position_fcn)( GdkEventMotion *event );
/*!
	This function pointer is used as user-callback for all mouse button press events. It can detect single-click and
	double-click events and differentiate between left, middle and right mouse button presses. 
*/
typedef void (*user_mouse_button_press_fcn)( GdkEventButton *event );
/*!
	This function pointer is used to generate description text of an image-map on the fly as the user clicks
	on it. The image maps is identified by its name and info is a vector of strings, such that each string is
	printed on a new line. 
*/
typedef void (*get_image_map_text)( string name, vector<string> *info );
//@}

enum file_extension{
	PDF,
	PNG,
	SVG,
	PS
};

enum direction{
	NORTH,
	EAST,
	SOUTH,
	WEST
};

/*! \class image_map
	\brief     Based on the concept of HTML image maps

 *  \details   An image-map in BridgeGL is based on the same concept as the HTML image maps:
	an area on canvas is defined to be clickable by the user. When clicked, it
	can perform many actions such as fire up an event, execute a callback function
	or just show some relevant information. As of now, only rectangular image maps
	are supported, and a click on an user-defined image map shows a text balloon,
	with text as provided by a user-defined funtion of type get_image_map_text(). An image map in this sense
	can be used to highlight otherwise hidden information from the drawing and make the user-experience interactive.
	For example, during placement, the user can click on a logic-block to have its
	exact coordinates displayed in a text-balloon, rather than guessing its
	cooridnates only from the canvas drawing. The image maps can be highlighted
	(or hidden) using the toolbar and menubar options, or by pressing the Ctrl+M key.
*  \author    Sandeep Chatterjee
*/
class image_map;

/*! \class style_button
	\brief     Used for styling sidepane buttons. Works only with GTK+2.0.

 *  \details   This is a wrapper class that is used to style GtkButtons attached
    to the sidepane. It uses GtkStyle, which has been deprecated in GTK+3.0 (CSS
    is used instead).
 *  \author    Sandeep Chatterjee
 */
class style_button;

/*! \class bgl_stats
	\brief     Used for collecting statistics for class gtk_win

 *  \details   This is an utility class, that maintains a record of how many shapes
    are being drawn in the current viewport. On linux, the functions from this class
    also read the proc file for the main-process
    and output various metrics, such as memory usage, CPU usage, event counts etc.
 *  \author    Sandeep Chatterjee
 */
class bgl_stats;

/*! \class     gtk_win
 *  \brief     The main class that renders the canvas and handles the GUI.
 *  \details   This is the main class that handles all the GUI and uses cairo to
               render 2D graphics on the canvas.  
 *  \author    Sandeep Chatterjee
 *  \version   1.6
 *  \date      2013-2014
 *  \pre       Requires GTK+2 or GTK+3 development libraries
 *  \bug       The application crashes sometimes after saving a file to the disk. There
               is probably some error in the freeing the filename.
 *  \bug       In some systems, the transform from canvas coordinates to user coordinates
               is erroneous: a y-offset is added to the user coordinate system. However, this
               error is removed after aby state-change event occurs. Also, the error effects only events
               (such as location of mouse_button_press, mouse_button_drag etc) but not the rendering itself.
 *  \bug       The 'translate *' menu items under 'Image' menu do not work.
 *  \bug       Using CSS to style bridgeGL GUI is inconsistent across platforms.
 *  \todo      A search bar at top right. Its intended
               functionality is to open a pop-menu provinding menu-items that matches the
               desription being typed, kind of like a start-menu on windows.
 *  \todo      Shift the callbacks for mouse button press from mainwindow to the canvas itself. This should
               avoid the erroneous y-offset being added to click events.
 *  \todo      As of now, user has to redraw the whole canvas every-time. Provide a
		       mechanism by which he/she can redraw only a part of the canvas.
 *  \todo      Provide a second function pointer to user for drawing, where he/she can 
     		   pass a (void*) pointer with parameter values to used while drawing,
      		   instead of using global variables.
 * \todo       Some functions from gtk_win require restrcited access in future, so that
      		   they are not accessibile from outside the file bgl_graphics.cpp.
 */

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
	bool drawpolypath(cairo_t* cr, vector<double> &x, vector<double> &y);
	bool drawroundedrectpath(cairo_t* cr, double x1, double y1, double x2, double y2, double xarc, double yarc);
	
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
	vector<image_map*> image_maps;  /*!< A vector of image maps defined by the user. Future versions
										should implement this as a tree.*/
	void toggle_image_map_visibility( ); //requires restricted access in future
	
	// for fullscreen
	void toggle_fullscreen( ); //requires restricted access in future
	
	/*! @brief Constructor for the main class
	
	    The constructor for the main class, that initializes the GUI and creates a canvas
	    to render the user drawing.
	    
	    @param[in] _drawscreen  A pointer to the main user-drawing function. The function pointed by _drawscreen
	    accepts two arguments, a pointer to the canvas and a cairo context, which are automatically passed to it
	    whenever it is called.
	    
	    @param[in] onset_width  The minimum width of the main top-level window.
	    
	    @param[in] onset_height The minimum height of the main top-level window.
	*/
	gtk_win( draw_gtk _drawscreen, int onset_width, int onset_height );

	/** @name GUI and Drawing related
	 *  Funtions to initialize the dimensions of user-drawing world and the create the GUI.
	 */
	//@{
	/*! @brief Initialize user-world
	
	    Sets up the user-coordinates to vary from (x1,y1) on top-left to (x2,y2) on bottom-right and thus defines
	    the dimenions of user-world. 
	    
	    @warning This function has to be called before calling init_graphics(char* windowtitle).
	    
	    @param[in] x1 x-coordinate of top-left corner of bounding box.
	    @param[in] y1 y-coordinate of top-left corner of bounding box.
	    @param[in] x2 x-coordinate of bottom-right corner of bounding box.
	    @param[in] y2 y-coordinate of bottom-right corner of bounding box.
	*/
	void init_world( double x1, double y1, double x2, double y2 );
	
	/*! @brief Creates the Graphical User Interface
	
	    This is the main function, that prepares the User Interface (UI), draws on the canvas as per
	    the users request, registers callbacks and passes control to the main event loop that waits for
	    user interaction.
	    
	    @pre This function should only be called after calling init_world(double x1, double y1, double x2, double y2).
	    
	    @param[in] windowtitle the title of the top level window, to be displayed as 'BridgeGL: ...'
	*/
	void init_graphics( char* windowtitle );
	/*! @brief Refreshes the drawing
	
	    Refreshes the drawing on the output device. It can also be called by pressing F5 on keyboard.
	    
	    @warning This is the only recommended way for updating the drawing area (in order to show progress or reflect
	    some changes etc). Calling the draw_gtk() function directly may lead to unknown behaviour.
	*/
	void redraw();
	//@}
	
	
	// utility to check if the bounding box of current shape is in viewport, used
	// for clipping
	bool rect_off_screen( cairo_t* cr, double x1, double y1, double x2, double y2 );
	void focus_on_area( double _xleft, double _ytop, double _xright, double _ybottom );
	void restore_to_onset_view( bool remove_transforms );
	
	
	/** @name Functions for coordinate transformations
	 *  BridgeGL gives user the freedom to choose his/her own coordinate space. As such, the following
	    functions are transform points from one space to another. The coordinate transforms are very 
	    important as many operations, such as panning, zooming, rotating etc depend on this.
	*/
	//@{
	/*! @brief User-to-device coordinate transform

		Converts the x-coordinate of a point from user space (for the current viewport configuration)
		to device (canvas) coordinate space. Used mainly for translating the viewport for panning and
		zooming operations. Its mainly for internal use.

		@param[in] user_x the x-coordinate of a point in user coordinate system
		@returns a double value, the corresponding x-coordinate of the point on the device
	*/
	double user2win_x( double user_x );
	/*! @brief User-to-device coordinate transform

		Converts the y-coordinate of a point from user space (for the current viewport configuration)
		to device (canvas) coordinate space. Used mainly for translating the viewport for panning and
		zooming operations. Its mainly for internal use.

		@param[in] user_y the y-coordinate of a point in user coordinate system
		@returns a double value, the corresponding y-coordinate of the point on the device
	*/
	double user2win_y( double user_y );
	/*! @brief Device-to-user coordinate transform

		Converts the x-coordinate of a point from device (window) space to user coordinate space for the current
		viewport configuration. Used mainly for detecting the location of clicks and mouse move events.

		@param[in] world_x the x-coordinate of a point in device (window) coordinate system
		@returns a double value, the corresponding x-coordinate of the point in user coordinate system
	*/
	double win2user_x( double world_x );
	/*! @brief Device-to-user coordinate transform

		Converts the y-coordinate of a point from device (window) space to user coordinate space for the current
		viewport configuration. Used mainly for detecting the location of clicks and mouse move events.

		@param[in] world_y the y-coordinate of a point in device (window) coordinate system
		@returns a double value, the corresponding y-coordinate of the point in user coordinate system
	*/
	double win2user_y( double world_y );
	/*! @brief User-to-Cairo context coordinate transform

		Converts the x-coordinate of a point from user coordinate space to cairo_t space. Used mainly
		for drawing on the canvas. 

		@param[in] user_x the x-coordinate of a point in user coordinate system
		@returns a double value, the corresponding x-coordinate of the point in cairo_t coordinate system
	*/
	double user2context_x( double user_x );
	/*! @brief User-to-Cairo context coordinate transform

		Converts the y-coordinate of a point from user coordinate space to cairo_t space. Used mainly
		for drawing on the canvas. 

		@param[in] user_y the y-coordinate of a point in user coordinate system
		@returns a double value, the corresponding y-coordinate of the point in cairo_t coordinate system
	*/
	double user2context_y( double user_y );
	/*! @brief Translate user coordinate system

		Translates the coordinates of the cairo context by (_tx,_ty). It's essentially a wrapper function,
		that translates coordinates in such that a way that panning and zooming don't distort the drawing.

		@param[in] cr  a pointer to a cairo context
		@param[in] _tx the amount, in user space, by which x-coordinate has to be translated
		@param[in] _ty the amount, in user space, by which y-coordinate has to be translated
	*/
	void translate_coordinates( cairo_t* cr, double _tx, double _ty );
	/*! @brief Transforms user coordinate to window coordinate

		Transforms the coordinates of a point from user coordinate space to window space. It is a
		convenice function designed to be used by image maps.

		@param[in] cr    a pointer to a cairo context
		@param[in,out] x the x-coordinate of a point in user coordinate system
		@param[in,out] y the y-coordinate of a point in user coordinate system
	*/
	void transform_user_to_window(cairo_t* cr, double &x, double &y);
	//@}
	
	
	/** @name Functions for UI customization
	 *  These functions allow the user to customize the user interface to suit their application needs. 
	*/
	//@{
	/*! @brief Update statusbar message

		The top level window has a statusbar at the bottom, which can be used provide meaningful feedback. The user
		can use this function to set the current message to be displayed. 

		@param[in] msg the message to be diaplyed on the statusbar.
	*/
	void update_statusbar_message_with( char* msg );
	/*! @brief Adds buttons to the side pane

		Adds a new button to the sidepane. 

		@param[in] new_button the new style_button() to be added to the sidepane 
	*/
	void add_button_to_sidepane( style_button *new_button );
	//@}
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

	/** @name Functions for drawing shapes on canvas
	 *  These functions aid in drawing primitive 2D shapes, and can act as building blocks
	 	for more complex shapes and patterns. Using these, the user can draw lines, arcs, ellipses,
	 	rectangles, rounded rectangles and polygons. Using polygon, the user can create all 2D shapes such as
	 	rhombus, parallelogram, traingle etc. All shapes can be drawn as an outline (using the
	 	corresponding 'draw_' prefixed functions) or as a filled shape (using the corresponding 
	 	'fill_' prefixed function). For any shape, the width, style and color of outline (or color of the fill)
	 	is determined by the values previously set by the user using functions
		setlinewidth(), setlinestyle() and setcolor(), respectively. All inputs are in user
		coordinate system, unless specified otherwise.
	*/
	//@{
	/*! @brief Clear the canvas before drawing

		Clears the canvas before any drawing is done. It is recommended that this function be called once
		before any drawing is done.

		@param[in] cr    a pointer to a cairo context
	*/
	void clearscreen(cairo_t* cr);
	/*! @brief Sets the background color

		Alternative to calling the clearscreen() function, paints the background in desired color. 
		
		@warning If used, this function should be called before any drawing is done.  

		@param[in] cr     a pointer to a cairo context
		@param[in] red    red component of the background color, range[0,1]
		@param[in] green  green component of the background color, range[0,1]
		@param[in] blue   blue component of the background color, range[0,1]
	*/
	void setbgcolor(cairo_t*cr, double red, double green, double blue);
	/*! @brief Draws a line on the output device

		Draws a line between user coordinates (x1,y1) and (x2,y2). 

		@param[in] cr   a pointer to a cairo context
		@param[in] x1   the x-coordinate of the first endpoint of the line 
		@param[in] y1   the y-coordinate of the first endpoint of the line 
		@param[in] x2   the x-coordinate of the second endpoint of the line 
		@param[in] y2   the y-coordinate of the second endpoint of the line 
	*/
	void drawline(cairo_t* cr, double x1, double y1, double x2, double y2);
	/*! @brief Draws an arc on the output device

		Draws the outline of a circular arc on the output device. The angle is positive and increasing
		when going clockwise from positive x-axis.

		@param[in] cr   a pointer to a cairo context
		@param[in] xcen the x-coordinate of the centre of the arc
		@param[in] ycen the y-coordinate of the centre of the arc
		@param[in] rad  the radius of the circular arc 
		@param[in] startang  the starting angle of the arc, in degrees
		@param[in] angextent the angular extent to which the arc is to be drawn, in degrees
	*/
	void drawarc(cairo_t* cr, double xcen, double ycen, double rad, double startang, double angextent);
	/*! @brief Draws a filled arc on the output device

		Draws a filled circular arc on the output device. The angle is positive and increasing when
		going clockwise from positive x-axis.

		@param[in] cr   a pointer to a cairo context
		@param[in] xcen the x-coordinate of the centre of the arc
		@param[in] ycen the y-coordinate of the centre of the arc
		@param[in] rad  the radius of the circular arc 
		@param[in] startang  the starting angle of the arc, in degrees
		@param[in] angextent the angular extent to which the arc is to be drawn, in degrees
	*/
	void fillarc(cairo_t* cr, double xcen, double ycen, double rad, double startang, double angextent);
	/*! @brief Draws an elliptic arc on the output device

		Draws the outline of an elliptic arc on the output device. The angle is positive and increasing
		when going clockwise from positive x-axis.

		@param[in] cr   a pointer to a cairo context
		@param[in] xcen the x-coordinate of the centre of the arc
		@param[in] ycen the y-coordinate of the centre of the arc
		@param[in] radx  the radius of the arc along x-axis
		@param[in] rady  the radius of the arc along y-axis 
		@param[in] startang  the starting angle of the arc, in degrees
		@param[in] angextent the angular extent to which the arc is to be drawn, in degrees
	*/
	void drawellipticarc(cairo_t* cr, double xcen, double ycen, double radx, double rady, double startang, double angextent);
	/*! @brief Draws a filled elliptic arc on the output device

		Draws a filled elliptic arc on the output device. The angle is positive and increasing
		when going clockwise from positive x-axis.

		@param[in] cr   a pointer to a cairo context
		@param[in] xcen the x-coordinate of the centre of the arc
		@param[in] ycen the y-coordinate of the centre of the arc
		@param[in] radx  the radius of the arc along x-axis
		@param[in] rady  the radius of the arc along y-axis 
		@param[in] startang  the starting angle of the arc, in degrees
		@param[in] angextent the angular extent to which the arc is to be drawn, in degrees
	*/
	void fillellipticarc(cairo_t* cr, double xcen, double ycen, double radx, double rady, double startang, double angextent);
	/*! @brief Draws an outline of a rectangle on the output device

		Draws an outline of a rectangle on the output device between diagonally opposite points
		(x1,y1) and (x2,y2).

		@param[in] cr  a pointer to a cairo context
		@param[in] x1  the x-coordinate of the first point
		@param[in] y1  the y-coordinate of the first point
		@param[in] x2  the x-coordinate of the second point
		@param[in] y2  the y-coordinate of the second point
	*/
	void drawrect(cairo_t* cr, double x1, double y1, double x2, double y2);
	/*! @brief Draws a filled rectangle on the output device

		Draws a filled rectangle on the output device between diagonally opposite points
		(x1,y1) and (x2,y2).

		@param[in] cr  a pointer to a cairo context
		@param[in] x1  the x-coordinate of the first point
		@param[in] y1  the y-coordinate of the first point
		@param[in] x2  the x-coordinate of the second point
		@param[in] y2  the y-coordinate of the second point
	*/
	void fillrect(cairo_t* cr, double x1, double y1, double x2, double y2);
	/*! @brief Draws an ouline of a polygon

		Draws an ouline of a polygon on the output device, with corners as given by vectors x and y (connected by straight lines).
		If vectors x and y are of different sizes, or any one of x and y has a size less than 2, then nothing is drawn on
		the canvas.

		@param[in] cr  a pointer to a cairo context
		@param[in] x   a vector of x-coordinates for the corners of the polygon
		@param[in] y   a vector of y-coordinates for the corners of the polygon
	*/
	void drawpolygon(cairo_t* cr, vector<double> &x, vector<double> &y);
	/*! @brief Draws a filled polygon

		Draws a filled polygon on the output device, with corners as given by vectors x and y (connected by straight lines).
		If vectors x and y are of different sizes, or any one of x and y has a size less than 2, then nothing is drawn on
		the canvas.

		@param[in] cr  a pointer to a cairo context
		@param[in] x   a vector of x-coordinates for the corners of the polygon
		@param[in] y   a vector of y-coordinates for the corners of the polygon
	*/
	void fillpolygon(cairo_t* cr, vector<double> &x, vector<double> &y);	
	/*! @brief Draws an outline of a rectangle with rounded corners

		Draws an outline of a rounded-rectangle on the output device between diagonally opposite points (x1,y1) and
		(x2,y2). 

		@param[in] cr  a pointer to a cairo context
		@param[in] x1  the x-coordinate of the first point
		@param[in] y1  the y-coordinate of the first point
		@param[in] x2  the x-coordinate of the second point
		@param[in] y2  the y-coordinate of the second point
		@param[in] xarc  the radius of the corner arc(s) along x-axis 
		@param[in] yarc  the radius of the corner arc(s) along y-axis 
	*/
	void drawroundedrect(cairo_t* cr, double x1, double y1, double x2, double y2, double xarc, double yarc);
	/*! @brief Draws a filled rectangle with rounded corners

		Draws a filled rounded-rectangle on the output device between diagonally opposite points (x1,y1) and
		(x2,y2). 

		@param[in] cr  a pointer to a cairo context
		@param[in] x1  the x-coordinate of the first point
		@param[in] y1  the y-coordinate of the first point
		@param[in] x2  the x-coordinate of the second point
		@param[in] y2  the y-coordinate of the second point
		@param[in] xarc  the radius of the corner arc(s) along x-axis 
		@param[in] yarc  the radius of the corner arc(s) along y-axis 
	*/
	void fillroundedrect(cairo_t* cr, double x1, double y1, double x2, double y2, double xarc, double yarc);
	/*! @brief Draws a text balloon
		
		\image html text-balloon.png
		\image latex text-balloon.png "A text balloon example"
		
		Draws a text balloon on the output surface. A text balloon can be used to point out the features of
		a drawing area. While using this function, it is the responsibility of the user to ensure that line
		with maximum length falls within the width of canvas at the chosen font size. Also, the text balloon
		does not adapt itself to any transfomration. From version 1.6, text-balloons now support pango
		markups.
		
		@param[in] cr  a pointer to a cairo context
		@param[in] user_x1  the x-coordinates of point where the text-balloon points
		@param[in] user_y1  the y-coordinates of point where the text-balloon points
		@param[in] tolerance  the text balloon is drawn only if the rectangle between (user_x−tolerance, user_y−tolerance)
		and (user_x1+tolerance, user_y1+tolerance) is in viewport
		@param[in] arr  a vector of strings which to be drawn in the text-balloon. Each element is drawn on a new line.
		@param[in] fontsize  the size of fonts to be used while drawing the strings. This will not change the fontsize
		property of the canvas for subsequent texts
		@param[in] fontface  the fontfamily to use while rendering the text in text-balloon. This will not change the
		font-family of the canvas for subsequent texts.
		@param[in] rgb_border a 3x1 1D array of doubles with range[0,1] specifying the amount of red, green and blue colors to be
		used while highlighting the border
		@param[in] rgba_fill a 4x1 1D array of doubles with range[0,1] specifying the amount of red, green, blue colors and
		transparency value to be used while filling the text-balloon
		@param[in] rgb_text a 3x1 1D array of doubles with range[0,1] specifying the amount of red, green and blue colors
		to be used while rendering the simple-text without markups
	*/
	void drawtextballoon(cairo_t* cr, double user_x1, double user_y1,  double tolerance, vector<string>* arr, int fontsize, 
	                     char* fontface, double *rgb_border, double *rgba_fill,  double *rgb_text);
	//@}
	
	/** @name Functions for drawing text on canvas
	 *  These functions are used to draw text on the canvas. The two functions use two different frameworks:
	 	drawtext() uses Cairo Toy Text API whereas drawpangotext() uses PangoCairo rendering. They both have
	 	their pros and cons. It is easier to work with Cairo Toy Text API, but it is definitely not as versatile
	 	and powerful as the PnagoCairo framework. Also, the text bouding box estimates are not accurate for
	 	Cairo Toy Text API, which hinders performance (because some text may be drawn even though not in the
	 	viewport). The recommended way of drawing text is drawpangotext(). 
	*/
	//@{
	/*! @brief Draws text using Cairo Toy Text API

		Draws text on the output device using Cairo Toy Text API. The text attributes, such as font-family,
		size etc are determined by the values previously set by the user by calling functions setfontface()
		and setfontsize(). Note that this function does not support any markups, it draws all the given text
		in a line single.
		
		@warning Newly written code should avoid using this function to draw text.
		
		@param[in] cr  a pointer to a cairo context
		@param[in] xc  the upper left x-coordinate for the bounding box of the text
		@param[in] yc  the upper left y-coordinate for the bounding box of the text
		@param[in] text  the text to be drawn
		@param[in] boundx  the maximum width of the bounding box for drawing the given text. If the width of
		text exceeds boundx, the text is not drawn. By setting boundx to a reasonable value, the cluttering
		due to text when zooming out too much can be avoided. If however, you still wish to display the text,
		set boundx to a very high value.
	*/
	void drawtext(cairo_t* cr, double xc, double yc, char *text, double boundx);
	/*! @brief Draws text using PangoCairo rendering

		Draws text on the output device using PangoCairo rendering. The text attributes, such as font-family,
		size etc are determined by the values previously set by the user by calling functions setpangofontdesc()
		and pango_markup_enable(). This function supports pango markups, the detailed list can be found 
		<a href="https://developer.gnome.org/pango/stable/PangoMarkupFormat.html">here</a>. In addition, it also
		supports &lt;verbatim&gt;..&lt;/verbatim&gt; tag, such anything in between those tags is assumed to be
		preformatted and is drawn as a plain text.
		
		@warning In order for zooming and other transforms to work, setpangofontdesc() should always be used to
		set font description (instead of 'font' attribute of span). So, please dont use the 'size' or 'font' attributes
		in span, use setpangofontdesc() instead.
		
		@param[in] cr  a pointer to a cairo context
		@param[in] xc  the upper left x-coordinate for the bounding box of the text
		@param[in] yc  the upper left y-coordinate for the bounding box of the text
		@param[in] text  the text to be drawn
		@param[in] boundx  the maximum width of the bounding box for drawing the given text. If the width of
		text exceeds boundx, the text is not drawn. By setting boundx to a reasonable value, the cluttering
		due to text when zooming out too much can be avoided. If however, you still wish to display the text,
		set boundx to a very high value.
		@param[out] width  the width of the bounding box of the text segment being drawn
		@param[out] height the height of the bounding box of the text segment being drawn
	*/
	void drawpangotext(cairo_t* cr, double xc, double yc, char *text, double &width, double &height, double boundx);
	//@}
	
	/** @name Functions for setting properties
	 *  Cairo, the underlying drawing engine, is a stateful API, which means that a settings 'sticks' until
	    changed. These settings, such as color (which can be the fill-color or outline-color of a shape, or
	    color a shape etc), font-face, line-width, line-style (solid or dashed) can be changed by calling
	    the corresponding 'set' functions.
	*/
	//@{
	/*! @brief Sets the font family and style to be used to draw text.

		Sets the font size to be used to draw text. This fontsize attribute is only applied to text drawn using 
		drawtext().
		
		@param[in] cr  a pointer to a cairo context
		@param[in] pointsize The size of the font to be drawn. 
	*/
	void setfontsize(cairo_t* cr,int pointsize);
	/*! @brief Sets the font size to be used to draw text.

		Sets the font family and style to be used to draw text. See
		<a href="http://cairographics.org/manual/cairo-text.html#cairo-select-font-face">cairo_select_font_face()</a>
		for more details. These attribute settings are applied only to text drawn using drawtext().
		
		@param[in] cr  a pointer to a cairo context
		@param[in] fontface a character array specifying the font family name. The standard CSS2 generic family names, ("serif", 			"sans-serif", "cursive", "fantasy", "monospace"), are likely to work as expected.
		@param[in] slant an enumerated data-type with the following values: CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_SLANT_OBLIQUE
		@param[in] weight an enumerated data-type with the following values: CAIRO_FONT_WEIGHT_NORMAL, CAIRO_FONT_WEIGHT_BOLD 
	*/
	void setfontface( cairo_t* cr, char* fontface, cairo_font_slant_t slant, cairo_font_weight_t weight);
	/*! @brief Enables/Disables pango markups

		Used to enable/disable pango markups. The detailed list of supported markups can be found 
		<a href="https://developer.gnome.org/pango/stable/PangoMarkupFormat.html">here</a>. This choice
		affects text rendering only for drawpangotext(). 
		
		@warning Don't enable pango markups to display big chunks of texts (say no more than 5K words),
		or else it would slow down your application. 

		@param[in] enable  a flag, which if TRUE, would parse pango markups to display formatted text, and
		if FALSE, would not parse markups and treat the text as plain text.
	*/
	void pango_markup_enable(bool enable);
	/*! @brief Sets the font description for Pango Font.

		Sets the font description for text drawn using drawpangotext(). Different text styles can be specified as:
		<ul>
		<li>Normal or Bold: using 'Sans Normal 16' or 'Sans Bold 26'</li>
		<li>Italic: using 'Helvetica Italic 18' </li>
		<li>Bold+Italic:  use 'Sans Bold Italic 26' </li>
		</ul>
		
		@warning If pango markups are enabled, span attributes have the final say whether as to whether a font
		will be Normal, Bold, Italic or Oblique.
		
		@param[in] _font_desc A font description string, such as "Sans Italic 12". See <a
		href="https://developer.gnome.org/pango/stable/pango-Fonts.html#pango-font-description-from-string">pango_font_description_from_string()</a>
		for a description of the format of the string representation 
	*/
	void setpangofontdesc( char *_font_desc );
	/*! @brief Sets the color of the cairo context. 

		Sets the color of the cairo context as specified by the input arguments. Note that this changes the color
		all subseqeunt drawings.
		
		@param[in] cr  a pointer to a cairo context
		@param[in] red    red component of the background color, range[0,1]
		@param[in] green  green component of the background color, range[0,1]
		@param[in] blue   blue component of the background color, range[0,1]
		@param[in] alpha   the alpha component of the color, range [0,1], 0 is completely transparent and 1
		is completely opaque
	*/
	void setcolor(cairo_t* cr, double red, double green, double blue, double alpha);
	/*! @brief Sets the linestyle to be used in subsequent drawings.

		Sets the linestyle to be used in subsequent drawings.
		
		@param[in] cr  a pointer to a cairo context
		@param[in] linestyle if 0, a solid line is drawn; if 1 or 2, a dashed line with a pre-determined pattern
		is drawn, range {0,1,2}. 
	*/
	void setlinestyle(cairo_t* cr, int linestyle);
	/*! @brief Sets the linestyle to be used in subsequent drawings.

		Sets the linestyle to be used in subsequent drawings.
		
		@param[in] cr  a pointer to a cairo context
		@param[in] num_dashes if 0, a solid line is drawn; if greater than 0, a dashed line with pattern determined by
		dashes is drawn, range greater than 0
		@param[in] dashes a 1-D double array of size linestyle, specifying alternate lengths of on and off positions of dashes.
	*/
	void setlinestyle(cairo_t* cr, int num_dashes, double *dashes);
	/*! @brief Sets the linewidth to be used in subsequent drawings. 

		Sets the linewidth to be used in subsequent drawings. 
		
		@param[in] cr  a pointer to a cairo context
		@param[in] linewidth the width of the line, range greater than 0
	*/
	void setlinewidth(cairo_t* cr, double linewidth);
	//@}
	
	/** @name User Callbacks
	 *  Public functions for registering user callbacks for mouse motion, mouse button press 
	    and keyboard press events.
	*/
	//@{
	/*! @brief Registers user's key press callback funtion. 

		Registers user's key press callback funtion, i.e now every key press event from the keyboard
		calls the user function. All the key press events are registered through the top-level window.
		
		@param[in] method  a pointer to a function of type user_key_press_fcn() (which accepts only
		one argument of type GdkEventKey).
	*/
	void activate_user_key_press_input( user_key_press_fcn method );
	/*! @brief Removes user's key press callback function.

		Removes user's key press callback function. After this function is called, the key presses will not
		call the corresponding user function.
	*/
	void deactivate_user_key_press_input();
	/*! @brief Registers user's mouse-move callback funtion. 

		Registers user's mouse-move callback funtion, i.e whenever the mouse moves over the application window, the user
		defined function is called.
		
		@warning Use this feature judiciously, as it may slow down the application.
		
		@param[in] method  a pointer to a function of type user_mouse_pointer_position_fcn() (which accepts only
		one argument of type GdkEventMotion).
	*/
	void activate_user_mouse_pointer_position_input( user_mouse_pointer_position_fcn method );
	/*! @brief Removes user's mouse pointer position callback function

		Removes user's mouse pointer position callback function. After this function is called, any mouse movement over the
		application window will not	call the corresponding user function.
	*/
	void deactivate_user_mouse_pointer_position_input();
	/*! @brief Registers user's mouse-button press callback funtion. 

		Registers user's mouse-button press callback funtion, i.e now for every mouse click, the user function is called.
		All the mouse-click events are registered through the top-level window.
		
		@param[in] method  a pointer to a function of type user_mouse_button_press_fcn() (which accepts only
		one argument of type GdkEventButton).
	*/
	void activate_user_mouse_button_press_input( user_mouse_button_press_fcn method );
	/*! @brief Removes user's mouse button press callback function.

		Removes user's mouse button press callback function. After this function is called, any mouse click
		will not call the corresponding user function.
	*/
	void deactivate_user_mouse_button_press_input();
	//@}
	
	// functions for saving in a particular format
	void save_as();
	void save_as_file_type(GtkWidget *widget); //requires restricted access in future
	
	// utility functions
	void line2arr (char* str, vector<string>* arr, char* tokenizer); 
	
	// public accessibility of restricted functions/variables by accessibility-key
	void turn_on_window_zoom( ); //requires restricted access in future
	void update_statusbar( ); //requires restricted access in future
	
	// to check for window state event
	void chk_mainwin_state_event(GdkEventWindowState *event); //requires restricted access in future
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
	
	/*! @brief the constructor

		Constructor for the class, used to image maps to make the user drawing more interactive. As of now, 
		only rectangular image maps are supported. 

		@param[in] _name  the name for this map, eacn image_map should have a unique name.
		@param[in] _x1   the x-coordinate of the upper-left corner for the user-defined image map
		@param[in] _y1   the y-ccordinate of the upper-left corner for the user-defined image map
		@param[in] _x2   the x-coordinate of the lower-right corner for the user-defined image map
		@param[in] _y2   the y-coordinate of the lower-right corner for the user-defined image map
		@param[in] tol   the text balloon corresponding to the image-map is drawn only if the rectangle between
		(event_x−tol, event_y−tol) and (event_x1+tol, event_y1+tol) is in viewport
		@param[in] _get_image_map_text_fcn a function of type get_image_map_text(), that is used to
		generate the text to be shown on the fly.
	*/
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
	/*! @brief Style button constructor

		Constructor for the class, used to create buttons to be added to the sidepane.

		@param[in] _button_text          the text to be displyed on the button 
		@param[in] _act_on_button_press  a pointer to a button_fcn() that is used as a callback when user
		clicks the button 
		@param[in] _application          the pointer to the instance of the main-class gtk_win() that handles the
		GUI and renders the canvas 
	*/
	style_button( string _button_text, button_fcn _act_on_button_press, gtk_win *_application );
	/*! @brief Sets tooltip text description

		Sets the description for a created button. It is displayed as a tooltip text when the user hovers over
		the button area.

		@param[in] _desc  a string describing what this button is supposed to do. 
	*/
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

