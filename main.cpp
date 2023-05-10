//             _____
//   /|       /     |
//  / |      /__    |
// | M|          |I |
// | i|          |T |
// | k|          |f |
// | a|          |2 |
//  \s|          |2/
//   \|__________|/
//    \Alisauskas/
//     \________/
// https://github.com/Mikas1526/LaivuMusis


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
	Enter, Space, Up, Down, East, West, Help, None
};
enum Block
{
	// edge
	TopLeft, Left, TopRight, Right, Top, BottomLeft, BottomRight, Bottom,
	// body
	Shot, RecentlyShot, Empty, Untouched,
	Target, Suggestion,
	// ship
	HorizontalBody, VerticalBody, ShipTop, ShipBottom, ShipRight, ShipLeft
};
enum Size
{
	Width, Height
};
enum Quarter
{
	I = 1, II = 2, III = 3, IV = 4
};
enum ShotResult
{
	AlreadyShot, Missed, Bullseye
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
	{ Block::Suggestion, "X" },

	// ship
	{ Block::ShipTop, "╥" },
	{ Block::ShipBottom, "╨"},
	{ Block::ShipLeft, "╞" },
	{ Block::ShipRight, "╡" },
	{ Block::HorizontalBody, "-" },
	{ Block::VerticalBody, "║"}
};
bool showEnemeyShips = false;
const short maxMessageShowTime = 5;

// GAME SETTINGS
// field size
map<const Size, const unsigned short> fieldSize
{
	{ Size::Height, 12 },
	{ Size::Width, 24 }
};

// settings of ships
map<const unsigned short, const unsigned short> shipSettings
{
	// length of the ship - how many
	{ 5, 1 },
	{ 4, 1 },
	{ 3, 3 },
	{ 2, 2 }
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
	case 'h':
		return Key::Help;
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

	unsigned short maxShipLenght = shipSettings.rbegin()->first;
	// can ships be fitted in field
	return (fieldSize[Size::Width] + 2) * (fieldSize[Size::Height] + 2) >= requiredBlocks &&
	(maxShipLenght <= fieldSize[Size::Height] || maxShipLenght <= fieldSize[Size::Width]);
}

// classes
class Seaman
{
protected:
	Block** area;

	string message;

	short points;

	// for shoot function recently shot coordinates
	short rRow, rCollumn;

	struct shipPar
	{
		short row, collumn, length;
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
		short topsBeforeTitle, topsAfterTitle;
		if (Title.size() > fieldSize[Size::Width])
		{
			Title = "";
			topsBeforeTitle = fieldSize[Size::Width];
		}
		else if ((fieldSize[Size::Width] - Title.size()) % 2 == 0)
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

	ShotResult shoot(short row, short collumn)
	{
		switch (area[row][collumn])
		{
		case Block::Empty:
		case Block::Shot:
		case Block::RecentlyShot:
			return ShotResult::AlreadyShot;
			break;
		case Block::Untouched:
			if (rRow != -1 && rCollumn != -1)
				area[rRow][rCollumn] = Block::Shot;
			area[row][collumn] = Block::Empty;

			return ShotResult::Missed;
			break;
		default:
			if (rRow != -1 && rCollumn != -1)
				area[rRow][rCollumn] = Block::Shot;
			area[row][collumn] = Block::RecentlyShot;

			rRow = row;
			rCollumn = collumn;

			points--;

			return ShotResult::Bullseye;
			break;
		}
	}

	virtual void setUpArea() = 0;
	virtual void getShot() = 0;
public:
	bool isBeaten()
	{
		return points == 0;
	}
};

class Player : public Seaman
{
private:
	// which quarter to shoot
	// II  | I
	// III | IV
	Quarter quarter;

	Quarter mostUntouchedQuarter()
	{
		unsigned short activity[4] = { 0, 0, 0, 0 };
		
		// counting activity
		for (unsigned short i = 0; i < fieldSize[Size::Height]; ++i)
		{
			for (unsigned short j = 0; j < fieldSize[Size::Width]; ++j)
			{
				switch (area[i][j])
				{
				case Block::Shot:
				case Block::RecentlyShot:
				case Block::Empty:
					// I quarter
					if (i < fieldSize[Size::Height] / 2 && j >= fieldSize[Size::Width] / 2)
						activity[0]++;
					// II
					else if (i < fieldSize[Size::Height] / 2 && j < fieldSize[Size::Width] / 2)
						activity[1]++;
					// III
					else if (i >= fieldSize[Size::Height] / 2 && j < fieldSize[Size::Width] / 2)
						activity[2]++;
					// IV
					else if (i >= fieldSize[Size::Height] / 2 && j >= fieldSize[Size::Width] / 2)
						activity[3]++;
					break;
				default:
					break;
				}
			}
		}

		// finding min
		unsigned short minn = activity[0], whitch = 0;
		for (unsigned short i = 0; i < 4; ++i)
		{
			if (activity[i] < minn)
			{
				minn = activity[i];
				whitch = i;
			}
		}
		return static_cast<Quarter>(whitch + 1);
	}
	/*
	struct shipHunter
	{
	private:
		unsigned short centerRow, centerCollumn; // first spot that was shot
		Key direction;
		bool firstEdgeHitted;
	unsigned short freeSpaceFromPoint(unsigned short startRow, unsigned short startCol, Key direction)
	{
		unsigned short freeSpace = 0;
		
		if (
			startRow < 0 || startRow >= fieldSize[Size::Height] ||
			startCol < 0 || startCol >= fieldSize[Size::Width]
			)
			return freeSpace;

		switch(direction)
		{
		case Key::Down:
			for (unsigned short i = startRow + 1; i < fieldSize[Size::Height]; ++i)
			{
				if (area[i][startCol] == Block::Untouched)
					freeSpace++;
				else
					return freeSpace;
			}
			return freeSpace;
			break;

		case Key::Right:
			for (unsigned short j = startCol + 1; j < fieldSize[Size::Width]; ++j)
			{
				if (area[startRow][j] == Block::Untouched)
					freeSpace++;
				else
					return freeSpace;
			}
			return freeSpace;
			break;

		case Key::Up:
			for (short i = startRow - 1; i >= 0; --i)
			{
				if (area[i][startCol] == Block::Untouched)
					freeSpace++;
				else
					return freeSpace;
			}
			return freeSpace;
			break;

		case Key::Left:
			for (short j = startCol - 1; j >= 0; --i)
			{
				if (area[startRow][j] == Block::Untouched)
					freeSpace++;
				else
					return freeSpace;
			}
			return freeSpace;
			break;

		default:
			return freeSpace;
			break;
		}
	}
	public:
		bool active;
		unsigned short nextRow, nextCollumn; // where to shoot
		shipHunter(unsigned short cRow, unsigned short cCol): firstEdgeHitted(false), active(true), centerRow(cRow), centerCollumn(cCol), nextRow(cRow), nextCollumn(cCol)
		{ 
			// searching for most likeable direction
			unsigned short maxx = 0;
			if (freeSpaceFromPoint(centerRow, centerCollumn, Key::Down) > maxx)
				direction = Key::Down;

			else if (freeSpaceFromPoint(centerRow, centerCollumn, Key::Right) > maxx)
				direction = Key::Right;

			else if (freeSpaceFromPoint(centerRow, centerCollumn, Key::Up) > maxx)
				direction = Key::Up;

			else if (freeSpaceFromPoint(centerRow, centerCollumn, Key::Left) > maxx)
				direction = Key::Left;

			else
				direction = Key::None;
		}
		void generateNextPoint(bool lastShotSuccess == true)
		{
			if (lastShotSuccess)
			{
				switch (direction)
				{
				case Key::Down:
					if (nextRow + 1 < fieldSize[Size::Height])
						nextRow++;
					else
						firstEdgeHitted = true;
					break;

				case Key::Up:
					if (nextRow - 1 >= 0)
						nextRow--;
					else
						
				}
			}
		}
	}
	unsigned short freeSpaceFromPoint(unsigned short startRow, unsigned short startCol, Key direction)
	{
		unsigned short freeSpace = 0;
		
		if (
			startRow < 0 || startRow >= fieldSize[Size::Height] ||
			startCol < 0 || startCol >= fieldSize[Size::Width]
			)
			return freeSpace;

		switch(direction)
		{
		case Key::Down:
			for (unsigned short i = startRow + 1; i < fieldSize[Size::Height]; ++i)
			{
				if (area[i][startCol] == Block::Untouched)
					freeSpace++;
				else
					return freeSpace;
			}
			return freeSpace;
			break;

		case Key::Right:
			for (unsigned short j = startCol + 1; j < fieldSize[Size::Width]; ++j)
			{
				if (area[startRow][j] == Block::Untouched)
					freeSpace++;
				else
					return freeSpace;
			}
			return freeSpace;
			break;

		case Key::Up:
			for (short i = startRow - 1; i >= 0; --i)
			{
				if (area[i][startCol] == Block::Untouched)
					freeSpace++;
				else
					return freeSpace;
			}
			return freeSpace;
			break;

		case Key::Left:
			for (short j = startCol - 1; j >= 0; --i)
			{
				if (area[startRow][j] == Block::Untouched)
					freeSpace++;
				else
					return freeSpace;
			}
			return freeSpace;
			break;

		default:
			return freeSpace;
			break;
		}
	}
	*/
	void getArea(/* ship settings while wharfing */ shipPar S = shipPar())
	{
		printUpperEdge("Your field");

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

		while(!(shipS == shipSettings.end() && howMany == 0))
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
public:
	Player()
	{
		createArea();
		setUpArea();
		setPoints();

		rRow = -1;
		rCollumn = -1;

		quarter = Quarter::I;
	}
	void getShot()
	{
		unsigned short row, collumn;
		ShotResult rez;
		do
		{

			// generate coordinates to shoot at
			switch (quarter)
			{
			case Quarter::I:
				row = rand() % (fieldSize[Size::Height] / 2);
				collumn = (rand() % (fieldSize[Size::Width] / 2)) + (fieldSize[Size::Width] / 2 + 0.5);
				break;
			case Quarter::II:
				row = rand() % (fieldSize[Size::Height] / 2);
				collumn = rand() % (fieldSize[Size::Width] / 2);
				break;
			case Quarter::III:
				row = (rand() % (fieldSize[Size::Height] / 2)) + (fieldSize[Size::Height] / 2 + 0.5);
				collumn = rand() % (fieldSize[Size::Width] / 2);
				break;
			case Quarter::IV:
				row = (rand() % (fieldSize[Size::Height] / 2)) + (fieldSize[Size::Height] / 2 + 0.5);
				collumn = (rand() % (fieldSize[Size::Width] / 2)) + (fieldSize[Size::Width] / 2 + 0.5);
				break;
			}

			// try to shoot
			rez = shoot(row, collumn);
			switch (rez)
			{
			case ShotResult::Missed:
				// change quarter
				if (rez == ShotResult::Missed)
					quarter = mostUntouchedQuarter();
				clearTerminal();
				message = "You're lucky!\nPress any key on keyboard to continue";
				getArea();
				onPressKey();
				break;
			case ShotResult::Bullseye:
				clearTerminal();
				message = "You've got shot!";
				
				if (!isBeaten())
				{
					rez = ShotResult::AlreadyShot;
					message = "Enemy is hitting you again!";
				}

				message += "\nPress any key on keyboard to continue\n";
				getArea();
				onPressKey();
				
				break;
			default:
				break;
			}
		} while (rez == ShotResult::AlreadyShot);
	}
};

class Enemy : public Seaman
{
private:
	bool helped;

	void findFirstUntouched(unsigned short &row, unsigned short &collumn)
	{
		for (int i = 0; i < fieldSize[Size::Height]; ++i)
		{
			for (int j = 0; j < fieldSize[Size::Width]; ++j)
			{
				switch (area[i][j])
				{
				case Block::Shot:
				case Block::RecentlyShot:
				case Block::Empty:
					break;
				default:
					row = i;
					collumn = j;
					return;
				}
			}
		}
	}
	void getArea(/* coordinates of the target mark */ short row = -1, short collumn = -1) const
	{
		printUpperEdge("Enemy's field");

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
					if (showEnemeyShips)
					{
						cout << field[area[i][j]];
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

		cout << message << endl;
	}
	void setUpArea()
	{
		shipPar S;

		// attempts to generate place for ship
		short attempts = 0;

		// going through shipSettings
		auto shipS = shipSettings.begin();
		unsigned short howMany = 0;

		while (!(shipS == shipSettings.end() && howMany == 0))
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
public:
	Enemy()
	{
		createArea();
		setUpArea();
		setPoints();

		rRow = -1;
		rCollumn = -1;

		helped = false;
	}
	void getShot()
	{
		// where is the target
		unsigned short row, collumn;
		Key action = Key::None;
		message = "";
		
		findFirstUntouched(row, collumn);
		while (action != Key::Enter)
		{
			clearTerminal();
			getArea(row, collumn);

			action = onPressKey();
			
			if (action == Key::Up && row > 0)
				row--;

			else if (action == Key::Down && row + 1 < fieldSize[Size::Height])
				row++;

			else if (action == Key::East && collumn + 1 < fieldSize[Size::Width])
				collumn++;

			else if (action == Key::West && collumn > 0)
				collumn--;

			else if (action == Key::Enter)
			{
				switch (shoot(row, collumn))
				{
				case ShotResult::Missed:
					message = "You missed";
					break;
				case ShotResult::Bullseye:
					message = "Nice shot!";
					if (!isBeaten())
					{
						message += "\nYou shall shoot again!";
						action = Key::None;
					}
					break;
				default:
					message = "You have already shot here";
					action = Key::None;
					break;
				}
			}

			else if (action == Key::Help && !helped)
			{
				bool found = false;
				for (unsigned short i = 0; i < fieldSize[Size::Height]; ++i)
				{
					for (unsigned short j = 0; j < fieldSize[Size::Width]; ++j)
					{
						switch (area[i][j])
						{
						case Block::HorizontalBody:
						case Block::VerticalBody:
						case Block::ShipTop:
						case Block::ShipBottom:
						case Block::ShipRight:
						case Block::ShipLeft:
							area[i][j] = Block::Suggestion;
							found = true;
							break;
						}
						if (found)
							break;
					}
					if (found)
						break;
				}
				helped = true;
			}
		}
		// show the result
		clearTerminal();
		message += "\nPress any key on keyboard to continue";
		getArea();
		onPressKey();
	}
};

// global methods
int main()
{
	if (isItPlayable())
	{
		srand(time(NULL));

		// set up
		Player player;
		Enemy enemy;

		// play
		while (!(player.isBeaten() || enemy.isBeaten()))
		{
			if (!player.isBeaten())
				enemy.getShot();
			if (!enemy.isBeaten())
				player.getShot();
		}
		
		// end
		if (player.isBeaten())
		{
			cout << "Better luck next time!" << endl;
		}
		else
		{
			cout << "Congrats!" << endl;
		}
	}
	else
	{
		cout << "Field is too small for ships" << endl;
	}
	return 0;
}
