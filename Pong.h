#ifndef _PONG_H
#define _PONG_H
#include <string>
#include <chrono>
#include <SDL.h>
#include <SDL_ttf.h>
#include <time.h>
#include <cstdlib>
#include <iostream>
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const int BALL_WIDTH = 15;
const int BALL_HEIGHT = 15;

const int PADDLE_WIDTH = 15;
const int PADDLE_HEIGHT = 15 * 5;

const float PADDLE_SPEED = 0.50f;
const float BALL_SPEED = 0.55f;

const float X_VEL_HI = 1.0;
const float X_VEL_LO = 0.5;
const float Y_VEL_HI = 0.75;
const float Y_VEL_LO = 0.25;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* typeFont = NULL;

bool init();
void destroy();
void DrawNet();
float Randomizer(float HI, float LO);
#endif


