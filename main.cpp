#include <iostream>
#include <conio.h>

using namespace std;

const int sizeL = 12, sizeW = 12, keyError = -1, keyEnter = 0, keySpace = 1, keyUp = 2, keyDown = 3, keyRight = 4, keyLeft = 5;
char const empty = '.', hurt = 'X', miss = 'O', arrow = '+', shipStart = '<', shipBody = '=', shipEnd = '>';

// funkcijos, kurios visada reikalingos
int signal() // fiksuoja klaviatūros paspaudimus
{
    char code;
    code = _getche();
    if (code == 10) // for Linux
    //if (code == 13) // for Windows
        return keyEnter;
    else if (code == 32) // space
        return keySpace;
    else if (code == 119) // w
        return keyUp;
    else if (code == 115) // s
        return keyDown;
    else if (code == 100) // d
        return keyRight;
    else if (code == 97) // a
        return keyLeft;
    return keyError; // by default error
}
void hold(int which) // laukimo ekranas
{
    system("clear"); // for Linux
    //system("CLS"); // for Windows
    cout << endl << "Laukiama, kol žaidėjas Nr. " << which << endl << "paspaus bet kokį mygtuką";
    signal();
}
void printGround()
{
    cout << "+";
    for (int i = 0; i < sizeL; i++)
        cout << "-";
    cout << "+" << endl;
}

// funkcijos, kurios skirtos pasiruošimui 
void fillWithEmpty(char area[sizeL][sizeW])
{
    for (int i = 0; i < sizeL; i++)
        for (int j = 0; j < sizeW; j++)
            area[i][j] = empty;
}
void printSetUp(char area[sizeL][sizeW], int X, int Y, int length, bool vertical)
{
    system("clear"); // for Linux
    //system("CLS"); // for Windows
    // upper wall
    printGround();
    // body
    for (int i = 0; i < sizeL; i++)
    {
        cout << "|";
        for (int j = 0; j < sizeW; j++)
        {
            if (vertical) // vertikaliai spausdinamas
            {
                if (Y == j)
                {
                    if (X == i)
                    {
                        cout << shipStart;
                    }
                    else if (X == i - length + 1)
                    {
                        cout << shipEnd;
                    }
                    else if (i > X && i < X + length - 1)
                    {
                        cout << shipBody;
                    }
                    else
                    {
                        cout << area[i][j];
                    }
                }
                else
                {
                    cout << area[i][j];
                }
            }
            else // horizontaliai spausdinamas 
            {
                if (X == i)
                {
                    if (Y == j)
                    {
                        cout << shipStart;
                    }
                    else if (Y == j - length + 1)
                    {
                        cout << shipEnd;
                    }
                    else if (j > Y && j < Y + length - 1)
                    {
                        cout << shipBody;
                    }
                    else
                    {
                        cout << area[i][j];
                    }
                }
                else
                {
                    cout << area[i][j];
                }
            }
            
        }
        cout << "|" << endl;
    }
    // lower wall
    printGround();
}
int dockShip(char area[sizeL][sizeW], int X, int Y, int length, bool vertical) // iraso laivo padeti (grazina -1, jeigu nepaejo, 0, jeigu paejo)
{
    // TODO - patikrint, ar galima parkuoti laiva cia
    area[X][Y] = shipStart;
    if (vertical)
    {
        for (int i = X + 1; i < X + length - 1; i++)
        {
            area[i][Y] = shipBody;
        }
        area[X + length - 1][Y] = shipEnd;
    }
    else
    {
        for (int i = Y + 1; i < Y + length - 1; i++)
        {
            area[X][i] = shipBody;
        }
        area[X][Y + length - 1] = shipEnd;
    }
    return keyEnter;
}
void setUp(char area[sizeL][sizeW], int which)
{
    hold(which);
    int lengths[8] = {5, 4, 3, 3, 3, 2, 2, 2}; // nurodomi, kokio ilgio laivai turi būti padėti
    int X, Y; // koordinatės, kur bus rodomas laivas
    bool vertical; //ar vertikali laivo padėtis
    int action; // fiksuos norą kažką daryti
    
    for (unsigned short int i = 0; i < 8; i++)
    {
        // atstatau reiksmes
        action = keyError;
        X = 0;
        Y = 0;
        vertical = false;

        while (action != keyEnter)
        {
            printSetUp(area, X, Y, lengths[i], vertical);
            action = signal();
            if (action == keySpace)
            {
                vertical = !vertical;
            }
            else if (action == keyUp && X > 0)
            {
                X -= 1;
            }
            else if (action == keyDown && ((X + lengths[i] < sizeW && vertical) || (X < sizeW - 1 && !vertical)))
            {
                X += 1;
            }
            else if (action == keyRight && ((Y + lengths[i] < sizeL && !vertical) || (Y < sizeL - 1 && vertical)))
            {
                Y += 1;
            }
            else if (action == keyLeft && Y > 0)
            {
                Y -= 1;
            }
            else if (action == keyEnter)
            {
                dockShip(area, X, Y, lengths[i], vertical);
            }
        }
    }
}

// funkcijos, kurios reikalingos žaidimo eigai
void printInGame(char area[sizeL][sizeW], int X, int Y)
{
    printGround();
    for (int i = 0; i < sizeL; i++)
    {
        cout << '|';
        for (int j = 0; j < sizeW; j++)
        {
            if (i == X && j == Y) // spausdiname rodykle
            {
                cout << arrow;
            }
            else if (area[i][j] == shipStart || area[i][j] == shipEnd || area[i][j] == shipBody) // nespausdiname nepazeisto laivo
            {
                cout << empty;
            }
            else
            {
                cout << area[i][j];
            }
        }
        cout << '|' << endl;
    }
    printGround();
}
void chooseTarget(char area[sizeL][sizeW], int &points /* iš kurio atiminėsime taškus */, int shooter /* kas šaudo */)
{
    hold(shooter);

    int action = keyError;
    int X = 0, Y = 0;
    while (action != keyEnter)
    {
        printInGame(area, X, Y);
        signal();
        if (action == keyUp && X > 0)
        {
            X -= 1;
        }
        else if (action == keyDown && X < sizeW)
        {
            X += 1;
        }
        else if (action == keyRight && Y < sizeL)
        {
            Y += 1;
        }
        else if (action == keyLeft && Y > 0)
        {
            Y -= 1;
        }
        else if (action == keyEnter)
        {
            // TODO shoot
        }
    }
}
int main()
{
    char Player1[sizeL][sizeW], Player2[sizeL][sizeW];
    int Points1 = 24, Points2 = 24; // pradedama su 24 taškais
    
    // lentų užpildymas
    fillWithEmpty(Player1);
    //fillWithEmpty(Player2);

    // laivų sustatymas
    setUp(Player1, 1);
    //setUp(Player2, 2);
    
    return 0;
}