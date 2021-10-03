#ifndef RANDOM_H
#define RANDOM_H

/**
 * Generate random number
 * @param min is minimal value that can be returned
 * @param max is maximum value that can be returned
 * @return value between min and max
 */
int randomNumber(int min, int max);

/**
 * Generate random number
 * @param max is maximum value that can be returned
 * @return value between 0 and max
 */
int randomNumber(int max);

/**
 * Generate random number
 * @return value between 0 and 100
 */
int randomNumber();

#endif