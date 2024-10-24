#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <chrono>
#include <random>

struct vertex
{
    float x;
    int y;
    bool beginning; // if this vertex is the beginning of a building,
                    // enables us to differentiate different buildings and saves me a lot of time 
                    // as i can reuse the standard library red-black tree implementation
    vertex(float xVal, float yVal, bool start) : x(xVal), y(yVal), beginning(start) {}
};

bool earlierVertex(vertex a, vertex b) {
    return a.x < b.x;
}

double bench(int size, std::mt19937 gen, std::uniform_int_distribution<> distrib) {
    
    std::vector<vertex> vertices;

    for (size_t i = 0; i < size; i++)
    {
        float start = distrib(gen);
        int length = 10000 + distrib(gen);
        int height = i;

        vertices.push_back(vertex(start, height, true));
        vertices.push_back(vertex(length, height, false)); 
    }

    std::sort(vertices.begin(), vertices.end(), earlierVertex);

    std::map<int, int> heightCount;

    int areaTotal = 0;
    int currentHeight = 0;
    int prevX = 0;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < vertices.size(); i++)
    {
        vertex V = vertices[i];
        areaTotal += currentHeight * (V.x - prevX);
        prevX = V.x;
        if (V.beginning)
        {
            auto count = heightCount.find(V.y);
            if (count == heightCount.end())
            {
                heightCount.insert({V.y, 1});
            }
            else 
            {
                heightCount.erase(count);
                heightCount.insert({V.y, count->second + 1});
            }
        }
        else
        {
            auto count = heightCount.find(V.y);
            if (count != heightCount.end())
            {
                heightCount.erase(count);
            }
            
            if (count->second > 1)
            {
                heightCount.insert({V.y, count->second - 1});
            }
        }
        if (heightCount.size() < 1)
        {
            currentHeight = 0;
            continue;
        }
        
        currentHeight = heightCount.rbegin()->first;
    }
    
    auto stopTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(stopTime - startTime);

    return duration.count();
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: ./bestBench <test size> <iterations>\n";
        return 0;
    }
    
    int iters = std::atoi(argv[1]);
    int nVals = std::atoi(argv[2]);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1,100);

    double smallTime = 0;

    std::cout << "iters,time s" << "\n";

    for (size_t i = 0; i < nVals; i++)
    {
        int amnt = i * 100000;
        for (size_t j = 0; j < iters; j++)
        {
            smallTime = smallTime + bench(amnt, gen, distrib);
        }
        std::cout << amnt << "," << smallTime/iters << "\n";
        smallTime = 0;
    }
    
}

