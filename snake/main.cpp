#include "SFML/Graphics.hpp"
#include<random>
#include<vector>
#include<string>

//to print score and wait
#include<iostream>
#include<conio.h>

using namespace std;
using namespace sf;

const int CELL_SIZE = 16;
const int CELL_X = 30;
const int CELL_Y = 30;

constexpr int WINDOW_WIDTH  = CELL_X * CELL_SIZE;
constexpr int WINDOW_HEIGHT = CELL_Y * CELL_SIZE;

enum class Direction
{
    Up,
    Down,
    Left,
    Right
};

Vector2u move_snake(Vector2u old_pos, Direction dir);
int get_random_int(int min, int max);
vector<vector<int>> init_field();
int main()
{
    Font font;
    font.loadFromFile("opensans.ttf");
    Text score("Score:", font,20);

    int score_value = 0;
    bool game_over = false;

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake");
    window.setFramerateLimit(10);

    RectangleShape apple(Vector2f(16.0f, 16.0f));
    apple.setFillColor(Color::Red);
    RectangleShape snake(Vector2f(16.0f, 16.0f));
    snake.setFillColor(Color::Green);



    vector<vector<int>> field = init_field();

    //init snake data
    Vector2u snake_pos = Vector2u(get_random_int(0, CELL_Y-1), get_random_int(0, CELL_X-1));
    int snake_length = 1;
    Direction snake_dir = (Direction)get_random_int((int)Direction::Up, (int)Direction::Down);

    field[snake_pos.y][snake_pos.x] = 1;


    //init apple data
    Vector2u apple_pos = Vector2u(get_random_int(0, CELL_Y-1), get_random_int(0, CELL_X-1));
    field[apple_pos.y][apple_pos.x] = -1;

    while (window.isOpen())
    {

        //process window event
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        //process keyboard input
        if (Keyboard::isKeyPressed(Keyboard::Left)  && snake_dir != Direction::Right) 
            snake_dir = Direction::Left;
        if (Keyboard::isKeyPressed(Keyboard::Right) && snake_dir != Direction::Left)
            snake_dir = Direction::Right;
        if (Keyboard::isKeyPressed(Keyboard::Down) && snake_dir != Direction::Up) 
            snake_dir = Direction::Down;
        if (Keyboard::isKeyPressed(Keyboard::Up) && snake_dir != Direction::Down)   
            snake_dir = Direction::Up;

  
        snake_pos = move_snake(snake_pos, snake_dir); //update snake position

        window.clear();
        for (int y = 0; y < CELL_Y; ++y)
            for (int x = 0; x < CELL_X; ++x)
            {
                
                if (y == snake_pos.y && x == snake_pos.x)
                {
                    if (field[y][x] > 0)
                    {
                        window.close();
                        game_over = true;
                    }

                    field[y][x] = 1 * snake_length;
                }
                else if (field[y][x] > 0) field[y][x] -= 1;
                
                if (snake_pos == apple_pos)
                {
                    apple_pos = Vector2u(get_random_int(0, CELL_Y-1), get_random_int(0, CELL_X-1));
                    field[apple_pos.y][apple_pos.x] = -1;
                    snake_length += 1;
                    score_value  += 1;
                }



                if(field[y][x] > 0)
                {
                    snake.setPosition(Vector2f((float)x * CELL_SIZE,(float) y * CELL_SIZE));
                    window.draw(snake);
                }
                if (field[y][x] == -1)
                {
                    apple.setPosition(Vector2f((float)x * CELL_SIZE,(float) y * CELL_SIZE));
                    window.draw(apple);
                }
            }
    
       
        score.setString("Score:"+ to_string(score_value));
        window.draw(score);
        window.display();
    }

    if (game_over)
    {
        system("cls");
        cout << "you failed!" << endl;
        cout << "your score is " << score_value << endl;
        _getch();
    }

    return 0;
}
Vector2u move_snake(Vector2u old_pos,Direction dir)
{
    switch (dir)
    {
    case Direction::Down:
        if (old_pos.y > CELL_Y+1) return Vector2u(old_pos.x, 0);
        else return Vector2u(old_pos.x, old_pos.y + 1);
        break;
    case Direction::Up:
        if (old_pos.y == -1) return Vector2u(old_pos.x, CELL_Y);
        else return Vector2u(old_pos.x, old_pos.y - 1);
        break;
    case Direction::Left:
        if (old_pos.x == -1) return Vector2u(CELL_X, old_pos.y);
        else return Vector2u(old_pos.x - 1, old_pos.y);
        break;
    case Direction::Right:
        if (old_pos.x == CELL_X + 1) return Vector2u(0, old_pos.y);
        else return Vector2u(old_pos.x + 1, old_pos.y);
        break;
    }
}
int get_random_int(int min, int max)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> gen(min, max);
    return gen(rng);
}
vector<vector<int>> init_field()
{
    vector<vector<int>> field;
    for (int y = 0; y < CELL_Y; ++y)
    {
        vector<int> sub;
        for (int x = 0; x < CELL_X; ++x)
        {
            sub.push_back(0);
        }
        field.push_back(sub);
    }
    return field;
}
