#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "Crack/Crack.h"
#include "Lib/Ctor.h"
#include "Lib/GameConfig.h"
#include "Lib/MainStruct.h"

void GateArrayResize(GateArray_t *gates)
{
    if (gates->size == gates->capacity)
    {
        gates->array    = (Gate*)realloc(gates->array, sizeof(Gate)*(gates->capacity*2 + 1));
        gates->capacity = gates->capacity*2 + 1;
    }
}

void CoastTileArrayResize(CoastTileArray_t *coast)
{
    if (coast->size == coast->capacity)
    {
        coast->array    = (CoastTile*)realloc(coast->array, sizeof(CoastTile)*(coast->capacity*2 + 1));
        coast->capacity = coast->capacity*2 + 1;
    }
}

void RiverTileArrayResize(RiverTileArray_t *river)
{
    if (river->size == river->capacity)
    {
        river->array    = (RiverTile*)realloc(river->array, sizeof(RiverTile)*(river->capacity*2 + 1));
        river->capacity = river->capacity*2 + 1;
    }
}

int CoastTileCtor(CoastTile *tile, float x, float y, CoastTileType tile_type, bool up_coast)
{
    CTOR(CoastTile, tile);      

    tile->type = tile_type;

    tile->texture = sf::Texture();

    if (tile_type == GRASS_TYPE)
    {
        if (!tile->texture.loadFromFile("Sprites/Grass.png", sf::IntRect(0, 0, 320, 64)))
        {
            printf("error during coast texture loading\n");
            return -1;
        }
    }
    else if (tile_type == COAST_TYPE)
    {
        if (!tile->texture.loadFromFile("Sprites/Coast.png", sf::IntRect(0, 0, 320, 64)))
        {
            printf("error during coast texture loading\n");
            return -1;
        }
    }
    else
    {
        printf("unnown tile type\n");
        return -1;
    }
    
    tile->texture.setSmooth(false);                                    //off the smooth  

    tile->sprite.setTexture(tile->texture, true);

    if (up_coast)
    {
        tile->sprite.setRotation(90);
        tile->x = x + SPRITE_SIZE*COAST_TILE_SCALE;
        tile->y = y - SPRITE_SIZE;
    }
    else
    {
        tile->sprite.setRotation(-90);
        tile->x = x - SPRITE_SIZE*COAST_TILE_SCALE;
        tile->y = y + SPRITE_SIZE*COAST_TILE_SCALE*1.5;
    }

    tile->sprite.setPosition(tile->x, tile->y);
    tile->sprite.setTextureRect(sf::IntRect(0, 0, SPRITE_SIZE, SPRITE_SIZE));
    tile->sprite.setScale(RIVER_TILE_SCALE, RIVER_TILE_SCALE);
    
    return 0;
}

int CharacterCtor(Character* character)
{
    CTOR(Character, character);

    if (!character->texture.loadFromFile("Sprites/Kayak.png", sf::IntRect(0, 0, 64, 64)))
    {
        printf("error during character texture loading\n");
        return -1;
    }
    character->texture.setSmooth(false);                                    //off the smooth    
    
    character->sprite.setTexture(character->texture, true);     
    character->sprite.setScale(2.5, 2.5);
    character->sprite.setPosition(START_X + CHARACT_SIZE*2, START_Y );
    character->sprite.rotate(90);

    character->x      = START_X;
    character->y      = START_Y;
    character->radius = CHARACT_SIZE;
    
    return 0;
}

void CoastCtor(Coast* coast, float y)
{
    CTOR(Coast, coast);

    coast->figure.setSize(COAST_SIZE);
    coast->figure.setFillColor(sf::Color(170, 170, 85));
    coast->y = y;
    coast->figure.setPosition(0, y);
}

int RiverTileCtor(RiverTile* river, float x, float y)
{
    CTOR(RiverTile, river);

    river->x = x + SPRITE_SIZE*RIVER_TILE_SCALE;
    river->y = y;
    
    if (!river->texture.loadFromFile("Sprites/Water.png", sf::IntRect(0, 0, 320, 64)))
    {
        printf("error during water texture loading\n");
        return -1;
    }
    river->texture.setSmooth(false);                                    //off the smooth   
    river->sprite.setTexture(river->texture, true);

    river->sprite.setRotation(90);
    river->sprite.setPosition(river->x, river->y);
    river->sprite.setTextureRect(sf::IntRect(0, 0, SPRITE_SIZE, SPRITE_SIZE));
    river->sprite.setScale(RIVER_TILE_SCALE, RIVER_TILE_SCALE);
    
    return 0;
}

void GateCtor(Gate* gate, float x, float y, GATE_TYPE gate_type)
{
    CTOR(Gate, gate);

    gate->coord.x = x;
    gate->coord.y = y;
    gate->status  = true;

    gate->bottom_mp.setRadius(20);
    gate->bottom_mp.setPosition(x, y);

    gate->up_mp.setRadius(20);
    gate->up_mp.setPosition(x, y + CHARACT_SIZE*2);

    gate->gate_type = gate_type;
    sf::Color gate_color = sf::Color(0, 0, 0);
    if (gate->gate_type == GREEN_GATE)
        gate_color = sf::Color(20, 220, 50);        //Green color
    else if (gate->gate_type == RED_GATE)
        gate_color = sf::Color::Red;

    gate->bottom_mp.setFillColor(gate_color);
    gate->up_mp.setFillColor(gate_color);
}

int ScoreCtor(Score* score, int points)
{
    CTOR(Score, score);
    
    score->points = points;
    score->text.setFillColor(sf::Color::Red);
    score->text.setCharacterSize(60);

    if (!score->font.loadFromFile("Font.ttf"))
    {
        printf("error during font loading\n");
        return -1;
    }
    score->text.setFont(score->font);

    char text[MAX_SCORE_TEXT_LENGTH] = "";
    sprintf(text, SCORE_STRING_FORMAT, points);
    score->text.setString(text);

    return 0;
}

void UpdateScore(Score* score)
{
    char text[MAX_SCORE_TEXT_LENGTH] = "";
    sprintf(text, SCORE_STRING_FORMAT, score->points);
    score->text.setString(text);
}

void CharMoveUp(Character* character)
{
    if (character->y > UP_COAST_Y + COAST_SIZE.y + CHARACT_STEP)
    {
        character->y -= CHARACT_STEP;
        character->sprite.move(0, -CHARACT_STEP);
    }
}

void CharMoveDown(Character* character)
{
    if (character->y < BOTTOM_COAST_Y - COAST_SIZE.y - CHARACT_STEP)
    {
        character->y += CHARACT_STEP;
        character->sprite.move(0, CHARACT_STEP);
    }
}

void FillCoast(CoastTileArray_t* up_coast, CoastTileArray_t* bottom_coast)
{
    float x_length = COAST_SIZE.x / (SPRITE_SIZE*COAST_TILE_SCALE);
    
    for (int i = 0; i < x_length; i++)
    {
        float x = i*COAST_TILE_SCALE*SPRITE_SIZE;

        CoastTileArrayResize(up_coast);
        
        CoastTileCtor(up_coast->array + up_coast->size, 
                      x, COAST_SIZE.y, 
                      COAST_TYPE, true);  
        up_coast->size++;

        CoastTileArrayResize(bottom_coast);

        CoastTileCtor(bottom_coast->array + bottom_coast->size, 
                      x, BOTTOM_COAST_Y - COAST_SIZE.y, 
                      COAST_TYPE, false);
        bottom_coast->size++;
    }
}

int FillRiver(RiverTileArray_t* river)
{
    int x_length = RIVER_SIZE.x / (SPRITE_SIZE*RIVER_TILE_SCALE);
    int y_length = RIVER_SIZE.y / (SPRITE_SIZE*RIVER_TILE_SCALE);
    for (int i = 0; i < x_length; i++)
    {
        for(int j = 0; j < y_length; j++)
        {   
            RiverTileArrayResize(river);

            float x = (float)i*RIVER_TILE_SCALE*SPRITE_SIZE;
            float y = UP_COAST_Y + COAST_SIZE.y + j*COAST_TILE_SCALE*SPRITE_SIZE;

            RiverTileCtor(&river->array[river->size], x, y);

            river->size++;
        }
    }

    return 0;
}

bool Charact_in_gate(Gate* gate, Character *character)
{
    return gate->coord.x <= CHARACT_SIZE &&
            character->y > gate->coord.y && character->y <= gate->coord.y + CHARACT_STEP;
}

void DrawGates(sf::RenderWindow* window, GateArray_t* gates, Character *character, Score *score)
{
    for(size_t i = 0; i < gates->size; i++)
    {
        gates->array[i].bottom_mp.move(-RIVER_SPEED, 0);
        gates->array[i].up_mp.move(-RIVER_SPEED, 0);
        gates->array[i].coord.x -= RIVER_SPEED;

        if (gates->array[i].coord.x < 0)
            gates->array[i].status = false;

        if (gates->array[i].status && Charact_in_gate(&gates->array[i], character))
        {
            if (gates->array[i].gate_type == GREEN_GATE)
                score->points += SCORING_POINTS;
            else if (gates->array[i].gate_type == RED_GATE)
                score->points -= SCORING_POINTS;

            gates->array[i].status = false;
            UpdateScore(score);
        }
        else if (gates->array[i].status)
        {
            window->draw(gates->array[i].bottom_mp);
            window->draw(gates->array[i].up_mp);
        }

        
    }
}

void CreateNewGate(GateArray_t* gates)
{
    if (rand()%GATE_SPAWN_RATE == 0)
    {
        GateArrayResize(gates);
        GATE_TYPE new_gate_type = UNDEF_GATE;
        if (rand()%3 == 0)
            new_gate_type = RED_GATE;
        else
            new_gate_type = GREEN_GATE;

        GateCtor(gates->array + gates->size, OUT_OF_WINDOW_X, CHARACT_STEP*(float)(rand()%3 + 1) - CHARACT_STEP/4, 
                 new_gate_type);
        gates->size++;
    }
}

void ChangeSpriteCoastAndRiver(RiverTileArray_t* river, CoastTileArray_t* up_coast, CoastTileArray_t* bottom_coast)
{
    for (size_t i = 0; i < river->size; i++)
        river->array[i].sprite.setTextureRect(sf::IntRect((rand() % 5)*SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE));
    for (size_t i = 0; i < up_coast->size; i++)
    {
        if (up_coast->array[i].type == COAST_TYPE)
            up_coast->array[i].sprite.setTextureRect(sf::IntRect((rand() % 3)*SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE));
    }
    for (size_t i = 0; i < bottom_coast->size; i++)
    {
        if (bottom_coast->array[i].type == COAST_TYPE)
            bottom_coast->array[i].sprite.setTextureRect(sf::IntRect((rand() % 3)*SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE));
    }
}

void MoveCoastAndRiver(RiverTileArray_t* river, CoastTileArray_t* up_coast, CoastTileArray_t* bottom_coast)
{
    for (size_t i = 0; i < river->size; i++)
    {
        river->array[i].x -= RIVER_SPEED;

        if (river->array[i].x < -SPRITE_SIZE*RIVER_TILE_SCALE)
            river->array[i].x = OUT_OF_WINDOW_X;

        river->array[i].sprite.setPosition(river->array[i].x, river->array[i].y);
    }
    for (size_t i = 0; i < up_coast->size; i++)
    {
        up_coast->array[i].x -= RIVER_SPEED;

        if (up_coast->array[i].x < -SPRITE_SIZE*COAST_TILE_SCALE)
            up_coast->array[i].x = OUT_OF_WINDOW_X;

        up_coast->array[i].sprite.setPosition(up_coast->array[i].x, up_coast->array[i].y);
    }
    for (size_t i = 0; i < bottom_coast->size; i++)
    {
        bottom_coast->array[i].x = bottom_coast->array[i].x - RIVER_SPEED;

        if (bottom_coast->array[i].x < -SPRITE_SIZE*COAST_TILE_SCALE)
            bottom_coast->array[i].x = OUT_OF_WINDOW_X;

        bottom_coast->array[i].sprite.setPosition(bottom_coast->array[i].x, bottom_coast->array[i].y);
    }
}

void DrawCoastAndRiver(sf::RenderWindow* window, RiverTileArray_t* river, 
                       CoastTileArray_t* up_coast, CoastTileArray_t* bottom_coast)
{
    for(size_t i = 0; i < river->size; i++)
        window->draw(river->array[i].sprite);
    
    for(size_t i = 0; i < up_coast->size; i++)
        window->draw(up_coast->array[i].sprite);
    
    for(size_t i = 0; i < bottom_coast->size; i++)
        window->draw(bottom_coast->array[i].sprite);
}

int PlayerWin(char* argv[], size_t crack_file_length, Label *main_label, 
              sf::RenderWindow* window, Character* character)
{
    main_label->text.setColor(sf::Color::Blue);
    main_label->text.setString("Cracking in process...");
    window->draw(main_label->text);
    window->draw(character->sprite);
    window->display();

    if (Crack(argv, crack_file_length) == -1)
        return -1;

    usleep(3000000);

    window->clear();
    main_label->text.setColor(sf::Color::Green);
    main_label->text.setString("Successful crack!");
    window->draw(main_label->text);
    window->display();
    usleep(3000000);

    window->close();
    return 0;
}

void FreeMem(GateArray_t* gates, RiverTileArray_t* river, CoastTileArray_t* up_coast, CoastTileArray_t* bottom_coast)
{
    for (size_t i = 0; i < gates->size; i++)
        gates->array[i].~Gate();

    for (size_t i = 0; i < river->size; i++)
        river->array[i].~RiverTile();
    
    for(size_t i = 0; i < up_coast->size; i++)
        up_coast->array[i].~CoastTile();

    for(size_t i = 0; i < up_coast->size; i++)
        bottom_coast->array[i].~CoastTile();

    free(gates->array); 
    free(river->array);
    free(up_coast->array);
    free(bottom_coast->array);
}

void MainLabelCtor(Label* main_label, Score* score)
{
    main_label->font = score->font;
    main_label->text.setPosition(window_weight/5, window_height*0.9);
    main_label->text.setCharacterSize(CHARACT_SIZE);
    main_label->text.setString("Score 2000 points to crack");
    main_label->text.setColor(sf::Color::Red);
    main_label->text.setFont(main_label->font);
}

int Game(int argc, char* argv[], size_t crack_file_length)
{
    sf::RenderWindow window(sf::VideoMode(window_weight, window_height), header, sf::Style::Fullscreen);

    sf::RectangleShape BackGround = sf::RectangleShape(sf::Vector2<float>(OUT_OF_WINDOW_X, OUT_OF_WINDOW_Y));
    BackGround.setFillColor(sf::Color(67, 148, 188));
    BackGround.setPosition(0, COAST_SIZE.y + UP_COAST_Y);
    BackGround.setSize(RIVER_SIZE);

    Character character = {};
    if (CharacterCtor(&character) != 0)
        return -1;

    CoastTileArray_t up_coast     = {};
    CoastTileArray_t bottom_coast = {};

    FillCoast(&up_coast, &bottom_coast);

    RiverTileArray_t river = {};
    FillRiver(&river);

    GateArray_t gates = {};

    Score score = {};
    if (ScoreCtor(&score, 0) != 0)
        return -1;

    Label main_label = {};
    MainLabelCtor(&main_label, &score);

    srand((unsigned int)time(NULL));
    int counter = 0;

    while (window.isOpen())
    {
        CreateNewGate(&gates);
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::KeyPressed:
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                        CharMoveUp(&character);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                        CharMoveDown(&character);
                    break;
                }
                case sf::Event::Closed:
                {
                    window.close();
                }
            }
        }


        counter++;
        if (counter % 500 == 0)
        {
            ChangeSpriteCoastAndRiver(&river, &up_coast, &bottom_coast);
            counter = 0;
        }

        MoveCoastAndRiver(&river, &up_coast, &bottom_coast);

        window.clear();
        window.draw(BackGround);

        DrawCoastAndRiver(&window, &river, &up_coast, &bottom_coast);

        DrawGates(&window, &gates, &character, &score);
        
        if (score.points >= POINTS_TO_WIN)
        {
            int return_code = PlayerWin(argv, crack_file_length, &main_label, &window, &character);
            FreeMem(&gates, &river, &up_coast, &bottom_coast);
            return return_code;
        }

        window.draw(character.sprite);
        window.draw(score.text);
        window.draw(main_label.text);

        window.display();
    }
    FreeMem(&gates, &river, &up_coast, &bottom_coast);

    return 0;
}

int main(int argc, char* argv[])
{
    size_t crack_file_length = 0;
    if (CheckCrackFile(argc, argv, &crack_file_length) != 0)
        return -1;
    
    printf("main_length = %zu\n", crack_file_length);
    if (Game(argc, argv, crack_file_length) == -1)
        return -1;

    return 0;
}