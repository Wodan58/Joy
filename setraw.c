/*
    module  : setraw.c
    version : 1.3
    date    : 08/29/24
*/
#include "globals.h"

/* #define EXPECT_ERROR */

#ifdef WINDOWS
static int size_x, size_y;
static DWORD input_mode, output_mode;
#else
static struct termios orig_mode;

#ifdef EXPECT_ERROR
static void die(const char *s)
{
    printf("\033[2J\033[H");	/* assume VT100 escape codes are supported */
    perror(s);
    abort();
}
#endif

static int sizeScreen(int *rows, int *cols)
{
#ifdef EXPECT_ERROR
    int num;
    char buf[MAXNUM];
#endif
    struct winsize ws;

    ws.ws_row = ws.ws_col = 0;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) < 0 || !ws.ws_row) {
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
 * initScreen - make rows and cols available on the stack.
 */
static void initScreen(pEnv env)
{
    int rows, cols;

#ifdef WINDOWS
    rows = size_y;
    cols = size_x;
#else
    if (sizeScreen(&rows, &cols) < 0) {
#ifdef EXPECT_ERROR
	die("sizeScreen");
#endif
    }
#endif
    NULLARY(INTEGER_NEWNODE, rows);
    NULLARY(INTEGER_NEWNODE, cols);
}

/*
 * Undo the machinations of SetRaw.
 */
static void SetNormal(void)
{
#ifdef WINDOWS
    HANDLE input, output;

    input = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(input, input_mode);

    output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleMode(output, output_mode);
#else
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_mode) < 0) {
#ifdef EXPECT_ERROR
	die("sizeScreen");
#endif
    }
#endif
}

/*
 * Set the terminal in raw mode.
 *
#define ENABLE_PROCESSED_INPUT 0x1
#define ENABLE_LINE_INPUT 0x2
#define ENABLE_ECHO_INPUT 0x4
#define ENABLE_WINDOW_INPUT 0x8
#define ENABLE_MOUSE_INPUT 0x10
#define ENABLE_INSERT_MODE 0x20
#define ENABLE_QUICK_EDIT_MODE 0x40
#define ENABLE_EXTENDED_FLAGS 0x80
#define ENABLE_AUTO_POSITION 0x100
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x200
 *
#define ENABLE_PROCESSED_OUTPUT 0x1
#define ENABLE_WRAP_AT_EOL_OUTPUT 0x2
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x4
#define DISABLE_NEWLINE_AUTO_RETURN 0x8
#define ENABLE_LVB_GRID_WORLDWIDE 0x10
 */
void SetRaw(pEnv env)
{
#ifdef WINDOWS
    DWORD mode;
    HANDLE input, output;
    CONSOLE_SCREEN_BUFFER_INFO info;

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
#else
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, &orig_mode) < 0) {
#ifdef EXPECT_ERROR
	die("sizeScreen");
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
#if  0
    /* output modes: disable post processing */
    raw.c_oflag &= ~OPOST;
#endif
    /* control modes: set 8 bit chars */
    raw.c_cflag |= CS8;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
#ifdef EXPECT_ERROR
	die("sizeScreen");
#endif
    }
#endif
    initScreen(env);
    atexit(SetNormal);
}
