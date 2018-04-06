#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>
using namespace std;

enum Stone
{
    black, white, none
};
Stone gameBoard[19][19];

enum Turn
{
    MIN, MAX
};

class State
{
public:
    int depth;
    float value;
    Turn turn;
    Stone gameBoard[19][19];
    vector <pair<int, int>> myStone;
    vector <pair<int, int>> aiStone;
    State *parent;
    vector<State*> child;

    State()
    {
        this->depth = 0;
        this->value = 0;
        this->turn = Turn::MAX;
        for (int i = 0; i < 19; i++) {
            for (int j = 0; j < 19; j++) {
                this->gameBoard[i][j] = gameBoard[i][j];
            }
        }
        this->parent = nullptr;
    }

};

void PrintBoard();
void SearchMove(State S, float timeLimit);
bool CutoffTest(State S, int d);
float Eval(State S);
State MinMax(State S, int d);

int main()
{
    // init
    int row;
    char column;
    for(int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            gameBoard[i][j] = Stone::none;
        }
    }

    string myColorInput;
    cout << "Choose your Stone Color." << endl;
    cout << "Black or White? : ";
    cin >> myColorInput;

    Stone myStone;
    if(myColorInput == "black" | myColorInput == "Black")
        myStone = Stone::black;

    else if(myColorInput == "white" | myColorInput == "White")
        myStone = Stone::white;

    else
    {
        cout << "wrong input. default color is black" << endl;
        myStone = Stone::black;
    }

    // main loop
    while (true)
    {
        // get input
        cout << "input : ";
        cin >> row >> column;

        if (row >= 1 && row <= 19 && column >= 'a' && column <= 's')
        {
            gameBoard[row - 1][column - 'a'] = myStone;
        }
        PrintBoard();

        // minimax
        State* root = new State();

        // print board
        PrintBoard();
        break;
    }
}

void PrintBoard()
{
    /*
    == Board =================================
       | a b c d e f g h i j k l m n o p q r s
    ---+--------------------------------------
    19 | . . . . . . . . . . . . . . . . . . .
    18 | . . . . . . . . . . . . . . . . . . .
    17 | . . . . . . . . . . . . . . . . . . .
    16 | . . . . . . . . . . . . . . . . . . .
    15 | . . . . . . . . . . . . . . . . . . .
    14 | . . . . . . . . . . . . . . . . . . .
    13 | . . . . . . . . . . . . . . . . . . .
    12 | . . . . . . . . . . . . . . . . . . .
    11 | . . ..X.O O O O O X . . .
    10 | . . . ..O.X.O . . . . .
    09 | . . . ..X O O O O X . . . .
    08 | . . . ..X.O X X . . . . .
    07 | . . . ..X X O X . . . . . .
    06 | . . . ..X O O . . . . . . .
    05 | . . . ..O.X . . . . . . .
    04 | . . ..X . . . . . . . . . .
    03 | . . . . . . . . . . . . . . .
    02 | . . . . . . . . . . . . . . . . . . .
    01 | . . . . . . . . . . . . . . . . . . .
    */

    cout << "== Board =================================" << endl;
    cout << "   | a b c d e f g h i j k l m n o p q r s" << endl;
    cout << "---+--------------------------------------" << endl;
    for (int i = 18; i >= 0; i--)
    {
        cout << i + 1;
        if (i + 1 < 10) cout << " ";
        cout << " |";
        for (int j = 0; j < 19; j++)
        {
            switch (gameBoard[i][j])
            {
                case Stone::black :
                    cout << " B";
                    break;
                case Stone::white :
                    cout << " W";
                    break;
                case Stone::none :
                    cout << " .";
                    break;
            }
        }
        cout << endl;
    }
}

void SearchMove(State S, float timeLimit)
{
    clock_t timer;
    timer = clock();
    set<pair<int, int>> border;
    border.clear();


    for(int row = 0; row < 19; row++)
    {
        for(int col = 0; col < 19; col++)
        {
            if(S.gameBoard[row][col] != Stone::none)
            {
                if(row < 18 && S.gameBoard[row + 1][col] != Stone::none)
                {
                    border.insert(make_pair(row + 1, col));
                }
                if(row > 0 && S.gameBoard[row - 1][col] != Stone::none)
                {
                    border.insert(make_pair(row - 1, col));
                }
                if(col < 18 && S.gameBoard[row][col + 1] != Stone::none)
                {
                    border.insert(make_pair(row, col + 1));
                }
                if(col > 0 && S.gameBoard[row][col - 1] != Stone::none)
                {
                    border.insert(make_pair(row, col - 1));
                }
                if(row < 18 && col < 18 && S.gameBoard[row + 1][col + 1] != Stone::none)
                {
                    border.insert(make_pair(row + 1, col + 1));
                }
                if(row < 18 && col > 0 && S.gameBoard[row + 1][col - 1] != Stone::none)
                {
                    border.insert(make_pair(row + 1, col - 1));
                }
                if(row > 0 && col < 18 && S.gameBoard[row - 1][col + 1] != Stone::none)
                {
                    border.insert(make_pair(row - 1, col + 1));
                }
                if(row > 0 && col > 0 && S.gameBoard[row - 1][col - 1] != Stone::none)
                {
                    border.insert(make_pair(row - 1, col - 1));
                }
            }
        }
    }

}

bool CutoffTest(State S, int d)
{
    if(S.depth >= d)
        return true;
    else return false;
}

float Eval(State S)
{
    return 0;
}

State MinMax(State S, int d)
{
    if(CutoffTest(S, d))
    {
        S.value = Eval(S);
        return S;
    }

    // Max
    if(S.turn == Turn::MAX)
    {
        float value = -1000000;
        State result;
        State MaxState = S;
        for(int i = 0; i < S.child.size(); i++)
        {
            result = MinMax(*S.child[i], d+1);
            if(value < result.value)
            {
                value = result.value;
                MaxState = result;
            }
        }
        return MaxState;
    }

    // Min
    else
    {
        float value = 1000000;
        State result;
        State MinState = S;
        for(int i = 0; i < S.child.size(); i++)
        {
            result = MinMax(*S.child[i], d+1);
            if(value > result.value)
            {
                value = result.value;
                MinState = result;
            }
        }
        return MinState;
    }
}


