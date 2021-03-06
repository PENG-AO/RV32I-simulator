#pragma once
#include "src/global.h"
#include "src/core.h"
#include "gui/gui.h"

typedef struct sim {
    // attributes
    CORE* core;
    GUI* gui;
    // insterfaces
    void (*load)(struct sim*, char*);
    void (*run)(struct sim* const);
} SIM;

void init_sim(SIM* sim);
