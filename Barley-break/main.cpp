#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>

using namespace sf;
using namespace std;
enum Mode
{
    Classic,
    Puzzle
};
#pragma region Signatures
void GameGenerating(int, RenderWindow&);
void SquareMove(int, int);

bool SquareExist(int, int, int);
#pragma endregion
#pragma region Glob variables 

const int x = 0,
          y = 1;
bool win = false;
int sixteen[2] = { 3, 3 };


VideoMode windowSize(905, 905);
VideoMode gameWindowSize(905, 905);
vector<vector<RectangleShape>> square(3, vector<RectangleShape>(3));
vector<pair<int, RectangleShape>> mainMenuSquareVisual; // pair square page + square
vector<Texture> images;
int mainMenuPages = 0;
int leaderBoardPages = 0;
Sound swapSound;

int imagesAmmount = 0;
int randomCoeficient;
vector<int> notExistSquare(2, 0);
int currentImageNumber;
bool selectMenuVisible = true;
bool helpMenuVisible = false;
int mode = 0;
int dificulty = 3;
int swaps = 0;
int imageElement;
string playerNameEnter = "";
bool dificultySelect = false;
bool classicMode;
bool musicPlay = true;
vector<tuple<string, int, int, int, int, int>> playerParametrs; // pair(player name, playerid->player parametrs: 0 - time, 1 - swaps, 2 - dificulty, 3 - score, 4 - page) 
vector<string> leaderBoardParametrs(5, "");
#pragma endregion
#pragma region Compare Methods

bool leader_board_time_max_compare(const std::tuple <string, int, int, int, int, int>& p1, const std::tuple<string, int, int, int, int, int>& p2) { return get<1>(p1) > get<1>(p2); } // sort time max -> time min
bool leader_board_swaps_max_compare(const std::tuple <string, int, int, int, int, int>& p1, const std::tuple<string, int, int, int, int, int>& p2) { return get<2>(p1) > get<2>(p2); } // sort swaps max -> swaps min
bool leader_board_score_max_compare(const std::tuple <string, int, int, int, int, int>& p1, const std::tuple<string, int, int, int, int, int>& p2) { return get<4>(p1) > get<4>(p2); } // sort score max -> score min

bool leader_board_time_min_compare(const std::tuple <string, int, int, int, int, int>& p1, const std::tuple<string, int, int, int, int, int>& p2) { return get<1>(p1) < get<1>(p2); } // sort time min -> time max
bool leader_board_swaps_min_compare(const std::tuple <string, int, int, int, int, int>& p1, const std::tuple<string, int, int, int, int, int>& p2) { return get<2>(p1) < get<2>(p2); } // sort swaps min -> swaps max
bool leader_board_score_min_compare(const std::tuple <string, int, int, int, int, int>& p1, const std::tuple<string, int, int, int, int, int>& p2) { return get<4>(p1) < get<4>(p2); } // sort score min -> time max
#pragma endregion
#pragma region Main methods
bool CheckPosition(RectangleShape square, Mouse mouse, RenderWindow& localWindow) // check mouse position relatively rectangle.
{
    if (square.getScale().x > 0)
    {
        if (square.getScale().y > 0)
        {
            return (mouse.getPosition(localWindow).x >= -square.getOrigin().x + square.getPosition().x &&
                    mouse.getPosition(localWindow).x <= -square.getOrigin().x + square.getPosition().x + (square.getSize().x * square.getScale().x))
                    &&
                   (mouse.getPosition(localWindow).y >= -square.getOrigin().y + square.getPosition().y &&
                    mouse.getPosition(localWindow).y <= -square.getOrigin().y + square.getPosition().y + (square.getSize().y * square.getScale().y))
        }
        else if (square.getScale().y < 0)
        {
            if ((mouse.getPosition(localWindow).x >= -square.getOrigin().x + square.getPosition().x &&
                 mouse.getPosition(localWindow).x <= -square.getOrigin().x + square.getPosition().x + (square.getSize().x * square.getScale().x))
                 &&
                (mouse.getPosition(localWindow).y >= -square.getOrigin().y + square.getPosition().y + (square.getSize().y * square.getScale().y) &&
                 mouse.getPosition(localWindow).y <= -square.getOrigin().y + square.getPosition().y))
                return true;
        }
    }
    else if (square.getScale().x < 0)
    {
        if (square.getScale().y > 0)
        {
            if ((mouse.getPosition(localWindow).x >= -square.getOrigin().x + square.getPosition().x + (square.getSize().x * square.getScale().x) &&
                mouse.getPosition(localWindow).x <= -square.getOrigin().x + square.getPosition().x)
                &&
                (mouse.getPosition(localWindow).y >= -square.getOrigin().y + square.getPosition().y &&
                 mouse.getPosition(localWindow).y <= -square.getOrigin().y + square.getPosition().y + (square.getSize().y * square.getScale().y)))
                return true;
        }
        else if (square.getScale().y < 0)
        {
            if ((mouse.getPosition(localWindow).x >= -square.getOrigin().x + square.getPosition().x + (square.getSize().x * square.getScale().x) &&
                 mouse.getPosition(localWindow).x <= -square.getOrigin().x + square.getPosition().x)
                 &&
                (mouse.getPosition(localWindow).y >= -square.getOrigin().y + square.getPosition().y + (square.getSize().y * square.getScale().y) &&
                 mouse.getPosition(localWindow).y <= -square.getOrigin().y + square.getPosition().y))
                return true;
        }
    }


    return false;
}

void MainMenuCreate() // create puuzle mode menu. Get images from texture file & creates masks to display them
{
    struct _finddata_t png_file;
    intptr_t hFile;
    int filesCounter = 0;
    int currentPage = 0;
    imagesAmmount = 0;
    string fullPath;
    vector<int> positionOffset(2, 0);
    positionOffset[y] = 1;
    images.clear();
    mainMenuSquareVisual.clear();

    // Find first .png file in current directory
    if ((hFile = _findfirst("Images\\*.png", &png_file)) == -1L)
        printf("Error! No *.png files in current directory!\n");
    else
    {
        do 
        {         
            imagesAmmount++;
            images.resize(imagesAmmount);

            fullPath = png_file.name;
            fullPath = "Images\\" + fullPath;
            images[filesCounter].loadFromFile(fullPath);
            filesCounter++;
            cout << "full path: " << fullPath << endl;
        } while (_findnext(hFile, &png_file) == 0);

        cout << "Images ammount: " << imagesAmmount << endl;
        mainMenuSquareVisual.resize(imagesAmmount);

        for(filesCounter = 0; filesCounter < mainMenuSquareVisual.size(); filesCounter++)
        {
            if (positionOffset[x] == 3)
            {
                positionOffset[x] = 0;
                positionOffset[y]++;
                if (positionOffset[y] == 3)
                {
                    positionOffset[y] = 0;
                    currentPage++;
                }
            }
            mainMenuSquareVisual[filesCounter].second.setTexture(&images[filesCounter]);
            mainMenuSquareVisual[filesCounter].second.setOrigin(-(positionOffset[x] * 305), -(positionOffset[y] * 305));
            mainMenuSquareVisual[filesCounter].second.setSize(Vector2f(300, 300));
            mainMenuSquareVisual[filesCounter].first = currentPage;
            positionOffset[x]++;
        }
        _findclose(hFile);
    }
    mainMenuPages = currentPage;
}
void GameGenerating(int imageElement, RenderWindow& localWindow) // generate puzzle mode gamefield
{
    int yCord, xCord, i;
    swaps = 0;
    int moves = 0;
    win = false;
    playerNameEnter = "";
    square.resize(dificulty);
    for (yCord = 0; yCord < square.size(); yCord++)
        square[yCord].resize(dificulty);
    vector<float> textureSizeModyfier(2);
    textureSizeModyfier[x] = images[imageElement].getSize().x / static_cast<float>(900);
    textureSizeModyfier[y] = images[imageElement].getSize().y / static_cast<float>(900);

    for (yCord = 0; yCord < square.size(); yCord++)
        for (xCord = 0; xCord < square[yCord].size(); xCord++)
        {
            square[yCord][xCord].setSize(Vector2f(floor(900 / dificulty), floor(900 / dificulty)));
            square[yCord][xCord].setTexture(&images[imageElement]);
            square[yCord][xCord].setTextureRect(IntRect(900 / dificulty * xCord * textureSizeModyfier[x], 900 / dificulty * yCord * textureSizeModyfier[y], 900 / dificulty * textureSizeModyfier[x], 900 / dificulty * textureSizeModyfier[y]));
            square[yCord][xCord].setOrigin(-floor(static_cast<float>(900) / dificulty + 5) * xCord, -floor(static_cast<float>(900) / dificulty + 5) * yCord);
            square[yCord][xCord].setPosition(0, 0);
        }

    notExistSquare[x] = rand() % dificulty;
    notExistSquare[y] = rand() % dificulty;
    randomCoeficient = 5 + rand() % 100 * dificulty + 20 * (dificulty - 1);

    square[notExistSquare[y]][notExistSquare[x]].setPosition(-1000, -1000);


    while (moves < randomCoeficient)
    {
        for (i = 0; i < randomCoeficient; i++)
            for (yCord = 0; yCord < square.size(); yCord++)
                for (xCord = 0; xCord < square[yCord].size(); xCord++)
                {
                    int move = rand() % 100;
                    int positionBufer[2] = { square[yCord][xCord].getPosition().x, square[yCord][xCord].getPosition().y };
                    if (move >= 30)
                        SquareMove(yCord, xCord);
                   if (square[yCord][xCord].getPosition().x != positionBufer[x] || square[yCord][xCord].getPosition().y != positionBufer[y])
                        moves++;
                }
    }
}
void GameGenerating(Texture &texture, RenderWindow& localWindow, int notExistSquare[2]) // generate classic mode gamefield
{
    int yCord, xCord, i;
    int moves = 0;
    win = false;
    square.resize(dificulty);
    for (yCord = 0; yCord < square.size(); yCord++)
        square[yCord].resize(dificulty);
    float textureSizeModyfier[2] = { texture.getSize().x / ((900 / dificulty) * dificulty), texture.getSize().y / ((900 / dificulty) * dificulty) };
    for (yCord = 0; yCord < square.size(); yCord++)
        for (xCord = 0; xCord < square[yCord].size(); xCord++)
        {
            square[yCord][xCord].setSize(Vector2f(floor(900 / dificulty), floor(900 / dificulty)));
            square[yCord][xCord].setTexture(&texture);
            square[yCord][xCord].setTextureRect(IntRect(floor(900 / dificulty) * xCord, floor(900 / dificulty) * yCord, floor(900 / dificulty) * textureSizeModyfier[x], floor(900 / dificulty) * textureSizeModyfier[y]));
            square[yCord][xCord].setOrigin(-floor(static_cast<float>(900) / dificulty + 5) * xCord, -floor(static_cast<float>(900) / dificulty + 5) * yCord);
            square[yCord][xCord].setPosition(0, 0);
        }
    randomCoeficient = 5 + rand() % 100 * dificulty + 20 * (dificulty - 1);

    square[notExistSquare[y]][notExistSquare[x]].setPosition(-1000, -1000);


    while (moves < randomCoeficient)
    {
        for (i = 0; i < randomCoeficient; i++)
            for (yCord = 0; yCord < square.size(); yCord++)
                for (xCord = 0; xCord < square[yCord].size(); xCord++)
                {
                    int move = rand() % 100;
                    int positionBufer[2] = { square[yCord][xCord].getPosition().x, square[yCord][xCord].getPosition().y };
                    if (move >= 70)
                        SquareMove(yCord, xCord);
                    if (square[yCord][xCord].getPosition().x != positionBufer[x] || square[yCord][xCord].getPosition().y != positionBufer[y])
                        moves++;

                }
    }
}

bool WinConditions() // check squares position. If all squares return position(0, 0) -> player win
{
    int yCord, xCord;
    int localWinSquareCounter = 0;
    for(yCord = 0; yCord < square.size() ; yCord++)
        for(xCord = 0; xCord < square[yCord].size(); xCord++)
            if((square[yCord][xCord].getPosition().x == 0) && (square[yCord][xCord].getPosition().y == 0))
                localWinSquareCounter++;
    if (localWinSquareCounter == (dificulty * dificulty) - 1)
        return true;
    return false;
}
bool SquareExist(int YPos, int XPos, int moveCoef) /*check position relative to the square on which the player clicked, respectively, with the move coefficient. if retun false - square on position not exist & the square the player clicked move
                                                     YPos - y cord on field of selected square
                                                     XPos - x cord of field of selected square
                                                     move coeficient - direction to check. 0 -> right, 1 -> left, 2 -> down, 3 -> up*/
{
    int yCord, xCord;
    vector<float> squarePos(2, 0);
    squarePos[x] = -square[YPos][XPos].getOrigin().x + square[YPos][XPos].getPosition().x + (static_cast<float>(square[YPos][XPos].getSize().x) / 2);
    squarePos[y] = -square[YPos][XPos].getOrigin().y + square[YPos][XPos].getPosition().y + (static_cast<float>(square[YPos][XPos].getSize().y) / 2);
    vector<float> assumeSquarePos(2, 0);

    for (yCord = 0; yCord < square.size(); yCord++)
        for (xCord = 0; xCord < square[yCord].size(); xCord++)
        {
            assumeSquarePos[x] = -square[yCord][xCord].getOrigin().x + square[yCord][xCord].getPosition().x + (static_cast<float>(square[yCord][xCord].getSize().x) / 2);
            assumeSquarePos[y] = -square[yCord][xCord].getOrigin().y + square[yCord][xCord].getPosition().y + (static_cast<float>(square[yCord][xCord].getSize().y) / 2);
            switch (moveCoef)
            {
            case 1:
                if ((squarePos[x] + floor(static_cast<float>(900) / dificulty + 5)) >= floor(static_cast<float>(dificulty) * square[0][0].getSize().x))
                    return true;
                if (((squarePos[x] + floor(static_cast<float>(900) / dificulty + 5)) == assumeSquarePos[x]) && (squarePos[y] == assumeSquarePos[y]))
                    return true;
                break;
            case 2:
                if ((squarePos[x] - floor(static_cast<float>(900) / dificulty + 5)) < floor(static_cast<float>(square[0][0].getSize().x) / 2))
                    return true;
                if (((squarePos[x] - floor(static_cast<float>(900) / dificulty + 5)) == assumeSquarePos[x]) && (squarePos[y] == assumeSquarePos[y]))
                    return true;
                break;
            case 3:
                if ((squarePos[y] + floor(static_cast<float>(900) / dificulty + 5)) >= floor(static_cast<float>(dificulty) * square[YPos][XPos].getSize().y))
                    return true;
                if (((squarePos[y] + floor(static_cast<float>(900) / dificulty + 5)) == assumeSquarePos[y]) && (squarePos[x] == assumeSquarePos[x]))
                    return true;
                break;
            case 4:
                if ((squarePos[y] - floor(static_cast<float>(900) / dificulty + 5)) < floor(static_cast<float>(square[0][0].getSize().y) / 2))
                    return true;
                if (((squarePos[y] - floor(static_cast<float>(900) / dificulty + 5)) == assumeSquarePos[y]) && (squarePos[x] == assumeSquarePos[x]))
                    return true;
                break;
            default:
                cout << "ERROR! Move coefficient indicated incorrectly! Press any button to exit...";
                system("pause");
                exit(1);
                break;
            }
        }
    return false;
}
void SquareMove(int YPos, int XPos) /*moves selected square if possible 
                                      YPos - y cord of selected square
                                      XPos - x cord of selected square*/
{
    if (!SquareExist(YPos, XPos, 1)) // right
    {
        square[YPos][XPos].move(floor(static_cast<float>(900) / dificulty + 5), 0);
        if (musicPlay)
            swapSound.play();
    }
    else if (!SquareExist(YPos, XPos, 2)) // left
    {
        square[YPos][XPos].move(-floor(static_cast<float>(900) / dificulty + 5), 0);
        if (musicPlay)
            swapSound.play();
    }
    else if (!SquareExist(YPos, XPos, 3)) // down
    {
        square[YPos][XPos].move(0, floor(static_cast<float>(900) / dificulty + 5));
        if (musicPlay)
            swapSound.play();
    }
    else if (!SquareExist(YPos, XPos, 4)) // up
    {
        square[YPos][XPos].move(0, -floor(static_cast<float>(900) / dificulty + 5));
        if (musicPlay)
            swapSound.play();
    }
}

void LeaderBoardText(int mode, pair<bool, int> sortType, int page) /* Take parametrs from leader board file & overwrites player parametrs vector
                                                                     mode - game mode; 0 -> classic, 1 -> puzzle;
                                                                     type - return type; 0 -> name, 1 -> time, 2 -> swaps, 3 -> dificulty, 4 -> scores
                                                                     sort type - type of sort; int - sort parametr: 0 -> time, 1 -> swaps, 2 -> scores
                                                                     bool - revert sort. Default sort from max to min. If true - sort from min to max (false -> max->min, true -> min->max)*/
{
    int playersCount = 0;
    int texSymbolCounter = 0, playerid = 0, parametrsCounter = 0;
    int symbolCounter = 1;
    int pageCounter = 0;
    int playersOnPage = 0;

    string fullText = "";
    vector<string> parametrsBufer(4, ""); // 0 - time, 1 - swaps

    ifstream leaderBoard;

    int leaders = 10;
    if(mode == 0)
        leaderBoard.open("Resources\\Saves\\leaderboardclassic.txt");
    else if(mode == 1)
        leaderBoard.open("Resources\\Saves\\leaderboardpuzzle.txt");

        while (leaderBoard)
            getline(leaderBoard, fullText);
        for (texSymbolCounter = 0; texSymbolCounter < fullText.size(); texSymbolCounter++)
            if (fullText[texSymbolCounter] == '|')
                playersCount++;

        if (playersCount < 10)
            leaders = playersCount;
        playerParametrs.clear();
        playerParametrs.resize(playersCount);
        leaderBoardPages = playersCount / 20;

        for (texSymbolCounter = 0; texSymbolCounter < fullText.size(); texSymbolCounter++)
        {
            if (symbolCounter % 5 == 0 && fullText[texSymbolCounter] != '>' && fullText[texSymbolCounter] != '|')
                parametrsBufer[3] += fullText[texSymbolCounter];
            else if (symbolCounter % 4 == 0 && fullText[texSymbolCounter] != '>' && fullText[texSymbolCounter] != '|')
                parametrsBufer[2] += fullText[texSymbolCounter];
            else if (symbolCounter % 3 == 0 && fullText[texSymbolCounter] != '>' && fullText[texSymbolCounter] != '|')
                parametrsBufer[1] += fullText[texSymbolCounter];
            else if (symbolCounter % 2 == 0 && fullText[texSymbolCounter] != '>' && fullText[texSymbolCounter] != '|')
                parametrsBufer[0] += fullText[texSymbolCounter];
            else if (fullText[texSymbolCounter] != '>' && fullText[texSymbolCounter] != '|')
                get<0>(playerParametrs[playerid]) += fullText[texSymbolCounter];

            if (fullText[texSymbolCounter] == '>')
                symbolCounter++;
            else if (fullText[texSymbolCounter] == '|')
            {
                 get<1>(playerParametrs[playerid]) = stoi(parametrsBufer[0]);
                 get<2>(playerParametrs[playerid]) = stoi(parametrsBufer[1]);
                 get<3>(playerParametrs[playerid]) = stoi(parametrsBufer[2])-2;
                 get<4>(playerParametrs[playerid]) = stoi(parametrsBufer[3]);
                 get<5>(playerParametrs[playerid]) = pageCounter;
                 parametrsBufer[0] = "";
                 parametrsBufer[1] = "";
                 parametrsBufer[2] = "";
                 parametrsBufer[3] = "";
                 symbolCounter = 1;

                playerid++;
            }

        }
        if (!sortType.first)
        {
            if (sortType.second == 0)
                sort(playerParametrs.begin(), playerParametrs.end(), leader_board_time_max_compare);
            else if (sortType.second == 1)
                sort(playerParametrs.begin(), playerParametrs.end(), leader_board_swaps_max_compare);
            else
                sort(playerParametrs.begin(), playerParametrs.end(), leader_board_score_max_compare);
        }
        else if (sortType.first)
        {
            if (sortType.second == 0)
                sort(playerParametrs.begin(), playerParametrs.end(), leader_board_time_min_compare);
            else if (sortType.second == 1)
                sort(playerParametrs.begin(), playerParametrs.end(), leader_board_swaps_min_compare);
            else
                sort(playerParametrs.begin(), playerParametrs.end(), leader_board_score_min_compare);
        }
        else
            sort(playerParametrs.begin(), playerParametrs.end(), leader_board_score_max_compare);

        cout.setf(ios::left);
        cout << "sorted leaders table:\nname\ttime\tswaps\tdificulty\n";
        for (playerid = 0; playerid < playersCount; playerid++)
        {
            cout << setw(7)  << get<0>(playerParametrs[playerid]) << "|";
            cout << setw(7)  << get<1>(playerParametrs[playerid]) << "|";
            cout << setw(10) << get<2>(playerParametrs[playerid]) << "|";
            cout << setw(8)  << get<3>(playerParametrs[playerid]) << "|";
            cout << setw(8)  << get<4>(playerParametrs[playerid]) << "\n";
        }

        leaderBoardParametrs[0] = "Name";
        leaderBoardParametrs[1] = "Time";
        leaderBoardParametrs[2] = "Moves";
        leaderBoardParametrs[3] = "Dificulty";
        leaderBoardParametrs[4] = "Score";
        playersOnPage = 20*(page)+20;
        if (playerParametrs.size() - (20 * page) < 20)
            playersOnPage = 20 * (page) + (playerParametrs.size() - (20 * page));
        for (playerid = 20*page; playerid < playersOnPage; playerid++)
        {
            leaderBoardParametrs[0] += "\n" + get<0>(playerParametrs[playerid]);
            leaderBoardParametrs[1] += "\n" + to_string(get<1>(playerParametrs[playerid]));
            leaderBoardParametrs[2] += "\n" + to_string(get<2>(playerParametrs[playerid]));
            leaderBoardParametrs[3] += "\n" + to_string(get<3>(playerParametrs[playerid]));
            leaderBoardParametrs[4] += "\n" + to_string(get<4>(playerParametrs[playerid]));

        }
}
void SaveResult(string name, int time, int swaps, int dificulty, int mode) /*save or overwrite player results in to leaderboard file
                                                                             name - player name;
                                                                             time - time during which the player won;
                                                                             swaps - the number of movements for which the player won;
                                                                             dificulty - dificulty of game;
                                                                             the mode the player was playing; 0 -> classic, 1 -> puzzle
                                                                             */
{
    int playersCount = 0;
    int texSymbolCounter = 0, playerid = 0, parametrsCounter = 0;
    int symbolCounter = 1;
    int score = floor( ((double)dificulty*1000000)/(double)swaps + ((double)dificulty*1000000)/(double)time) * dificulty;

    string fullText = "";
    vector<string> parametrsBufer(4, ""); // 0 - time, 1 - swaps, 2 - dificulty, 3 - score
    string nameBufer = "";

    ifstream leaderBoard;

    if (mode == 0)
        leaderBoard.open("Resources\\Saves\\leaderboardclassic.txt");
    else if (mode == 1)
        leaderBoard.open("Resources\\Saves\\leaderboardpuzzle.txt");

    while (leaderBoard)
        getline(leaderBoard, fullText);
    for (texSymbolCounter = 0; texSymbolCounter < fullText.size(); texSymbolCounter++)
        if (fullText[texSymbolCounter] == '|')
            playersCount++;

    for (playerid = 0; playerid < playersCount; playerid++)
    {
        if (name == get<0>(playerParametrs[playerid]))
        {
            nameBufer = "";
            symbolCounter = 1;

            if (score <= get<4>(playerParametrs[playerid]) && dificulty-2 == get<3>(playerParametrs[playerid]))
                return;
            else if (dificulty-2 == get<3>(playerParametrs[playerid]) && score > get<4>(playerParametrs[playerid]))
            {
                for (texSymbolCounter = 0; texSymbolCounter < fullText.size(); texSymbolCounter++)
                {
                    if (symbolCounter % 5 == 0 && fullText[texSymbolCounter] != '>' && fullText[texSymbolCounter] != '|')
                        parametrsBufer[3] += fullText[texSymbolCounter];
                    else if (symbolCounter % 4 == 0 && fullText[texSymbolCounter] != '>' && fullText[texSymbolCounter] != '|')
                        parametrsBufer[2] += fullText[texSymbolCounter];
                    else if (symbolCounter % 3 == 0 && fullText[texSymbolCounter] != '>' && fullText[texSymbolCounter] != '|')
                        parametrsBufer[1] += fullText[texSymbolCounter];
                    else if (symbolCounter % 2 == 0 && fullText[texSymbolCounter] != '>' && fullText[texSymbolCounter] != '|')
                        parametrsBufer[0] += fullText[texSymbolCounter];
                    else if (fullText[texSymbolCounter] != '>' && fullText[texSymbolCounter] != '|')
                        nameBufer += fullText[texSymbolCounter];

                    if (fullText[texSymbolCounter] == '>')
                        symbolCounter++;
                    else if (fullText[texSymbolCounter] == '|')
                    {
                        if (nameBufer == name)
                        {
                            ofstream leaderBoardR;
                            if (mode == 0)
                                leaderBoardR.open("Resources\\Saves\\leaderboardclassic.txt");
                            else if (mode == 1)
                                leaderBoardR.open("Resources\\Saves\\leaderboardpuzzle.txt");
                            for (int i = 3; i >= 0; i--)
                            {
                                texSymbolCounter -= parametrsBufer[i].size() + 1;
                                fullText.erase(texSymbolCounter, parametrsBufer[i].size() + 1);
                            }
                            fullText.insert(texSymbolCounter, to_string(time) + ">" + to_string(swaps) + ">" + to_string(dificulty) + ">" + to_string(score) + ">");
                            leaderBoardR << fullText;
                            leaderBoardR.close();

                            return;
                        }
                        parametrsBufer[0] = "";
                        parametrsBufer[1] = "";
                        parametrsBufer[2] = "";
                        parametrsBufer[3] = "";
                        nameBufer = "";
                        symbolCounter = 1;
                    }
                }
            }
        }
    }
    fstream leaderBoardO;
    if (mode == 0)
        leaderBoardO.open("Resources\\Saves\\leaderboardclassic.txt", ios_base::app);
    else if (mode == 1)
        leaderBoardO.open("Resources\\Saves\\leaderboardpuzzle.txt", ios_base::app);
    leaderBoardO << name << ">" << to_string(time) << ">" << to_string(swaps) << ">" << to_string(dificulty) << ">" << to_string(score) << ">|";
    leaderBoardO.close();
}

public void GameLoop()
{
    Event gameEvent;
    Event helpEvent;
    clickDelayTimer = clickDelay.getElapsedTime();
    if (!win)
        playerTimer = playerTime.getElapsedTime();

    // game event handlingt
    while (gameField.pollEvent(gameEvent))
    {
        if (gameEvent.type == Event::Closed)
        {
            gameField.close();
            selectMenu.close();
            helpMenu.close();
            selectMenuVisible = false;
        }

        if (gameEvent.type == Event::MouseButtonPressed)
        {
            if (!classicMode)
                if (CheckPosition(helpButton, mouse, gameField) && clickDelayTimer.asSeconds() >= 0.1)
                {
                    if (!helpMenuVisible)
                    {
                        helpMenuVisible = true;
                        helpMenu.setVisible(true);
                    }
                    else
                    {
                        helpMenuVisible = false;
                        helpMenu.setVisible(false);
                    }
                    if (musicPlay)
                        buttonClick.play();
                    clickDelay.restart();
                    clickDelayTimer = clickDelay.getElapsedTime();
                }
            if (CheckPosition(restartButton, mouse, gameField) && clickDelayTimer.asSeconds() >= 0.1)
            {
                if (!classicMode)
                    GameGenerating(currentImageNumber, gameField);
                else
                    GameGenerating(classicGameTexture, gameField, sixteen);
                playerTime.restart();
                if (musicPlay)
                    buttonClick.play();
            }

            else if (CheckPosition(returnButton, mouse, gameField) && clickDelayTimer.asSeconds() >= 0.1)
            {
                selectMenuVisible = true;
                selectMenu.setVisible(true);
                classicMode = false;
                helpMenuVisible = false;
                helpMenu.setVisible(false);
                gameField.close();
                clickDelay.restart();
                if (musicPlay)
                    buttonClick.play();
            }
            else if (CheckPosition(musicSwitch, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
            {
                if (musicPlay)
                {
                    music.pause();
                    musicPlay = false;
                    musicSwitch.setTexture(&musicNotPlayTexture);
                }
                else if (!musicPlay)
                {
                    music.play();
                    musicPlay = true;
                    musicSwitch.setTexture(&musicPlayTexture);
                }
                if (musicPlay)
                    buttonClick.play();
            }
            if (!win)
            {
                for (yCord = 0; yCord < square.size(); yCord++)
                    for (xCord = 0; xCord < square[yCord].size(); xCord++)
                        if (CheckPosition(square[yCord][xCord], mouse, gameField) && clickDelayTimer.asSeconds() >= 0.2)
                        {
                            SquareMove(yCord, xCord);
                            swaps++;
                            clickDelay.restart();
                            break;
                        }
            }
            else
            {
                if (CheckPosition(saveMyResultsButton, mouse, gameField))
                {
                    if (classicMode)
                    {
                        LeaderBoardText(0, pair<bool, int>(false, 3), currentPage);
                        SaveResult(playerNameEnter, playerTimer.asSeconds(), swaps, dificulty, 0);
                    }
                    else
                    {
                        LeaderBoardText(1, pair<bool, int>(false, 3), currentPage);
                        SaveResult(playerNameEnter, playerTimer.asSeconds(), swaps, dificulty, 1);
                    }
                    playerNameEnter = "";
                    selectMenuVisible = true;
                    selectMenu.setVisible(true);
                    classicMode = false;
                    gameField.close();
                    clickDelay.restart();
                    if (musicPlay)
                        buttonClick.play();
                }
                else if (CheckPosition(notSaveMyResultsButton, mouse, gameField))
                {
                    selectMenuVisible = true;
                    selectMenu.setVisible(true);
                    classicMode = false;
                    gameField.close();
                    clickDelay.restart();
                    if (musicPlay)
                        buttonClick.play();
                }
            }

        }
        if (win)
        {
            if (gameEvent.type == sf::Event::TextEntered)
            {

                if (gameEvent.text.unicode == 8)
                {
                    if (!playerNameEnter.empty())
                        playerNameEnter.pop_back();
                }
                else if (gameEvent.text.unicode == 32 && playerNameEnter.size() < 7)
                {
                    if (!playerNameEnter.empty())
                        playerNameEnter += char(gameEvent.text.unicode);
                }
                else if (playerNameEnter.size() < 7)
                    playerNameEnter += char(gameEvent.text.unicode);

            }
        }
    }

    //help menu event handling
    while (helpMenu.pollEvent(helpEvent))
    {
        if (helpEvent.type == Event::Closed)
        {
            helpMenuVisible = false;
            helpMenu.setVisible(false);
        }
    }

    gameField.clear(Color(54, 57, 62));

    for (yCord = 0; yCord < square.size(); yCord++)
        for (xCord = 0; xCord < square[yCord].size(); xCord++)
            gameField.draw(square[yCord][xCord]);
    if (!win && WinConditions())
    {
        win = true;
        if (!classicMode)
            square[notExistSquare[y]][notExistSquare[x]].setPosition(0, 0);
    }
    if (win)
    {
        playerNameEnterText.setString(playerNameEnter);
        gameField.draw(blackoutSquare);
        gameField.draw(winText);
        gameField.draw(notSaveMyResultsButton);
        gameField.draw(saveMyResultsButton);
        gameField.draw(myNameEnterField);
        gameField.draw(saveMyResultsText);
        gameField.draw(notSaveMyResultsText);
        gameField.draw(myNameText);
        gameField.draw(playerNameEnterText);
    }
    gameField.draw(restartButton);
    gameField.draw(returnButton);
    gameField.draw(musicSwitch);

    if (!classicMode)
        gameField.draw(helpButton);

    gameField.display();
    if (helpMenuVisible)
    {
        helpMenu.clear(Color(54, 57, 62));
        helpMenu.draw(helpMenuMasc);
        helpMenu.display();
    }
}
#pragma endregion


int main()
{
#pragma region Variables

    //timers
    Clock clickDelay;
    Time clickDelayTimer;
    Clock playerTime;
    Time playerTimer;
    // cord counters
    int yCord, xCord, i;
    //textures & fonts
    Texture texture;
    Texture classicGameTexture;
    Texture arrow;
    Texture restartTexture;
    Texture returnTexture;
    Texture musicPlayTexture;
    Texture musicNotPlayTexture;
    Texture helpButtonTexture;
    Image icon;
    Font font;
    //Audio
    SoundBuffer swapSoundBufer;
    SoundBuffer buttonClickSound;
    Sound buttonClick;
    Music music;
    // Controls
    Mouse mouse;
    //windows     
    RenderWindow selectMenu(windowSize, "Barley-Break");
    RenderWindow gameField;
    RenderWindow helpMenu(VideoMode(300, 300), "Help");
#pragma endregion
#pragma region Preparamentrs
    // preparametrs
    //Window Parametrs
    helpMenu.setVisible(false);
    //reset random
    std::srand(time(0));
    //setLoacales
    setlocale(LC_ALL, "Russian");
    //load textures
    font.loadFromFile("Resources\\Textures\\Font\\Arial.ttf");
    icon.loadFromFile("Resources\\Textures\\Icon\\game icon.png");
    arrow.loadFromFile("Resources\\Textures\\Icon\\Arrow.png");
    classicGameTexture.loadFromFile("Resources\\Textures\\Classic\\ClassicGame.png");
    restartTexture.loadFromFile("Resources\\Textures\\Icon\\Restart.png");
    returnTexture.loadFromFile("Resources\\Textures\\Icon\\Return.png");
    musicNotPlayTexture.loadFromFile("Resources\\Textures\\Icon\\Music not play.png");
    musicPlayTexture.loadFromFile("Resources\\Textures\\Icon\\Music play.png");
    helpButtonTexture.loadFromFile("Resources\\Textures\\Icon\\help.png");
    //load audio
    swapSoundBufer.loadFromFile("Resources\\Audio\\swapSound.ogg");
    music.openFromFile("Resources\\Audio\\music.ogg");
    buttonClickSound.loadFromFile("Resources\\Audio\\buttonClickSound.ogg");
    swapSound.setBuffer(swapSoundBufer);
    buttonClick.setBuffer(buttonClickSound);
    // check leaderboard files to exist
    ifstream leaderBoardPuzzleTest("Resources\\Saves\\leaderboardpuzzle.txt");
    if (!leaderBoardPuzzleTest.is_open())
    {
        ofstream leaderBoardPuzzleCreate("Resources\\Saves\\leaderboardpuzzle.txt");
        leaderBoardPuzzleCreate.close();
    }
    leaderBoardPuzzleTest.close();

    ifstream leaderBoardClassicTest("Resources\\Saves\\leaderboardclassic.txt");
    if (!leaderBoardClassicTest.is_open())
    {
        ofstream leaderBoardClassicCreate("Resources\\Saves\\leaderboardclassic.txt");
        leaderBoardClassicCreate.close();
    }
    leaderBoardClassicTest.close();

    LeaderBoardText(0, pair<bool, int>(false, 3), 0);
    //set window icon
    selectMenu.setIcon(32, 32, icon.getPixelsPtr());
    helpMenu.setIcon(32, 32, icon.getPixelsPtr());
#pragma endregion
#pragma region Variables continue
    //Variables countinue   
    //Buttons & other squares & parametrs
    RectangleShape restartButton(Vector2f(48, 48));
    restartButton.setTexture(&restartTexture);
    restartButton.setFillColor(Color(254, 254, 34, 180));
    RectangleShape returnButton(Vector2f(48, 48));
    returnButton.setTexture(&returnTexture);
    returnButton.setFillColor(Color(254, 254, 34, 180));
    returnButton.setPosition(selectMenu.getSize().x - 48, 0);
    RectangleShape classicModeButton(Vector2f(192, 48));
    classicModeButton.setFillColor(Color(254, 254, 34, 200));
    classicModeButton.setPosition(128, 100);
    RectangleShape puzzleModeButton(Vector2f(192, 48));
    puzzleModeButton.setFillColor(Color(254, 254, 34, 200));
    puzzleModeButton.setPosition(592, 100);
    RectangleShape prequesiteDificultyButton(Vector2f(99, 124));
    prequesiteDificultyButton.setTexture(&arrow);
    prequesiteDificultyButton.setOrigin(0, 0);
    prequesiteDificultyButton.setScale(-1, 1);
    prequesiteDificultyButton.setPosition(selectMenu.getSize().x/static_cast<float>(2) - 125, selectMenu.getSize().y / static_cast<float>(2) - 31);
    RectangleShape nextDificultyButton(Vector2f(99, 124));
    nextDificultyButton.setTexture(&arrow);
    nextDificultyButton.move(selectMenu.getSize().x / static_cast<float>(2) + 125, selectMenu.getSize().y/static_cast<float>(2)-31);
    RectangleShape dificultyApplyButton(Vector2f(275, 50));
    dificultyApplyButton.setPosition((int)(selectMenu.getSize().x / (float)2)-137, 570);
    dificultyApplyButton.setFillColor(Color(254, 254, 34, 200));
    RectangleShape blackoutSquare;
    blackoutSquare.setFillColor(Color(130, 130, 130, 125));
    RectangleShape saveMyResultsButton(Vector2f(300, 48));
    saveMyResultsButton.setFillColor(Color(254, 254, 34, 230));
    saveMyResultsButton.setPosition(236, 254);
    // countinue withous save
    RectangleShape notSaveMyResultsButton(Vector2f(300, 48));
    notSaveMyResultsButton.setFillColor(Color(254, 254, 34, 230));
    notSaveMyResultsButton.setPosition(100, 288);
    // name enter field
    RectangleShape myNameEnterField(Vector2f(300, 48));
    myNameEnterField.setFillColor(Color(54, 57, 62));
    myNameEnterField.setPosition(130, 254);
    RectangleShape leaderBoardOpenButton(Vector2f(300, 48));
    leaderBoardOpenButton.setFillColor(Color(254, 254, 34, 200));
    leaderBoardOpenButton.setPosition(selectMenu.getSize().x / 2 - leaderBoardOpenButton.getSize().x / 2, selectMenu.getSize().y / 2 - leaderBoardOpenButton.getSize().y / 2);
    RectangleShape leaderBoardModeChange(Vector2f(300, 48));
    leaderBoardModeChange.setPosition(selectMenu.getSize().x / 2 - leaderBoardModeChange.getSize().x / 2, 70);
    leaderBoardModeChange.setFillColor(Color(254, 254, 34, 200));
    RectangleShape timeSortButton(Vector2f(96, 24));
    timeSortButton.setPosition(150, 160);
    timeSortButton.setFillColor(Color(100, 100, 100, 100));
    RectangleShape timeSortButtonMasc(Vector2f(24, 24));
    timeSortButtonMasc.setPosition(244, 160);
    timeSortButtonMasc.setTexture(&arrow);
    timeSortButtonMasc.setRotation(90);
    RectangleShape swapsSortButton(Vector2f(110, 24));
    swapsSortButton.setPosition(300, 160);
    swapsSortButton.setFillColor(Color(100, 100, 100, 100));
    RectangleShape swapsSortButtonMasc(Vector2f(24, 24));
    swapsSortButtonMasc.setPosition(409, 180);
    swapsSortButtonMasc.setTexture(&arrow);
    swapsSortButtonMasc.setRotation(90);
    RectangleShape scoreSortButton(Vector2f(110, 24));
    scoreSortButton.setPosition(600, 160);
    scoreSortButton.setFillColor(Color(100, 100, 100, 100));
    RectangleShape scoreSortButtonMasc(Vector2f(24, 24));
    scoreSortButtonMasc.setTexture(&arrow);
    scoreSortButtonMasc.setPosition(700, 184);
    scoreSortButtonMasc.setRotation(90);
    scoreSortButtonMasc.setScale(-1, 1);
    RectangleShape nextPage(Vector2f(48, 48));
    nextPage.setPosition(selectMenu.getSize().x - 48, selectMenu.getSize().y - 48);
    nextPage.setTexture(&arrow);
    nextPage.setFillColor(Color(254, 254, 34, 180));
    RectangleShape previousPage(Vector2f(48, 48));
    previousPage.setPosition(48, selectMenu.getSize().y - 48);
    previousPage.setFillColor(Color(254, 254, 34, 180));
    previousPage.setTexture(&arrow);
    previousPage.setScale(-1, 1);
    RectangleShape musicSwitch(Vector2f(48, 48));
    musicSwitch.setFillColor(Color(254, 254, 34, 180));
    musicSwitch.setTexture(&musicPlayTexture);
    RectangleShape helpButton(Vector2f(48, 48));
    helpButton.setFillColor(Color(254, 254, 34, 180));
    helpButton.setTexture(&helpButtonTexture);
    RectangleShape helpMenuMasc(Vector2f(290, 290));
    helpMenuMasc.setPosition(5, 5);
    //text
    //string
    string imageName = "";
    //visual
    Text startText("Select image", font, 60);
    startText.setFillColor(Color::White);
    startText.setPosition(selectMenu.getSize().x / 2 - startText.findCharacterPos(12).x / 2, 65);
    Text winText("Congratulations! You Win!", font);
    winText.setFillColor(Color::White);
    Text imageNameText(imageName, font, 24);
    imageNameText.setFillColor(Color::White);
    imageNameText.setPosition(0, 59);
    Text modeSelectMenuTile("Select game mode", font, 50);
    modeSelectMenuTile.setFillColor(Color::White);
    modeSelectMenuTile.setOrigin(-240, 0);
    Text classicModeText("Classic mode", font, 30);
    classicModeText.setFillColor(Color(54, 57, 62));
    classicModeText.setOrigin(-135, -105);
    Text puzzleModeText("Puzzle mode", font, 30);
    puzzleModeText.setFillColor(Color(54, 57, 62));
    puzzleModeText.setOrigin(-600, -105);
    Text setDificultyText("Dificulty", font, 100);
    setDificultyText.setOrigin(165 - (selectMenu.getSize().x / static_cast<float>(2)), 150 - (selectMenu.getSize().y / static_cast<float>(2) - 31));
    Text currentDificultyMasc(to_string(dificulty - 2), font, 90);
    currentDificultyMasc.setOrigin(25 - (selectMenu.getSize().x / static_cast<float>(2)), 0 - (selectMenu.getSize().y / static_cast<float>(2) - 31));
    Text dificultyApplyText("Apply", font, 50);
    dificultyApplyText.setOrigin(-80 - dificultyApplyButton.getPosition().x, 12 - dificultyApplyButton.getPosition().y);
    dificultyApplyText.setFillColor(Color(54, 57, 62));
    // my name
    Text myNameText("Your name", font, 72);
    myNameText.setFillColor(Color::White);
    myNameText.setPosition(0, 250);
    //not save my results text
    Text notSaveMyResultsText("Not save", font, 48);
    notSaveMyResultsText.setFillColor(Color(54, 57, 62));
    notSaveMyResultsText.setPosition(115, 280);
    //save my results text
    Text saveMyResultsText("Save results", font, 48);
    saveMyResultsText.setFillColor(Color(54, 57, 62));
    saveMyResultsText.setPosition(236, 250);
    // player enter name
    Text playerNameEnterText("", font, 32);
    playerNameEnterText.setFillColor(Color::White);
    playerNameEnterText.setPosition(130, 254);
    Text leaderBoardTile("Leaderboard", font, 48);
    leaderBoardTile.setPosition(selectMenu.getSize().x / 2 - 140, 0);
    Text leaderName("", font, 30);
    leaderName.setPosition(0, 150);
    Text leaderTime("", font, 30);
    leaderTime.setPosition(150, 150);
    Text leaderSwaps("", font, 30);
    leaderSwaps.setPosition(300, 150);
    Text leaderDificulty("", font, 30);
    leaderDificulty.setPosition(450, 150);
    Text leaderScore("", font, 30);
    leaderScore.setPosition(600, 150);
    Text leaderBoardOpenText("Leaderboard", font, 48);
    leaderBoardOpenText.setPosition(leaderBoardOpenButton.getPosition().x + 10, leaderBoardOpenButton.getPosition().y - 5);
    leaderBoardOpenText.setFillColor(Color(54, 57, 62));
    Text leaderBoardModeText("Classic", font, 48);
    leaderBoardModeText.setPosition(leaderBoardModeChange.getPosition().x + 70, leaderBoardModeChange.getPosition().y - 10);
    leaderBoardModeText.setFillColor(Color(54, 57, 62));

    //other parametrs
    int leaderBoardMode = 0;
    bool revert = false;
    int sortParametr = 3;
    int currentPage = 0;
#pragma endregion

    //start
    MainMenuCreate();
    music.play();
    music.setLoop(true);

    //main loop
    while (gameField.isOpen() || selectMenu.isOpen())
    {
        //main Menu
        while (selectMenuVisible) 
        {
            clickDelayTimer = clickDelay.getElapsedTime();

            if (mode == 0)
            {
                Event selectionEvent;
                while (selectMenu.pollEvent(selectionEvent))
                {
                    if (selectionEvent.type == Event::Closed)
                    {
                        selectMenu.close();
                        gameField.close();
                        selectMenuVisible = false;
                        break;
                    }
                }
                if (selectionEvent.type == Event::MouseButtonPressed && selectionEvent.mouseButton.button == Mouse::Left)
                {
                    if (CheckPosition(puzzleModeButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                    {
                        currentPage = 0;
                        mode = 1;
                        if(musicPlay)
                            buttonClick.play();
                    }
                    else if (CheckPosition(classicModeButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                    {
                        dificulty = 4;
                        gameWindowSize.height = (900 / dificulty + 5) * dificulty - 5;
                        gameWindowSize.width = (900 / dificulty + 5) * dificulty - 5;
                        gameField.create(gameWindowSize, "Barley-Break");
                        gameField.setIcon(32, 32, icon.getPixelsPtr());
                        selectMenu.setVisible(false);
                        selectMenuVisible = false;
                        classicMode = true;
                        GameGenerating(classicGameTexture, gameField, sixteen);
                        playerTime.restart();
                        if(musicPlay)
                            buttonClick.play();
                    }
                    else if (CheckPosition(musicSwitch, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                    {
                        if (musicPlay)
                        {
                            music.pause();
                            musicPlay = false;
                            musicSwitch.setTexture(&musicNotPlayTexture, true);
                        }
                        else if (!musicPlay)
                        {
                            music.play();
                            musicPlay = true;
                            musicSwitch.setTexture(&musicPlayTexture, true);
                        }
                        if (musicPlay)
                            buttonClick.play();
                    }
                    else if (CheckPosition(leaderBoardOpenButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                    {
                        mode = 3;
                        currentPage = 0;
                        leaderBoardMode = 0;
                        revert = false;
                        returnButton.setPosition(selectMenu.getSize().x - 48, 0);
                        sortParametr = 2;
                        currentPage = 0;
                        LeaderBoardText(leaderBoardMode, pair<bool, int>(revert, sortParametr), currentPage);
                        leaderName.setString(leaderBoardParametrs[0]);
                        leaderTime.setString(leaderBoardParametrs[1]);
                        leaderSwaps.setString(leaderBoardParametrs[2]);
                        leaderDificulty.setString(leaderBoardParametrs[3]);
                        leaderScore.setString(leaderBoardParametrs[4]);
                        leaderBoardModeText.setString("Classic");
                        if (musicPlay)
                            buttonClick.play();
                    }
                    clickDelay.restart();
                }
                selectMenu.clear(Color(54, 57, 62));
                selectMenu.draw(modeSelectMenuTile);
                selectMenu.draw(classicModeButton);
                selectMenu.draw(puzzleModeButton);
                selectMenu.draw(classicModeText);
                selectMenu.draw(puzzleModeText);
                selectMenu.draw(leaderBoardOpenButton);
                selectMenu.draw(leaderBoardOpenText);
                selectMenu.draw(musicSwitch);
                selectMenu.display();
            }
            else if (mode == 1)
            {
                if (!dificultySelect)
                {
                    Event puzzleSelectionEvent;
                    while (selectMenu.pollEvent(puzzleSelectionEvent))
                    {
                        if (puzzleSelectionEvent.type == Event::Closed)
                        {
                            selectMenu.close();
                            gameField.close();
                            selectMenuVisible = false;
                        }
                        if (puzzleSelectionEvent.type == Event::MouseButtonPressed && puzzleSelectionEvent.mouseButton.button == Mouse::Left)
                        {
                            if (currentPage < mainMenuPages && CheckPosition(nextPage, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                            {
                                currentPage++;
                                if (musicPlay)
                                    buttonClick.play();
                            }
                            else if (currentPage > 0 && CheckPosition(previousPage, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                            {
                                currentPage--;
                                if (musicPlay)
                                    buttonClick.play();
                            }
                            else if (CheckPosition(musicSwitch, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                            {
                                if (musicPlay)
                                {
                                    music.pause();
                                    musicPlay = false;
                                    musicSwitch.setTexture(&musicNotPlayTexture);
                                }
                                else if (!musicPlay)
                                {
                                    music.play();
                                    musicPlay = true;
                                    musicSwitch.setTexture(&musicPlayTexture);
                                }
                                if (musicPlay)
                                    buttonClick.play();
                            }
                            else if (CheckPosition(returnButton, mouse, selectMenu))
                            {
                                mode = 0;
                                if(musicPlay)
                                    buttonClick.play();
                            }
                            else if (CheckPosition(restartButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                            {
                                if (musicPlay)
                                    buttonClick.play();
                                MainMenuCreate();
                                currentPage = 0;
                            }
                            else
                            {
                                for (i = 0; i < mainMenuSquareVisual.size(); i++)
                                    if (mainMenuSquareVisual[i].first == currentPage && CheckPosition(mainMenuSquareVisual[i].second, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                                    {
                                        dificultySelect = true;
                                        currentImageNumber = i;
                                        if (musicPlay)
                                            buttonClick.play();
                                    }
                            }
                            clickDelay.restart();
                        }
                        else break;
                    }
                    restartButton.setPosition(selectMenu.getSize().x / 2 - 24, 0);
                    selectMenu.clear(Color(54, 57, 62));
                    selectMenu.draw(startText);
                    for (i = 0; i < mainMenuSquareVisual.size(); i++)
                        if (mainMenuSquareVisual[i].first == currentPage)
                            selectMenu.draw(mainMenuSquareVisual[i].second);
                    returnButton.setPosition(selectMenu.getSize().x - 48, 0);
                    selectMenu.draw(returnButton);
                    if (currentPage < mainMenuPages)
                        selectMenu.draw(nextPage);
                    if (currentPage > 0)
                        selectMenu.draw(previousPage);
                    selectMenu.draw(restartButton);
                    selectMenu.draw(musicSwitch);
                    selectMenu.display();
                }
                else
                {
                    Event dificultySelectionEvent;
                    while (selectMenu.pollEvent(dificultySelectionEvent))
                    {
                        if (dificultySelectionEvent.type == Event::Closed)
                        {
                            selectMenu.close();
                            gameField.close();
                            selectMenuVisible = false;
                        }

                        if (dificultySelectionEvent.type == Event::MouseButtonPressed && dificultySelectionEvent.mouseButton.button == Mouse::Left)
                        {
                            if (CheckPosition(returnButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                            {
                                dificultySelect = false;
                                if (musicPlay)
                                    buttonClick.play();
                                clickDelay.restart();
                            }
                            else if (CheckPosition(prequesiteDificultyButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                            {
                                if (dificulty > 3)
                                    dificulty--;
                                else if (dificulty <= 3)
                                    dificulty = 9;
                                clickDelay.restart();
                                if (musicPlay)
                                    buttonClick.play();
                            }
                            else if (CheckPosition(musicSwitch, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                            {
                                if (musicPlay)
                                {
                                    music.pause();
                                    musicPlay = false;
                                    musicSwitch.setTexture(&musicNotPlayTexture);
                                }
                                else if (!musicPlay)
                                {
                                    music.play();
                                    musicPlay = true;
                                    musicSwitch.setTexture(&musicPlayTexture);
                                }
                                if (musicPlay)
                                    buttonClick.play();
                            }
                            else if (CheckPosition(nextDificultyButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                            {
                                if (dificulty < 9)
                                    dificulty++;
                                else if (dificulty >= 9)
                                    dificulty = 3;
                                clickDelay.restart();
                                if (musicPlay)
                                    buttonClick.play();
                            }
                            else if (CheckPosition(dificultyApplyButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                            {
                                gameWindowSize.height = (900 / dificulty + 5) * dificulty - 5;
                                gameWindowSize.width = (900 / dificulty + 5) * dificulty - 5;
                                gameField.create(gameWindowSize, "Barley-Break");
                                gameField.setIcon(32, 32, icon.getPixelsPtr());
                                selectMenu.setVisible(false);
                                selectMenuVisible = false;
                                GameGenerating(currentImageNumber, gameField);
                                dificultySelect = false;
                                playerTime.restart();
                                if (musicPlay)
                                    buttonClick.play();
                            }
                        }
                        currentDificultyMasc.setString(to_string(dificulty - 2));
                        selectMenu.clear(Color(54, 57, 62));
                        selectMenu.draw(nextDificultyButton);
                        selectMenu.draw(setDificultyText);
                        selectMenu.draw(prequesiteDificultyButton);
                        selectMenu.draw(currentDificultyMasc);
                        selectMenu.draw(dificultyApplyButton);
                        returnButton.setPosition(selectMenu.getSize().x - 48, 0);
                        selectMenu.draw(returnButton);
                        selectMenu.draw(musicSwitch);
                        selectMenu.draw(dificultyApplyText);
                        selectMenu.display();
                    }
                }
            }
            else if (mode == 3) // leaderBoard
            {
            Event selectionEvent;
                while (selectMenu.pollEvent(selectionEvent))
                {
                    if (selectionEvent.type == Event::Closed)
                    {
                        selectMenu.close();
                        gameField.close();
                        selectMenuVisible = false;
                    }
                    if (selectionEvent.type == Event::MouseButtonPressed && selectionEvent.mouseButton.button == Mouse::Left)
                    {
                        if (currentPage < leaderBoardPages && CheckPosition(nextPage, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                        {
                            currentPage++;
                            if (musicPlay)
                                buttonClick.play();
                        }
                        else if (currentPage > 0 && CheckPosition(previousPage, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                        {
                            currentPage--;
                            if (musicPlay)
                                buttonClick.play();
                        }
                        else if (CheckPosition(returnButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                        {
                            mode = 0;
                            clickDelay.restart();
                            if (musicPlay)
                                buttonClick.play();
                        }
                        else if (CheckPosition(musicSwitch, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                        {
                            if (musicPlay)
                            {
                                music.pause();
                                musicPlay = false;
                                musicSwitch.setTexture(&musicNotPlayTexture);
                            }
                            else if (!musicPlay)
                            {
                                music.play();
                                musicPlay = true;
                                musicSwitch.setTexture(&musicPlayTexture);
                            }
                            if (musicPlay)
                                buttonClick.play();
                        }
                        else if (CheckPosition(leaderBoardModeChange, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                        {
                            if (leaderBoardMode == 0)
                            {
                                leaderBoardMode = 1;
                                LeaderBoardText(0, pair<bool, int>(false, 3), currentPage);
                                leaderBoardModeText.setString("Puzzle");
                            }
                            else if (leaderBoardMode == 1)
                            {
                                leaderBoardMode = 0;                               
                                leaderBoardModeText.setString("Classic");
                            }
                            if (musicPlay)
                                buttonClick.play();
                        }
                        else if (CheckPosition(timeSortButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                        {
                            if (sortParametr != 0)
                            {
                                sortParametr = 0;
                                revert = true;
                            }
                            else if (!revert)
                                revert = true;
                            else
                                revert = false;
                            if (musicPlay)
                                buttonClick.play();
                        }
                        else if (CheckPosition(swapsSortButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                        {
                            if (sortParametr != 1)
                            {
                                sortParametr = 1;
                                revert = true;
                            }
                            else if (!revert)
                                revert = true;
                            else
                                revert = false;
                            if (musicPlay)
                                buttonClick.play();
                        }
                        else if (CheckPosition(scoreSortButton, mouse, selectMenu) && clickDelayTimer.asSeconds() >= 0.1)
                        {
                            if (sortParametr != 2)
                            {
                                sortParametr = 2;
                                revert = false;
                            }
                            else if (!revert)
                                revert = true;
                            else
                                revert = false;
                            if (musicPlay)
                                buttonClick.play();
                        }
                        if (!revert)
                        {
                            timeSortButtonMasc.setScale(-1, 1);
                            swapsSortButtonMasc.setScale(-1, 1);
                            scoreSortButtonMasc.setScale(-1, 1);
                            timeSortButtonMasc.setPosition(244, 184);
                            swapsSortButtonMasc.setPosition(409, 184);
                            scoreSortButtonMasc.setPosition(700, 184);
                        }
                        else
                        {
                            timeSortButtonMasc.setScale(1, 1);
                            swapsSortButtonMasc.setScale(1, 1);
                            scoreSortButtonMasc.setScale(1, 1);
                            timeSortButtonMasc.setPosition(244, 160);
                            swapsSortButtonMasc.setPosition(409, 160);
                            scoreSortButtonMasc.setPosition(700, 160);
                        }
                        LeaderBoardText(leaderBoardMode, pair<bool, int>(revert, sortParametr), currentPage);
                        leaderName.setString(leaderBoardParametrs[0]);
                        leaderTime.setString(leaderBoardParametrs[1]);
                        leaderSwaps.setString(leaderBoardParametrs[2]);
                        leaderDificulty.setString(leaderBoardParametrs[3]);
                        leaderScore.setString(leaderBoardParametrs[4]);
                        clickDelay.restart();
                    }
                }
                for (int playerid = 0; playerid < playerParametrs.size(); playerid++)
                selectMenu.clear(Color(54, 57, 62));
                selectMenu.draw(returnButton);
                if(sortParametr == 0)
                    selectMenu.draw(timeSortButtonMasc);
                if (sortParametr == 1)
                    selectMenu.draw(swapsSortButtonMasc);
                if (sortParametr == 2)
                    selectMenu.draw(scoreSortButtonMasc);
                selectMenu.draw(leaderBoardTile);
                selectMenu.draw(leaderName);
                selectMenu.draw(leaderTime);
                selectMenu.draw(leaderSwaps);
                if (leaderBoardMode == 1)
                    selectMenu.draw(leaderDificulty);
                selectMenu.draw(leaderScore);
                selectMenu.draw(musicSwitch);
                selectMenu.draw(leaderBoardModeChange);
                selectMenu.draw(leaderBoardModeText);
                if (currentPage < leaderBoardPages)
                    selectMenu.draw(nextPage);
                if (currentPage > 0)
                    selectMenu.draw(previousPage);
                selectMenu.display();
            }
        }
        //Start game
        if (gameField.isOpen())
        {
            blackoutSquare.setSize(Vector2f(gameField.getSize().x, gameField.getSize().y));
            returnButton.setPosition(gameField.getSize().x - 48, 0);
            myNameEnterField.setPosition(gameField.getSize().x / 2 - myNameEnterField.getSize().x, gameField.getSize().y / 2 - myNameEnterField.getSize().y / 2);
            myNameText.setPosition(myNameEnterField.getPosition().x - 22, myNameEnterField.getPosition().y - 96);
            saveMyResultsButton.setPosition(myNameEnterField.getPosition().x + myNameEnterField.getSize().x/2 + saveMyResultsButton.getSize().x/2 + 12, myNameEnterField.getPosition().y);
            notSaveMyResultsButton.setPosition(gameField.getSize().x / 2 - notSaveMyResultsButton.getSize().x/2, gameField.getSize().y / 2 + myNameEnterField.getSize().y  + 24);
            myNameText.setPosition(myNameEnterField.getPosition().x-22, myNameEnterField.getPosition().y - 96);
            notSaveMyResultsText.setPosition(notSaveMyResultsButton.getPosition().x + 48, notSaveMyResultsButton.getPosition().y - 5);
            saveMyResultsText.setPosition(saveMyResultsButton.getPosition().x + 12, saveMyResultsButton.getPosition().y - 5);
            playerNameEnterText.setPosition(myNameEnterField.getPosition().x + 10, myNameEnterField.getPosition().y + 2);
            restartButton.setPosition(gameField.getSize().x/2 - 24, 0);
            returnButton.setPosition(gameField.getSize().x - 48, 0);
            winText.setPosition(gameField.getSize().x/2 - winText.findCharacterPos(25).x/2 , 0);
            helpButton.setPosition(0, gameField.getSize().y - 48);
            if (!classicMode)
                helpMenuMasc.setTexture(square[0][0].getTexture(), true);        
            helpMenu.setPosition(Vector2i(gameField.getPosition().x + gameField.getSize().x, gameField.getPosition().y));
        }
        //game
        while(gameField.isOpen())
        {
            GameLoop();
        }
    }
    return 0;
}
