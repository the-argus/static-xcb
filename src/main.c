#include <chipmunk/chipmunk.h>
#include <xcb/xcb.h>

int main()
{
	xcb_connection_t* c = NULL;
	xcb_screen_t* screen = NULL;
	xcb_window_t win = 0;

	/* Open the connection to the X server */
	c = xcb_connect(NULL, NULL);

	/* Get the first screen */
	screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

	/* Ask for our window's Id */
	win = xcb_generate_id(c);

	xcb_gcontext_t foreground = xcb_generate_id(c);
	uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	uint32_t values[2] = {screen->black_pixel, 0};

	xcb_create_gc(c, foreground, screen->root, mask, values);

	xcb_rectangle_t rects[] = {
		(xcb_rectangle_t){.x = 0, .y = 0, .width = 10, .height = 10},
	};

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE;

	/* Create the window */
	xcb_create_window(c,							 /* Connection          */
					  XCB_COPY_FROM_PARENT,			 /* depth (same as root)*/
					  win,							 /* window Id           */
					  screen->root,					 /* parent window       */
					  0, 0,							 /* x, y                */
					  150, 150,						 /* width, height       */
					  10,							 /* border_width        */
					  XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
					  screen->root_visual,			 /* visual              */
					  mask, values);				 /* masks, not used yet */

	/* Map the window on the screen */
	xcb_map_window(c, win);

	/* Make sure commands are sent before we pause, so window is shown */
	xcb_flush(c);

	/* draw primitives */
	xcb_generic_event_t* event = NULL;
	while ((event = xcb_wait_for_event(c))) {
		switch (event->response_type & ~0x80) {
		case XCB_EXPOSE:
			xcb_poly_rectangle(c, screen->root, foreground, 1, rects);

			/* flush the request */
			xcb_flush(c);

			break;
		default:
			/* Unknown event type, ignore it */
			break;
		}

		free(event);
	}

	return 0;
}
