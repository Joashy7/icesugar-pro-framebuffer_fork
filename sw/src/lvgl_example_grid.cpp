#include <lvgl_example_grid.h>
#include "lv_conf.h"
#include <lvgl.h>

namespace ucr { namespace bcoe { namespace cs { namespace cs122 {

#if LV_USE_FLOAT
    #define my_PRIprecise "f"
#else
    #define my_PRIprecise LV_PRId32
#endif

void lv_example_grid_descriptors(void)
{
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

    for(int row = 0; row < 10; row++) {
        for(int col = 0; col < 10; col++) {
            lv_obj_t * label = lv_label_create(container);

            lv_obj_set_style_grid_cell_x_align(label, LV_GRID_ALIGN_STRETCH, 0);
            lv_obj_set_style_grid_cell_column_pos(label, col, 0);

            lv_obj_set_style_grid_cell_y_align(label, LV_GRID_ALIGN_STRETCH, 0);
            lv_obj_set_style_grid_cell_row_pos(label, row, 0);

            lv_obj_set_style_bg_color(label, lv_color_hex(0xFFFFFF), 0);
            lv_obj_set_style_border_color(label, lv_color_hex(0x000000), 0);
            lv_obj_set_style_border_width(label, 1, 0);

            lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);
        }
    }
}

uint32_t LVGL_Example_Grid::run() {
    lv_example_grid_descriptors();
    return loop();
}

}}}}