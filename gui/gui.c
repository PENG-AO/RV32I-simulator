#include "gui.h"
#include "splash_win.h"
#include "help_win.h"
#include "main_win.h"
#include "analysis_win.h"
#include "cache_win.h"

typedef struct command {
    s8 type;
    s32 argc;
    s64 argv[2];
} COMMAND;

s8 reg2idx(char* reg) {
    if (!strcmp(reg, "fp")) return 8;
    // try regs
    for (s8 idx = 0; idx < 32; ++idx) {
        if (!strcmp(reg, reg_name[idx]))
            return idx;
    }
    return 0;
}

COMMAND get_command(GUI* gui) {
    // echo + blinking cursor
    echo();
    curs_set(1);
    // prepare a COMMAND struct
    COMMAND com;
    com.argc = 0;
    // get instruction
    char input[73], output[5][12];
    WINDOW* com_win = newwin(1, 77, 22, 2);
    wclear(com_win);
    wtimeout(com_win, gui->stepping_interval);
    mvwgetstr(com_win, 0, 0, input);
    delwin(com_win);
    // split with space (exactly 1 space)
    s32 counter = 0, argc = 0;
    strcat(input, " "); // add an end point
    for (int idx = 0; idx < strlen(input); ++idx) {
        if (input[idx] == ' ') {
            output[argc][counter] = '\0';
            ++argc;
            counter = 0;
        } else {
            output[argc][counter] = input[idx];
            ++counter;
        }
    }
    // pack into COMMAND
    if (!strcmp(output[0], "quit")) {
        com.type = 'q';
    } else if (!strcmp(output[0], "step")) {
        com.type = 's';
        com.argc = 1;
        com.argv[0] = argc > 1 ? atoi(output[1]) : STAT_INFO_MAX;
    } else if (!strcmp(output[0], "auto")) {
        com.type = 'o';
        com.argc = 1;
        com.argv[0] = argc > 1 ? atoi(output[1]) : 1500;
    } else if (!strcmp(output[0], "stop")) {
        // quit auto-stepping
        com.type = 'o';
        com.argc = 1;
        com.argv[0] = -1;
    } else if (!strcmp(output[0], "reg")) {
        com.type = 'r';
        com.argc = 1;
        com.argv[0] = (argc > 1) ? reg2idx(output[1]) : -1;
    } else if (!strcmp(output[0], "mem")) {
        com.type = 'm';
        if (argc > 1) {
            com.argc = 1;
            sscanf(output[1], "0x%llX", com.argv);
            if (!com.argv[0]) // not recognizable
                com.argc = 0;
        } else {
            com.argc = 0;
        }
    } else if (!strcmp(output[0], "help")) {
        com.type = 'h';
    } else if (!strcmp(output[0], "analysis")) {
        com.type = 'a';
    } else if (!strcmp(output[0], "cache")) {
        com.type = 'c';
        com.argc = 1;
        com.argv[0] = argc > 1 ? atoi(output[1]) : 0;
    } else if (!strcmp(output[0], "")) {
        // parse KEY_ENTER as step 1
        com.type = 's';
        com.argc = 1;
        com.argv[0] = 1;
    }
    // noecho + hide cursor
    noecho();
    curs_set(0);
    return com;
}

STATE wait4command(GUI* gui, CORE* core) {
    COMMAND com = get_command(gui);
    switch (com.type) {
    case 'q':
        return STAT_QUIT;
    case 's':
        return STAT_STEP | ((u64)com.argv[0] << STAT_SHIFT_AMOUNT);
    case 'o':
        gui->stepping_interval = (com.argv[0] == 0) ? -1 : com.argv[0];
        return STAT_HALT;
    case 'r':
        gui->focused_win = REG_WIN;
        gui->reg_start = (com.argv[0] < 0) ? gui->reg_start : com.argv[0];
        show_main_win(gui, core);
        return STAT_HALT;
    case 'm':
        gui->focused_win = MEM_WIN;
        gui->mem_start = (com.argc > 0) ? (com.argv[0] >> 4) : 0;
        show_main_win(gui, core);
        return STAT_HALT;
    case 'a':
        show_analysis_win(core);
        return STAT_HALT;
    case 'c':
        show_cache_win(core, com.argv[0]);
        return STAT_HALT;
    case 'h':
        show_help_win();
        return STAT_HALT;
    default:
        return STAT_HALT;
    }
}

STATE gui_update(GUI* gui, CORE* core) {
    show_main_win(gui, core);
    // wait for a new command
    return wait4command(gui, core);
}

void gui_activate(GUI* gui) {
    initscr();
    noecho();
    curs_set(0);
    // set colors if supported
    if (has_colors()) start_color();
    init_pair(TITLE_COLOR, COLOR_BLUE, COLOR_BLACK);
    init_pair(SUBTITLE_COLOR, COLOR_CYAN, COLOR_BLACK);
    init_pair(WARNING_COLOR, COLOR_RED, COLOR_BLACK);
    init_pair(HIGHLIGHT_COLOR, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(STANDOUT_COLOR, COLOR_YELLOW, COLOR_BLACK);
    // switch to splash screen
    show_splash_win();
    if (getch() == 'h') show_help_win();
}

void gui_deinit(GUI* gui) {
    // at the end of program ncurses mode should be exited properly
    endwin();
}

void init_gui(GUI* gui) {
    // resize the terminal to 80 * 24
    printf("\e[8;24;80t");
    // init basic info
    gui->focused_win = COM_WIN;
    gui->reg_start = 0;
    memset(gui->reg_focus, 0, 32);
    gui->stepping_interval = -1;
    // assign interfaces
    gui->activate = gui_activate;
    gui->update = gui_update;
    gui->deinit = gui_deinit;
}
