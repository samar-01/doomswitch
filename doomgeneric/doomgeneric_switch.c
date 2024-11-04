#include "doomkeys.h"

#include "doomgeneric.h"
#include "i_system.h"

#include <ctype.h>

// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <unistd.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

#ifdef DISPLAY_IMAGE
#include "image_bin.h"//Your own raw RGB888 1280x720 image at "data/image.bin" is required.
#endif

// See also libnx display/framebuffer.h.

// Define the desired framebuffer resolution (here we set it to 720p).
#define FB_WIDTH  1280
#define FB_HEIGHT 720
#include <sys/time.h>

// #include <X11/Xlib.h>
// #include <X11/Xutil.h>
// #include <X11/Xatom.h>
// #include <X11/XKBlib.h>

// static Display *s_Display = NULL;
// static Window s_Window = NULL;
// static int s_Screen = 0;
// static GC s_Gc = 0;
// static XImage *s_Image = NULL;

// #define KEYQUEUE_SIZE 16
#define KEYQUEUE_SIZE 50

u64 KEYS[] = { HidNpadButton_A,HidNpadButton_B,HidNpadButton_X,HidNpadButton_Y,HidNpadButton_StickL,HidNpadButton_StickR,HidNpadButton_L,HidNpadButton_R,HidNpadButton_ZL,HidNpadButton_ZR,HidNpadButton_Plus,HidNpadButton_Minus,HidNpadButton_Left,HidNpadButton_Up,HidNpadButton_Right,HidNpadButton_Down,HidNpadButton_StickLLeft,HidNpadButton_StickLUp,HidNpadButton_StickLRight,HidNpadButton_StickLDown,HidNpadButton_StickRLeft,HidNpadButton_StickRUp,HidNpadButton_StickRRight,HidNpadButton_StickRDown,HidNpadButton_LeftSL,HidNpadButton_LeftSR,HidNpadButton_RightSL,HidNpadButton_RightSR,HidNpadButton_Palma,HidNpadButton_Verification,HidNpadButton_HandheldLeftB,HidNpadButton_LagonCLeft,HidNpadButton_LagonCUp,HidNpadButton_LagonCRight,HidNpadButton_LagonCDown };
// int KEYS = [1,2,3];

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

static unsigned char convertToDoomKey(int key) {
	// if (kDown & HidNpadButton_A){
	// 	return KEY_ENTER;
	// }
	// if (kDown & HidNpadButton_Minus){
	// 	return KEY_ESCAPE;
	// }
	// if (kDown & HidNpadButton_AnyLeft){
	// 	return KEY_LEFTARROW;
	// }
	// if (kDown & HidNpadButton_AnyRight){
	// 	return KEY_RIGHTARROW;
	// }
	// if (kDown & HidNpadButton_AnyUp){
	// 	return KEY_UPARROW;
	// }
	// if (kDown & HidNpadButton_AnyDown){
	// 	return KEY_DOWNARROW;
	// }
	// if (kDown & HidNpadButton_B){
	// 	return KEY_FIRE;
	// }
	// if (kDown & HidNpadButton_X){
	// 	return KEY_USE;
	// }
	// if (kDown & HidNpadButton_ZR){
	// 	return KEY_RSHIFT;
	// }
	switch (key) {
		case HidNpadButton_A:
			return KEY_ENTER;
			break;
		case HidNpadButton_Minus:
			return KEY_ESCAPE;
			break;
		case HidNpadButton_AnyLeft:
			return KEY_LEFTARROW;
			break;
		case HidNpadButton_AnyRight:
			return KEY_RIGHTARROW;
			break;
		case HidNpadButton_AnyUp:
			return KEY_UPARROW;
			break;
		case HidNpadButton_AnyDown:
			return KEY_DOWNARROW;
			break;
			// case XK_Control_L:
		case HidNpadButton_B:
			return KEY_FIRE;
			break;
		case HidNpadButton_X:
			return KEY_USE;
			break;
			// case XK_Shift_L:
		case HidNpadButton_ZR:
			return KEY_RSHIFT;
			break;
		default:
			return 0;
			break;
	}
}


static void addKeyToQueue(int pressed, unsigned char key) {
	// unsigned char key = convertToDoomKey(keyCode);

	unsigned short keyData = (pressed << 8) | key;

	s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
	s_KeyQueueWriteIndex++;
	s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

void DG_Init() {
	memset(s_KeyQueue, 0, KEYQUEUE_SIZE * sizeof(unsigned short));

	// // window creation

	// s_Display = XOpenDisplay(NULL);

	// s_Screen = DefaultScreen(s_Display);

	// int blackColor = BlackPixel(s_Display, s_Screen);
	// int whiteColor = WhitePixel(s_Display, s_Screen);

	// XSetWindowAttributes attr;
	// memset(&attr, 0, sizeof(XSetWindowAttributes));
	// attr.event_mask = ExposureMask | KeyPressMask;
	// attr.background_pixel = BlackPixel(s_Display, s_Screen);

	// int depth = DefaultDepth(s_Display, s_Screen);

	// s_Window = XCreateSimpleWindow(s_Display, DefaultRootWindow(s_Display), 0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY, 0, blackColor, blackColor);

	// XSelectInput(s_Display, s_Window, StructureNotifyMask | KeyPressMask | KeyReleaseMask);

	// XMapWindow(s_Display, s_Window);

	// s_Gc = XCreateGC(s_Display, s_Window, 0, NULL);

	// XSetForeground(s_Display, s_Gc, whiteColor);

	// XkbSetDetectableAutoRepeat(s_Display, 1, 0);

	// // Wait for the MapNotify event

	// while(1)
	// {
	//     XEvent e;
	//     XNextEvent(s_Display, &e);
	//     if (e.type == MapNotify)
	//     {
	//         break;
	//     }
	// }

	// s_Image = XCreateImage(s_Display, DefaultVisual(s_Display, s_Screen), depth, ZPixmap, 0, (char *)DG_ScreenBuffer, DOOMGENERIC_RESX, DOOMGENERIC_RESX, 32, 0);
}


void DG_DrawFrame() {
	// DG_ScreenBuffer;
	// if (s_Display)
	// {
	//     while (XPending(s_Display) > 0)
	//     {
	//         XEvent e;
	//         XNextEvent(s_Display, &e);
	//         if (e.type == KeyPress)
	//         {
	//             KeySym sym = XKeycodeToKeysym(s_Display, e.xkey.keycode, 0);
	//             //printf("KeyPress:%d sym:%d\n", e.xkey.keycode, sym);

				// addKeyToQueue(1, sym);
	//         }
	//         else if (e.type == KeyRelease)
	//         {
	//             KeySym sym = XKeycodeToKeysym(s_Display, e.xkey.keycode, 0);
	//             //printf("KeyRelease:%d sym:%d\n", e.xkey.keycode, sym);
	//             addKeyToQueue(0, sym);
	//         }
	//     }

		// XPutImage(s_Display, s_Window, s_Gc, s_Image, 0, 0, 0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY);

	//     //XFlush(s_Display);
	// }



	// printf("frame\n");
}

void DG_SleepMs(uint32_t ms) {
	// usleep (ms * 1000);
	svcSleepThread(ms * 1000000);
}

uint32_t DG_GetTicksMs() {
	struct timeval  tp;
	struct timezone tzp;

	gettimeofday(&tp, &tzp);

	return (tp.tv_sec * 1000) + (tp.tv_usec / 1000); /* return milliseconds */
}

int DG_GetKey(int* pressed, unsigned char* doomKey) {
	if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex) {
		//key queue is empty

		return 0;
	}
	// else
	{
		unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
		s_KeyQueueReadIndex++;
		s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

		*pressed = keyData >> 8;
		*doomKey = keyData & 0xFF;

		return 1;
	}
}

void DG_SetWindowTitle(const char* title) {
	// if (s_Window)
	// {
	//     XChangeProperty(s_Display, s_Window, XA_WM_NAME, XA_STRING, 8, PropModeReplace, title, strlen(title));
	// }
}

void addQueues(u64 k, int status) {
	if (k & HidNpadButton_A) {
		addKeyToQueue(status, KEY_ENTER);
	}
	if (k & HidNpadButton_Minus) {
		addKeyToQueue(status, KEY_ESCAPE);
	}
	if (k & HidNpadButton_AnyLeft) {
		addKeyToQueue(status, KEY_LEFTARROW);
	}
	if (k & HidNpadButton_AnyRight) {
		addKeyToQueue(status, KEY_RIGHTARROW);
	}
	if (k & HidNpadButton_AnyUp) {
		addKeyToQueue(status, KEY_UPARROW);
	}
	if (k & HidNpadButton_AnyDown) {
		addKeyToQueue(status, KEY_DOWNARROW);
	}
	if (k & HidNpadButton_B) {
		addKeyToQueue(status, KEY_FIRE);
	}
	if (k & HidNpadButton_X) {
		addKeyToQueue(status, KEY_USE);
	}
	if (k & HidNpadButton_ZR) {
		addKeyToQueue(status, KEY_RSHIFT);
	}
}

int main(int argc, char** argv) {
	// Retrieve the default window
	NWindow* win = nwindowGetDefault();

	// Create a linear double-buffered framebuffer
	Framebuffer fb;
	framebufferCreate(&fb, win, FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
	framebufferMakeLinear(&fb);

	#ifdef DISPLAY_IMAGE
	u8* imageptr = (u8*)image_bin;
	const u32 image_width = 1280;
	const u32 image_height = 720;
	#endif
	u32 cnt = 0;
	// Configure our supported input layout: a single player with standard controller styles
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);

	// Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
	PadState pad;
	padInitializeDefault(&pad);

	socketInitializeDefault();
	printf("Hello World!\n");
	// Display arguments sent from nxlink
	printf("%d arguments\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("argv[%d] = %s\n", i, argv[i]);
	}
	// the host ip where nxlink was launched
	printf("nxlink host is %s\n", inet_ntoa(__nxlink_host));
	// redirect stdout & stderr over network to nxlink
	nxlinkStdio();
	// this text should display on nxlink host
	printf("printf output now goes to nxlink server\n");

	// #ifndef GRAPHISC1
	// consoleInit(NULL);
	// // Other initialization goes here. As a demonstration, we print hello world.
	// // printf("Hello World!\n");
	// I_PrintBanner("Startup");
	// #endif

	doomgeneric_Create(argc, argv);
	// for (int i = 0; ; i++)
	// {
	// }
	while (appletMainLoop()) {
		// Scan the gamepad. This should be done once for each frame
		padUpdate(&pad);

		// padGetButtonsDown returns the set of buttons that have been
		// newly pressed in this frame compared to the previous one
		u64 kDown = padGetButtonsDown(&pad);
		u64 kUp = padGetButtonsUp(&pad);
		// memset(KEYS, 0, sizeof(KEYS));
		// int n = 0;
		if (kDown & HidNpadButton_Plus) {
			printf("Exiting\n");
			break; // break in order to return to hbmenu
		} else {
			addQueues(kDown, 1);
			addQueues(kUp, 0);
			// for (int i = 0; i <= sizeof(KEYS) / sizeof(KEYS[0]); i++) {

			// unsigned char key = convertToDoomKey(KEYS[i]);
			// if (key != 0) {
				// if (kDown & i) {
				// 	printf("%d\n",i);
					// addKeyToQueue(1, key);
				// }
			// 	else if (kUp & i) {
			// 		addKeyToQueue(0, key);
			// 	}
			// }
		// }
		}


		// Retrieve the framebuffer
		u32 stride;
		u32* framebuf = (u32*)framebufferBegin(&fb, &stride);
		if (cnt != 60)
			cnt++;
		else
			cnt = 0;

		// Each pixel is 4-bytes due to RGBA8888.
		for (u32 y = 0; y < FB_HEIGHT; y++) {
			for (u32 x = 0; x < FB_WIDTH; x++) {
				// for (u32 y = 0; y < DOOMGENERIC_RESY; y++) {
				// 	for (u32 x = 0; x < DOOMGENERIC_RESX; x++) {
				u32 pos = y * stride / sizeof(u32) + x;
				// printf("%d,%d\n",y,x);
				u32 a = x * DOOMGENERIC_RESX / FB_WIDTH;
				u32 b = y * DOOMGENERIC_RESY / FB_HEIGHT;
				// u32 a = x;
				// u32 b = y;
				u32 pos1 = (a + b * DOOMGENERIC_RESX);
				// u32 pos1 = b * (stride/2) / sizeof(u32) + a;

				// framebuf[pos] = a * 0x100 + b;//Set framebuf to different shades of grey.
				// printf("%d\n",DG_ScreenBuffer[0]);
				// printf("%d\n",DG_ScreenBuffer[0]);
				u32 color = DG_ScreenBuffer[pos1];
				// u32 color = DG_ScreenBuffer[0];
				// 7602433 is red
				// printf("%d\n",color);
				unsigned int red = (color >> 16) & 0xFF;   // Extract the red component
				unsigned int green = (color >> 8) & 0xFF;  // Extract the green component
				unsigned int blue = color & 0xFF;
				framebuf[pos] = (blue << 16) | (green << 8) | red;

			}
		}
		// doomgeneric_Tick();
		// printf("hello\n");
		// if (kDown & HidNpadButton_A) {
		// 	printf("A Pressed\n");
		// }
		// if (kDown & HidNpadButton_B) {
		// 	printf("B Pressed\n");
		// }
		// We're done rendering, so we end the frame here.

		framebufferEnd(&fb);
		doomgeneric_Tick();
	}
	socketExit();
	framebufferClose(&fb);

	return 0;
}
