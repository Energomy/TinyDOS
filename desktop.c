// desktop.c - A simple integrated GUI shell for TinyDOS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <microwin/nano-X.h>

#define TASKBAR_HEIGHT 30

// Helper to launch an application
void launch_app(const char* app_name) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp(app_name, app_name, NULL);
        // If we get here, execlp failed
        GrError("Failed to launch %s", app_name);
        exit(1);
    }
}

int main(void) {
    GR_WINDOW_ID taskbar, term_button, clock_button;
    GR_GC_ID gc;
    GR_EVENT event;
    GR_SCREEN_INFO sinfo;

    // --- Step 1: Launch the Nano-X server ---
    pid_t server_pid = fork();
    if (server_pid == 0) {
        execlp("nanox", "nanox", NULL);
        perror("Failed to start nanox server");
        exit(1);
    }
    sleep(1); // Give server time to initialize

    // --- Step 2: Connect to server and get screen info ---
    if (GrOpen() < 0) {
        fprintf(stderr, "Desktop: Failed to connect to Nano-X server\n");
        return 1;
    }
    GrGetScreenInfo(&sinfo);

    // --- Step 3: Create the GUI elements ---
    // Create the taskbar at the bottom
    taskbar = GrNewWindow(GR_ROOT_WINDOW_ID, "", 0, sinfo.rows - TASKBAR_HEIGHT, sinfo.cols, TASKBAR_HEIGHT, GR_RGB(192, 192, 192));
    GrMapWindow(taskbar);

    // Create a graphics context for drawing
    gc = GrNewGC();
    GrSetGCForeground(gc, GR_RGB(0, 0, 0)); // Black text

    // Create a "Terminal" button on the taskbar
    term_button = GrNewWindow(taskbar, "Terminal", 5, 5, 80, 20, GR_RGB(220, 220, 220));
    GrMapWindow(term_button);
    GrDrawString(term_button, gc, 10, 15, "Terminal", -1, GR_TFASCII);

    // Create a "Clock" button on the taskbar
    clock_button = GrNewWindow(taskbar, "Clock", 90, 5, 80, 20, GR_RGB(220, 220, 220));
    GrMapWindow(clock_button);
    GrDrawString(clock_button, gc, 25, 15, "Clock", -1, GR_TFASCII);

    // Tell the server which events we care about for our buttons
    GrSelectEvents(term_button, GR_EVENT_MASK_BUTTON_DOWN);
    GrSelectEvents(clock_button, GR_EVENT_MASK_BUTTON_DOWN);
    GrSelectEvents(GR_ROOT_WINDOW_ID, GR_EVENT_MASK_CLOSE_REQ); // To catch server shutdown

    // --- Step 4: The Main Event Loop ---
    printf("Desktop shell running...\n");
    while (1) {
        GrGetNextEvent(&event);

        if (event.type == GR_EVENT_TYPE_BUTTON_DOWN) {
            if (event.wid == term_button) {
                printf("Terminal button clicked!\n");
                launch_app("term");
            } else if (event.wid == clock_button) {
                printf("Clock button clicked!\n");
                launch_app("clock");
            }
        } else if (event.type == GR_EVENT_TYPE_CLOSE_REQ) {
            // The server is shutting down, so we should too
            break;
        }
    }

    GrClose();
    kill(server_pid, SIGTERM); // Clean up the server process
    waitpid(server_pid, NULL, 0);
    return 0;
}