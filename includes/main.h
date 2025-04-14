#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL.h>
#include <SDL3_gfxPrimitives.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <portmidi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <smf.h>
#include <math.h>
#include <time.h>

#define SCREEN_WIDTH_LARGE 1920
#define SCREEN_HEIGHT_LARGE 1080
#define SCREEN_WIDTH_SMALL 800
#define SCREEN_HEIGHT_SMALL 600

#define WHITE_KEY_WIDTH 34
#define BLACK_KEY_WIDTH 18
#define WHITE_KEY_HEIGHT 230
#define BLACK_KEY_HEIGHT 145

#define MIN_NOTE_HEIGHT 20
#define NOTE_COUNT 127
#define MAX_NOTES 512

#define MAX_MIDI_EVENTS 256
#define MIDI_NOTE_MIN 21
#define MIDI_NOTE_MAX 108

#define RADIUS 4
#define TEXT_SIZE 80
#define TEXT_COLOR (SDL_Color){255, 255, 255, 255}

#endif