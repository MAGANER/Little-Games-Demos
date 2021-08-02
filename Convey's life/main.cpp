#include"SFML/Graphics.hpp"
#include<random>
#include<vector>
#include<iostream>

using namespace std;
using namespace sf;


const int CELL_SIZE = 16;
const int MAX_X = 30;
const int MAX_Y = 30;

constexpr int WINDOW_WIDTH = MAX_X * CELL_SIZE;
constexpr int WINDOW_HEIGHT = MAX_Y * CELL_SIZE;

typedef vector<vector<bool>> StateMatrix;

enum class State
{
    live,
    generate,
    die
};


int get_random_int(int min, int max);
StateMatrix init_field();
vector<Vector2u> get_random_cells(int number);
void draw_field(RenderWindow& win, const StateMatrix& field);
State get_state(const StateMatrix& field, Vector2u pos);
void update(StateMatrix& field);
int main()
{
    StateMatrix field = init_field();
    vector<Vector2u> first_cells = get_random_cells(70);
    for (size_t i = 0; i < first_cells.size(); ++i)
    {
        Vector2u pos = first_cells[i];
        field[pos.y][pos.x] = true;
    }

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Life");
    window.setFramerateLimit(10);
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
            if (event.type == Event::Closed) window.close();

        update(field);
        window.clear();
        draw_field(window, field);
        window.display();
    }
}
int get_random_int(int min, int max)
{
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<std::mt19937::result_type> gen(min, max);
    return gen(rng);
}
StateMatrix init_field()
{
    StateMatrix field;
    for (int y = 0; y < MAX_Y; ++y)
    {
        vector<bool> sub;
        for (int x = 0; x < MAX_X; ++x)
        {
            sub.push_back(false);
        }
        field.push_back(sub);
    }
    return field;
}
vector<Vector2u> get_random_cells(int number)
{
    vector<Vector2u> cells;
   for (int i = 0; i < number; ++i)
   {
       int x = get_random_int(0, MAX_X - 1);
       int y = get_random_int(0, MAX_Y - 1);
       cells.push_back(Vector2u(x, y));
   }

   return cells;
}
void draw_field(RenderWindow& win, const StateMatrix& field)
{
    RectangleShape cell(Vector2f((float)CELL_SIZE, (float)CELL_SIZE));
    for (int y = 0; y < MAX_Y; ++y)
    {
        for (int x = 0; x < MAX_X; ++x)
        {
            if (field[y][x])
            {
                cell.setPosition(Vector2f((float)x * CELL_SIZE, (float)y*CELL_SIZE));
                win.draw(cell);
            }
        }
    }
}

State get_state(const StateMatrix& field, Vector2u pos)
{
    int counter = 0;

    auto not_out_of_x = [&](int x) {return x > 0 && x < MAX_X-1; };
    auto not_out_of_y = [&](int y) {return y > 0 && y < MAX_Y-1; };

    bool inside = not_out_of_y(pos.y) && not_out_of_x(pos.x);
    if (inside) if (field[pos.y][pos.x - 1])++counter;
    if (inside) if (field[pos.y][pos.x + 1])++counter;
    if (inside) if (field[pos.y+1][pos.x])  ++counter;
    if (inside) if (field[pos.y-1][pos.x])  ++counter;
    if (inside) if (field[pos.y + 1][pos.x + 1]) ++counter;
    if (inside) if (field[pos.y - 1][pos.x - 1]) ++counter;
    if (inside) if (field[pos.y - 1][pos.x + 1]) ++counter;
    if (inside) if (field[pos.y + 1][pos.x - 1])++counter;

    if (counter < 2 || counter > 3) return State::die;
    else if (counter == 2) return State::live;
    else return State::generate;
}
void update(StateMatrix& field)
{
    for (int y = 0; y < MAX_Y; ++y)
        for (int x = 0; x < MAX_X; ++x)
        {
            State state = get_state(field, Vector2u(x, y));
            switch (state)
            {
            case State::live:
                break;
            case State::generate:
                field[y][x] = true;
                break;
            case State::die:
                field[y][x] = false;
                break;
            }
        }
}