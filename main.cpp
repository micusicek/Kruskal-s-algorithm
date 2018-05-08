#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <cstring>
#include <cctype>

typedef std::pair <long long, std::pair<int, int> > Graph;

const int MAX = 1e4 + 5;
int id[MAX];
std::string cityNames[MAX];
int cityCount = 0;

std::string trim(const std::string &s)
{
    auto it = s.begin();
    auto rit = s.rbegin();

    while (it != s.end() && isspace(*it)) { it++; }
    while (rit.base() != it && isspace(*rit)) { rit++; }

    return std::string(it, rit.base());
}

void initialize()
{
    for(int i = 0;i < MAX;++i) { id[i] = i; }
}

std::string parseCityName(const std::string s) {
    size_t pos = s.find_first_of("[", 0);
    return (pos == std::string::npos) ? s : s.substr(0,pos);
}

void readGraph(
    std::string filename,
    Graph g[],
    int &graphNodeCount,
    int &graphEdgeCount
) {
    std::ifstream in(filename.c_str());
    if(!in) {
        std::cerr
            << "ERROR: could not open [" << filename << "] for reading,"
            << " error: " << std::strerror(errno) << std::endl;
        std::exit(1);
    }

    // initialize global element counts
    cityCount      = 0;
    graphEdgeCount = 0;

    int cityIndexOther, cityIndexThis;
    std::string line;

    while (std::getline(in, line)) {
        line = trim(line);
        // ignore empty lines
        if(line.length() == 0) { continue; }
        // ignore comments (start with a star)
        if(line.at(0) == '*') { continue; }
        // set of weights to previous cities start with a digit
        if(isdigit(line.at(0))) {
            std::istringstream ss(line);
            int weight;
            // add edges
            while(ss >> weight) {
                g[graphEdgeCount++] = std::make_pair(
                    weight,
                    std::make_pair(
                        cityIndexThis,
                        cityIndexOther-- // go to previous city
                    )
                );
            }
        } else { // otherwise it's a city name
            std::string cityName = parseCityName(line);
            cityNames[cityCount++] = cityName;
            cityIndexThis          = cityCount - 1; // current city
            cityIndexOther         = cityCount - 2; // previous city
        }
    }
}

int root(int x)
{
    while(id[x] != x) {
        id[x] = id[id[x]];
        x = id[x];
    }

    return x;
}

void unionEdge(int x, int y)
{
    id[root(x)] = id[root(y)];
}

long long kruskal(
    Graph g[], int graphEdgeCount,
    Graph t[], int &treeNodeCount, int &treeEdgeCount
) {
    long long minWeight = 0;
    for(int i = 0;i < graphEdgeCount;++i)
    {
        int x = g[i].second.first;
        int y = g[i].second.second;
        long long weight = g[i].first;
        // check for a cycle
        if(root(x) != root(y))
        {
            minWeight += weight;
            unionEdge(x, y);
            t[treeNodeCount++] = std::make_pair(weight, std::make_pair(x, y));
            treeEdgeCount++;
        }    
    }
    return minWeight;
}

int main()
{
    std::string filename = "./miles.dat";
    Graph graph[MAX], tree[MAX];
    int graphNodeCount = 0;
    int graphEdgeCount = 0;
    int treeNodeCount = 0;
    int treeEdgeCount = 0;

    initialize();
    readGraph(filename, graph, graphNodeCount, graphEdgeCount);

    // ascending sort 
    std::sort(graph, graph + graphEdgeCount);

    long long minWeight = kruskal(
        graph, graphEdgeCount,
        tree, treeNodeCount, treeEdgeCount);

    std::cout << "minimal weight: " << minWeight << std::endl;
    // hack to be able to sort them by name in reverse order
    // assume they were read from file in reverse alphabetical order
    // so index 0 will be last alphabetically
    //
    for(int i = 0; i < treeNodeCount; i++) {
        // make sure that alphabetically second name is the first
        // node in the edge (so that the last city can be first)
        if(tree[i].second.second < tree[i].second.first) {
            std::swap(tree[i].second.first, tree[i].second.second);
        }
        // first was originally weight but we don't need it anymore,
        // just put the index there so that std::sort sorts it by index
        tree[i].first = tree[i].second.first;
    }

    std::sort(tree, tree + treeNodeCount);

    for(int i = 0; i < treeNodeCount; i++) {
        std::cout
            << "tree edge [" << cityNames[tree[i].second.first]
            << "] -> [" << cityNames[tree[i].second.second] << "]\n";
    }

    return 0;
}
