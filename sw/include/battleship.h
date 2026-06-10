#include "hardware/adc.h"
#include <lvgl.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <pico/time.h>
#include <hardware/spi.h>
#include <pico/cyw43_arch.h>
#include "cells.h"



enum directions{REST, UP, DOWN, LEFT, RIGHT} direction;
enum modeStates {MODE_START, WAIT, PUSH, POSITION, PLAY} modeState;
enum placeStates {PLACE_START, PLACE_IDLE, PLACE_MOVE, ROTATE, PLACE, READY} placeState;
enum playStates {PLAY_START, PLAY_IDLE, PLAY_MOVE, ATTACK, WON} playState;

cell playerBoard[2][ROWS][COLUMNS] = {};

const int OFFSETS[] = {4, 3, 2, 2, 1};
int x = 0, y = 0, xOffset = 0, yOffset = OFFSETS[0], playerIndex[2] = {0, 0};
unsigned int VRx, VRy;
bool player = 0, rotate, place;
bool displayDirty = true;
bool debugInput = true;

void RequestDisplayUpdate() {
    displayDirty = true;
}

bool ConsumeDisplayUpdate() {
    bool shouldUpdate = displayDirty;
    displayDirty = false;
    return shouldUpdate;
}

bool GetDirections() {
    adc_select_input(0); 
    VRx = adc_read();

    adc_select_input(1); 
    VRy = adc_read();

    directions nextDirection = REST;

    if (VRy > 2500 && VRx > 250 && VRx < 2500) nextDirection = UP;
    else if (VRy < 250 && VRx > 250 && VRx < 2500) nextDirection = DOWN;
    else if (VRx < 250 && VRy > 250 && VRy < 2500) nextDirection = LEFT;
    else if (VRx > 2500 && VRy > 250 && VRy < 2500) nextDirection = RIGHT;

    bool changed = nextDirection != direction;
    direction = nextDirection;

    if (changed) {
        RequestDisplayUpdate();
        if (debugInput) {
            printf("Direction changed: %d\n", direction);
        }
    }

    return changed;
}

void MoveCursor(bool boardIndex) {

    for (int i = 0; i <= xOffset; i++) {
        for (int j = 0; j <= yOffset; j++) playerBoard[boardIndex][y + j][x + i].cursor = false;
    }

    if (direction == UP && y > 0) y--;
    else if (direction == DOWN && y < ROWS - 1 - yOffset) y++;
    else if (direction == LEFT && x > 0) x--;
    else if (direction == RIGHT && x < COLUMNS - 1 - xOffset) x++;

    for (int i = 0; i <= xOffset; i++) {
        for (int j = 0; j <= yOffset; j++) playerBoard[boardIndex][y + j][x + i].cursor = true;
    }

    RequestDisplayUpdate();
}

bool GetPress() {
    bool nextPlace = gpio_get(21);
    bool nextRotate = gpio_get(22);
    bool changed = nextPlace != place || nextRotate != rotate;

    place = nextPlace;
    rotate = nextRotate;

    if (changed) {
        RequestDisplayUpdate();
        if (debugInput) {
            printf("Buttons changed: place=%d rotate=%d\n", place, rotate);
        }
    }

    return changed;
}

void ResetCursor() {
    playerBoard[player][y][x].cursor = false;
    x = 0; y = 0;
    RequestDisplayUpdate();
}

void PrintBoard(int boardIndex) {
    printf("\nBoard %d\n", boardIndex);

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            printf("(%d,%d) ",
                playerBoard[boardIndex][row][col].value,
                playerBoard[boardIndex][row][col].cursor);
        }
        printf("\n");
    }
    printf("\n");
}

void SetCell() {
    for (int i = 0; i <= xOffset; i++) {
        for (int j = 0; j <= yOffset; j++) {
            if (playerBoard[player][y + j][x + i].value == PLACED) return;
        }
    }

    for (int i = 0; i <= xOffset; i++) {
        for (int j = 0; j <= yOffset; j++) {
            playerBoard[player][y + j][x + i].value = PLACED;
        }
    }

    playerIndex[player]++;
    ResetCursor();

    if (playerIndex[player] >= sizeof(OFFSETS)/sizeof(OFFSETS[0])) {
        xOffset = 0;
        yOffset = 0;
    }

    RequestDisplayUpdate();
}

void Attack() {
    if (playerBoard[!player][y][x].value == HIT || playerBoard[!player][y][x].value == MISS) return;

    if (playerBoard[!player][y][x].value == EMPTY) {
        playerBoard[!player][y][x].value = MISS;
        gpio_put(18, 0);
    }
    else if (playerBoard[!player][y][x].value == PLACED) {
        playerBoard[!player][y][x].value = HIT;
        gpio_put(18, 1);
    }
    x = 0; y = 0;
    RequestDisplayUpdate();
}

bool CheckForWinner(int boardIndex) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if (playerBoard[boardIndex][i][j].value == PLACED) return false;
        }
    }

    return true;
}

void RotateShip() {
    for (int i = 0; i <= xOffset; i++) {
        for (int j = 0; j <= yOffset; j++) playerBoard[player][y + j][x + i].cursor = false;
    }

    if (x + yOffset > COLUMNS - 1) x = x - ((x + yOffset) - (COLUMNS - 1));
    if (y + xOffset > ROWS - 1) y = y - ((y + xOffset) - (ROWS - 1));
    
    int temp = xOffset; xOffset = yOffset; yOffset = temp;

    for (int i = 0; i <= xOffset; i++) {
        for (int j = 0; j <= yOffset; j++) playerBoard[player][y + j][x + i].cursor = true;
    }

    RequestDisplayUpdate();
}
