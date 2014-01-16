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
#include <math.h>
#include <string.h>

static string accessibility_key;
//string accessibility_key = "ty";
static void generate_accessibility_key( int length );

static gboolean mainwin_expose (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{	
	gtk_win* g_win = (gtk_win*) data;
	return g_win->mainwin_expose_event (event, accessibility_key);
}

gboolean gtk_win::mainwin_expose_event (GdkEventExpose *event, string key)
{
	if ( key == access_key )
	{
		cairo_t *cr;
	
		// get a cairo_t
		cr = gdk_cairo_create (mainwin->window);
		
		// set a clip region for the expose event 
		cairo_rectangle (cr, event->area.x, event->area.y,  event->area.width, event->area.height);
	
		cairo_clip (cr);
	
		cairo_set_source_rgb(cr, 0.82745, 0.8549, 0.88627);
		//cairo_pattern_t *linpat;
		//linpat = cairo_pattern_create_linear (0, 0, 600, 600);
		//cairo_pattern_add_color_stop_rgb (linpat, 1,  0.02745, 0.12157, 0.262745);
		//cairo_pattern_add_color_stop_rgb (linpat, 0.2,  0.596, 0.7647, 0.8353);
		//cairo_set_source (cr, linpat);
		cairo_paint(cr);

		cairo_destroy (cr);
		 stats->count_expose_event++;
	}
	else
	{
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::mainwin_expose_event() \n");
	}
	return false;
}

static gboolean canvas_expose (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{	
	gtk_win* g_win = (gtk_win*) data;
	return g_win->canvas_expose_event (event, accessibility_key);
}

gboolean gtk_win::canvas_expose_event (GdkEventExpose *event, string key)
{
	if ( key == access_key )
	{
		cairo_t *cr;
	
		stats->draw_start_fcn();
		stats->reset_counts(); 
		// get a cairo_t
		cr = gdk_cairo_create (canvas->window);
		this->cr = cr;
		
		// set a clip region for the expose event 
		cairo_rectangle (cr, event->area.x, event->area.y,  event->area.width, event->area.height);
		cairo_clip (cr);
	
		cairo_identity_matrix(cr);
		user_tx = 0; user_ty = 0;
		cairo_translate (cr, user2win_x(0), user2win_y(0) );
		cairo_save(cr);
		
		// paint the canvas as per transformed and rotated coordinates
		paint_canvas(cr);
	
		cairo_restore(cr);
	
		if (in_window_zoom_mode)
		{
			//cairo_identity_matrix(cr);
			//user_tx = 0; user_ty = 0;
			//cairo_translate (cr, user2win_x(0), user2win_y(0) );
			cairo_set_source_rgba(cr, 0.882353, 0.67843, 0.12549, 0.4);
			fillrect( cr, z_xleft, z_ytop, z_xright, z_ybottom);
			cairo_set_source_rgb(cr, 0.70980, 0.572549, 0.207843);
			cairo_set_line_width(cr, 2);
			drawrect( cr, z_xleft, z_ytop, z_xright, z_ybottom);
		}
		#ifdef BOUNDING_BOX
			cairo_save (cr);
			cairo_set_operator(cr, CAIRO_OPERATOR_XOR); 
			cairo_identity_matrix(cr);
			cairo_set_source_rgb(cr, 0, 0, 0);
			cairo_set_line_width(cr, 2);
			cairo_move_to(cr, user2win_x(saved_xleft),  user2win_y(saved_ytop) );
			cairo_line_to(cr, user2win_x(saved_xleft),  user2win_y(saved_ybottom) );
			cairo_line_to(cr, user2win_x(saved_xright), user2win_y(saved_ybottom) );
			cairo_line_to(cr, user2win_x(saved_xright), user2win_y(saved_ytop) );
			cairo_line_to(cr, user2win_x(saved_xleft),  user2win_y(saved_ytop) );
			cairo_stroke(cr);
			cairo_restore (cr);
		#endif
	
		#ifdef VIRTUAL_SCROLLBARS
			draw_virtual_scrollbars();
		#endif
		cairo_destroy (cr);
		 stats->draw_end_fcn();
		 stats->count_redraw++;
	}
	else
	{
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::canvas_expose_event() \n");
	}
	return false;
}

static void gtk_win_scroll(GtkWidget *widget, GdkEventScroll* event, gpointer data)
{	
	gtk_win* g_win = (gtk_win*) data;
	g_win->mainwin_scroll_event (event, accessibility_key);
}

void gtk_win::mainwin_scroll_event (GdkEventScroll* event, string key)
{
	if ( key == access_key )
	{
		double userx = win2user_x(event->x);
		double usery = win2user_y(event->y);
	
		double x_left_margin   = userx - xleft;
		double x_right_margin  = xright - userx;
		double y_top_margin    = usery - ytop;
		double y_bottom_margin = ybottom - usery;
	
		if ( event->direction == GDK_SCROLL_UP )
		{
			xleft   += x_left_margin*scroll_zoom_in_multiplier;
			xright  -= x_right_margin*scroll_zoom_in_multiplier; 
			ytop    += y_top_margin*scroll_zoom_in_multiplier; 
			ybottom -= y_bottom_margin*scroll_zoom_in_multiplier; 
		}
		else if ( event->direction == GDK_SCROLL_DOWN )
		{
			xleft   -= x_left_margin*scroll_zoom_out_multiplier;
			xright  += x_right_margin*scroll_zoom_out_multiplier; 
			ytop    -= y_top_margin*scroll_zoom_out_multiplier;
			ybottom += y_bottom_margin*scroll_zoom_out_multiplier;
		}
	
		update_statusbar_msg();
		focus();
		 stats->count_scroll_events++;
	}
	else
	{
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::mainwin_scroll_event() \n");
	}
}

static gboolean gtk_mainwin_configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	return g_win->mainwin_configure_event (event, accessibility_key);
}

gboolean gtk_win::mainwin_configure_event (GdkEventConfigure *event, string key)
{
	if ( key == access_key )
	{
		win_current_width = event->width;
		win_current_height = event->height;
		 stats->count_configure_event++;
	}
	else
	{
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::mainwin_configure_event() \n");
	}
	return false;
}

static gboolean gtk_canvas_configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	return g_win->canvas_configure_event (event, accessibility_key);
}

gboolean gtk_win::canvas_configure_event  (GdkEventConfigure *event, string key)
{
	if ( key == access_key )
	{
		canvas_width = event->width;
		canvas_height = event->height;
		update_transform();
	}
	else
	{
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::canvas_configure_event() \n");
	}
	return false;
}

static gboolean act_on_key_press (GtkWidget *widget, GdkEventKey *event,  gpointer data)
{	
	gtk_win* g_win = (gtk_win*) data;
	return g_win->mainwin_key_press_event (event, accessibility_key);
}

gboolean gtk_win::mainwin_key_press_event (GdkEventKey *event, string key)
{
	if ( key == access_key )
	{
		switch (event->keyval)
		{		
			/*case GDK_s:
			if (event->state & GDK_CONTROL_MASK)
			{
				save_as();
			}
			break;*/
		
			case GDK_Escape:
			restore_to_onset_view(1);
			update_statusbar_msg();
			break;
		
			case GDK_Up:
			translate_up();
			break;
		
			case GDK_Down:
			translate_down();
			break;
		
			case GDK_Left:
			translate_left();
			break;
		
			case GDK_Right:
			translate_right();
			break;
		
			case 0xffc2:
			redraw();
			break;
		
			case 0x06d:
			toggle_image_map_visibility(access_key);
			break;
		
			case 0x066:
			toggle_fullscreen(access_key);
			break;
		
			case 0xffbe:
			show_about_dialouge(access_key);
			break;
		
			default:
			break;
		}
	
		if (user_key_press_active)
		{
			user_key_press_method(event);
		}
		 stats->count_keypress_event++;
		return FALSE; 
	}
	else
	{
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::mainwin_key_press_event() \n");
		return false;
	}
}

static gboolean mouse_position_tracker(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{	
	gtk_win* g_win = (gtk_win*) data;
	return g_win->mainwin_mouse_move_event (event, accessibility_key);
}

gboolean gtk_win::mainwin_mouse_move_event(GdkEventMotion *event, string key)
{
	if ( key == access_key )
	{	
		#ifdef FOLLOW_MOUSE_POINTER
			GdkModifierType state;

			if (event->is_hint)
				gdk_window_get_pointer (event->window, &worldx, &worldy, &state);
			else
			{
				worldx = event->x;
				worldy = event->y;
			}

			update_statusbar_msg();
		#endif
	
		if (in_window_zoom_mode)
		{
			#ifndef FOLLOW_MOUSE_POINTER
				GdkModifierType state;

				if (event->is_hint)
					gdk_window_get_pointer (event->window, &worldx, &worldy, &state);
				else
				{
					worldx = event->x;
					worldy = event->y;
				}

				update_statusbar_msg();
			#endif
		
			z_xright  = win2user_x( event->x );
			z_ybottom = win2user_y( event->y );
			
			/* The follwoing code segment supposedly should mark the zooming rectnagle
			as dirty so that only the area within it is redrawn. but it is not working, so 
			commented out.
			cr = gdk_cairo_create( canvas->window );
			cs = cairo_get_target (cr);
			cairo_surface_mark_dirty_rectangle (cs, min(z_xleft, z_xright), min(z_ytop, z_ybottom), 
			fabs(z_xleft - z_xright)*xmult, fabs(z_ytop - z_ybottom)*ymult );
			//cairo_surface_destroy(cs);
			cairo_destroy(cr);*/
			
			gtk_widget_queue_draw_area ( canvas,  tx, ty, canvas_width, canvas_height);
		}
	
		if (user_mouse_position_active)
		{
			user_mouse_pos_method(event);
		}
		 stats->count_mousemove_event++;
	}
	else
	{
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::mainwin_mouse_move_event() \n");
		return false;
	}
	return TRUE;
}

static gboolean mouse_button_callback(GtkWidget *widget, GdkEventButton *event, gpointer data)
{	
	gtk_win* g_win = (gtk_win*) data;
	return g_win->mainwin_mouse_button_event (event, accessibility_key);
}

// function to highlight image maps
static void highlight_image_maps_fcn(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->toggle_image_map_visibility(accessibility_key);
}

void gtk_win::toggle_image_map_visibility(string key)
{
	if ( key == access_key )
	{
		highlight_image_maps = !highlight_image_maps;
		redraw();
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::toggle_image_map_visibility() \n");
}

// function to toggle_fullscreen
static void toggle_fullscreen_fcn(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->toggle_fullscreen(accessibility_key);
}

void gtk_win::toggle_fullscreen(string key)
{
	if ( key == access_key )
	{
		make_fullscreen = !make_fullscreen;
		if ( make_fullscreen )
		{
			gtk_window_fullscreen(GTK_WINDOW(mainwin) );
		}
		else
		{
			gtk_window_unfullscreen(GTK_WINDOW(mainwin) );
			gtk_widget_set_sensitive(GTK_WIDGET(ti_fullscreen), TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(ti_exitfullscreen), FALSE);
		}
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::toggle_fullscreen() \n");
}

// to verify that main windows' state changed as desired
static void chk_mainwin_state_event_fcn(GtkWidget *widget, GdkEventWindowState *event, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->chk_mainwin_state_event(event, accessibility_key);
}

void gtk_win::chk_mainwin_state_event(GdkEventWindowState *event, string key)
{
	if ( key == access_key )
	{
		// at present checking only for fullscreen and unfullscreen events
		if ( toolbar_created && make_fullscreen && event->new_window_state && GDK_WINDOW_STATE_FULLSCREEN )
		{
			fprintf(stderr, "Window Fullscreen Successfull \n");
			gtk_widget_set_sensitive(GTK_WIDGET(ti_fullscreen), FALSE);
			gtk_widget_set_sensitive(GTK_WIDGET(ti_exitfullscreen), TRUE);
		}
		//else if ( toolbar_created && !make_fullscreen && !(event->new_window_state && GDK_WINDOW_STATE_FULLSCREEN) )
		//{
		//	fprintf(stderr, "Window Exited Fullscreen\n");
		//}
		else if ( toolbar_created && make_fullscreen && !(event->new_window_state && GDK_WINDOW_STATE_FULLSCREEN) )
		{
			fprintf(stderr, "Window Fullscreen Un-successfull \n");
		}
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::chk_mainwin_state_event() \n");
}

// function to increase or decrease shear
static void increase_horizontal_shear_fcn(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->increase_horizontal_shear(accessibility_key);
}
void gtk_win::increase_horizontal_shear(string key)
{
	if (key == access_key)
	{
		horizontal_shear = horizontal_shear + delta_horz_shear;
		if ( horizontal_shear > MAX_SHEAR )
			horizontal_shear = MAX_SHEAR;
		redraw();
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::increase_horizontal_shear() \n");
}

static void decrease_horizontal_shear_fcn(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->decrease_horizontal_shear(accessibility_key);
}
void gtk_win::decrease_horizontal_shear(string key)
{
	if (key == access_key)
	{
		horizontal_shear = horizontal_shear - delta_horz_shear;
		if ( horizontal_shear < -1*MAX_SHEAR )
			horizontal_shear = -1*MAX_SHEAR;
		redraw();
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::decrease_horizontal_shear() \n");
}

static void increase_vertical_shear_fcn(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->increase_vertical_shear(accessibility_key);
}
void gtk_win::increase_vertical_shear(string key)
{
	if (key == access_key)
	{
		vertical_shear = vertical_shear + delta_vert_shear;
		if ( vertical_shear > MAX_SHEAR )
			vertical_shear = MAX_SHEAR;
		redraw();
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::increase_vertical_shear() \n");
}

static void decrease_vertical_shear_fcn(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->decrease_vertical_shear(accessibility_key);
}
void gtk_win::decrease_vertical_shear(string key)
{
	if (key == access_key)
	{
		vertical_shear = vertical_shear - delta_vert_shear;
		if ( vertical_shear < -1*MAX_SHEAR )
			vertical_shear = -1*MAX_SHEAR;
		redraw();
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::decrease_vertical_shear() \n");
}

//functions to rotate the canvas in clockwise or anticlockwise direction
void gtk_win::set_xy_translate_and_scale()
{
	if ( angle_multiplier%4 == 0 )
	{
		rx_translate = 0;
		ry_translate = 0;
	}
	else if ( angle_multiplier%4 == 1 )
	{
		rx_translate = saved_xright;
		ry_translate = 0;
	}
	else if ( fabs(angle_multiplier%4) == 2 )
	{
		rx_translate = saved_xright;
		ry_translate = saved_ybottom;
	}
	else if ( angle_multiplier%4 == 3 )
	{
		rx_translate = 0;
		ry_translate = saved_ybottom;
	}
	else if ( angle_multiplier%4 == -1 )
	{
		rx_translate = 0;
		ry_translate = saved_ybottom;
	}
	else if ( angle_multiplier%4 == -3 )
	{
		rx_translate = saved_xright;
		ry_translate = 0;
	}

}
static void rotate_clockwise_fcn(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->rotate_clockwise(accessibility_key);
}
void gtk_win::rotate_clockwise(string key)
{
	if (key == access_key)
	{
		angle_multiplier++;
		set_xy_translate_and_scale();
		redraw();
	}
}
static void rotate_anticlockwise_fcn(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->rotate_anticlockwise(accessibility_key);
}
void gtk_win::rotate_anticlockwise(string key)
{
	if (key == access_key)
	{
		angle_multiplier--;
		set_xy_translate_and_scale();
		redraw();
	}
}


gboolean gtk_win::mainwin_mouse_button_event (GdkEventButton *event, string key)
{
	if ( key == access_key )
	{		
		double x, y;
		worldx = event->x;
		worldy = event->y;
		x = win2user_x( event->x ); 
		y = win2user_y( event->y );
		bool coming_out_of_window_zoom_mode = false;
		bool going_into_window_zoom_mode = false;
		if ( event->type == GDK_BUTTON_PRESS && window_zoom_mode_on ) 
		{
			in_window_zoom_mode = true;
			z_xleft = x;
			z_ytop  = y;
			// to prevnt drawing a rectangle based on
			// previous xright and ybottom
			z_xright  = x;
			z_ybottom = y;
			
			going_into_window_zoom_mode = true;
			
		}
		else if ( event->type == GDK_BUTTON_RELEASE && window_zoom_mode_on)
		{
			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(zoom_window), 0);
			window_zoom_mode_on = false;
			in_window_zoom_mode = false;
			z_xright  = x;
			z_ybottom = y;
			xleft   = ((z_xleft < z_xright)?z_xleft:z_xright);
			xright  = ((z_xleft > z_xright)?z_xleft:z_xright); 
			ytop    = ((z_ytop < z_ybottom)?z_ytop:z_ybottom);
			ybottom = ((z_ytop > z_ybottom)?z_ytop:z_ybottom);
			focus();
			
			coming_out_of_window_zoom_mode = true;
		}
		update_statusbar_msg();
		
		if (!going_into_window_zoom_mode && !coming_out_of_window_zoom_mode)
		{
			bool to_redraw = false;	
			for ( unsigned int i = 0; i < image_maps.size(); i++)
			{
				to_redraw = to_redraw | image_maps[i]->to_show_info; 
				to_redraw = to_redraw | image_maps[i]->try_set_event_coordinates(event->x,event->y, this);
			}
			if ( to_redraw )
				redraw();
		}
	
		if (user_mouse_press_active && event->type == GDK_BUTTON_PRESS)
		{
			user_mouse_button_press_method(event);
		}
		 stats->count_mousebuttonpress_event++;
	}
	else
	{
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::mainwin_mouse_button_event() \n");
		return false;
	}
	
	return FALSE;
}

void gtk_win::turn_on_window_zoom( string key )
{
	if ( key == access_key)
		window_zoom_mode_on = true;
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::turn_on_window_zoom() \n");
}

void gtk_win::update_statusbar( string key )
{
	if ( key == access_key)
		update_statusbar_msg();
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::update_statusbar() \n");
}

static void turn_on_window_zoom_mode(GtkWidget *widget, gpointer data)
{
	printf("Turned on window zoom mode!!\n======================\n");
	gtk_win* g_win = (gtk_win*) data;
	g_win->turn_on_window_zoom (accessibility_key);
}
static void save_as_fcn(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	//save_as function is accessible, and can be called by the user when needed.
	g_win->save_as();
}

void gtk_win::toolbar_zoom_in( string key )
{
	if ( key == access_key )
	{
		double xdiff, ydiff;
		xdiff = xright  - xleft; 
		ydiff = ybottom - ytop;
		xleft   += xdiff/zoom_in_factor;
		xright  -= xdiff/zoom_in_factor;
		ytop    += ydiff/zoom_in_factor;
		ybottom -= ydiff/zoom_in_factor;
	
		focus();
		update_statusbar_msg();
	}
	else
	{
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::toolbar_zoom_in() \n");
	}
}

static void zoom_in_fcn(GtkWidget *widget, gpointer data)
{	
	gtk_win* g_win = (gtk_win*) data;
	g_win->toolbar_zoom_in(accessibility_key);
}

void gtk_win::toolbar_zoom_out( string key )
{
	if ( key == access_key )
	{
		double xdiff, ydiff;
		xdiff = xright  - xleft; 
		ydiff = ybottom - ytop;
		xleft   -= xdiff/zoom_out_factor;
		xright  += xdiff/zoom_out_factor;
		ytop    -= ydiff/zoom_out_factor;
		ybottom += ydiff/zoom_out_factor;
	
		focus();
		update_statusbar_msg();
	}
	else
	{
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::toolbar_zoom_in() \n");
	}
}

static void zoom_out_fcn(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->toolbar_zoom_out(accessibility_key);
}

static void zoom_fit_fcn(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->restore_to_onset_view(0);
}

static void refresh_drawing_area(GtkWidget *widget, gpointer data)
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->redraw();
}

void gtk_win::activate_user_key_press_input( user_key_press_fcn method )
{
	user_key_press_active = true;
	user_key_press_method = method;
}
void gtk_win::deactivate_user_key_press_input( )
{
	user_key_press_active = false;
	user_key_press_method = NULL;
}
void gtk_win::activate_user_mouse_pointer_position_input( user_mouse_pointer_position_fcn method )
{
	user_mouse_position_active = true;
	user_mouse_pos_method = method;
}
void gtk_win::deactivate_user_mouse_pointer_position_input()
{
	user_mouse_position_active = false;
	user_mouse_pos_method = NULL;
}
void gtk_win::activate_user_mouse_button_press_input( user_mouse_button_press_fcn method )
{
	user_mouse_press_active = true;
	user_mouse_button_press_method = method;
}
void gtk_win::deactivate_user_mouse_button_press_input()
{
	user_mouse_press_active = false;
	user_mouse_button_press_method = NULL;
}

static void request_about_dialouge( GtkWidget *widget, gpointer* data )
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->show_about_dialouge( accessibility_key );
}

static void request_preferences_dialog( GtkWidget *widget, gpointer data )
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->open_preferences_dialouge( accessibility_key );
}
static void request_properties_dialog( GtkWidget *widget, gpointer data )
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->show_properties_dialog( accessibility_key );
}
static void request_statistics_dialog( GtkWidget *widget, gpointer data )
{
	gtk_win* g_win = (gtk_win*) data;
	g_win->show_statistics_dialog( accessibility_key );
}
void gtk_win::show_statistics_dialog( string key )
{
	if ( access_key == key )
	{
		GtkWidget *dialog, *content_area;
		GtkWidget *main_vbox, *hbox, *main_hbox;
		GtkWidget *frame;
		GtkWidget *table;
		vector <string> arr;
		GtkWidget *label1, *label2;
		int index = 0;
		string markup;
		
		dialog = gtk_dialog_new_with_buttons ("Statistics",
		                                 GTK_WINDOW(mainwin),
		                                 GTK_DIALOG_MODAL, GTK_STOCK_CLOSE,
		                                 GTK_RESPONSE_NONE,
		                                 NULL);
		        
		content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
		
		main_vbox = gtk_vbox_new (FALSE, 5);
		gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 5);
		//gtk_container_add (GTK_CONTAINER (content_area), main_vbox);
		
		frame = gtk_frame_new ("Current Shape Count");
		gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 10);
		
		hbox = gtk_hbox_new (FALSE, 10);
		char buffer[300];
		stats->get_shape_counts( buffer );
		
		table = gtk_table_new(13, 2, 0);
		arr.clear();
		line2arr( buffer, &arr, ":,");
		index = 0;
		for ( int row = 0; row < 13; row++ )
		{
			for ( int col = 0; col < 2; col++ )
			{
				if ( col%2 == 0 )
				{
					label1 = gtk_label_new( NULL );
					markup = "<b>"
			        	+ arr[index] + ":</b>";
					gtk_label_set_markup ( GTK_LABEL ( label1 ), markup.c_str() );
				}
				else
					label1 = gtk_label_new( arr[index].c_str() );
				gtk_label_set_justify(GTK_LABEL (label1), GTK_JUSTIFY_RIGHT);
				gtk_misc_set_alignment (GTK_MISC (label1), 1, 0.5);
				gtk_table_attach( GTK_TABLE(table), label1, col, col+1, row, row+1, GTK_FILL, GTK_FILL, 3, 3 );
				index++;
			}
		}
		
		gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 10);
		gtk_container_add (GTK_CONTAINER (frame), hbox);
		
		frame = gtk_frame_new ("Total Event Count");
		gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 10);
		
		hbox = gtk_hbox_new (FALSE, 10);
		char buffer2[300];
		stats->get_event_count( buffer2 );
		
		table = gtk_table_new(8, 2, 0);
		arr.clear();
		line2arr( buffer2, &arr, ":,");
		index = 0;
		for ( int row = 0; row < 8; row++ )
		{
			for ( int col = 0; col < 2; col++ )
			{
				if ( col%2 == 0 )
				{
					label1 = gtk_label_new( NULL );
					markup = "<b>"
			        	+ arr[index] + ":</b>";
					gtk_label_set_markup ( GTK_LABEL ( label1 ), markup.c_str() );
				}
				else
					label1 = gtk_label_new( arr[index].c_str() );
				gtk_label_set_justify(GTK_LABEL (label1), GTK_JUSTIFY_RIGHT);
				gtk_misc_set_alignment (GTK_MISC (label1), 1, 0.5);
				gtk_table_attach( GTK_TABLE(table), label1, col, col+1, row, row+1, GTK_FILL, GTK_FILL, 3, 3 );
				index++;
			}
		}
		
		gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 10);
		gtk_container_add (GTK_CONTAINER (frame), hbox);
		
		frame = gtk_frame_new ("Resource Usage and Profile");
		//gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 10);
		
		hbox = gtk_hbox_new (FALSE, 10);
		char buffer3[800];
		stats->get_resource_usage( buffer3 );
		#ifdef linux OR unix
			table = gtk_table_new(23, 2, 0);
		# else
			table = gtk_table_new(3, 2, 0);
		#endif
		arr.clear();
		line2arr( buffer3, &arr, ":,");
		index = 0;
		#ifdef linux OR_unix
		for ( int row = 0; row < 23; row++ )
		#else
		for ( int row = 0; row < 1; row++ )
		#endif
		{
			for ( int col = 0; col < 2; col++ )
			{
				if ( col%2 == 0 )
				{
					label1 = gtk_label_new( NULL );
					markup = "<b>"+ arr[index] + ":</b>";
					gtk_label_set_markup ( GTK_LABEL ( label1 ), markup.c_str() );
				}
				else
					label1 = gtk_label_new( arr[index].c_str() );
				gtk_label_set_justify(GTK_LABEL (label1), GTK_JUSTIFY_RIGHT);
				gtk_misc_set_alignment (GTK_MISC (label1), 1, 0.5);
				gtk_table_attach( GTK_TABLE(table), label1, col, col+1, row, row+1, GTK_FILL, GTK_FILL, 3, 3 );
				index++;
			}
		}
		#ifndef linux AND unix
			label1 = gtk_label_new( NULL );
			markup = "<span size=\"x-large\"><b> Cannot show other details\nfor non-Linux Platforms</b></span>";
			gtk_label_set_markup ( GTK_LABEL ( label1 ), markup.c_str() );
			gtk_table_attach( GTK_TABLE(table), label1, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 3, 3 );
		#endif
		
		gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 10);
		gtk_container_add (GTK_CONTAINER (frame), hbox);
		
		main_hbox = gtk_hbox_new (FALSE, 5);
		gtk_container_set_border_width (GTK_CONTAINER (main_hbox), 10);
		gtk_box_pack_start (GTK_BOX (main_hbox), main_vbox, TRUE, TRUE, 10);
		gtk_box_pack_start (GTK_BOX (main_hbox), frame, TRUE, TRUE, 10);
		gtk_container_add (GTK_CONTAINER (content_area), main_hbox);
		
		
		/* Ensure that the dialog box is destroyed when the user responds */
		g_signal_connect(dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
		
		#ifdef UNDECORATED_DIALOG
			gtk_window_set_decorated (GTK_WINDOW(dialog), FALSE);
		#endif
		gtk_widget_show_all (dialog);
		
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::show_statistics_dialog() \n");
}



void gtk_win::show_about_dialouge( string key )
{
	if ( key == access_key )
	{
		#ifndef EXPERIMENTAL_ABOUT_DIALOG
			GtkWidget *dialog, *content_area;
			GtkWidget *main_hbox;
			GtkWidget *logo;
			GtkWidget *vbox; 
			GtkWidget *name_label;
			GtkWidget *copyright_label;
			GtkWidget *comments_label;
			GtkWidget *license_label;
			GtkWidget *documentation_label;
			
			dialog = gtk_dialog_new_with_buttons ("About BridgeGL",
		                                 GTK_WINDOW(mainwin),
		                                 GTK_DIALOG_MODAL, GTK_STOCK_CLOSE,
		                                 GTK_RESPONSE_NONE,
		                                 NULL);
		        
		        content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
		        
		        main_hbox = gtk_hbox_new (FALSE, 5);
			gtk_container_set_border_width (GTK_CONTAINER (main_hbox), 10);
			gtk_container_add (GTK_CONTAINER (content_area), main_hbox);
			
			logo = gtk_image_new_from_file ("logo.png");
		        gtk_box_pack_start (GTK_BOX (main_hbox), logo, TRUE, TRUE, 5);
		        
		        vbox = gtk_vbox_new (FALSE, 5);
		        gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
		        gtk_box_pack_start (GTK_BOX (main_hbox), vbox, TRUE, TRUE, 5);
		        
		        name_label = gtk_label_new(NULL);
		        string name_and_version = "<span size=\"x-large\"><b>" + name + " " + version + "</b></span>";
		        gtk_label_set_markup (GTK_LABEL (name_label), name_and_version.c_str() );
		        
		        copyright_label = gtk_label_new(NULL);
		        string copyright = "<i>(c)2013</i> <b>Sandeep Chatterjee</b> [chatte45@eecg.utoronto.ca]";
		        gtk_label_set_markup (GTK_LABEL (copyright_label), copyright.c_str() );
		        gtk_label_set_justify(GTK_LABEL(copyright_label), GTK_JUSTIFY_LEFT);
		        
		        comments_label = gtk_label_new(NULL);
		        string comment = "An easy-to-use light-weight 2D-graphics package powered by <a href=\"http://www.cairographics.org/\">Cairo</a> \nand <a href=\"http://www.gtk.org/\">GTK+</a>";
		        gtk_label_set_markup (GTK_LABEL (comments_label), comment.c_str() );
		        gtk_label_set_justify(GTK_LABEL(comments_label), GTK_JUSTIFY_LEFT);
		        gtk_misc_set_alignment (GTK_MISC (comments_label), 0, 0);
		        
		        license_label = gtk_label_new("Released under GNU license:\n\
This program is free software: you can redistribute it and/or modify it\n\
under the terms of the GNU General Public License as published by the\n\
Free Software Foundation, either version 3 of the License, or (at your\n\
option) any later version. \n\n\
This program is distributed in the hope that it will be useful, but WITHOUT\n\
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY\n\
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public\n\
License for more details.");

		        gtk_label_set_justify(GTK_LABEL(license_label), GTK_JUSTIFY_LEFT);
		        gtk_misc_set_alignment (GTK_MISC (license_label), 0, 0);
		        
		        documentation_label = gtk_label_new(NULL);
		        string documentation = "For documentation, click <a href=\"http://www.eecg.utoronto.ca/~chatte45/bgl_doc/index.html\">Here</a>";
		        gtk_label_set_markup (GTK_LABEL (documentation_label), documentation.c_str() );
		         
		        gtk_box_pack_start (GTK_BOX (vbox), name_label, TRUE, TRUE, 5);
		        gtk_box_pack_start (GTK_BOX (vbox), copyright_label, TRUE, TRUE, 5);
		        gtk_box_pack_start (GTK_BOX (vbox), comments_label, TRUE, TRUE, 5);
		        gtk_box_pack_start (GTK_BOX (vbox), license_label, TRUE, TRUE, 5);
		        gtk_box_pack_start (GTK_BOX (vbox), documentation_label, TRUE, TRUE, 5);
		        
		        /* Ensure that the dialog box is destroyed when the user responds */
		        g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
			#ifdef UNDECORATED_DIALOG
				gtk_window_set_decorated (GTK_WINDOW(dialog), FALSE);
			#endif
			gtk_widget_show_all (dialog);
		#endif
		
		#ifdef EXPERIMENTAL_ABOUT_DIALOG
			double xcen = canvas_width/2;
			double ycen = canvas_height/2; 
			double 
			x             = xcen - 250,        // parameters like cairo_rectangle
			y             = ycen - 200,
			width         = 500,
			height        = 400,
			aspect        = 1.25,     // aspect ratio 
			corner_radius = height / 10.0;   // and corner curvature radius

			double radius = corner_radius / aspect;
			double degrees = M_PI / 180.0;

			cairo_t* cr = gdk_cairo_create (canvas->window);

			cairo_new_sub_path (cr);
			cairo_arc (cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
			cairo_arc (cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
			cairo_arc (cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
			cairo_arc (cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
			cairo_close_path (cr);

			cairo_set_source_rgba (cr, 0, 0, 0, 0.85);
			cairo_fill_preserve (cr);
			cairo_set_source_rgba (cr, 0.5, 0.5, 0.5, 1);
			cairo_set_line_width (cr, 5.0);
			cairo_stroke (cr);
		
			cairo_select_font_face (cr, "Sans",
			    CAIRO_FONT_SLANT_NORMAL,
			    CAIRO_FONT_WEIGHT_NORMAL);
			
			y = y - 15;
			cairo_set_font_size (cr, 40.0);
			cairo_set_source_rgb (cr, 1, 1, 1);
			cairo_move_to (cr, x + 30, y + 80);
			cairo_show_text (cr, "BridgeGL 1.0");
			cairo_set_font_size (cr, 20.0);
			cairo_move_to (cr, x + 30, y + 110);
			cairo_show_text (cr, "(c) Sandeep Chatterjee 2013"); 
			cairo_set_font_size (cr, 17.0);
			cairo_move_to (cr, x + 30, y + 135);
			cairo_show_text (cr, "[chatte45@eecg.utoronto.ca]");
			cairo_move_to (cr, x + 30, y + 160);
			cairo_show_text (cr, "www.eecg.utoronto.ca/~chatte45/doc/index.html");
		
			cairo_pattern_t *pat;

			double sep_x1 = x+80, sep_y1 = y+175, sep_x2 = x+width-80;

			pat = cairo_pattern_create_linear (sep_x1, sep_y1,  sep_x2, sep_y1);
			cairo_pattern_add_color_stop_rgba (pat, 1, 1, 1, 1, 0);
			cairo_pattern_add_color_stop_rgba (pat, 0.5, 0.5, 0.5, 0.5, 1);
			cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 0);
			cairo_rectangle (cr, sep_x1, sep_y1-1.5, sep_x2-sep_x1, 3);
			cairo_set_source (cr, pat);
			cairo_fill (cr);
			cairo_pattern_destroy (pat);
		
			cairo_set_source_rgb (cr, 1, 1, 1);
			cairo_set_font_size (cr, 20.0);
			cairo_move_to (cr, x + 30, y + 205);
			cairo_show_text (cr, "Released under GNU License");
			cairo_set_font_size (cr, 13.0);
			cairo_move_to (cr, x + 30, y + 230);
			cairo_show_text (cr, "This program is free software: you can redistribute it and/or modify");
			cairo_move_to (cr, x + 30, y + 245);
			cairo_show_text (cr, "it under the terms of the GNU General Public License as published");
			cairo_move_to (cr, x + 30, y + 260);
			cairo_show_text (cr, "by the Free Software Foundation, either version 3 of the License,");
			cairo_move_to (cr, x + 30, y + 275);
			cairo_show_text (cr, "or (at your option) any later version.");
			cairo_move_to (cr, x + 40, y + 295);
			cairo_show_text (cr, "This program is distributed in the hope that it will be useful, but");
			cairo_move_to (cr, x + 30, y + 310);
			cairo_show_text (cr, "WITHOUT ANY WARRANTY; without even the implied warranty of");
			cairo_move_to (cr, x + 30, y + 325);
			cairo_show_text (cr, "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See");
			cairo_move_to (cr, x + 30, y + 340);
			cairo_show_text (cr, "the GNU General Public License for more details.");
			cairo_move_to (cr, x + 40, y + 360);
			cairo_show_text (cr, "You should have received a copy of the GNU General Public ");
			cairo_move_to (cr, x + 30, y + 375);
			cairo_show_text (cr, "License along with this program. If not, get it here: ");
			cairo_set_font_size (cr, 17.0);
			cairo_move_to (cr, x + 130, y + 395);
			cairo_show_text (cr, "www.gnu.org/licenses/");
		
			cairo_destroy(cr);
		#endif
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::show_about_dialouge() \n");
}

/* Accessor functions for settings */
void gtk_win::set_zoom_in_factor(double n)
{
	zoom_in_value = n;
	zoom_in_factor = 2*n/(n-1);
}
void gtk_win::set_zoom_out_factor(double n)
{
	zoom_out_value = n;
	zoom_out_factor = 2*n/(1-n);
}
void gtk_win::set_scroll_zoom_in_factor(double n)
{
	zoom_in_value = n;
	scroll_zoom_in_multiplier = (n-1)/n;
}
void gtk_win::set_scroll_zoom_out_factor(double n)
{
	zoom_out_value = n;
	scroll_zoom_out_multiplier = (1-n)/n;
}
void gtk_win::set_translate_up_factor(double n)
{
	translate_u_value = n;
	translate_u_factor = 100.0/n;
}
void gtk_win::set_translate_down_factor(double n)
{
	translate_d_value = n;
	translate_d_factor = 100.0/n;
}
void gtk_win::set_translate_left_factor(double n)
{
	translate_l_value = n;
	translate_l_factor = 100.0/n;
}
void gtk_win::set_translate_right_factor(double n)
{
	translate_r_value = n;
	translate_r_factor = 100.0/n;
}
void gtk_win::set_horizontal_shear_delta(double angle)// angle in degrees
{
	hshear_angle = angle;
	delta_horz_shear = atan(CONV_DEG_TO_RAD*hshear_angle);
}
void gtk_win::set_vertical_shear_delta(double angle)// angle in degrees
{
	vshear_angle = angle;
	delta_vert_shear = atan(CONV_DEG_TO_RAD*vshear_angle);
}

double gtk_win::get_zoom_in_factor()
{
	return zoom_in_factor/(zoom_in_factor - 2 );
}	
double gtk_win::get_zoom_out_factor()
{
	return zoom_out_factor/(2 + zoom_out_factor );
}
double gtk_win::get_scroll_zoom_in_factor()
{
	return 1/(1 - scroll_zoom_in_multiplier);
}
double gtk_win::get_scroll_zoom_out_factor()
{
	return 1/(1 + scroll_zoom_out_multiplier);
}
double gtk_win::get_translate_up_factor()
{
	return 100/translate_u_factor;
}
double gtk_win::get_translate_down_factor()
{
	return 100/translate_d_factor;
}
double gtk_win::get_translate_left_factor()
{
	return 100/translate_l_factor;
}
double gtk_win::get_translate_right_factor()
{
	return 100/translate_r_factor;
}
double gtk_win::get_max_slope()
{
	return max_slope;
}
double gtk_win::get_min_slope()
{
	return min_slope;
}
double gtk_win::get_horizontal_shear()
{
	return horizontal_shear;
}
double gtk_win::get_vertical_shear()
{
	return vertical_shear;
}
/* END of accessor functions */


void gtk_win::paint_canvas(cairo_t *cr)
{
	translate_coordinates(cr, rx_translate, ry_translate);
	cairo_rotate(cr,  angle_multiplier*M_PI/2.0);
	if ( ( fabs(angle_multiplier%4) == 1 || fabs(angle_multiplier%4) == 3 ) &&
	     saved_xright > saved_ybottom )
	{
		cairo_scale(cr, saved_ybottom/saved_xright, saved_ybottom/saved_xright);
	}

	cairo_matrix_t matrix;
	cairo_matrix_init(&matrix,
	1.0, vertical_shear,
	horizontal_shear, 1.0,
	0., 0.);
	cairo_transform(cr, &matrix);
	
	drawscreen (canvas, cr);
	// image maps
	image_maps_store_transformed_coordinates();
	if ( highlight_image_maps )
		image_maps_highlight();
	image_maps_show_text_balloon();
}

gtk_win::gtk_win( draw_gtk _drawscreen, int onset_width, int onset_height  )
{
	//cout<<"constructor"<<endl;
	stats = new bgl_stats( this );
	stats->start_time = time(NULL);
	
	name = "BridgeGL";
	version = "1.4";	
	
	win_current_width  = onset_width;
	win_current_height = onset_height;
	tx = 0;
	ty = 0;
	scale = 1;
	user_tx = 0;
	user_ty = 0; 
	drawscreen = _drawscreen;
	window_zoom_mode_on = false;
	
	// These are desired value,s actual values are
	// stored after the widgets are associated with a screen  
	stbar_height = 30;
	sdp_width = 90;
	top_gui_height = 67;
	
	type = PNG;
	angle = 0;
	
	highlight_image_maps = false;
	make_fullscreen = false;
	toolbar_created = false;
	
	// shear transform
	max_slope = pow(10, 10);
	min_slope = pow(10, -10);
	horizontal_shear = 0;
	vertical_shear = 0;
	hshear_angle = 5;
	vshear_angle = 5;
	delta_horz_shear = atan(CONV_DEG_TO_RAD*hshear_angle);
	delta_vert_shear = atan(CONV_DEG_TO_RAD*vshear_angle);
	
	// rotation
	angle_multiplier = 0;
	rx_translate = 0;
	ry_translate = 0;
	
	#ifdef MENU_BAR
	accel_group = NULL;
	#endif
	
	// zoom and translate
	zoom_in_value = 1.04;
	zoom_out_value = 0.96;
	translate_u_value = 5;
	translate_d_value = 5;
	translate_l_value = 5;
	translate_r_value = 5;
	zoom_in_factor = 2*1.04/(1.04-1);
	zoom_out_factor = 2*0.96/(1-0.96);
	scroll_zoom_in_multiplier = (1.04-1)/1.04;
	scroll_zoom_out_multiplier = (1-0.96)/0.96;
	translate_u_factor = 100/5.0;
	translate_d_factor = 100/5.0;
	translate_l_factor = 100/5.0;
	translate_r_factor = 100/5.0;
	
	user_key_press_method = NULL;
	user_key_press_active = false;
	user_mouse_pos_method = NULL;
	user_mouse_position_active = false;
	user_mouse_button_press_method = NULL;
	user_mouse_press_active = false;
	
	srand( time(NULL) );
	int length = rand()%20 + 30;
	generate_accessibility_key( length );
	access_key = ::accessibility_key;
	//printf("Access key: %s and length: %d and str_len: %d, \naccess_key: %s, required equality: %d\n", 
	//::accessibility_key.c_str(), length, ::accessibility_key.length(), access_key.c_str(), (accessibility_key == access_key));
}

void gtk_win::init_world( double x1, double y1, double x2, double y2 )
{
	//cout<<"init_world"<<endl;
	xleft = x1;
	xright = x2;
	ytop = y1;
	ybottom = y2;
	
	saved_xleft = xleft;     /* Save initial world coordinates to allow full */
	saved_xright = xright;   /* view button to zoom all the way out.         */
	saved_ytop = ytop;
	saved_ybottom = ybottom;
	
	//update_transform();
	// Fire up GTK!    
	//gtk_rc_parse( "ob_gtkrc" ); 
	//g_mem_set_vtable (glib_mem_profiler_table);
	//g_atexit (g_mem_profile);
	
	gtk_init (NULL, NULL); 
}

void
popup_menu(GtkAction *action, GtkMenu *menu)
{
    gtk_menu_popup(menu, NULL, NULL, NULL, NULL, 1, gtk_get_current_event_time());
}

void my_getsize(GtkWidget *widget, GtkAllocation *allocation, void *data) {
    printf("width = %d, height = %d\n", allocation->width, allocation->height);
}

void gtk_win::init_graphics( char* windowtitle )
{
	mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_app_paintable(mainwin, TRUE);
	g_signal_connect(G_OBJECT(mainwin), "expose-event", G_CALLBACK(mainwin_expose), (gpointer)this );
	
	gtk_container_set_border_width (GTK_CONTAINER (mainwin), 0);
	
	canvas = gtk_drawing_area_new ();
	gtk_widget_set_double_buffered ( canvas, true);
	int canvas_width = win_current_width - sdp_width;
	int canvas_height = win_current_height - top_gui_height-stbar_height;
	gtk_widget_set_size_request (canvas, canvas_width, canvas_height);
	
	statusbar = gtk_statusbar_new();
	gtk_widget_set_size_request (statusbar, win_current_width, stbar_height);
	
	char modifiedtitle[100];
	sprintf(modifiedtitle, "BridgeGL: %s", windowtitle);
	gtk_window_set_title(GTK_WINDOW(mainwin), modifiedtitle);	
	
	gtk_widget_add_events(mainwin, GDK_SCROLL_MASK);
	gtk_widget_add_events(mainwin, GDK_POINTER_MOTION_MASK);
	gtk_widget_add_events(mainwin, GDK_BUTTON_PRESS_MASK|GDK_BUTTON_MOTION_MASK);
	gtk_widget_add_events(mainwin, GDK_BUTTON_RELEASE_MASK);
		
	vbox = gtk_vbox_new(FALSE, 1);
	hbox = gtk_hbox_new(FALSE, 1);
	sidepane = gtk_vbox_new(FALSE, 1);
	gtk_widget_set_size_request (sidepane, sdp_width, canvas_height);
	
	gtk_box_pack_start(GTK_BOX(hbox), sidepane, FALSE, FALSE, 0);
	//gtk_box_pack_start(GTK_BOX(hbox), vseparator, FALSE, TRUE, 0);
	gtk_box_pack_end(GTK_BOX(hbox), canvas, TRUE, TRUE, 0); 
	
	gtk_container_add (GTK_CONTAINER (mainwin), vbox);
	
	//hseparator1 = gtk_hseparator_new();
	//hseparator2 = gtk_hseparator_new();
	
	for( int i = sidepane_buttons.size()-1; i >=0; i-- )
	{
		gtk_box_pack_start(GTK_BOX(sidepane), sidepane_buttons[i]->get_widget(access_key), FALSE, FALSE, 0);
	}
	
	#ifdef MENU_BAR
	create_custom_menu();
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
	#endif
	
	#ifdef TOOLBAR
	create_toolbar();
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
	#endif
	
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0); 
	
	gtk_box_pack_end(GTK_BOX(vbox), statusbar, FALSE, TRUE, 0);
	
	g_signal_connect (mainwin, "destroy", G_CALLBACK (gtk_main_quit), NULL); // Quit graphically 

	// key press events are connected to act_on_key_press
	g_signal_connect (mainwin, "key_press_event", G_CALLBACK (act_on_key_press),  (gpointer)this );
	
	g_signal_connect(G_OBJECT(canvas), "configure-event", G_CALLBACK(gtk_canvas_configure), (gpointer)this);
	g_signal_connect(G_OBJECT(mainwin), "configure-event", G_CALLBACK(gtk_mainwin_configure), (gpointer)this);
	
	g_signal_connect(mainwin, "scroll-event", G_CALLBACK(gtk_win_scroll), (gpointer)this);
	gtk_signal_connect(GTK_OBJECT(mainwin), "motion_notify_event", G_CALLBACK(mouse_position_tracker), (gpointer)this);
	g_signal_connect(mainwin, "button_press_event", G_CALLBACK(mouse_button_callback), (gpointer)this);
	g_signal_connect(mainwin, "button_release_event", G_CALLBACK(mouse_button_callback), (gpointer)this);

	// Whenever exposed, do the drawing on canvas as per user function
	g_signal_connect (G_OBJECT (canvas), "expose-event", G_CALLBACK (canvas_expose),  (gpointer)this );
	
	// connecting window state event
	g_signal_connect(G_OBJECT(mainwin),"window-state-event", G_CALLBACK(chk_mainwin_state_event_fcn), (gpointer)this);
	
	// make the icon
	GdkPixbuf *pixbuf;
	GError *error = NULL;
	pixbuf = gdk_pixbuf_new_from_file("logo.png", &error);
	if(!pixbuf) 
	{
		fprintf(stderr, "%s\n", error->message);
		g_error_free(error);
	}
	else
		gtk_window_set_icon(GTK_WINDOW(mainwin), pixbuf);
	
	// Show the window on the screen 
	gtk_widget_show_all (mainwin); 	
	
	// try to get width and height
	
	stbar_height = statusbar->allocation.height;
	sdp_width = sidepane->allocation.width;
	top_gui_height = menubar->allocation.height+toolbar->allocation.height;
	
	 // Enter the main event loop, and wait for user interaction 
	gtk_main (); //*/
	
	delete stats;
}

static void print_hello( GtkWidget *w,
                         gpointer   data )
{
  g_message ("Hello, World!\n");
}

#ifdef MENU_BAR
void gtk_win::create_custom_menu()
{
	menubar = gtk_menu_bar_new();
	filemenu = gtk_menu_new();
	viewmenu = gtk_menu_new();
	imagemenu = gtk_menu_new();
	editmenu = gtk_menu_new();
	helpmenu = gtk_menu_new();
	
	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(mainwin), accel_group);

	// Create file submenu
	file = gtk_menu_item_new_with_mnemonic("_File");
	mi_refresh = gtk_menu_item_new_with_label("Refresh");
	mi_saveas = gtk_menu_item_new_with_label("Save as");
	mi_property = gtk_menu_item_new_with_label("Properties");
	mi_stats = gtk_menu_item_new_with_label("Stats");
	mi_sep = gtk_separator_menu_item_new();
	mi_quit = gtk_menu_item_new_with_label("Quit");

	gtk_widget_add_accelerator(mi_quit, "activate", accel_group, 
	GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(mi_saveas, "activate", accel_group, 
	GDK_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 


	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), mi_refresh);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), mi_saveas);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), mi_property);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), mi_stats);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), mi_sep);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), mi_quit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);

	g_signal_connect(G_OBJECT(mi_quit), "activate",
	G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(mi_saveas), "activate",
	G_CALLBACK(save_as_fcn), (gpointer)this);
	g_signal_connect(G_OBJECT(mi_property), "activate",
	G_CALLBACK(request_properties_dialog), (gpointer)this);
	g_signal_connect(G_OBJECT(mi_stats), "activate",
	G_CALLBACK(request_statistics_dialog), (gpointer)this);
	g_signal_connect(G_OBJECT(mi_refresh), "activate",
	G_CALLBACK(refresh_drawing_area), (gpointer)this);
	
	// Create edit submenu
	edit = gtk_menu_item_new_with_mnemonic("_Edit");
	mi_preferences = gtk_menu_item_new_with_label("Preferences");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit), editmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), mi_preferences);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), edit);

	g_signal_connect(G_OBJECT(mi_preferences), "activate",
	G_CALLBACK(request_preferences_dialog), (gpointer)this);
	
	// Create view submenu
	view = gtk_menu_item_new_with_mnemonic("_View");
	mi_window  = gtk_menu_item_new_with_label("Zoom-Window");
	mi_zoomin  = gtk_menu_item_new_with_label("Zoom-In");
	mi_zoomout = gtk_menu_item_new_with_label("Zoom-Out");
	mi_zoomfit = gtk_menu_item_new_with_label("Zoom-Fit");
	mi_sep2 = gtk_separator_menu_item_new();
	tog_maps = gtk_check_menu_item_new_with_label("View Image-maps");
	mi_fullscreen = gtk_check_menu_item_new_with_label("Fullscreen");
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(tog_maps), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mi_fullscreen), FALSE);
	
	gtk_widget_add_accelerator(mi_window, "activate", accel_group, 
	GDK_w, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(mi_zoomin, "activate", accel_group, 
	GDK_plus, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(mi_zoomout, "activate", accel_group, 
	GDK_minus, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(mi_zoomfit, "activate", accel_group, 
	GDK_0, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);


	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view), viewmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), mi_window);
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), mi_zoomin);
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), mi_zoomout);
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), mi_zoomfit);
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), mi_sep2);
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), tog_maps);
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), mi_fullscreen);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view);

	g_signal_connect(G_OBJECT(mi_window), "activate",
	G_CALLBACK(turn_on_window_zoom_mode), (gpointer)this);
	g_signal_connect(G_OBJECT(mi_zoomin), "activate",
	G_CALLBACK(zoom_in_fcn), (gpointer)this);
	g_signal_connect(G_OBJECT(mi_zoomout), "activate",
	G_CALLBACK(zoom_out_fcn), (gpointer)this);
	g_signal_connect(G_OBJECT(mi_zoomfit), "activate",
	G_CALLBACK(zoom_fit_fcn), (gpointer)this);
	g_signal_connect(G_OBJECT(tog_maps), "activate", 
        G_CALLBACK(highlight_image_maps_fcn), (gpointer)this);
        g_signal_connect(G_OBJECT(mi_fullscreen), "activate", 
        G_CALLBACK(toggle_fullscreen_fcn), (gpointer)this);
        
        //create Image submenu
	image = gtk_menu_item_new_with_mnemonic("_Image");
	mi_translateUp  = gtk_menu_item_new_with_label("Translate Up");
	mi_translateDown  = gtk_menu_item_new_with_label("Translate Down");
	mi_translateLeft = gtk_menu_item_new_with_label("Translate Left");
	mi_translateRight = gtk_menu_item_new_with_label("Translate Right");
	mi_sep3 = gtk_separator_menu_item_new();
	mi_shearHplus = gtk_menu_item_new_with_label("Shear Horz+");
	mi_shearHminus = gtk_menu_item_new_with_label("Shear Horz-");
	mi_shearVplus = gtk_menu_item_new_with_label("Shear Vert+");
	mi_shearVminus = gtk_menu_item_new_with_label("Shear Vert-");
	mi_sep4 = gtk_separator_menu_item_new();
	mi_rotateClockwise = gtk_menu_item_new_with_label("Rotate Clockwise");
	mi_rotateantiClockwise = gtk_menu_item_new_with_label("Rotate Anti-Clockwise");
	
	gtk_widget_add_accelerator(mi_shearHplus, "activate", accel_group, 
	GDK_h, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(mi_shearHminus, "activate", accel_group, 
	GDK_h, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(mi_shearVplus, "activate", accel_group, 
	GDK_v, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(mi_shearVminus, "activate", accel_group, 
	GDK_v, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(mi_rotateClockwise, "activate", accel_group, 
	GDK_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(mi_rotateantiClockwise, "activate", accel_group, 
	GDK_r, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(image), imagemenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_translateUp);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_translateDown);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_translateLeft);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_translateRight);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_sep3);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_shearHplus);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_shearHminus);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_shearVplus);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_shearVminus);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_sep4);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_rotateClockwise);
	gtk_menu_shell_append(GTK_MENU_SHELL(imagemenu), mi_rotateantiClockwise);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), image);
	
	g_signal_connect(G_OBJECT(mi_shearHplus), "activate",
	G_CALLBACK(increase_horizontal_shear_fcn), (gpointer)this);
	g_signal_connect(G_OBJECT(mi_shearHminus), "activate",
	G_CALLBACK(decrease_horizontal_shear_fcn), (gpointer)this);
	g_signal_connect(G_OBJECT(mi_shearVplus), "activate",
	G_CALLBACK(increase_vertical_shear_fcn), (gpointer)this);
	g_signal_connect(G_OBJECT(mi_shearVminus), "activate",
	G_CALLBACK(decrease_vertical_shear_fcn), (gpointer)this);
	g_signal_connect(G_OBJECT(mi_rotateClockwise), "activate",
	G_CALLBACK(rotate_clockwise_fcn), (gpointer)this);
	g_signal_connect(G_OBJECT(mi_rotateantiClockwise), "activate",
	G_CALLBACK(rotate_anticlockwise_fcn), (gpointer)this);
	
	// Create edit submenu
	help = gtk_menu_item_new_with_mnemonic("_Help");
	mi_about = gtk_menu_item_new_with_label("About BridgeGL");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), mi_about);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);

	g_signal_connect(G_OBJECT(mi_about), "activate",
	G_CALLBACK(request_about_dialouge), (gpointer)this);
}
#endif

#ifdef TOOLBAR
void gtk_win::create_toolbar()
{
	/* List of functionalities and their classification
	   file (refresh, save as, quit), View ( window, zoom-in, zoom-out, zoom-fit), translate (up, down, left, right)
	   shear (horz+, horz-, vert+, vert-), settings (handle the zoom, translate and shear settings)
	*/
	
	toolbar = gtk_toolbar_new();
	#ifndef MENU_BAR
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH);
	gtk_toolbar_set_icon_size( GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_DND);
	#else
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_icon_size( GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_LARGE_TOOLBAR);
	#endif
	
	gtk_container_set_border_width(GTK_CONTAINER(toolbar), 0);

	GtkWidget *custom_icon1 = gtk_image_new_from_file("icons//save.png");
	save = gtk_tool_button_new(custom_icon1, "Save As");
	//gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(save), custom_icon1);
	//gtk_tool_button_set_label(GTK_TOOL_BUTTON(save), "Save As");
	gtk_tool_item_set_tooltip_text (save, "Save As");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save, -1);
	g_signal_connect(G_OBJECT(save), "clicked", G_CALLBACK(save_as_fcn), (gpointer)this );
	
	GtkWidget *custom_icon2 = gtk_image_new_from_file("icons//refresh.png");
	refresh = gtk_tool_button_new(custom_icon2, "Refresh");
	gtk_tool_item_set_tooltip_text (refresh, "Refresh Canvas");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), refresh, -1);
	g_signal_connect(G_OBJECT(refresh), "clicked", G_CALLBACK(refresh_drawing_area), (gpointer)this );
	
	ti_sep1 = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_sep1, -1); 
	
	GtkWidget *custom_icon3 = gtk_image_new_from_file("icons//zoom_in.png");
	zoom_in = gtk_tool_button_new(custom_icon3, "Zoom in");
	gtk_tool_item_set_tooltip_text (zoom_in, "Zoom In");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), zoom_in, -1);
	g_signal_connect(G_OBJECT(zoom_in), "clicked", G_CALLBACK(zoom_in_fcn), (gpointer)this );
	
	GtkWidget *custom_icon4 = gtk_image_new_from_file("icons//zoom_out.png");
	zoom_out = gtk_tool_button_new(custom_icon4, "Zoom out");
	gtk_tool_item_set_tooltip_text (zoom_out, "Zoom Out");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), zoom_out, -1);
	g_signal_connect(G_OBJECT(zoom_out), "clicked", G_CALLBACK(zoom_out_fcn), (gpointer)this );
	
	GtkWidget *custom_icon5 = gtk_image_new_from_file("icons//zoom_fit.png");
	zoom_fit = gtk_tool_button_new(custom_icon5, "Zoom fit");
	gtk_tool_item_set_tooltip_text (zoom_fit, "Zoom Fit");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), zoom_fit, -1);
	g_signal_connect(G_OBJECT(zoom_fit), "clicked", G_CALLBACK(zoom_fit_fcn), (gpointer)this );
	
	GtkWidget *custom_icon6 = gtk_image_new_from_file("icons//window_zoom.png");
	//zoom_window = gtk_tool_button_new(custom_icon6, "Zoom Window");
	zoom_window = gtk_toggle_tool_button_new();
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(zoom_window), custom_icon6);
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(zoom_window), "Window Zoom");
	gtk_tool_item_set_tooltip_text (zoom_window, "Window Zoom: Select a rectangular area to zoom");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), zoom_window, -1);
	g_signal_connect(G_OBJECT(zoom_window), "clicked", G_CALLBACK(turn_on_window_zoom_mode), (gpointer)this );
	
	ti_sep2 = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_sep2, -1);
	
	GtkWidget *custom_icon7 = gtk_image_new_from_file("icons//rotate_clockwise.png");
	ti_rotate_clockwise = gtk_tool_button_new(custom_icon7, "Rotate Clockwise");
	gtk_tool_item_set_tooltip_text (ti_rotate_clockwise, "Rotate Clockwise by 90 degrees");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_rotate_clockwise, -1);
	g_signal_connect(G_OBJECT(ti_rotate_clockwise), "clicked", G_CALLBACK(rotate_clockwise_fcn), (gpointer)this );
	
	GtkWidget *custom_icon8 = gtk_image_new_from_file("icons//rotate_anticlockwise.png");
	ti_rotate_anticlockwise = gtk_tool_button_new(custom_icon8, "Rotate Anti-Clockwise");
	gtk_tool_item_set_tooltip_text (ti_rotate_anticlockwise, "Rotate counter-clockwise by 90 degrees");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_rotate_anticlockwise, -1);
	g_signal_connect(G_OBJECT(ti_rotate_anticlockwise), "clicked", G_CALLBACK(rotate_anticlockwise_fcn), (gpointer)this );
	
	ti_sep3 = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_sep3, -1);
	
	GtkWidget *custom_icon9 = gtk_image_new_from_file("icons//image_maps.png");
	ti_imagemaps = gtk_toggle_tool_button_new();
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(ti_imagemaps), custom_icon9);
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(ti_imagemaps), "Image Maps");
	gtk_tool_item_set_tooltip_text (ti_imagemaps, "Show/Hide Image-maps");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_imagemaps, -1);
	g_signal_connect(G_OBJECT(ti_imagemaps), "clicked", G_CALLBACK(highlight_image_maps_fcn), (gpointer)this );
	
	GtkWidget *custom_icon10 = gtk_image_new_from_file("icons//fullscreen.png");
	ti_fullscreen = gtk_tool_button_new(custom_icon10, "Go Fullscreen");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_fullscreen, -1);
	gtk_tool_item_set_tooltip_text (ti_fullscreen, "Go Fullscreen");
	g_signal_connect(G_OBJECT(ti_fullscreen), "clicked", G_CALLBACK(toggle_fullscreen_fcn), (gpointer)this );
	
	GtkWidget *custom_icon11 = gtk_image_new_from_file("icons//exit_fullscreen.png");
	ti_exitfullscreen = gtk_tool_button_new(custom_icon11, "Exit Fullscreen");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_exitfullscreen, -1);
	gtk_tool_item_set_tooltip_text (ti_exitfullscreen, "Exit Fullscreen");
	g_signal_connect(G_OBJECT(ti_exitfullscreen), "clicked", G_CALLBACK(toggle_fullscreen_fcn), (gpointer)this );
	
	gtk_widget_set_sensitive(GTK_WIDGET(ti_fullscreen), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(ti_exitfullscreen), FALSE);
	
	ti_sep4 = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_sep4, -1);
	
	GtkWidget *custom_icon12 = gtk_image_new_from_file("icons//hshear_plus.png");
	ti_hshearplus = gtk_tool_button_new(custom_icon12, "Hshear+");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_hshearplus, -1);
	gtk_tool_item_set_tooltip_text (ti_hshearplus, "Increase Horizontal Shear");
	g_signal_connect(G_OBJECT(ti_hshearplus), "clicked", G_CALLBACK(increase_horizontal_shear_fcn), (gpointer)this );
	
	GtkWidget *custom_icon13 = gtk_image_new_from_file("icons//hshear_minus.png");
	ti_hshearminus = gtk_tool_button_new(custom_icon13, "Hshear-");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_hshearminus, -1);
	gtk_tool_item_set_tooltip_text (ti_hshearminus, "Decrease Horizontal Shear");
	g_signal_connect(G_OBJECT(ti_hshearminus), "clicked", G_CALLBACK(decrease_horizontal_shear_fcn), (gpointer)this );
	
	GtkWidget *custom_icon14 = gtk_image_new_from_file("icons//vshear_plus.png");
	ti_vshearplus = gtk_tool_button_new(custom_icon14, "Vshear+");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_vshearplus, -1);
	gtk_tool_item_set_tooltip_text (ti_vshearplus, "Increase Vertical Shear");
	g_signal_connect(G_OBJECT(ti_vshearplus), "clicked", G_CALLBACK(increase_vertical_shear_fcn), (gpointer)this );
	
	GtkWidget *custom_icon15 = gtk_image_new_from_file("icons//vshear_minus.png");
	ti_vshearminus = gtk_tool_button_new(custom_icon15, "Vshear-");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_vshearminus, -1);
	gtk_tool_item_set_tooltip_text (ti_vshearminus, "Decrease Vertical Shear");
	g_signal_connect(G_OBJECT(ti_vshearminus), "clicked", G_CALLBACK(decrease_vertical_shear_fcn), (gpointer)this );
	
	ti_sep5 = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti_sep5, -1);
	
	GtkWidget *custom_icon16 = gtk_image_new_from_file("icons//settings.png");
	preferences = gtk_tool_button_new(custom_icon16, "Settings");
	gtk_tool_item_set_tooltip_text (preferences, "Settings/Prefernces");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), preferences, -1);
	g_signal_connect(G_OBJECT(preferences), "clicked", G_CALLBACK(request_preferences_dialog), (gpointer)this );
	
	GtkWidget *custom_icon17 = gtk_image_new_from_file("icons//about.png");
	about = gtk_tool_button_new(custom_icon17, "Refresh");
	gtk_tool_item_set_tooltip_text (about, "About BridgeGL");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), about, -1);
	g_signal_connect(G_OBJECT(about), "clicked", G_CALLBACK(request_about_dialouge), (gpointer)this );
	
	toolbar_created = true;
	
	/*GdkColor color;
	gdk_color_parse ("#c0550d", &color);
	gtk_widget_modify_bg (toolbar, GTK_STATE_NORMAL, &color);*/
}
#endif

void gtk_win::add_button_to_sidepane( style_button *new_button )
{
	sidepane_buttons.push_back(new_button);
}

void gtk_win::redraw()
{
	 gtk_widget_queue_draw_area ( canvas,  tx, ty,
		             canvas_width, canvas_height);
	
	//gtk_statusbar_push( GTK_STATUSBAR(statusbar), 
	//	            gtk_statusbar_get_context_id( GTK_STATUSBAR(statusbar), "Refreshed Drawing Area"), "Refreshed Drawing Area");
}

void gtk_win::draw_virtual_scrollbars()
{
	//#CCCCCC"
	cr = gdk_cairo_create (canvas->window);
	cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 0.6);

	double xratio = (saved_xright - saved_xleft)/canvas_width; 
	double yratio = (saved_ybottom - saved_ytop)/canvas_height;
	
	double scroll_xleft   = xleft*xratio;
	if ( scroll_xleft < 0 )
		scroll_xleft = 0;

	double scroll_xright = canvas_width - (saved_xright - xright)*xratio;
	if ( scroll_xright > canvas_width )
		scroll_xright = canvas_width; 
	
	double scroll_ytop    = ytop*yratio;
	if ( scroll_ytop < 0 )
		scroll_ytop = 0;
	
	double scroll_ybottom = canvas_height - (saved_ybottom - ybottom)*yratio;
	if ( scroll_ybottom > canvas_height )
		scroll_ybottom = canvas_height;
	
	#ifdef DEBUG
		printf("scroll_xleft: %.2f, scroll_xright: %.2f, scroll_ytop: %.2f, scroll_ybottom: %.2f\n\n", 
	       		scroll_xleft, scroll_xright, scroll_ytop, scroll_ybottom);
	#endif
	
	cairo_move_to(cr, scroll_xleft, canvas_height - 8 );
	cairo_line_to(cr, scroll_xleft, canvas_height - 3 );
	cairo_line_to(cr, scroll_xright, canvas_height - 3 );
	cairo_line_to(cr, scroll_xright, canvas_height - 8 );
	cairo_line_to(cr, scroll_xleft, canvas_height - 8 );
	cairo_fill(cr);
	
	cairo_move_to(cr, 3, scroll_ytop );
	cairo_line_to(cr, 3, scroll_ybottom );
	cairo_line_to(cr, 8, scroll_ybottom );
	cairo_line_to(cr, 8, scroll_ytop );
	cairo_line_to(cr, 3, scroll_ytop );
	cairo_fill(cr);
	
	cairo_destroy(cr);
}

void gtk_win::update_statusbar_msg()
{
	gchar *str;
	double userx = win2user_x(worldx);
	double usery = win2user_y(worldy);
	
	if( window_zoom_mode_on && !in_window_zoom_mode )
		str = g_strdup_printf("  Canvas: [%d, %d], User (untransformed): [%.2f, %.2f], \
Mode: Window_zoom, Todo: Drag the mouse to zoom in on an area.", 
		                         worldx-sdp_width-1, worldy-top_gui_height-2, userx, usery );
	else if ( window_zoom_mode_on && in_window_zoom_mode )
		str = g_strdup_printf("  Canvas: [%d, %d], User (untransformed): [%.2f, %.2f], \
Mode: Window_zoom, Zoom Area: %.2fx%.2f from [%.2f, %.2f]", 
		                         worldx-sdp_width-1, worldy-top_gui_height-2, z_xright, z_ybottom, z_xright - z_xleft,
		                         z_ybottom - z_ytop, z_xleft, z_ytop );
	else
		str = g_strdup_printf("  Canvas: [%d, %d], User (untransformed): [%.2f, %.2f], \
Mode: Normal, Magnification: %.2fX", 
		                         worldx-sdp_width-1, worldy-top_gui_height-2, userx, usery, 
		                         max( (saved_xright - saved_xleft)/(xright-xleft), (saved_ybottom - saved_ytop)/(ybottom-ytop) ) );

	gtk_statusbar_push( GTK_STATUSBAR(statusbar), 
	                    gtk_statusbar_get_context_id( GTK_STATUSBAR(statusbar), str), str);
	g_free(str);
}

void gtk_win::update_statusbar_message_with( char* msg )
{
	gtk_statusbar_push( GTK_STATUSBAR(statusbar), 
	                    gtk_statusbar_get_context_id( GTK_STATUSBAR(statusbar), msg), msg);
}

// Set up the factors for transforming from the user world to GTK Windows coordinates.
// main concept taken from Prof vaughn betz's easygl code.                                                           
void gtk_win::update_transform()
{
	double mult, y1, y2, x1, x2;
	
	// gtk_drawing_area() coordinates go from (0,0) to 
	// (canvas_width-1, canvas_height-1)
	xmult = (canvas_width - 1)/(xright - xleft);
	ymult = (canvas_height - 1)/(ybottom - ytop);
	
	// Need to use same scaling factor to preserve aspect ratio
	if (fabs(xmult) <= fabs(ymult)) {
		mult = (fabs(ymult/xmult));
		y1 = ytop    - (ybottom-ytop)*(mult-1)/2;
		y2 = ybottom + (ybottom-ytop)*(mult-1)/2;
		ytop = y1;
		ybottom = y2;
	}
	else 
	{
		mult = (fabs(xmult/ymult));
		x1 = xleft - (xright-xleft)*(mult-1)/2;
		x2 = xright + (xright-xleft)*(mult-1)/2;
		xleft = x1;
		xright = x2;
	}
	
	xmult = (canvas_width - 1)/ (xright - xleft);
	ymult = (canvas_height - 1)/ (ybottom - ytop);
	
	// can queue an expose event here to automatically draw after
	// updating transform. But sometimes, we may need to update
	// the transform and wait for another event to draw. hence,
	// we always queue an expose event separately. 
	#ifdef DEBUG
		printf("\nxleft: %.2f, ytop: %.2f, xright: %.2f, ybottom: %.2f\n", 
		       xleft, ytop, xright, ybottom);
		printf("xmult: %.2f, ymult: %.2f, win_current_width: %d, win_current_height: %d, canvas_width: %d, canvas_height: %d\n", 
		       xmult, ymult, win_current_width, win_current_height, canvas_width, canvas_height);
	#endif
}

double gtk_win::user2win_x( double user_x )
{
	return (user_x - xleft)*xmult;
}
	
double gtk_win::user2win_y( double user_y )
{
	return (user_y - ytop)*ymult;
}
	
double gtk_win::win2user_x( double world_x )
{
	return ( xleft + (world_x - sdp_width - 1)/xmult );
}
	
double gtk_win::win2user_y( double world_y )
{
	return ( ytop + (world_y - top_gui_height - 2)/ymult);
}

double gtk_win::user2context_x( double user_x )
{
	return user_x*xmult;
}
	
double gtk_win::user2context_y( double user_y )
{
	return user_y*ymult;
}

void gtk_win::transform_user_to_window(cairo_t* cr, double &x, double &y)
{
	x = x*xmult;
	y = y*ymult;
	cairo_user_to_device( cr, &x, &y );
	x = x + sdp_width + 1;
	y = y + top_gui_height + 2;
}

void gtk_win::focus_on_area( double _xleft, double _ytop, double _xright, double _ybottom )
{
	// no check on limits, is probably dangerous, could overflow or underflow        
	xleft = _xleft;
	xright = _xright;
	ytop = _ytop;
	ybottom = _ybottom;
	
	update_transform();
	
        gtk_widget_queue_draw_area ( canvas,  tx, ty,
		             canvas_width, canvas_height);
}

void gtk_win::focus()
{
	update_transform();		
        gtk_widget_queue_draw_area ( canvas,  tx, ty,
		             canvas_width, canvas_height);
}


void gtk_win::restore_to_onset_view( bool remove_transforms )
{
	// bring back the saved coordinates
	xleft = saved_xleft;
	xright = saved_xright;
	ytop = saved_ytop;
	ybottom = saved_ybottom;
	
	// remove the transforms
	if (remove_transforms)
	{
		horizontal_shear = 0;
		vertical_shear = 0;
		angle_multiplier = 0;
		rx_translate = 0;
		ry_translate = 0;
	}
	focus();
}

void gtk_win::translate_up()
{
	double ystep = (ybottom - ytop)/translate_u_factor;	//user2win_y(50); 
	ytop    -= ystep;
	ybottom -= ystep;
	update_transform();
	
	gtk_widget_queue_draw_area ( canvas,  tx, ty,
		             canvas_width, canvas_height);
}

void gtk_win::translate_down()
{
	double ystep = (ybottom - ytop)/translate_d_factor;	//user2win_y(50); 
	ytop    += ystep;
	ybottom += ystep;
	update_transform();
	
	gtk_widget_queue_draw_area ( canvas,  tx, ty,
		             canvas_width, canvas_height);
}

void gtk_win::translate_left()
{
	double xstep = (xright - xleft)/translate_l_factor;	//user2win_x(50); 
	xleft  -= xstep;
	xright -= xstep; 
	update_transform();
	
	gtk_widget_queue_draw_area ( canvas,  tx, ty,
		             canvas_width, canvas_height);
}

void gtk_win::translate_right()
{
	double xstep = (xright - xleft)/translate_r_factor;	//user2win_x(50); 
	xleft  += xstep;
	xright += xstep; 
	update_transform();
	
	gtk_widget_queue_draw_area ( canvas,  tx, ty,
		             canvas_width, canvas_height);
}

void gtk_win::translate_coordinates(cairo_t* cr, double _tx, double _ty )
{
	user_tx += _tx;
	user_ty += _ty;
	//cairo_translate(cr,  user2win_x(0) + _tx*xmult, user2win_y(0) + _ty*ymult);
	cairo_translate(cr, _tx*xmult, _ty*ymult);
}

bool gtk_win::rect_off_screen (cairo_t* cr, double x1, double y1, double x2, double y2) 
{	
	double mapped_x1 = xmult*x1;
	double mapped_y1 = ymult*y1;
	cairo_user_to_device(cr, &mapped_x1, &mapped_y1);
	
	double mapped_x2 = xmult*x2;
	double mapped_y2 = ymult*y2;
	cairo_user_to_device(cr, &mapped_x1, &mapped_y2);
	
	// instead, work in canvas coordinates directly
	if (mapped_x1 < 0 && mapped_x2 < 0)
	{
		return (1);
	}
	
	if (mapped_x1 > canvas_width && mapped_x2 > canvas_width)
	{
		return (1);
	}
	
	if (mapped_y1 < 0 && mapped_y2 < 0)
	{
		return (1);
	}
	
	if (mapped_y1 > canvas_height && mapped_y2 > canvas_height)
	{
		return (1);
	}
	
	return (0);
}

void gtk_win::clearscreen(cairo_t* cr)
{
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_paint(cr);
}

void gtk_win::setbgcolor(cairo_t*cr, double red, double green, double blue)
{
	cairo_set_source_rgb(cr, red, green, blue);
	cairo_save(cr);
	fillrect(cr, saved_xleft, saved_ytop, saved_xright, saved_ybottom);
	cairo_restore(cr);
}

void gtk_win::setcolor(cairo_t* cr, double red, double green, double blue, double alpha)
{
	cairo_set_source_rgba(cr, red, green, blue, alpha);
}

void gtk_win::setradialgradient(cairo_t* cr, double cx0, double cy0, double r0, double cx1, double cy1, double r1)
{
	
}

void gtk_win::setlineargradient(cairo_t* cr, double cx0, double cy0, double cx1, double cy1)
{
	
}

void gtk_win::setlinestyle(cairo_t* cr, int linestyle)
{
	double dashes[3][2] =
	{
	{0.0, 0.0},
	{4.0, 2.0},
	{2.0, 4.0},
	};
	int num_dashes = (linestyle > 0) ? 1 : 0;
	cairo_set_dash(cr, dashes[linestyle], num_dashes, 0.0);
}

void gtk_win::setlinestyle(cairo_t* cr, int num_dashes, double *dashes)
{
	//int num_dashes = (linestyle > 0) ? 1 : 0;
	cairo_set_dash(cr, dashes, num_dashes, 0.0);
}

void gtk_win::setlinewidth(cairo_t* cr, double linewidth)
{
	cairo_set_line_width(cr, linewidth);
}

void gtk_win::drawline( cairo_t* cr, double x1, double y1, double x2, double y2)
{
	if (rect_off_screen(cr,x1,y1,x2,y2))	return;
	
	//cairo_move_to(cr, user2win_x(x1), user2win_y(y1) );
	//cairo_line_to(cr, user2win_x(x2), user2win_y(y2) );
	cairo_move_to(cr, x1*xmult, y1*ymult );
	cairo_line_to(cr, x2*xmult, y2*ymult );
	cairo_stroke(cr);
	 stats->count_lines++;
}

void gtk_win::setfontsize(cairo_t* cr, int pointsize)
{
	#ifdef SCALE_TEXT
		//double fsize = pointsize*max( (saved_xright - saved_xleft)/(xright-xleft), (saved_ybottom - saved_ytop)/(ybottom-ytop) );
		double fsize = pointsize*max( canvas_width/(xright-xleft), canvas_height/(ybottom-ytop) );
		cairo_set_font_size(cr, fsize);
	#else
		cairo_set_font_size(cr, (double)pointsize); 
	#endif
}

void gtk_win::setfontface( cairo_t* cr, char* fontface, cairo_font_slant_t slant, cairo_font_weight_t weight )
{
	cairo_select_font_face (cr, fontface, slant, weight); 
}

void gtk_win::drawtext(cairo_t* cr, double xc, double yc, char *text, double boundx)
{
	cairo_text_extents_t extents;
	cairo_text_extents(cr, text, &extents);
	
	if (rect_off_screen(cr, xc, yc-extents.height, xc+extents.width, yc))	
	{
		//printf("rectangle outside visible area\n");
		return;
	}
	
	if ( extents.width > fabs(xmult*boundx) )
	{
		printf("width more than bound specified: textwidth: %g and canvas_boundx: %.2f\n", 
		       extents.width,fabs(xmult*boundx) );
		return;
	}
	
	//cairo_move_to(cr, user2win_x(xc), user2win_y(yc) );
	cairo_move_to(cr, xmult*xc, ymult*yc );
	cairo_show_text(cr, text);
	cairo_close_path (cr);
	 stats->count_text++;
	
}

void gtk_win::drawellipticarc(cairo_t* cr, double xcen, double ycen, double radx, double rady, 
                                           double startang, double angextent)
{
	if (rect_off_screen(cr,xcen-radx, ycen-rady, xcen+radx, ycen+rady))	return;
	
	cairo_save (cr);
	//cairo_translate (cr, user2win_x(xcen), user2win_y(ycen) );
	cairo_translate (cr, xmult*xcen, ymult*ycen );
	cairo_scale (cr, fabs(xmult*radx), fabs(ymult*rady));
	cairo_arc (cr, 0., 0., 1., startang*(M_PI/180.0), angextent*(M_PI/180.0) );
	cairo_restore (cr);
	
	cairo_stroke(cr);
	 stats->count_d_ellipses++;
}

void gtk_win::fillellipticarc(cairo_t* cr, double xcen, double ycen, double radx, double rady, 
                                           double startang, double angextent)
{
	if (rect_off_screen(cr,xcen-radx, ycen-rady, xcen+radx, ycen+rady))	return;
	
	cairo_move_to(cr, xmult*xcen, ymult*ycen );
	cairo_line_to(cr, xmult*xcen + xmult*radx*cos(startang*(M_PI/180.0)), 
	                  ymult*ycen + ymult*rady*sin(startang*(M_PI/180.0)) );
	cairo_save(cr);
	cairo_translate (cr, xmult*xcen, ymult*ycen );
	cairo_scale (cr, fabs(xmult*radx), fabs(ymult*rady));
	cairo_arc (cr, 0., 0., 1., startang*(M_PI/180.0), angextent*(M_PI/180.0) );
	cairo_restore (cr);
	
	cairo_fill(cr);
	 stats->count_f_ellipses++;
}


void gtk_win::drawarc(cairo_t* cr, double xcen, double ycen, double rad, double startang, double angextent)
{
	if (rect_off_screen(cr,xcen-rad, ycen-rad, xcen+rad ,ycen+rad))	return;
	
	cairo_move_to(cr, xmult*xcen + fabs(xmult*rad)*cos(startang*(M_PI/180.0)), 
	                  ymult*ycen + fabs(ymult*rad)*sin(startang*(M_PI/180.0)) );
	cairo_arc(cr, xmult*xcen, ymult*ycen, fabs(xmult*rad), 
	              startang*(M_PI/180.0), angextent*(M_PI/180.0));
	cairo_stroke(cr);
	 stats->count_d_arcs++;
}

void gtk_win::fillarc(cairo_t* cr, double xcen, double ycen, double rad, double startang, double angextent)
{
	if (rect_off_screen(cr,xcen-rad, ycen-rad, xcen+rad ,ycen+rad))	return;	
	fillellipticarc(cr, xcen, ycen, rad, rad, startang, angextent);
	 stats->count_f_arcs++;
}
  
void gtk_win::drawrect(cairo_t* cr, double x1, double y1, double x2, double y2)
{
	if (rect_off_screen(cr,x1,y1,x2,y2))	return;
		
	/*cairo_move_to(cr, user2win_x(x1), user2win_y(y1) );
	cairo_line_to(cr, user2win_x(x1), user2win_y(y2) );
	cairo_line_to(cr, user2win_x(x2), user2win_y(y2) );
	cairo_line_to(cr, user2win_x(x2), user2win_y(y1) );
	cairo_line_to(cr, user2win_x(x1), user2win_y(y1) );*/
	
	cairo_move_to(cr, xmult*x1, ymult*y1 );
	cairo_line_to(cr, xmult*x1, ymult*y2 );
	cairo_line_to(cr, xmult*x2, ymult*y2 );
	cairo_line_to(cr, xmult*x2, ymult*y1 );
	cairo_close_path (cr);
	cairo_stroke(cr);
	 stats->count_d_rects++;
}

void gtk_win::fillrect(cairo_t* cr, double x1, double y1, double x2, double y2)
{
	if (rect_off_screen(cr,x1,y1,x2,y2))	return;
	
	/*cairo_move_to(cr, user2win_x(x1), user2win_y(y1) );
	cairo_line_to(cr, user2win_x(x1), user2win_y(y2) );
	cairo_line_to(cr, user2win_x(x2), user2win_y(y2) );
	cairo_line_to(cr, user2win_x(x2), user2win_y(y1) );
	cairo_line_to(cr, user2win_x(x1), user2win_y(y1) );*/
	
	cairo_move_to(cr, xmult*x1, ymult*y1 );
	cairo_line_to(cr, xmult*x1, ymult*y2 );
	cairo_line_to(cr, xmult*x2, ymult*y2 );
	cairo_line_to(cr, xmult*x2, ymult*y1 );
	cairo_close_path (cr);
	cairo_fill(cr);
	 stats->count_f_rects++;
}

void gtk_win::drawpolygon(cairo_t* cr, vector<double> &x, vector<double> &y)
{
	bool to_stroke = drawpolypath(cr, x, y);
	
	if( to_stroke )
		cairo_stroke (cr);
	
	 stats->count_d_poly++;
}

void gtk_win::fillpolygon(cairo_t* cr, vector<double> &x, vector<double> &y)
{
	bool to_fill = drawpolypath(cr, x, y);
	
	if( to_fill )
		cairo_fill(cr);
	
	 stats->count_f_poly++;
}

bool gtk_win::drawpolypath(cairo_t* cr, vector<double> &x, vector<double> &y)
{
	if ( ( x.size() != y.size() ) || x.size() <= 1 || y.size() <= 1 )
	{
		fprintf(stderr, "ERROR: cannot draw polygon, illegal input\n");
		return false;
	}
	
	double xmin = x[0], xmax = x[0];
	double ymin = y[0], ymax = y[0];
	
	
	for ( unsigned int i=1; i < x.size(); i++) 
	{
		xmin = min (xmin, x[i] );
		xmax = max (xmax, x[i] );
		ymin = min (ymin, y[i] );
		ymax = max (ymax, y[i] );
	}
	
	if (rect_off_screen(cr,xmin,ymin,xmax,ymax))
		return false;

	/*cairo_move_to(cr, user2win_x(x[0]), user2win_y(y[0]) );
	for( unsigned int i=1; i < x.size(); i++ )
	{
		cairo_line_to(cr, user2win_x(x[i]), user2win_y(y[i]) );
	}*/
	cairo_move_to(cr, xmult*x[0], ymult*y[0] );
	for( unsigned int i=1; i < x.size(); i++ )
	{
		cairo_line_to(cr, xmult*x[i], ymult*y[i] );
	}
	cairo_close_path (cr);
	
	return true;
}

bool gtk_win::drawroundedrectpath( cairo_t* cr, double x1, double y1, double x2, double y2, double xarc, double yarc )
{
	if (rect_off_screen(cr,x1,y1,x2,y2))	return false;
	
	xarc = min( xarc, fabs(x1-x2)/2 );
	xarc = min( yarc, fabs(y1-y2)/2 );
	
	double xcen1 = x1+xarc;
	double xcen2 = x2-xarc;
	double ycen1 = y1+yarc;
	double ycen2 = y2-yarc; 
	
	cairo_move_to( cr, xmult*x1, ymult*ycen2 );
	
	cairo_line_to( cr, xmult*x1, ymult*ycen1 );
	cairo_save(cr);
	cairo_translate (cr, xmult*xcen1, ymult*ycen1 );
	cairo_scale (cr, fabs(xmult*xarc), fabs(ymult*yarc));
	cairo_arc (cr, 0., 0., 1., M_PI, 1.5*M_PI );
	cairo_restore (cr);
	
	cairo_line_to( cr, xmult*xcen2, ymult*y1 );
	cairo_save(cr);
	cairo_translate (cr, xmult*xcen2, ymult*ycen1 );
	cairo_scale (cr, fabs(xmult*xarc), fabs(ymult*yarc));
	cairo_arc (cr, 0., 0., 1., 1.5*M_PI, 2*M_PI );
	cairo_restore (cr);
	
	cairo_line_to( cr, xmult*x2, ymult*ycen2 );
	cairo_save(cr);
	cairo_translate (cr, xmult*xcen2, ymult*ycen2 );
	cairo_scale (cr, fabs(xmult*xarc), fabs(ymult*yarc));
	cairo_arc (cr, 0., 0., 1., 0, 0.5*M_PI );
	cairo_restore (cr);
	
	cairo_line_to( cr, xmult*xcen1, ymult*y2 );
	cairo_save(cr);
	cairo_translate (cr, xmult*xcen1, ymult*ycen2 );
	cairo_scale (cr, fabs(xmult*xarc), fabs(ymult*yarc));
	cairo_arc (cr, 0., 0., 1., 0.5*M_PI, 1*M_PI );
	cairo_restore (cr);
	
	return true;
}

void gtk_win::drawroundedrect( cairo_t* cr, double x1, double y1, double x2, double y2, double xarc, double yarc )
{
	if ( xarc <= 0 || yarc <= 0)
	{
		drawrect(cr, x1, y1, x2, y2);
		return;
	}
	bool to_stroke = drawroundedrectpath(cr, x1, y1, x2, y2, xarc, yarc);
	
	if ( to_stroke )	
		cairo_stroke(cr);
	 stats->count_d_roundrect++;
}

void gtk_win::fillroundedrect( cairo_t* cr, double x1, double y1, double x2, double y2, double xarc, double yarc )
{
	if ( xarc <= 0 || yarc <= 0)
	{
		fillrect(cr, x1, y1, x2, y2);
		return;
	}
	bool to_fill = drawroundedrectpath(cr, x1, y1, x2, y2, xarc, yarc);
	
	if ( to_fill )	
		cairo_fill(cr);
	 stats->count_f_roundrect++;
}

/* It is the responsibility of the user to ensure that his line falls within screen width at given font size*/
void gtk_win::drawtextballoon( cairo_t *cr, double user_x1, double user_y1, double tolerance, vector<string>* arr, 
                               int fontsize, char* fontface, double *rgb_border, double *rgba_fill, double *rgb_text )
{
	cairo_save(cr);
	cairo_identity_matrix(cr);
	cairo_translate (cr, user2win_x(0), user2win_y(0) );
	
	if ( rect_off_screen(cr, user_x1-tolerance, user_y1-tolerance, user_x1+tolerance, user_y1+tolerance) )
	{
		cairo_restore(cr);
	 	return;	
	}
	
	cairo_set_font_size(cr, 1.6*fontsize);
	cairo_select_font_face (cr, fontface, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

	cairo_text_extents_t extents;
	cairo_text_extents(cr, "M", &extents);
	
	cairo_set_font_size(cr, (double)fontsize);
	
	// top and bottom margin
	double tb_margin = 2.0*extents.height;
	
	// left and right side margin
	double lr_margin = 2*extents.width;
	
	double diagbox_ascent = 30;
	double diagbox_height = extents.height*arr->size() + 2*tb_margin;
	double diagbox_width= -1;
	for ( unsigned int i = 0; i < arr->size(); i++ )
	{
		cairo_text_extents(cr, (*arr)[i].c_str(), &extents);
		if ( diagbox_width < extents.width )
		{
			diagbox_width = extents.width;
		}
	}
	
	diagbox_width  = diagbox_width + 2*lr_margin + 20;
	
	//double cx1 = user2win_x(user_x1) + user_tx*xmult;
	//double cy1 = user2win_y(user_y1) + user_ty*ymult;
	double cx1 = user_x1*xmult;
	double cy1 = user_y1*ymult;
	
	double offsetx = user2win_x(0);
	double offsety = user2win_y(0);
	
	double temp_c2a = cx1 + diagbox_width;
	double temp_c2b = cx1 - diagbox_width;
	double cx2 = ( (temp_c2a + offsetx - canvas_width) > (0 -temp_c2b - offsetx) )?
	               temp_c2b : temp_c2a;
	
	//printf("temp_c2a+...: %g, temp_c2b-...: %g and cx2: %g, offsetx: %g\n", 
	//       temp_c2a + offsetx - canvas_width, temp_c2b - offsetx, cx2, offsetx);
	
	temp_c2a = cy1 + diagbox_height + diagbox_ascent;
	temp_c2b = cy1 - diagbox_height - diagbox_ascent;
	double cy2 = ( (temp_c2a + offsety - canvas_height) > (0 - temp_c2b - offsety) )?
	               temp_c2b : temp_c2a;
	
	
	/*double cx2 = ( cx1 + diagbox_width > canvas_width )?
		       cx1 - diagbox_width: cx1 + diagbox_width;
	double cy2 = ( cy1 + diagbox_height + diagbox_ascent  > canvas_height )?
		       cy1 - diagbox_height - diagbox_ascent:
		       cy1 + diagbox_height + diagbox_ascent;*/
	
	double cdelx = cx2-cx1;	
	double c_xanchor1 = cx1 + 0.1*cdelx; 
	double c_xanchor2 = cx1 + 0.25*cdelx;
	double c_yanchor = ((cy1<cy2)?cy1 + diagbox_ascent :cy1 - diagbox_ascent);
	
	double x[7] = {cx1, c_xanchor1,      cx1, cx1, cx2,       cx2, c_xanchor2};
	double y[7] = {cy1, c_yanchor, c_yanchor, cy2, cy2, c_yanchor, c_yanchor };
	
	cairo_set_source_rgba(cr, rgba_fill[0], rgba_fill[1], rgba_fill[2], rgba_fill[3]);

	cairo_move_to(cr, x[0], y[0] );
	for( unsigned int i=1; i < 7; i++ )
	{
		cairo_line_to(cr, x[i], y[i] );
	}
	cairo_close_path (cr);
	cairo_fill(cr);
	
	cairo_set_source_rgb(cr, rgb_border[0], rgb_border[1], rgb_border[2]);
	cairo_move_to(cr, x[0], y[0] );
	setlinewidth(cr, 2);
	for( unsigned int i=1; i < 7; i++ )
	{
		cairo_line_to(cr, x[i], y[i] );
	}
	cairo_close_path (cr);
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
	cairo_stroke(cr);
	
	double xref = lr_margin + ((cx1 < cx2)?cx1:cx2);
	double yref = tb_margin + ((c_yanchor < cy2)?c_yanchor:cy2);

	cairo_set_source_rgb(cr, rgb_text[0], rgb_text[1], rgb_text[2]);
	for ( unsigned int i = 0; i < arr->size(); i++ )
	{
		if ( (*arr)[i] == "--" )
			draw_separator( cr, xref, yref+0.54*(i-0.5)*tb_margin, xref+diagbox_width/2, 3, rgb_text );
		else
		{
			cairo_move_to( cr, xref, yref+0.54*i*tb_margin );
			cairo_show_text(cr, (*arr)[i].c_str() );
		}
	}
	cairo_restore(cr);
	 stats->count_textballoons++;
}

void gtk_win::draw_separator( cairo_t* cr, double sep_x1, double sep_y1, double sep_x2, double height, double *rgb_sep )
{
	cairo_save(cr);
	
	cairo_pattern_t *pat;
	pat = cairo_pattern_create_linear (sep_x1, sep_y1,  sep_x2, sep_y1);
	cairo_pattern_add_color_stop_rgba (pat, 1, 1, 1, 1, 0);
	cairo_pattern_add_color_stop_rgba (pat, 0.5, rgb_sep[0], rgb_sep[1], rgb_sep[2], 1);
	cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 0);
	cairo_rectangle (cr, sep_x1, sep_y1, sep_x2-sep_x1, height);
	cairo_set_source (cr, pat);
	cairo_fill (cr);
	cairo_pattern_destroy (pat);
	
	cairo_restore(cr);
}

/* Functions which aid in saving viewport to file */
static void file_type_selected(GtkWidget *widget, gpointer data)
{	
	gtk_win* g_win = (gtk_win*) data;
	g_win->save_as_file_type(widget, accessibility_key);
}

void gtk_win::save_as_file_type(GtkWidget *widget, string key)
{
	if ( key == access_key )
	{
		gchar *active_text =  gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
	
		if ( g_strcmp0 (active_text, "PDF") == 0 )
			type = PDF;
		else if ( g_strcmp0 (active_text, "PNG") == 0 )
			type = PNG;
		else if ( g_strcmp0 (active_text, "SVG") == 0 )
			type = SVG;
		else if ( g_strcmp0 (active_text, "PS") == 0 )
			type = PS;
	}
	else
	{
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::save_as_file_type() \n");
	}
}

void gtk_win::save_as()
{
	GtkWidget *dialog;
	GtkWidget *combo_box;
	
	dialog = gtk_file_chooser_dialog_new ("Save File",
				      GTK_WINDOW(mainwin),
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
	combo_box = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), "PDF");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), "PNG");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), "SVG");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), "PS");
	gtk_combo_box_set_active (GTK_COMBO_BOX(combo_box), 1);
	g_signal_connect(G_OBJECT(combo_box), "changed", G_CALLBACK(file_type_selected), (gpointer)this);
	gtk_file_chooser_set_extra_widget ( GTK_FILE_CHOOSER(dialog), combo_box);
	
	/*if (user_edited_a_new_document)
	{
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), default_folder_for_saving);
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), "Untitled document");
	}
	else
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), filename_for_existing_document);*/

	char* filename;
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		string fname(filename);
		if (fname != "")
		{
			int index = fname.find_last_of(".");

			if (index != -1)
			{
				string ftype = fname.substr(index+1); 
				if ( ftype == "pdf" )
					save_as_pdf(filename, 0);
				else if ( ftype == "png" )
					save_as_png(filename, 0);
				else if ( ftype == "svg" )
					save_as_svg(filename, 0);
				else if ( ftype == "ps" )
					save_as_ps(filename, 0);
				else if ( type == PDF )
					save_as_pdf(filename, 1);
				else if ( type == PNG )
					save_as_png(filename, 1);
				else if ( type == SVG )
					save_as_svg(filename, 1);
				else if ( type == PS )
					save_as_ps(filename, 1);
			}
			else if ( type == PDF )
				save_as_pdf(filename, 1);
			else if ( type == PNG )
				save_as_png(filename, 1);
			else if ( type == SVG )
				save_as_svg(filename, 1);
			else if ( type == PS )
				save_as_ps(filename, 1);
		}
		//g_free (filename);
		 stats->count_saved_to_file++;
	}
	//g_free (filename);
	gtk_widget_destroy (combo_box);
	gtk_widget_destroy (dialog);
}

void gtk_win::save_as_pdf( char* filename, bool appendtype )
{
	if ( appendtype )
		strcat(filename, ".pdf");
        cs = cairo_pdf_surface_create(filename, canvas_width,canvas_height);
        cr = cairo_create(cs);
        
        // repaint method
        cairo_identity_matrix(cr);
        user_tx = 0; user_ty = 0;
        cairo_translate (cr, user2win_x(0), user2win_y(0) );
        
        // paint the canvas as per transformed and rotated coordinates
	paint_canvas(cr);
	
        cairo_destroy(cr);
        cairo_surface_destroy(cs);
}
void gtk_win::save_as_png( char* filename, bool appendtype )
{
	if ( appendtype )
		strcat(filename, ".png");
        cs = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, canvas_width, canvas_height);
        cr = cairo_create(cs);
        
        // repaint method
        cairo_identity_matrix(cr);
        user_tx = 0; user_ty = 0;
        cairo_translate (cr, user2win_x(0), user2win_y(0) );
        
        // paint the canvas as per transformed and rotated coordinates
	paint_canvas(cr);
	
	cairo_surface_write_to_png(cs, filename);
        cairo_destroy(cr);
        cairo_surface_destroy(cs);
}
void gtk_win::save_as_svg( char* filename, bool appendtype )
{
	if ( appendtype )
		strcat(filename, ".svg");
        cs = cairo_svg_surface_create(filename, canvas_width, canvas_height);
        cr = cairo_create(cs);
        
        // repaint method
        cairo_identity_matrix(cr);
        user_tx = 0; user_ty = 0;
        cairo_translate (cr, user2win_x(0), user2win_y(0) );
        
        // paint the canvas as per transformed and rotated coordinates
	paint_canvas(cr);
	
        cairo_destroy(cr);
        cairo_surface_destroy(cs);
}
void gtk_win::save_as_ps( char* filename, bool appendtype )
{
	if ( appendtype )
		strcat(filename, ".ps");
        cs = cairo_ps_surface_create(filename, canvas_width, canvas_height);
        cr = cairo_create(cs);
        
        // repaint method
        cairo_identity_matrix(cr);
        user_tx = 0; user_ty = 0;
        cairo_translate (cr, user2win_x(0), user2win_y(0) );
        
        // paint the canvas as per transformed and rotated coordinates
	paint_canvas(cr);
	
        cairo_destroy(cr);
        cairo_surface_destroy(cs);
}
/* end of save_as functionality */

/************** Its all about the preferences ***************/
static void update_values_and_destroy_widget(GtkWidget *widget, gint response_id, gpointer data)
{
	gtk_win* g_win = (gtk_win*)data;
	g_win->update_values_for_zoom_translate_shear(accessibility_key);
	
	gtk_widget_destroy(widget);
}


void gtk_win::update_values_for_zoom_translate_shear(string key)
{
	if ( access_key == key )
	{
		double new_zoomin_value  = gtk_spin_button_get_value_as_float
		( GTK_SPIN_BUTTON (zoomin_spinner) );
		double new_zoomout_value = gtk_spin_button_get_value_as_float
		( GTK_SPIN_BUTTON (zoomout_spinner) );
	
		int new_translate_u_factor = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON (up_spinner) );
		int new_translate_d_factor = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON (down_spinner) );
		int new_translate_l_factor = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON (left_spinner) );
		int new_translate_r_factor = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON (right_spinner) );
		
		double new_hshear_angle  = gtk_spin_button_get_value_as_float
		( GTK_SPIN_BUTTON (hshear_spinner) );
		double new_vshear_angle = gtk_spin_button_get_value_as_float
		( GTK_SPIN_BUTTON (vshear_spinner) );
		
		set_zoom_in_factor( new_zoomin_value );
		set_zoom_out_factor( new_zoomout_value );
		set_scroll_zoom_in_factor( new_zoomin_value );
		set_scroll_zoom_out_factor( new_zoomout_value );
		set_translate_up_factor( new_translate_u_factor );
		set_translate_down_factor( new_translate_d_factor );
		set_translate_left_factor( new_translate_l_factor );
		set_translate_right_factor( new_translate_r_factor );
		set_horizontal_shear_delta( new_hshear_angle );
		set_vertical_shear_delta( new_vshear_angle );
		char status[40];
		sprintf(status, "Updated values for Zoom, Translate and Shear Operations");
		gtk_statusbar_push( GTK_STATUSBAR(statusbar), 
				    gtk_statusbar_get_context_id( GTK_STATUSBAR(statusbar), status), status );
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::update_values_for_zoom_translate_shear() \n");
}

// functions to lock all toggle spinbuttons together, so that their values change simultaneolusly
static void toggle_translate_lock_state_fcn (GtkToggleButton *togglebutton, gpointer data)
{
	gtk_win* g_win = (gtk_win*)data;
	g_win->toggle_translate_lock_state(accessibility_key);
}

static void lock_translate_spinners_fcn (GtkSpinButton *spinbutton, gpointer data)
{
	gtk_win* g_win = (gtk_win*)data;
	g_win->lock_translate_spinners(spinbutton, accessibility_key);
}

void gtk_win::lock_translate_spinners(GtkSpinButton *spinbutton, string key )
{
	if ( access_key == key )
	{
		int new_translate_factor = gtk_spin_button_get_value_as_int( spinbutton );
		gtk_spin_button_set_value( GTK_SPIN_BUTTON (up_spinner), new_translate_factor);
		gtk_spin_button_set_value( GTK_SPIN_BUTTON (down_spinner), new_translate_factor);
		gtk_spin_button_set_value( GTK_SPIN_BUTTON (left_spinner), new_translate_factor);
		gtk_spin_button_set_value( GTK_SPIN_BUTTON (right_spinner), new_translate_factor);
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::lock_translate_spinners() \n");
	
}

void gtk_win::toggle_translate_lock_state( string key )
{
	if ( access_key == key )
	{
		if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(translate_lock_button) ) )
		{
			up_spinner_handler_id = g_signal_connect(G_OBJECT(up_spinner),"value-changed",
			G_CALLBACK(lock_translate_spinners_fcn), (gpointer)this);
			down_spinner_handler_id = g_signal_connect(G_OBJECT(down_spinner),"value-changed",
			G_CALLBACK(lock_translate_spinners_fcn), (gpointer)this);
			left_spinner_handler_id = g_signal_connect(G_OBJECT(left_spinner),"value-changed",
			G_CALLBACK(lock_translate_spinners_fcn), (gpointer)this);
			right_spinner_handler_id = g_signal_connect(G_OBJECT(right_spinner),"value-changed",
			G_CALLBACK(lock_translate_spinners_fcn), (gpointer)this);
		}
		else
		{
			g_signal_handler_disconnect(GTK_OBJECT(up_spinner), up_spinner_handler_id);
			g_signal_handler_disconnect(GTK_OBJECT(down_spinner), down_spinner_handler_id);
			g_signal_handler_disconnect(GTK_OBJECT(left_spinner), left_spinner_handler_id);
			g_signal_handler_disconnect(GTK_OBJECT(right_spinner), right_spinner_handler_id);
		}
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::toggle_translate_lock_state() \n");
}

// functions to lock all shear spinbuttons together, so that their values change simultaneolusly
static void toggle_shear_lock_state_fcn (GtkToggleButton *togglebutton, gpointer data)
{
	gtk_win* g_win = (gtk_win*)data;
	g_win->toggle_shear_lock_state(accessibility_key);
}

static void lock_shear_spinners_fcn (GtkSpinButton *spinbutton, gpointer data)
{
	gtk_win* g_win = (gtk_win*)data;
	g_win->lock_shear_spinners(spinbutton, accessibility_key);
}

void gtk_win::lock_shear_spinners(GtkSpinButton *spinbutton, string key )
{
	if ( access_key == key )
	{
		int new_shear_angle = gtk_spin_button_get_value_as_float( spinbutton );
		gtk_spin_button_set_value( GTK_SPIN_BUTTON (hshear_spinner), new_shear_angle);
		gtk_spin_button_set_value( GTK_SPIN_BUTTON (vshear_spinner), new_shear_angle);
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::lock_shear_spinners() \n");
}

void gtk_win::toggle_shear_lock_state( string key )
{
	if ( access_key == key )
	{
		if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(shear_lock_button) ) )
		{
			hshear_spinner_handler_id = g_signal_connect(G_OBJECT(hshear_spinner),"value-changed",
			G_CALLBACK(lock_shear_spinners_fcn), (gpointer)this);
			vshear_spinner_handler_id = g_signal_connect(G_OBJECT(vshear_spinner),"value-changed",
			G_CALLBACK(lock_shear_spinners_fcn), (gpointer)this);
		}
		else
		{
			g_signal_handler_disconnect(GTK_OBJECT(hshear_spinner), hshear_spinner_handler_id);
			g_signal_handler_disconnect(GTK_OBJECT(vshear_spinner), vshear_spinner_handler_id);
		}
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::toggle_translate_lock_state() \n");
}

// main function to open preferences dialouge
void gtk_win::open_preferences_dialouge( string key )
{
	if ( access_key == key )
	{
		GtkWidget *dialog, *content_area;
		GtkWidget *frame;
		GtkWidget *hbox;
		GtkWidget *main_vbox;
		GtkWidget *label;
		GtkWidget *vbox;
		
		char str[20];
		guint char_width_label = 19;
		
		/* Create the widgets */
	  	dialog = gtk_dialog_new_with_buttons ("Preferences",
		                                 GTK_WINDOW(mainwin),
		                                 GTK_DIALOG_MODAL, GTK_STOCK_CLOSE,
		                                 GTK_RESPONSE_NONE,
		                                 NULL);
		content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
		
		main_vbox = gtk_vbox_new (FALSE, 5);
		gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 10);
		gtk_container_add (GTK_CONTAINER (content_area), main_vbox);
		
		//  **********************  Zoom preferences 
		// for toolbar buttons and scroll-zoom
		frame = gtk_frame_new ("Zoom Preferences");
		gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 0);
		
		vbox = gtk_vbox_new (FALSE, 0);
		gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
		gtk_container_add (GTK_CONTAINER (frame), vbox);

		// Zoom in
		hbox = gtk_hbox_new (FALSE, 0);
		label = gtk_label_new ("Zoom-in by:");
		gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
		gtk_label_set_width_chars (GTK_LABEL(label), char_width_label);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 5);
		
		adj = (GtkAdjustment *) gtk_adjustment_new (zoom_in_value, 1.04, 5.0, 0.04, 1.0, 0.0);
		zoomin_spinner = gtk_spin_button_new (adj, 0, 2);
		gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (zoomin_spinner), TRUE);
		gtk_spin_button_set_numeric( GTK_SPIN_BUTTON (zoomin_spinner), 1);
		gtk_box_pack_start (GTK_BOX (hbox), zoomin_spinner, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 5);
		
		//Zoom out
		hbox = gtk_hbox_new (FALSE, 0);
		label = gtk_label_new ("Zoom-out by:");
		gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
		gtk_label_set_width_chars (GTK_LABEL(label), char_width_label);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 5);

		adj = (GtkAdjustment *) gtk_adjustment_new (zoom_out_value, 0.1, 0.98, 0.02, 0.2, 0.0);
		zoomout_spinner = gtk_spin_button_new (adj, 0, 2);
		gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (zoomout_spinner), TRUE);
		gtk_spin_button_set_numeric( GTK_SPIN_BUTTON (zoomout_spinner), 1);
		gtk_box_pack_start (GTK_BOX (hbox), zoomout_spinner, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 5);
		
		// ********************** Translate preferences
		frame = gtk_frame_new ("Viewport Translate Preferences");
		gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 0);
		
		vbox = gtk_vbox_new (FALSE, 0);
		gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
		gtk_container_add (GTK_CONTAINER (frame), vbox);
		
		//translate up
		hbox = gtk_hbox_new (FALSE, 0);
		label = gtk_label_new ("Translate Up by:");
		gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
		gtk_label_set_width_chars (GTK_LABEL(label), char_width_label);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 5);
		
		adj = (GtkAdjustment *) gtk_adjustment_new (translate_u_value, 1.0, 100.0, 1.0, 5.0, 0.0);
		up_spinner = gtk_spin_button_new (adj, 0, 0);
		gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (up_spinner), TRUE);
		gtk_spin_button_set_numeric( GTK_SPIN_BUTTON (up_spinner), 1);
		gtk_box_pack_start (GTK_BOX (hbox), up_spinner, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 5);
		
		//translate down
		hbox = gtk_hbox_new (FALSE, 0);
		label = gtk_label_new ("Translate Down by:");
		gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
		gtk_label_set_width_chars (GTK_LABEL(label), char_width_label);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 5);
		
		adj = (GtkAdjustment *) gtk_adjustment_new (translate_d_value, 1.0, 100.0, 1.0, 5.0, 0.0);
		down_spinner = gtk_spin_button_new (adj, 0, 0);
		gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (down_spinner), TRUE);
		gtk_spin_button_set_numeric( GTK_SPIN_BUTTON (down_spinner), 1);
		gtk_box_pack_start (GTK_BOX (hbox), down_spinner, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 5);
		
		//translate left
		hbox = gtk_hbox_new (FALSE, 0);
		label = gtk_label_new ("Translate Down by:");
		gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
		gtk_label_set_width_chars (GTK_LABEL(label), char_width_label);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 5);
		
		adj = (GtkAdjustment *) gtk_adjustment_new (translate_l_value, 1.0, 100.0, 1.0, 5.0, 0.0);
		left_spinner = gtk_spin_button_new (adj, 0, 0);
		gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (left_spinner), TRUE);
		gtk_spin_button_set_numeric( GTK_SPIN_BUTTON (left_spinner), 1);
		gtk_box_pack_start (GTK_BOX (hbox), left_spinner, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 5);
		
		//translate right
		hbox = gtk_hbox_new (FALSE, 0);
		label = gtk_label_new ("Translate Down by:");
		gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
		gtk_label_set_width_chars (GTK_LABEL(label), char_width_label);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 5);
		
		adj = (GtkAdjustment *) gtk_adjustment_new (translate_r_value, 1.0, 100.0, 1.0, 5.0, 0.0);
		right_spinner = gtk_spin_button_new (adj, 0, 0);
		gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (right_spinner), TRUE);
		gtk_spin_button_set_numeric( GTK_SPIN_BUTTON (right_spinner), 1);
		gtk_box_pack_start (GTK_BOX (hbox), right_spinner, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 5);
				
		// lock all options butons
		translate_lock_button = gtk_check_button_new_with_label ("Change all Viewport Translate values together");
		gtk_box_pack_start (GTK_BOX (vbox), translate_lock_button, TRUE, TRUE, 0);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (translate_lock_button), FALSE);
		g_signal_connect(G_OBJECT(translate_lock_button),"toggled",
		G_CALLBACK(toggle_translate_lock_state_fcn), (gpointer)this);
		
		//  **********************  Shear preferences 
		frame = gtk_frame_new ("Shear Preferences");
		gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 0);
		
		vbox = gtk_vbox_new (FALSE, 0);
		gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
		gtk_container_add (GTK_CONTAINER (frame), vbox);

		// hshear
		hbox = gtk_hbox_new (FALSE, 0);
		label = gtk_label_new ("Shear Horizontally by:");
		gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
		gtk_label_set_width_chars (GTK_LABEL(label), char_width_label);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 5);
		
		adj = (GtkAdjustment *) gtk_adjustment_new (hshear_angle, 1.0, 75.0, 1.0, 5.0, 0.0);
		hshear_spinner = gtk_spin_button_new (adj, 0, 2);
		gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (hshear_spinner), TRUE);
		gtk_spin_button_set_numeric( GTK_SPIN_BUTTON (hshear_spinner), 1);
		gtk_box_pack_start (GTK_BOX (hbox), hshear_spinner, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 5);
		
		//vshear
		hbox = gtk_hbox_new (FALSE, 0);
		label = gtk_label_new ("Shear Vertically by:");
		gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
		gtk_label_set_width_chars (GTK_LABEL(label), char_width_label);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 5);

		adj = (GtkAdjustment *) gtk_adjustment_new (vshear_angle, 1.0, 75.0, 1.0, 5.0, 0.0);
		vshear_spinner = gtk_spin_button_new (adj, 0, 2);
		gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (vshear_spinner), TRUE);
		gtk_spin_button_set_numeric( GTK_SPIN_BUTTON (vshear_spinner), 1);
		gtk_box_pack_start (GTK_BOX (hbox), vshear_spinner, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 5);
		
		// lock all options butons
		shear_lock_button = gtk_check_button_new_with_label ("Change all Shear values together");
		gtk_box_pack_start (GTK_BOX (vbox), shear_lock_button, TRUE, TRUE, 0);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (shear_lock_button), FALSE);
		g_signal_connect(G_OBJECT(shear_lock_button),"toggled",
		G_CALLBACK(toggle_shear_lock_state_fcn), (gpointer)this);
		
		/* Ensure that the dialog box is destroyed when the user responds */
		g_signal_connect(dialog, "response", G_CALLBACK (update_values_and_destroy_widget), this);
		#ifdef UNDECORATED_DIALOG
			gtk_window_set_decorated (GTK_WINDOW(dialog), FALSE);
		#endif
		gtk_widget_show_all (dialog);
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::open_preferences_dialog() \n");

}
/************** End preferences related functions ***********/


void gtk_win::show_properties_dialog( string key )
{
	if ( access_key == key )
	{
		GtkWidget *dialog, *content_area;
		GtkWidget *main_vbox, *hbox;
		GtkWidget *frame;
		GtkWidget *table;
		dialog = gtk_dialog_new_with_buttons ("Properties",
		                                 GTK_WINDOW(mainwin),
		                                 GTK_DIALOG_MODAL, GTK_STOCK_CLOSE,
		                                 GTK_RESPONSE_NONE,
		                                 NULL);
		content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
		
		/*g_signal_connect(sidepane, "size-allocate", G_CALLBACK(my_getsize), NULL);
		g_signal_connect(canvas, "size-allocate", G_CALLBACK(my_getsize), NULL);*/
		
		GtkWidget *label1, *label2;
		char buffer[200];
		
		main_vbox = gtk_vbox_new (FALSE, 5);
		gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 10);
		gtk_container_add (GTK_CONTAINER (content_area), main_vbox);
		
		frame = gtk_frame_new ("Dimensions");
		gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 10);
		
		sprintf(buffer, "%s: %5d x %5d %s: %5d x %5d %s: %5d x %5d %s: %5d x %5d %s: %5d x %5d %s: %5d x %5d",
		"Canvas", canvas->allocation.width, canvas->allocation.height, 
		"Menubar", menubar->allocation.width, menubar->allocation.height,
		"Toolbar", toolbar->allocation.width, toolbar->allocation.height,
		"Sidepane",sidepane->allocation.width, sidepane->allocation.height,
		"Statusbar", statusbar->allocation.width, statusbar->allocation.height,
		"Top-Window", mainwin->allocation.width, mainwin->allocation.height);
		
		hbox = gtk_hbox_new (FALSE, 10);
		
		table = gtk_table_new(6, 4, 0);
		vector <string> arr;
		line2arr( buffer, &arr, " ");
		int index = 0;
		string markup;
		for ( int row = 0; row < 6; row++ )
		{
			for ( int col = 0; col < 4; col++ )
			{
				if ( col == 0 )
				{
					label1 = gtk_label_new( NULL );
					markup = "<b>"
			        	+ arr[index] + "</b>";
					gtk_label_set_markup ( GTK_LABEL ( label1 ), markup.c_str() );
				}
				else
					label1 = gtk_label_new( arr[index].c_str() );
				gtk_label_set_justify(GTK_LABEL (label1), GTK_JUSTIFY_RIGHT);
				gtk_misc_set_alignment (GTK_MISC (label1), 1, 0.5);
				gtk_table_attach( GTK_TABLE(table), label1, col, col+1, row, row+1, GTK_FILL, GTK_FILL, 3, 3 );
				index++;
			}
		}
		
		gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 10);
		gtk_container_add (GTK_CONTAINER (frame), hbox);
		
		frame = gtk_frame_new ("Preference Settings");
		gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 10);
		
		hbox = gtk_hbox_new (FALSE, 10);
		
		char buffer2[400];
		sprintf(buffer2, "%s: %.2fX,  %s: %.2fX, %s: %.2f\%, %s: %.2f\%, %s: %.2f\%, %s: %.2f\%, %s: %.2f, %s: %.2f",
		"Zoom-in Multiplier", zoom_in_value, 
		"Zoom-out Multiplier", zoom_out_value,
		"Translate-up by", translate_u_value,   
		"Translate-down by",translate_d_value,
		"Translate-left by", translate_l_value,
		"Translate-right by", translate_r_value,
		"Change Shear Horizontally by", vshear_angle,
		"Change Shear Vertically by", hshear_angle);
		
		table = gtk_table_new(8, 2, 0);
		arr.clear();
		line2arr( buffer2, &arr, ":,");
		index = 0;
		for ( int row = 0; row < 8; row++ )
		{
			for ( int col = 0; col < 2; col++ )
			{
				if ( col == 0 )
				{
					label1 = gtk_label_new( NULL );
					markup = "<b>"
			        	+ arr[index] + ":</b>";
					gtk_label_set_markup ( GTK_LABEL ( label1 ), markup.c_str() );
				}
				else
					label1 = gtk_label_new( arr[index].c_str() );
				gtk_label_set_justify(GTK_LABEL (label1), GTK_JUSTIFY_RIGHT);
				gtk_misc_set_alignment (GTK_MISC (label1), 1, 0.5);
				gtk_table_attach( GTK_TABLE(table), label1, col, col+1, row, row+1, GTK_FILL, GTK_FILL, 3, 3 );
				index++;
			}
		}
		
		gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 10);
		gtk_container_add (GTK_CONTAINER (frame), hbox);
		
		frame = gtk_frame_new ("Current Configuration");
		gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 10);
		
		hbox = gtk_hbox_new (FALSE, 10);
		
		char buffer3[600];
		sprintf(buffer3, "%s: %.2fX;  %s: (%.2f,%.2f); %s: (%.2f,%.2f); %s: %.2f; %s: %.2f; %s: %.2f; %s: %d; %s: %d",
		"Magnification", max( (saved_xright - saved_xleft)/(xright-xleft), (saved_ybottom - saved_ytop)/(ybottom-ytop) ),
		"Left-Top Coordinates", xleft, ytop,
		"Right-Bottom Coordinates", xright, ybottom,    
		"Rotation By", (angle_multiplier%4)*90.0,
		"Shear in Horizontal Direction", atan(horizontal_shear)/CONV_DEG_TO_RAD,
		"Shear in Vertical Direction", atan(vertical_shear)/CONV_DEG_TO_RAD,
		"No. of Image maps", image_maps.size(),
		"No. of sidepane buttons", sidepane_buttons.size());
		
		table = gtk_table_new(8, 2, 0);
		arr.clear();
		line2arr( buffer3, &arr, ":;");
		index = 0;
		for ( int row = 0; row < 8; row++ )
		{
			for ( int col = 0; col < 2; col++ )
			{
				if ( col == 0 )
				{
					label1 = gtk_label_new( NULL );
					markup = "<b>"
			        	+ arr[index] + ":</b>";
					gtk_label_set_markup ( GTK_LABEL ( label1 ), markup.c_str() );
				}
				else
					label1 = gtk_label_new( arr[index].c_str() );
				gtk_label_set_justify(GTK_LABEL (label1), GTK_JUSTIFY_RIGHT);
				gtk_misc_set_alignment (GTK_MISC (label1), 1, 0.5);
				gtk_table_attach( GTK_TABLE(table), label1, col, col+1, row, row+1, GTK_FILL, GTK_FILL, 3, 3 );
				index++;
			}
		}
		
		gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 10);
		gtk_container_add (GTK_CONTAINER (frame), hbox);
		
		/* Ensure that the dialog box is destroyed when the user responds */
		g_signal_connect(dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
		
		#ifdef UNDECORATED_DIALOG
			gtk_window_set_decorated (GTK_WINDOW(dialog), FALSE);
		#endif
		gtk_widget_show_all (dialog);
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute gtk_win::show_properties_dialog() \n");
}

/* end propoerties dialog */
void gtk_win::line2arr (char* str, vector<string>* arr, char *tokenizer)
{	
	string ts;
	char* tok;
	(*arr).clear();
	tok = strtok(str,tokenizer);
	while ( tok != NULL )
	{
		//printf("%s", tok);
		ts.assign(tok);
		(*arr).push_back(ts);
		tok = strtok(NULL,tokenizer);
	}
}

/* Accessibility key Function */
static void generate_accessibility_key( int length ) 
{
	static const char alphanum[] =
		"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	
	//length = 8;
	//char* s = new char [length];
	vector<char> s(length);
	for (int i = 0; i < length; i++) 
	{
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	//s[length] = 0;
	string key_temp(s.begin(), s.end());
	//string key_temp(s);
	//string key_temp("yusfguysd");
	::accessibility_key = key_temp;//.sub_str(0, length-1);
	//delete(s);
}

/** image-maps related functions from gtk_win ***/
void gtk_win::image_maps_store_transformed_coordinates()
{
	for ( unsigned int i = 0; i < image_maps.size(); i++ )
	{
		image_maps[i]->x1t = image_maps[i]->x1; image_maps[i]->y1t = image_maps[i]->y1;
		image_maps[i]->x2t = image_maps[i]->x2; image_maps[i]->y2t = image_maps[i]->y1;
		image_maps[i]->x3t = image_maps[i]->x2; image_maps[i]->y3t = image_maps[i]->y2;
		image_maps[i]->x4t = image_maps[i]->x1; image_maps[i]->y4t = image_maps[i]->y2;
		transform_user_to_window(cr, image_maps[i]->x1t, image_maps[i]->y1t);
		transform_user_to_window(cr, image_maps[i]->x2t, image_maps[i]->y2t);
		transform_user_to_window(cr, image_maps[i]->x3t, image_maps[i]->y3t);
		transform_user_to_window(cr, image_maps[i]->x4t, image_maps[i]->y4t);
		
		/*if ( image_maps[i]->to_show_info )
		{
			gtkw.transform_user_to_window(cr, image_maps[i]->event_x, image_maps[i]->event_y);
			image_maps[i]->try_set_event_coordinates( image_maps[i]->event_x, image_maps[i]->event_y );
		}*/
	}
}

void gtk_win::image_maps_highlight()
{
	//if ( highlight_image_maps )
	//{
		for ( unsigned int i = 0; i < image_maps.size(); i++ )
		{
			setcolor(cr, 0.1, 0.65, 0.77, 0.2);
			fillrect(cr, image_maps[i]->x1, image_maps[i]->y1, image_maps[i]->x2, image_maps[i]->y2 );
			setcolor(cr, 0.1, 0.65, 0.77, 1);
			setlinewidth(cr, 2);
			drawrect(cr, image_maps[i]->x1, image_maps[i]->y1, image_maps[i]->x2, image_maps[i]->y2 );
			setfontface(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			setfontsize(cr, 20);
			drawtext(cr, image_maps[i]->x1+20, image_maps[i]->y2+20, (char*)image_maps[i]->name.c_str(), 500);
		}
	//}
}

void gtk_win::image_maps_show_text_balloon()
{
	double rgb_border[3] = {192./255, 152./255, 83./255};//0.992157, 0.42353, 0};
	double rgba_fill[4] = {252./255, 248./255, 227./255, 0.9}; 
	double rgb_text[3] = {0./255, 0./255, 0./255};
	vector<string> info;
	for ( unsigned int i = 0; i < image_maps.size(); i++ )
	{
		if ( image_maps[i]->to_show_info )
		{
			//(*info).clear(); // doubtful about this line
			image_maps[i]->get_image_map_text_fcn(image_maps[i]->name, &info );
			drawtextballoon(cr, image_maps[i]->event_x, image_maps[i]->event_y, image_maps[i]->tolerance, &info, 
					     12, "Sans", rgb_border, rgba_fill, rgb_text );
		}
	}
}

/*********** START IMAGE-MAPS CLASS*************/
image_map::image_map( string _name, double _x1, double _y1, double _x2, double _y2, double tol,
                      get_image_map_text _get_image_map_text_fcn )
{
	name = _name;
	x1 = _x1; y1 = _y1;
	x2 = _x2; y2 = _y2;
	x1t = _x1; y1t = _y1;
	x2t = _x2; y2t = _y1;
	x3t = _x2; y3t = _y2;
	x4t = _x1; y4t = _y2;
	tolerance = fabs(tol);
	to_show_info = false;
	event_x = -20; event_y = -20;
	get_image_map_text_fcn = _get_image_map_text_fcn;
}

bool image_map::try_set_event_coordinates( double ex, double ey, gtk_win *_application  )
{
	if ( in_bouding_box(ex, ey, _application)
	     //ex >= min(x1t,x3t) + horizontal_shear*ey && ex <= max(x1t, x3t) + horizontal_shear*ey && 
	     //ey >= min(y1t,y3t) + vertical_shear*ex && ey <= max(y1t, y3t) + vertical_shear*ex
	   )
	{
		event_x = _application->win2user_x(ex);
		event_y = _application->win2user_y(ey);
		to_show_info = true;
	}
	else
	{
		event_x = -20;
		event_y = -20;
		to_show_info = false;
	}
	
	return to_show_info;
}

bool image_map::in_bouding_box( double ex, double ey, gtk_win *_application )
{
	if ( fabs(_application->get_horizontal_shear() ) < 0.04 &&
	     fabs(_application->get_vertical_shear() ) < 0.04 )
	{
		return ( ex >= min(x1t,x3t) && ex <= max(x1t, x3t) && 
		         ey >= min(y1t,y3t) && ey <= max(y1t, y3t) ) ;
	}

	double m1 = (y2t - y1t)/(x2t - x1t);
	double m2 = (y3t - y2t)/(x3t - x2t);
	double m3 = (y4t - y3t)/(x4t - x3t);
	double m4 = (y1t - y4t)/(x1t - x4t);
	double mt = (ey)/(ex);
	
	double x1i, x2i, x3i, x4i;
	double y1i, y2i, y3i, y4i;
	
	if ( fabs(m1) > _application->get_max_slope() || fabs(m3) > _application->get_max_slope() )
	{
		x1i = x1t;
		y1i = mt*x1i;
		x3i = x3t;
		y3i = mt*x3i;
	}
	else if ( fabs(m1) < _application->get_min_slope() || fabs(m3) < _application->get_min_slope() )
	{
		y1i = y1t;
		x1i = y1i/mt;
		y3i = y3t;
		x3i = y3i/mt;
	}
	else
	{
		double c1 = y1t - m1*x1t;
		x1i =  c1/(mt - m1);
		y1i = mt*x1i;
		double c3 = y3t - m3*x3t;
		x3i =  c3/(mt - m3);
		y3i = mt*x3i;
	}
	
	if ( fabs(m2) > _application->get_max_slope() || fabs(m4) > _application->get_max_slope() )
	{
		x2i = x2t;
		y2i = mt*x2i;
		x4i = x4t;
		y4i = mt*x4i;
	}
	else if ( fabs(m2) < _application->get_min_slope() || fabs(m4) < _application->get_min_slope() )
	{
		y2i = y2t;
		x2i = y2i/mt;
		y4i = y4t;
		x4i = y4i/mt;
	}
	else
	{
		double c2 = y2t - m2*x2t;
		x2i =  c2/(mt - m2);
		y2i = mt*x2i;
		double c4 = y4t - m4*x4t;
		x4i =  c4/(mt - m4);
		y4i = mt*x4i;
	}
	
	int count = 0;
	if ( (x1i < ex && y1i < ey ) && (x1i >= min(x1t, x2t) && x1i <= max(x1t, x2t) ) 
	                             && (y1i >= min(y1t, y2t) && y1i <= max(y1t, y2t) ) )
		count++;
	
	if ( (x2i < ex && y2i < ey ) && (x2i >= min(x2t, x3t) && x2i <= max(x2t, x3t) ) 
	                             && (y2i >= min(y2t, y3t) && y2i <= max(y2t, y3t) ) )
		count++;
	
	if ( (x3i < ex && y3i < ey ) && (x3i >= min(x3t, x4t) && x3i <= max(x3t, x4t) ) 
	                             && (y3i >= min(y3t, y4t) && y3i <= max(y3t, y4t) ) )
		count++;
		
	if ( (x4i < ex && y4i < ey ) && (x4i >= min(x4t, x1t) && x4i <= max(x4t, x1t) ) 
	                             && (y4i >= min(y4t, y1t) && y4i <= max(y4t, y1t) ) )
		count++;
	
	//if (name == "POLO")
	//	printf("\n-----------------------------------------------\n");
	//printf("NAME: %-15s; count: %d; map_clicked: %d\n", name.c_str(), count, ((count%2)==1) );
	return ((count%2)==1);
}

/*************** START STYLE BUTTON ***************/
static void enter_button(GtkWidget *widget, gpointer data) 
{
	style_button* s_button = (style_button*) data;
	s_button->act_on_enter_button_signal(accessibility_key);
}
static void leave_button(GtkWidget *widget, gpointer data) 
{ 
	style_button* s_button = (style_button*) data;
	s_button->act_on_leave_button_signal(accessibility_key);
}

style_button::style_button( string _button_text, button_fcn _act_on_button_press, gtk_win *_application )
{
	button_text = _button_text;
	act_on_button_press = _act_on_button_press;
	application = _application;
	desc = "NO IDEA!!!";
	
	#ifdef STYLE_BUTTONS
		button = gtk_button_new();
	#else 
		button = gtk_button_new_with_label( button_text.c_str() );
	#endif
	
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(act_on_button_press), (gpointer)this );
	gtk_button_set_relief( GTK_BUTTON(button), GTK_RELIEF_HALF );
	
				
	access_id = accessibility_key;
	#ifdef STYLE_BUTTONS
		button_label = gtk_label_new("");
		gtk_label_set_width_chars( GTK_LABEL (button_label), button_text.length()+2);

		string markup = "<span face=\"Arial\" style=\"normal\" color=\"#ffffff\" bgcolor=\"#528ae1\" size=\"x-large\">"
			        + button_text + "</span>";
		gtk_label_set_markup(GTK_LABEL (button_label), markup.c_str() );
		gtk_container_add (GTK_CONTAINER (button), button_label);
		GtkStyle *style = gtk_widget_get_style(button);

		GdkColor my_white;
		gdk_color_parse("#528ae1", &my_white);
		GdkColor my_orange; 
		gdk_color_parse("#c0550d", &my_orange);
	
		style->bg[GTK_STATE_PRELIGHT] = my_orange;
		style->bg[GTK_STATE_NORMAL]   = my_white;
		style->bg[GTK_STATE_ACTIVE]   = my_orange;
		style->bg[GTK_STATE_SELECTED] = my_white;
		style->bg[GTK_STATE_INSENSITIVE] = my_white;
		style->xthickness = 2;
	  	style->ythickness = 2;
		gtk_widget_set_style(button, style);
		g_signal_connect(G_OBJECT(button), "enter", G_CALLBACK(enter_button), (gpointer)this);
		g_signal_connect(G_OBJECT(button), "leave", G_CALLBACK(leave_button), (gpointer)this);
	#endif
}

GtkWidget* style_button::get_widget(string key)
{
	if ( key == accessibility_key)
	{
		return button;
	}
	else
		return NULL;
}

void style_button::set_desc( string _desc )
{
	desc = _desc;
	char status[150];
	sprintf(status, "Button name: %s, why is it there? %s", button_text.c_str(), desc.substr(0,120).c_str() );
	gtk_widget_set_tooltip_text (button, status);
}

void style_button::act_on_enter_button_signal(string key)
{
	if ( key == accessibility_key)
	{
		string markup = "<span face=\"Helvetica\" style=\"normal\" color=\"#ffffff\" bgcolor=\"#c0550d\" size=\"x-large\">"
			        + button_text + "</span>";
		gtk_label_set_markup ( GTK_LABEL ( button_label ), markup.c_str() );
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute style_button::act_on_enter_button_signal() \n");
}

//
void style_button::act_on_leave_button_signal(string key)
{
	if ( key == accessibility_key)
	{
		string markup = "<span face=\"Helvetica\" style=\"normal\" color=\"#ffffff\" bgcolor=\"#528ae1\" size=\"x-large\">"
	                + button_text + "</span>";
		gtk_label_set_markup ( GTK_LABEL (button_label), markup.c_str() );
	}
	else
		fprintf(stderr, "Error: Insufficient priviledges to execute style_button::act_on_leave_button_signal() \n");
}

/*************** END STYLE BUTTON ***************/

/**** start bgl_stats ******/
bgl_stats::bgl_stats( gtk_win *_application )
{
	application = _application;
	reset_counts();
	count_redraw = 0; count_saved_to_file = 0; 
	count_mousebuttonpress_event= 0; count_keypress_event = 0; count_mousemove_event = 0;
	count_scroll_events = 0; count_configure_event = 0; count_expose_event = 0;
	cpu_time_used = 0;
	
	num_cores = sysconf( _SC_NPROCESSORS_ONLN );
	/*bool stat_available = true;
	ifstream stat;
	vector<string> arr;
	stat.open("/proc/self/stat");
	if (!stat)
	{
		printf("%s\n", "Could not open file for reading options\n");
		stat_available = false;
	}
	if( stat_available )
	{
		char str[400];
		stat.getline(str, 400);
		line2arr(str, &arr, " ");
		init_process_use		
	}
	stat.close();*/
}
void bgl_stats::reset_counts()
{
	count_f_ellipses = 0; count_f_arcs = 0; count_f_rects = 0; count_f_poly = 0; count_f_roundrect = 0;
	count_d_ellipses = 0; count_d_arcs = 0; count_d_rects = 0; count_d_poly = 0; count_d_roundrect = 0;
	count_lines = 0; count_text = 0; count_textballoons = 0;
}
void bgl_stats::draw_start_fcn()
{
	draw_start = clock();
}
void bgl_stats::draw_end_fcn()
{
	draw_end = clock();
	cpu_time_used += ((double) (draw_end - draw_start)) / CLOCKS_PER_SEC;
}
void bgl_stats::get_shape_counts( char *buffer )
{
	sprintf(buffer, "%s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d",
	"Rectangles Outlined", count_d_rects, 
	"Rectangles Filled", count_f_rects,
	"Arcs Outlined", count_d_arcs,   
	"Arcs Filled", count_f_arcs,
	"Ellipses Outlined", count_d_ellipses,   
	"Ellipses Filled", count_f_ellipses,
	"Polygons Outlined", count_d_poly,   
	"Polygons Filled", count_f_poly,
	"Rounded-Rect Outlined", count_d_roundrect,   
	"Rounded-Rect Filled", count_f_roundrect,
	"Lines drawn", count_lines,   
	"Text-Segments drawn", count_text,
	"Text-balloons drawn", count_textballoons);
}

void bgl_stats::get_event_count( char *buffer )
{
	sprintf(buffer, "%s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d",
	"Redrawn Canvas", count_redraw, 
	"Saved to File", count_saved_to_file,
	"Mouse Button Press", count_mousebuttonpress_event,   
	"Key Press", count_keypress_event,
	"Mouse Move", count_mousemove_event,   
	"Mouse Scroll Event", count_scroll_events,
	"Main-window configure",count_configure_event,   
	"Main-window expose", count_expose_event);
}

void bgl_stats::get_resource_usage( char *buffer )
{
	current_time = time(NULL);
	double diff_time = (current_time - start_time);
#ifdef linux OR unix
	struct rusage usage;
	int who = RUSAGE_SELF;
	int result = getrusage(who, &usage);
	vector<string> arr;

	//process_status = "";
	bool status_available = true;
	ifstream status;
	status.open("/proc/self/status");
	if (!status)
	{
		printf("%s\n", "Could not open file for reading status\n");
		status_available = false;
	}
	if( status_available )
	{
		char str[255];
		while (!status.eof())
		{
			status.getline(str, 255);
			//printf("%s\n", str);
			arr.clear();
			application->line2arr(str, &arr, "\t: ");
			//printf("arr[0] = %s, arr[1] = %s\n", arr[0].c_str(), arr[1].c_str());
			if ( arr[0] == "VmPeak" )
				VmPeak = atoi((char*)arr[1].c_str());
			else if ( arr[0] == "VmSize" )
				VmSize = atoi((char*)arr[1].c_str());
			else if ( arr[0] == "VmLck" )
				VmLck = atoi((char*)arr[1].c_str());
			else if ( arr[0] == "VmHWM" )
				VmHWM = atoi((char*)arr[1].c_str());
			else if ( arr[0] == "VmRSS" )
				VmRSS = atoi((char*)arr[1].c_str());
			else if ( arr[0] == "VmData" )
				VmData = atoi((char*)arr[1].c_str());
			else if ( arr[0] == "VmStk" )
				VmStk = atoi((char*)arr[1].c_str());
			else if ( arr[0] == "VmExe" )
				VmExe = atoi((char*)arr[1].c_str());
			else if ( arr[0] == "VmLib" )
				VmLib = atoi((char*)arr[1].c_str());
			else if ( arr[0] == "VmPTE" )
				VmPTE = atoi((char*)arr[1].c_str());
			else if ( arr[0] == "Threads" )
				Threads = atoi((char*)arr[1].c_str());
		}
	}
	status.close();
	
	double total_user_cpu_time = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec*pow(10,-6),
	       total_syst_cpu_time = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec*pow(10,-6);
	double total_cpu_process_time = total_user_cpu_time + total_syst_cpu_time;
	
	
	if ( result != -1 )
	{		
		sprintf(buffer, "%s:%.4f sec, %s:%.4f sec, %s:%.4f sec, %s:%.2f\%, %s:%.2f Sec, %s:%.2f megabyte, %s:%.2f megabyte, %s:%.2f megabyte, %s:%.2f megabyte, %s:%.2f megabyte, %s:%.2f megabyte, %s:%.2f megabyte, %s:%.2f megabyte, %s:%.2f megabyte, %s:%.2f megabyte, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d",
		"Total user CPU-time used", total_user_cpu_time, 
		"Total system CPU-time used", total_syst_cpu_time,
		"Average Time to Redraw Canvas", cpu_time_used/(count_redraw),   
		"Percent CPU used by this process", 100*total_cpu_process_time/(num_cores*diff_time),   
		"This application has been running for", diff_time,
		"Peak virtual memory size", 0.000976562*VmPeak,
		"Current virtual memory size", 0.000976562*VmSize,
		"Current locked memory size", 0.000976562*VmLck,
		"Peak resident set size", 0.000976562*VmHWM,
		"Current resident set size", 0.000976562*VmRSS,
		"Size of Data", 0.000976562*VmData,
		"Size of stack", 0.000976562*VmStk,
		"Size of text segments", 0.000976562*VmExe,
		"Shared library code size", 0.000976562*VmLib,
		"Page table entries size (since Linux 2.6.10)", 0.000976562*VmPTE,
		"Number of Threads", Threads,
		"page reclaims (soft page faults) no I/O",usage.ru_minflt,   
		"page faults (hard page faults) required I/O", usage.ru_majflt,
		"Number of Swaps", usage.ru_nswap,
		"Number of times the file system had to perform input", usage.ru_inblock,
		"Number of times the file system had to perform output", usage.ru_oublock,
		"Number of voluntary context switches", usage.ru_nvcsw,
		"Number of involuntary context switches", usage.ru_nivcsw );
	}
	else
	 	printf("There was an error retreiving the stats!!!!!\n");
#else
	sprintf(buffer, "%s:%.4f sec,%s:%.2f sec",
		"Average Time to Redraw Canvas", cpu_time_used/(count_redraw),
		"This application has been running for", diff_time ); 
#endif
}
/****** end bgl_stats ******/
