#include "ex2.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <cstring>
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

bool exhaustiveSearch(const vector<vector<uint32_t>>& graph, int k, bool **covered, bool included[], int cover_size, uint32_t i)
{
    if (cover_size > k)
        return false;
    if (i >= graph.size())
        return false;
    
    bool has_cover = true;
    for (uint32_t x = 0; x < graph.size(); x++)
        for (uint32_t y = 0; y < graph.size(); y++)
            if (!covered[x][y]) has_cover = false;
    if (has_cover) return true;

    bool **new_covered;
    new_covered = (bool**)malloc(sizeof(bool*)*graph.size());
    bool new_included[graph.size()];
    for (uint32_t x = 0; x < graph.size(); x++)
    {
        new_covered[x] = (bool*)malloc(sizeof(bool)*graph.size());
        for (uint32_t y = 0; y < graph.size(); y++)
            new_covered[x][y] = covered[x][y];
    }
    for (uint32_t i = 0; i < graph.size(); i++)
        new_included[i] = included[i];

    new_included[i] = true;
    vector<uint32_t> vertex = graph[i];
    for (uint32_t edge : vertex)
    {
        new_covered[i][edge] = true;
        new_covered[edge][i] = true;
    }
    bool result = exhaustiveSearch(graph, k, new_covered, new_included, cover_size+1, i+1)
        || exhaustiveSearch(graph, k, covered, included, cover_size, i+1);
    for (uint32_t x = 0; x < graph.size(); x++)
        free(new_covered[x]);
    free(new_covered);
    return result;
}

bool hasVertexCover(const vector<vector<uint32_t>>& graph, int k){

    //Your code here
    if (graph.size() <= k)
        return true;

    // H is all vertices with degree above k
    vector<vector<uint32_t>> H;
    vector<vector<uint32_t>> invgraph;
    for (vector<uint32_t> vertex : graph)
    {
        if (vertex.size() > k) H.push_back(vertex);
        else invgraph.push_back(vertex);
    }

    // If there are more vertices of degree > k than k, we are out of luck (observation 3.3.2.2)
    if (H.size() > k)
        return false;


    uint32_t m = k - H.size();

    if (invgraph.size() > m*(k+1))
        return false;

    // Exhaustive search in invgraph

    // Specify that no nodes are included
    bool included[graph.size()];
    for (uint32_t i = 0; i < graph.size(); i++)
        included[i] = false;

    // Find all edges
    bool **covered;
    covered = (bool**)malloc(sizeof(bool*)*graph.size());
    for (uint32_t x = 0; x < graph.size(); x++)
    {
        covered[x] = (bool*)malloc(sizeof(bool)*graph.size());
        for (uint32_t y = 0; y < graph.size(); y++)
            covered[x][y] = true;
    }

    // Remove "cover" on those that really exist
    for (uint32_t i = 0; i < graph.size(); i++)
        for (uint32_t edge : graph[i])
        {
            covered[i][edge] = false;
            covered[edge][i] = false;
        }

        printf("\n");
        printf("\n");
    for (uint32_t x = 0; x < graph.size(); x++)
    {
        for (uint32_t y = 0; y < graph.size(); y++)
            printf("%s", covered[x][y]?"1\t":"0\t");
        printf("\n");
    }
        printf("\n");
        printf("\n");
    // Do the search
    bool result = exhaustiveSearch(invgraph, m, covered, included, 0, 0);
    for (uint32_t x = 0; x < graph.size(); x++)
    {
        free(covered[x]);
    }
    free(covered);
    return result;
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
