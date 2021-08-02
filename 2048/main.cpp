#include"SFML/Graphics.hpp"
#include<vector>
#include<random>
#include<iostream>

using namespace sf;
using namespace std;


const int MAX_X = 4;
const int MAX_Y = 4;
const int CELL_SIZE = 48;
constexpr int WIDTH = MAX_X * CELL_SIZE;
constexpr int HEIGHT = (MAX_Y * CELL_SIZE) + CELL_SIZE;//leave place for score text


typedef vector<vector<int>> StateMatrix;

enum class Direction
{
    Up,
    Down,
    Left,
    Right
};

int get_random_int(int min, int max);
StateMatrix init_field();
void draw_field(const StateMatrix& field,
    RenderWindow& window,
    const Font& font,
    const vector<Color>& colors);
vector<Color> generate_random_colors();
vector<Vector2u> get_free_cells(const StateMatrix& field);
void add_random_cells(StateMatrix& field, int number);
void update(StateMatrix& field, Direction dir, int& score);
void check_progress(const StateMatrix& field, int score,RenderWindow& win);
bool did_win(const StateMatrix& field);
int main()
{
    vector<Color> colors = generate_random_colors();
    StateMatrix field = init_field();
    add_random_cells(field, 2);

    Font font;
    font.loadFromFile("font.ttf");
    Text score("Score:", font, CELL_SIZE);
    score.setCharacterSize(32);
    int score_val = 0;


    bool key_pressed = false;
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "2048");
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
            if (event.type == Event::Closed) window.close();

        if (Keyboard::isKeyPressed(Keyboard::Up) && !key_pressed)
        {
            update(field, Direction::Up, score_val);
            key_pressed = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Down) && !key_pressed)
        {
            update(field, Direction::Down, score_val);
            key_pressed = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Left) && !key_pressed)
        {
            update(field, Direction::Left, score_val);
            key_pressed = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Right) && !key_pressed)
        {
            update(field, Direction::Right, score_val);
            key_pressed = true;
        }

        check_progress(field, score_val,window);

        window.clear();

        score.setString("Score:" + to_string(score_val));
        score.setPosition(Vector2f(0.0f, (float)HEIGHT - 32));
        window.draw(score);

        draw_field(field, window, font, colors);
        window.display();


        if (!Keyboard::isKeyPressed(Keyboard::Right) &&
            !Keyboard::isKeyPressed(Keyboard::Left) &&
            !Keyboard::isKeyPressed(Keyboard::Up) &&
            !Keyboard::isKeyPressed(Keyboard::Down))
            key_pressed = false;
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
    for (int y = 0; y != MAX_Y; ++y)
    {
        vector<int> sub;
        for (int x = 0; x != MAX_X; ++x)
        {
            sub.push_back(0);
        }
        field.push_back(sub);
    }
    return field;
}
void draw_field(const StateMatrix& field, RenderWindow& window, const Font& font, const vector<Color>& colors)
{
    RectangleShape object(Vector2f((float)CELL_SIZE, (float)CELL_SIZE));

    Text val;
    val.setFont(font);
    val.setCharacterSize(16);
    val.setFillColor(Color::Black);

    for (int y = 0; y != MAX_Y; ++y)
        for (int x = 0; x != MAX_X; ++x)
        {
            const int x_pos = x * CELL_SIZE;
            const int y_pos = y * CELL_SIZE;

            int cell_val = field[y][x];

            object.setPosition(Vector2f((float)x_pos, (float)y_pos));
            if (cell_val != 0) object.setFillColor(colors[cell_val]);
            else object.setFillColor(Color::White);

            window.draw(object);


            if (cell_val != 0)
            {
                val.setString(to_string(cell_val));
                val.setPosition(Vector2f(x_pos + 16, y_pos + 16));
                window.draw(val);
            }
        }
}
vector<Color> generate_random_colors()
{
    vector<Color> colors;
    for (int i = 0; i != 1024; ++i)
    {
        int red = get_random_int(0, 255);
        int green = get_random_int(0, 255);
        int blue = get_random_int(0, 255);
        Color color = Color((Uint8)red, (Uint8)green, (Uint8)blue);
        colors.push_back(color);
    }
    return colors;
}
vector<Vector2u> get_free_cells(const StateMatrix& field)
{
    vector<Vector2u> cells;
    for (int y = 0; y < MAX_Y; ++y)
    {
        for (int x = 0; x < MAX_X; ++x)
        {
            int val = field[y][x];
            if (val == 0)cells.push_back(Vector2u(x, y));
        }
    }
    return cells;
}
void add_random_cells(StateMatrix& field, int number)
{
    vector<Vector2u> free_poses = get_free_cells(field);
    if (!free_poses.empty())
        for (int i = 0; i < number; ++i)
        {
            int pos = get_random_int(0, free_poses.size() - 1);
            Vector2u _pos = free_poses[pos];
            field[_pos.y][_pos.x] = 2;
        }
}
void update(StateMatrix& field, Direction dir, int& score)
{
    for (int i = 0; i < 4; ++i)
    {
        for (int y = 0; y != MAX_Y; ++y)
            for (int x = 0; x != MAX_X; ++x)
            {
                switch (dir)
                {
                case Direction::Up:
                    if (y != 0)
                    {
                        int curr = field[y][x];
                        int next = field[y - 1][x];
                        if (curr == next || next == 0)
                        {
                            field[y - 1][x] = next + curr;
                            field[y][x] = 0;
                        }
                        if (curr == next && next != 0)score += 1;
                    }
                    break;
                case Direction::Down:
                    if (y != MAX_Y - 1)
                    {
                        int curr = field[y][x];
                        int next = field[y + 1][x];
                        if (curr == next || next == 0)
                        {
                            field[y + 1][x] = next + curr;
                            field[y][x] = 0;
                        }
                        if (curr == next && next != 0)score += 1;
                    }
                    break;
                case Direction::Left:
                    if (x != 0)
                    {
                        int curr = field[y][x];
                        int next = field[y][x - 1];
                        if (curr == next || next == 0)
                        {
                            field[y][x - 1] = next + curr;
                            field[y][x] = 0;
                        }
                        if (curr == next && next != 0)score += 1;
                    }
                    break;
                case Direction::Right:
                    if (x != MAX_X - 1)
                    {
                        int curr = field[y][x];
                        int next = field[y][x + 1];
                        if (curr == next || next == 0)
                        {
                            field[y][x + 1] = next + curr;
                            field[y][x] = 0;
                        }
                        if (curr == next && next != 0)score += 1;
                    }
                    break;
                }
            }
    }
    add_random_cells(field, 1);
}
void check_progress(const StateMatrix& field, int score,RenderWindow& win)
{
    bool victory = did_win(field);

    vector<Vector2u> poses = get_free_cells(field);
    if (poses.empty() && !victory)
    {
        win.close();
        cout << "You failed! Your total score is " << score << endl;
        system("pause");
        exit(0);
    }
    if (victory)
    {
        win.close();
        cout << "GZ! Victory is yours! Your total score is " << score << endl;
        system("pause");
        exit(2048);
    }
}
bool did_win(const StateMatrix& field)
{
    for (int y = 0; y < MAX_Y; ++y)
        for (int x = 0; x < MAX_X; ++x)
            if (field[y][x] == 2048) return true;

    return false;
}