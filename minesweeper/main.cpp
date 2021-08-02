#include"SFML/Graphics.hpp"
#include<vector>
#include<random>
#include<iostream>
using namespace sf;
using namespace std;

enum class CELL_TYPE {mine, empty};

const int MAP_WIDTH   = 16;
const int MAP_HEIGHT  = 12;
const int WIN_WIDTH   = 640;
const int WIN_HEIGHT  = 480;

const int MINE_NUMBER = 10;

const int MAX_CELL_X = MAP_WIDTH - 3;
const int MAX_CELL_Y = MAP_HEIGHT + 2;

bool EVIL_GLOBAL_SWITCHER = false; //set true if you died

template<class T>
vector<vector<T>> get_submatrix(const vector<vector<T>>& matrix,
				size_t row, 
			    size_t col,
				size_t center_x,
			    size_t center_y)
{
	if (matrix.empty())return vector<vector<T>>();

	size_t left_x = (int)(center_x - row) < 0 ? 0 : center_x - row;
	size_t right_x= center_x + row > matrix[0].size() ? matrix[0].size() : row + center_x;

	size_t high_y = (int)(center_y - col) < 0 ? 0 : center_y - col;
	size_t bottom_y = center_y+col > matrix.size() ? matrix.size() : col + center_y;


	vector<vector<T>> sub_matrix;
	for (int y = high_y; y < bottom_y; y++)
	{
		vector<T> row;
		for (int x = left_x; x < right_x; x++) row.push_back(matrix[y][x]);
		sub_matrix.push_back(row);
	}
	return sub_matrix;
}


typedef vector<vector<CELL_TYPE>> _map;
typedef pair<Vector2i, IntRect> cell_rect;

_map get_map();
_map set_mines(const _map& map);
void render_net(RenderWindow& window);
int get_random_number(int begin, int end);
int get_mine_number(const _map& map);
string convert_secs_to_regular_time_view(float seconds);
void check_mouse_hit(_map& map,
					 vector<cell_rect>& bounds,
					 RenderWindow& window,
				     vector<Text>& numbers,
					 Font& font);
void run(Vector2i start,
		 _map& map,
		 vector<cell_rect>& bounds,
		 RenderWindow& window,
		 vector<Text>& numbers,
		 Font& font,
	     int d);
void erase_cell(const Vector2i& pos, vector<cell_rect>& bounds);
bool is_mine(const Vector2i& pos, _map& map);
bool out_of_map(const Vector2i& pos);

vector<cell_rect> get_net_bounds();

vector<Vector2i> get_positions_around(const Vector2i& start);
Vector2i get_curr_pos(vector<cell_rect>& bounds, RenderWindow& window);
Vector2f get_pos(const Vector2i& pos, vector<cell_rect>& bounds);

int compute_mine_number(const _map& map,int x, int y);
int main()
{
	RenderWindow window(VideoMode(WIN_WIDTH, WIN_HEIGHT), "MineSweeper");

	auto map = set_mines(get_map());
	auto net_bounds = get_net_bounds();

	Font font;
	font.loadFromFile("font.ttf");

	Text mine_number;
	mine_number.setFont(font);
	mine_number.setCharacterSize(16);
	mine_number.setPosition(Vector2f(540.0f, 10.0f));
	mine_number.setFillColor(Color::Black);
	

	Text time;
	time.setFont(font);
	time.setCharacterSize(16);
	time.setPosition(Vector2f(540.0f, 30.0f));
	time.setFillColor(Color::Black);

	vector<Text> numbers;

	Clock clock;

	auto elapsed_time = clock.getElapsedTime();
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)window.close();
		}
		if(!EVIL_GLOBAL_SWITCHER)check_mouse_hit(map,net_bounds, window,numbers,font);

		window.clear(Color(128,128,128));
		render_net(window);

		mine_number.setString("mines:" + to_string(get_mine_number(map)));
		window.draw(mine_number);

		if(!EVIL_GLOBAL_SWITCHER)elapsed_time = clock.getElapsedTime();
		time.setString("time" + convert_secs_to_regular_time_view(elapsed_time.asSeconds()));
		window.draw(time);
		for (auto& r : net_bounds)
		{
			RectangleShape shape;
			shape.setSize(Vector2f(r.second.width, r.second.height));
			shape.setPosition(Vector2f(r.second.left, r.second.top));

			auto mouse_pos = Mouse::getPosition(window);
			if (r.second.contains(mouse_pos))
				shape.setFillColor(Color::Yellow);
			else 
				shape.setFillColor(Color::Red);
			
			window.draw(shape);
		}
		for (auto& n : numbers)
		{
			window.draw(n);
		}
		window.display();
	}

	return 0;
}
_map get_map()
{
	_map map;
	for (size_t y = 0; y < MAX_CELL_Y; ++y)
	{
		vector<CELL_TYPE> sub;
		for (size_t x = 0; x < MAX_CELL_X; ++x) sub.push_back(CELL_TYPE::empty);
		map.push_back(sub);
	}
	return map;
}
void render_net(RenderWindow& window)
{
	RectangleShape vert_bar;

	Vector2f vpos((float)MAP_WIDTH * 3, 0.0f);
	vert_bar.setSize(Vector2f(2.0f, WIN_HEIGHT));
	vert_bar.setPosition(vpos);

	for (size_t x = 0; x < MAP_WIDTH; ++x)
	{
		window.draw(vert_bar);
		vpos.x += MAP_WIDTH * 2;
		vert_bar.setPosition(vpos);
	}

	RectangleShape hor_bar;

	auto size = WIN_WIDTH - ((float)MAP_WIDTH * 12);
	Vector2f hpos((float)MAP_WIDTH * 4, (float)MAP_HEIGHT * 2+MAP_HEIGHT/2);
	hor_bar.setSize(Vector2f(size, 2.0f));
	hor_bar.setPosition(hpos);

	for (size_t y = 0; y < MAP_HEIGHT+4; ++y)
	{
		window.draw(hor_bar);
		hpos.y += (float)MAP_HEIGHT * 2 + MAP_HEIGHT / 2;
		hor_bar.setPosition(hpos);
	}
}
int get_random_number(int begin, int end)
{
	random_device rd; 
	mt19937 gen(rd()); 
	uniform_int_distribution<> dist(begin, end);
	return dist(gen);
}
_map set_mines(const _map& map)
{
	_map updated = map;
	for (int i = 0; i != MINE_NUMBER; i++)
	{
		int rand_x = get_random_number(0, MAX_CELL_X-1);
		int rand_y = get_random_number(0, MAX_CELL_Y-1);
		updated[rand_y][rand_x] = CELL_TYPE::mine;
	}
	return updated;
}
int get_mine_number(const _map& map)
{
	int counter = 0;
	for (size_t y = 0; y < MAX_CELL_Y; ++y)
	{
		for (size_t x = 0; x < MAX_CELL_X; ++x)
			if (map[y][x] == CELL_TYPE::mine)counter++;
	}
	return counter;
}
string convert_secs_to_regular_time_view(float seconds)
{
	auto min  = (int)seconds / 60;
	auto hour = (int)min / 60;
	auto secs = (int)seconds % 60;
	return to_string(hour) + ":" + to_string(min) + ":" + to_string(secs);
}
void check_mouse_hit(_map& map, 
					 vector<cell_rect>& bounds, 
					 RenderWindow& window,
					 vector<Text>& numbers,
					 Font& font)
{
	if (Mouse::isButtonPressed(Mouse::Left))
	{
		auto start = get_curr_pos(bounds, window);
		if (start != Vector2i(-1, -1))
		{
			run(start, map, bounds, window, numbers, font, 1);
			run(start, map, bounds, window, numbers, font, 2);
			run(start, map, bounds, window, numbers, font, 3);
			run(start, map, bounds, window, numbers, font, 4);
			run(start, map, bounds, window, numbers, font, 5);
		}
	}
}
void run(Vector2i start,
		 _map& map,
		 vector<cell_rect>& bounds,
		 RenderWindow& window,
		 vector<Text>& numbers,
		 Font& font,
		 int d)
{
	if (out_of_map(start)) return;

	auto cell_pos = get_pos(start, bounds);

	if (is_mine(start, map))
	{
		if (cell_pos != Vector2f(-1, -1))
		{
			Text number;
			number.setFillColor(Color::Blue);
			number.setCharacterSize(18);
			number.setString("x");
			number.setFont(font);
			number.setPosition(cell_pos);
			numbers.push_back(number);
		}
		EVIL_GLOBAL_SWITCHER = true;
		erase_cell(start, bounds);//if able	
		return;
	}
	else
	{
		int mine_number = compute_mine_number(map, start.x, start.y);
		if (mine_number != 0)
		{
			if (cell_pos != Vector2f(-1, -1))
			{
				Text number;
				number.setFillColor(Color::Blue);
				number.setCharacterSize(18);
				number.setString(to_string(mine_number));
				number.setFont(font);
				number.setPosition(get_pos(start, bounds));
				numbers.push_back(number);
			}
			erase_cell(start, bounds);//if able	
			return;
		}
		erase_cell(start, bounds);//if able	
	}


	if (d == 1)
	{
		Vector2i p1(start.x - 1, start.y);
		run(p1, map, bounds, window, numbers, font,-1);

		Vector2i p2(start.x, start.y + 1);
		run(p2, map, bounds, window, numbers, font,-1);
	}
	if (d == 2)
	{
		Vector2i p1(start.x, start.y-1);
		run(p1, map, bounds, window, numbers, font, -1);

		Vector2i p2(start.x+1, start.y);
		run(p2, map, bounds, window, numbers, font, -1);
	}
	if (d == 3)
	{
		Vector2i p1(start.x - 1, start.y - 1);
		run(p1, map, bounds, window, numbers, font, -1);

		Vector2i p2(start.x + 1, start.y + 1);
		run(p2, map, bounds, window, numbers, font, -1);
	}
	if (d == 4)
	{
		Vector2i p1(start.x + 1, start.y - 1);
		run(p1, map, bounds, window, numbers, font, -1);

		Vector2i p2(start.x + 1, start.y - 1);
		run(p2, map, bounds, window, numbers, font, -1);
	}
	if (d == 5)
	{
		Vector2i p1(start.x - 1, start.y + 1);
		run(p1, map, bounds, window, numbers, font, -1);

		Vector2i p2(start.x - 1, start.y + 1);
		run(p2, map, bounds, window, numbers, font, -1);
	}

}
Vector2f get_pos(const Vector2i& pos, vector<cell_rect>& bounds)
{
	for (auto& b : bounds)
	{
		if (b.first.x == pos.x && b.first.y == pos.y)
		{
			return Vector2f(b.second.left+2, b.second.top);
		}
	}
	return Vector2f(-1.0f, -1.0f);
}
int compute_mine_number(const _map& map,int x, int y)
{
	auto matrix = get_submatrix(map, 2, 2, x, y);
	int mine_number = 0;

	for (int y = 0; y < matrix.size(); y++)
		for (int x = 0; x < matrix[y].size(); x++)
		{
			auto cell_type = matrix[y][x];
			if (cell_type == CELL_TYPE::mine)mine_number++;
		}
	
	
	return mine_number;
}
vector<cell_rect> get_net_bounds()
{
	const Vector2f VER_BAR_MIN_POS(80.0f, 0.0f);
	constexpr int VER_BAR_STEP = MAP_WIDTH * 2;

	const Vector2f HOR_BAR_MIN_POS(0.0f, 31.0f);
	constexpr int HOR_BAR_STEP = MAP_HEIGHT * 2 + MAP_HEIGHT / 2;

	vector<cell_rect> bounds;

	auto curr_vert_bar = VER_BAR_MIN_POS;
	auto curr_hor_bar  = HOR_BAR_MIN_POS;
	auto size = Vector2i(30.0f, 26.0f);
	for (int y = 0; y < MAX_CELL_Y; y++)
	{
		for (int x = 0; x < MAX_CELL_X; x++)
		{
			auto pos = Vector2i(curr_vert_bar.x, curr_hor_bar.y);
			IntRect rect(pos, size);
			auto cell = make_pair(Vector2i(x, y), rect);
			bounds.push_back(cell);
			curr_vert_bar.x += VER_BAR_STEP;
		}
		curr_vert_bar.x = VER_BAR_MIN_POS.x;
		curr_hor_bar.y += HOR_BAR_STEP;
	}
	return bounds;
}
vector<Vector2i> get_positions_around(const Vector2i& start)
{
	auto p1 = Vector2i(start.x - 1, start.y);
	auto p2 = Vector2i(start.x + 1, start.y);
	auto p3 = Vector2i(start.x, start.y + 1);
	auto p4 = Vector2i(start.x, start.y + 1);
	auto p5 = Vector2i(start.x - 1, start.y - 1);
	auto p6 = Vector2i(start.x + 1, start.y + 1);
	auto p7 = Vector2i(start.x - 1, start.y + 1);
	auto p8 = Vector2i(start.x + 1, start.y - 1);

	return vector<Vector2i>{p1, p2, p3, p4, p5, p6, p7, p8};
}
Vector2i get_curr_pos(vector<cell_rect>& bounds, RenderWindow& window)
{
	Vector2i pos(-1,-1);
	for (auto& b : bounds)
	{
		if (b.second.contains(Mouse::getPosition(window)))
		{
			pos = b.first;
		}
	}
	return pos;
}
void erase_cell(const Vector2i& pos, vector<cell_rect>& bounds)
{
	for (auto& b : bounds)
	{
		if (b.first.x == pos.x && b.first.y == pos.y)
		{
			auto found = find(bounds.begin(), bounds.end(), b);
			bounds.erase(found);
		}
	}
}
bool is_mine(const Vector2i& pos, _map& map)
{
	return map[pos.y][pos.x] == CELL_TYPE::mine;
}
bool out_of_map(const Vector2i& pos)
{
	return pos.x < 0 || pos.y < 0 ||
		pos.x >= MAX_CELL_X || pos.y >= MAX_CELL_Y;
}