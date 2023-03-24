#ifndef SYM_GAME_CONFIG_CRACK
#define SYM_GAME_CONFIG_CRACK

#include <SFML/Graphics.hpp>

typedef sf::CircleShape CharacterShape;

//=====================WINDOW=========================
const int    window_weight   = 1000;
const int    window_height   = 1000;
const size_t OUT_OF_WINDOW_X = 2500;
const size_t OUT_OF_WINDOW_Y = 1500;
const char   header[]        = "SLALOM IS MY LIFE!!! SLAAAALOOOOM!!";

//=====================SCORE==========================
const size_t MAX_SCORE_TEXT_LENGTH = 100;
const char   SCORE_STRING_FORMAT[] = "Points  %d";
const int    POINTS_TO_WIN         = 2000;

//=====================SPRITES==========================
const int SPRITE_SIZE = 64;  

//=====================CHARACTER==========================
const float CHARACT_SIZE = 100;
const float CHARACT_STEP = CHARACT_SIZE*2;
const float START_X      = 0;
const float START_Y      = 400;

//=====================COAST==========================
const float        UP_COAST_Y     = 0;
const float        BOTTOM_COAST_Y = UP_COAST_Y + CHARACT_STEP*3 + CHARACT_SIZE*2 + CHARACT_SIZE/2;
sf::Vector2<float> COAST_SIZE     = {OUT_OF_WINDOW_X*2, 150};
const float        COAST_TILE_SCALE    = 2;

enum CoastTileType
{
    NOT_DEF_TYPE = 0,
    GRASS_TYPE   = 1,
    COAST_TYPE   = 2
};

//=====================RIVER==========================
const float        RIVER_SPEED      = 0.4f;
const float        RIVER_TILE_SCALE = 2;
sf::Vector2<float> RIVER_SIZE       = {OUT_OF_WINDOW_X*2, BOTTOM_COAST_Y - UP_COAST_Y - COAST_SIZE.y};

//=====================GATE==========================
const int GATE_SPAWN_RATE = 1000 / RIVER_SPEED;
const int SCORING_POINTS  = 100;

#endif