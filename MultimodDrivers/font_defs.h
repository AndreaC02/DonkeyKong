// font_defs.h
#ifndef FONT_DEFS_H_
#define FONT_DEFS_H_

#include <stdint.h>

typedef struct {
   uint8_t num_pixels;
   uint8_t pixels[30][2];
} CharacterDef;

extern const CharacterDef CHAR_A;
extern const CharacterDef CHAR_B;
extern const CharacterDef CHAR_C;
extern const CharacterDef CHAR_D;
extern const CharacterDef CHAR_E;
extern const CharacterDef CHAR_F;
extern const CharacterDef CHAR_G;
extern const CharacterDef CHAR_H;
extern const CharacterDef CHAR_I;
extern const CharacterDef CHAR_J;
extern const CharacterDef CHAR_K;
extern const CharacterDef CHAR_L;
extern const CharacterDef CHAR_M;
extern const CharacterDef CHAR_N;
extern const CharacterDef CHAR_O;
extern const CharacterDef CHAR_P;
extern const CharacterDef CHAR_Q;
extern const CharacterDef CHAR_R;
extern const CharacterDef CHAR_S;
extern const CharacterDef CHAR_T;
extern const CharacterDef CHAR_U;
extern const CharacterDef CHAR_V;
extern const CharacterDef CHAR_W;
extern const CharacterDef CHAR_X;
extern const CharacterDef CHAR_Y;
extern const CharacterDef CHAR_Z;
extern const CharacterDef CHAR_SPACE;
extern const CharacterDef CHAR_EXCLAIM;

// Helper function declarations
const CharacterDef* GetCharacterDef(char c);

const CharacterDef CHAR_A = {
   16,
   {
       {2,0}, {3,0},                       // Top
       {1,1}, {4,1},                       // Upper sides
       {0,2}, {5,2},                       // Sides
       {0,3}, {5,3},                       // Sides
       {0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4},  // Middle
       {0,5}, {5,5},                       // Lower sides
       {0,6}, {5,6},                       // Lower sides
       {0,7}, {5,7},                       // Bottom sides
       {0,8}, {5,8}                        // Bottom
   }
};

const CharacterDef CHAR_B = {
    20,
    {
        {0,0}, {1,0}, {2,0}, {3,0}, {4,0},          // Top
        {0,1}, {5,1}, {0,2}, {5,2}, {0,3}, {5,3},   // Sides top
        {0,4}, {1,4}, {2,4}, {3,4}, {4,4},          // Middle
        {0,5}, {5,5}, {0,6}, {5,6}, {0,7}, {5,7},   // Sides bottom
        {0,8}, {1,8}, {2,8}, {3,8}, {4,8}           // Bottom
    }
};

const CharacterDef CHAR_C = {
    16,
    {
        {1,0}, {2,0}, {3,0}, {4,0},                 // Top
        {0,1}, {5,1}, {0,2}, {0,3}, {0,4},         // Left side
        {0,5}, {0,6}, {0,7},                        // Left side continue
        {1,8}, {2,8}, {3,8}, {4,8}                 // Bottom
    }
};

const CharacterDef CHAR_D = {
    19,
    {
        {0,0}, {1,0}, {2,0}, {3,0}, {4,0},         // Top
        {0,1}, {5,1}, {0,2}, {5,2},                // Upper sides
        {0,3}, {5,3}, {0,4}, {5,4},                // Middle sides
        {0,5}, {5,5}, {0,6}, {5,6},                // Lower sides
        {0,7}, {5,7},                              // Bottom sides
        {0,8}, {1,8}, {2,8}, {3,8}, {4,8}         // Bottom
    }
};

const CharacterDef CHAR_E = {
    19,
    {
        {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},  // Top
        {0,1}, {0,2}, {0,3},                        // Left upper
        {0,4}, {1,4}, {2,4}, {3,4}, {4,4},         // Middle
        {0,5}, {0,6}, {0,7},                        // Left lower
        {0,8}, {1,8}, {2,8}, {3,8}, {4,8}, {5,8}   // Bottom
    }
};

const CharacterDef CHAR_F = {
    15,
    {
        {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},  // Top
        {0,1}, {0,2}, {0,3},                        // Left upper
        {0,4}, {1,4}, {2,4}, {3,4}, {4,4},         // Middle
        {0,5}, {0,6}, {0,7}, {0,8}                 // Left lower
    }
};

const CharacterDef CHAR_G = {
    20,
    {
        {1,0}, {2,0}, {3,0}, {4,0},                // Top
        {0,1}, {5,1}, {0,2}, {0,3},                // Upper sides
        {0,4}, {3,4}, {4,4}, {5,4},                // Middle
        {0,5}, {5,5}, {0,6}, {5,6},                // Lower sides
        {1,7}, {2,7}, {3,7}, {4,7}, {5,7}         // Bottom curve
    }
};

const CharacterDef CHAR_H = {
    19,
    {
        {0,0}, {5,0},                              // Top
        {0,1}, {5,1}, {0,2}, {5,2},                // Upper sides
        {0,3}, {5,3},                              // Middle sides
        {0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4},  // Middle
        {0,5}, {5,5}, {0,6}, {5,6},                // Lower sides
        {0,7}, {5,7}, {0,8}, {5,8}                 // Bottom
    }
};

const CharacterDef CHAR_I = {
    15,
    {
        {0,0}, {1,0}, {2,0}, {3,0}, {4,0},         // Top
        {2,1}, {2,2}, {2,3},                        // Upper middle
        {2,4}, {2,5}, {2,6},                        // Lower middle
        {0,8}, {1,8}, {2,8}, {3,8}, {4,8}          // Bottom
    }
};

const CharacterDef CHAR_J = {
    14,
    {
        {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},  // Top
        {3,1}, {3,2}, {3,3}, {3,4},                // Vertical
        {3,5}, {3,6},                              // Continue vertical
        {0,7}, {1,7}, {2,7}                        // Bottom curve
    }
};

const CharacterDef CHAR_K = {
    15,
    {
        {0,0}, {5,0},                              // Top
        {0,1}, {4,1}, {0,2}, {3,2},                // Upper
        {0,3}, {2,3}, {0,4}, {1,4},                // Middle upper
        {0,5}, {2,5}, {0,6}, {3,6},                // Middle lower
        {0,7}, {4,7}, {0,8}, {5,8}                 // Bottom
    }
};

const CharacterDef CHAR_L = {
    12,
    {
        {0,0},                                      // Top
        {0,1}, {0,2}, {0,3},                        // Upper vertical
        {0,4}, {0,5}, {0,6},                        // Lower vertical
        {0,8}, {1,8}, {2,8}, {3,8}, {4,8}          // Bottom
    }
};

const CharacterDef CHAR_M = {
    22,
    {
        {0,0}, {5,0},                              // Top edges
        {0,1}, {1,1}, {4,1}, {5,1},                // Top inner
        {0,2}, {2,2}, {3,2}, {5,2},                // Upper middle
        {0,3}, {2,3}, {3,3}, {5,3},                // Middle
        {0,4}, {5,4}, {0,5}, {5,5},                // Lower middle
        {0,6}, {5,6}, {0,7}, {5,7},                // Bottom middle
        {0,8}, {5,8}                               // Bottom
    }
};

const CharacterDef CHAR_N = {
    18,
    {
        {0,0}, {5,0},                              // Top
        {0,1}, {1,1}, {5,1},                       // Upper
        {0,2}, {2,2}, {5,2},                       // Upper middle
        {0,3}, {3,3}, {5,3},                       // Middle
        {0,4}, {4,4}, {5,4},                       // Lower middle
        {0,5}, {5,5}, {0,6}, {5,6},                // Lower
        {0,7}, {5,7}, {0,8}, {5,8}                 // Bottom
    }
};

const CharacterDef CHAR_O = {
    20,
    {
        {1,0}, {2,0}, {3,0}, {4,0},                // Top
        {0,1}, {5,1}, {0,2}, {5,2},                // Upper sides
        {0,3}, {5,3}, {0,4}, {5,4},                // Middle
        {0,5}, {5,5}, {0,6}, {5,6},                // Lower sides
        {1,7}, {2,7}, {3,7}, {4,7}                 // Bottom
    }
};

const CharacterDef CHAR_P = {
    17,
    {
        {0,0}, {1,0}, {2,0}, {3,0}, {4,0},         // Top
        {0,1}, {5,1}, {0,2}, {5,2},                // Upper sides
        {0,3}, {5,3},                              // Middle sides
        {0,4}, {1,4}, {2,4}, {3,4}, {4,4},         // Middle
        {0,5}, {0,6}, {0,7}, {0,8}                 // Bottom vertical
    }
};

const CharacterDef CHAR_Q = {
    22,
    {
        {1,0}, {2,0}, {3,0}, {4,0},                // Top
        {0,1}, {5,1}, {0,2}, {5,2},                // Upper sides
        {0,3}, {5,3}, {0,4}, {5,4},                // Middle sides
        {0,5}, {5,5}, {0,6}, {4,6}, {5,6},         // Lower sides
        {1,7}, {2,7}, {3,7}, {4,7}, {5,8}          // Bottom + tail
    }
};

const CharacterDef CHAR_R = {
    20,
    {
        {0,0}, {1,0}, {2,0}, {3,0}, {4,0},         // Top
        {0,1}, {5,1}, {0,2}, {5,2},                // Upper sides
        {0,3}, {5,3},                              // Middle sides
        {0,4}, {1,4}, {2,4}, {3,4}, {4,4},         // Middle
        {0,5}, {3,5}, {0,6}, {4,6},                // Lower diagonal
        {0,7}, {5,7}, {0,8}, {5,8}                 // Bottom
    }
};

const CharacterDef CHAR_S = {
    16,
    {
        {1,0}, {2,0}, {3,0}, {4,0},                // Top line
        {0,1}, {0,2},                              // Top left
        {1,4}, {2,4}, {3,4}, {4,4},                // Middle line
        {5,5}, {5,6},                              // Bottom right
        {1,8}, {2,8}, {3,8}, {4,8}                 // Bottom line
    }
};

const CharacterDef CHAR_T = {
    11,
    {
        {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},  // Top line
        {2,1}, {2,2}, {2,3}, {2,4}, {2,5},         // Vertical line
        {2,6}, {2,7}, {2,8}, {2,9}                 // Extended vertical
    }
};

const CharacterDef CHAR_U = {
    18,
    {
        {0,0}, {5,0},                              // Top
        {0,1}, {5,1}, {0,2}, {5,2},                // Upper sides
        {0,3}, {5,3}, {0,4}, {5,4},                // Middle
        {0,5}, {5,5}, {0,6}, {5,6},                // Lower sides
        {1,7}, {2,7}, {3,7}, {4,7}                 // Bottom curve
    }
};

const CharacterDef CHAR_V = {
    15,
    {
        {0,0}, {5,0},                              // Top
        {0,1}, {5,1}, {0,2}, {5,2},                // Upper
        {1,3}, {4,3}, {1,4}, {4,4},                // Middle
        {2,5}, {3,5}, {2,6}, {3,6},                // Lower
        {2,7}, {3,7}                               // Bottom point
    }
};

const CharacterDef CHAR_W = {
    22,
    {
        {0,0}, {5,0},                              // Top edges
        {0,1}, {5,1}, {0,2}, {5,2},                // Upper outer
        {0,3}, {2,3}, {3,3}, {5,3},                // Upper inner
        {0,4}, {2,4}, {3,4}, {5,4},                // Middle
        {0,5}, {2,5}, {3,5}, {5,5},                // Lower inner
        {1,6}, {4,6}, {1,7}, {4,7}                 // Bottom points
    }
};

const CharacterDef CHAR_X = {
    14,
    {
        {0,0}, {5,0}, {1,1}, {4,1},                // Top
        {2,2}, {3,2}, {2,3}, {3,3},                // Upper middle
        {2,4}, {3,4},                              // Center
        {1,5}, {4,5}, {1,6}, {4,6},                // Lower middle
        {0,7}, {5,7}, {0,8}, {5,8}                 // Bottom
    }
};

const CharacterDef CHAR_Y = {
    14,
    {
        {0,0}, {5,0}, {0,1}, {5,1},                // Top
        {1,2}, {4,2}, {2,3}, {3,3},                // Upper
        {2,4}, {2,5}, {2,6},                       // Middle stem
        {2,7}, {2,8}                               // Bottom stem
    }
};

const CharacterDef CHAR_Z = {
    15,
    {
        {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},  // Top
        {4,1}, {3,2},                              // Upper diagonal
        {2,3}, {2,4},                              // Middle
        {1,5}, {1,6},                              // Lower diagonal
        {0,8}, {1,8}, {2,8}, {3,8}, {4,8}, {5,8}   // Bottom
    }
};

// Digits 0-9
const CharacterDef CHAR_0 = {
   18,
   {
       {1,0}, {2,0}, {3,0}, {4,0},                // Top curve
       {0,1}, {5,1}, {0,2}, {5,2},                // Upper sides
       {0,3}, {4,3}, {0,4}, {3,4},                // Middle with diagonal
       {0,5}, {2,5}, {0,6}, {1,6}, {5,6},         // Lower with diagonal
       {1,7}, {2,7}, {3,7}, {4,7}                 // Bottom curve
   }
};

const CharacterDef CHAR_1 = {
   12,
   {
       {2,0}, {3,0},                              // Top
       {1,1}, {2,1}, {3,1},                       // Upper
       {2,2}, {3,2}, {2,3}, {3,3},                // Upper middle
       {2,4}, {3,4}, {2,5}, {3,5},                // Lower middle
       {2,6}, {3,6}, {2,7}, {3,7},                // Bottom
       {1,8}, {2,8}, {3,8}, {4,8}, {5,8}          // Base
   }
};

const CharacterDef CHAR_2 = {
   17,
   {
       {1,0}, {2,0}, {3,0}, {4,0},                // Top curve
       {0,1}, {5,1},                              // Top sides
       {4,2}, {3,3},                              // Upper diagonal
       {2,4}, {1,5},                              // Middle diagonal
       {0,6},                                     // Bottom left
       {0,8}, {1,8}, {2,8}, {3,8}, {4,8}, {5,8}   // Bottom
   }
};

const CharacterDef CHAR_3 = {
   18,
   {
       {1,0}, {2,0}, {3,0}, {4,0},                // Top curve
       {0,1}, {5,1},                              // Top sides
       {4,2}, {3,3},                              // Upper right
       {1,4}, {2,4}, {3,4},                       // Middle
       {4,5}, {5,5}, {4,6}, {5,6},                // Lower right
       {1,7}, {2,7}, {3,7}, {4,7}                 // Bottom curve
   }
};

const CharacterDef CHAR_4 = {
   15,
   {
       {4,0}, {4,1},                              // Top right
       {3,1}, {4,1}, {3,2}, {4,2},                // Upper right
       {2,2}, {4,2}, {2,3}, {4,3},                // Middle right
       {1,3}, {4,3}, {1,4}, {4,4},                // Middle
       {0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4},  // Horizontal
       {4,5}, {4,6}, {4,7}, {4,8}                 // Bottom vertical
   }
};

const CharacterDef CHAR_5 = {
   18,
   {
       {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},  // Top
       {0,1}, {0,2},                              // Upper left
       {0,3}, {1,3}, {2,3}, {3,3}, {4,3},         // Middle
       {5,4}, {5,5},                              // Lower right
       {0,6}, {5,6},                              // Bottom sides
       {1,7}, {2,7}, {3,7}, {4,7}                 // Bottom curve
   }
};

const CharacterDef CHAR_6 = {
   19,
   {
       {1,0}, {2,0}, {3,0}, {4,0},                // Top curve
       {0,1}, {0,2}, {0,3},                       // Left side
       {1,3}, {2,3}, {3,3}, {4,3},                // Middle
       {0,4}, {5,4}, {0,5}, {5,5},                // Lower sides
       {0,6}, {5,6},                              // Bottom sides
       {1,7}, {2,7}, {3,7}, {4,7}                 // Bottom curve
   }
};

const CharacterDef CHAR_7 = {
   12,
   {
       {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},  // Top
       {5,1}, {4,2},                              // Upper right
       {3,3}, {2,4},                              // Middle
       {1,5}, {1,6},                              // Lower
       {1,7}, {1,8}                               // Bottom
   }
};

const CharacterDef CHAR_8 = {
   22,
   {
       {1,0}, {2,0}, {3,0}, {4,0},                // Top curve
       {0,1}, {5,1}, {0,2}, {5,2},                // Upper sides
       {1,3}, {2,3}, {3,3}, {4,3},                // Middle curve
       {0,4}, {5,4}, {0,5}, {5,5},                // Lower sides
       {0,6}, {5,6},                              // Bottom sides
       {1,7}, {2,7}, {3,7}, {4,7}                 // Bottom curve
   }
};

const CharacterDef CHAR_9 = {
   19,
   {
       {1,0}, {2,0}, {3,0}, {4,0},                // Top curve
       {0,1}, {5,1}, {0,2}, {5,2},                // Upper sides
       {1,3}, {2,3}, {3,3}, {4,3}, {5,3},         // Middle
       {5,4}, {5,5},                              // Lower right
       {5,6},                                     // Bottom right
       {1,7}, {2,7}, {3,7}, {4,7}                 // Bottom curve
   }
};

const CharacterDef CHAR_SPACE = {
   0,
   {{0,0}}  // Empty character
};

const CharacterDef CHAR_EXCLAIM = {
   8,
   {
       {0,0},
       {0,1},
       {0,2},
       {0,3},
       {0,4},
       {0,5},
       {0,6},
       {0,8}  // Bottom dot
   }
};

// Helper function to get character definition
const CharacterDef* GetCharacterDef(char c) {
   switch(c) {
       case 'A': return &CHAR_A;
       case 'B': return &CHAR_B;
       case 'C': return &CHAR_C;
       case 'D': return &CHAR_D;
       case 'E': return &CHAR_E;
       case 'F': return &CHAR_F;
       case 'G': return &CHAR_G;
       case 'H': return &CHAR_H;
       case 'I': return &CHAR_I;
       case 'J': return &CHAR_J;
       case 'K': return &CHAR_K;
       case 'L': return &CHAR_L;
       case 'M': return &CHAR_M;
       case 'N': return &CHAR_N;
       case 'O': return &CHAR_O;
       case 'P': return &CHAR_P;
       case 'Q': return &CHAR_Q;
       case 'R': return &CHAR_R;
       case 'S': return &CHAR_S;
       case 'T': return &CHAR_T;
       case 'U': return &CHAR_U;
       case 'V': return &CHAR_V;
       case 'W': return &CHAR_W;
       case 'X': return &CHAR_X;
       case 'Y': return &CHAR_Y;
       case 'Z': return &CHAR_Z;
       case '0': return &CHAR_0;
       case '1': return &CHAR_1;
       case '2': return &CHAR_2;
       case '3': return &CHAR_3;
       case '4': return &CHAR_4;
       case '5': return &CHAR_5;
       case '6': return &CHAR_6;
       case '7': return &CHAR_7;
       case '8': return &CHAR_8;
       case '9': return &CHAR_9;
       case ' ': return &CHAR_SPACE;
       case '!': return &CHAR_EXCLAIM;
       default: return &CHAR_SPACE;  // Return space for unknown characters
   }
}

#endif /* FONT_DEFS_H_ */

