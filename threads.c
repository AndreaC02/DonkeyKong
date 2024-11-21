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
/*********************************Global Variables**********************************/
GameState currentState = GAMEPLAY; // START
uint32_t score = 0;
uint32_t high_score = 0;
uint8_t lives = 3;
uint8_t level = 1;
bool transition = 0;
bool win = 0;

uint8_t SLOPE_ADJUST = 50;
uint8_t MOVE_PIXELS = 5;
uint8_t MOVE_BARRELS = 1;
uint8_t JUMP_PIXELS = 2;
uint8_t MAX_JUMP_HEIGHT = 8;

uint16_t start_string_x = 107;
uint16_t gameover_string_x = 115;
uint16_t string_y = 140;

//Mario coords
int16_t mario_x = 40;
int16_t mario_y = 20;
int16_t mario_w = 5;
int16_t mario_h = 8;
uint8_t mario_up = 0;
uint8_t mario_down = 0;
uint8_t mario_platform = 0;
Direction mario_dir = STILL;
bool on_ladder = false;

//Donkey Kong coords
int16_t dk_x = 15;
int16_t dk_y = 175;
int16_t dk_w = 12;
int16_t dk_h = 12;

//Princess coords
int16_t princess_x = X_MAX/2;
int16_t princess_y = 200;
int16_t princess_w = 5;
int16_t princess_h = 10;

//Array of Platforms
Platform platforms[] = {
   // Bottom platform (flat) - spans full width
   {0, 20, X_MAX, 0, 0, FLAT, ST7789_LIGHTRED},

   // Sloped platforms from bottom to top - shorter, more evenly spaced
   {10, 50, 200, 5, 4, SLOPE_LEFT_DOWN, ST7789_LIGHTRED},
   {30, 70, 200, 5, 4, SLOPE_RIGHT_DOWN, ST7789_LIGHTRED}, //y2
   {10, 110, 200, 5, 4, SLOPE_LEFT_DOWN, ST7789_LIGHTRED},
   {30, 135, 200, 5, 4, SLOPE_RIGHT_DOWN, ST7789_LIGHTRED}, //y2 = 13
   //{40, 170, 100, 5, 15, SLOPE_LEFT_DOWN, ST7789_LIGHTRED},
   // Top DK platform - flat section followed by slope
   {100, 170, 100, 5, 4, SLOPE_LEFT_DOWN, ST7789_LIGHTRED},    // Sloped section
   {10, 174, 100, 0, 0, FLAT, ST7789_LIGHTRED},    // Flat section for DK

   // Princess platform at top - centered small platform
   {X_MAX/2 - 30, 200, 60, 5, 0, FLAT, ST7789_LIGHTRED}
};

//Array of Ladders
Ladder ladders[] = {
   // Ground floor ladders
   {60, 20, 55, 12, 8, true, ST7789_YELLOW},
   {X_MAX-80, 20, 55, 12, 8, false, ST7789_YELLOW},

   // Second level ladders
   //{X_MAX-120, 50, 80, 10, 8, false, ST7789_YELLOW},
   //{60, 50, 80, 10, 8, true, ST7789_YELLOW},

   // Third level ladders
   //{60, 80, 110, 10, 8, false, ST7789_YELLOW},
   //{X_MAX-80, 80, 110, 10, 8, true, ST7789_YELLOW},

   // Fourth level ladders
   //{X_MAX-120, 110, 140, 10, 8, false, ST7789_YELLOW},
   //{60, 110, 140, 10, 8, true, ST7789_YELLOW},

   // Fifth level ladders
   //{60, 140, 170, 10, 8, false, ST7789_YELLOW},
   //{X_MAX-80, 140, 170, 10, 8, false, ST7789_YELLOW},

   // Two ladders from DK platform to princess platform
   {70, 170, 200, 10, 8, false, ST7789_YELLOW},    // Left ladder next to DK
   {90, 170, 200, 10, 8, false, ST7789_YELLOW}     // Right ladder next to DK
};

#define NUM_PLATFORMS (sizeof(platforms) / sizeof(Platform))
#define NUM_LADDERS (sizeof(ladders) / sizeof(Ladder))

#define MAX_BARRELS 1 // 4
#define BARREL_SIZE 5  // width and height of barrel
Barrel barrels[MAX_BARRELS];
uint8_t active_barrel_count = 0;

/*********************************Global Variables**********************************/

/*********************************Helper Functions**********************************/
void drawPlatform(const Platform* platform) {
   switch(platform->type) {
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

void drawAllPlatforms(void) {
   for(int i = 0; i < NUM_PLATFORMS; i++) {
       drawPlatform(&platforms[i]);
   }
}

void drawLadder(const Ladder* ladder) {
    // Draw vertical sides

    // Draw rungs
    if (!ladder->is_broken) {
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

        for(uint16_t y = ladder->y_bottom; y <= ladder->y_top; y += ladder->rung_space) {
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawLine(ladder->x, y,
                          ladder->x + ladder->width, y,
                          ladder->color);
            G8RTOS_SignalSemaphore(&sem_SPIA);
        }
    }
    else {
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
        for(uint16_t y = ladder->y_bottom; y <= ladder->y_bottom + quarter_height; y += ladder->rung_space) {
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawLine(ladder->x, y,
                          ladder->x + ladder->width, y,
                          ladder->color);
            G8RTOS_SignalSemaphore(&sem_SPIA);
        }

        // Draw top quarter
        for(uint16_t y = ladder->y_top - quarter_height; y <= ladder->y_top; y += ladder->rung_space) {
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawLine(ladder->x, y,
                          ladder->x + ladder->width, y,
                          ladder->color);
            G8RTOS_SignalSemaphore(&sem_SPIA);
        }
\
    }
}

void drawAllLadders(void) {
   for(int i = 0; i < NUM_LADDERS; i++) {
       drawLadder(&ladders[i]);
   }
}

void initBarrels() {
    for(int i = 0; i < MAX_BARRELS; i++) {
        barrels[i].active = false;
    }
    active_barrel_count = 0;
}

void drawBarrels() {
    if(currentState == GAMEPLAY) {
        for(int i = 0; i < MAX_BARRELS; i++) {
            //UARTprintf("BARREL INDEX AND ACTIVE?: %d\t%d\n", i, barrels[i].active);
            if(barrels[i].active == 1) {
                //UARTprintf("ACTIVE BARREL INDEX: %d\n", i);
                //UARTprintf("I: %d\tDIR: %d\tFALL: %d\tPI: %d\n", i, barrels[i].dir, barrels[i].falling, barrels[i].platform_index);
                // Save old position for erasing
                int16_t old_x = barrels[i].x;
                int16_t old_y = barrels[i].y;

                uint16_t landing_y = 0;
                if(platforms[barrels[i].platform_index - 1].type != SLOPE_RIGHT_DOWN){
                    landing_y = platforms[barrels[i].platform_index - 1].y + platforms[barrels[i].platform_index - 1].height;
                }
                else{
                    landing_y = platforms[barrels[i].platform_index - 1].y + platforms[barrels[i].platform_index - 1].height + (platforms[barrels[i].platform_index - 1].width * platforms[barrels[i].platform_index - 1].slope / 100);
                }
                // Move barrel based on state
                if(barrels[i].falling) {
                    //Check for collision with lower platform
                    if(barrels[i].y <= landing_y){
                        barrels[i].falling = 0;
                        --barrels[i].platform_index;
                        barrels[i].dir *= -1;
                    }
                    //else: no collision:
                    else{
                        --barrels[i].y;
                    }
                } 
                else {
                    //special case: reached end of platform 6, DK flat platform
                    if(barrels[i].platform_index == 6 &&
                        (barrels[i].x < platforms[barrels[i].platform_index].x ||
                         barrels[i].x + BARREL_SIZE > platforms[barrels[i].platform_index].x + platforms[barrels[i].platform_index].width)){
                        barrels[i].platform_index--;
                        // Move horizontally along platform
                        barrels[i].x += (MOVE_BARRELS) * barrels[i].dir;

                       if(platforms[barrels[i].platform_index].type != FLAT) {
                           // Convert slope percentage to pixel movement
                           float slope_pixel_adjust = (float)(platforms[barrels[i].platform_index].slope) / 100.0f * MOVE_BARRELS;
                           barrels[i].y -= (int16_t)slope_pixel_adjust;
                       }

                    }
                    //else check if barrel is on bottom flat platform and offscreen
                    else if(barrels[i].platform_index == 0){
                        barrels[i].x += (MOVE_BARRELS) * barrels[i].dir;
                        if(barrels[i].x <= 0 || barrels[i].x + BARREL_SIZE >= X_MAX){
                            barrels[i].active = false;
                            active_barrel_count--;
                        }
                    }
                    //else check if barrel reached end of current platform
                    else if(barrels[i].x + BARREL_SIZE < platforms[barrels[i].platform_index].x ||
                            barrels[i].x > platforms[barrels[i].platform_index].x + platforms[barrels[i].platform_index].width){
                        //set barrel to fall
                        barrels[i].falling = true;
                    }
                    else {
                        // Move horizontally along platform
                        barrels[i].x += (MOVE_BARRELS) * barrels[i].dir;

                        if(platforms[barrels[i].platform_index].type != FLAT) {
                            // Calculate how many horizontal pixels per 1 vertical pixel
                            int16_t pixels_per_step = platforms[barrels[i].platform_index].width / platforms[barrels[i].platform_index].height;

                            // Adjust y position based on relative movement along platform
                            if((barrels[i].x - platforms[barrels[i].platform_index].x) % pixels_per_step == 0) {
                                barrels[i].y--;  // Move down

                            }
                        }
                    }
                }
                // Erase old position
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(old_x, old_y, BARREL_SIZE, BARREL_SIZE, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);
                // Draw new position if still active
                if(barrels[i].active) {
                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawRectangle(barrels[i].x, barrels[i].y, BARREL_SIZE, BARREL_SIZE, ST7789_WHITE);
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                }
            }
        }
    }
}

void clearBarrels() {
    if(currentState == GAMEPLAY) {
        for(int i = 0; i < MAX_BARRELS; i++) {
            if(barrels[i].active == 1) {
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(barrels[i].x, barrels[i].y, BARREL_SIZE, BARREL_SIZE, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);
            }
        }
    }
}

//!!!!!!!!!!! MAKE THIS CLEANER !!!!!!!!!!!!!!
bool checkCollision(){
    //UARTprintf("CHECK COLLISION");
    if(currentState == GAMEPLAY){
        //UARTprintf("CHECK COLLISION\n");
        for(int i = 0; i < MAX_BARRELS; i++){
            if(barrels[i].active == 1){
                //check every pixel in mario
                for(int x = mario_x; x < mario_x + mario_w; x++){
                    for(int y = mario_y; y < mario_y + mario_h; y++){
                        //check every pixel of barrel
                        for(int a = barrels[i].x; a < barrels[i].x + BARREL_SIZE; a++){
                            for(int b = barrels[i].y; b < barrels[i].y + BARREL_SIZE; b++){
                                if(a == x && b == y){
                                    //UARTprintf("COLLISION DETECTED\n");
                                    // Collision detected
                                    return true;
                                }
                            }

                        }

                    }
                }
            }
        }
        return false;
    }
    return false;
}
/*********************************Helper Functions**********************************/


/*************************************Threads***************************************/

void Idle_Thread(void)
{
    while (1);
}

/*
void MarioMove_Thread(void) {
    volatile uint32_t joystick_vals;
    volatile uint16_t js_x, js_y;
    int16_t old_mario_x, old_mario_y;

    while(1) {
        if(currentState == GAMEPLAY) {
            old_mario_x = mario_x;
            old_mario_y = mario_y;

            // Get result from joystick
            joystick_vals = G8RTOS_ReadFIFO(JOYSTICK_FIFO);
            js_x = joystick_vals >> 16;
            js_y = joystick_vals & 0xFFFF;

            // If joystick axis within deadzone, set to 0. Otherwise normalize it.
            float js_x_add = ((2.0F*(float)(js_x)) / 4096.0F) - 1.0F;
            float js_y_add = ((2.0F*(float)(js_y)) / 4096.0F) - 1.0F;

            // If joystick axis within deadzone, set to 0.
            if(js_x_add < 0.35 && js_x_add > -0.35) {
                js_x_add = 0;
                mario_dir = STILL;
            } else if(js_x_add > 0) {
                mario_dir = RIGHT;
            } else if(js_x_add < 0) {
                mario_dir = LEFT;
            }

            // First check for ladder movement
            on_ladder = false;
            for(int i = 0; i < NUM_LADDERS; i++) {
                if(mario_x >= ladders[i].x - mario_w/2 && 
                   mario_x <= ladders[i].x + ladders[i].width - mario_w/2) {
                    if(mario_y >= ladders[i].y_top && mario_y <= ladders[i].y_bottom) {
                        if(js_y_add > 0.35 || js_y_add < -0.35) {  // Only engage ladder if trying to move vertically
                            on_ladder = true;
                            // Move vertically on ladder
                            if(js_y_add > 0.35 && mario_y < ladders[i].y_bottom) {
                                mario_y++;
                            } else if(js_y_add < -0.35 && mario_y > ladders[i].y_top) {
                                mario_y--;
                            }
                            break;
                        }
                    }
                }
            }

            // Handle platform movement if not on ladder
            if(!on_ladder) {
                if(mario_platform == 0 || mario_platform == 6 || mario_platform == 7) {
                    // Flat platforms - simple horizontal movement
                    int16_t new_x = mario_x - (js_x_add * MOVE_PIXELS);
                    
                    // Check if new position would still be on platform
                    if(new_x >= platforms[mario_platform].x && 
                       new_x + mario_w <= platforms[mario_platform].x + platforms[mario_platform].width) {
                        mario_x = new_x;
                    }
                }
                else if(mario_platform > 0 && mario_platform < NUM_PLATFORMS - 1) {
                    // Sloped platforms
                    int16_t new_x = mario_x - (js_x_add * MOVE_PIXELS);
                    
                    if(new_x >= platforms[mario_platform].x && 
                       new_x + mario_w <= platforms[mario_platform].x + platforms[mario_platform].width) {
                        mario_x = new_x;
                        
                        // Handle slope movement
                        if(platforms[mario_platform].type == SLOPE_LEFT_DOWN) {
                            if(mario_dir == RIGHT) {
                                mario_y++;
                            } else if(mario_dir == LEFT) {
                                mario_y--;
                            }
                        }
                        else if(platforms[mario_platform].type == SLOPE_RIGHT_DOWN) {
                            if(mario_dir == RIGHT) {
                                mario_y--;
                            } else if(mario_dir == LEFT) {
                                mario_y++;
                            }
                        }
                    }
                }

                // Update platform tracking
                bool on_platform = false;
                for(int i = 0; i < NUM_PLATFORMS; i++) {
                    if(platforms[i].type == FLAT) {
                        if(mario_y == platforms[i].y && 
                           mario_x >= platforms[i].x && 
                           mario_x + mario_w <= platforms[i].x + platforms[i].width) {
                            mario_platform = i;
                            on_platform = true;
                            break;
                        }
                    }
                }

                // Handle falling if not on platform and not on ladder
                if(!on_platform && !on_ladder) {
                    mario_y++;
                }
            }

            // Check for collisions with barrels
            if(checkCollision()) {
                // Clear lives display
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_WriteScore(7 * 10 + 10, 32, lives, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);

                lives--;

                // Erase Mario
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(mario_x, mario_y, mario_w, mario_h, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);

                // Reset Mario position
                mario_x = 40;
                mario_y = 20;
                mario_platform = 0;

                // Clear barrels
                clearBarrels();
                initBarrels();

                if(lives == 0) {
                    if(score > high_score) {
                        high_score = score;
                    }
                    transition = 1;
                    currentState = GAMEOVER;
                }
            }

            // Update Mario's position on screen if changed
            if(old_mario_x != mario_x || old_mario_y != mario_y) {
                // Erase old position
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(old_mario_x, old_mario_y, mario_w, mario_h, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);

                // Check win condition
                if(mario_y >= 200) {
                    transition = 1;
                    win = 1;
                    currentState = LEVELWON;
                } else {
                    // Draw new position
                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawRectangle(mario_x, mario_y, mario_w, mario_h, ST7789_RED);
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                }
            }
        }
        else {
            sleep(15);
        }
    }
}
*/

/*
void MarioMove_Thread(void) {
   volatile uint32_t joystick_vals;
   volatile uint16_t js_x, js_y;
   int16_t old_mario_x, old_mario_y;

    while(1) {
        //UARTprintf("In MARIO MOVE THREAD\n");
        if(currentState == GAMEPLAY) {
            old_mario_x = mario_x;
            old_mario_y = mario_y;

            // Get result from joystick
            joystick_vals = G8RTOS_ReadFIFO(JOYSTICK_FIFO);
            js_x = joystick_vals >> 16;
            js_y = joystick_vals & 0xFFFF;


            // If joystick axis within deadzone, set to 0. Otherwise normalize it.
            float js_x_add = ((2.0F*(float)(js_x)) / 4096.0F) - 1.0F;
            float js_y_add = ((2.0F*(float)(js_y)) / 4096.0F) - 1.0F;

            // If joystick axis within deadzone, set to 0.
            if(js_x_add < 0.35 && js_x_add > -0.35)
            {
                js_x_add = 0;
            }
            if(js_y_add < 0.35 && js_y_add > (-0.35))
            {
                js_y_add = 0;
            }

            // Update MARIO pos
            //!!!!!!!! update all pixels once graphics better !!!!!!!!!
            mario_y += js_y_add * MOVE_PIXELS;
            mario_x -= js_x_add * MOVE_PIXELS;
           
            if(old_mario_x != mario_x || old_mario_y != mario_y || (old_mario_y == 30 && old_mario_x == 40)){
                //erase mario
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(old_mario_x, old_mario_y, mario_w, mario_h, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);

                //if won level:
                if( mario_y >= 200){
                    transition = 1;
                    win = 1;
                    currentState = LEVELWON;

                }

                else{
                    //draw new mario
                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawRectangle(mario_x, mario_y, mario_w, mario_h, ST7789_RED);
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                }



            }
        }
        else{
            sleep(15);
        }

        // sleep
        //sleep(15);
    }
}
*/

void MarioMove_Thread(void) {
    volatile uint32_t joystick_vals;
    volatile uint16_t js_x, js_y;
    int16_t old_mario_x, old_mario_y;
    bool on_platform = false;
    const int GRAVITY = 3;

    while(1) {
        if(currentState == GAMEPLAY) {
            old_mario_x = mario_x;
            old_mario_y = mario_y;

            // Get result from joystick
            joystick_vals = G8RTOS_ReadFIFO(JOYSTICK_FIFO);
            js_x = joystick_vals >> 16;
            js_y = joystick_vals & 0xFFFF;

            // Normalize joystick values
            float js_x_add = ((2.0F*(float)(js_x)) / 4096.0F) - 1.0F;
            float js_y_add = ((2.0F*(float)(js_y)) / 4096.0F) - 1.0F;

            // Apply deadzone
            if(js_x_add < 0.35 && js_x_add > -0.35) js_x_add = 0;
            if(js_y_add < 0.35 && js_y_add > -0.35) js_y_add = 0;

            // Handle horizontal movement
            mario_x -= js_x_add * MOVE_PIXELS;

            // Constrain to screen bounds
            if(mario_x < 0) mario_x = 0;
            if(mario_x > X_MAX - mario_w) mario_x = X_MAX - mario_w;

            // Check if Mario is on a ladder
            on_ladder = false;
            for(int i = 0; i < NUM_LADDERS; i++) {
                if(!ladders[i].is_broken && 
                   mario_x + mario_w/2 >= ladders[i].x &&
                   mario_x + mario_w/2 <= ladders[i].x + ladders[i].width &&
                   mario_y >= ladders[i].y_bottom && 
                   mario_y <= ladders[i].y_top) {
                    on_ladder = true;
                    break;
                }
            }

            // Handle vertical movement
            if(on_ladder) {
                // On ladder: direct vertical control
                mario_y += js_y_add * MOVE_PIXELS;  // Negative because Y_MAX is at top
            }
            else {
                /*

                // Check if Mario is on a platform
                on_platform = false;
                int16_t lowest_platform_y = 0;  // Track the highest platform Mario could be on

                for(int i = 0; i < NUM_PLATFORMS; i++) {
                    Platform* p = &platforms[i];
                    
                    // Only check if Mario's center is within the platform's x-range
                    if(mario_x + mario_w/2 >= p->x && mario_x + mario_w/2 <= p->x + p->width) {
                        int16_t platform_y;
                        float x_progress = (float)(mario_x + mario_w/2 - p->x) / p->width;

                        if(p->type == FLAT) {
                            platform_y = p->y;
                        } else if(p->type == SLOPE_LEFT_DOWN) {
                            // For left-down slope, y decreases as x increases
                            platform_y = p->y - (x_progress * p->slope * p->width / 100);
                        } else { // SLOPE_RIGHT_DOWN
                            // For right-down slope, y increases as x increases
                            platform_y = p->y + (x_progress * p->slope * p->width / 100);
                        }

                        // Check if this platform is below Mario but higher than previous platforms
                        if(mario_y + mario_h >= platform_y && platform_y > lowest_platform_y) {
                            lowest_platform_y = platform_y;
                            // Only set on_platform if Mario is very close to the platform
                            if(mario_y + mario_h >= platform_y &&
                               mario_y + mario_h <= platform_y + GRAVITY * 2) {
                                on_platform = true;
                                mario_y = platform_y - mario_h;  // Place Mario on platform
                            }
                        }
                    }


                }
                

                // Apply gravity if not on a platform
                if(!on_platform) {
                    mario_y += GRAVITY;  // Add because Y_MAX is at top
                }
                */
                mario_x -= js_x_add;
            }

            // Constrain vertical position to screen bounds
            if(mario_y < 0) mario_y = 0;
            if(mario_y > Y_MAX - mario_h) mario_y = Y_MAX - mario_h;

            // If position changed, update display
            if(old_mario_x != mario_x || old_mario_y != mario_y) {
                // Check for level completion
                if(mario_y >= 200) {
                    transition = 1;
                    win = 1;
                    currentState = LEVELWON;
                } else {
                    // Erase old position
                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawRectangle(old_mario_x, old_mario_y, mario_w, mario_h, ST7789_BLACK);
                    G8RTOS_SignalSemaphore(&sem_SPIA);

                    // Draw new position
                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawRectangle(mario_x, mario_y, mario_w, mario_h, ST7789_RED);
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                }
            }
        } else {
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
        //G8RTOS_WaitSemaphore(&sem_I2CA);
        buttons = MultimodButtons_Get();
        //G8RTOS_SignalSemaphore(&sem_I2CA);

        //button functionality depend on state
        if(currentState == START){
            if(buttons == SW1 || buttons == SW2 || buttons == SW3 || buttons == SW4){
                transition = 1;
                currentState = GAMEPLAY;
            }
        }

        else if(currentState == GAMEPLAY){
            //allow for jump somehow
            //if gameover -> transition = 1;
        }
        else if(currentState == GAMEOVER || currentState == LEVELWON){
            if(buttons == SW1 || buttons == SW2 || buttons == SW3 || buttons == SW4){
                transition = 1;
                currentState = START;
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
    //UARTprintf("IN GET JOYSTICK");
    if(currentState == GAMEPLAY){
        uint32_t xy = JOYSTICK_GetXY();

        // Send through FIFO.
        G8RTOS_WriteFIFO(JOYSTICK_FIFO, xy);
    }
}

void Draw_Screen(void){
    // Draw the screen
    //UARTprintf("IN DRAW SCREEN");
    if(transition == 1){
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_Fill(ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);
        transition = 0;

        if(currentState == GAMEPLAY){
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(mario_x, mario_y, mario_w, mario_h, ST7789_RED);
            G8RTOS_SignalSemaphore(&sem_SPIA);
        }

    }
    if(currentState == START){
        //reset necessary variables
        win = 0;
        lives = 3;
        score = 0;
        //reset mario
        mario_x = 40;
        mario_y = 20;
        //reset barrels
        clearBarrels();
        initBarrels();
        //!!!!!!!!!!!! NEED SOME WAY TO CLEAR FIFO !!!!!!!!!!

        //draw start screen
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
    else if(currentState == GAMEPLAY){
        //draw gameplay screen, only static stuff?
        //draw score
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteString(10, 20, "SCORE ", ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteScore(7 * 10 + 10, 20, score, ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        //draw lives
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteString(10, 32, "LIVES ", ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteScore(7 * 10 + 10, 32, lives, ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        //draw level
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteString(10, 44, "LEVEL: ", ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteScore(7 * 10 + 10, 44, level, ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        //draw bonus score !!!!!!!!!!! LATER FEATURE !!!!!!!!!!!!!

        //draw platforms
        drawAllPlatforms();
        //draw ladders
        drawAllLadders();

        //draw princess
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawRectangle(princess_x, princess_y, princess_w, princess_h, ST7789_PINK);
        G8RTOS_SignalSemaphore(&sem_SPIA);
        //draw dk
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawRectangle(dk_x, dk_y, dk_w, dk_h, ST7789_BROWN);
        G8RTOS_SignalSemaphore(&sem_SPIA);
        //draw hazards, rn barrels other stuff later
        drawBarrels();

        //draw bonus items !!!!!!!!!!! LATER FEATURE !!!!!!!!!!!!!
    }
    else if (currentState == LEVELWON){
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteString(X_MAX/2 - 25, string_y, "LEVEL WON!", ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteString(X_MAX/2 - 50, string_y + 22, "FINAL SCORE ", ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteScore(X_MAX/2 - 50 + (10 * 10), string_y + 22, score, ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteString(X_MAX/2 - 90, string_y +44, "PRESS ANY SW TO RESTART", ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);
    }
    else if(currentState == GAMEOVER){
        //draw gameover screen
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteString(X_MAX/2 - 35, string_y, "GAME OVER", ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteString(X_MAX/2 - 50, string_y + 22, "FINAL SCORE ", ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteScore(X_MAX/2 - 50 + (10 * 10), string_y + 22, score, ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_WriteString(X_MAX/2 - 90, string_y +44, "PRESS ANY SW TO RESTART", ST7789_WHITE);
        G8RTOS_SignalSemaphore(&sem_SPIA);
    }
}

void Generate_Barrel(void) {
    //UARTprintf("IN GENERATE BARRREL\n");
    if(currentState == GAMEPLAY){
        if(active_barrel_count < MAX_BARRELS && currentState == GAMEPLAY) {
            for(int i = 0; i < MAX_BARRELS; i++) {
                if(barrels[i].active == 0) {
                    //UARTprintf("GENERATING BARRREL: %d\n", i);
                    barrels[i].x = dk_x + dk_w + 1;
                    barrels[i].y = dk_y;
                    barrels[i].active = 1;
                    barrels[i].platform_index = 6;  // DK's platform
                    barrels[i].falling = false;
                    barrels[i].dir = 1;  // Start moving Right
                    barrels[i].err = platforms[barrels[i].platform_index].width / 2;
                    active_barrel_count++;
                    break;
                }
            }
        }
    }
}


/*******************************Aperiodic Threads***********************************/

void GPIOE_Handler() {
    // Disable interrupt
    GPIOIntDisable(GPIO_PORTE_BASE, GPIO_PIN_4);

    // Signal relevant semaphore
    G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);

}
