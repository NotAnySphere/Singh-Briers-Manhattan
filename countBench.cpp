#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <chrono>
#include <random>

struct vertex
{
    float x;
    float y;
    bool beginning; // if this vertex is the beginning of a building,
                    // enables us to differentiate different buildings and saves me a lot of time 
                    // as i can reuse the standard library red-black tree implementation
    vertex(float xVal, float yVal, bool start) : x(xVal), y(yVal), beginning(start) {}
};

bool earlierVertex(vertex a, vertex b) {
    return a.x < b.x;
}

int bench(int size, std::mt19937 gen, std::uniform_real_distribution<> distrib) {
    
    std::vector<vertex> vertices;

    for (size_t i = 0; i < size; i++)
    {
        float start = distrib(gen);
        float length = start + distrib(gen);
        float height = distrib(gen);
        
        //printf("start: %f   length: %f   height: %f\n",start, length, height);

        vertices.push_back(vertex(start, height, true));
        vertices.push_back(vertex(length, height, false)); 
    }

    std::sort(vertices.begin(), vertices.end(), earlierVertex);

    std::map<float, int> heightCount;

    double areaTotal = 0;
    float currentHeight = 0;
    float prevX = 0;
    int operations = 0;

    for (size_t i = 0; i < vertices.size(); i++)
    {
        vertex V = vertices[i];
        areaTotal += currentHeight * (V.x - prevX);
        prevX = V.x;
        if (V.beginning)
        {
            auto count = heightCount.find(V.y);
            operations = operations + 1;
            if (count == heightCount.end())
            {
                heightCount.insert({V.y, 1});
                operations = operations + 1;
            }
            else 
            {
                heightCount.erase(count);
                operations = operations + 1;
                heightCount.insert({V.y, count->second + 1});
                operations = operations + 1;
            }
        }
        else
        {
            auto count = heightCount.find(V.y);
            operations = operations + 1;
            if (count != heightCount.end())
            {
                heightCount.erase(count);
                operations = operations + 1;
            }
            
            if (count->second > 1)
            {
                heightCount.insert({V.y, count->second - 1});
                operations = operations + 1;
            }
        }
        if (heightCount.size() < 1)
        {
            currentHeight = 0;
            continue;
        }
        
        currentHeight = heightCount.rbegin()->first;
        operations = operations + 1;
    }
    
    return operations;
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: ./opBench <test size> <iterations>\n";
        return 0;
    }

    int testSize = std::atoi(argv[1]);
    int iters = std::atoi(argv[2]);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(1,100);

    int largeOps = 0;
    int smallOps = 0;

    for (size_t i = 0; i < iters; i++)
    {
        largeOps = largeOps + bench(testSize * 2, gen, distrib);
        smallOps = smallOps + bench(testSize, gen, distrib);
    }
    
    double ratio = largeOps/smallOps;

    std::cout << "big Time average Operations: " << largeOps/iters << "\n";
    std::cout << "small Time average Operations: " << smallOps/iters << "\n";
    std::cout << "big/small ratio: " << ratio << "\n";
}

