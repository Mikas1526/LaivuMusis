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
	Shot, RecentlyShot, Empty, Untouched,
	Target,
	// ship
	HorizontalBody, VerticalBody, ShipTop, ShipBottom, ShipRight, ShipLeft
};
enum Size
{
	Width, Height
};

// DISPLAY SETTINGS
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

	// info
	{ Block::Untouched, " " },
	{ Block::Empty, "░" },
	{ Block::Shot, "▓" },
	{ Block::RecentlyShot, "█" },
	{ Block::Target, "+" },

	// ship
	{ Block::ShipTop, "╥" },
	{ Block::ShipBottom, "╨"},
	{ Block::ShipLeft, "╞" },
	{ Block::ShipRight, "╡" },
	{ Block::HorizontalBody, "-" },
	{ Block::VerticalBody, "║"}
};

bool showEnemeyShips = true;

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
				if (area[row][collumn + 1] != Block::Untouched)
					return false;

			// THE BOTTOM
			if (row + length - 1 < fieldSize[ Size::Height ])
			{
				if (area[row + length - 1][collumn] != Block::Untouched) // bottom
					return false;
				if (collumn + 1 < fieldSize[ Size::Width ])
					if (area[row + length - 1][collumn + 1] != Block::Untouched)// bottom right
						return false;
				if (collumn - 1 >= 0)
					if (area[row + length - 1][collumn - 1] != Block::Untouched) // bottom left
						return false;
			}
			if (collumn - 1 >= 0)
				if (area[row + length - 2][collumn - 1] != Block::Untouched) // left
					return false;
			if (collumn + 1 < fieldSize[ Size::Width ])
				if (area[row + length - 2][collumn + 1] != Block::Untouched) // right
					return false;
			
			// THE BODY
			for (short i = row + 1; i < length - 2; i++)
			{
				if (collumn + 1 < fieldSize[ Size::Width ])
					if (area[i][collumn + 1] != Block::Untouched) // right
						return false;
				if (collumn - 1 >= 0)
					if (area[i][collumn - 1] != Block::Untouched) // left
						return false;
			}

			// WHARF
			area[row][collumn] = Block::ShipTop;
			area[row + length - 2][collumn] = Block::ShipBottom;
			for (short i = row + 1; i < row + length - 2; i++)
				area[i][collumn] = Block::VerticalBody;
			
			return true;
		}
		else
		{
			// THE LEFT
			if (collumn - 1 >= 0)
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
			if (collumn + length - 1 < fieldSize[ Size::Width ])
			{
				if (area[row][collumn + length - 1] != Block::Untouched) // right
					return false;
				if (row - 1 >= 0)
					if (area[row - 1][collumn + length - 1] != Block::Untouched) // right top
						return false;
				if (row + 1 < fieldSize[ Size::Height ])
					if (area[row + 1][collumn + length - 1] != Block::Untouched) // right bottom
						return false;
			}
			if (row - 1 >= 0)
				if (area[row - 1][collumn + length - 2] != Block::Untouched)
					return false;
			if (row + 1 < fieldSize[ Size::Height ])
				if (area[row + 1][collumn + length - 2] != Block::Untouched)
					return false;

			// THE BODY
			for (short j = collumn + 1; j < length - 2; j++)
			{
				if (row - 1 >= 0)
					if (area[row - 1][j] != Block::Untouched) // top
						return false;
				if (row + 1 < fieldSize[ Size::Height ])
					if (area[row + 1][j] != Block::Untouched) // bottom
						return false;
			}

			// WHARF
			area[row][collumn] = Block::ShipLeft;
			area[row][collumn + length - 2] = Block::ShipRight;
			for (short j = collumn + 1; j < collumn + length - 2; j++)
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
		setUpArea();
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
								cout << field[Block::ShipTop];
							else if (S.row == i - S.length + 1)
								cout << field[Block::ShipBottom];
							else if (i > S.row && i < S.row + S.length - 1)
								cout << field[Block::VerticalBody];
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
								cout << field[Block::ShipLeft];
							else if (S.collumn == j - S.length + 1)
								cout << field[Block::ShipRight];
							else if (j > S.collumn && j < S.collumn + S.length - 1)
								cout << field[Block::HorizontalBody];
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

		Key action;
		while(shipS != shipSettings.end())
		{
			action = Key::None;

			if (howMany == 0)
			{
				howMany = shipS->second;
				S = shipPar(S.row, S.collumn, shipS->first, S.vertical);

				shipS++;
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
					else
					{
						howMany--;
						message = "";
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
		setUpArea();
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
				if (showEnemeyShips)
				{
					cout << field[area[i][j]];
				}
				else
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
				row = rand() % (fieldSize [ Size::Height ] - shipS->first);
				collumn = rand() % fieldSize[ Size::Width ];
			}
			else
			{
				row = rand() % fieldSize [ Size::Height ];
				collumn = rand() % (fieldSize[ Size::Width ] - shipS->first);
			}

			cout << row << " " << collumn << endl;
			// trying to wharf a ship
			if (wharfShip(row, collumn, vertical, shipS->first))
			{
				howMany--;
				cout << "paejo" << endl;
			}
			else
			{
				vertical != vertical;
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

	// play
	enemy.getArea();
	player.getArea();
	// end


	return 0;
}
