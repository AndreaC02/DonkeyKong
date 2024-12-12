// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/
#include "./threads.h"

#include "./MultimodDrivers/multimod.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "driverlib/timer.h"
/*********************************Global Variables**********************************/
GameState currentState = GAMEPLAY; // START
uint32_t score = 0;
uint32_t level_timer = 500;
bool jumped_barrel = false;
uint32_t high_score = 0;
uint32_t level_score = 0;
uint8_t lives = 3;
uint8_t level = 1;
bool transition = 0;
bool win = 0;
bool draw_static_elements = false;

// Audio stuff
uint8_t current_note = 0;  
bool note_ready = false; 
#define SAMPLE_RATE 8000 
int16_t current_volume = 0xFFF;

uint8_t SLOPE_ADJUST = 50;
uint8_t MOVE_PIXELS = 2;
uint8_t MOVE_BARRELS = 3;
uint8_t JUMP_PIXELS = 1;
uint8_t MAX_JUMP_HEIGHT = 15;

// String coords
uint16_t start_string_x = 107;
uint16_t gameover_string_x = 115;
uint16_t string_y = 140;

bool collided = false;

// Mario coords
int16_t mario_x = 40;
int16_t mario_y = 22;
int16_t mario_w = MARIO_SPRITE_WIDTH;
int16_t mario_h = MARIO_SPRITE_HEIGHT;
uint8_t mario_up = 0;
uint8_t mario_platform = 0;
bool on_ladder = false;
bool jump = 0;
uint16_t *mario_sprite = mario_left_sprite;

// Donkey Kong coords
int16_t dk_x = 15;
int16_t dk_y = 175;
int16_t dk_w = DK_SPRITE_WIDTH;
int16_t dk_h = DK_SPRITE_HEIGHT;

// Princess coords
int16_t princess_x = X_MAX / 2;
int16_t princess_y = 205;
int16_t princess_w = PRINCESS_SPRITE_WIDTH;
int16_t princess_h = PRINCESS_SPRITE_HEIGHT;

#define MIN_PLATFORM_Y_SPACING 25  
#define MAX_SLOPE 6               
#define PLATFORM_COUNT 8          
#define LADDERS_PER_LEVEL 2      
#define MIN_LADDER_X 35          
#define MAX_LADDER_X 200         
#define LADDER_SPACING 34 
#define PLATFORM_WIDTH 200
// Array of Platforms
Platform platforms[] = {
    // Bottom platform (flat) - spans full width
    {0, 20, X_MAX, 0, 0, FLAT, ST7789_LIGHTRED},

    // Sloped platforms from bottom to top
    {0, 50, PLATFORM_WIDTH, 5, 4, SLOPE_LEFT_DOWN, ST7789_LIGHTRED},
    {35, 70, PLATFORM_WIDTH, 5, 4, SLOPE_RIGHT_DOWN, ST7789_LIGHTRED}, // y2
    {0, 110, PLATFORM_WIDTH, 5, 4, SLOPE_LEFT_DOWN, ST7789_LIGHTRED},
    {35, 135, PLATFORM_WIDTH, 5, 4, SLOPE_RIGHT_DOWN, ST7789_LIGHTRED}, // y2 = 13

    // Top DK platform - flat section followed by slope
    {100, 170, 100, 5, 4, SLOPE_LEFT_DOWN, ST7789_LIGHTRED}, // Sloped section
    {0, 174, 100, 0, 0, FLAT, ST7789_LIGHTRED},              // Flat section for DK

    // Princess platform at top - centered small platform
    {X_MAX / 2 - 30, 205, 60, 5, 0, FLAT, ST7789_LIGHTRED}};

#define LADDER_WIDTH 12
// Array of Ladders
Ladder ladders[] = {
    // Ground floor ladders
    {60, 20, 55, 0, 1, LADDER_WIDTH, 8, true, ST7789_YELLOW},
    {X_MAX - 80, 20, 55, 0, 1, LADDER_WIDTH, 8, false, ST7789_YELLOW},

    // Second level ladders
    {90, 50, 80, 1, 2, LADDER_WIDTH, 8, false, ST7789_YELLOW},
    {X_MAX - 120, 50, 173, 1, 2, LADDER_WIDTH, 8, false, ST7789_YELLOW},

    // Third level ladders
    {60, 75, 110, 2, 3, LADDER_WIDTH, 8, false, ST7789_YELLOW},
    {X_MAX - 80, 80, 110, 2, 3, LADDER_WIDTH, 8, true, ST7789_YELLOW},

    // Fourth level ladders
    {70, 110, 140, 3, 4, LADDER_WIDTH, 8, true, ST7789_YELLOW},
    {X_MAX - 120, 110, 140, 3, 4, LADDER_WIDTH, 8, false, ST7789_YELLOW},

    // Fifth level ladders
    {100, 140, 170, 4, 5, LADDER_WIDTH, 8, true, ST7789_YELLOW},
    {X_MAX - 80, 140, 170, 4, 5, LADDER_WIDTH, 8, false, ST7789_YELLOW}, // maybe a 4 to 6?

    // Ladder to princess platform
    {90, 170, 200, 6, 7, LADDER_WIDTH, 8, false, ST7789_YELLOW} // Right ladder next to DK
};

#define NUM_PLATFORMS (sizeof(platforms) / sizeof(Platform))
#define NUM_LADDERS (sizeof(ladders) / sizeof(Ladder))

#define MAX_BARRELS 4
Barrel barrels[MAX_BARRELS];
uint8_t active_barrel_count = 0;

// #define SIGNAL_STEPS (sizeof(music) / sizeof(music))
// uint16_t dac_step = 0;
// int16_t current_volume = 0xFFF;

/*********************************Global Variables**********************************/

/*********************************Helper Functions**********************************/
void drawPlatform(const Platform *platform)
{
   switch (platform->type)
   {
   case FLAT:
      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_DrawLine(platform->x, platform->y,
                      platform->x + platform->width, platform->y,
                      platform->color);
      G8RTOS_SignalSemaphore(&sem_SPIA);
      break;

   case SLOPE_RIGHT_DOWN:
      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_DrawDiagonalPlatform(platform->x, platform->y,
                                  platform->width, platform->height,
                                  platform->slope, platform->color);
      G8RTOS_SignalSemaphore(&sem_SPIA);
      break;

   case SLOPE_LEFT_DOWN:
      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_DrawDiagonalPlatform(platform->x, platform->y,
                                  platform->width, platform->height,
                                  -platform->slope, platform->color);
      G8RTOS_SignalSemaphore(&sem_SPIA);
      break;
   }
}

void drawAllPlatforms(void)
{
   for (int i = 0; i < NUM_PLATFORMS; i++)
   {
      drawPlatform(&platforms[i]);
   }
}

void drawLadder(const Ladder *ladder)
{
   // Draw vertical sides

   // Draw rungs
   if (!ladder->is_broken)
   {
      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_DrawLine(ladder->x, ladder->y_bottom,
                      ladder->x, ladder->y_top,
                      ladder->color);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_DrawLine(ladder->x + ladder->width, ladder->y_bottom,
                      ladder->x + ladder->width, ladder->y_top,
                      ladder->color);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      for (uint16_t y = ladder->y_bottom; y <= ladder->y_top; y += ladder->rung_space)
      {
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_DrawLine(ladder->x, y,
                         ladder->x + ladder->width, y,
                         ladder->color);
         G8RTOS_SignalSemaphore(&sem_SPIA);
      }
   }
   else
   {
      uint16_t quarter_height = (ladder->y_top - ladder->y_bottom) / 4;
      uint16_t extra_height = quarter_height / 3; // Draw a bit extra for vertical line

      // Draw bottom section vertical lines (with extra height)
      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_DrawLine(ladder->x, ladder->y_bottom,
                      ladder->x, ladder->y_bottom + quarter_height + extra_height,
                      ladder->color);
      ST7789_DrawLine(ladder->x + ladder->width, ladder->y_bottom,
                      ladder->x + ladder->width, ladder->y_bottom + quarter_height + extra_height,
                      ladder->color);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      // Draw top section vertical lines (with extra height)
      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_DrawLine(ladder->x, ladder->y_top - quarter_height - extra_height,
                      ladder->x, ladder->y_top,
                      ladder->color);
      ST7789_DrawLine(ladder->x + ladder->width, ladder->y_top - quarter_height - extra_height,
                      ladder->x + ladder->width, ladder->y_top,
                      ladder->color);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      // Draw bottom quarter
      for (uint16_t y = ladder->y_bottom; y <= ladder->y_bottom + quarter_height; y += ladder->rung_space)
      {
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_DrawLine(ladder->x, y,
                         ladder->x + ladder->width, y,
                         ladder->color);
         G8RTOS_SignalSemaphore(&sem_SPIA);
      }

      // Draw top quarter
      for (uint16_t y = ladder->y_top - quarter_height; y <= ladder->y_top; y += ladder->rung_space)
      {
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_DrawLine(ladder->x, y,
                         ladder->x + ladder->width, y,
                         ladder->color);
         G8RTOS_SignalSemaphore(&sem_SPIA);
      }
   }
}

void drawAllLadders(void)
{
   for (int i = 0; i < NUM_LADDERS; i++)
   {
      drawLadder(&ladders[i]);
   }
}

void initBarrels()
{
   for (int i = 0; i < MAX_BARRELS; i++)
   {
      barrels[i].active = false;
   }
   active_barrel_count = 0;
}

void drawBarrels()
{
   if (currentState == GAMEPLAY)
   {
      for (int i = 0; i < MAX_BARRELS; i++)
      {
         if (barrels[i].active)
         {
            // Save old position for erasing
            int16_t old_x = barrels[i].x;
            int16_t old_y = barrels[i].y;

            if (barrels[i].falling)
            {
               drawPlatform(&platforms[barrels[i].platform_index]);
               // Move down when falling
               barrels[i].y -= 3; // Falling speed

               // Check for landing on lower platform
               int16_t platform_y = getPlatformYAtX(&platforms[barrels[i].platform_index - 1], barrels[i].x);
               if (platform_y != -1 && barrels[i].y + 1 <= platform_y)
               {
                  barrels[i].falling = false;
                  barrels[i].platform_index--;
                  barrels[i].y = platform_y;
                  barrels[i].dir *= -1; // Change direction when landing
               }
            }
            else
            {
               // Special case: transition from platform 6 (DK's flat platform) to platform 5
               if (barrels[i].platform_index == 6 &&
                   (barrels[i].x < platforms[barrels[i].platform_index].x ||
                    barrels[i].x + BARREL_SPRITE_WIDTH > platforms[barrels[i].platform_index].x + platforms[barrels[i].platform_index].width))
               {
                  barrels[i].platform_index--;
                  // Move horizontally along platform
                  barrels[i].x += (MOVE_BARRELS)*barrels[i].dir;
               }
               // Handle top platform
               else if (barrels[i].platform_index == 6)
               {
                  barrels[i].x += (MOVE_BARRELS * barrels[i].dir);
               }
               // Handle Bottom platform
               else if (barrels[i].platform_index == 0)
               {
                  // Bottom platform - check if off screen
                  barrels[i].x += (MOVE_BARRELS * barrels[i].dir);
                  if (barrels[i].x <= 0 || barrels[i].x + BARREL_SPRITE_WIDTH >= X_MAX)
                  {
                     barrels[i].active = false;
                     active_barrel_count--;
                     continue;
                  }
               }
               // handle all other platforms
               else
               {
                  // Move horizontally
                  barrels[i].x += (MOVE_BARRELS * barrels[i].dir);

                  // Get current platform y at barrel's x position
                  int16_t platform_y = getPlatformYAtX(&platforms[barrels[i].platform_index], barrels[i].x);

                  // Check if barrel has reached end of platform
                  if (platform_y == -1)
                  {
                     barrels[i].falling = true;
                  }
                  else
                  {
                     // Update y position based on platform slope
                     barrels[i].y = platform_y;
                  }
               }
            }

            // Erase old position
            eraseBarrel(old_x, old_y);

            // Draw new position if still active
            if (barrels[i].active)
            {
               drawBarrel(barrels[i].x, barrels[i].y);
            }
         }
      }
   }
}

void clearBarrels()
{
   if (currentState == GAMEPLAY)
   {
      for (int i = 0; i < MAX_BARRELS; i++)
      {
         eraseBarrel(barrels[i].x, barrels[i].y);
      }
   }
}

bool checkCollision()
{
   if (currentState != GAMEPLAY)
   {
      return false;
   }

   // Rectangle collision bounds for Mario (calculate once)
   int16_t mario_left = mario_x;
   int16_t mario_right = mario_x + mario_w;
   int16_t mario_top = mario_y + mario_h;
   int16_t mario_bottom = mario_y;

   // Check collision with each active barrel
   for (int i = 0; i < MAX_BARRELS; i++)
   {
      if (!barrels[i].active)
      {
         continue;
      }

      // Rectangle collision bounds for current barrel
      int16_t barrel_left = barrels[i].x;
      int16_t barrel_right = barrels[i].x + BARREL_SPRITE_WIDTH;
      int16_t barrel_top = barrels[i].y + BARREL_SPRITE_HEIGHT; // Adding size because y increases downward
      int16_t barrel_bottom = barrels[i].y;

      // First check if rectangles do NOT overlap
      if (mario_right < barrel_left || // Mario is completely to left of barrel
          mario_left > barrel_right || // Mario is completely to right of barrel
          mario_bottom > barrel_top || // Mario is completely below barrel
          mario_top < barrel_bottom)
      {            // Mario is completely above barrel
         continue; // No collision with this barrel, check next one
      }

      return true;
   }

   return false;
}

int16_t getPlatformYAtX(const Platform *platform, int16_t x)
{
   // First check if x is within platform bounds
   if (x < platform->x || x > platform->x + platform->width)
   {
      return -1; // Return -1 if x is outside platform bounds
   }

   // For flat platforms, just return the y value plus height
   if (platform->type == FLAT)
   {
      return platform->y + platform->height;
   }

   // Calculate how far along the platform we are
   float progress = (float)(x - platform->x) / platform->width;

   // Calculate total height change based on slope
   int16_t height_change = (platform->width * platform->slope) / 100;

   if (platform->type == SLOPE_LEFT_DOWN)
   {
      // Platform slopes down from left to right
      int16_t y = platform->y - (progress * height_change);
      return y + platform->height + 2;
   }
   else // SLOPE_RIGHT_DOWN
   {
      // Platform slopes up from left to right
      int16_t y = platform->y + (progress * height_change);
      return y + platform->height + 2;
   }
}

bool isOnPlatform(int16_t x, int16_t y, int8_t *platform_index)
{
   const int16_t PLATFORM_TOLERANCE = 2;

   for (int i = 0; i < NUM_PLATFORMS; i++)
   {
      if (x >= platforms[i].x && x <= platforms[i].x + platforms[i].width)
      {
         int16_t platform_y = getPlatformYAtX(&platforms[i], x);

         if (abs(y - platform_y) <= PLATFORM_TOLERANCE)
         {
            if (platform_index != NULL)
            {
               *platform_index = i;
            }
            return true;
         }
      }
   }

   return false;
}

void drawMario(int16_t old_mario_x, int16_t old_mario_y)
{
   // draw mario sprite
   int i = 0;
   G8RTOS_WaitSemaphore(&sem_SPIA);
   for (int y = mario_y + MARIO_SPRITE_HEIGHT; y >= mario_y + 1; y--)
   {
      for (int x = mario_x; x < mario_x + MARIO_SPRITE_WIDTH; x++)
      {
         ST7789_DrawPixel(x, y, mario_sprite[i]);
         i++;
      }
   }
   G8RTOS_SignalSemaphore(&sem_SPIA);

   // Check both current and previous positions for ladder intersections
   int16_t prev_mario_x = old_mario_x;
   int16_t prev_mario_y = old_mario_y;

   // Create a bounding box that covers both old and new positions
   int16_t min_x = (prev_mario_x < mario_x) ? prev_mario_x : mario_x;
   int16_t max_x = (prev_mario_x > mario_x) ? prev_mario_x + MARIO_SPRITE_WIDTH : mario_x + MARIO_SPRITE_WIDTH;
   int16_t min_y = (prev_mario_y < mario_y) ? prev_mario_y : mario_y;
   int16_t max_y = (prev_mario_y > mario_y) ? prev_mario_y + MARIO_SPRITE_HEIGHT : mario_y + MARIO_SPRITE_HEIGHT;

   // Check all ladders that intersect with this larger bounding box
   for (int i = 0; i < NUM_LADDERS; i++)
   {
      if (max_x >= ladders[i].x &&
          min_x <= ladders[i].x + ladders[i].width &&
          max_y >= ladders[i].y_bottom &&
          min_y <= ladders[i].y_top)
      {
         drawLadder(&ladders[i]);
      }
   }
}

void eraseMario(int16_t old_mario_x, int16_t old_mario_y)
{
   G8RTOS_WaitSemaphore(&sem_SPIA);
   for (int y = old_mario_y + MARIO_SPRITE_HEIGHT; y >= old_mario_y + 1; y--)
   {
      for (int x = old_mario_x; x < old_mario_x + MARIO_SPRITE_WIDTH; x++)
      {
         ST7789_DrawPixel(x, y, ST7789_BLACK);
      }
   }
   G8RTOS_SignalSemaphore(&sem_SPIA);

   // Redraw any intersecting ladders
   for (int i = 0; i < NUM_LADDERS; i++)
   {
      if (intersectsLadder(old_mario_x, old_mario_y, MARIO_SPRITE_WIDTH, MARIO_SPRITE_HEIGHT, &ladders[i]))
      {
         drawLadder(&ladders[i]);
      }
   }
}

void drawDK()
{
   int i = 0;
   for (int y = dk_y + DK_SPRITE_HEIGHT; y >= dk_y; y--)
   {
      for (int x = dk_x; x < dk_x + DK_SPRITE_WIDTH; x++)
      {
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_DrawPixel(x, y, dk_sprite[i]);
         G8RTOS_SignalSemaphore(&sem_SPIA);
         i++;
      }
   }
}

void eraseDK()
{
   for (int y = dk_y + DK_SPRITE_HEIGHT; y >= dk_y; y--)
   {
      for (int x = dk_x; x < dk_x + DK_SPRITE_WIDTH; x++)
      {
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_DrawPixel(x, y, ST7789_BLACK);
         G8RTOS_SignalSemaphore(&sem_SPIA);
      }
   }
}

void drawPrincess()
{
   // draw princess sprite
   int i = 0;
   for (int y = princess_y + PRINCESS_SPRITE_HEIGHT; y > princess_y; y--)
   {
      for (int x = princess_x; x < princess_x + PRINCESS_SPRITE_WIDTH; x++)
      {
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_DrawPixel(x, y, princess_sprite[i]);
         G8RTOS_SignalSemaphore(&sem_SPIA);
         i++;
      }
   }
}

void erasePrincess()
{
   // erase princess sprite
   for (int y = princess_y + PRINCESS_SPRITE_HEIGHT; y > princess_y; y--)
   {
      for (int x = princess_x + PRINCESS_SPRITE_WIDTH; x >= princess_x; x--)
      {
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_DrawPixel(x, y, ST7789_BLACK);
         G8RTOS_SignalSemaphore(&sem_SPIA);
      }
   }
}

void drawBarrel(int16_t barrel_x, int16_t barrel_y)
{
   int i = 0;
   // Draw barrel
   for (int y = barrel_y + BARREL_SPRITE_HEIGHT; y >= barrel_y; y--)
   {
      for (int x = barrel_x; x < barrel_x + BARREL_SPRITE_WIDTH; x++)
      {
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_DrawPixel(x, y, barrel_sprite[i]);
         G8RTOS_SignalSemaphore(&sem_SPIA);
         i++;
      }
   }

   // Check and redraw any intersecting ladders
   for (int i = 0; i < NUM_LADDERS; i++)
   {
      if (intersectsLadder(barrel_x, barrel_y, BARREL_SPRITE_WIDTH, BARREL_SPRITE_HEIGHT, &ladders[i]))
      {
         drawLadder(&ladders[i]);
      }
   }
}

void eraseBarrel(int16_t barrel_x, int16_t barrel_y)
{
   G8RTOS_WaitSemaphore(&sem_SPIA);
   for (int y = barrel_y + BARREL_SPRITE_HEIGHT; y >= barrel_y; y--)
   {
      for (int x = barrel_x; x < barrel_x + BARREL_SPRITE_WIDTH; x++)
      {
         ST7789_DrawPixel(x, y, ST7789_BLACK);
      }
   }
   G8RTOS_SignalSemaphore(&sem_SPIA);

   // Redraw any intersecting ladders
   for (int i = 0; i < NUM_LADDERS; i++)
   {
      if (intersectsLadder(barrel_x, barrel_y, BARREL_SPRITE_WIDTH, BARREL_SPRITE_HEIGHT, &ladders[i]))
      {
         drawLadder(&ladders[i]);
      }
   }
}

bool intersectsLadder(int16_t x, int16_t y, int16_t width, int16_t height, const Ladder *ladder)
{
   return (x < ladder->x + ladder->width &&
           x + width > ladder->x &&
           y < ladder->y_top &&
           y + height > ladder->y_bottom);
}

void initLadders(void)
{
   // Update y_top values based on platform heights
   for (int i = 0; i < NUM_LADDERS; i++)
   {
      ladders[i].y_bottom = getPlatformYAtX(&platforms[ladders[i].lower_platform], ladders[i].x);
      ladders[i].y_top = getPlatformYAtX(&platforms[ladders[i].upper_platform], ladders[i].x);
      // UARTprintf("ladder %d: %d, %d\n", i, ladders[i].y_bottom, ladders[i].y_top);
   }
   ladders[1].y_top = getPlatformYAtX(&platforms[1], ladders[1].x);
}

void addJumpPoints()
{
   // Check if Mario is above a barrel and hasn't gotten points for this jump yet
   if (!jumped_barrel && jump)
   {
      for (int i = 0; i < MAX_BARRELS; i++)
      {
         if (barrels[i].active)
         {
            // If Mario is above a barrel
            if (mario_y > barrels[i].y &&
                mario_x < barrels[i].x + BARREL_SPRITE_WIDTH &&
                mario_x + mario_w > barrels[i].x)
            {
               level_score += 100;
               jumped_barrel = true;
               break;
            }
         }
      }
   }
   // Reset jumped_barrel when Mario lands
   if (!jump)
   {
      jumped_barrel = false;
   }
}

void generateRandomLadders(void) {
    // Seed random number generator
    srand(time(NULL));
    
    // Generate two ladders between each main platform
    int ladder_index = 0;
    // Generate ladders for main platforms (excluding DK's sloped platform)
    for (int i = 0; i < PLATFORM_COUNT - 4; i++) {
        // Calculate available range for first ladder
        int first_max = MAX_LADDER_X - LADDER_SPACING;  // Leave room for second ladder
        bool broken = (rand() % 2) ? true : false;

        // First ladder
        int first_x = MIN_LADDER_X + (rand() % (first_max - MIN_LADDER_X));
        ladders[ladder_index++] = (Ladder){
            .x = first_x,
            .width = LADDER_WIDTH,
            .rung_space = 8,
            .is_broken = broken,
            .color = ST7789_YELLOW,
            .lower_platform = i,
            .upper_platform = i + 1
        };
        
        // Second ladder - must be at least LADDER_SPACING pixels away from first
        int second_min = first_x + LADDER_SPACING;
        int second_max = MAX_LADDER_X;
        if (second_min < MAX_LADDER_X) {
            int second_x = second_min + (rand() % (second_max - second_min));
            if (second_x > MAX_LADDER_X) second_x = MAX_LADDER_X - LADDER_WIDTH;
            
            ladders[ladder_index++] = (Ladder){
                .x = second_x,
                .width = LADDER_WIDTH,
                .rung_space = 8,
                .is_broken = !broken,
                .color = ST7789_YELLOW,
                .lower_platform = i,
                .upper_platform = i + 1
            };
        }
    }

    //Add ladder between 4 & 5 and 4 & 6
    int first_max = MAX_LADDER_X - LADDER_SPACING;  // Leave room for second ladder
   bool broken = (rand() % 2) ? true : false;

   // First ladder
   int first_x = MIN_LADDER_X + (rand() % (first_max - MIN_LADDER_X));
   ladders[ladder_index++] = (Ladder){
      .x = first_x,
      .width = LADDER_WIDTH,
      .rung_space = 8,
      .is_broken = broken,
      .color = ST7789_YELLOW,
      .lower_platform = 4,
      .upper_platform = 6
   };
   
   // Second ladder - must be at least LADDER_SPACING pixels away from first
   int second_min = first_x + LADDER_SPACING;
   int second_max = MAX_LADDER_X;
   if (second_min < MAX_LADDER_X) {
      int second_x = second_min + (rand() % (second_max - second_min));
      if (second_x > MAX_LADDER_X) second_x = MAX_LADDER_X - LADDER_WIDTH;
      
      ladders[ladder_index++] = (Ladder){
            .x = second_x,
            .width = LADDER_WIDTH,
            .rung_space = 8,
            .is_broken = !broken,
            .color = ST7789_YELLOW,
            .lower_platform = 4,
            .upper_platform = 5
      };
   }

    
    // Add the final ladder from DK's platform to princess platform
    ladders[ladder_index] = (Ladder){90, 170, 200, 6, 7, LADDER_WIDTH, 8, false, ST7789_YELLOW};
}
/*********************************Helper Functions**********************************/

/*************************************Threads***************************************/

void Idle_Thread(void)
{
   while (1)
      ;
}

void MarioMove_Thread(void)
{
   volatile uint32_t joystick_vals;
   volatile uint16_t js_x, js_y;
   int16_t old_mario_x, old_mario_y;
   const int GRAVITY = 3;
   const float LADDER_MARGIN = 2;
   int8_t ladder_index = -1;
   bool falling = false;

   while (1)
   {
      if (currentState == GAMEPLAY)
      {
         old_mario_x = mario_x;
         old_mario_y = mario_y;

         ladder_index = -1;

         // Get result from joystick
         joystick_vals = G8RTOS_ReadFIFO(JOYSTICK_FIFO);
         js_x = joystick_vals >> 16;
         js_y = joystick_vals & 0xFFFF;

         // Normalize joystick values
         float js_x_add = ((2.0F * (float)(js_x)) / 4096.0F) - 1.0F;
         float js_y_add = ((2.0F * (float)(js_y)) / 4096.0F) - 1.0F;

         // Apply deadzone
         if (js_x_add < 0.35 && js_x_add > -0.35)
            js_x_add = 0;
         if (js_y_add < 0.35 && js_y_add > -0.35)
            js_y_add = 0;

         // select mario sprite based on direction
         if (js_x_add < 0)
         {
            mario_sprite = mario_right_sprite;
         }
         else if (js_x_add > 0)
         {
            mario_sprite = mario_left_sprite;
         }

         // Check if Mario is on a ladder
         on_ladder = false;
         for (int i = 0; i < NUM_LADDERS; i++)
         {
            // Calculate Mario's center x-position
            int16_t mario_center = mario_x + (mario_w / 2);

            // Use margins for easier ladder grabbing
            if ((!ladders[i].is_broken &&
                 mario_center >= ladders[i].x - LADDER_MARGIN &&
                 mario_center <= ladders[i].x + ladders[i].width + LADDER_MARGIN &&
                 mario_y >= ladders[i].y_bottom &&
                 mario_y <= ladders[i].y_top) &&
                !jump) // IGNORE LADDERS WHEN JUMPING
            {
               on_ladder = true;
               ladder_index = i;

               break;
            }
         }

         // Handle vertical movement
         if (on_ladder && js_y_add != 0)
         {
            // On ladder: direct vertical control
            // UARTprintf("js: %d\n", js_y_add);
            // redraw ladder
            // drawLadder(&ladders[ladder_index]);
            // UARTprintf("on ladder\n");

            if (js_y_add > 0)
            {
               mario_y += 1;
            }
            else if (js_y_add < 0)
            {
               mario_y -= 1;
            }

            // Only constrain to ladder if between platforms
            if (mario_y > platforms[mario_platform].y + platforms[mario_platform].height && mario_y < platforms[mario_platform + 1].y + platforms[mario_platform + 1].height)
            {
               // Calculate ladder bounds relative to Mario's center
               int16_t ladder_left = ladders[ladder_index].x;
               int16_t ladder_center = ladder_left + (ladders[ladder_index].width / 2);

               // Keep Mario centered on the ladder
               mario_x = ladder_center - (mario_w / 2);
            }

            // allow to get off ladder and update platform
            int16_t next_top_platform = -1;
            if (mario_platform < NUM_PLATFORMS - 1)
            {
               next_top_platform = getPlatformYAtX(&platforms[mario_platform + 1], mario_x);
               drawPlatform(&platforms[mario_platform]);
            }
            int16_t next_bottom_platform = getPlatformYAtX(&platforms[0], mario_x);
            if (mario_platform > 0)
            {
               next_bottom_platform = getPlatformYAtX(&platforms[mario_platform - 1], mario_x);
            }

            // if move up:
            if (mario_y >= next_top_platform && next_top_platform != -1)
            {
               mario_platform++;
               mario_y = next_top_platform;
               mario_x -= js_x_add;
            }
            // if move down:
            else if (mario_y <= next_bottom_platform)
            {
               if (mario_platform != 0)
               {
                  mario_platform--;
               }
               else
               {
                  mario_platform = 0;
                  drawPlatform(&platforms[0]);
               }
               mario_y = next_bottom_platform;
               mario_x -= js_x_add;
            }
         }
         // handle jump, should go 6 pixels in air and then come down
         else if (jump && !on_ladder)
         {
            // add jump points and update score on screen if necessary
            uint32_t old_score = level_score;
            addJumpPoints();
            if (old_score != level_score)
            {
               G8RTOS_WaitSemaphore(&sem_SPIA);
               ST7789_WriteScore(7 * 10 + 10, 20, old_score, ST7789_BLACK);
               G8RTOS_SignalSemaphore(&sem_SPIA);

               G8RTOS_WaitSemaphore(&sem_SPIA);
               ST7789_WriteScore(7 * 10 + 10, 20, level_score, ST7789_WHITE);
               G8RTOS_SignalSemaphore(&sem_SPIA);
            }

            mario_x -= js_x_add * MOVE_PIXELS;
            // Constrain to screen bounds
            if (mario_x < 0)
               mario_x = 0;
            if (mario_x > X_MAX - mario_w)
               mario_x = X_MAX - mario_w;
            // mario_up++;
            uint8_t max_height = MAX_JUMP_HEIGHT;
            if (getPlatformYAtX(&platforms[mario_platform + 1], mario_x) != -1)
            {
               max_height = getPlatformYAtX(&platforms[mario_platform + 1], mario_x) - 3 - platforms[mario_platform + 1].height - mario_y;
               // UARTprintf("max height got : %d\n", max_height);
            }
            // UARTprintf("max height: %d\n", max_height);
            if (mario_up >= max_height)
            {
               mario_y -= JUMP_PIXELS;

               if (mario_platform + 1 < NUM_PLATFORMS)
               {
                  drawPlatform(&platforms[mario_platform + 1]);
               }
               // Check for landing
               if (mario_y <= getPlatformYAtX(&platforms[mario_platform], mario_x))
               {
                  // UARTprintf("MARIO LANDED\n");
                  jump = 0;
                  mario_up = 0;
                  mario_y = getPlatformYAtX(&platforms[mario_platform], mario_x);
                  if (mario_platform == 0)
                  {
                     mario_y = 22;
                  }
                  drawPlatform(&platforms[mario_platform]);
               }
            }
            else
            {
               mario_y += JUMP_PIXELS;
               mario_up += JUMP_PIXELS;
               // UARTprintf("updating mario up: %d\n", mario_up);
            }
         }
         else
         {
            mario_x -= js_x_add * MOVE_PIXELS;
            // Constrain to screen bounds
            if (mario_x < 0)
               mario_x = 0;
            if (mario_x > X_MAX - mario_w)
               mario_x = X_MAX - mario_w;

            if (falling == true)
            {
               uint16_t lower_bound = (mario_platform == 0) ? 22 : 174;
               if (mario_platform != 0 && mario_platform != 6)
               {
                  lower_bound = getPlatformYAtX(&platforms[mario_platform - 1], mario_x);
               }
               if (mario_y > lower_bound)
               {
                  mario_y--;
               }
               else
               {
                  falling = false;
                  mario_y = lower_bound;
                  if (mario_platform > 0)
                  {
                     mario_platform--;
                  }
               }
            }
            else if (!falling)
            {
               // Special case for platform 5 to 6 transition
               if (mario_platform == 5 && mario_x >= platforms[6].x && mario_x <= platforms[6].x + platforms[6].width)
               {
                  mario_platform = 6;
                  mario_y = platforms[6].y + platforms[6].height;
               }
               // Normal platform handling
               else if (mario_platform != 0)
               {
                  int16_t platform_y = getPlatformYAtX(&platforms[mario_platform], mario_x);
                  if (platform_y == -1)
                  {
                     mario_y -= GRAVITY;
                     falling = true;
                  }
                  else
                  {
                     mario_y = platform_y;
                  }
               }
            }
         }

         if (checkCollision() && !collided)
         {
            // UARTprintf("COLLIDED\n");
            collided = true;
            level_score = 0;
            lives--;

            if (lives == 0)
            {
               if (score > high_score)
               {
                  high_score = score;
               }
               transition = 1;
               currentState = GAMEOVER;
            }
         }

         // If position changed, update display
         if (old_mario_x != mario_x || old_mario_y != mario_y)
         {
            // Check for level completion
            if (mario_y >= 200)
            {
               score += (level_timer * 100 + 1000 + level_score);
               level++;
               transition = 1;
               win = 1;
               generateRandomLadders();
               currentState = LEVELWON;
            }
            else
            {
               // Erase old position
               eraseMario(old_mario_x, old_mario_y);

               // Draw new position
               drawMario(old_mario_x, old_mario_y);
            }
         }
      }
      else
      {
         sleep(15);
      }
   }
}

void Read_Buttons()
{
   // Initialize / declare any variables here
   uint8_t buttons;

   while (1)
   {
      // UARTprintf("IN READ BUTTONS\n");
      //  Wait for a signal to read the buttons on the Multimod board.
      G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);
      // Sleep to debounce
      sleep(10);
      // Read the buttons status on the Multimod board.
      // G8RTOS_WaitSemaphore(&sem_I2CA);
      buttons = MultimodButtons_Get();
      // G8RTOS_SignalSemaphore(&sem_I2CA);

      // button functionality depend on state
      if (currentState == START)
      {
         if (buttons == SW1 || buttons == SW2 || buttons == SW3 || buttons == SW4)
         {
            transition = 1;
            currentState = GAMEPLAY;
         }
      }

      else if (currentState == GAMEPLAY)
      {
         if (buttons == SW1)
            jump = 1;
      }
      else if (currentState == GAMEOVER)
      {
         if (buttons == SW1 || buttons == SW2 || buttons == SW3 || buttons == SW4)
         {
            transition = 1;
            currentState = START;
         }
      }
      else if (currentState == LEVELWON)
      {
         if (buttons == SW1 || buttons == SW2 || buttons == SW3 || buttons == SW4)
         {
            if(level == 3)
            {
               if (score > high_score)
               {
                  high_score = score;
               }
               transition = 1;
               currentState = START;
            }
            else
            {
            transition = 1;
            currentState = GAMEPLAY;
            }
         }
      }

      // Clear the interrupt
      GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);

      // Re-enable the interrupt so it can occur again.
      GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
   }
}

/********************************Periodic Threads***********************************/

void Get_Joystick(void)
{
   // Read the joystick
   // UARTprintf("IN GET JOYSTICK");
   if (currentState == GAMEPLAY)
   {
      uint32_t xy = JOYSTICK_GetXY();

      // Send through FIFO.
      G8RTOS_WriteFIFO(JOYSTICK_FIFO, xy);
   }
}

void Draw_Screen(void)
{
   // Draw the screen
   // UARTprintf("IN DRAW SCREEN");
   if (transition == 1)
   {
      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_Fill(ST7789_BLACK);
      G8RTOS_SignalSemaphore(&sem_SPIA);
      transition = 0;
      draw_static_elements = false;
      // reset necessary variables
      win = 0;
      level_score = 0;
      score = 0;
      level_timer = 500;
      jumped_barrel = false;
      // reset mario
      mario_x = 40;
      mario_y = 20;
      mario_platform = 0;
      mario_sprite = mario_left_sprite;
      // reset barrels
      clearBarrels();
      initBarrels();
      generateRandomLadders();
      initLadders();
   }
   if (currentState == START)
   {
      // reset necessary variables
      win = 0;
      lives = 3;
      level_score = 0;
      score = 0;
      level_timer = 500;
      jumped_barrel = false;
      level = 0;
      // reset mario
      mario_x = 40;
      mario_y = 20;
      mario_platform = 0;
      mario_sprite = mario_left_sprite;
      // reset barrels
      clearBarrels();
      initBarrels();
      initLadders();

      // draw start screen
      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_WriteString(start_string_x - 5, string_y, "START", ST7789_RED);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_WriteString(start_string_x - 70, string_y + 12, "PRESS ANY SW TO BEGIN", ST7789_WHITE);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_WriteString(start_string_x - 50, string_y + 24, "HIGH SCORE ", ST7789_WHITE);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_WriteScore(start_string_x - 50 + (10 * 10), string_y + 24, high_score, ST7789_WHITE);
      G8RTOS_SignalSemaphore(&sem_SPIA);
   }
   else if (currentState == GAMEPLAY)
   {
      if (collided == true)
      {
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_Fill(ST7789_BLACK);
         G8RTOS_SignalSemaphore(&sem_SPIA);
         collided = false;
         initBarrels();
         mario_x = 40;
         mario_y = 20;
         mario_platform = 0;
         level_score = 0;
         drawMario(0, 0);
         draw_static_elements = false;
      }

      if (!draw_static_elements)
      {
         initLadders();
         draw_static_elements = true;
         
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteString(10, 20, "SCORE ", ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);

         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteScore(7 * 10 + 10, 20, level_score, ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);

         // draw lives
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteString(10, 32, "LIVES ", ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);

         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteScore(7 * 10 + 10, 32, lives, ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);

         // draw level
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteString(10, 44, "LEVEL: ", ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);

         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteScore(7 * 10 + 10, 44, level, ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);

         // draw time
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteString(120, 20, "TIME: ", ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);

         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteScore(7 * 10 + 90, 20, level_timer, ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);

         // draw platforms
         drawAllPlatforms();
         // draw ladders
         drawAllLadders();
         // draw princess
         drawPrincess();
         // draw dk
         drawDK();
      }

      // draw hazards, rn barrels other stuff later if time, fireballs?
      drawBarrels();

      // draw bonus items !!!!!!!!!!! LATER FEATURE !!!!!!!!!!!!!
   }
   else if (currentState == LEVELWON)
   {
      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_WriteString(X_MAX / 2 - 25, string_y, "LEVEL WON!", ST7789_WHITE);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      if(level < 3){
         win = 0;
         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteString(X_MAX / 2 - 90, string_y + 22, "PRESS ANY SW TO CONTINUE", ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);
      }
      else{
          G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteString(X_MAX / 2 - 50, string_y + 22, "FINAL SCORE ", ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);

         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteScore(X_MAX / 2 - 50 + (10 * 10), string_y + 22, score, ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);

         G8RTOS_WaitSemaphore(&sem_SPIA);
         ST7789_WriteString(X_MAX / 2 - 90, string_y + 44, "PRESS ANY SW TO RESTART", ST7789_WHITE);
         G8RTOS_SignalSemaphore(&sem_SPIA);
      }
   }
   else if (currentState == GAMEOVER)
   {
      // draw gameover screen
      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_WriteString(X_MAX / 2 - 35, string_y, "GAME OVER", ST7789_WHITE);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_WriteString(X_MAX / 2 - 50, string_y + 22, "FINAL SCORE ", ST7789_WHITE);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_WriteScore(X_MAX / 2 - 50 + (10 * 10), string_y + 22, score, ST7789_WHITE);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_WriteString(X_MAX / 2 - 90, string_y + 44, "PRESS ANY SW TO RESTART", ST7789_WHITE);
      G8RTOS_SignalSemaphore(&sem_SPIA);
   }
}

void Generate_Barrel(void)
{
   // UARTprintf("IN GENERATE BARRREL\n");
   if (currentState == GAMEPLAY)
   {
      if (active_barrel_count < MAX_BARRELS && currentState == GAMEPLAY)
      {
         for (int i = 0; i < MAX_BARRELS; i++)
         {
            if (barrels[i].active == 0)
            {
               // UARTprintf("GENERATING BARRREL: %d\n", i);
               barrels[i].x = dk_x + dk_w + 1;
               barrels[i].y = dk_y + 2;
               barrels[i].active = 1;
               barrels[i].platform_index = 6; // DK's platform
               barrels[i].falling = false;
               barrels[i].dir = 1; // Start moving Right
               barrels[i].err = platforms[barrels[i].platform_index].width / 2;
               active_barrel_count++;
               break;
            }
         }
      }
   }
}

void Update_Timer(void)
{
   if (currentState == GAMEPLAY && level_timer > 0)
   {
      level_timer--;

      // Update timer on screen
      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_WriteScore(7 * 10 + 90, 20, level_timer + 1, ST7789_BLACK);
      G8RTOS_SignalSemaphore(&sem_SPIA);

      G8RTOS_WaitSemaphore(&sem_SPIA);
      ST7789_WriteScore(7 * 10 + 90, 20, level_timer, ST7789_WHITE);
      G8RTOS_SignalSemaphore(&sem_SPIA);
   }
}
/*******************************Aperiodic Threads***********************************/

void GPIOE_Handler()
{
   // Disable interrupt
   GPIOIntDisable(GPIO_PORTE_BASE, GPIO_PIN_4);

   // Signal relevant semaphore
   G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);
}

void UART4_Handler() {
    uint32_t status = UARTIntStatus(UART4_BASE, true);
    
    // Read single byte/note from UART
    if(UARTCharsAvail(UART4_BASE)) {
        current_note = UARTCharGet(UART4_BASE);

        // Convert 8-bit unsigned (0-255) to signed (-128 to 127)
        int16_t signed_sample = current_note - 128;

        // Scale signed value to 12-bit DAC range and center around midpoint
        uint32_t output;
        if (signed_sample >= 0) {
            output = 2048 + ((signed_sample * 2047) / 127);
        } else {
            output = 2048 + ((signed_sample * 2048) / 128);
        }

        // Apply volume scaling if needed
        output = (output * current_volume) >> 12;

        // Ensure output stays within DAC range
        if (output > 4095) output = 4095;

        MutimodDAC_Write(DAC_OUT_REG, output);
    }
    
    //UARTprintf("current note: %d", current_note);

    UARTIntClear(UART4_BASE, status);
}
