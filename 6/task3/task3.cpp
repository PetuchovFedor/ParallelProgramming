#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <omp.h>
using namespace std;

vector<vector<double>> ReadMatrixFromFile(const string& fileName)
{
    vector<vector<double>> result(50);
    string str;
    ifstream input(fileName);
    random_device rd;
    mt19937 rnd(rd());
    for (size_t i = 0; i < result.size(); i++)
    {
        vector<double> tmp;
        for (size_t i = 0; i < result.size(); i++)
        {
            tmp.push_back(rnd());
        }
        result[i] = tmp;
    }
    /*while (getline(input, str))
    {
        stringstream ss(str);
        vector<double> tmp;
        double num;
        while (ss >> num)
        {
            tmp.push_back(num);
        }
        result.push_back(tmp);
    }*/
    return result;
}

vector<vector<double>> MatrixMultiplication(const vector<vector<double>>& first,
    const vector<vector<double>>& second)
{
    vector<vector<double>> result(second);
    #pragma omp parallel for
    for (int i = 0; i < result.size(); i++)
        std::fill(result[i].begin(), result[i].end(), 0);

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < first.size(); i++)
    {
        for (int j = 0; j < second[i].size(); j++)
        {
            //vector<double> str(second[0].size(), 0.0);
            for (int k = 0; k < first[i].size(); k++)
            {
                result[i][j] += first[i][k] * second[k][j];
            }
            //result.push_back(str);
        }
    }
    return result;
}

int main(int argc, char* argv[])
{
    auto m1 = ReadMatrixFromFile(argv[1]);
    auto m2 = ReadMatrixFromFile(argv[2]);
    auto m3 = MatrixMultiplication(m1, m2);
    for (int i = 0; i < m3.size(); i++)
    {
        for (int j = 0; j < m3[i].size(); j++)
        {
            cout << m3[i][j] << '\t';
        }
        cout << endl;
    }
}
