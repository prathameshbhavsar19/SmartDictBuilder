#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <string>
using namespace std;

void readTestData(const string &filePath, vector<string> &testData) {
    ifstream file(filePath.c_str());
    if (!file) {
        cerr << "Enter Correct Path" << endl;
        exit(1);
    }
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            testData.push_back(line);
        }
    }
    file.close();
}

bool areCompatible(const string &word1, const string &word2) {
    for (size_t i = 0; i < word1.length(); i++) { // Changed int to size_t
        if (word1[i] != 'X' && word2[i] != 'X' && word1[i] != word2[i]) {
            return false;
        }
    }
    return true;
}

void buildGraph(const vector<string> &testData, map<int, vector<int> > &adjList) {
    size_t n = testData.size();
    for (int i = 1; i <= n; ++i) { // Changed int to size_t
        for (int j = i + 1; j <= n; ++j) { // Changed int to size_t
            if (areCompatible(testData[i - 1], testData[j - 1])) {
                adjList[i].push_back(j);
                adjList[j].push_back(i);
            }
        }
    }
}

int findMaxDegVertex(const map<int, vector<int> > &adjList, const set<int> &visited) {
    int maxDegree = -1;
    int startVertex = -1;

    for (map<int, vector<int> >::const_iterator it = adjList.begin(); it != adjList.end(); ++it) {
        int vertex = it->first;
        const vector<int> &edges = it->second;

        if (visited.find(vertex) == visited.end() && static_cast<int>(edges.size()) > maxDegree) {
            maxDegree = edges.size();
            startVertex = vertex;
        }
    }
    return startVertex;
}

set<int> formClique(const map<int, vector<int> > &adjList, int &startVertex, const set<int> &visited) {
    set<int> clique;

    if (startVertex == -1) {
        return clique;
    }

    clique.insert(startVertex);
    const vector<int> &neighbors = adjList.find(startVertex)->second;
    for (size_t i = 0; i < neighbors.size(); ++i) {
        int neighbor = neighbors[i];
        if (visited.find(neighbor) == visited.end()) {
            bool isCompatible = true;

            for (set<int>::const_iterator it = clique.begin(); it != clique.end(); ++it) {
                const vector<int> &cliqueNeighbors = adjList.find(*it)->second;
                if (find(cliqueNeighbors.begin(), cliqueNeighbors.end(), neighbor) == cliqueNeighbors.end()) {
                    isCompatible = false;
                    break;
                }
            }
            if (isCompatible) {
                clique.insert(neighbor);
            }
        }
    }
    return clique;
}

void findCliques(const map<int, vector<int> > &adjList, int maxCliques, vector<set<int> > &cliques) {
    set<int> visited;

    while (visited.size() < adjList.size() && cliques.size() < static_cast<size_t>(maxCliques)) {
        int startVertex = findMaxDegVertex(adjList, visited);
        set<int> clique = formClique(adjList, startVertex, visited);
        visited.insert(clique.begin(), clique.end());
        if (!clique.empty()) {
            cliques.push_back(clique);
        }
    }
}

string mergePatterns(const set<int> &clique, const vector<string> &patterns) {
    string merged = patterns[*clique.begin() - 1];
    for (set<int>::const_iterator it = clique.begin(); it != clique.end(); ++it) {
        int idx = *it - 1;
        for (size_t i = 0; i < merged.length(); ++i) { // Changed int to size_t
            if (patterns[idx][i] != 'X' && merged[i] != 'X' && patterns[idx][i] != merged[i]) {
                merged[i] = 'X';
            }
        }
    }
    return merged;
}

void writeDictionary(const string &filename, const vector<set<int> > &cliques, const vector<string> &patterns) {
    ofstream file(filename.c_str());
    if (!file) {
        cerr << "Error writing to file" << endl;
        exit(1);
    }
    for (size_t i = 0; i < cliques.size(); ++i) {
        string merged = mergePatterns(cliques[i], patterns);
        file << merged << "\n";
    }
    file.close();
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "Incorrect number of arguments" << endl;
        return 1;
    }

    string testFile = argv[1];
    int dictSize = atoi(argv[2]);
    int wordLength = atoi(argv[3]);
    string dictFile = argv[4];

    vector<string> testPatterns;
    ifstream file(testFile.c_str());
    if (!file) {
        cerr << "Wrong file name" << endl;
        return 1;
    }
    string line;
    while (getline(file, line)) {
        if (!line.empty())
            testPatterns.push_back(line);
    }
    file.close();

    if (testPatterns.empty()) {
        cerr << "Input file is empty or invalid" << endl;
        return 1;
    }

    map<int, vector<int> > adjList;
    buildGraph(testPatterns, adjList);

    vector<set<int> > cliques;
    findCliques(adjList, dictSize, cliques);

    if (cliques.size() < static_cast<size_t>(dictSize)) {
        cout << "Only " << cliques.size() << " dictionary entries are possible" << endl;
    }

    writeDictionary(dictFile, cliques, testPatterns);
    cout << "Dictionary written to " << dictFile << endl;

    testPatterns.clear();
    adjList.clear();
    cliques.clear();

    return 0;
}
