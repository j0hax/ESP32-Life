// Hardware-specific library
#include <SPI.h>
#include <TFT_eSPI.h>

// Invoke custom library
TFT_eSPI tft = TFT_eSPI();

// Size of our cells
#define CELLXY 2

// Resolution of our display
#define SCREENX 240
#define SCREENY 135

// Compute the needed size of our grid
#define GRIDX SCREENX / CELLXY
#define GRIDY SCREENY / CELLXY

// Target: 60 FPS
#define GEN_DELAY (1000 / 60)

// Maximum number of generations before resetting
#define NUMGEN 2048

// Set up our types of cell
typedef enum { DEAD, RED, GREEN, BLUE, YELLOW } cell;

// Keep track of the generations
uint16_t genCount = 0;

// Current grid
cell grid[GRIDX][GRIDY];

// The new grid for the next generation
cell newgrid[GRIDX][GRIDY];

void setup() {
  // Set up the display
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.println(F("***   Quad-Life  ***\n"));
  delay(500);
  tft.println(F("By Johannes Arnold\n"));
  delay(500);
  tft.println(F("    Enjoy..."));
  delay(1000);
}

void loop() {
  tft.fillScreen(TFT_BLACK);

  initGrid();
  drawGrid();
  delay(GEN_DELAY);

  for (uint16_t gen = 0; gen < NUMGEN; gen++) {
    computeCA();
    drawGrid();
    delay(GEN_DELAY);
    // copy the grid
    for (uint16_t x = 0; x < GRIDX; x++) {
      for (uint16_t y = 0; y < GRIDY; y++) {
        grid[x][y] = newgrid[x][y];
      }
    }
  }
}

// Draws only changes in grid on display
void drawGrid(void) {
  uint16_t color = TFT_BLACK;
  for (uint16_t x = 0; x < GRIDX; x++) {
    for (uint16_t y = 0; y < GRIDY; y++) {
      if (grid[x][y] != newgrid[x][y]) {
        switch (newgrid[x][y]) {
        case DEAD:
          color = TFT_BLACK;
          break;
        case RED:
          color = TFT_RED;
          break;
        case GREEN:
          color = TFT_GREEN;
          break;
        case BLUE:
          color = TFT_BLUE;
          break;
        case YELLOW:
          color = TFT_YELLOW;
          break;
        }
        tft.fillRect(CELLXY * x, CELLXY * y, CELLXY, CELLXY, color);
      }
    }
  }
}

// Initialise Grid
void initGrid(void) {
  for (uint16_t x = 0; x < GRIDX; x++) {
    for (uint16_t y = 0; y < GRIDY; y++) {
      // empty new grid
      newgrid[x][y] = DEAD;
      // 1-in-3 chance to turn a spot on
      if (random(3) == 1)
        grid[x][y] = cell(random(1, 5));
      else
        grid[x][y] = DEAD;
    }
  }
}

// Compute the CA. Basically everything related to CA starts here
void computeCA() {
  for (uint16_t x = 0; x < GRIDX; x++) {
    for (uint16_t y = 0; y < GRIDY; y++) {
      int neighbors = getNumberOfNeighbors(x, y);
      if (grid[x][y] != DEAD && (neighbors == 2 || neighbors == 3)) {
        // Rule 2: Any live cell with two or three live neighbours lives on to
        // the next generation.
        newgrid[x][y] = computeAvg(x, y);
      } else if (grid[x][y] != DEAD) {
        // Rule 1: Any live cell with fewer than two live neighbours dies, as if
        // by underpopulation. Rule 3: Any live cell with more than three live
        // neighbours dies, as if by overpopulation.
        newgrid[x][y] = DEAD;
      }
      if (grid[x][y] == DEAD && (neighbors == 3)) {
        // Rule 4: Any dead cell with exactly three live neighbours becomes a
        // live cell, as if by reproduction.
        newgrid[x][y] = computeAvg(x, y);
      } else if (grid[x][y] == DEAD)
        newgrid[x][y] = DEAD;
    }
  }
}

// Check the Moore neighborhood
int getNumberOfNeighbors(int x, int y) {
  return existsAt(x - 1, y) + existsAt(x - 1, y - 1) + existsAt(x, y - 1) +
         existsAt(x + 1, y - 1) + existsAt(x + 1, y) + existsAt(x + 1, y + 1) +
         existsAt(x, y + 1) + existsAt(x - 1, y + 1);
}

int existsAt(int x, int y) {
  /* Typically, we would wrap the grid using modulos, however in this case,
   * cells wrap automatically. */
  // int r = (x + GRIDX) % GRIDX;
  // int c = (y + GRIDY) % GRIDY;

  return (grid[x][y] != DEAD);
}

// Compute the color of a newly born cell
cell computeAvg(int x, int y) {
  /* Count, and if we know one has the majority, return immediately. Otherwise
   * keep the result and move on */
  int reds = countCells(x, y, RED);
  if (reds > 1)
    return RED;
  int greens = countCells(x, y, GREEN);
  if (greens > 1)
    return GREEN;
  int blues = countCells(x, y, BLUE);
  if (blues > 1)
    return BLUE;
  int yellows = countCells(x, y, YELLOW);
  if (yellows > 1)
    return YELLOW;

  // If all 3 parents are different, spawn the fourth color
  if (greens && blues && yellows)
    return RED;
  if (blues && yellows && reds)
    return GREEN;
  if (greens && reds && yellows)
    return BLUE;
  if (reds && greens && blues)
    return YELLOW;

  // Otherwise, we need to return something.
  return DEAD;
}

// Helper function to count the cells of a given type
uint8_t countCells(int x, int y, cell type) {
  return (grid[x - 1][y] == type) + (grid[x - 1][y - 1] == type) +
         (grid[x][y - 1] == type) + (grid[x + 1][y - 1] == type) +
         (grid[x + 1][y] == type) + (grid[x + 1][y + 1] == type) +
         (grid[x][y + 1] == type) + (grid[x - 1][y + 1] == type);
}
