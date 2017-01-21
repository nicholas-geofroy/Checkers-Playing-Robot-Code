#ifndef CONFIGURATION_H
#define CONFIGURATION_H

static const int SPEED = 1; //don't change unless you want to recallibrate the mm
static const double STEPS_PER_MM = 4.75;
static const int BAUD = 9600;
static const int TILE_WIDTH = 46; //width in mm
static const int DIAGONAL_MULTIPLIER = 0.69; //bot overshoots when moving diagonally.  This variable corrects it.

static const int SERVO_RAISE_ANGLE = 83;
static const int SERVO_LOWER_ANGLE = 35;

#endif
