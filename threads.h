// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef THREADS_H_
#define THREADS_H_

/************************************Includes***************************************/

#include <G8RTOS/G8RTOS.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/
#define JOYSTICK_FIFO       0


typedef enum GameState {
   START,
   GAMEPLAY,
   LEVELWON,
   GAMEOVER
} GameState;

typedef enum {
   FLAT,
   SLOPE_RIGHT_DOWN,
   SLOPE_LEFT_DOWN
} PlatformType;

typedef struct {
   uint16_t x;         // starting x position
   uint16_t y;         // starting y position
   uint16_t width;     // width of platform
   uint16_t height;    // height/thickness of platform
   uint16_t slope;     // slope percentage (0 for flat)
   PlatformType type;  // type of platform
   uint16_t color;     // color of platform
} Platform;

typedef struct {
   uint16_t x;          // x position
   uint16_t y_bottom;   // bottom y position
   uint16_t y_top;      // top y position
   uint16_t width;      // width of ladder
   uint16_t rung_space; // space between rungs
   bool is_broken;      // whether ladder is broken (can't be climbed)
   uint16_t color;      // color of ladder
} Ladder;

typedef struct {
    int16_t x;
    int16_t y;
    int8_t dir;
    bool active;  // whether barrel is currently in play
    bool falling;
    uint8_t platform_index;  // which platform the barrel is currently on
    int16_t err;
} Barrel;

/*************************************Defines***************************************/

/***********************************Semaphores**************************************/

semaphore_t sem_I2CA;
semaphore_t sem_SPIA;
semaphore_t sem_PCA9555_Debounce;
//semaphore_t sem_Joystick_Debounce;
//semaphore_t sem_KillCube;

/***********************************Semaphores**************************************/

/***********************************Structures**************************************/
/***********************************Structures**************************************/


/*******************************Background Threads**********************************/

void Idle_Thread(void);
void MarioMove_Thread(void);
void Read_Buttons(void);

/*******************************Background Threads**********************************/

/********************************Periodic Threads***********************************/

void Get_Joystick(void);
void Draw_Screen(void);
void Generate_Barrel(void);

/********************************Periodic Threads***********************************/

/*******************************Aperiodic Threads***********************************/

void GPIOE_Handler(void);

/*******************************Aperiodic Threads***********************************/


#endif /* THREADS_H_ */

