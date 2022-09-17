# ESP32-Life

Colored Game of Life for the LILYGO® TTGO T-Display

## Rules of the Game

1. As in Life, a live cell dies of underpopulation if it is surrounded by fewer than two live cells in its Moore neighbourhood;
2. A cell dies of overpopulation if it is surrounded by more than three live cells;
3. A cell is born if and only if there are exactly three live neighbours in its Moore neighbourhood.

### QuadLife

A colored variant, implementing 4 colors:

1. When newborn cells are created, they take on the majority color of their neighbours.
2. If the three neighbours are all different colors, the newborn cell takes on the remaining color.

### For Platfromio

Use the PIO.ini in a new project and the main.ino build and flash.. Quick and easy
