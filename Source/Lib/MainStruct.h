#ifndef SYM_MAIN_STRUCT_CRACK
#define SYM_MAIN_STRUCT_CRACK

#include <SFML/Graphics.hpp>

#include "GameConfig.h"

//==================================STRUCTURES==================================

struct Character
{
    sf::Texture    texture = sf::Texture();
    sf::Sprite     sprite  = sf::Sprite();
    float          radius  = 0;
    float          x       = 0;
    float          y       = 0;
};

struct RiverTile
{
    sf::Sprite  sprite  = sf::Sprite();
    sf::Texture texture = sf::Texture();
    float       x       = 0;
    float       y       = 0;
};

struct Score
{
    int      points = 0;
    sf::Text text   = sf::Text();
    sf::Font font   = sf::Font();
};

struct Label
{
    sf::Text text = sf::Text();
    sf::Font font = sf::Font();
};

enum GATE_TYPE
{
    UNDEF_GATE = 0,
    GREEN_GATE = 1,
    RED_GATE   = 2,
};

struct Gate
{
    GATE_TYPE      gate_type = UNDEF_GATE;
    bool           status    = true;
    CharacterShape up_mp     = CharacterShape(1);
    CharacterShape bottom_mp = CharacterShape(1);
    sf::Vector2<float> coord = sf::Vector2<float>(0, 0);
};

struct Coast
{
    sf::RectangleShape figure = sf::RectangleShape();
    float              y      = 0;
};

enum CoastTileType
{
    NOT_DEF_TYPE = 0,
    GRASS_TYPE   = 1,
    COAST_TYPE   = 2
};

struct CoastTile
{
    sf::Texture   texture = sf::Texture();
    sf::Sprite    sprite  = sf::Sprite();
    CoastTileType type    = NOT_DEF_TYPE;
    float         x       = 0;
    float         y       = 0;
};

struct GateArray_t
{
    Gate* array     = nullptr;
    size_t size     = 0;
    size_t capacity = 0;
};

struct CoastTileArray_t
{
    CoastTile* array    = nullptr;
    size_t     size     = 0;
    size_t     capacity = 0;
};

struct RiverTileArray_t
{
    RiverTile* array    = nullptr;
    size_t     size     = 0;
    size_t     capacity = 0;
};


void GateArrayResize(GateArray_t *gates);

void CoastTileArrayResize(CoastTileArray_t *coast);

void RiverTileArrayResize(RiverTileArray_t *river);

int CoastTileCtor(CoastTile *tile, double x, double y, CoastTileType tile_type, bool up_coast);

int CharacterCtor(Character* character);

void CoastCtor(Coast* coast, float y);

int RiverTileCtor(RiverTile* river, float x, float y);

void GateCtor(Gate* gate, float x, float y, GATE_TYPE gate_type);

int ScoreCtor(Score* score, int points);

void UpdateScore(Score* score);

void CharMoveUp(Character* character);

void CharMoveDown(Character* character);

void FillCoast(CoastTileArray_t* up_coast, CoastTileArray_t* bottom_coast);

int FillRiver(RiverTileArray_t* river);

void DrawGates(sf::RenderWindow* window, GateArray_t* gates, Character *character, Score *score);

void CreateNewGate(GateArray_t* gates);

void ChangeSpriteCoastAndRiver(RiverTileArray_t* river, CoastTileArray_t* up_coast, CoastTileArray_t* bottom_coast);

void MoveCoastAndRiver(RiverTileArray_t* river, CoastTileArray_t* up_coast, CoastTileArray_t* bottom_coast);

void DrawCoastAndRiver(sf::RenderWindow* window, RiverTileArray_t* river, 
                       CoastTileArray_t* up_coast, CoastTileArray_t* bottom_coast);

int PlayerWin(char* argv[], size_t crack_file_length, Label *main_label, 
              sf::RenderWindow* window, Character* character);

void FreeMem(GateArray_t* gates, RiverTileArray_t* river, CoastTileArray_t* up_coast, CoastTileArray_t* bottom_coast);

int Game(int argc, char* argv[], size_t crack_file_length);

#endif