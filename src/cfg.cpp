#include "cfg.h"
#include <vector>


void CFGNode::insertChild(CFGNode* node){
    if(std::find(children.begin(), children.end(), node) == children.end()){
        children.push_back(node);
    }
}

void CFGNode::insertParent(CFGNode* node){
    if(std::find(parents.begin(), parents.end(), node) == parents.end()){
        parents.push_back(node);
    }
}

void CFGCreator::registerCFGBlock(CFGNode* node){
    cfgBlocks[rid] = node;
    rid += 1;
}
void CFGCreator::resolveChildren(){
    for(auto elem : cfgBlocks){
        for(auto parent : elem.second->parents){
            parent->insertChild(elem.second);
        }
    }
}
std::vector<CFGNode*> CFGCreator::traverse(AstNode* node, std::vector<CFGNode*> parents){
    if(node->type == NodeType::ROOT || node->type == NodeType::STAT_LIST){
        // start chaining statements
        std::vector<CFGNode*> node_parents = parents;
        for(auto* child : node->children){
            node_parents = traverse(child, node_parents);
        }
        return node_parents;
    }
    else if(node->type == NodeType::VARDECL){
        CFGNode* new_node = new CFGNode(rid, parents, node);
        registerCFGBlock(new_node);
        return {new_node};
        // std::vector<CFGNode*> node_parents = {};
    }
    else if(node->type == NodeType::IF){
        CFGNode* new_node = new CFGNode(rid, parents, node);
        registerCFGBlock(new_node);
        auto if_par =  traverse(node->children[1], {new_node});
        if(node->children.size() == 3){
            // has else statement
            for(auto* par : traverse(node->children[2], {new_node})){
                if_par.push_back(par);
            }
        }
        else{
            // no else block
            if_par.push_back(new_node);
        }
        return if_par;
    }
    else if(node->type == NodeType::WHILE){
        CFGNode* new_node = new CFGNode(rid, parents, node);
        registerCFGBlock(new_node);
        auto backedge =  traverse(node->children[1], {new_node});
        new_node->insertParent(backedge.at(0));
        return {new_node};
    }
    return {};
}
CFGNode* CFGCreator::genCFG(AstNode* root){
    CFGNode* cfg_root = new CFGNode(rid, {}, nullptr);
    registerCFGBlock(cfg_root);
    auto end = traverse(root, {cfg_root});
    CFGNode* cfg_end = new CFGNode(rid, end, nullptr);
    registerCFGBlock(cfg_end);
    resolveChildren();
    return cfg_root;
}



void CFGCreator::outputCFG(std::string filename){
    std::ofstream outfile;
    outfile.open(filename, std::ios::trunc);
    outfile << "stateDiagram-v2" << std::endl;
    for(auto elem : cfgBlocks){
        std::string blockData = "(" + std::to_string(elem.second->id) + ") ";
        if(!elem.second->astNode) {
            blockData.append(elem.second->children.size() ? "START" : "END");
        }else{
            blockData.append(elem.second->astNode->toString());
        }
        outfile << elem.second->id  << ": " << blockData << std::endl;
        for(auto* child : elem.second->children){
            outfile <<  elem.second->id << " --> " << child->id << std::endl;
        }
    }  
    outfile.close();
}

