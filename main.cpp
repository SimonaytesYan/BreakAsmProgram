#include <SFML/Graphics.hpp>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int Crack(int argc, char* argv[], size_t crack_file_length);

#define gates_array(i) (*((Gate*)(gates->array + (i)*gates->elem_size)))

#define river_array(i) (*((RiverTile*)(river->array + (i)*river->elem_size)))

#define up_coast_array(i) (*((CoastTile*)(up_coast->array + (i)*up_coast->elem_size)))

#define bottom_coast_array(i) (*((CoastTile*)(bottom_coast->array + (i)*bottom_coast->elem_size)))

const size_t MAX_SCORE_TEXT_LENGTH = 100;
const char   SCORE_STRING_FORMAT[] = "Points  %d";

struct Vector
{
    void*  array     = nullptr;
    size_t elem_size = 0;
    size_t size      = 0;
    size_t capacity  = 0;
};

int VectorCtor(Vector* vector, size_t elem_size)
{
    vector->array     = calloc(1, elem_size);
    vector->capacity  = 1;
    vector->size      = 0;
    vector->elem_size = elem_size;

    if (vector->array == nullptr)
        return -1;
    return 0;
}

int PushBack(Vector* vector, void* elem)
{
    if (vector == nullptr)
        return -1;

    if (vector->size == vector->capacity)
    {
        vector->array    = realloc(vector->array, vector->elem_size*(vector->capacity*2 + 1));
        vector->capacity = vector->capacity*2 + 1;
    }
    
    memcpy(vector->array + vector->size*vector->elem_size, elem, vector->elem_size);
    vector->size++;
    return 0;
}

void PopBack(Vector* vector)
{
    if (vector->size > 0)
    {
        vector->size--;
    }
}

const unsigned long long right_hash_sum = 13624739409245231419ULL;

typedef sf::CircleShape CharacterShape;

const int  SPRITE_SIZE       = 64;
const int  POINTS_TO_WIN     = 2000;

const int    window_weight    = 1000;
const int    window_height    = 1000;
const size_t OUT_OF_WINDOW_X  = 2000;
const size_t OUT_OF_WINDOW_Y  = 1500;

const float character_size    = 100;
const float step              = character_size*2;
const float start_x           = 0;
const float start_y           = 400;

const float        up_coast_y     = 0;
const float        bottom_coast_y = up_coast_y + step*3 + character_size*2 + character_size/2;
sf::Vector2<float> coast_size     = {OUT_OF_WINDOW_X*2, 150};
const float        coast_scale    = 2;

float              RIVER_SPEED = 0.4f;  
sf::Vector2<float> river_size  = {OUT_OF_WINDOW_X*2, bottom_coast_y - up_coast_y - coast_size.y};
const float        river_scale = 2;

const char header[] = "SLALOM IS MY LIFE!!! SLAAAALOOOOM!!";

enum CoastTileType
{
    NOT_DEF_TYPE = 0,
    GRASS_TYPE   = 1,
    COAST_TYPE   = 2
};

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

struct Gate
{
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

struct CoastTile
{
    sf::Texture   texture = sf::Texture();
    sf::Sprite    sprite  = sf::Sprite();
    CoastTileType type    = NOT_DEF_TYPE;
    float       x         = 0;
    float       y         = 0;
};

int CoastTileCtor(CoastTile *tile, float x, float y, CoastTileType tile_type, bool up_coast)
{
    tile->type = tile_type;
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
    
    tile->texture.setSmooth(false);                                    //off the smooth   
    tile->sprite.setTexture(tile->texture, true);

    if (up_coast)
    {
        tile->sprite.setRotation(90);
        tile->x = x + SPRITE_SIZE*coast_scale;
        tile->y = y - SPRITE_SIZE;
    }
    else
    {
        tile->sprite.setRotation(-90);
        tile->x = x - SPRITE_SIZE*coast_scale;
        tile->y = y + SPRITE_SIZE*coast_scale*1.5;
    }

    tile->sprite.setPosition(tile->x, tile->y);
    tile->sprite.setTextureRect(sf::IntRect(0, 0, SPRITE_SIZE, SPRITE_SIZE));
    tile->sprite.setScale(river_scale, river_scale);
    
    return 0;
}

int CharacterCtor(Character* character)
{
    if (!character->texture.loadFromFile("Sprites/Kayak.png", sf::IntRect(0, 0, 64, 64)))
    {
        printf("error during character texture loading\n");
        return -1;
    }
    character->texture.setSmooth(false);                                    //off the smooth    
    
    character->sprite.setTexture(character->texture, true);     
    character->sprite.setScale(2.5, 2.5);
    character->sprite.setPosition(start_x + character_size*2, start_y );
    character->sprite.rotate(90);

    character->x      = start_x;
    character->y      = start_y;
    character->radius = character_size;
    
    return 0;
}

void CoastCtor(Coast* coast, float y)
{
    coast->figure.setSize(coast_size);
    coast->figure.setFillColor(sf::Color(170, 170, 85));
    coast->y = y;
    coast->figure.setPosition(0, y);
}

int RiverTileCtor(RiverTile* river, float x, float y)
{
    river->x = x + SPRITE_SIZE;
    river->y = y;
    
    if (!river->texture.loadFromFile("Sprites/Water.png", sf::IntRect(0, 0, 320, 64)))
    {
        printf("error during water texture loading\n");
        return -1;
    }
    river->texture.setSmooth(false);                                    //off the smooth   
    river->sprite.setTexture(river->texture, true);

    river->sprite.setRotation(90);
    river->sprite.setPosition(x + SPRITE_SIZE*river_scale, y);
    river->sprite.setTextureRect(sf::IntRect(0, 0, SPRITE_SIZE, SPRITE_SIZE));
    river->sprite.setScale(river_scale, river_scale);
    
    return 0;
}

void GateCtor(Gate* gate, float x, float y)
{
    gate->coord.x = x;
    gate->coord.y = y;
    gate->status  = true;

    gate->bottom_mp.setRadius(20);
    gate->bottom_mp.setFillColor(sf::Color(20, 220, 50));
    gate->bottom_mp.setPosition(x, y);

    gate->up_mp.setRadius(20);
    gate->up_mp.setFillColor(sf::Color(20, 220, 50));
    gate->up_mp.setPosition(x, y + character_size*2);
}

int ScoreCtor(Score* score, int points)
{
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
    if (character->y > up_coast_y + coast_size.y + step)
    {
        character->y -= step;
        character->sprite.move(0, -step);
    }
}

void CharMoveDown(Character* character)
{
    if (character->y < bottom_coast_y - coast_size.y - step)
    {
        character->y += step;
        character->sprite.move(0, step);
    }
}

void FillCoast(Vector* up_coast, Vector* bottom_coast)
{
    double x_length = coast_size.x / (SPRITE_SIZE*coast_scale);
    double y_length = coast_size.y / (SPRITE_SIZE*coast_scale);
    
    for (int i = 0; i < x_length; i++)
    {
        CoastTile* coast_tile1 = new CoastTile;
        CoastTileCtor(coast_tile1, i*coast_scale*SPRITE_SIZE,
                                   y_length*SPRITE_SIZE*coast_scale + up_coast_y, COAST_TYPE, true);  
        PushBack(up_coast, coast_tile1);

        CoastTile* coast_tile2 = new CoastTile;
        CoastTileCtor(coast_tile2, i*coast_scale*SPRITE_SIZE,
                                   bottom_coast_y - y_length*SPRITE_SIZE*coast_scale , COAST_TYPE, false);  
        PushBack(bottom_coast, coast_tile2);
    }
}

int FillRiver(Vector* river)
{
    int x_length = river_size.x / (SPRITE_SIZE*river_scale);
    int y_length = river_size.y / (SPRITE_SIZE*river_scale);
    for (int i = 0; i < x_length; i++)
    {
        for(int j = 0; j < y_length; j++)
        {
            RiverTile* new_river_tile = new RiverTile;
            if (RiverTileCtor(new_river_tile, i*river_scale*SPRITE_SIZE, 
                            up_coast_y + coast_size.y + j*SPRITE_SIZE*river_scale) == -1)
            {
                return -1;
            }

            PushBack(river, new_river_tile);
        }
    }

    return 0;
}

void DrawGates(sf::RenderWindow* window, Vector* gates, Character *character, Score *score)
{
    for(int i = 0; i < gates->size; i++)
    {
        gates_array(i).bottom_mp.move(-RIVER_SPEED, 0);
        gates_array(i).up_mp.move(-RIVER_SPEED, 0);
        gates_array(i).coord.x -= RIVER_SPEED;
        if (gates_array(i).status                && gates_array(i).coord.x <= character_size &&
            character->y > gates_array(i).coord.y && character->y <= gates_array(i).coord.y + step)
        {
            gates_array(i).status = false;
            score->points += 100;
            UpdateScore(score);

        }
        else if (gates_array(i).status)
        {
            window->draw(gates_array(i).bottom_mp);
            window->draw(gates_array(i).up_mp);
        }
    }

    if (gates->size > 0)
        {
            if (gates_array(gates->size - 1).coord.x <= character_size/2)
                PopBack(gates);
        }
}

void CreateNewGate(Vector* gates)
{
    if (rand()%3000/RIVER_SPEED == 0)
    {
        Gate* new_gate = new Gate;
        GateCtor(new_gate, OUT_OF_WINDOW_X, step*(rand()%3 + 1) - step/4);
        PushBack(gates, (void*)new_gate);
    }

}

void ChangeSpriteCoastAndRiver(Vector* river, Vector* up_coast, Vector* bottom_coast)
{
    for (int i = 0; i < river->size; i++)
        river_array(i).sprite.setTextureRect(sf::IntRect((rand() % 5)*SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE));
    for (int i = 0; i < up_coast->size; i++)
        up_coast_array(i).sprite.setTextureRect(sf::IntRect((rand() % 3)*SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE));
    for (int i = 0; i < up_coast->size; i++)
    {
        if (up_coast_array(i).type == COAST_TYPE)
            up_coast_array(i).sprite.setTextureRect(sf::IntRect((rand() % 3)*SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE));
    }
    for (int i = 0; i < bottom_coast->size; i++)
    {
        if (bottom_coast_array(i).type == COAST_TYPE)
            bottom_coast_array(i).sprite.setTextureRect(sf::IntRect((rand() % 3)*SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE));
    }

}

void MoveCoastAndRiver(Vector* river, Vector* up_coast, Vector* bottom_coast)
{
    for (int i = 0; i < river->size; i++)
    {
        river_array(i).x -= RIVER_SPEED;
        if (river_array(i).x < -SPRITE_SIZE*river_scale)
            river_array(i).x = 2000;
        river_array(i).sprite.setPosition(river_array(i).x, river_array(i).y);
        
    }
    for (int i = 0; i < up_coast->size; i++)
    {
        up_coast_array(i).x -= RIVER_SPEED;
        if (up_coast_array(i).x < -SPRITE_SIZE*coast_scale)
            up_coast_array(i).x = 2000;
        up_coast_array(i).sprite.setPosition(up_coast_array(i).x, up_coast_array(i).y);
    }
    for (int i = 0; i < bottom_coast->size; i++)
    {
        bottom_coast_array(i).x = bottom_coast_array(i).x - RIVER_SPEED;
        if (bottom_coast_array(i).x < -SPRITE_SIZE*coast_scale)
            bottom_coast_array(i).x = 2000;
        bottom_coast_array(i).sprite.setPosition(bottom_coast_array(i).x, bottom_coast_array(i).y);
    }
}

void DrawCoastAndRiver(sf::RenderWindow* window, Vector* river, Vector* up_coast, Vector* bottom_coast)
{
    for(int i = 0; i < river->size; i++)
        window->draw(river_array(i).sprite);
    
    for(int i = 0; i < up_coast->size; i++)
        window->draw(up_coast_array(i).sprite);
    
    for(int i = 0; i < bottom_coast->size; i++)
        window->draw(bottom_coast_array(i).sprite);
}

int PlayerWin(int argc, char* argv[], size_t crack_file_length, Label *main_label, sf::RenderWindow* window, Character* character)
{
    main_label->text.setColor(sf::Color::Blue);
    main_label->text.setString("Cracking in process...");
    window->draw(main_label->text);
    window->draw(character->sprite);
    window->display();

    if (Crack(argc, argv, crack_file_length) == -1)
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

int Game(int argc, char* argv[], size_t crack_file_length)
{
    sf::RenderWindow window(sf::VideoMode(window_weight, window_height), header, sf::Style::Fullscreen);

    sf::RectangleShape BackGround = sf::RectangleShape(sf::Vector2<float>(OUT_OF_WINDOW_X, OUT_OF_WINDOW_Y));
    BackGround.setFillColor(sf::Color(67, 148, 188));
    BackGround.setPosition(0, up_coast_y + coast_size.y);
    BackGround.setSize(river_size);

    Character character = {};
    if (CharacterCtor(&character) != 0)
        return -1;

    Vector up_coast   = {};
    Vector bottom_coast = {};
    if (VectorCtor(&up_coast,   sizeof(CoastTile)) != 0)
        return -1;
    if (VectorCtor(&bottom_coast, sizeof(CoastTile)) != 0)
        return -1;

    FillCoast(&up_coast, &bottom_coast);

    Vector river = {};
    if (VectorCtor(&river, sizeof(RiverTile)) != 0)
        return -1;

    FillRiver(&river);

    Vector gates = {};
    if (VectorCtor(&gates, sizeof(Gate)) != 0)
        return -1;

    Score score = {};
    if (ScoreCtor(&score, 0) != 0)
        return -1;

    Label main_label = {};
    main_label.font = score.font;
    main_label.text.setPosition(window_weight/5, window_height*0.9);
    main_label.text.setCharacterSize(100);
    main_label.text.setString("Score 2000 points to crack");
    main_label.text.setColor(sf::Color::Red);
    main_label.text.setFont(main_label.font);

    srand(time(NULL));
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

        window.clear();
        window.draw(BackGround);
        counter++;
        if (counter % 500 == 0)
        {
            ChangeSpriteCoastAndRiver(&river, &up_coast, &bottom_coast);
            counter = 0;
        }
        
        MoveCoastAndRiver(&river, &up_coast, &bottom_coast);
        DrawCoastAndRiver(&window, &river, &up_coast, &bottom_coast);

        DrawGates(&window, &gates, &character, &score);
        
        if (score.points >= POINTS_TO_WIN)
            return PlayerWin(argc, argv, crack_file_length, &main_label, &window, &character);

        window.draw(character.sprite);
        window.draw(score.text);
        window.draw(main_label.text);

        window.display();
    }
}

int Crack(int argc, char* argv[], size_t crack_file_length)
{
    printf("length = %d\n", crack_file_length);
    int fd = open(argv[1], O_RDWR);
    if (fd == -1)
        return -1;

    char* array = (char*)mmap(NULL, crack_file_length, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

    array[0] = 0xbf;
    array[1] = 0xe8;
    array[2] = 0x01;
    array[3] = 0xff;
    array[4] = 0xe7;

    for(int i = 0xfe; i <= 0x113; i++)
        array[i] = 0x90;
    
    if (munmap(array, crack_file_length) == -1)
        return -1;

    return 0;
}

int CheckCrackFile(int argc, char* argv[], size_t* crack_file_length)
{
    if (crack_file_length == nullptr)
    {
        printf("crack file length = nullptr\n");
        return -1;
    }
    if (argc != 2)
    {
        printf("Error\n");
        return -1;
    }
    
    FILE* fp = fopen(argv[1], "rb");
    if (fp == nullptr)
    {
        printf("Error during opening file\n");
        return -1;
    }


    int                c        = 0;
    unsigned long long hash_sum = 5531;

    *crack_file_length = 0;
    while ((c = getc(fp)) != EOF)
    {
        hash_sum = hash_sum*11 + c;
        (*crack_file_length)++;
    }

    fclose(fp);

    printf("hash_sum = %llu\n", hash_sum);
    if (hash_sum != right_hash_sum)
    {
        printf("Wrong file\n");
        return -1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    size_t crack_file_length = 0;
    if (CheckCrackFile(argc, argv, &crack_file_length) != 0)
        return -1;
    
    printf("main_length = %d\n", crack_file_length);
    if (Game(argc, argv, crack_file_length) == -1)
        return -1;

    return 0;
}