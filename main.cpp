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
const short maxMessageShowTime = 5;

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

// clears the terminal
void clearTerminal()
{
	system("clear");
}

bool isItPlayable()
{
	// how many blocks are required
	short requiredBlocks = 0;
	for (auto shipS = shipSettings.begin(); shipS != shipSettings.end(); shipS++)
	{
		requiredBlocks += shipS->first * shipS->second;
	}
	requiredBlocks *= 3;
	requiredBlocks += 6;

	// can ships be fitted in field
	return (fieldSize[Size::Width] + 1) * (fieldSize[Size::Height] + 1) >= requiredBlocks;
}

// classes
class Seaman
{
protected:
	Block** area;
	short points;

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

	void setPoints()
	{
		points = 0;
		for (auto shipS = shipSettings.begin(); shipS != shipSettings.end(); shipS++)
		{
			points += shipS->first * shipS->second;
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
	bool wharfShip(shipPar S) // returns wheter ship was wharfed
	{
		// ship on ship
		if (area[S.row][S.collumn] != Block::Untouched)
			return false;

		if (S.vertical)
		{
			// THE TOP
			if (S.row - 1 >= 0)
			{
				if (area[S.row - 1][S.collumn] != Block::Untouched) // top
					return false;
				if (S.collumn - 1 >= 0)
					if (area[S.row - 1][S.collumn - 1] != Block::Untouched) // top left
						return false;
				if (S.collumn + 1 < fieldSize[ Size::Width ])
					if (area[S.row - 1][S.collumn + 1] != Block::Untouched) // top right
						return false;
			}
			if (S.collumn - 1 >= 0)
				if (area[S.row][S.collumn - 1] != Block::Untouched) // left
					return false;
			if (S.collumn + 1 < fieldSize[ Size::Width ]) // right
				if (area[S.row][S.collumn + 1] != Block::Untouched)
					return false;

			// THE BOTTOM
			// ship on ship
			if (area[S.row + S.length - 1][S.collumn] != Block::Untouched)
				return false;

			if (S.row + S.length < fieldSize[ Size::Height ])
			{
				if (area[S.row + S.length][S.collumn] != Block::Untouched) // bottom
					return false;
				if (S.collumn + 1 < fieldSize[ Size::Width ])
					if (area[S.row + S.length][S.collumn + 1] != Block::Untouched)// bottom right
						return false;
				if (S.collumn - 1 >= 0)
					if (area[S.row + S.length][S.collumn - 1] != Block::Untouched) // bottom left
						return false;
			}
			if (S.collumn - 1 >= 0)
				if (area[S.row + S.length - 1][S.collumn - 1] != Block::Untouched) // left
					return false;
			if (S.collumn + 1 < fieldSize[ Size::Width ])
				if (area[S.row + S.length - 1][S.collumn + 1] != Block::Untouched) // right
					return false;
			
			// THE BODY
			for (short i = S.row + 1; i < S.row + S.length - 1; i++)
			{
				if (S.collumn + 1 < fieldSize[ Size::Width ])
					if (area[i][S.collumn + 1] != Block::Untouched) // right
						return false;
				if (S.collumn - 1 >= 0)
					if (area[i][S.collumn - 1] != Block::Untouched) // left
						return false;
				// ship on ship
				if (area[i][S.collumn] != Block::Untouched)
					return false;
			}

			// WHARF
			area[S.row][S.collumn] = Block::ShipTop;
			area[S.row + S.length - 1][S.collumn] = Block::ShipBottom;
			for (short i = S.row + 1; i < S.row + S.length - 1; i++)
				area[i][S.collumn] = Block::VerticalBody;
			
			return true;
		}
		else
		{
			// THE LEFT
			if (S.collumn - 1 >= 0)
			{
				if (area[S.row][S.collumn - 1] != Block::Untouched) // left
					return false;
				if (S.row - 1 >= 0)
					if (area[S.row - 1][S.collumn - 1] != Block::Untouched) // left top
						return false;
				if (S.row + 1 < fieldSize[ Size::Height ])
					if (area[S.row + 1][S.collumn - 1] != Block::Untouched) // left bottom
						return false;
			}
			if (S.row - 1 >= 0)
				if (area[S.row - 1][S.collumn] != Block::Untouched) // top
					return false;
			if (S.row + 1 < fieldSize[ Size::Height ])
				if (area[S.row + 1][S.collumn] != Block::Untouched) // bottom
					return false;

			// THE RIGHT
			// ship on ship
			if (area[S.row][S.collumn + S.length - 1] != Block::Untouched)
				return false;

			if (S.collumn + S.length < fieldSize[ Size::Width ])
			{
				if (area[S.row][S.collumn + S.length] != Block::Untouched) // right
					return false;
				if (S.row - 1 >= 0)
					if (area[S.row - 1][S.collumn + S.length] != Block::Untouched) // right top
						return false;
				if (S.row + 1 < fieldSize[ Size::Height ])
					if (area[S.row + 1][S.collumn + S.length] != Block::Untouched) // right bottom
						return false;
			}
			if (S.row - 1 >= 0)
				if (area[S.row - 1][S.collumn + S.length - 1] != Block::Untouched)
					return false;
			if (S.row + 1 < fieldSize[ Size::Height ])
				if (area[S.row + 1][S.collumn + S.length - 1] != Block::Untouched)
					return false;

			// THE BODY
			for (short j = S.collumn + 1; j < S.collumn + S.length - 1; j++)
			{
				if (S.row - 1 >= 0)
					if (area[S.row - 1][j] != Block::Untouched) // top
						return false;
				if (S.row + 1 < fieldSize[ Size::Height ])
					if (area[S.row + 1][j] != Block::Untouched) // bottom
						return false;
				// ship on ship
				if (area[S.row][j] != Block::Untouched)
					return false;
			}

			// WHARF
			area[S.row][S.collumn] = Block::ShipLeft;
			area[S.row][S.collumn + S.length - 1] = Block::ShipRight;
			for (short j = S.collumn + 1; j < S.collumn + S.length - 1; j++)
				area[S.row][j] = Block::HorizontalBody;

			return true;
		}
	}
public:
	bool isBeaten()
	{
		return points == 0;
	}
};

class Player : public Seaman
{
private:
	string message;
public:
	Player()
	{
		createArea();
		setUpArea();
		setPoints();
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

		// message dissapear counter
		short timeShowed = 0;

		while(shipS != shipSettings.end())
		{
			action = Key::None;

			if (howMany == 0)
			{
				// setting variables up
				howMany = shipS->second;
				S = shipPar(S.row, S.collumn, shipS->first, S.vertical);
				
				// checking, if the ship is out of the bounds
				if (S.vertical && S.row > fieldSize[Size::Height] - S.length - 1)
				{
					S.row = fieldSize[Size::Height] - S.length;
				}
				else if (!S.vertical && S.collumn > fieldSize[Size::Width] - S.length - 1)
				{
					S.collumn = fieldSize[Size::Width] - S.length;
				}

				// preparing the other ship type
				shipS++;
			}

			while (action != Key::Enter)
			{
				clearTerminal();
				
				if (timeShowed == maxMessageShowTime)
					message = "";

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
					timeShowed++;
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
					timeShowed++;
				}
				else if (action == Key::West && S.collumn > 0)
				{
					S.collumn -= 1;
					timeShowed++;
				}
				else if (action == Key::Up && S.row > 0)
				{
					S.row -= 1;
					timeShowed++;
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
					timeShowed++;
				}
				else if (action == Key::Enter)
				{
					if (!wharfShip(S))
					{
						action = Key::None;
						message = "You cannot wharf your ship here";
					}
					else
					{
						howMany--;
						message = "Ship has been wharfed";
					}
					timeShowed = 0;
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
		setPoints();
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
	void setUpArea()
	{
		shipPar S;

		// attempts to generate place for ship
		short attempts = 0;

		// going through shipSettings
		auto shipS = shipSettings.begin();
		unsigned short howMany = 0;

		while (shipS != shipSettings.end())
		{
			if (howMany == 0)
			{
				// setting variables up
				howMany = shipS->second;
				S = shipPar(shipS->first);

				shipS++;
			}
			// generating if the ship is vertical
			S.vertical = rand() % 10 % 2 == 0;
			// generating numbers
			if (S.vertical)
			{
				S.row = rand() % (fieldSize [ Size::Height ] - S.length);
				S.collumn = rand() % fieldSize[ Size::Width ];
			}
			else
			{
				S.row = rand() % fieldSize [ Size::Height ];
				S.collumn = rand() % (fieldSize[ Size::Width ] - S.length);
			}
			// trying to wharf a ship
			if (wharfShip(S))
			{
				howMany--;
				attempts = 0;
			}
			else
			{
				attempts++;
				if (attempts >= 100)
				{
					createArea();
					setUpArea();
				}
			}
		}
	}
};

// global methods
int main()
{
	if (isItPlayable())
	{
		srand(time(0));

		// set up
		Player player;
		Enemy enemy;

		// play
		while (!(player.isBeaten() || enemy.isBeaten()))
		{
			enemy.getArea();
			player.getArea();
		}
		
		// end
	}
	else
	{
		cout << "Field is too small for ships" << endl;
	}
	return 0;
}
