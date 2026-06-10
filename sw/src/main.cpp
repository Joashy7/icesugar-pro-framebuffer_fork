#include "spi_display.h"
#include "lv_conf.h"
#include "lvgl_example_grid.h"
#include "battleship.h"
#include <pico/stdio_usb.h>

ucr::bcoe::SPIDisplay spi_display(480, 272, 10000000, 20);
static ucr::bcoe::cs::cs122::LVGL_Example_Grid *app = nullptr;

/*Return the elapsed milliseconds since startup.
 *It needs to be implemented by the user*/
uint32_t cs122_get_millis(void) {
    return to_ms_since_boot(get_absolute_time());
}

static uint8_t buffer[OLEDRGB_WIDTH * OLEDRGB_HEIGHT / 10];

/*Copy the rendered image to the screen. */
void cs122_flush_cb_direct(lv_display_t * disp, const lv_area_t * area, uint8_t * px_buf) {
    ucr::bcoe::SPIDisplay *spi_display = reinterpret_cast<ucr::bcoe::SPIDisplay *>(lv_display_get_user_data(disp));
	uint32_t i = 0;
	for (uint32_t y = area->y1; y <= area->y2; y++) {
		for(uint32_t x = area->x1; x <= area->x2; x++) {
			uint32_t px_buf_idx = x * 2 + y * (spi_display->getWidth() * 2);
		    buffer[i++] =  (px_buf[px_buf_idx+1] & 0xE0) | ((px_buf[px_buf_idx+1] & 0x7) << 2) | (px_buf[px_buf_idx] & 0x1f) >> 3;
		}
	}

    /*Show the rendered image on the display*/
    spi_display->drawBitmap(area->x1, area->y1, area->x2, area->y2, buffer);

    /*Indicate that the buffer is available.
     *If DMA were used, call in the DMA complete interrupt*/
    lv_display_flush_ready(disp);
}

/*It needs to be implemented by the user*/
void cs122_flush_cb_partial(lv_display_t * disp, const lv_area_t * area, uint8_t * px_buf) {
	uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);

    /*Show the rendered image on the display*/
    ucr::bcoe::SPIDisplay *spi_display = reinterpret_cast<ucr::bcoe::SPIDisplay *>(lv_display_get_user_data(disp));
    spi_display->drawBitmap(2 * area->x1, area->y1, 2 * area->x2+1, area->y2, px_buf);

    /*Indicate that the buffer is available.
     *If DMA were used, call in the DMA complete interrupt*/
    lv_display_flush_ready(disp);
}

void DisplayWithShips(bool p) {
    app->DisplayWithShips(playerBoard[p]);
}

void DisplayWithoutShips(bool p) {
    app->DisplayWithoutShips(playerBoard[p]);
}

void DisplayCurrentState() {
    switch (modeState) {
        case WAIT:
            if (place) DisplayWithoutShips(player);
            else DisplayWithShips(player);
            break;
        case POSITION:
            DisplayWithShips(player);
            break;
        case PLAY:
            DisplayWithoutShips(!player);
            break;
        default:
            DisplayWithShips(player);
            break;
    }
}

void Tick() {

    switch (modeState) {
        case MODE_START:
            modeState = WAIT;
            if (!player) gpio_put(18, 1);
            else gpio_put(16, 1);
            RequestDisplayUpdate();
            break;
        case WAIT:
            GetPress();
            if (place) {
                modeState = PUSH;
            }
            break;
        case PUSH:
            GetPress();
            if (!place) {
                modeState = POSITION;
            }
            break;
        case POSITION:
            switch (placeState) {
                case PLACE_START:
                    LoadCurrentShipSize(player);
                    MoveCursor(player);
                    placeState = PLACE_IDLE;
                    break;
                case PLACE_IDLE:
                    GetDirections();
                    GetPress();
                    if(direction && !rotate && !place) {
                        placeState = PLACE_MOVE;
                        MoveCursor(player);
                    } else if (!direction && rotate && !place) {
                        placeState = ROTATE;
                        RotateShip();
                    } else if (!direction && !rotate && place) {
                        placeState = PLACE;
                        SetCell();
                        if (ShipsRemaining(player)) MoveCursor(player);
                    } else if (!ShipsRemaining(player)) {                       
                        placeState = READY;
                    }
                    break;
                case PLACE_MOVE:
                    GetDirections();
                    if (!direction) placeState = PLACE_IDLE;
                    break;
                case ROTATE:
                    GetPress();
                    if (!rotate) placeState = PLACE_IDLE;
                    break;
                case PLACE:
                    GetPress();
                    if (!place) placeState = PLACE_IDLE;
                    break;
                case READY:
                    if (!player) {
                        player = 1;
                        modeState = MODE_START;
                        placeState = PLACE_START;
                        gpio_put(18, 0);
                        RequestDisplayUpdate();
                    } else {
                        player = 0;
                        x = 0;
                        y = 0;


                        modeState = PLAY;

                        MoveCursor(!player);
                        RequestDisplayUpdate();
                    }
                    break;
                default:
                    placeState = PLACE_START;
                    break;
            }
            //DisplayWithShips(player);
            break;
        case PLAY:
            gpio_put(16, player);    

            switch(playState) {
                case PLAY_START:
                    xOffset = 0;
                    yOffset = 0;
                    MoveCursor(!player);
                    playState = PLAY_IDLE;
                    break;
                case PLAY_IDLE:
                    GetDirections();
                    GetPress();
                    if(direction && !place) {
                        playState = PLAY_MOVE;
                        MoveCursor(!player);
                    } else if (!direction && place) {
                        int target = !player;
                        Attack();

                        if (CheckForWinner(target)) {
                            playState = WON;
                        } else {
                            playState = ATTACK;
                            player = !player;
                        }
                    }
                    break;
                case PLAY_MOVE:
                    GetDirections();
                    if (!direction) playState = PLAY_IDLE;
                    break;
                case ATTACK:
                    GetPress();
                    if (!place) {
                        MoveCursor(!player);
                        playState = PLAY_IDLE;
                    }
                    break;
                case WON:
                    gpio_put(18, 0);
                    //gpio_put(16, 0);
                    break;
                default:
                    playState = PLAY_START;
                    break;
            }
            break;
        default:
            modeState = MODE_START;
            break;
    }

    if (ConsumeDisplayUpdate()) {
        printf("Display update: mode=%d placeState=%d playState=%d player=%d place=%d direction=%d\n",
               modeState, placeState, playState, player, place, direction);
        DisplayCurrentState();
    }
}

void TickTimerCallback(lv_timer_t *timer) {
    Tick();
}

int main(void) {
    // Init drivers
	stdio_init_all();
    adc_init();
	cyw43_arch_init();

    adc_gpio_init(26);
    adc_gpio_init(27);

    gpio_init(22); gpio_set_dir(22, false); gpio_pull_down(22);
    gpio_init(21); gpio_set_dir(21, false); gpio_pull_down(21);

	spi_display.begin();
	spi_display.clear();

    modeState = MODE_START;
    placeState = PLACE_START;
    playState = PLAY_START;

    static ucr::bcoe::cs::cs122::LVGL_Example_Grid grid_app(
        &spi_display,
        cs122_flush_cb_partial,
        cs122_get_millis,
        playerBoard[0]
    );

    app = &grid_app;

    lv_timer_create(TickTimerCallback, 20, NULL);
    return grid_app.run();

}
