/**
 * Sends an XClientMessageEvent to all running st instances telling them to reload colors.
 *
 */


#include <stdio.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

void ScanTreeChildren(Display *display, Window *window, char *name);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s target1 target2 ...", argv[0]);
        return 1;
    }

    Display *display = XOpenDisplay(NULL);
    Window rootWindow = RootWindow(display, 0);

    for (int i = 1; i < argc; i++)
        ScanTreeChildren(display, &rootWindow, argv[i]);

    return 0;
}

void ScanTreeChildren(Display *display, Window *window, char *name) {
    Window root;
    Window parent;
    Window *children = NULL;
    int nchildren = 0;

    if (!XQueryTree(display, *window, &root, &parent, &children, &nchildren) || !nchildren) {
        return;
    }

    for (int i = 0; i < nchildren; i++) {
        XClassHint hint;
        if (!XGetClassHint(display, children[i], &hint)) {
            continue;
        }

        if (strcmp(name, hint.res_name) == 0) {
            XClientMessageEvent event;
            event.type = ClientMessage;
            event.message_type = XInternAtom(display, "ReloadColors", False);
            event.format = 8;
            event.data.b[0] = 0;
            //strcpy(event.data.b, message);

            XSendEvent(display, children[i], True, NoEventMask, (XEvent*)&event);
            //printf("res_name: %s; res_class: %s\n", hint.res_name, hint.res_class);
        }

        // printf("res_name: %s; res_class: %s\n", hint.res_name, hint.res_class);
        ScanTreeChildren(display, &children[i], name);
   }
}
