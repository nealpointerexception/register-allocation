#include "graph_coloring.h"

void GraphColoring::createGraph(std::unordered_map<int, CFGNode*>& cfgBlocks){
    for(auto elem : cfgBlocks){
        auto* cfgNode = elem.second;
        if(cfgNode->liveout.size()){
            for(auto a : cfgNode->liveout){
                if(!graph.count(a)){
                    graph[a] = {};
                }
                for(auto z : cfgNode->liveout){
                    if(a != z){
                        graph[a].insert(z);
                    }
                }
            }
        }
    }

    std::cout << "Graph:" << std::endl;
    for(auto elem : graph){
        std::cout << elem.first << ": ";
        for(auto e : elem.second){
            std::cout << e << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int randNum(int from, int to){
    return rand()%(to-from + 1) + from;
}

void GraphColoring::insertNode(graphPair g){
    // generate available registers
    std::unordered_set<int> registerSet;
    for(int i = 0; i < totalRegisters; ++i){
        registerSet.insert(i);
    }
    // add edge back into graph and check if available color
    graph.insert(g);
    for(auto e : g.second){
        if(graph.count(e)){
            // edge exists, remove from available reg set
            registerSet.erase(regMap.at(e));
        }
    }
    if(registerSet.size()){
        // assign available register
        regMap[g.first] = *registerSet.begin();
    }
    else{
        // spill node
        regMap[g.first] = -1;
        graph.erase(g.first);
    }
}

graphPair GraphColoring::removeOneNode(){
    // find the first elem that has < register num connects and remove
    graphPair toRemove;
    bool found = false;
    for(auto elem : graph){
        if(elem.second.size() < totalRegisters){
            // found our guy
            toRemove =  elem;
            found = true;
            break;
        }
    }
    if(!found){
        // remove node at random, register as problematic
        toRemove = *std::next(std::begin(graph), randNum(0, graph.size()-1));
        // problematic.insert(toRemove.first);
    }

    // remove from links
    for(auto v : toRemove.second){
        graph[v].erase(toRemove.first);
    }
    graph.erase(toRemove.first);
    // std::cout << "remove: " << toRemove.first << std::endl;
    return toRemove;
}

void GraphColoring::colorGraph(){
    // first remove nodes until none left
    std::stack<graphPair> removals;
    while(graph.size()){
        removals.push(removeOneNode());
    }

    // add nodes back in from stack and color
    while(!removals.empty()){
        graphPair toAdd = removals.top();
        removals.pop();
        // std::cout << "add: " << toAdd.first << std::endl;
        insertNode(toAdd);
    }

    std::cout << "Graph Coloring Results:" << std::endl;
    for(auto elem : regMap){
        std::cout << elem.first << ": r" << elem.second << std::endl;
    }

}