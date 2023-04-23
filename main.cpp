#include <iostream>
#include <conio.h>
#include <map>
#include <string>
#include <time.h>

using namespace std;

// global variables
// enums
enum Key
{
	Enter, Space, Up, Down, East, West, None
};
enum Block
{
	// edge
	TopLeft, Left, TopRight, Right, Top, BottomLeft, BottomRight, Bottom,
	// body
	HorizontalBody, VerticalBody,
	Shot, RecentlyShot, Empty, Untouched,
	Target
};
enum Size
{
	Width, Height
};

// DISPLAY SETTINGS
// ship body
map<const Block, const string> ship
{
	{ Block::Top, "╥" },
	{ Block::Bottom, "╨"},
	{ Block::Left, "╞" },
	{ Block::Right, "╡" },
	{ Block::HorizontalBody, "-" },
	{ Block::VerticalBody, "║"}
};
// field
map<const Block, const string> field
{
	// edge
	{ Block::Top, "═" },
	{ Block::TopRight, "╗" },
	{ Block::TopLeft, "╔" },
	{ Block::Bottom, "═" },
	{ Block::BottomRight, "╝" },
	{ Block::BottomLeft, "╚" },
	{ Block::Right, "║" },
	{ Block::Left, "║" },

	// body
	{ Block::Untouched, " " },
	{ Block::Empty, "░" },
	{ Block::Shot, "▓" },
	{ Block::RecentlyShot, "█" },
	{ Block::Target, "+" }
};

// GAME SETTINGS
// field size
map<const Size, const unsigned short> fieldSize
{
	{ Size::Height, 12},
	{ Size::Width, 24}
};

// settings of ships
map<const unsigned short, const unsigned short> shipSettings
{
	// length of the ship - how many
	{ 5, 1 },
	{ 4, 1 },
	{ 3, 3 },
	{ 2, 3 }
};

// return what key on keyboard was pressed
Key onPressKey()
{
	char code = _getche();
	switch (code)
	{
	case 10: // enter for Linux
	case 13: // enter for Windows
		return Key::Enter;
		break;
	case 32:
		return Key::Space;
		break;
	case 'w':
	case 65: // arrow up
		return Key::Up;
		break;
	case 's':
	case 66: // arrow down
		return Key::Down;
		break;
	case 'd':
	case 67: // arrow right
		return Key::East;
		break;
	case 'a':
	case 68: // arrow left
		return Key::West;
		break;
	default:
		return Key::None;
		break;
	}
}

// classes
class Seaman
{
protected:
	Block** area;
	
	void createArea()
	{
		// Height - count of ROWS
		// Width - count of COLLUMNS
		area = new Block*[ fieldSize[Size::Height] ];
		for (unsigned short i = 0; i < fieldSize[Size::Height]; i++)
		{
			area[i] = new Block[ fieldSize[Size::Width] ];
			// fill up with untouched
			for (int j = 0; j < fieldSize[Size::Width]; j++)
				area[i][j] = Block::Untouched;
		}
	}

	// functions for getArea
	void printUpperEdge(string Title = "") const
	{
		unsigned short topsBeforeTitle, topsAfterTitle;
		if ((fieldSize[Size::Width] - Title.size()) % 2 == 0)
		{
			topsBeforeTitle = (fieldSize[Size::Width] - Title.size()) / 2;
			topsAfterTitle = topsBeforeTitle;
		}
		else
		{
			topsBeforeTitle = (fieldSize[Size::Width] - Title.size()) / 2;
			topsAfterTitle = topsBeforeTitle + 1;
		}

		// printing
		cout << field[Block::TopLeft];

		for (unsigned short i = 0; i < topsBeforeTitle; i++)
			cout << field[Block::Top];

		cout << Title;

		for (unsigned short i = 0; i < topsAfterTitle; i++)
			cout << field[Block::Top];

		cout << field[Block::TopRight] << endl;
	}
	void printBottomEdge() const
	{
		cout << field[Block::BottomLeft];

		for (unsigned short i = 0; i < fieldSize[Size::Width]; i++)
			cout << field[Block::Bottom];

		cout << field[Block::BottomRight] << endl;
	}

	// for setUpArea
	bool wharfShip(short row, short collumn, bool vertical, short length) // returns wheter ship was wharfed
	{
		if (vertical)
		{
			// checking
			if (
				// the top
				(area[row - 1][collumn] != Block::Untouched && row - 1 >= 0) || // top
				(area[row - 1][collumn - 1] != Block::Untouched && row - 1 >= 0 && collumn - 1 >= 0) || // top left
				(area[row - 1][collumn + 1] != Block::Untouched && row - 1 >= 0 && collumn + 1 < fieldSize[ Size::Width ]) || // top right
				(area[row][collumn - 1] != Block::Untouched && collumn - 1 >= 0) || // left
				(area[row][collumn + 1] != Block::Untouched && row - 1 >= 0 && collumn + 1 < fieldSize[ Size::Width ]) || // right

				// the bottom
				(area[row + length][collumn] != Block::Untouched && row + length < fieldSize[ Size::Height ]) || // bottom
				(area[row + length][collumn + 1] != Block::Untouched && row + length < fieldSize[ Size::Height ] && collumn + 1 < fieldSize[ Size::Width ]) || // bottom right
				(area[row + length][collumn - 1] != Block::Untouched && row + length < fieldSize[ Size::Height ] && collumn - 1 >= 0) || // bottom left
				(area[row + length - 1][collumn - 1] != Block::Untouched && collumn - 1 >= 0) || // left
				(area[row + length - 1][collumn + 1] != Block::Untouched && collumn + 1 < fieldSize[ Size::Width ]) // right
			)
				return false;
			// the body
			for (short i = row + 1; i < length - 1; i++)
				if (
					(area[i][collumn + 1] != Block::Untouched && collumn + 1 < fieldSize[ Size::Width ]) || // right
					(area[i][collumn - 1] != Block::Untouched && collumn - 1 >= 0) // left
				)
					return false;

			// wharf
			area[row][collumn] = Block::Top;
			area[row + length - 1][collumn] = Block::Bottom;
			for (short i = row + 1; i < i < length - 1; i++)
				area[i][collumn] = Block::VerticalBody;
			
			return true;
		}
		else
		{
			// checking
			if (
				// the left
				(area[row][collumn - 1] != Block::Untouched && collumn >= 0) || // left
				(area[row - 1][collumn - 1] != Block::Untouched && row - 1 >= 0 && collumn >= 0) || // left top
				(area[row + 1][collumn - 1] != Block::Untouched && row + 1 < fieldSize[ Size::Height ] && collumn >= 0) || // left bottom
				(area[row - 1][collumn] != Block::Untouched && row - 1 >= 0) || // top
				(area[row + 1][collumn] != Block::Untouched && row + 1 < fieldSize[ Size::Height ]) || // bottom

				// the right
				(area[row][collumn + length] != Block::Untouched && collumn + length < fieldSize[ Size::Width ]) || // right
				(area[row - 1][collumn + length] != Block::Untouched && row - 1 >= 0 && collumn + length < fieldSize[ Size::Width ]) || // right top
				(area[row + 1][collumn + length] != Block::Untouched && row + 1 < fieldSize[ Size::Height ] && collumn + length < fieldSize[ Size::Width ]) || // right bottom
				(area[row - 1][collumn + length - 1] != Block::Untouched && row - 1 >= 0) || // top
				(area[row + 1][collumn + length - 1] != Block::Untouched && row + 1 < fieldSize[ Size::Height ]) // bottom
			)
				return false;
			for (short j = collumn + 1; j < length - 1; j++)
				if (
					(area[row - 1][j] != Block::Untouched && row - 1 >= 0) || // top
					(area[row + 1][j] != Block::Untouched && row + 1 < fieldSize[ Size::Height ]) // bottom
				)
					return false;

			// wharf
			area[row][collumn] = Block::Left;
			area[row + length - 1][collumn] = Block::Right;
			for (short j = collumn + 1; j < length - 1; j++)
				area[row][j] = Block::HorizontalBody;

			return true;
		}
	}
};

class Player : public Seaman
{
public:
	Player()
	{
		createArea();
	}
	void getArea(/* ship settings while wharfing */ short row = -1, short collumn = -1, bool vertical = false, short length = -1) const
	{
		printUpperEdge("You");

		// mid
		for (unsigned short i = 0; i < fieldSize[Size::Height]; i++)
		{
			cout << field[Block::Left];

			// TODO ship whraf
			for (unsigned short j = 0; j < fieldSize[Size::Width]; j++)
				cout << field[area[i][j]];

			cout << field[Block::Right] << endl;
		}
		
		printBottomEdge();
	}
	void setUpArea()
	{
		//TODO
	}
};

class Enemy : public Seaman
{
public:
	Enemy()
	{
		createArea();
	}
	void getArea(/* coordinates of the target mark */ short row = -1, short collumn = -1) const
	{
		printUpperEdge("Enemy");

		// mid
		for (unsigned short i = 0; i < fieldSize[Size::Height]; i++)
		{
			cout << field[Block::Left];
			for (unsigned short j = 0; j < fieldSize[Size::Width]; j++)
			{
				if (row == i && collumn == j)
				{
					cout << field[Block::Target];
				}
				else
				{
					switch (area[i][j])
					{
					case Block::Empty:
					case Block::Shot:
					case Block::RecentlyShot:
						cout << field[area[i][j]];
						break;
					default:
						cout << field[Block::Untouched];
						break;
					}
				}
			}
			cout << field[Block::Right] << endl;
		}
		
		printBottomEdge();
	}
	void setUpArea()
	{
		// settings for each ship
		unsigned short row, collumn;
		bool vertical;
		
	}
};

// global methods
int main()
{
	srand(time(0));

	// set up
	Player player;
	Enemy enemy;
	// play
	enemy.getArea();
	player.getArea();
	// end


	return 0;
}
