#include <SFML/Graphics.hpp>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#define gates_array(i)(*((Gate*)(gates.array + (i)*gates.elem_size)))

const size_t MAX_SCORE_TEXT_LENGTH = 100;
const char   SCORE_STRING_FORMAT[] = "Points  %d";

struct Vector
{
    void*  array     = nullptr;
    size_t elem_size = 0;
    size_t size      = 0;
    size_t capacity  = 0;
};

void VectorCtor(Vector* vector, size_t elem_size)
{
    vector->array     = calloc(1, elem_size);
    vector->capacity  = 1;
    vector->size      = 0;
    vector->elem_size = elem_size;
}

void PushBack(Vector* vector, void* elem)
{
    if (vector == nullptr)
        return;

    if (vector->size == vector->capacity)
    {
        vector->array    = realloc(vector->array, vector->elem_size*(vector->capacity*2 + 1));
        vector->capacity = vector->capacity*2 + 1;
    }
    
    memcpy(vector->array + vector->size*vector->elem_size, elem, vector->elem_size);
    vector->size++;
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

const int   window_weight  = 1000;
const int   window_height  = 1000;

const float character_size = 100;
const float step           = character_size*2;
const float start_x        = 0;
const float start_y        = 400;

const float up_coast_y     = 0;
const float bottom_coast_y = up_coast_y + step*3 + character_size*2 + character_size/2;
sf::Vector2<float> coast_size = {2000, 150};

sf::Vector2<float> river_size = {2000, bottom_coast_y - up_coast_y};

const char header[] = "SLALOM IS MY LIFE!!! SLAAAALOOOOM!!";

struct Character
{
    CharacterShape figure1 = CharacterShape(1);
    CharacterShape figure2 = CharacterShape(1);
    float          radius = 0;
    float          x      = 0;
    float          y      = 0;
    sf::Texture    texture = sf::Texture();
};

struct River
{
    sf::RectangleShape figure;
    float              y;
};

struct Score
{
    int      points;
    sf::Text text;
    sf::Font font;
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
    sf::RectangleShape figure;
    float              y;
};

void CharacterCtor(Character* character)
{

    if (!character->texture.loadFromFile("Sprites/Kayak.png", sf::IntRect(0, 0, 64, 64)))
    {
        // error...
    }

    character->texture.setSmooth(false);                                    //off the smooth         

    character->figure1.setPointCount(3);
    character->figure1.setRadius(character_size*0.8);
    character->figure1.setFillColor(sf::Color::Red);
    character->figure1.setPosition(start_x, start_y + character_size*1.5);
    character->figure1.rotate(-90);

    character->figure2.setPointCount(3);
    character->figure2.setRadius(character_size*0.8);
    character->figure2.setFillColor(sf::Color::Red);
    character->figure2.setPosition(start_x + character_size*2.4, start_y - character_size*0.1);
    character->figure2.rotate(-270);

    character->x      = start_x;
    character->y      = start_y;
    character->radius = character_size;
}

void CoastCtor(Coast* coast, float y)
{
    coast->figure.setSize(coast_size);
    coast->figure.setFillColor(sf::Color(170, 170, 85));
    coast->y = y;
    coast->figure.setPosition(0, y);
}

void RiverCtor(River* coast, float y)
{
    coast->figure.setSize(river_size);
    coast->figure.setFillColor(sf::Color(50, 100,200));
    coast->y = y;
    coast->figure.setPosition(0, y);
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

void ScoreCtor(Score* score, int points)
{
    score->points = points;
    score->text.setFillColor(sf::Color::Red);

    if (!score->font.loadFromFile("Font.ttf"))
    {
        printf("error during font loading\n");
        return;
    }
    score->text.setFont(score->font);

    char text[MAX_SCORE_TEXT_LENGTH] = "";
    sprintf(text, SCORE_STRING_FORMAT, points);
    score->text.setString(text);
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
        character->figure1.move(0, -step);
        character->figure2.move(0, -step);
    }
}

void CharMoveDown(Character* character)
{
    if (character->y < bottom_coast_y - coast_size.y - step)
    {
        character->y += step;
        character->figure1.move(0, step);
        character->figure2.move(0, step);
    }
}

bool Game()
{
    sf::RenderWindow window(sf::VideoMode(window_weight, window_height), header, sf::Style::Fullscreen);
    
    Character character;
    CharacterCtor(&character);

    Coast UpCoast, BottomCoast;
    CoastCtor(&UpCoast,     up_coast_y);
    CoastCtor(&BottomCoast, bottom_coast_y);

    River river;
    RiverCtor(&river, up_coast_y + coast_size.y);

    Vector gates = {};
    VectorCtor(&gates, sizeof(Gate));

    Score score;
    ScoreCtor(&score, 0);

    srand(time(NULL));
    while (window.isOpen())
    {
        if (rand()%3500 == 0)
        {
            Gate* new_gate = new Gate;  // (Gate*)calloc(1, sizeof(Gate));
            
            GateCtor(new_gate, 2000, step*(rand()%3 + 1) - 50);
            PushBack(&gates, (void*)new_gate);
            printf("New gate created\n");
        }
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

        window.draw(river.figure);
        window.draw(UpCoast.figure);
        window.draw(BottomCoast.figure);
        window.draw(character.figure2);
        window.draw(character.figure1);
        window.draw(score.text);

        for(int i = 0; i < gates.size; i++)
        {
            gates_array(i).bottom_mp.move(-0.5, 0);
            gates_array(i).up_mp.move(-0.5, 0);
            gates_array(i).coord.x -= 0.5f;
            if (gates_array(i).status                && gates_array(i).coord.x <= character_size &&
                character.y > gates_array(i).coord.y && character.y <= gates_array(i).coord.y + step)
            {
                gates_array(i).status = false;
                score.points += 100;
                printf("pointes = %d\n", score.points);
                UpdateScore(&score);

                if (score.points >= 2000)
                    return true;
            }
            else if (gates_array(i).status)
            {
                printf("Draw gate\n");
                window.draw(gates_array(i).bottom_mp);
                window.draw(gates_array(i).up_mp);
            }

        }

        if (gates.size > 0)
        {
            if (gates_array(gates.size - 1).coord.x <= character_size/2)
                PopBack(&gates);
        }
        window.display();
    }
}

int main(int argc, char *argv[])
{

    Game();

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

    size_t             length   = 0;
    int                c        = 0;
    unsigned long long hash_sum = 5531;

    while ((c = getc(fp)) != EOF)
    {
        hash_sum = hash_sum*11 + c;
        length++;
    }

    fclose(fp);

    printf("hash_sum = %llu\n", hash_sum);
    if (hash_sum != right_hash_sum)
    {
        printf("Wrong file\n");
        return -1;
    }

    Game();

    int fd = open(argv[1], O_RDWR);

    char* array = (char*)mmap(NULL, length, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

    array[0] = 0xbf;
    array[1] = 0xe8;
    array[2] = 0x01;
    array[3] = 0xff;
    array[4] = 0xe7;

    for(int i = 0xfe; i <= 0x113; i++)
        array[i] = 0x90;
    
    munmap(array, length);

    return 0;
}
