#include "std.h"
#include "staticexperiment.h"
#include "globals.h"
#include "util.h"
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <assert.h>

std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str)
{
    std::vector<std::string>   result;
    std::string                line;
    std::getline(str,line);

    std::stringstream          lineStream(line);
    std::string                cell;

    while(std::getline(lineStream,cell, ','))
    {
        result.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty())
    {
        // If there was a trailing comma then add an empty element.
        result.push_back("");
    }
    return result;
}

class CSVRow
{
    public:
        std::string const& operator[](std::size_t index) const
        {
            return m_data[index];
        }
        std::size_t size() const
        {
            return m_data.size();
        }
        void readNextRow(std::istream& str)
        {
            std::string         line;
            std::getline(str, line);

            std::stringstream   lineStream(line);
            std::string         cell;

            m_data.clear();
            while(std::getline(lineStream, cell, ','))
            {
                m_data.push_back(cell);
            }
            if (!lineStream && cell.empty())
            {
                m_data.push_back("");
            }
        }
    private:
        std::vector<std::string>    m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}

class CSVIterator
{
    public:
        typedef std::input_iterator_tag     iterator_category;
        typedef CSVRow                      value_type;
        typedef std::size_t                 difference_type;
        typedef CSVRow*                     pointer;
        typedef CSVRow&                     reference;

        CSVIterator(std::istream& str)  :m_str(str.good()?&str:NULL) { ++(*this); }
        CSVIterator()                   :m_str(NULL) {}
        CSVIterator& operator++()               {if (m_str) { if (!((*m_str) >> m_row)){m_str = NULL;}}return *this;}
        CSVIterator operator++(int)             {CSVIterator    tmp(*this);++(*this);return tmp;}
        CSVRow const& operator*()   const       {return m_row;}
        CSVRow const* operator->()  const       {return &m_row;}

        bool operator==(CSVIterator const& rhs) {return ((this == &rhs) || ((this->m_str == NULL) && (rhs.m_str == NULL)));}
        bool operator!=(CSVIterator const& rhs) {return !((*this) == rhs);}
    private:
        std::istream*       m_str;
        CSVRow              m_row;
};

using namespace NEAT;
using namespace std;

static struct XorInit {
    XorInit() {
        create_static_experiment("xor", [] () {
            int runs;
            std::ifstream       input("x.csv");
            std::ifstream       output("y.csv");
            CSVRow              x;
            CSVRow              y;
            const real_t weight = 1.0;
            vector<Test> tests;
            bool test;
            if (runs != 10) {
                while(input >> x && output >> y) {
                    vector<Step> steps;
                    vector <real_t> X;
                    for (int i = 0; i < 100; ++i) {
                        float xx = std::stof(x[i]);
                        append(X, xx);
                    }
                    vector <real_t> Y;
                    for (int i = 0; i < 1; ++i) {
                        float yy = std::stof(y[i]);
                        append(Y, yy);
                    }
                    steps.emplace_back(X, Y, weight);
                    tests.emplace_back(steps);
                }
            } else {
                vector<Test> tests;
                vector<Step> steps;
                vector <real_t> X;
                for (int i = 0; i < 100; ++i) {
                    float xx = rand();
                    append(X, xx);
                }
                vector <real_t> Y;
                for (int i = 0; i < 1; ++i) {
                    float yy = 0.0;
                    append(Y, yy);
                }
                steps.emplace_back(X, Y, weight);
                tests.emplace_back(steps);
            }
            return tests;
        });
    }
} init;
