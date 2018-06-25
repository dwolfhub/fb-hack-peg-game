#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

struct BestProb
{
    int col;
    double prob;
};

const vector<string> explode(const string &s, const char &c)
{
    string buff{""};
    vector<string> v;

    for (auto n : s)
    {
        if (n != c)
        {
            buff += n;
        }
        else if (n == c && buff != "")
        {
            v.push_back(buff);
            buff = "";
        }
    }
    if (buff != "")
        v.push_back(buff);

    return v;
}

bool goesleft(int x, int y)
{
    return (y % 2 == 1) || (y % 2 == 0 && x >= 1);
}

bool goesright(int x, int y, int cols)
{
    return (y % 2 == 0 && x <= cols - 2) || (y % 2 == 1 && x <= cols - 1);
}

int goleft(int x, int y)
{
    if (y % 2 == 0)
        x -= 1;
    return x;
}

int goright(int x, int y)
{
    if (y % 2 == 1)
        x += 1;
    return x;
}

bool missing_peg(int x, int y, int cols, int rows, vector<vector<bool>> empties)
{
    if (y % 2 == 1)
        x += 1;
    return empties[x][y + 1];
}

double drop(
    int x,
    int y,
    int rows,
    int cols,
    int goal,
    double prob,
    vector<vector<bool>> empties,
    vector<vector<bool>> &memoed,
    vector<vector<double>> &memo)
{
    if (y >= rows - 1)
        return (x == goal) ? prob : 0.0;

    if (memoed[x][y])
        return memo[x][y] * prob;

    double probsum = 0.0;
    if (missing_peg(x, y, cols, rows, empties))
    {
        probsum = drop(x, y + 1, rows, cols, goal, prob, empties, memoed, memo);
    }
    else
    {
        if (goesleft(x, y))
            probsum += drop(goleft(x, y), y + 1, rows, cols, goal, goesright(x, y, cols) ? prob * 0.5 : prob, empties, memoed, memo);
        if (goesright(x, y, cols))
            probsum += drop(goright(x, y), y + 1, rows, cols, goal, goesleft(x, y) ? prob * 0.5 : prob, empties, memoed, memo);
    }

    memoed[x][y] = true;
    memo[x][y] = probsum / prob;

    return probsum;
}

BestProb bestprob(int rows, int cols, int goal, vector<vector<bool>> empties)
{
    int bestcol;
    double currprob, bestprob = 0.0;
    vector<vector<double>> memo(cols, vector<double>(cols, 0.0));
    vector<vector<bool>> memoed(cols, vector<bool>(cols, false));

    for (int x = 0; x < cols - 1; x++)
    {
        currprob = drop(x, 0, rows, cols, goal, 1.0, empties, memoed, memo);

        if (currprob > bestprob)
        {
            bestprob = currprob;
            bestcol = x;
        }
    }

    return BestProb{bestcol, bestprob};
}

BestProb prob_from_line(string line)
{
    vector<string> line_parts = explode(line, ' ');

    int rows = stoi(line_parts[0]),
        cols = stoi(line_parts[1]),
        goal = stoi(line_parts[2]);

    vector<vector<bool>> empties(cols, vector<bool>(cols, false));

    if (stoi(line_parts[3]) > 0)
    {
        int x, y;
        for (int i = 4; i < line_parts.size(); i += 2)
        {
            y = stoi(line_parts[i]);
            x = stoi(line_parts[i + 1]);
            empties[x][y] = true;
        }
    }

    return bestprob(rows, cols, goal, empties);
}

int main()
{
    ifstream ifile;
    ofstream ofile;
    string line, result, prob;
    vector<string> line_parts;
    int i = 1;
    stringstream stream;

    ifile.open("input.txt");
    ofile.open("output.txt");
    if (ifile.is_open() && ofile.is_open())
    {
        getline(ifile, line);
        while (getline(ifile, line))
        {
            BestProb bestprob = prob_from_line(line);

            result = "XXX";
            if (bestprob.prob > 0.0)
            {
                stream << fixed << setprecision(6) << bestprob.prob;
                prob = stream.str();
                stream.str("");

                result = to_string(bestprob.col) + " " + prob;
            }

            ofile << "Case #" << i << ": " << result << endl;
            i++;
        }
    }

    return 0;
}
