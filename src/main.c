#include <stdlib.h>
#include <xcb/xcb.h>

int main()
{
	xcb_rectangle_t rectangles[] = {{10, 50, 40, 20}};
	
    /* Open the connection to the X server */
	xcb_connection_t* connection = xcb_connect(NULL, NULL);

	/* Get the first screen */
	xcb_screen_t* screen =
		xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

	/* Create black (foreground) graphic context */
	xcb_drawable_t window = screen->root;
	xcb_gcontext_t foreground = xcb_generate_id(connection);
	uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	uint32_t values[2] = {screen->black_pixel, 0};

	xcb_create_gc(connection, foreground, window, mask, values);

	/* Create a window */
	window = xcb_generate_id(connection);

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE;

	xcb_create_window(connection,					 /* connection          */
					  XCB_COPY_FROM_PARENT,			 /* depth               */
					  window,						 /* window Id           */
					  screen->root,					 /* parent window       */
					  0, 0,							 /* x, y                */
					  150, 150,						 /* width, height       */
					  10,							 /* border_width        */
					  XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
					  screen->root_visual,			 /* visual              */
					  mask, values);				 /* masks */

	/* Map the window on the screen and flush*/
	xcb_map_window(connection, window);
	xcb_flush(connection);

	/* draw primitives */
	xcb_generic_event_t* event;
	while ((event = xcb_wait_for_event(connection))) {
		switch (event->response_type & ~0x80) {
		case XCB_EXPOSE:
			/* draw the rectangles */
			xcb_poly_rectangle(connection, window, foreground, 1, rectangles);

			/* flush the request */
			xcb_flush(connection);

			break;
		default:
			/* Unknown event type, ignore it */
			break;
		}

		free(event);
	}

	return 0;
}
