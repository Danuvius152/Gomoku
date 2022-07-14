// https://www.xqbase.com/computer/outline.htm
#pragma GCC optimize(2)
#include <cstring>
#include <queue>
#include <set>
#include <utility>
#include <vector>

#include "AIController.h"
using namespace std;
extern int ai_side; // 0: black, 1: white
std::string ai_name = "Celty";
const int MAX = 10000000;
const int MIN = -10000000;
const int Unknown = 10000010;
int DEPTH = 8;
int turn = 0;
int chess = 0;
int board[15][15];
int score[2][72];
int chess_score[2]; // chess_score[0]是自己

int directions[8][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

long long Zobrist, boardZobrist[15][15][2];

bool out_of_bound(int x, int y)
{
    if (x < 0 || x >= 15 || y < 0 || y >= 15)
        return true;
    else
        return false;
}
//生成64位随机数
long long random64() { return rand() | ((long long)rand() << 32); }

//生成zobrist键值
void randomBoardZobrist()
{
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            for (int k = 0; k < 2; k++)
                boardZobrist[i][j][k] = random64();
}

struct Pattern
{
    string p;
    int n;
};
vector<Pattern> patterns = {
    {"11111", 500000},
    {"011110", 10000},
    {"011100", 1000},
    {"001110", 1000},
    {"01110", 1000},
    {"011010", 1000},
    {"010110", 1000},
    {"01111", 1000},
    {"11110", 1000},
    {"11011", 1000},
    {"11101", 1000},
    {"10111", 1000},
    {"001100", 200},
    {"001010", 200},
    {"010100", 200},
    {"000100", 40},
    {"001000", 40},
};

class searcher
{
    struct Node // trie树node
    {
        vector<int> exist;
        Node *fail;
        Node *child[2];
        Node()
        {
            fail = nullptr;
            child[0] = child[1] = nullptr;
        }
        ~Node()
        {
            fail = nullptr;
            child[0] = child[1] = nullptr;
        }
    };

private:
    Node *root;

public:
    searcher() { root = new Node(); }
    ~searcher() { destroy(root); }
    void trie_insert(Node *root, int k)
    {
        Node *tmp = root;
        for (int i = 0; i < patterns[k].p.size(); i++)
        {
            int c = patterns[k].p[i] - '0';
            if (nullptr == tmp->child[c])
                tmp->child[c] = new Node();
            tmp = tmp->child[c];
        }
        tmp->exist.push_back(k);
    }

    void ac_build()
    {
        for (int i = 0; i < 17; i++)
        {
            trie_insert(root, i);
        }
        queue<Node *> q;
        for (int i = 0; i < 2; i++)
        {
            if (root->child[i])
            {
                root->child[i]->fail = root;
                q.push(root->child[i]);
            }
        }
        while (!q.empty())
        {
            Node *x = q.front();
            q.pop();
            for (int i = 0; i < 2; i++)
            {
                if (x->child[i])
                {
                    Node *y = x->child[i], *fa_fail = x->fail;
                    while (fa_fail && nullptr == fa_fail->child[i])
                        fa_fail = fa_fail->fail;
                    if (nullptr == fa_fail)
                        y->fail = root;
                    else
                        y->fail = fa_fail->child[i];

                    if (y->fail->exist.size())
                        for (int j = 0; j < y->fail->exist.size(); j++)
                            y->exist.push_back(y->fail->exist[j]);
                    q.push(y);
                }
            }
        }
    }
    int ac_query(Node *root, string T)
    {
        int score = 0;
        Node *tmp = root;
        for (int i = 0; i < T.size(); i++)
        {
            int c = T[i] - '0';
            while (nullptr == tmp->child[c] && tmp->fail)
                tmp = tmp->fail;
            if (tmp->child[c])
                tmp = tmp->child[c];
            else
                continue;
            if (tmp->exist.size())
            {
                for (int j = 0; j < tmp->exist.size(); j++)
                {
                    int x = tmp->exist[j];
                    score += patterns[x].n;
                }
            }
        }
        return score;
    }
    void destroy(Node *root)
    {
        if (root)
            return;
        if (!root->child[0])
            destroy(root->child[0]);
        if (!root->child[1])
            destroy(root->child[1]);
        delete root;
        root = nullptr;
    }
    int get_score(string T) { return ac_query(root, T); }

} ACsearch;

int evaluate(int x, int y)
{ //该点必为空
  //敌我一视同仁
    int res = 0;
    string lines[4][2];
    for (int i = max(0, x - 5); i < min(15, x + 6); i++)
    {
        if (i != x)
        {
            int a = board[i][y] == ai_side ? '1'
                    : board[i][y] == -1    ? '0'
                                           : '2';
            int b = board[i][y] == ai_side ? '2'
                    : board[i][y] == -1    ? '0'
                                           : '1';
            lines[0][0].push_back(a);
            lines[0][1].push_back(b);
        }
        else
        {
            lines[0][0].push_back('1');
            lines[0][1].push_back('1');
        }
    }
    for (int i = max(0, y - 5); i < min(15, y + 6); i++)
    {
        if (i != y)
        {
            int a = board[x][i] == ai_side ? '1'
                    : board[x][i] == -1    ? '0'
                                           : '2';
            int b = board[x][i] == ai_side ? '2'
                    : board[x][i] == -1    ? '0'
                                           : '1';
            lines[1][0].push_back(a);
            lines[1][1].push_back(b);
        }
        else
        {
            lines[1][0].push_back('1');
            lines[1][1].push_back('1');
        }
    }
    int i, j;
    for (i = x - min(min(x, y), 5), j = y - min(min(x, y), 5);
         i < min(15, x + 6) && j < min(15, y + 6); i++, j++)
    {
        if (i != x)
        {
            int a = board[i][j] == ai_side ? '1'
                    : board[i][j] == -1    ? '0'
                                           : '2';
            int b = board[i][j] == ai_side ? '2'
                    : board[i][j] == -1    ? '0'
                                           : '1';
            lines[2][0].push_back(a);
            lines[2][1].push_back(b);
        }
        else
        {
            lines[2][0].push_back('1');
            lines[2][1].push_back('1');
        }
    }
    for (i = x - min(min(14 - y, x), 5), j = y + min(min(14 - y, x), 5);
         i < min(15, x + 6) && j >= max(0, y - 5); i++, j--)
    {
        if (i != x)
        {
            int a = board[i][j] == ai_side ? '1'
                    : board[i][j] == -1    ? '0'
                                           : '2';
            int b = board[i][j] == ai_side ? '2'
                    : board[i][j] == -1    ? '0'
                                           : '1';
            lines[3][0].push_back(a);
            lines[3][1].push_back(b);
        }
        else
        {
            lines[3][0].push_back('1');
            lines[3][1].push_back('1');
        }
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            res += ACsearch.get_score(lines[i][j]);
        }
    }
    return res;
}

struct Coordinate
{
    int x, y, score;
    Coordinate(int _x = 0, int _y = 0, int _score = 0)
        : x(_x), y(_y), score(_score) {}
    Coordinate operator+(const Coordinate &b) const
    {
        return Coordinate(x + b.x, y + b.y);
    }
    Coordinate operator-(const Coordinate &b) const
    {
        return Coordinate(x - b.x, y - b.y);
    }
    Coordinate operator*(int b) const { return Coordinate(x * b, y * b); }
    bool operator<(const Coordinate &b) const
    {
        return score == b.score ? (x == b.x ? y < b.y : x < b.x)
                                : score > b.score;
    }
} next_step;

void update_score(Coordinate p)
{ //更新整个棋盘
    int x = p.x, y = p.y;
    string lines[4][2];
    for (int i = 0; i < 15; i++)
    {
        int a = board[i][y] == ai_side ? '1' : board[i][y] == -1 ? '0'
                                                                 : '2';
        int b = board[i][y] == ai_side ? '2' : board[i][y] == -1 ? '0'
                                                                 : '1';
        lines[0][0].push_back(a);
        lines[0][1].push_back(b);
    }
    for (int i = 0; i < 15; i++)
    {
        int a = board[x][i] == ai_side ? '1' : board[x][i] == -1 ? '0'
                                                                 : '2';
        int b = board[x][i] == ai_side ? '2' : board[x][i] == -1 ? '0'
                                                                 : '1';
        lines[1][0].push_back(a);
        lines[1][1].push_back(b);
    }
    int i, j;
    for (i = x - min(x, y), j = y - min(x, y); i < 15 && j < 15; i++, j++)
    {
        int a = board[i][j] == ai_side ? '1' : board[i][j] == -1 ? '0'
                                                                 : '2';
        int b = board[i][j] == ai_side ? '2' : board[i][j] == -1 ? '0'
                                                                 : '1';
        lines[2][0].push_back(a);
        lines[2][1].push_back(b);
    }
    for (i = x - min(14 - y, x), j = y + min(14 - y, x); i < 15 && j >= 0;
         i++, j--)
    {
        int a = board[i][j] == ai_side ? '1' : board[i][j] == -1 ? '0'
                                                                 : '2';
        int b = board[i][j] == ai_side ? '2' : board[i][j] == -1 ? '0'
                                                                 : '1';
        lines[3][0].push_back(a);
        lines[3][1].push_back(b);
    }
    int x_0 = y;          //竖
    int x_1 = 15 + x;     //横
    int x_2 = 40 + y - x; //捺 y在[x-10,x+10]
    int x_3 = 47 + x + y; //撇 x+y在[4,24]

    //先处理横竖：剪掉未落子后加上落子
    for (int i = 0; i < 2; i++)
    {
        chess_score[i] -= score[i][x_0];
        chess_score[i] -= score[i][x_1];
        score[i][x_0] = ACsearch.get_score(lines[0][i]);
        score[i][x_1] = ACsearch.get_score(lines[1][i]);
        chess_score[i] += score[i][x_0];
        chess_score[i] += score[i][x_1];
    }
    //再处理撇捺
    if (y - x >= -10 && y - x <= 10)
    {
        for (int i = 0; i < 2; i++)
        {
            chess_score[i] -= score[i][x_2];
            score[i][x_2] = ACsearch.get_score(lines[2][i]);
            chess_score[i] += score[i][x_2];
        }
    }
    if (y + x >= 4 && y + x <= 24)
    {
        for (int i = 0; i < 2; i++)
        {
            chess_score[i] -= score[i][x_3];
            score[i][x_3] = ACsearch.get_score(lines[3][i]);
            chess_score[i] += score[i][x_3];
        }
    }
}

struct possible_Coordinate
{
    set<Coordinate> cur_possible; //去重，无需map
    vector<pair<set<Coordinate>, Coordinate>> his;

    void add(const Coordinate &p)
    {
        int x = p.x, y = p.y;
        pair<set<Coordinate>, Coordinate> history; //储存 p的四周新加入的点与p
        if (turn < 9)
        {
            for (int i = 0; i < 8; i++)
            {
                if (out_of_bound(x + directions[i][0], y + directions[i][1]) ||
                    board[x + directions[i][0]][y + directions[i][1]] != -1)
                    continue;
                Coordinate pos(x + directions[i][0], y + directions[i][1]);
                if (cur_possible.insert(pos).second)
                    history.first.insert(pos);
            }
        }
        else
        {
            for (int i = 0; i < 8; i++)
            {
                for (int j = 1; j <= 2; j++)
                {
                    if (out_of_bound(x + j * directions[i][0],
                                     y + j * directions[i][1]))
                        continue;
                    if (board[x + j * directions[i][0]]
                             [y + j * directions[i][1]] == -1)
                    {
                        Coordinate pos(x + j * directions[i][0],
                                       y + j * directions[i][1]);
                        if (cur_possible.insert(pos).second)
                            history.first.insert(pos);
                    }
                }
            }
        }
        cur_possible.erase(p); //本算法一定从cur_possible里选点
        history.second = p;

        his.push_back(history);
    }
    void rollback()
    {
        if (cur_possible.empty())
            return;
        auto history = his.back();
        his.pop_back();
        for (auto it = history.first.begin(); it != history.first.end(); ++it)
            cur_possible.erase(*it); //清除掉前一步加入的点

        cur_possible.insert(history.second); //加入前一步删除的点
    }
} possible_position;

enum idx
{
    EMPTY,
    ALPHA,
    BETA,
    EXACT
};
struct Hash
{
    long long key;
    int depth;
    int score;
    idx state;
} hashs[0xffff];

void record(int depth, int score, idx state)
{
    Hash *hash = &hashs[Zobrist & 0xffff];
    if (hash->state != EMPTY && hash->depth > depth)
        return; //“深度优先覆盖”策略
    hash->key = Zobrist;
    hash->depth = depth;
    hash->score = score;
    hash->state = state;
}

void clear_hash()
{
    for (int i = 0; i < 0xffff; i++)
        hashs[i].state = EMPTY;
}

int getHashScore(int depth, int alpha, int beta)
{
    Hash *hash = &hashs[Zobrist & 0xffff];

    if (hash->state == EMPTY)
        return Unknown;

    if (hash->key == Zobrist && depth != DEPTH)
    {
        if (hash->depth >= depth)
        { //储存的depth搜得更深才有效
            if (hash->state == EXACT)
                return hash->score;
            if (hash->state == ALPHA && hash->score <= alpha)
                return alpha;
            if (hash->state == BETA && hash->score >= beta)
                return beta;
        }
    }
    return Unknown;
}

// alpha-beta剪枝
// alpha:当前搜索到的最大值
// beta: 对手给予的最小值
int AB_search(int depth, int alpha, int beta, int role)
{
    idx state = ALPHA;

    if (depth != DEPTH)
    {
        int score = getHashScore(depth, alpha, beta);
        if (score != Unknown)
            return score;
    }

    int my_score = (role == ai_side) ? chess_score[0] : chess_score[1];
    int base_score = (role == ai_side) ? chess_score[1] : chess_score[0];
    if (my_score >= 50000)
        return MAX - (DEPTH - depth);
    if (base_score >= 50000)
        return MIN + (DEPTH - depth);
    if (depth == 0)
    {
        record(depth, my_score - base_score, EXACT);
        return my_score - base_score;
    }

    set<Coordinate> possible;
    for (auto it = possible_position.cur_possible.begin();
         it != possible_position.cur_possible.end(); ++it)
    {
        possible.insert(Coordinate(it->x, it->y, evaluate(it->x, it->y)));
    }

    int cnt = 8;
    while (cnt-- && !possible.empty())
    {
        Coordinate p = *possible.begin();
        possible.erase(possible.begin());

        board[p.x][p.y] = role;
        Zobrist ^= boardZobrist[p.x][p.y][role];
        update_score(p);
        p.score = 0; // essential for set.find()
        //全局的cur_possible中score均为0
        possible_position.add(p);

        int val = -AB_search(depth - 1, -beta, -alpha, 1 - role);
        // alpha = max(val), beta = min(val)
        possible_position.rollback();
        board[p.x][p.y] = -1;
        Zobrist ^= boardZobrist[p.x][p.y][role];
        update_score(p);

        if (val >= beta)
        {
            record(depth, beta, BETA);
            return beta;
        }
        if (val > alpha)
        {
            state = EXACT;
            alpha = val;
            if (depth == DEPTH)
                next_step = p;
        }
    }

    record(depth, alpha, state);
    return alpha;
}

void flip()
{
    swap(chess_score[0], chess_score[1]);
    swap(score[0], score[1]);

    for (int i = 0; i < 15; ++i)
        for (int j = 0; j < 15; ++j)
            if (board[i][j] != -1)
            {
                board[i][j] ^= 1;
                Zobrist ^= boardZobrist[i][j][0];
                Zobrist ^= boardZobrist[i][j][1];
            }
}

std::pair<int, int> generate_next_step()
{
    chess++;
    if (chess == 225)
    {
        auto t = possible_position.cur_possible.begin();
        return {t->x, t->y};
    }
    if (chess >= 218)
        DEPTH = 225 - chess;

    AB_search(DEPTH, MIN, MAX, ai_side);
    clear_hash();

    board[next_step.x][next_step.y] = ai_side;
    Zobrist ^= boardZobrist[next_step.x][next_step.y][ai_side];
    update_score(next_step);
    possible_position.add(next_step);

    cerr << "My score: " << chess_score[0] << endl
         << "Baseline score: " << chess_score[1] << endl;
    return {next_step.x, next_step.y};
}

// init function is called once at the beginning
void init()
{
    memset(board, -1, sizeof(board));
    ACsearch.ac_build();
    Zobrist = random64();
    randomBoardZobrist();
}

// loc is the action of your opponent
// Initially, loc being (-1,-1) means it's your first move
// If this is the third step(with 2 black ), where you can use the swap rule,
// your output could be either (-1, -1) to indicate that you choose a swap, or a
// coordinate (x,y) as normal.
std::pair<int, int> action(std::pair<int, int> loc)
{
    turn++;
    int x = loc.first, y = loc.second;
    if (x == -1 && y == -1)
    {
        if (turn == 1)
        {
            chess++;
            Zobrist ^= boardZobrist[2][2][ai_side];
            board[2][2] = ai_side;
            update_score(Coordinate(2, 2));
            possible_position.add(Coordinate(2, 2));

            cerr << "My score: " << chess_score[0] << endl
                 << "Baseline score: " << chess_score[1] << endl;
            return {2, 2};
        }
        if (turn == 3)
        { //应对反转
            flip();
            cerr << "My score: " << chess_score[0] << endl
                 << "Baseline score: " << chess_score[1] << endl;
            return generate_next_step();
        }
    }

    //保存对手
    chess++;
    board[x][y] = 1 - ai_side;
    Zobrist ^= boardZobrist[x][y][1 - ai_side];
    update_score(Coordinate(x, y));
    possible_position.add(Coordinate(x, y));

    //生成下一步
    if (turn == 2 && ai_side == 0)
    {
        chess++;
        if (abs(x - 2) + abs(y - 2) > 10)
        {
            next_step = Coordinate(x - 1, y - 1);
            board[next_step.x][next_step.y] = ai_side;
            Zobrist ^= boardZobrist[next_step.x][next_step.y][ai_side];
            update_score(next_step);
            possible_position.add(next_step);

            cerr << "My score: " << chess_score[0] << endl
                 << "Baseline score: " << chess_score[1] << endl;
            return {x - 1, y - 1};
        }
        else
        {
            next_step = Coordinate(14, 14);
            board[next_step.x][next_step.y] = ai_side;
            Zobrist ^= boardZobrist[next_step.x][next_step.y][ai_side];
            update_score(next_step);
            possible_position.add(next_step);

            cerr << "My score: " << chess_score[0] << endl
                 << "Baseline score: " << chess_score[1] << endl;
            return {14, 14};
        }
    }

    if (turn == 2 && ai_side == 1)
    {
        // flip();
        // int value1 = AB_search(DEPTH, MIN, MAX, ai_side);
        // flip();
        // int value2 = AB_search(DEPTH, MIN, MAX, ai_side);
        // if (value1 >= value2)
        // {
        //     flip();
        //     return {-1, -1};
        // }
        if (chess_score[0] < chess_score[1])
        {
            flip();
            cerr << "My score: " << chess_score[0] << endl
                 << "Baseline score: " << chess_score[1] << endl;
            return {-1, -1};
        }
        else
            return generate_next_step();
    }

    return generate_next_step();
}
