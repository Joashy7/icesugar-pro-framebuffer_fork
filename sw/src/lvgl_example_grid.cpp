#include <lvgl_example_grid.h>
#include "lv_conf.h"
#include <lvgl.h>

namespace ucr { namespace bcoe { namespace cs { namespace cs122 {

#if LV_USE_FLOAT
    #define my_PRIprecise "f"
#else
    #define my_PRIprecise LV_PRId32
#endif

LVGL_Example_Grid::LVGL_Example_Grid(SPIDisplay *spi_disp,
                                     lv_display_flush_cb_t fcallback,
                                     lv_tick_get_cb_t tcallback,
                                     const cell board[ROWS][COLUMNS])                               
    : CS122_App(spi_disp, fcallback, tcallback),board_(board) {}

    // static void LVGL_Example_Grid::DisplayWithShips(const cell board[ROWS][COLUMNS], lv_obj_t * container) {
    //     for(int row = 0; row < 10; row++) {
    //         for(int col = 0; col < 10; col++) {
    //             lv_obj_t * label = lv_label_create(container);

    //             lv_obj_set_style_grid_cell_x_align(label, LV_GRID_ALIGN_STRETCH, 0);
    //             lv_obj_set_style_grid_cell_column_pos(label, col, 0);

    //             lv_obj_set_style_grid_cell_y_align(label, LV_GRID_ALIGN_STRETCH, 0);
    //             lv_obj_set_style_grid_cell_row_pos(label, row, 0);

    //             if(board[row][col].value == PLACED) {
    //                 lv_label_set_text_fmt(label, "P");
    //                 lv_obj_set_style_bg_color(label, lv_color_hex(0x0000FF), 0);
    //             }
    //             else if(board[row][col].value == HIT) {
    //                 lv_label_set_text_fmt(label, "H");
    //                 lv_obj_set_style_bg_color(label, lv_color_hex(0xFF0000), 0);
    //             }
    //             else if(board[row][col].value == MISS) {
    //                 lv_label_set_text_fmt(label, "M");
    //                 lv_obj_set_style_bg_color(label, lv_color_hex(0x00FF00), 0);
    //             }
    //             else if(board[row][col].value == EMPTY) {
    //                 lv_label_set_text_fmt(label, "E");
    //                 lv_obj_set_style_bg_color(label, lv_color_hex(0x888888), 0);
    //             }
    //             else {
    //                 lv_label_set_text_fmt(label, "");
    //             }
                
    //             lv_obj_set_style_border_color(label, lv_color_hex(0x000000), 0);
    //             lv_obj_set_style_border_width(label, 1, 0);

    //             lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);
    //         }
    //     }
    // }

    void LVGL_Example_Grid::createGrid() {
        lv_obj_t * screen = lv_screen_active();
        lv_obj_set_style_pad_all(screen, 0, 0);
        lv_obj_set_style_border_width(screen, 0, 0);
        lv_obj_set_style_margin_all(screen, 0, 0);
        lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);

        /* Main grid container with fixed descriptors */
        lv_obj_t * container = lv_obj_create(screen);
        static const int32_t container_style_grid_column_dsc_array_0[] = {27, 27, 27, 27, 27, 27, 27, 27, 27, 27, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_style_grid_column_dsc_array(container, container_style_grid_column_dsc_array_0, 0);
        static const int32_t container_style_grid_row_dsc_array_1[] = {27, 27, 27, 27, 27, 27, 27, 27, 27, 27, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_style_grid_row_dsc_array(container, container_style_grid_row_dsc_array_1, 0);
        lv_obj_set_pos(container, 0, 0);
        lv_obj_set_style_pad_all(container, 0, 0);
        lv_obj_set_style_pad_row(container, 0, 0);
        lv_obj_set_style_pad_column(container, 0, 0);
        lv_obj_set_style_border_width(container, 0, 0);
        lv_obj_set_style_margin_all(container, 0, 0);
        lv_obj_set_style_radius(container, 0, 0);
        lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_style_layout(container, LV_LAYOUT_GRID, 0);
        lv_obj_set_size(container, 270, 270);

        for(int row = 0; row < ROWS; row++) {
            for(int col = 0; col < COLUMNS; col++) {
                lv_obj_t * cell_obj = lv_obj_create(container);
                cells_[row][col] = cell_obj;

                lv_obj_set_style_grid_cell_x_align(cell_obj, LV_GRID_ALIGN_STRETCH, 0);
                lv_obj_set_style_grid_cell_column_pos(cell_obj, col, 0);

                lv_obj_set_style_grid_cell_y_align(cell_obj, LV_GRID_ALIGN_STRETCH, 0);
                lv_obj_set_style_grid_cell_row_pos(cell_obj, row, 0);

                lv_obj_set_style_bg_color(cell_obj, lv_color_hex(0xFFFFFF), 0);
                lv_obj_set_style_border_color(cell_obj, lv_color_hex(0x000000), 0);
                lv_obj_set_style_border_width(cell_obj, 1, 0);
                lv_obj_set_style_bg_opa(cell_obj, LV_OPA_COVER, 0);
            }
        }
    }

    void LVGL_Example_Grid::renderBoard(const cell board[ROWS][COLUMNS], bool hideShips) {
        for(int row = 0; row < ROWS; row++) {
            for(int col = 0; col < COLUMNS; col++) {
                lv_obj_t * cell_obj = cells_[row][col];
                
                if (hideShips) {
                    lv_obj_set_style_bg_color(cell_obj, lv_color_hex(0x0000FF), 0);
                } else {
                    lv_obj_set_style_bg_color(cell_obj, lv_color_hex(0xFF0000), 0);
                }
                if (board[row][col].value == HIT) {
                    lv_obj_set_style_bg_color(cell_obj, lv_color_hex(0xFF0000), 0);
                }
                else if (board[row][col].value == MISS) {
                    lv_obj_set_style_bg_color(cell_obj, lv_color_hex(0x00FF00), 0);
                }
                else if (board[row][col].value == EMPTY) {
                    lv_obj_set_style_bg_color(cell_obj, lv_color_hex(0x0000FF), 0);
                }
                lv_obj_invalidate(cell_obj);

                if (board[row][col].cursor) {
                    lv_obj_set_style_border_color(cell_obj, lv_color_hex(0xFFFF00), 0);
                    lv_obj_set_style_border_width(cell_obj, 3, 0);
                }
                else {
                    lv_obj_set_style_border_color(cell_obj, lv_color_hex(0x000000), 0);
                    lv_obj_set_style_border_width(cell_obj, 1, 0);
                }
            }
        }
    }   

    void LVGL_Example_Grid::DisplayWithShips(const cell board[ROWS][COLUMNS]) {
        board_ = board;
        hideShips_ = false;
        redraw_pending_ = true;
    }

    void LVGL_Example_Grid::DisplayWithoutShips(const cell board[ROWS][COLUMNS]) {
        board_ = board;
        hideShips_ = true;
        redraw_pending_ = true;
    }

    void LVGL_Example_Grid::redrawTimerCallback(lv_timer_t *timer) {
        LVGL_Example_Grid *self =
            static_cast<LVGL_Example_Grid *>(lv_timer_get_user_data(timer));

        self->applyPendingRender();
    }

    void LVGL_Example_Grid::applyPendingRender() {
        if(!redraw_pending_) {
            return;
        }

        redraw_pending_ = false;
        renderBoard(board_, hideShips_);
    }

    uint32_t LVGL_Example_Grid::run() {
        createGrid();
        renderBoard(board_, false);

        lv_timer_create(redrawTimerCallback, 30, this);

        return loop();
    }

}}}}