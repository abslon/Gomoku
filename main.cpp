#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <ctime>
#include <cstdlib>
using namespace std;

// 돌 종류
enum Stone
{
    black, white, none
};

Stone myStoneColor;
Stone aiStoneColor;
// 게임 판
Stone IgameBoard[19][19];

// 누구 턴인지
enum Turn
{
    MIN, MAX
};

// 사용자 돌 위치
vector <pair<int, int>> ImyStone;
// AI 돌 위치
vector <pair<int, int>> IaiStone;

class State
{
public:
    int depth; 
    int value;
    Turn turn;
    vector <pair<int, int>> myStone;
    vector <pair<int, int>> aiStone;
    State *parent;
    vector<State*> child;
	pair<int, int> action;

	// 사용자가 둔 순간 생성되어 그 밑 node를 생성해나감
    State()
    {
        this->depth = 0;
        this->value = 0;
        this->turn = Turn::MAX;
		for (int i = 0; i < ImyStone.size(); i++)
			this->myStone.push_back(ImyStone[i]);
		for (int i = 0; i < IaiStone.size(); i++)
			this->aiStone.push_back(IaiStone[i]);
        this->parent = nullptr;
    }

	State(vector <pair<int, int>> ImyStone, vector <pair<int, int>> IaiStone)
	{
		this->depth = 0;
		this->value = 0;
		this->turn = Turn::MIN;
		this->myStone.assign(ImyStone.begin(), ImyStone.end());
		this->aiStone.assign(IaiStone.begin(), IaiStone.end());
		this->parent = nullptr;
	}

	~State()
	{

	}
};

void PrintBoard();
void DeleteTree(State* root);
void SearchMove(State* S, clock_t timeLimit, int* depth);
bool CheckWin(Stone color);
bool CutoffTest(State S, int d);
int Eval(State S);
State MiniMax(State S, int d, int alpha, int beta);

int main()
{
    // init
    int row;
    char column;
    for(int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            IgameBoard[i][j] = Stone::none;
        }
    }
	srand((unsigned int)time(NULL));

	// 색 선택
    string myColorInput;
    cout << "Choose your Stone Color." << endl;
    cout << "Black or White? : ";
	cin >> myColorInput;

	if (myColorInput == "black" | myColorInput == "Black")
	{
		myStoneColor = Stone::black;
		aiStoneColor = Stone::white;
	}
	else if (myColorInput == "white" | myColorInput == "White")
	{
		myStoneColor = Stone::white;
		aiStoneColor = Stone::black;
	}
    else
    {
        cout << "wrong input. default color is black" << endl;
        myStoneColor = Stone::black;
		aiStoneColor = Stone::white;
    }

	// 선공 후공 선택
	string turnInput;
	cout << "Select attack turn." << endl;
	cout << "First or Second? : ";
	cin >> turnInput;

	if (turnInput == "second" | turnInput == "Second")
	{
		IgameBoard[9][9] = aiStoneColor;
		IaiStone.push_back(make_pair(9, 9));
		
	}

	else if (turnInput == "first" | turnInput == "First")
	{
	}
	else
	{
		cout << "wrong input. default turn is 'First'" << endl;
	}

	// 연산 시간 입력
	double execTime;
	cout << "Set program execution time." << endl;
	cout << "Input (unit = second) : ";
	cin >> execTime;

	PrintBoard();
    // main loop
    while (true)
    {
		cout << "Input your stone position. ex)3 c" << endl;
		cout << "input : ";
		cin >> row >> column;

		if (row >= 1 && row <= 19 && column >= 'a' && column <= 's')
		{
			if (IgameBoard[row - 1][column - 'a'] != none)
				cout << "you can't place there" << endl;
			else
			{
				IgameBoard[row - 1][column - 'a'] = myStoneColor;
				ImyStone.push_back(make_pair(row - 1, column - 'a'));
			}
		}
		
        
        PrintBoard();

		if (CheckWin(myStoneColor))
		{
			cout << "You Win!" << endl;
			break;
		}

        // minimax
		cout << "AI running" << endl;
		int depth = 0;
        State* root = new State();
		SearchMove(root, execTime, &depth);
		State result = MiniMax(*root, depth, -1000000, 1000000);
		State dest = result;
		while (dest.depth != 1)
		{
			dest = *dest.parent;
		}
		
		IgameBoard[dest.action.first][dest.action.second] = aiStoneColor;
		IaiStone.push_back(dest.action);

        // print board
        PrintBoard();
		cout << "deleting buffer... please wait" << endl;
		DeleteTree(root);
		if(CheckWin(aiStoneColor))
		{
			cout << "You Lose!" << endl;
			break;
		}
        //break;
    }
	system("pause");
}

void DeleteTree(State* root)
{
	State* Node;
	for each (Node in root->child)
	{
		DeleteTree(Node);
	}
	delete root;
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
            switch (IgameBoard[i][j])
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
	cout << endl;
}

void MakeGameboard(State S, Stone gameboard[19][19])
{
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			gameboard[i][j] = Stone::none;
		}
	}

	pair<int, int> point;
	for each(point in S.myStone)
	{
		gameboard[point.first][point.second] = myStoneColor;
	}

	for each(point in S.aiStone)
	{
		gameboard[point.first][point.second] = aiStoneColor;
	}
}

void GetBorderPos(State CurrentS, vector<pair<int, int>>* input)
{
	Stone CgameBoard[19][19];
	MakeGameboard(CurrentS, CgameBoard);

	for (int i = 0; i < CurrentS.myStone.size() + CurrentS.aiStone.size(); i++)
	{
		int first, second;
		if (i < CurrentS.myStone.size())
		{
			first = CurrentS.myStone[i].first;
			second = CurrentS.myStone[i].second;
		}
		else
		{
			first = CurrentS.aiStone[i - CurrentS.myStone.size()].first;
			second = CurrentS.aiStone[i - CurrentS.myStone.size()].second;
		}
		
		// 위가 비었는지
		if (first < 18 && CgameBoard[first + 1][second] == Stone::none)
		{
			vector<pair<int, int>>::iterator it;
			pair<int, int> p = make_pair(first + 1, second);

			it = find(input->begin(), input->end(), p);
			if (it == input->end())
				input->push_back(p);
		}
			
		// 아래가 비었는지
		if (first > 0 && CgameBoard[first - 1][second] == Stone::none)
		{
			vector<pair<int, int>>::iterator it;
			pair<int, int> p = make_pair(first - 1, second);

			it = find(input->begin(), input->end(), p);
			if (it == input->end())
				input->push_back(p);
		}
		// 오른쪽이 비었는지
		if (second < 18 && CgameBoard[first][second + 1] == Stone::none)
		{
			vector<pair<int, int>>::iterator it;
			pair<int, int> p = make_pair(first, second + 1);

			it = find(input->begin(), input->end(), p);
			if (it == input->end())
				input->push_back(p);
		}
		// 왼쪽이 비었는지
		if (second > 0 && CgameBoard[first][second - 1] == Stone::none)
		{
			vector<pair<int, int>>::iterator it;
			pair<int, int> p = make_pair(first, second - 1);

			it = find(input->begin(), input->end(), p);
			if (it == input->end())
				input->push_back(p);
		}
		// 오른쪽 위가 비었는지
		if (first < 18 && second < 18 && CgameBoard[first + 1][second + 1] == Stone::none)
		{
			vector<pair<int, int>>::iterator it;
			pair<int, int> p = make_pair(first + 1, second + 1);

			it = find(input->begin(), input->end(), p);
			if (it == input->end())
				input->push_back(p);
		}
		// 오른쪽 아래가 비었는지
		if (first > 0 && second < 18 && CgameBoard[first - 1][second + 1] == Stone::none)
		{
			vector<pair<int, int>>::iterator it;
			pair<int, int> p = make_pair(first - 1, second + 1);

			it = find(input->begin(), input->end(), p);
			if (it == input->end())
				input->push_back(p);
		}
		// 왼쪽 위가 비었는지
		if (first < 18 && second > 0 && CgameBoard[first + 1][second - 1] == Stone::none)
		{
			vector<pair<int, int>>::iterator it;
			pair<int, int> p = make_pair(first + 1, second - 1);

			it = find(input->begin(), input->end(), p);
			if (it == input->end())
				input->push_back(p);
		}
		// 왼쪽 아래가 비었는지
		if (first > 0 && second > 0 && CgameBoard[first - 1][second - 1] == Stone::none)
		{
			vector<pair<int, int>>::iterator it;
			pair<int, int> p = make_pair(first - 1, second - 1);

			it = find(input->begin(), input->end(), p);
			if (it == input->end())
				input->push_back(p);
		}
	}
}

void SearchMove(State* CurrentS, clock_t timeLimit, int* depth)
{
	queue<State*> StateQueue;
	clock_t timer = clock();
	StateQueue.push(CurrentS);
	
	while ((clock() - timer)/CLOCKS_PER_SEC < timeLimit)
	{
		State* S = StateQueue.front();
		vector<pair<int, int>> border;
		GetBorderPos(*S, &border);
		if ((clock() - timer) / CLOCKS_PER_SEC >= timeLimit) return;

		pair<int, int> point;
		for each (point in border)
		{
			State* C = new State(S->myStone, S->aiStone);
			C->depth = S->depth + 1;
			*depth = S->depth;
			//C->value = Eval(*C);
			C->turn = (S->turn == Turn::MAX ? Turn::MIN : Turn::MAX);
			(S->turn == Turn::MAX ? C->aiStone : C->myStone).push_back(point);
			C->parent = S;
			C->action = point;
			S->child.push_back(C);
			StateQueue.push(C);
			if ((clock() - timer) / CLOCKS_PER_SEC >= timeLimit) return;
		}
		StateQueue.pop();
	}
}

bool CheckWin(Stone color)
{
	State* S = new State();
	Stone SgameBoard[19][19];
	MakeGameboard(*S, SgameBoard);
	pair<int, int> point;

	if (color == myStoneColor)
	{
		for each (point in S->myStone)
		{
			if (S->myStone.size() >= 5)
			{
				// 가로
				if (point.second < 15 &&
					SgameBoard[point.first][point.second] == myStoneColor &&
					SgameBoard[point.first][point.second + 1] == myStoneColor &&
					SgameBoard[point.first][point.second + 2] == myStoneColor &&
					SgameBoard[point.first][point.second + 3] == myStoneColor &&
					SgameBoard[point.first][point.second + 4] == myStoneColor)
				{
					return true;
				}

				// 세로
				if (point.first < 15 &&
					SgameBoard[point.first][point.second] == myStoneColor &&
					SgameBoard[point.first + 1][point.second] == myStoneColor &&
					SgameBoard[point.first + 2][point.second] == myStoneColor &&
					SgameBoard[point.first + 3][point.second] == myStoneColor &&
					SgameBoard[point.first + 4][point.second] == myStoneColor)
				{
					return true;
				}

				// \대각선
				if (point.first < 15 && point.second > 3 &&
					SgameBoard[point.first][point.second] == myStoneColor &&
					SgameBoard[point.first + 1][point.second - 1] == myStoneColor &&
					SgameBoard[point.first + 2][point.second - 2] == myStoneColor &&
					SgameBoard[point.first + 3][point.second - 3] == myStoneColor &&
					SgameBoard[point.first + 4][point.second - 4] == myStoneColor)
				{
					return true;
				}

				// /대각선
				if (point.first < 15 && point.second < 15 &&
					SgameBoard[point.first][point.second] == myStoneColor &&
					SgameBoard[point.first + 1][point.second + 1] == myStoneColor &&
					SgameBoard[point.first + 2][point.second + 2] == myStoneColor &&
					SgameBoard[point.first + 3][point.second + 3] == myStoneColor &&
					SgameBoard[point.first + 4][point.second + 4] == myStoneColor)
				{
					return true;
				}
			}
		}
	}

	else 
	{
		for each (point in S->aiStone)
		{
			// 5줄(승리)
			if (S->aiStone.size() >= 5)
			{
				// 가로
				if (point.second < 15 &&
					SgameBoard[point.first][point.second] == aiStoneColor &&
					SgameBoard[point.first][point.second + 1] == aiStoneColor &&
					SgameBoard[point.first][point.second + 2] == aiStoneColor &&
					SgameBoard[point.first][point.second + 3] == aiStoneColor &&
					SgameBoard[point.first][point.second + 4] == aiStoneColor)
				{
					return true;
				}

				// 세로
				if (point.first < 15 &&
					SgameBoard[point.first][point.second] == aiStoneColor &&
					SgameBoard[point.first + 1][point.second] == aiStoneColor &&
					SgameBoard[point.first + 2][point.second] == aiStoneColor &&
					SgameBoard[point.first + 3][point.second] == aiStoneColor &&
					SgameBoard[point.first + 4][point.second] == aiStoneColor)
				{
					return true;
				}

				// \대각선
				if (point.first < 15 && point.second > 3 &&
					SgameBoard[point.first][point.second] == aiStoneColor &&
					SgameBoard[point.first + 1][point.second - 1] == aiStoneColor &&
					SgameBoard[point.first + 2][point.second - 2] == aiStoneColor &&
					SgameBoard[point.first + 3][point.second - 3] == aiStoneColor &&
					SgameBoard[point.first + 4][point.second - 4] == aiStoneColor)
				{
					return true;
				}

				// /대각선
				if (point.first < 15 && point.second < 15 &&
					SgameBoard[point.first][point.second] == aiStoneColor &&
					SgameBoard[point.first + 1][point.second + 1] == aiStoneColor &&
					SgameBoard[point.first + 2][point.second + 2] == aiStoneColor &&
					SgameBoard[point.first + 3][point.second + 3] == aiStoneColor &&
					SgameBoard[point.first + 4][point.second + 4] == aiStoneColor)
				{
					return true; 
				}
			}
		}
	}

	return false;
}

// 점수가 양수이면 플레이어에게, 음수이면 AI에게 유리. 
int Eval(State S)
{
	int sum = 0;
	pair<int, int> point;

	Stone SgameBoard[19][19];
	MakeGameboard(S, SgameBoard);

	// 플레이어 
	for each (point in S.myStone)
	{
		// 5줄(승리)
		if (S.myStone.size() >= 5)
		{
			// 가로
			if (point.second < 15 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first][point.second + 1] == myStoneColor &&
				SgameBoard[point.first][point.second + 2] == myStoneColor &&
				SgameBoard[point.first][point.second + 3] == myStoneColor &&
				SgameBoard[point.first][point.second + 4] == myStoneColor)
			{
				sum += 10000;
			}

			// 세로
			if (point.first < 15 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second] == myStoneColor &&
				SgameBoard[point.first + 2][point.second] == myStoneColor &&
				SgameBoard[point.first + 3][point.second] == myStoneColor &&
				SgameBoard[point.first + 4][point.second] == myStoneColor)
			{
				sum += 10000;
			}

			// \대각선
			if (point.first < 15 && point.second > 3 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second - 1] == myStoneColor &&
				SgameBoard[point.first + 2][point.second - 2] == myStoneColor &&
				SgameBoard[point.first + 3][point.second - 3] == myStoneColor &&
				SgameBoard[point.first + 4][point.second - 4] == myStoneColor)
			{
				sum += 10000;
			}

			// /대각선
			if (point.first < 15 && point.second < 15 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second + 1] == myStoneColor &&
				SgameBoard[point.first + 2][point.second + 2] == myStoneColor &&
				SgameBoard[point.first + 3][point.second + 3] == myStoneColor &&
				SgameBoard[point.first + 4][point.second + 4] == myStoneColor)
			{
				sum += 10000;
			}
		}

		// 4줄
		if (S.myStone.size() >= 4)
		{
		// 4칸 연속
			// 가로
			if (point.second < 16 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first][point.second + 1] == myStoneColor &&
				SgameBoard[point.first][point.second + 2] == myStoneColor &&
				SgameBoard[point.first][point.second + 3] == myStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.second + 4 == 19 || point.second - 1 == -1)
				{
					sum += 50;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first][point.second - 1] == Stone::none &&
						 SgameBoard[point.first][point.second + 4] == Stone::none)
				{
					sum += 100;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first][point.second - 1] == Stone::none &&
						  SgameBoard[point.first][point.second + 4] == aiStoneColor) ||
						 (SgameBoard[point.first][point.second - 1] == aiStoneColor &&
						  SgameBoard[point.first][point.second + 4] == Stone::none))
				{
					sum += 50;
				}
				
				// 둘다 막혀있을 때
				else if (SgameBoard[point.first][point.second - 1] == aiStoneColor &&
						 SgameBoard[point.first][point.second + 4] == aiStoneColor)
				{
					sum += 0;
				}
			}

			// 세로
			if (point.first < 16 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second] == myStoneColor &&
				SgameBoard[point.first + 2][point.second] == myStoneColor &&
				SgameBoard[point.first + 3][point.second] == myStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.first + 4 == 19 || point.first - 1 == -1)
				{
					sum += 50;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second] == Stone::none &&
					     SgameBoard[point.first + 4][point.second] == Stone::none)
				{
					sum += 100;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second] == Stone::none &&
					      SgameBoard[point.first + 4][point.second] == aiStoneColor) ||
					     (SgameBoard[point.first - 1][point.second] == aiStoneColor &&
						  SgameBoard[point.first + 4][point.second] == Stone::none))
				{
					sum += 50;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second] == aiStoneColor &&
					     SgameBoard[point.first + 4][point.second] == aiStoneColor)
				{
					sum += 0;
				}
			}

			// \대각선
			if (point.first < 16 && point.second > 2 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second - 1] == myStoneColor &&
				SgameBoard[point.first + 2][point.second - 2] == myStoneColor &&
				SgameBoard[point.first + 3][point.second - 3] == myStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 4 == 19 && point.second - 4 == -1) &&
					(point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 4 == 19 && point.second - 4 == -1) ||
						 (point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum += 50;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
					     SgameBoard[point.first + 4][point.second - 4] == Stone::none)
				{
					sum += 100;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
						  SgameBoard[point.first + 4][point.second - 4] == aiStoneColor) ||
						 (SgameBoard[point.first - 1][point.second + 1] == aiStoneColor &&
					      SgameBoard[point.first + 4][point.second - 4] == Stone::none))
				{
					sum += 50;
				}
				

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second + 1] == aiStoneColor &&
						 SgameBoard[point.first + 4][point.second - 4] == aiStoneColor)
				{
					sum += 0;
				}
			}

			// /대각선
			if (point.first < 16 && point.second < 16 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second + 1] == myStoneColor &&
				SgameBoard[point.first + 2][point.second + 2] == myStoneColor &&
				SgameBoard[point.first + 3][point.second + 3] == myStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 4 == 19 && point.second + 4 == 19) &&
					(point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 4 == 19 && point.second + 4 == 19) ||
					     (point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum += 50;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
					     SgameBoard[point.first + 4][point.second + 4] == Stone::none)
				{
					sum += 100;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
						  SgameBoard[point.first + 4][point.second + 4] == aiStoneColor) ||
					     (SgameBoard[point.first - 1][point.second - 1] == aiStoneColor &&
						  SgameBoard[point.first + 4][point.second + 4] == Stone::none))
				{
					sum += 50;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second - 1] == aiStoneColor &&
					     SgameBoard[point.first + 4][point.second + 4] == aiStoneColor)
				{
					sum += 0;
				}
			}

		// 두칸 두칸 사이에 없을 때

		// 세칸 한칸 사이에 없을 때

		// 한칸 세칸 사이에 없을 때

		}

		// 3줄 연속
		if (S.myStone.size() >= 3)
		{
			// 가로
			if (point.second < 17 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first][point.second + 1] == myStoneColor &&
				SgameBoard[point.first][point.second + 2] == myStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.second + 3 == 19 || point.second - 1 == -1)
				{
					sum += 10;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first][point.second - 1] == Stone::none &&
						 SgameBoard[point.first][point.second + 3] == Stone::none)
				{
					sum += 20;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first][point.second - 1] == Stone::none &&
						  SgameBoard[point.first][point.second + 3] == aiStoneColor) ||
						 (SgameBoard[point.first][point.second - 1] == aiStoneColor &&
						  SgameBoard[point.first][point.second + 3] == Stone::none))
				{
					sum += 10;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first][point.second - 1] == aiStoneColor &&
						 SgameBoard[point.first][point.second + 3] == aiStoneColor)
				{
					sum += 0;
				}
			}

			// 세로
			if (point.first < 17 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second] == myStoneColor &&
				SgameBoard[point.first + 2][point.second] == myStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.first + 3 == 19 || point.first - 1 == -1)
				{
					sum += 10;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second] == Stone::none &&
						 SgameBoard[point.first + 3][point.second] == Stone::none)
				{
					sum += 20;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second] == Stone::none &&
						  SgameBoard[point.first + 3][point.second] == aiStoneColor) ||
						 (SgameBoard[point.first - 1][point.second] == aiStoneColor &&
						  SgameBoard[point.first + 3][point.second] == Stone::none))
				{
					sum += 10;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second] == aiStoneColor &&
						 SgameBoard[point.first + 3][point.second] == aiStoneColor)
				{
					sum += 0;
				}
			}

			// \대각선
			if (point.first < 17 && point.second > 1 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second - 1] == myStoneColor &&
				SgameBoard[point.first + 2][point.second - 2] == myStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 3 == 19 && point.second - 3 == -1) &&
					(point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 3 == 19 && point.second - 3 == -1) ||
						 (point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum += 10;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
						 SgameBoard[point.first + 3][point.second - 3] == Stone::none)
				{
					sum += 20;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
						  SgameBoard[point.first + 3][point.second - 3] == aiStoneColor) ||
						 (SgameBoard[point.first - 1][point.second + 1] == aiStoneColor &&
						  SgameBoard[point.first + 3][point.second - 3] == Stone::none))
				{
					sum += 10;
				}


				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second + 1] == aiStoneColor &&
						 SgameBoard[point.first + 3][point.second - 3] == aiStoneColor)
				{
					sum += 0;
				}
			}

			// /대각선
			if (point.first < 17 && point.second < 17 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second + 1] == myStoneColor &&
				SgameBoard[point.first + 2][point.second + 2] == myStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 3 == 19 && point.second + 3 == 19) &&
					(point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 3 == 19 && point.second + 3 == 19) ||
						 (point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum += 10;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
						 SgameBoard[point.first + 3][point.second + 3] == Stone::none)
				{
					sum += 20;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
						  SgameBoard[point.first + 3][point.second + 3] == aiStoneColor) ||
						 (SgameBoard[point.first - 1][point.second - 1] == aiStoneColor &&
						  SgameBoard[point.first + 3][point.second + 3] == Stone::none))
				{
					sum += 10;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second - 1] == aiStoneColor &&
						 SgameBoard[point.first + 3][point.second + 3] == aiStoneColor)
				{
					sum += 0;
				}
			}
		}

		// 2줄 연속
		if (S.myStone.size() >= 2)
		{
			// 가로
			if (point.second < 18 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first][point.second + 1] == myStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.second + 2 == 19 || point.second - 1 == -1)
				{
					sum += 1;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first][point.second - 1] == Stone::none &&
						 SgameBoard[point.first][point.second + 2] == Stone::none)
				{
					sum += 3;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first][point.second - 1] == Stone::none &&
						  SgameBoard[point.first][point.second + 2] == aiStoneColor) ||
						 (SgameBoard[point.first][point.second - 1] == aiStoneColor &&
						  SgameBoard[point.first][point.second + 2] == Stone::none))
				{
					sum += 1;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first][point.second - 1] == aiStoneColor &&
						 SgameBoard[point.first][point.second + 2] == aiStoneColor)
				{
					sum += 0;
				}
			}

			// 세로
			if (point.first < 18 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second] == myStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.first + 2 == 19 || point.first - 1 == -1)
				{
					sum += 1;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second] == Stone::none &&
						 SgameBoard[point.first + 2][point.second] == Stone::none)
				{
					sum += 3;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second] == Stone::none &&
						  SgameBoard[point.first + 2][point.second] == aiStoneColor) ||
						 (SgameBoard[point.first - 1][point.second] == aiStoneColor &&
						  SgameBoard[point.first + 2][point.second] == Stone::none))
				{
					sum += 1;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second] == aiStoneColor &&
						 SgameBoard[point.first + 2][point.second] == aiStoneColor)
				{
					sum += 0;
				}
			}

			// \대각선
			if (point.first < 18 && point.second > 0 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second - 1] == myStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 2 == 19 && point.second - 2 == -1) &&
					(point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 2 == 19 && point.second - 2 == -1) ||
						 (point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum += 1;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
						 SgameBoard[point.first + 2][point.second - 2] == Stone::none)
				{
					sum += 3;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
						  SgameBoard[point.first + 2][point.second - 2] == aiStoneColor) ||
						 (SgameBoard[point.first - 1][point.second + 1] == aiStoneColor &&
						  SgameBoard[point.first + 2][point.second - 2] == Stone::none))
				{
					sum += 1;
				}


				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second + 1] == aiStoneColor &&
						 SgameBoard[point.first + 2][point.second - 2] == aiStoneColor)
				{
					sum += 0;
				}
			}

			// /대각선
			if (point.first < 18 && point.second < 18 &&
				SgameBoard[point.first][point.second] == myStoneColor &&
				SgameBoard[point.first + 1][point.second + 1] == myStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 2 == 19 && point.second + 2 == 19) &&
					(point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 2 == 19 && point.second + 2 == 19) ||
						 (point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum += 1;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
						 SgameBoard[point.first + 2][point.second + 2] == Stone::none)
				{
					sum += 3;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
						  SgameBoard[point.first + 2][point.second + 2] == aiStoneColor) ||
						 (SgameBoard[point.first - 1][point.second - 1] == aiStoneColor &&
						  SgameBoard[point.first + 2][point.second + 2] == Stone::none))
				{
					sum += 1;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second - 1] == aiStoneColor &&
						 SgameBoard[point.first + 2][point.second + 2] == aiStoneColor)
				{
					sum += 0;
				}
			}
		}
	}

	// AI
	for each (point in S.aiStone)
	{
		// 5줄(승리)
		if (S.aiStone.size() >= 5)
		{
			// 가로
			if (point.second < 15 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first][point.second + 1] == aiStoneColor &&
				SgameBoard[point.first][point.second + 2] == aiStoneColor &&
				SgameBoard[point.first][point.second + 3] == aiStoneColor &&
				SgameBoard[point.first][point.second + 4] == aiStoneColor)
			{
				sum -= 10000;
			}

			// 세로
			if (point.first < 15 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second] == aiStoneColor &&
				SgameBoard[point.first + 2][point.second] == aiStoneColor &&
				SgameBoard[point.first + 3][point.second] == aiStoneColor &&
				SgameBoard[point.first + 4][point.second] == aiStoneColor)
			{
				sum -= 10000;
			}

			// \대각선
			if (point.first < 15 && point.second > 3 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second - 1] == aiStoneColor &&
				SgameBoard[point.first + 2][point.second - 2] == aiStoneColor &&
				SgameBoard[point.first + 3][point.second - 3] == aiStoneColor &&
				SgameBoard[point.first + 4][point.second - 4] == aiStoneColor)
			{
				sum -= 10000;
			}

			// /대각선
			if (point.first < 15 && point.second < 15 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second + 1] == aiStoneColor &&
				SgameBoard[point.first + 2][point.second + 2] == aiStoneColor &&
				SgameBoard[point.first + 3][point.second + 3] == aiStoneColor &&
				SgameBoard[point.first + 4][point.second + 4] == aiStoneColor)
			{
				sum -= 10000;
			}
		}

		// 4줄
		if (S.aiStone.size() >= 4)
		{
			// 4칸 연속
			// 가로
			if (point.second < 16 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first][point.second + 1] == aiStoneColor &&
				SgameBoard[point.first][point.second + 2] == aiStoneColor &&
				SgameBoard[point.first][point.second + 3] == aiStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.second + 4 == 19 || point.second - 1 == -1)
				{
					sum -= 50;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first][point.second - 1] == Stone::none &&
					SgameBoard[point.first][point.second + 4] == Stone::none)
				{
					sum -= 100;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first][point.second - 1] == Stone::none &&
					SgameBoard[point.first][point.second + 4] == myStoneColor) ||
					(SgameBoard[point.first][point.second - 1] == myStoneColor &&
						SgameBoard[point.first][point.second + 4] == Stone::none))
				{
					sum -= 50;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first][point.second - 1] == myStoneColor &&
					SgameBoard[point.first][point.second + 4] == myStoneColor)
				{
					sum += 0;
				}
			}

			// 세로
			if (point.first < 16 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second] == aiStoneColor &&
				SgameBoard[point.first + 2][point.second] == aiStoneColor &&
				SgameBoard[point.first + 3][point.second] == aiStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.first + 4 == 19 || point.first - 1 == -1)
				{
					sum -= 50;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second] == Stone::none &&
					SgameBoard[point.first + 4][point.second] == Stone::none)
				{
					sum -= 100;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second] == Stone::none &&
					SgameBoard[point.first + 4][point.second] == aiStoneColor) ||
					(SgameBoard[point.first - 1][point.second] == aiStoneColor &&
						SgameBoard[point.first + 4][point.second] == Stone::none))
				{
					sum -= 50;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second] == aiStoneColor &&
					SgameBoard[point.first + 4][point.second] == aiStoneColor)
				{
					sum += 0;
				}
			}

			// \대각선
			if (point.first < 16 && point.second > 2 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second - 1] == aiStoneColor &&
				SgameBoard[point.first + 2][point.second - 2] == aiStoneColor &&
				SgameBoard[point.first + 3][point.second - 3] == aiStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 4 == 19 && point.second - 4 == -1) &&
					(point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 4 == 19 && point.second - 4 == -1) ||
					(point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum -= 50;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
					SgameBoard[point.first + 4][point.second - 4] == Stone::none)
				{
					sum -= 100;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
					SgameBoard[point.first + 4][point.second - 4] == myStoneColor) ||
					(SgameBoard[point.first - 1][point.second + 1] == myStoneColor &&
						SgameBoard[point.first + 4][point.second - 4] == Stone::none))
				{
					sum -= 50;
				}


				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second + 1] == myStoneColor &&
					SgameBoard[point.first + 4][point.second - 4] == myStoneColor)
				{
					sum += 0;
				}
			}

			// /대각선
			if (point.first < 16 && point.second < 16 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second + 1] == aiStoneColor &&
				SgameBoard[point.first + 2][point.second + 2] == aiStoneColor &&
				SgameBoard[point.first + 3][point.second + 3] == aiStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 4 == 19 && point.second + 4 == 19) &&
					(point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 4 == 19 && point.second + 4 == 19) ||
					(point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum -= 50;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
					SgameBoard[point.first + 4][point.second + 4] == Stone::none)
				{
					sum -= 100;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
					SgameBoard[point.first + 4][point.second + 4] == myStoneColor) ||
					(SgameBoard[point.first - 1][point.second - 1] == myStoneColor &&
						SgameBoard[point.first + 4][point.second + 4] == Stone::none))
				{
					sum -= 50;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second - 1] == myStoneColor &&
					SgameBoard[point.first + 4][point.second + 4] == myStoneColor)
				{
					sum -= 0;
				}
			}

			// 두칸 두칸 사이에 없을 때

			// 세칸 한칸 사이에 없을 때

			// 한칸 세칸 사이에 없을 때

		}

		// 3줄 연속
		if (S.aiStone.size() >= 3)
		{
			// 가로
			if (point.second < 17 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first][point.second + 1] == aiStoneColor &&
				SgameBoard[point.first][point.second + 2] == aiStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.second + 3 == 19 || point.second - 1 == -1)
				{
					sum -= 10;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first][point.second - 1] == Stone::none &&
					SgameBoard[point.first][point.second + 3] == Stone::none)
				{
					sum -= 20;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first][point.second - 1] == Stone::none &&
					SgameBoard[point.first][point.second + 3] == myStoneColor) ||
					(SgameBoard[point.first][point.second - 1] == myStoneColor &&
						SgameBoard[point.first][point.second + 3] == Stone::none))
				{
					sum -= 10;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first][point.second - 1] == myStoneColor &&
					SgameBoard[point.first][point.second + 3] == myStoneColor)
				{
					sum += 0;
				}
			}

			// 세로
			if (point.first < 17 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second] == aiStoneColor &&
				SgameBoard[point.first + 2][point.second] == aiStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.first + 3 == 19 || point.first - 1 == -1)
				{
					sum -= 10;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second] == Stone::none &&
					SgameBoard[point.first + 3][point.second] == Stone::none)
				{
					sum -= 20;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second] == Stone::none &&
					SgameBoard[point.first + 3][point.second] == myStoneColor) ||
					(SgameBoard[point.first - 1][point.second] == myStoneColor &&
						SgameBoard[point.first + 3][point.second] == Stone::none))
				{
					sum -= 10;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second] == myStoneColor &&
					SgameBoard[point.first + 3][point.second] == myStoneColor)
				{
					sum += 0;
				}
			}

			// \대각선
			if (point.first < 17 && point.second > 1 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second - 1] == aiStoneColor &&
				SgameBoard[point.first + 2][point.second - 2] == aiStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 3 == 19 && point.second - 3 == -1) &&
					(point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 3 == 19 && point.second - 3 == -1) ||
					(point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum -= 10;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
					SgameBoard[point.first + 3][point.second - 3] == Stone::none)
				{
					sum -= 20;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
					SgameBoard[point.first + 3][point.second - 3] == myStoneColor) ||
					(SgameBoard[point.first - 1][point.second + 1] == myStoneColor &&
						SgameBoard[point.first + 3][point.second - 3] == Stone::none))
				{
					sum -= 10;
				}


				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second + 1] == myStoneColor &&
					SgameBoard[point.first + 3][point.second - 3] == myStoneColor)
				{
					sum -= 0;
				}
			}

			// /대각선
			if (point.first < 17 && point.second < 17 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second + 1] == aiStoneColor &&
				SgameBoard[point.first + 2][point.second + 2] == aiStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 3 == 19 && point.second + 3 == 19) &&
					(point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 3 == 19 && point.second + 3 == 19) ||
					(point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum -= 10;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
					SgameBoard[point.first + 3][point.second + 3] == Stone::none)
				{
					sum -= 20;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
					SgameBoard[point.first + 3][point.second + 3] == myStoneColor) ||
					(SgameBoard[point.first - 1][point.second - 1] == myStoneColor &&
						SgameBoard[point.first + 3][point.second + 3] == Stone::none))
				{
					sum -= 10;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second - 1] == myStoneColor &&
					SgameBoard[point.first + 3][point.second + 3] == myStoneColor)
				{
					sum += 0;
				}
			}
		}

		// 2줄 연속
		if (S.aiStone.size() >= 2)
		{
			// 가로
			if (point.second < 18 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first][point.second + 1] == aiStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.second + 2 == 19 || point.second - 1 == -1)
				{
					sum -= 1;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first][point.second - 1] == Stone::none &&
					SgameBoard[point.first][point.second + 2] == Stone::none)
				{
					sum -= 3;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first][point.second - 1] == Stone::none &&
					SgameBoard[point.first][point.second + 2] == myStoneColor) ||
					(SgameBoard[point.first][point.second - 1] == myStoneColor &&
						SgameBoard[point.first][point.second + 2] == Stone::none))
				{
					sum -= 1;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first][point.second - 1] == myStoneColor &&
					SgameBoard[point.first][point.second + 2] == myStoneColor)
				{
					sum += 0;
				}
			}

			// 세로
			if (point.first < 18 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second] == aiStoneColor)
			{
				// 놓을 곳 중에 하나가 게임 판 밖일 때
				if (point.first + 2 == 19 || point.first - 1 == -1)
				{
					sum -= 1;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second] == Stone::none &&
					SgameBoard[point.first + 2][point.second] == Stone::none)
				{
					sum -= 3;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second] == Stone::none &&
					SgameBoard[point.first + 2][point.second] == myStoneColor) ||
					(SgameBoard[point.first - 1][point.second] == myStoneColor &&
						SgameBoard[point.first + 2][point.second] == Stone::none))
				{
					sum -= 1;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second] == myStoneColor &&
					SgameBoard[point.first + 2][point.second] == myStoneColor)
				{
					sum += 0;
				}
			}

			// \대각선
			if (point.first < 18 && point.second > 0 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second - 1] == aiStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 2 == 19 && point.second - 2 == -1) &&
					(point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 2 == 19 && point.second - 2 == -1) ||
					(point.first - 1 == -1 && point.second + 1 == 19))
				{
					sum -= 1;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
					SgameBoard[point.first + 2][point.second - 2] == Stone::none)
				{
					sum -= 3;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second + 1] == Stone::none &&
					SgameBoard[point.first + 2][point.second - 2] == myStoneColor) ||
					(SgameBoard[point.first - 1][point.second + 1] == myStoneColor &&
						SgameBoard[point.first + 2][point.second - 2] == Stone::none))
				{
					sum -= 1;
				}


				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second + 1] == myStoneColor &&
					SgameBoard[point.first + 2][point.second - 2] == myStoneColor)
				{
					sum += 0;
				}
			}

			// /대각선
			if (point.first < 18 && point.second < 18 &&
				SgameBoard[point.first][point.second] == aiStoneColor &&
				SgameBoard[point.first + 1][point.second + 1] == aiStoneColor)
			{
				// 놓을 곳 둘다 게임 판 밖일 때
				if ((point.first + 2 == 19 && point.second + 2 == 19) &&
					(point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum += 0;
				}

				// 놓을 곳 중에 하나가 게임 판 밖일 때
				else if ((point.first + 2 == 19 && point.second + 2 == 19) ||
					(point.first - 1 == -1 && point.second - 1 == -1))
				{
					sum -= 1;
				}

				// 양 옆이 둘다 비어있을 경우
				else if (SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
					SgameBoard[point.first + 2][point.second + 2] == Stone::none)
				{
					sum -= 3;
				}

				// 한쪽만 막혀 있을 때
				else if ((SgameBoard[point.first - 1][point.second - 1] == Stone::none &&
					SgameBoard[point.first + 2][point.second + 2] == myStoneColor) ||
					(SgameBoard[point.first - 1][point.second - 1] == myStoneColor &&
						SgameBoard[point.first + 2][point.second + 2] == Stone::none))
				{
					sum -= 1;
				}

				// 둘다 막혀있을 때
				else if (SgameBoard[point.first - 1][point.second - 1] == myStoneColor &&
					SgameBoard[point.first + 2][point.second + 2] == myStoneColor)
				{
					sum += 0;
				}
			}
		}
	}

    return sum;
}

bool CutoffTest(State S, int d)
{
	if (S.depth >= d)
		return true;
	else return false;
}

State MiniMax(State S, int depthLimit, int alpha, int beta)
{
    if(CutoffTest(S, depthLimit))
    {
        S.value = Eval(S);
        return S;
    }

    // Max
    if(S.turn == Turn::MIN)
    {
		int _random = 2;
        State result;
        State MaxState = S;

        for(int i = 0; i < S.child.size(); i++)
        {
            result = MiniMax(*S.child[i], depthLimit, alpha, beta);
            if(alpha < result.value)
            {
                alpha = result.value;
                MaxState = result;
				_random = 2;
            }
			// 같은 경우 무작위로 선택. 갈 수록 있던 녀석이 남아있을 확률이 증가
			else if (alpha == result.value)
			{
				// 바뀔 확률
				if (rand() % _random >= _random - 1)
				{
					MaxState = result;
				}
				_random++;
			}
			// beta cut-off
			if (beta <= alpha)
				break;
        }
        return MaxState;
    }

    // Min
    else
    {
		int _random = 2;
        State result;
        State MinState = S;
        for(int i = 0; i < S.child.size(); i++)
        {
            result = MiniMax(*S.child[i], depthLimit, alpha, beta);
            if(beta > result.value)
            {
                beta = result.value;
                MinState = result;
				_random = 2;
            }
			// 같은 경우 무작위로 선택. 갈 수록 있던 녀석이 남아있을 확률이 증가
			else if (beta == result.value)
			{
				// 바뀔 확률
				if (rand() % _random >= _random - 1)
				{
					MinState = result;
				}
				_random++;
			}
			// alpha cut-off
			if (beta <= alpha)
				break;
        }
        return MinState;
    }
}


