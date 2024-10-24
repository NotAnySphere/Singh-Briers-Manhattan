#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <chrono>
#include <random>

struct vertex
{
    int x;
    int y;
    bool beginning; // if this vertex is the beginning of a building,
                    // this bool enables us to differentiate different buildings and saves me a lot of time 
                    // as i can reuse the standard library red-black tree implementation
    vertex(int xVal, int yVal, bool start) : x(xVal), y(yVal), beginning(start) {}
};

bool earlierVertex(vertex a, vertex b) {
    return a.x < b.x;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./manhattan {test size}\n");
        return 0;
    }
    int testSize = std::atoi(argv[1]);
    printf("%d\n", testSize);
    std::vector<vertex> vertices;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1,100);

    for (size_t i = 0; i < testSize; i++) 
    {
        int start = distrib(gen);
        int length = start + distrib(gen);
        int height = distrib(gen);

        vertices.push_back(vertex(start, height, true));
        vertices.push_back(vertex(length, height, false)); 
    }

    std::sort(vertices.begin(), vertices.end(), earlierVertex);

    std::map<int, int> heightCount;

    long long areaTotal = 0;
    int currentHeight = 0;
    int prevX = 0;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < vertices.size(); i++) // O(n) operation
    {
        vertex V = vertices[i];
        areaTotal += currentHeight * (V.x - prevX);
        prevX = V.x;
        if (V.beginning)
        {
            auto count = heightCount.find(V.y); // O(log n), red black tree search
            if (count == heightCount.end())
            {
                heightCount.insert({V.y, 1}); // O(log n), red black tree insert
            }
            else 
            {
                heightCount.erase(count); // O(log n), red black tree delete
                heightCount.insert({V.y, count->second + 1}); // O(log n), red black tree insert
            }
        }
        else
        {
            auto count = heightCount.find(V.y); // idem
            if (count != heightCount.end())
            {
                heightCount.erase(count); // idem
            }
            
            if (count->second > 1)
            {
                heightCount.insert({V.y, count->second - 1}); // idem
            }
        }
        if (heightCount.size() < 1) // O(1)
        {
            currentHeight = 0;
            continue;
        }
        
        currentHeight = heightCount.rbegin()->first; // O(log n), finding highest element in sorted tree
    }
    
    // so O(n*log n), the constant could be improved with a custom red-black tree,
    // as we could assimilate multiple standard library operations into a single one e.g.
    // to increment the value of a given key, not having to search through the whole tree 3 times as above

    auto stopTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(stopTime - startTime);

    std::cout << "Time elapsed: " << duration.count() << "\n";

    printf("area: %lld\n", areaTotal);

    return 0;
}
