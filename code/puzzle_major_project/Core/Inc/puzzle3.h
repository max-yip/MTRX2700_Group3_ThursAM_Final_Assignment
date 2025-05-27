// puzzle3.h
#ifndef PUZZLE3_H
#define PUZZLE3_H

#include <stdint.h>

// Run the “find the 3,3” puzzle: polls the two flex-pots every 2 s,
// gives closer/further feedback, rotates the servo on (3,3), then returns.
void Puzzle3_Run(void);

#endif // PUZZLE3_H
