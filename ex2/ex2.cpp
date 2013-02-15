#include "ex2.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <cstring>
#include <algorithm>

using namespace std;

/*
 * Implement algo. 3.3.2.4 from Hromkovic
 * Improve with 3.3.2.6
 */

//Example graphs, both with min vertex cover = 2
const vector<vector<uint32_t>> graph1 = {
                                    {1, 2, 3},
                                    {0, 2},
                                    {0, 1},
                                    {0},
                                   };

const vector<vector<uint32_t>> graph2 = {
                                    {1},
                                    {0},
                                    {3},
                                    {2},
                                   };

const vector<vector<uint32_t>> graph3 = {};

void removeNode(vector<vector<uint32_t>>& graph, int node){
            graph[node].clear();
            for (vector<uint32_t> n : graph)
                remove(n.begin(), n.end(), node);
}

uint32_t removeGradeGT(vector<vector<uint32_t>>& graph, uint32_t k){
    uint32_t n = 0;
    for (uint32_t i = 0; i < graph.size(); i++)
        if (graph[i].size() > k)
        {
            removeNode(graph, i);
            n++;
        }
    return n;
}

void cover(vector<vector<bool>> &covered, uint32_t node)
{
   for (uint32_t x = 0; x < covered.size(); x++)
   {
           covered[x][node] = true;
           covered[node][x] = true;
   }
}


bool exhaustingSearch(
        const vector<vector<uint32_t>>& graph,
        uint32_t k,
        vector<vector<bool>> covered,
        vector<uint32_t> set,
        uint32_t current_node
        )
{
    // Is set too large, or are we out of new nodes to try?
    if (set.size() > k || current_node >= graph.size()) return false;


    bool allCovered = true;
    for (uint32_t x = 0; x < graph.size(); x++)
        for (uint32_t y = 0; y < graph.size(); y++)
            allCovered &= covered[x][y];
    if (allCovered) return true;

    vector<vector<bool>> newCovered(covered);
    vector<uint32_t> newSet(set);

    if (exhaustingSearch(graph, k, covered, set, current_node+1)) return true;

    cover(newCovered, current_node);
    newSet.push_back(current_node);

    return exhaustingSearch(graph, k, newCovered, newSet, current_node+1);
}

bool exhaustingSearch(const vector<vector<uint32_t>>& graph, uint32_t k){

    vector<vector<bool>> covered(graph.size(), vector<bool> (graph.size(), true));
    vector<uint32_t> set;
    for (uint32_t x = 0; x < graph.size(); x++)
        for(uint32_t edge : graph[x])
        {
            covered[x][edge] = false;
            covered[edge][x] = false;
        }
    return
        exhaustingSearch(graph, k, covered, set, 0);
}

bool hasVertexCover(const vector<vector<uint32_t>>& graph, uint32_t k){

    //Your code here
    if (k >= graph.size()) return true;

    // Remove all vertices from G with degree greater than k and put in G'
    // A graph H of size |H| is the number of vertices with degree > k
    vector<vector<uint32_t>> Gm(graph);
    uint32_t size_H = removeGradeGT(Gm, k);

    // If |H| > k return false
    if (size_H > k) return false;
    // else |H| =< k
    uint32_t m = k - size_H;

    // If G' has more than m(k+1) vertices, return false
    if (Gm.size() > m*(k+1)) return false;

    if (graph.size() == Gm.size())
        return exhaustingSearch(Gm, m);
    else
        return hasVertexCover(Gm, m);
}



int main(){

    Evaluation eval;

    //Evaluate your code on example graph 1
    for(int k = 0; k <=5; k++){
        bool correct = k >= 2;
        eval = evaluateEx2( bind(hasVertexCover, graph1, k), [correct](bool result){return result == correct ? 1.0 : 0.0;} );
        cout << "graph1 (k="<<k<<"): Your code got the "<<(eval.accuracy == 1.0 ? "right":"wrong")<<" answer in "<<eval.seconds<<" seconds."<<endl;
    }

    //Evaluate your code on example graph 2
    for(int k = 0; k <=5; k++){
        bool correct = k >= 2;
        eval = evaluateEx2( bind(hasVertexCover, graph2, k), [correct](bool result){return result == correct ? 1.0 : 0.0;} );
        cout << "graph2 (k="<<k<<"): Your code got the "<<(eval.accuracy == 1.0 ? "right":"wrong")<<" answer in "<<eval.seconds<<" seconds."<<endl;
    }

    //Evaluate your code on example graph 3
    for(int k = 0; k <=1; k++){
        eval = evaluateEx2( bind(hasVertexCover, graph3, k), [](bool result){return result == true ? 1.0 : 0.0;} );
        cout << "graph3 (k="<<k<<"): Your code got the "<<(eval.accuracy == 1.0 ? "right":"wrong")<<" answer in "<<eval.seconds<<" seconds."<<endl;
    }

    //Set parameters for generated graph
    const int nVertices = 10;
    const int testK = 2;
    const double edgeDensity = .5;
    const int maxVertexCoverSize = 4;

    //generate graph
    auto generatedGraph = generateGraph(nVertices, edgeDensity, maxVertexCoverSize);

    //test speed of your code, no meaningful measurement of accuracy
    eval = evaluateEx2( bind(hasVertexCover, generatedGraph, testK), [](bool result){ return 0.0; } );
    cout << "generated graph (k="<<testK<<"): Your code got an answer in "<<eval.seconds<<" seconds."<<endl;

    return 0;
}
