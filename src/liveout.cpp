#include "liveout.h"


std::unordered_set<std::string> LiveOut::singleLiveOut(CFGNode* node){
    std::unordered_set<std::string> ret = node->uevar;
    std::unordered_set<std::string> nvarkill = varDomain;
    // compute nvarkill
    for(auto v : node->varkill){
        nvarkill.erase(v);
    }
    // n.uevar | (n.liveout & n.nvarkill)
    for(auto v : nvarkill){
        if(node->liveout.count(v)){
            // intersection
            ret.insert(v);
        }
    }
    return ret;
}

bool LiveOut::updateLiveOut(CFGNode* node){
    auto old = node->liveout;
    for(auto* cn : node->children){
        for(auto v : singleLiveOut(cn)){
            node->liveout.insert(v);
        }
    }

    // std::cout << node->id << ": ";
    // for(auto v : node->liveout){
    //     std::cout << v << ", ";
    // }
    // std::cout << std::endl;

    return old != node->liveout;

}

void LiveOut::computeLiveOut(){
    bool changed = true;
    while(changed){
        changed = false;
        for(int i = cfgBlocks.size()-1; i >= 0; --i){
            changed = changed | updateLiveOut(cfgBlocks[i]);
        }
    }
}

std::unordered_set<std::string> getUEVar(AstNode* node){
    std::unordered_set<std::string> uevar;
    if(node->children.size()){
        for(auto* n : node->children){
            for(auto v : getUEVar(n)){
                uevar.insert(v);
            }
        }
    }
    if(node->type == NodeType::VAR){
        uevar.insert(node->value);
    }
    return uevar;
}

void LiveOut::prepCFG(){
    for(int i = 0; i < cfgBlocks.size(); ++i){
        auto* cfgNode = cfgBlocks[i];
        auto* astNode = cfgNode->astNode;
        if(!astNode) {continue;}
        // compute uevar and livevar
        if(astNode->type == NodeType::VARDECL){
            
            // resolve expr for uevar:
            for(auto u : getUEVar(astNode->children[1])){
                cfgNode->uevar.insert(u);
                varDomain.insert(u);
            }
            
            if(!cfgNode->uevar.count(astNode->children[0]->value)){
                // lhs is part of varkill, only add if not using in rhs
                cfgNode->varkill.insert(astNode->children[0]->value);
                varDomain.insert(astNode->children[0]->value);
            }

        }
        else if(astNode->type == NodeType::IF || astNode->type == NodeType::WHILE){
            // for if/while only eval expr for uevar
             for(auto u : getUEVar(astNode->children[0])){
                cfgNode->uevar.insert(u);
                varDomain.insert(u);
            }
        }
    }
    std::cout << std::endl;
}