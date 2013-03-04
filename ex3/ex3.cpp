#include "ex3.h"
#include <vector>
#include <functional>
#include <cassert>
using namespace std;

//Example graphs
const vector<pair<vector<vector<int>>, vector<int>>> examples = {
    //graph 0:
    {
        //graph
        {},
        //example min-vertex-cover
        {}
    },
    //graph 1:
    {
        //graph
        {{},{}},
        //example min-vertex-cover
        {}
    },
    //graph 2
    {
        //graph
        {{1, 2, 3}, {0, 2}, {0, 1}, {0}},
        //example min-vertex-cover
        {0, 1}
    },
    //graph 3
    {
        //graph
        {{1}, {0}, {3}, {2}},
        //example min-vertex-cover
        {0, 2}
    },
    //graph 4
    {
        //graph
        {{1,2}, {0}, {0} },
        //example min-vertex-cover
        {0}
    },
    //graph 5
    {
        //graph
        {{4}, {3, 4}, {4}, {1}, {0, 1, 2} },
        //example min-vertex-cover
        {4, 1}
    },
    //graph 6
    {
        //graph
        {{3}, {}, {3, 4}, {0, 2, 4}, {2, 3} },
        //example min-vertex-cover
        {2, 3}
    }
    //graph self-made
    ,{
        //graph
        {{3}, {5}, {3, 4}, {0, 2, 4}, {2, 3}, {1}, {7}, {6}},
        //example min-vertex-cover
        {2, 3, 5, 7}
    }
};

vector<vector<int>> removeVertex(const vector<vector<int>>& graph, int node)
{
    vector<vector<int>> newGraph(graph);

    for (int i : newGraph[node])
        for (unsigned int j = 0; j < newGraph[i].size(); j++)
        {
            if (newGraph[i][j] == node) newGraph[i].erase(newGraph[i].begin() + j);
            else if (newGraph[i][j] > node) newGraph[i][j] -= 1;
        }
    newGraph[node].clear();//erase(newGraph.begin()+node);
    return newGraph;

}

/*
 * Finding an approximation to a minimal vertex cover using simulated annealing
 */
vector<int> minVertexCoverSA(const vector<vector<int>>& graph, unsigned int temp){
   for (int i = 0; i < graph.size(); i++)
   {
        
   }
}


/*
 * Finding the minimal vertex cover of a graph using branch-and-bound technices
 */
unsigned int globalBest = 999999999;
vector<int> minVertexCoverBB(const vector<vector<int>>& graph, unsigned int level){
    //your code here
    vector<int> best;

    for (unsigned int i = 0; i < graph.size(); i++)
            if (!graph[i].empty()) best.push_back(i);
    if (globalBest < (best.size())) return best;
    for (unsigned int i = 0; i < graph.size(); i++)
    {
       if (graph[i].empty()) continue;
       vector<vector<int>> newGraph = removeVertex(graph, i);
       vector<int> newBest = minVertexCoverBB(newGraph, level+1);
       if (newBest.size() < (best.size()-1))
       {
            best = newBest;
            for (unsigned int j = 0; j < best.size(); j++)
                if (best[j] >= static_cast<int>(i) ) best[j];// -= 1;
            best.push_back(i);
       }
    }

    return best;
}

vector<int> minVertexCover(const vector<vector<int>>& graph){
    globalBest = minVertexCoverSA(graph,1000).size();
    return minVertexCoverBB(graph,0);
}

int main(){

    //Test correctness on examples
    if(checkExamples(examples, minVertexCover)){
        cout << "Correct answer for all examples"<<endl;
    }
    else{
        cout << "Not correct answer for all examples, aborting..."<<endl;
        return 0;
    }


    //Number of vertices in the graph
    const int nVertices = 9;
    //Probability of edge between two vertices
    const double edgeDensity = .3;

    //generate graph
    cout << "Generating graph...";
    const auto generatedGraph = generateGraph(nVertices, edgeDensity);
    cout << "done"<<endl;
    //cout << "Generating correct answer...";
    auto exampleCertificate = getExampleCertificate(generatedGraph);
    cout << "done"<<endl;

    auto evaluateCertificate = [&generatedGraph, &exampleCertificate](vector<int> certificate){
        if(isVertexCover(generatedGraph, certificate)){
            if(certificate.size() == 0){
                return 1.0;
            }
            return (double)exampleCertificate.size()/(double)certificate.size();
        }
        return 0.0;
    };

    //test speed and accuracy of your code
    Evaluation eval = evaluateEx3( bind(minVertexCover, generatedGraph), evaluateCertificate);
    cout << "Your code got accuracy " << eval.accuracy << " in "<<eval.seconds<<" seconds."<<endl;

    //Evaluation eval2 = evaluateEx3( bind(getExampleCertificate, generatedGraph), evaluateCertificate );
    //cout << "Example code got accuracy " << eval2.accuracy << " in "<<eval2.seconds<<" seconds."<<endl;

    return 0;
}
