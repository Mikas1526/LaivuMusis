#include <iostream>
#include <conio.h>
#include <string>
#include <map>

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
	Shot, RecentlyShot, Empty, Untouched
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
	{ Block::RecentlyShot, "█" }
};

// GAME SETTINGS
// field size
map<const Size, const unsigned short> fieldSize
{
	{ Size::Height, 12},
	{ Size::Width, 24}
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

void printField()
{
	// upper edge
	cout << field[Block::TopLeft];
	for (unsigned short i = 0; i < fieldSize[Size::Width]; i++)
		cout << field[Block::Top];
	cout << field[Block::TopRight] << endl;

	// mid
	for (unsigned short i = 0; i < fieldSize[Size::Height]; i++)
	{
		cout << field[Block::Left];
		for (unsigned short i = 0; i < fieldSize[Size::Width]; i++)
		{
			// TODO
			cout << field[Block::Untouched];
		}
		cout << field[Block::Right] << endl;
	}
	
	// bottom edge
	cout << field[Block::BottomLeft];
	for (unsigned short i = 0; i < fieldSize[Size::Width]; i++)
		cout << field[Block::Bottom];
	cout << field[Block::BottomRight] << endl;
}
int main()
{
	// set up ships
	printField();
	// play

	// end


	return 0;
}
