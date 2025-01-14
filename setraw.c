/*
 *  module  : setraw.c
 *  version : 1.10
 *  date    : 01/14/25
 */
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR) || defined(__TINYC__)
#define WINDOWS
#endif

#if !defined(ATARI) && !defined(WINDOWS)
#define UNIX
#endif

/*
 * This preprocessor test may not be available on all systems.
 */
#if !defined(ATARI) && !defined(WINDOWS) && !defined(UNIX)
#error "either ATARI, WINDOWS, or UNIX should be defined"
#endif

#ifdef ATARI
#include <sgtty.h>
#endif

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>	/* pollute name space as much as possible */
#include <io.h>		/* also import deprecated POSIX names */
#endif

#ifdef UNIX
#include <termios.h>
#include <sys/ioctl.h>
#endif

#ifdef NPROTO
void do_push_int();
#else
void do_push_int(int num);
#endif

/*
 * Flag that tells whether the terminal is in raw mode or not; set in
 * initScreen and reset in SetNormal. This allows SetNormal te be called
 * even if SetRaw was never called.
 */
static unsigned char raw_mode;

#ifdef ATARI
struct sgttyb old_tty;
struct tchars old_tch;
#endif

#ifdef WINDOWS
static int size_x, size_y;
static DWORD input_mode, output_mode;
#endif

#ifdef UNIX
static struct termios orig_mode;
#endif

#ifdef EXPECT_ERROR
/*
 * Abort program execution when a fatal error occurs.
 */
static void die(s)
char *s;
{
    printf("\033[2J\033[H");	/* assume VT100 escape codes are supported */
    perror(s);
    abort();
}
#endif

#ifdef UNIX
/*
 * Retrieve or calculate the screen dimensions.
 */
static int sizeScreen(int *rows, int *cols)
{
#ifdef EXPECT_ERROR
    int num;
    char buf[20];
#endif
    struct winsize ws;

    ws.ws_row = ws.ws_col = 0;
    if (ioctl(1, TIOCGWINSZ, &ws) < 0 || !ws.ws_row) {
#ifdef EXPECT_ERROR
	printf("\033[999C\033[999B\033[6n\r");
	fflush(stdout);
	if ((num = fread(buf, 1, sizeof(buf), stdin)) <= 0)
	    return -1;
	if (buf[0] == '\033' && buf[1] == '[' && buf[num - 1] == 'R') {
	    sscanf(&buf[2], "%d;%d", rows, cols);
	    return 0;
	}
#endif
    }
    *rows = ws.ws_row;
    *cols = ws.ws_col;
    return 0;
}
#endif

/*
 * initScreen - make rows and cols available.
 */
static void initScreen()
{
    int rows, cols;

    raw_mode = 1;

#ifdef ATARI    
    rows = 25;
    cols = 80;
#endif

#ifdef WINDOWS
    rows = size_y;
    cols = size_x;
#endif

#ifdef UNIX    
    if (sizeScreen(&rows, &cols) < 0) {
#ifdef EXPECT_ERROR
	die("sizeScreen");
#endif
    }
#endif

    /*
     * External function that should be present.
     */
    do_push_int(rows);
    do_push_int(cols);
}

/*
 * Undo the machinations of SetRaw.
 */
void SetNormal()
{
    if (!raw_mode)
	return;
    raw_mode = 0;
#ifdef ATARI
    ioctl(0, TIOCSETP, &old_tty);
    ioctl(0, TIOCSETC, &old_tch);
#endif

#ifdef WINDOWS
    HANDLE input, output;

    input = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(input, input_mode);

    output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleMode(output, output_mode);
#endif

#ifdef UNIX
    if (tcsetattr(0, TCSAFLUSH, &orig_mode) < 0) {
#ifdef EXPECT_ERROR
	die("SetNormal");
#endif
    }
#endif
}

/*
 * Set the terminal in raw mode.
 *
#define ENABLE_PROCESSED_INPUT			0x1
#define ENABLE_LINE_INPUT			0x2
#define ENABLE_ECHO_INPUT			0x4
#define ENABLE_WINDOW_INPUT			0x8
#define ENABLE_MOUSE_INPUT			0x10
#define ENABLE_INSERT_MODE			0x20
#define ENABLE_QUICK_EDIT_MODE			0x40
#define ENABLE_EXTENDED_FLAGS			0x80
#define ENABLE_AUTO_POSITION			0x100
 */
#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#define ENABLE_VIRTUAL_TERMINAL_INPUT		0x200
#endif
/*
#define ENABLE_PROCESSED_OUTPUT			0x1
#define ENABLE_WRAP_AT_EOL_OUTPUT		0x2
#define DISABLE_NEWLINE_AUTO_RETURN		0x8
#define ENABLE_LVB_GRID_WORLDWIDE		0x10
 */
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING	0x4
#endif

/*
 * SetRaw sets the terminal in raw mode and also calls initScreen.
 */
void SetRaw()
{
#ifdef ATARI
    struct sgttyb new_tty;
    struct tchars new_tch;

    ioctl(0, TIOCGETP, &old_tty);
    ioctl(0, TIOCGETC, &old_tch);
    /*
     * Set tty to CBREAK mode.
     */
    new_tty = old_tty;
    new_tty.sg_flags |= CBREAK;
    new_tty.sg_flags &= ~ECHO;
    ioctl(0, TIOCSETP, &new_tty);
    /*
     * Unset signal characters.
     */
    memset(&new_tch, -1, sizeof(new_tch));
    ioctl(0, TIOCSETC, &new_tch);
#endif

#ifdef WINDOWS
    HWND hwnd;
    DWORD mode;
    HANDLE input, output;
    CONSOLE_SCREEN_BUFFER_INFO info;

#ifndef __TINYC__
    hwnd = GetConsoleWindow();
    SetWindowLongA(hwnd, GWL_STYLE,
		   GetWindowLongA(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
#endif
    input = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(input, &input_mode);
    mode = ENABLE_VIRTUAL_TERMINAL_INPUT;
    SetConsoleMode(input, mode);
    SetConsoleCP(65001);

    output = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(output, &output_mode);
    mode = ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT;
    SetConsoleMode(output, mode);
    SetConsoleOutputCP(65001);

    GetConsoleScreenBufferInfo(output, &info);
    size_x = info.srWindow.Right - info.srWindow.Left + 1;
    size_y = info.srWindow.Bottom - info.srWindow.Top + 1;
#endif

#ifdef UNIX    
    struct termios raw;

    if (tcgetattr(0, &orig_mode) < 0) {
#ifdef EXPECT_ERROR
	die("GetNormal");
#endif
    }
    raw = orig_mode;
    /*
     * control chars: set return condition: min number of bytes and timer.
     */
    raw.c_cc[VMIN] = 1;		/* return each byte. */
    raw.c_cc[VTIME] = 0;	/* no timeout. */
    /*
     * input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control.
     */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /*
     * local modes: echoing off, canonical off, no extended functions,
     * no signal chars (^Z,^C)
     */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
#if 0
    /* output modes: disable post processing */
    raw.c_oflag &= ~OPOST;
#endif
    /* control modes: set 8 bit chars */
    raw.c_cflag |= CS8;
    if (tcsetattr(0, TCSAFLUSH, &raw) < 0) {
#ifdef EXPECT_ERROR
	die("SetRaw");
#endif
    }
#endif
    initScreen();
}
