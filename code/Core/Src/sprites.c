/*
 * sprites.c
 *
 *  Created on: 3 maj 2024
 *      Author: Dominik
 */

#include "sprites.h"
#include "ILI9488.h"

uint8_t trigRisingIcon[15][9] = {
    {0     , 0     , 0     , 0     , WHITE , WHITE , WHITE , WHITE , WHITE},
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0     },
    {0     , 0     , WHITE , WHITE , WHITE , WHITE , WHITE , 0     , 0     },
    {0     , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     },
    {WHITE , WHITE , WHITE , WHITE , WHITE , 0     , 0     , 0     , 0     }
};

uint8_t arrowUpDown[15][8] = {
    {0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , WHITE , WHITE , WHITE , WHITE , WHITE , 0     , 0},
    {WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , 0},
	{0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , 0},
    {0     , WHITE , WHITE , WHITE , WHITE , WHITE , 0     , 0},
    {0     , 0     , WHITE , WHITE , WHITE , 0     , 0     , 0},
    {0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0}
};

uint8_t arrowLeftRight[7][15] = {
    {0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0},
    {0     , 0     , WHITE , WHITE , 0     , 0     , 0     , 0     , 0     , 0     , 0     , WHITE , WHITE , 0     , 0},
    {0     , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , 0},
    {WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE},
    {0     , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , WHITE , 0},
    {0     , 0     , WHITE , WHITE , 0     , 0     , 0     , 0     , 0     , 0     , 0     , WHITE , WHITE , 0     , 0},
    {0     , 0     , 0     , WHITE , 0     , 0     , 0     , 0     , 0     , 0     , 0     , WHITE , 0     , 0     , 0}
};
