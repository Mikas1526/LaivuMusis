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
	{ 5, 1 }/*,
	{ 4, 1 },
	{ 3, 3 },
	{ 2, 3 }*/
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
	string message = "";

	struct shipPar
	{
		unsigned short row, collumn, length;
		bool vertical;
		shipPar(): row(0), collumn(0), length(0), vertical(false) { }
		shipPar(unsigned short _l): row(0), collumn(0), length(_l), vertical(false) { }
		shipPar(unsigned short _r, unsigned short _c, unsigned short _l, bool _v): row(_r), collumn(_c), length(_l), vertical(_v) { }
	};
	
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
			// THE TOP
			if (row - 1 >= 0)
			{
				if (area[row - 1][collumn] != Block::Untouched) // top
					return false;
				if (collumn - 1 >= 0)
					if (area[row - 1][collumn - 1] != Block::Untouched) // top left
						return false;
				if (collumn + 1 < fieldSize[ Size::Width ])
					if (area[row - 1][collumn + 1] != Block::Untouched) // top right
						return false;
			}
			if (collumn - 1 >= 0)
				if (area[row][collumn - 1] != Block::Untouched) // left
					return false;
			if (collumn + 1 < fieldSize[ Size::Width ]) // right
				if (area[row][collumn + 1])
					return false;

			// THE BOTTOM
			if (row + length < fieldSize[ Size::Height ])
			{
				if (area[row + length][collumn] != Block::Untouched) // bottom
					return false;
				if (collumn + 1 < fieldSize[ Size::Width ])
					if (area[row + length][collumn + 1])// bottom right
						return false;
				if (collumn - 1 >= 0)
					if (area[row + length][collumn - 1] != Block::Untouched) // bottom left
						return false;
			}
			if (collumn - 1 >= 0)
				if (area[row + length - 1][collumn - 1] != Block::Untouched) // left
					return false;
			if (collumn + 1 < fieldSize[ Size::Width ])
				if (area[row + length - 1][collumn + 1] != Block::Untouched) // right
					return false;
			
			// THE BODY
			for (short i = row + 1; i < length - 1; i++)
			{
				if (collumn + 1 < fieldSize[ Size::Width ])
					if (area[i][collumn + 1] != Block::Untouched) // right
						return false;
				if (collumn - 1 >= 0)
					if (area[i][collumn - 1] != Block::Untouched) // left
						return false;
			}

			// WHARF
			area[row][collumn] = Block::Top;
			area[row + length - 1][collumn] = Block::Bottom;
			for (short i = row + 1; i < i < length - 1; i++)
				area[i][collumn] = Block::VerticalBody;
			
			return true;
		}
		else
		{
			// THE LEFT
			if (collumn >= 0)
			{
				if (area[row][collumn - 1] != Block::Untouched) // left
					return false;
				if (row - 1 >= 0)
					if (area[row - 1][collumn - 1] != Block::Untouched) // left top
						return false;
				if (row + 1 < fieldSize[ Size::Height ])
					if (area[row + 1][collumn - 1] != Block::Untouched) // left bottom
						return false;
			}
			if (row - 1 >= 0)
				if (area[row - 1][collumn] != Block::Untouched) // top
					return false;
			if (row + 1 < fieldSize[ Size::Height ])
				if (area[row + 1][collumn] != Block::Untouched) // bottom
					return false;

			// THE RIGHT
			if (collumn + length < fieldSize[ Size::Width ])
			{
				if (area[row][collumn + length] != Block::Untouched) // right
					return false;
				if (row - 1 >= 0)
					if (area[row - 1][collumn + length] != Block::Untouched) // right top
						return false;
				if (row + 1 < fieldSize[ Size::Height ])
					if (area[row + 1][collumn + length] != Block::Untouched) // right bottom
						return false;
			}
			if (row - 1 >= 0)
				if (area[row - 1][collumn + length - 1])
					return false;
			if (row + 1 < fieldSize[ Size::Height ])
				if (area[row + 1][collumn + length - 1])
					return false;

			// THE BODY
			for (short j = collumn + 1; j < length - 1; j++)
			{
				if (row - 1 >= 0)
					if (area[row - 1][j] != Block::Untouched) // top
						return false;
				if (row + 1 < fieldSize[ Size::Height ])
					if (area[row + 1][j] != Block::Untouched) // bottom
						return false;
			}

			// WHARF
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
	void getArea(/* ship settings while wharfing */ shipPar S = shipPar())
	{
		printUpperEdge("You");

		// mid
		for (unsigned short i = 0; i < fieldSize[Size::Height]; i++)
		{
			cout << field[Block::Left];

			for (unsigned short j = 0; j < fieldSize[Size::Width]; j++)
			{
				if (S.length == 0)
					cout << field[area[i][j]];
				else
				{
					if (S.vertical)
					{
						if (S.collumn == j)
						{
							if (S.row == i)
								cout << ship[Block::Top];
							else if (S.row == i - S.length + 1)
								cout << ship[Block::Bottom];
							else if (i > S.row && i < S.row + S.length - 1)
								cout << ship[Block::VerticalBody];
							else
								cout << field[area[i][j]];
						}
						else
							cout << field[area[i][j]];
					}
					else
					{
						if (S.row == i)
						{
							if (S.collumn == j)
								cout << ship[Block::Left];
							else if (S.collumn == j - S.length + 1)
								cout << ship[Block::Right];
							else if (j > S.collumn && j < S.collumn + S.length - 1)
								cout << ship[Block::HorizontalBody];
							else
								cout << field[area[i][j]];
						}
						else
							cout << field[area[i][j]];
					}
				}
			}
			cout << field[Block::Right] << endl;
		}
		
		printBottomEdge();

		cout << message << endl;
	}
	void setUpArea()
	{
		shipPar S;

		// going through ship settings
		auto shipS = shipSettings.begin();
		unsigned short howMany = 0;

		Key action = Key::None;
		while(shipS != shipSettings.end())
		{
			if (howMany == 0)
			{
				howMany = shipS->second;
				S = shipPar(shipS->first);
			}
			while (action != Key::Enter)
			{
				system("clear");
				getArea(S);

				action = onPressKey();
				// Key Left
				if (
					action == Key::East && (
						(S.vertical && S.collumn + 1 < fieldSize[Size::Width]) ||
						(!S.vertical && S.collumn + S.length < fieldSize[Size::Width])
					)
				)
				{
					S.collumn += 1;
				}
				// Key Down
				else if (
					action == Key::Down && (
						(!S.vertical && S.row + 1 < fieldSize[Size::Height]) ||
						(S.vertical && S.row + S.length < fieldSize[Size::Height])
					)
				)
				{
					S.row += 1;
				}
				else if (action == Key::West && S.collumn > 0)
				{
					S.collumn -= 1;
				}
				else if (action == Key::Up && S.row > 0)
				{
					S.row -= 1;
				}
				else if (action == Key::Space)
				{
					if (S.vertical && S.collumn > fieldSize[Size::Width] - S.length - 1)
					{
						S.collumn = fieldSize[Size::Width] - S.length;
					}
					else if (!S.vertical && S.row > fieldSize[Size::Height] - S.length - 1)
					{
						S.row = fieldSize[Size::Height] - S.length;
					}
					S.vertical = !S.vertical;
				}
				else if (action == Key::Enter)
				{
					if (!wharfShip(S.row, S.collumn, S.vertical, shipS->first))
					{
						action = Key::None;
						message = "You cannot wharf your ship here";
					}
				}
			}
		}
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
				/*if (row == i && collumn == j)
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
				}*/
				cout << field[area[i][j]];
			}
			cout << field[Block::Right] << endl;
		}
		
		printBottomEdge();
	}
	void setUpArea() // TODO
	{
		// settings for each ship
		unsigned short row, collumn;
		bool vertical = true;

		// going through shipSettings
		auto shipS = shipSettings.begin();
		unsigned short howMany = 0;
		while (shipS != shipSettings.end())
		{
			if (howMany == 0)
				howMany = shipS->second;

			// generating numbers
			if (vertical)
			{
				row = rand() % fieldSize [ Size::Height ] - shipS->first + 1;
				collumn = rand() % fieldSize[ Size::Width ];
			}
			else
			{
				row = rand() % fieldSize [ Size::Height ];
				collumn = rand() % fieldSize[ Size::Width ] - shipS->first + 1;
			}

			// trying to wharf a ship
			if (wharfShip(row, collumn, vertical, shipS->first))
			{
				howMany--;
				cout << "paejo" << endl;
			}

			if (howMany == 0)
				shipS++;
		}
	}
};

// global methods
int main()
{
	srand(time(0));

	// set up
	Player player;
	Enemy enemy;
	player.setUpArea();
	//enemy.setUpArea();

	// play
	//enemy.getArea();
	//player.getArea();
	// end


	return 0;
}
