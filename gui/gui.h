#pragma once
#include <ncurses.h>
#include "../src/global.h"
#include "../src/core.h"
#include "../src/instr.h"

#define TITLE_COLOR     1
#define SUBTITLE_COLOR  2
#define WARNING_COLOR   3
#define HIGHLIGHT_COLOR 4
#define STANDOUT_COLOR  5

#define REG_WIN 0
#define MEM_WIN 1
#define COM_WIN 2

typedef struct gui {
    // display control
    u8 focused_win;
    s32 reg_start;
    u8 reg_focus[64];
    u32 mem_start;
    s32 stepping_interval;
    // interfaces
    void (*activate)(struct gui*);
    STATE (*update)(struct gui*, CORE*);
    void (*deinit)(struct gui*);
} GUI;

void init_gui(GUI* win);
