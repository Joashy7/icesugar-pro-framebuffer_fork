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
    : CS122_App(spi_disp, fcallback, tcallback), board_(board), statusLabel_(nullptr) {}

    void LVGL_Example_Grid::createGrid() {
        lv_obj_t * screen = lv_screen_active();
        lv_obj_set_style_pad_all(screen, 0, 0);
        lv_obj_set_style_border_width(screen, 0, 0);
        lv_obj_set_style_margin_all(screen, 0, 0);
        lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);

        statusLabel_ = lv_label_create(screen);
        lv_obj_set_pos(statusLabel_, 286, 10);
        lv_obj_set_width(statusLabel_, 180);
        lv_obj_set_style_text_color(statusLabel_, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_font(statusLabel_, &lv_font_montserrat_18, 0);

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

                lv_obj_set_style_bg_color(cell_obj, lv_color_hex(0x0B4F8A), 0);
                lv_obj_set_style_border_color(cell_obj, lv_color_hex(0x000000), 0);
                lv_obj_set_style_border_width(cell_obj, 1, 0);
                lv_obj_set_style_bg_opa(cell_obj, LV_OPA_COVER, 0);
            }
        }
    }

    void LVGL_Example_Grid::renderBoard(const cell board[ROWS][COLUMNS], ViewMode viewMode) {
        for(int row = 0; row < ROWS; row++) {
            for(int col = 0; col < COLUMNS; col++) {
                lv_obj_t * cell_obj = cells_[row][col];

                uint32_t color = 0x0B4F8A;
                cellValue value = board[row][col].value;

                if (value == PLACED && viewMode == ViewMode::Placement) {
                    color = 0x6F7D8C;
                }
                else if (value == HIT) {
                    color = 0xD62828;
                }
                else if (value == MISS) {
                    color = 0xE9EEF2;
                }

                lv_obj_set_style_bg_color(cell_obj, lv_color_hex(color), 0);

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

    void LVGL_Example_Grid::renderStatus() {
        if(statusLabel_ == nullptr) {
            return;
        }

        lv_label_set_text_fmt(statusLabel_, "P%d\n%s",
                              player_ ? 2 : 1,
                              viewMode_ == ViewMode::Placement ? "PLACE" : "ATTACK");
    }

    void LVGL_Example_Grid::DisplayPlacements(const cell board[ROWS][COLUMNS], bool player) {
        board_ = board;
        viewMode_ = ViewMode::Placement;
        player_ = player;
        redraw_pending_ = true;
    }

    void LVGL_Example_Grid::DisplayAttacks(const cell board[ROWS][COLUMNS], bool player) {
        board_ = board;
        viewMode_ = ViewMode::Attack;
        player_ = player;
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
        renderStatus();
        renderBoard(board_, viewMode_);
    }

    uint32_t LVGL_Example_Grid::run() {
        createGrid();
        renderStatus();
        renderBoard(board_, viewMode_);

        lv_timer_create(redrawTimerCallback, 10, this);

        return loop();
    }

}}}}
