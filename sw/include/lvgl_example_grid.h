#ifndef _LVGL_EXAMPLE_GRID_H_
#define _LVGL_EXAMPLE_GRID_H_
        
#include <cs122_app.h>
#include "cells.h"

namespace ucr { namespace bcoe { namespace cs { namespace cs122 {
    class LVGL_Example_Grid : CS122_App {
    public:
        LVGL_Example_Grid(SPIDisplay *spi_disp,
                        lv_display_flush_cb_t fcallback,
                        lv_tick_get_cb_t tcallback,
                        const cell board[ROWS][COLUMNS]);

        uint32_t run() override;

        void DisplayPlacements(const cell board[ROWS][COLUMNS], bool player);
        void DisplayAttacks(const cell board[ROWS][COLUMNS], bool player);

    private:
        enum class ViewMode { Placement, Attack };

        const cell (*board_)[COLUMNS];
        lv_obj_t *cells_[ROWS][COLUMNS];
        lv_obj_t *statusLabel_;

        ViewMode viewMode_ = ViewMode::Placement;
        bool player_ = false;
        volatile bool redraw_pending_ = false;

        static void redrawTimerCallback(lv_timer_t *timer);
        void applyPendingRender();

        void createGrid();
        void renderBoard(const cell board[ROWS][COLUMNS], ViewMode viewMode);
        void renderStatus();
    };
}}}}
#endif
