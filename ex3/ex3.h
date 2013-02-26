#include "evaluate.h"
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <unordered_map>

std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
typedef std::vector<std::vector<int>> GraphType;
typedef std::vector<int> CertificateType;
auto evaluateEx3 = evaluate<std::vector<int>>;

GraphType generateGraph(int nVertices, double edgeDensity){
    GraphType graph(nVertices);
    std::bernoulli_distribution edgeDistribution(edgeDensity);

    for(int i = 0; i<nVertices; i++){
        std::cout << std::setw(3) << (i*100)/nVertices << "%";
        for(int j = i+1; j<nVertices; j++){
            if(edgeDistribution(generator)){
                graph[i].push_back(j);
                graph[j].push_back(i);
            }
        }
        std::cout << "\b\b\b\b";

    }
    return graph;
}

bool isVertexCover(const GraphType& graph, CertificateType certificate){
    std::sort(certificate.begin(), certificate.end());
    for(unsigned int vertex1 = 0; vertex1 < graph.size(); vertex1++ ){
        if(!binary_search(certificate.begin(), certificate.end(),vertex1)){
            for(int vertex2 : graph[vertex1]){
                if(!binary_search(certificate.begin(), certificate.end(),vertex2)){
                    return false;
                }
            }
        }
    }
    return true;
}

bool checkExamples(const std::vector<std::pair<GraphType, CertificateType>>& examples,
                   std::function<CertificateType(const GraphType&)> minVertexCover){
    for(auto example : examples){
        GraphType graph = example.first;
        CertificateType exampleCertificate = example.second;

        CertificateType certificate = minVertexCover(graph);

        if( !isVertexCover(graph, certificate) ){
            std::cout << "Invalid certificate for example graph:"<<std::endl;
            std::cout << "{";
            if(graph.size() > 0){
                std::cout << std::endl;
            }
            for(unsigned int from = 0; from < graph.size(); ++from){
                std::cout <<"  "<< from << ": {";
                for(int to : graph[from]){
                    std::cout << to;
                    if(to != *graph[from].rbegin()){
                        std::cout << ", ";
                    }
                }
                std::cout << "}"<<std::endl;
            }
            std::cout << "}"<<std::endl;
            std::cout <<"Your code returned: { ";
            for(int vertex : certificate){ std::cout << vertex << " "; }
            std::cout << "}"<<std::endl;
            std::cout <<"Example certificate: { ";
            for(int vertex : exampleCertificate){ std::cout << vertex << " "; }
            std::cout <<"}"<<std::endl;

            return false;
        }

    }
    return true;
}

typedef std::unordered_map<int, std::vector<int>> MapGraphType;

void removeVertexFromGraph(MapGraphType& graph, int vertex){
    for(int otherVertex : graph[vertex]){
        if( graph[otherVertex].size() == 1 ){
            graph.erase(otherVertex);
        }
        else{
            graph[otherVertex].erase(lower_bound(graph[otherVertex].begin(), graph[otherVertex].end(),vertex));
        }
    }
    graph.erase(vertex);
}

CertificateType getExampleCertificateInner(MapGraphType graph, unsigned int upperLimit){

    std::vector<int> ret;
    for(auto vertexPair : graph){ ret.push_back(vertexPair.first); };

    if(graph.size() == 0){
        return ret;
    }

    if(upperLimit <= 0){
        return ret;
    }

    std::vector<std::pair<int, int>> degrees;
    for(auto vertexPair : graph){ degrees.push_back(std::make_pair(vertexPair.first, vertexPair.second.size())); }
    std::sort(degrees.begin(), degrees.end(), [](std::pair<int,int> vertex1, std::pair<int,int> vertex2){ return vertex1.second < vertex2.second; });

    auto sumEdges = [](int acc, std::pair<int,int> vertex){return acc + vertex.second; };

    int edgeCount = std::accumulate( degrees.begin(), degrees.end(), 0, sumEdges)/2;

    if( upperLimit < degrees.size() && std::accumulate( degrees.end() - upperLimit, degrees.end(), 0, sumEdges ) < edgeCount){
        return ret;
    }

    auto graphCopy = graph;

    int vertexToUse = degrees.back().first;

    auto adjacencyList = graph[vertexToUse];

    removeVertexFromGraph(graphCopy, vertexToUse);

    auto certificate = getExampleCertificateInner(graphCopy, upperLimit - 1);

    if(certificate.size() + 1 < upperLimit){
        upperLimit = certificate.size() + 1;
        ret = certificate;
        ret.push_back( vertexToUse );
    }

    for(int otherVertex : adjacencyList){
        removeVertexFromGraph(graph, otherVertex);
    }

    certificate = getExampleCertificateInner(graph, upperLimit - adjacencyList.size());

    if(certificate.size() + adjacencyList.size() < upperLimit){
        ret = certificate;
        for(int otherVertex : adjacencyList){
            ret.push_back( otherVertex );
        }
    }
    return ret;
}

CertificateType getExampleCertificate(const GraphType& graph){
    MapGraphType newGraph;
    for(unsigned int vertex1 = 0; vertex1 < graph.size(); vertex1++){
        if(graph[vertex1].size() > 0){
            newGraph[vertex1] = graph[vertex1];
        }
    }
    return getExampleCertificateInner(newGraph, newGraph.size());
}
