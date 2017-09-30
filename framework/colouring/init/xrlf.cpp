#include "colouring/init/xrlf.h"
#include <unordered_set>
#include <unordered_map>
#include <stdlib.h>
#include <boost/heap/fibonacci_heap.hpp>
#include <debug.h>

using namespace graph_colouring;

Colouring graph_colouring::initByXRLFIgnored(const graph_access &G, const ColorCount _) {
    xrlf::XRLFParameters params;
    params.COLORCOUNT = _;
    return xrlf::initByXRLF(G, params);
}

Colouring graph_colouring::initByXRLFRandom(const graph_access &G, const ColorCount k) {
    xrlf::XRLFParameters params;
    params.COLORCOUNT = k;
    params.MODE = xrlf::XRLFMode::RANDOM_COLOR_REMAINNIG;
    return xrlf::initByXRLF(G, params);
}

Colouring graph_colouring::initByXRLFUncolored(const graph_access &G, const ColorCount k) {
    xrlf::XRLFParameters params;
    params.COLORCOUNT = k;
    params.MODE = xrlf::XRLFMode::UNCOLOR_REMAINING;
    return xrlf::initByXRLF(G, params);
}

namespace xrlf {
    std::random_device randomNumberDevice;
    std::mt19937 rngGenerator(xrlf::randomNumberDevice());

    
    Colouring initByXRLF(const graph_access &G, XRLFParameters parameters) {
        Colouring s(G.number_of_nodes(), std::numeric_limits<NodeID>::max());
        Color k = 0;

        NodeID nodeCount = G.number_of_nodes();
        xrlf::Subgraph subgraph(G);
        while (subgraph.getNumberOfNodes() > parameters.EXACTLIM) {
            std::unordered_set<NodeID> independentSet = xrlf::calculateIndependentSet(subgraph, parameters);
            assert(xrlf::isIndependentSet(independentSet, G));
            subgraph.removeNodes(independentSet);
            // Color Independent set
            for (auto n_ = independentSet.begin(); n_ != independentSet.end(); ++n_) {

                NodeID n = *n_;
                s[n] = k;
            }
            k++;
            if (k == parameters.COLORCOUNT && (parameters.MODE == XRLFMode::UNCOLOR_REMAINING || parameters.MODE == XRLFMode::RANDOM_COLOR_REMAINNIG)) {
                if (parameters.MODE == XRLFMode::UNCOLOR_REMAINING) {
                    return s;
                } else if (parameters.MODE == XRLFMode::RANDOM_COLOR_REMAINNIG) {
                    std::unordered_set<NodeID> nodes = subgraph.getNodes();
                    std::uniform_int_distribution<Color>  distr(0, k - 1);

                    for (auto n_ = nodes.begin(); n_ != nodes.end(); n_++) {
                        NodeID n = *n_;
                        Color r = distr(xrlf::rngGenerator);
                        s[n] = r;
                    }
                    return s;
                }
            }
        }

        // B&B Color remaining nodes
        if (subgraph.getNumberOfNodes() > 0) {
            std::unordered_set<NodeID> nodes = subgraph.getNodes();
            findOptimalColouring(subgraph, s, k);
        }
        return s;

    }

    bool isIndependentSet(std::unordered_set<NodeID> set, const graph_access &G) {
        bool ok = true;
        for (auto r = set.begin(); r != set.end(); ++r) {
            NodeID n = *r;
            auto neighbors = G.neighbours(n);
            for (auto neighbor_ = neighbors.begin(); neighbor_ != neighbors.end(); ++neighbor_) {
                NodeID neighbor = *neighbor_;
                if (set.find(neighbor) != set.end()) {
                    std::cout << n << " and " << neighbor << " in conflict" << std::endl;
                    ok = false;
                }
            }
        }
        return ok;
    }

    std::unordered_set<NodeID> calculateGraphComponent(std::unordered_set<NodeID>& nodes, std::unordered_map<NodeID, std::unordered_set<NodeID>>& ineligibleNodes) {
        std::unordered_set<NodeID> componentNodes;
        std::unordered_set<NodeID> queue;
        queue.reserve(nodes.size());
        queue.insert(*nodes.begin());
        while(queue.size() > 0) {
            NodeID current = *queue.begin();
            componentNodes.insert(current);
            std::unordered_set<NodeID>& neighbors = ineligibleNodes[current];
            for (auto n_ = neighbors.begin(); n_ != neighbors.end(); ++n_) {
                NodeID neighbor = *n_;
                if (componentNodes.find(neighbor) == componentNodes.end()) {
                    queue.insert(neighbor);
                }
            }
            queue.erase(current);
        }
  
        return componentNodes;
    }

    void recursiveExhaustiveSearch(std::unordered_set<NodeID>& nodes, NodeID degreeSum, std::unordered_map<NodeID, std::unordered_set<NodeID>>& ineligibleNodes, std::unordered_map<NodeID, NodeID>& nodeDegrees, SubsetPair & best, SubsetPair & current, NodeID level);

    SubsetPair recursiveExhaustiveSearchStart(std::unordered_set<NodeID>& nodes, NodeID degreeSum, std::unordered_map<NodeID, std::unordered_set<NodeID>>& ineligibleNodes, std::unordered_map<NodeID, NodeID>& nodeDegrees, NodeID level) {
        NodeID bestDegree = 0;
        std::unordered_set<NodeID> best;
        SubsetPair bestSet = std::make_pair(best, bestDegree);
        NodeID degree = 0;
        std::unordered_set<NodeID> current;
        SubsetPair currentSet = std::make_pair(current, degree);

        recursiveExhaustiveSearch(nodes, degreeSum, ineligibleNodes, nodeDegrees, bestSet, currentSet, 0);
        return bestSet;
    }

    void removeNode(NodeID& node, std::unordered_set<NodeID>& nodes, NodeID& degreeSum, std::unordered_map<NodeID, NodeID>& nodeDegrees, NodeID level) {
        degreeSum -= nodeDegrees[node];
        size_t removed = nodes.erase(node);
        assert(removed == 1);
    }

    void addNode(NodeID& node, std::unordered_set<NodeID>& nodes, NodeID& degreeSum, std::unordered_map<NodeID, NodeID>& nodeDegrees, NodeID level) {
        degreeSum += nodeDegrees[node];
        auto inserted = nodes.insert(node);
        assert(inserted.second);
    }

    void addToSubsetpair(NodeID& node, std::unordered_set<NodeID>& nodes, NodeID& degreeSum, std::unordered_map<NodeID, NodeID>& nodeDegrees, SubsetPair& pair, NodeID level) {
        removeNode(node, nodes, degreeSum, nodeDegrees, level);
        pair.first.insert(node);
        pair.second += nodeDegrees[node];         
    }

    void removeFromSubsetPair(NodeID& node, std::unordered_set<NodeID>& nodes, NodeID& degreeSum, std::unordered_map<NodeID, NodeID>& nodeDegrees, SubsetPair& pair, NodeID level) {
        addNode(node, nodes, degreeSum, nodeDegrees, level);
        pair.first.erase(node);
        pair.second -= nodeDegrees[node];                 
    }

    void addTupleNodes(NodeID& neighbor1, NodeID& neighbor2, std::unordered_set<NodeID>& nodes, NodeID& degreeSum, std::unordered_map<NodeID, NodeID>& nodeDegrees, SubsetPair& pair, NodeID level) {
        bool larger = nodeDegrees[neighbor2] > nodeDegrees[neighbor1];
        NodeID add = larger ? neighbor2 : neighbor1;
        NodeID remove = larger ? neighbor1 : neighbor2;
        addToSubsetpair(add, nodes, degreeSum, nodeDegrees, pair, level);
        removeNode(remove, nodes, degreeSum, nodeDegrees, level);
    }

    void removeTupleNodes(NodeID& n1, NodeID& n2, std::unordered_set<NodeID>& nodes, NodeID& degreeSum, std::unordered_map<NodeID, NodeID>& nodeDegrees, SubsetPair& pair, NodeID level) {
        bool larger = pair.first.find(n2) != pair.first.end();
        NodeID added = larger ? n2 : n1;
        NodeID removed = larger ? n1 : n2;
        removeFromSubsetPair(added, nodes, degreeSum, nodeDegrees, pair, level);
        addNode(removed, nodes, degreeSum, nodeDegrees, level);        
    }

    void recursiveExhaustiveSearch(std::unordered_set<NodeID>& nodes, NodeID degreeSum, std::unordered_map<NodeID, std::unordered_set<NodeID>>& ineligibleNodes, std::unordered_map<NodeID, NodeID>& nodeDegrees, SubsetPair & best, SubsetPair & current, NodeID level) {
        assert(degreeSum < std::numeric_limits<NodeID>::max() - 1000); // TODO: remove, checks for overflows
        if (nodes.size() <= 1) {
            if (nodes.size() == 1) {
                NodeID node = *nodes.begin();
                addToSubsetpair(node, nodes, degreeSum, nodeDegrees, current, level);
            }
    
            if (!current.first.empty() && (current.second > best.second || best.first.empty() || (current.second == best.second && current.first.size() > best.first.size()))) {
                best.first = current.first;
                assert(!(current.first.find(91) != current.first.end() && current.first.find(125) != current.first.end()));
                best.second = current.second;
            }
        } else {
            NodeID node = *nodes.begin();
            size_t removed = nodes.erase(node);
            assert(removed == 1);
            
            degreeSum -= nodeDegrees[node];

            // Case A: node is not in the independent set
            std::unordered_set<NodeID>& neighbors = ineligibleNodes[node];
            NodeID beforeDegreeSum = degreeSum;
            NodeID beforeSize = nodes.size();
            std::vector<NodeID> isolatedNodes;
            isolatedNodes.reserve(nodes.size());
            std::vector<NodeID> tupleNodes;
            tupleNodes.reserve(nodes.size());

            NodeID countCupleNodes = 0;
            NodeID removedIsolatedNodes = 0;
            for (auto r_  = neighbors.begin(); r_ != neighbors.end(); ++r_) {
                NodeID neighbor = *r_;
                std::unordered_set<NodeID>& neighborSet = ineligibleNodes[neighbor];
                auto removedCount = neighborSet.erase(node); 
                assert(neighborSet.size() == ineligibleNodes[neighbor].size());
                assert(removedCount == 1);
                if (neighborSet.size() == 0) { // r is now isolated => we can add it
                    isolatedNodes.push_back(neighbor);
                    removedIsolatedNodes++;
                    addToSubsetpair(neighbor, nodes, degreeSum, nodeDegrees, current, level);
                } else if (neighborSet.size() == 1) {
                    NodeID neighbor2 = *neighborSet.begin();
                    std::unordered_set<NodeID>& neighborSet2 = ineligibleNodes[neighbor2];
                    if (neighborSet2.size() == 1) {
                        assert(neighbor == *neighborSet2.begin());
                        tupleNodes.push_back(neighbor);
                        tupleNodes.push_back(neighbor2);
                        countCupleNodes += 2;
                        addTupleNodes(neighbor, neighbor2, nodes, degreeSum, nodeDegrees, current, level);
                    }
                }
            }
            assert(isolatedNodes.size() == removedIsolatedNodes);
            assert(tupleNodes.size() == countCupleNodes);
            if (current.second + degreeSum > best.second || best.first.empty() || (current.second + degreeSum == best.second && current.first.size() + nodes.size() > best.first.size())) {
                recursiveExhaustiveSearch(nodes, degreeSum, ineligibleNodes, nodeDegrees, best, current, level+1);
            }
            for (auto n_ = isolatedNodes.begin(); n_ != isolatedNodes.end(); n_++) {
                removeFromSubsetPair(*n_, nodes, degreeSum, nodeDegrees, current, level);
            }
            for (auto n_ = tupleNodes.begin(); n_ != tupleNodes.end(); n_++) {
                NodeID n1 = *n_;
                n_++;
                NodeID n2 = *n_;
                removeTupleNodes(n1, n2, nodes, degreeSum, nodeDegrees, current, level);
            }
            for (auto r_  = neighbors.begin(); r_ != neighbors.end(); ++r_) {
                NodeID r = *r_;
                std::unordered_set<NodeID>& neighborSet = ineligibleNodes[r];
                neighborSet.insert(node);
            }
            assert(degreeSum == beforeDegreeSum);
            assert(nodes.size() == beforeSize);

            // Case B: node is in the independent set     
            current.first.insert(node);
            current.second += nodeDegrees[node];
            std::unordered_set<NodeID>& nodesToBeRemoved = ineligibleNodes[node];
            beforeDegreeSum = degreeSum;
            beforeSize = nodes.size();

            isolatedNodes.clear();
            isolatedNodes.reserve(nodes.size());
            tupleNodes.clear();
            tupleNodes.reserve(nodes.size());
            countCupleNodes = 0;
            removedIsolatedNodes = 0;
        
            for (auto r_  = nodesToBeRemoved.begin(); r_ != nodesToBeRemoved.end(); ++r_) { // neighbors which we need to remove
                NodeID r = *r_;
                removeNode(r, nodes, degreeSum, nodeDegrees, level);
        
                std::unordered_set<NodeID>& neighbors = ineligibleNodes[r]; // update neighbors of neighbors
                neighbors.erase(node);
                for (auto n_ = neighbors.begin(); n_ != neighbors.end(); ++n_) {
                    NodeID n = *n_;
                    std::unordered_set<NodeID>& nNeighbors = ineligibleNodes[n];
                    nNeighbors.erase(r);
                    // n became isolated and is NOT one of the removed nodes => add it to the current set
                    if (nNeighbors.size() == 0 && nodesToBeRemoved.find(n) == nodesToBeRemoved.end()) { 
                        addToSubsetpair(n, nodes, degreeSum, nodeDegrees, current, level);
                        isolatedNodes.push_back(n);
                        removedIsolatedNodes++;
                        assert(degreeSum < std::numeric_limits<NodeID>::max() - 1000);
                    } else if (nNeighbors.size() == 1) {
                        NodeID neighbor = *nNeighbors.begin();
                        std::unordered_set<NodeID>& neighborSet = ineligibleNodes[neighbor];
                        if (neighborSet.size() == 1 && nNeighbors.find(neighbor) == nNeighbors.end()) { // choose the node with the larger degreeSum
                            assert(n == *neighborSet.begin()); // the neighbor of the neighbor has to be myself
                            tupleNodes.push_back(n);
                            tupleNodes.push_back(neighbor);
                            countCupleNodes += 2;
                            addTupleNodes(n, neighbor, nodes, degreeSum, nodeDegrees, current, level);
                        }
                    }
                }
            }
            assert(isolatedNodes.size() == removedIsolatedNodes);
            assert(tupleNodes.size() == countCupleNodes);
            
            if (current.second + degreeSum > best.second || best.first.empty() || (current.second + degreeSum == best.second && current.first.size() + nodes.size() > best.first.size())) {
                recursiveExhaustiveSearch(nodes, degreeSum, ineligibleNodes, nodeDegrees, best, current, level+1);
            }

            for (auto n_ = isolatedNodes.begin(); n_ != isolatedNodes.end(); n_++) {
                removeFromSubsetPair(*n_, nodes, degreeSum, nodeDegrees, current, level);
            }
            for (auto n_ = tupleNodes.begin(); n_ != tupleNodes.end(); n_++) {
                NodeID n1 = *n_;
                n_++;
                NodeID n2 = *n_;
                removeTupleNodes(n1, n2, nodes, degreeSum, nodeDegrees, current, level);
            }
            for (auto r_  = nodesToBeRemoved.begin(); r_ != nodesToBeRemoved.end(); ++r_) {
                NodeID r = *r_;
                addNode(r, nodes, degreeSum, nodeDegrees, level);
        
                std::unordered_set<NodeID>& neighbors = ineligibleNodes[r];
                for (auto n_ = neighbors.begin(); n_ != neighbors.end(); ++n_) {
                    NodeID n = *n_;
                    std::unordered_set<NodeID>& nNeighbors = ineligibleNodes[n];
                    nNeighbors.insert(r);
                }
                neighbors.insert(node);
            }
            assert(beforeDegreeSum == degreeSum);
            assert(nodes.size() == beforeSize);
            auto inserted = nodes.insert(node);
            assert(inserted.second);
            current.first.erase(node);
            current.second -= nodeDegrees[node];            
        }
    }

    std::unordered_set<NodeID> exhaustiveSearch(const RandomAccessSet<NodeID> &W, const Subgraph& s, std::unordered_set<NodeID> const& C) {
        assert(W.size() > 0);
        std::vector<NodeID> data = W.getData();
        std::unordered_set<NodeID> nodeData;
        nodeData.reserve(data.size());
        nodeData.insert(data.begin(), data.end());

        // pre calc node degrees and ineligibilty of nodes and degree sum
        std::unordered_map<NodeID, NodeID> nodeDegrees;
        nodeDegrees.reserve(nodeData.size());

        std::unordered_map<NodeID, std::unordered_set<NodeID>> ineligibleNodes;
        ineligibleNodes.reserve(nodeData.size());
        std::vector<NodeID> nodeDegreeSum;
        nodeDegreeSum.resize(nodeData.size());

        NodeID degreeSum = 0;
        std::unordered_set<NodeID> isolatedNodes;
        for (auto i  = nodeData.begin(); i != nodeData.end();) {            
            NodeID node = *i;
            NodeID degree = s.getNodeDegree(node);

            // count neighbors in W
            NodeID wNeighbors = 0;
            std::unordered_set<NodeID> neighbors = *s.getNeighbors(node);
            for (auto n_ = neighbors.begin(); n_ != neighbors.end(); ++n_) {
                NodeID neighbor = *n_;
                if (W.contains(neighbor)) {
                    wNeighbors++;
                }
            }

            // isolated nodes do not have to be taken into account
            if (wNeighbors == 0) {
                isolatedNodes.insert(node);
                i = nodeData.erase(i);
                continue;
            } else {
                ++i;
            }

            // calculate the degree to other nodes
            for (auto c_ = C.begin(); c_ != C.end(); ++c_) {
                NodeID c = *c_;
                if (neighbors.find(c) != neighbors.end()) {
                    degree--;
                }
            }
            
            nodeDegrees[node] = degree;
            degreeSum += degree;
        }

        // calculate the neighbors (wrt to W)
        for (auto i = nodeData.begin(); i != nodeData.end(); ++i) {
            NodeID node = *i;
            std::unordered_set<NodeID> newSet;
            std::unordered_set<NodeID> neighbors = *s.getNeighbors(node);
            for (auto n_ = neighbors.begin(); n_ != neighbors.end(); ++n_) {
                NodeID neighbor = *n_;
                if (nodeData.find(neighbor) != nodeData.end()) {
                    newSet.insert((neighbor));
                }
            }
            ineligibleNodes.insert(std::make_pair(node, newSet));
        }

        if (nodeData.size() > 0) {
            SubsetPair bestSet = recursiveExhaustiveSearchStart(nodeData, degreeSum, ineligibleNodes, nodeDegrees, 0);
            bestSet.first.insert(isolatedNodes.begin(), isolatedNodes.end());
            return bestSet.first;
        } else {
            return isolatedNodes;
        }
    }

    std::unordered_set<NodeID> calculateIndependentSet(xrlf::Subgraph &subgraph, XRLFParameters parameters) {
        // 1.
        bool setBest = false;
        NodeID best = 0;
        std::unordered_set<NodeID> C_ = std::unordered_set<NodeID>();
        std::unordered_set<NodeID> C0 = std::unordered_set<NodeID>();
        NodeID dMin = subgraph.getMinDegree();
        
        NodeID numNodes = subgraph.getNumberOfNodes();
        // 2.
        if (parameters.TRIALNUM == 1 && numNodes > parameters.SETLIM) {
            C0.insert(subgraph.randomMaxDegreeNode());            
        }

        // 3.
        if (std::min(parameters.TRIALNUM, parameters.SETLIM + dMin) >= numNodes) {
            parameters.TRIALNUM= 1;
            parameters.SETLIM= numNodes; 
        }

        // 4.
        for (int i = 0; i < parameters.TRIALNUM; i++) {
            std::unordered_set<NodeID> X;
            std::unordered_set<NodeID> C;
            // 4.1
            if (C0.size() > 0) {
                C = C0;
                auto neighbors = subgraph.getNeighbors(*C0.begin());
                X.insert((*neighbors).begin(), (*neighbors).end());
            }

            // 4.2
            RandomAccessSet<NodeID> W(subgraph.getNodes());
            for (auto x = X.begin(); x != X.end(); ++x) {
                W.erase(*x);
            }
            for (auto c = C.begin(); c != C.end(); ++c) {
                W.erase(*c);
            }            

            // 4.3
            while (W.size() > 0) {
                // 4.3.1
                if (W.size() <= parameters.SETLIM) {
                    std::unordered_set<NodeID> W_ = exhaustiveSearch(W, subgraph, C);
                    assert(W_.size() > 0);
                    C.insert(W_.begin(), W_.end());
                    NodeID totalDegree = 0;
                    for (auto c = C.begin(); c != C.end(); ++c) {
                        // C is an independent set. therefore there exist no edges between any elements of C. Therefore we can just sum the number of neighbors
                        totalDegree += subgraph.getNodeDegree(*c);
                    }
                    if (!setBest || totalDegree > best) {
                        best = totalDegree;
                        C_ = C;
                        setBest = true;
                    }
                    break;
                }

                // 4.3.2
                bool notSet = true;
                NodeID bestdegree = 0;
                NodeID cand; 

                std::uniform_int_distribution<NodeID> distr(0, W.size() - 1);

                for (int i = 0; i < parameters.CANDNUM; i++) {
                    auto r = distr(xrlf::rngGenerator);
                    NodeID u = W.getElementAt(r);
                    std::shared_ptr<std::unordered_set<NodeID>> neighbors = subgraph.getNeighbors(u);
                    NodeID deg = 0;
                    for (auto neighbor = (*neighbors).begin(); neighbor != (*neighbors).end(); ++neighbor) {
                        if (X.find(*neighbor) == X.end()) {
                            deg++;
                        }
                    }
                    if (notSet || deg > bestdegree) {
                        bestdegree = deg;
                        cand = u;
                        notSet = false;
                    }
                }

                // 4.3.3
                C.insert(cand);
                std::shared_ptr<std::unordered_set<NodeID>> neighbors = subgraph.getNeighbors(cand);                
                for (auto n = (*neighbors).begin(); n != (*neighbors).end(); ++n) {
                    X.insert(*n);
                    W.erase(*n);
                }
                auto sizeBefore = W.size();
                W.erase(cand);
            }
        }
        return C_;
    }

    NodeID Subgraph::getNodeDegree(NodeID node) const {
        return (*neighbors.find(node)->second).size();
    }
    
    std::unordered_set<NodeID> Subgraph::getNodes() const {
        return remainingNodes;
    }

    std::unordered_set<NodeID>& Subgraph::getNodesRef() {
        return remainingNodes;
    }

    std::shared_ptr<std::unordered_set<NodeID>> Subgraph::getNeighbors(NodeID node) const {
        return neighbors.find(node)->second;
    }

    NodeID Subgraph::getNumberOfNodes() const {
        return nodeCount;
    }

    NodeID Subgraph::getMinDegree() const {
        return minDegree;
    }

    Subgraph::Subgraph(const graph_access& G): originalGraphNodeCount(G.number_of_nodes()), minDegree(std::numeric_limits<NodeID>::max()), nodeCount(G.number_of_nodes()), neighbors(), maxDegreeNodes(), maxDegree(0) {
        for (NodeID node = 0; node < G.number_of_nodes(); node++) {
            remainingNodes.insert(node);
            NodeID nodeDegree = G.getNodeDegree(node);
            minDegree = std::min(nodeDegree, minDegree);
            maxDegree = std::max(maxDegree, nodeDegree);

            // Do we have nodes without neighbors? => possible in subgraphs..
            if (neighbors.find(node) == neighbors.end()) {
                neighbors[node] = std::make_shared<std::unordered_set<NodeID>>();
            }
            for (auto neighbor: G.neighbours(node)) {
                (*neighbors[node]).insert(neighbor);
            }
        }

        for (NodeID node = 0; node < G.number_of_nodes(); node++) {
            NodeID nodeDegree = (*(neighbors[node])).size();
            boost::heap::fibonacci_heap<NodeData>::handle_type h = nodeDegrees.push(NodeData(node, nodeDegree));
            handles[node] = h;
            if (nodeDegree == maxDegree) {
                maxDegreeNodes.insert(node);
            }
        }
    }

    NodeID Subgraph::getTotalNumberOfNodes() {
        return originalGraphNodeCount;
    }
    RandomAccessSet<NodeID> Subgraph::getMaxDegreeNodes() const {
        return maxDegreeNodes;
    }

    void Subgraph::removeNodes(std::unordered_set<NodeID> nodes) {
        nodeCount -= nodes.size();
        if (nodeCount == 0) {
            minDegree = 0;
        }  

        for(auto node_ = nodes.begin(); node_ != nodes.end(); ++node_) {
            NodeID node = *node_;
            remainingNodes.erase(node);
            std::shared_ptr<std::unordered_set<NodeID>> nodeNeighbors = neighbors[node];
            for (auto neighbor_ = (*nodeNeighbors).begin(); neighbor_ != (*nodeNeighbors).end(); ++neighbor_) {
                NodeID neighbor = *neighbor_;

                if (nodes.find(neighbor) != nodes.end()) {
                    continue;
                }
                // remove current node from neighbors of neighbor
                (*neighbors[neighbor]).erase(node);

                if (maxDegreeNodes.contains(neighbor)) {
                    maxDegreeNodes.erase(neighbor);
                }

                (*handles[neighbor]).neighborCount--;
                if (nodes.find(neighbor) == nodes.end() && (*handles[neighbor]).neighborCount < getMinDegree()) {
                    minDegree = (*handles[neighbor]).neighborCount;
                }
                nodeDegrees.decrease(handles[neighbor]);
            }
            if (maxDegreeNodes.contains(node)) {
                maxDegreeNodes.erase(node);
            }
            (*nodeNeighbors).erase(node);
            nodeDegrees.erase(handles[node]);
        }

        if (maxDegreeNodes.size() == 0 && !nodeDegrees.empty()) {
            NodeData maxDegreeNode = nodeDegrees.top();
            NodeID maxDeg = maxDegreeNode.neighborCount;
            for (auto nodeData = nodeDegrees.ordered_begin(); nodeData != nodeDegrees.ordered_end(); ++nodeData) {
                if ((*nodeData).neighborCount < maxDeg) {
                    break;
                }
                maxDegreeNodes.insert((*nodeData).nodeId);
            }
        }
    }

    NodeID Subgraph::randomMaxDegreeNode() const {
        assert(maxDegreeNodes.size() > 0);
        std::uniform_int_distribution<NodeID>  distr(0, maxDegreeNodes.size() - 1);
        auto r = distr(xrlf::rngGenerator);
        return maxDegreeNodes.getElementAt(r);
    }

    bool Node::isAdjacentTo(Color k) {
        return adjacentColors.find(k) != adjacentColors.end() && adjacentColors[k] > 0;
    }
    
    Node::Node(NodeID node, NodeID degree): adjacentColors(), nodeId{node}, uncoloredNodeCount{degree} {
    }

    Color Node::findUnusedColor(Color usedColors) {
        for (Color i = 0; i  < usedColors; i++) {
            if (adjacentColors.find(i) == adjacentColors.end()) {
                return i;
            }
        }
        return UNCOLORED;
    }

    bool Node::colorNeighbor(NodeID node, Color k) {
        auto present = adjacentColors.find(k);
        auto val = ++adjacentColors[k];
        uncoloredNodeCount--;
        return val == 1;
    }

    bool Node::uncolorNeighbor(NodeID node, Color k) {
        auto val = --adjacentColors[k];
        uncoloredNodeCount++;
        if (val == 0) {
            adjacentColors.erase(k);
        }
        return val == 0;
    }

    Color Node::getAdjacentColorCount() const {
        return static_cast<Color>(adjacentColors.size());
    }

    NodeID Node::getUncoloredNodeCount() const {
        return uncoloredNodeCount;
    }

    NodeID Node::getNodeID() const {
        return nodeId;
    }

    bool Node::operator< (const Node& rhs) const {
        if (getAdjacentColorCount() == rhs.getAdjacentColorCount()) {
            if(getUncoloredNodeCount() == rhs.getUncoloredNodeCount()) {
                return getNodeID() > rhs.getNodeID();
            } else {
                return getUncoloredNodeCount() < rhs.getUncoloredNodeCount();
            }
        } else {
            return getAdjacentColorCount() < rhs.getAdjacentColorCount();
        }
    }

    // ========================= END NODE ========================== //
    Colouring& ColouringState::getColouring() {
        return colouring;
    }

    Color ColouringState::getUsedColors() {
        return usedColors;
    }

    std::shared_ptr<xrlf::Node> ColouringState::getBestNode() {
        return uncoloredNodes.top();
    }

    NodeID ColouringState::uncoloredNodeCount() {
        return static_cast<NodeID>(uncoloredNodes.size());
    }

    ColouringState::ColouringState(Subgraph &G): colouring(G.getTotalNumberOfNodes(), std::numeric_limits<NodeID>::max()), usedColors(0), coloredNodes(), uncoloredNodes(), g(G) {
        g = G;
        std::unordered_set<NodeID> nodes = g.getNodes();
        for (auto node_ = nodes.begin(); node_ != nodes.end() ; ++node_) {
            NodeID node = *node_;
            NodeID degree = g.getNodeDegree(node);
            auto nodeInstance = std::make_shared<Node>(node, degree);
            auto handle = uncoloredNodes.push(nodeInstance);
            handles[node] = handle;
        }
    }

    void ColouringState::colorNode(NodeID u, Color k, bool isNewColor) {
        colouring[u] = k;
        usedColors += isNewColor;

        auto uncoloredNodesCount = uncoloredNodes.size();

        auto toBeErased = *handles[u];
        NodeID tbeNodeID = toBeErased->getNodeID();
        coloredNodes.insert(std::make_pair(u, toBeErased));
        auto handle = handles[tbeNodeID];
        uncoloredNodes.erase(handle);
        handles.erase(tbeNodeID);

        assert(uncoloredNodesCount - 1 == uncoloredNodes.size());

        for (const NodeID& neighbor: *(g.getNeighbors(u))) {
            auto it = handles.find(neighbor);
            bool isUncolored = it != handles.end();
            std::shared_ptr<Node> nodePtr = isUncolored ? *(it->second) : coloredNodes.find(neighbor)->second;
            NodeID oldUncoloredCount = nodePtr->getUncoloredNodeCount();

            bool priorityIncreased = nodePtr->colorNeighbor(u, k);

            assert(nodePtr->getUncoloredNodeCount() == oldUncoloredCount - 1);

            if (isUncolored) {
                if (priorityIncreased) { 
                    uncoloredNodes.increase(it->second);
                } else {
                    uncoloredNodes.decrease(it->second);
                }
                 
            }
        }
    }

    void ColouringState::uncolorNode(NodeID u, Color k, bool wasNewColor) {
        colouring[u] = UNCOLORED;
        usedColors -= wasNewColor;

        for (auto neighbor: *(g.getNeighbors(u))) {
            auto it = handles.find(neighbor);
            bool isUncolored = it != handles.end();

            auto nodePtr = isUncolored ? *(handles.find(neighbor)->second) : coloredNodes.find(neighbor)->second;
            
            NodeID oldUncoloredCount = nodePtr->getUncoloredNodeCount();
            bool priorityDecreased = nodePtr->uncolorNeighbor(u, k);
            assert(nodePtr->getUncoloredNodeCount() == oldUncoloredCount + 1);

            if (isUncolored) {
                if (priorityDecreased) {
                    uncoloredNodes.decrease(it->second);
                } else {
                    uncoloredNodes.increase(it->second);
                }
            }
        }

        NodeID uncoloredNodesCount = uncoloredNodeCount();
        assert(coloredNodes.find(u) != coloredNodes.end());
        auto node = coloredNodes.find(u)->second;
        auto handle = uncoloredNodes.push(node);
        handles[u] = handle;
        assert(uncoloredNodesCount + 1 == uncoloredNodeCount());
    }

    xrlf::Subgraph& ColouringState::getSubgraph() {
        return g;
    }

    void makeSameColouring(Colouring& src, Colouring& dest, std::unordered_set<NodeID>& nodes, Color& offset) {
        for (auto n_ = nodes.begin(); n_ != nodes.end(); ++n_) {
            dest[*n_] = offset + src[*n_];
        }
    }

    void findBestColouring(ColouringState &state, Colouring& best, Color& bestColors, Color& offset, int depth) {
        if (state.uncoloredNodeCount() == 1) {
            auto node = state.getBestNode();
            Color colorCount = state.getUsedColors();
            if (node->getAdjacentColorCount() < colorCount) {
                Color c = node->findUnusedColor(colorCount);
                state.colorNode(node->getNodeID(), c, false);
                makeSameColouring(state.getColouring(), best, state.getSubgraph().getNodesRef(), offset);
                bestColors = colorCount;
                state.uncolorNode(node->getNodeID(), c, false);
                return;
            } else if (colorCount + 1 < bestColors) {
                state.colorNode(node->getNodeID(), colorCount, true);
                makeSameColouring(state.getColouring(), best, state.getSubgraph().getNodesRef(), offset);
                bestColors = colorCount + 1;
                state.uncolorNode(node->getNodeID(), colorCount, true);
                return;
            } else { // in this case we keep our "best" colouring
                return;
            }
        } else {
            auto u = state.getBestNode();
            if (u->getAdjacentColorCount() == bestColors - 1) {
                return;
            }
            Color usedColors =  state.getUsedColors();
            for (Color i = 0; i < usedColors; i++) {
                if (u->isAdjacentTo(i)) {
                    continue;
                }
                state.colorNode(u->getNodeID(), i, false);
                findBestColouring(state, best, bestColors, offset, depth + 1);
                state.uncolorNode(u->getNodeID(), i, false);
                if (bestColors == usedColors) {
                    return;
                }
            }
    
            if (usedColors < bestColors - 1) {
                state.colorNode(u->getNodeID(), usedColors, true);
                findBestColouring(state, best, bestColors, offset, depth + 1);
                state.uncolorNode(u->getNodeID(), usedColors, true);
            }
            return;
        }
    }

    void findOptimalColouring(xrlf::Subgraph &G, Colouring& colouring, Color& offset) {
        ColouringState state(G);
        // puts the best colouring into colouring
        NodeID tooManyColors = G.getNumberOfNodes() + 1;
        findBestColouring(state, colouring, tooManyColors, offset, 0);

    }
}

