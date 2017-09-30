#include "data_structure/io/graph_io.h"
#include "util/graph_util.h"
#include "colouring/init/xrlf.h"
#include <unordered_map>
#include <unordered_set>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <stdlib.h>

// TODO: add test with exactlim 0

TEST(XRLF, FindOptimalColouring) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);

    graph_colouring::Colouring s_init(G.number_of_nodes(), std::numeric_limits<NodeID>::max());
    Color offset = 0;
    xrlf::findOptimalColouring(s, s_init, offset);

    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, s_init), 0);
    ASSERT_EQ(graph_colouring::colorCount(s_init), 3);
}

// TODO: add graph_colouring::isfullyColored

TEST(XRLF, DSJC1000) {
    xrlf::rngGenerator = std::mt19937(1);
    graph_access G;
    std::string graph_filename = "../../input/DSJC1000.5-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);


    xrlf::XRLFParameters parameters;    
    parameters.EXACTLIM = 10;
    parameters.TRIALNUM = 10;
    parameters.SETLIM = 10;
    parameters.CANDNUM = 10;
    auto c = xrlf::initByXRLF(G, parameters);
    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, c), 0);
}

TEST(XRLF, DSJC1000_EXACTLIM) {
    xrlf::rngGenerator = std::mt19937(1);
    graph_access G;
    std::string graph_filename = "../../input/DSJC1000.5-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);

    xrlf::XRLFParameters parameters;    
    parameters.EXACTLIM = 70;
    parameters.TRIALNUM = 1;
    parameters.SETLIM = 10;
    parameters.CANDNUM = 10;
    auto c = xrlf::initByXRLF(G, parameters);
    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, c), 0);
}


TEST(XRLF, Miles250Setlim16) {
    xrlf::XRLFParameters parameters;    
    parameters.EXACTLIM = 0;
    parameters.TRIALNUM = 1;
    parameters.SETLIM = 16;
    parameters.CANDNUM = 10;
    xrlf::rngGenerator = std::mt19937(1);
    graph_access G;
    std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    auto c = xrlf::initByXRLF(G, parameters);

    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, c), 0);
    ASSERT_EQ(graph_colouring::colorCount(c), 8);

    for (NodeID i = 0; i < c.size(); i++) {
        ASSERT_NE(c[i], std::numeric_limits<NodeID>::max());
    }
}

/*TEST(XRLF, Miles250Setlim63) {
    xrlf::EXACTLIM = 0;
    xrlf::TRIALNUM = 1;
    xrlf::SETLIM = 63;
    xrlf::rngGenerator = std::mt19937(1);
    graph_access G;
    std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    auto c = graph_colouring::initByXRLFIgnored(G, 0);
    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, c), 0);
    ASSERT_EQ(graph_colouring::colorCount(c), 11);

    for (NodeID i = 0; i < c.size(); i++) {
        ASSERT_NE(c[i], std::numeric_limits<NodeID>::max());
    }

}*/

TEST(XRLF, Miles250Test) {
    xrlf::rngGenerator = std::mt19937(1);
    graph_access G;
    std::string graph_filename = "../../input/miles250-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::XRLFParameters parameters;    
    parameters.EXACTLIM = 10;
    parameters.TRIALNUM = 10;
    parameters.SETLIM = 10;
    parameters.CANDNUM = 10; 
    auto c = xrlf::initByXRLF(G, parameters);
    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, c), 0);
    ASSERT_EQ(graph_colouring::colorCount(c), 9);

    for (NodeID i = 0; i < c.size(); i++) {
        ASSERT_NE(c[i], std::numeric_limits<NodeID>::max());
    }

}

TEST(XRLF, DSJC500) {
    xrlf::rngGenerator = std::mt19937(1);
    graph_access G;
    std::string graph_filename = "../../input/DSJC500.5-sorted.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::XRLFParameters parameters;    
    parameters.EXACTLIM = 10;
    parameters.TRIALNUM = 10;
    parameters.SETLIM = 10;
    parameters.CANDNUM = 10;
    auto c = xrlf::initByXRLF(G, parameters);
    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, c), 0);
    ASSERT_EQ(graph_colouring::colorCount(c), 63);

    for (NodeID i = 0; i < c.size(); i++) {
        ASSERT_NE(c[i], std::numeric_limits<NodeID>::max());
    }

}

TEST(XRLF, InitByXRLF) {
    xrlf::rngGenerator = std::mt19937(1);
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::XRLFParameters parameters;    
    parameters.EXACTLIM = 10;
    parameters.TRIALNUM = 10;
    parameters.SETLIM = 10;
    parameters.CANDNUM = 10;
    auto c = xrlf::initByXRLF(G, parameters);
    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, c), 0);
    ASSERT_EQ(graph_colouring::colorCount(c), 3);   
}

TEST(XRLF, NODE) {
    NodeID node = 5;
    NodeID degree = 5;
    xrlf::Node n(node, degree);

    ASSERT_EQ(n.getAdjacentColorCount(), 0);
    ASSERT_FALSE(n.isAdjacentTo(10));
    ASSERT_EQ(n.getNodeID(), 5);
    ASSERT_EQ(n.getUncoloredNodeCount(), 5);
    ASSERT_EQ(n.findUnusedColor(1), 0);

    n.colorNeighbor(4, 0);
    n.colorNeighbor(3,2);
    ASSERT_EQ(n.getAdjacentColorCount(), 2);
    ASSERT_EQ(n.getUncoloredNodeCount(), 3);
    ASSERT_EQ(n.findUnusedColor(3), 1);

    ASSERT_TRUE(n.isAdjacentTo(0));
    ASSERT_TRUE(n.isAdjacentTo(2));
    ASSERT_FALSE(n.isAdjacentTo(1));

    n.uncolorNeighbor(4, 0);
    ASSERT_EQ(n.getAdjacentColorCount(), 1);
    ASSERT_EQ(n.getUncoloredNodeCount(), 4); // TODO rename to getUncoloredNeighborCount
    ASSERT_FALSE(n.isAdjacentTo(0));
    ASSERT_TRUE(n.isAdjacentTo(2));
    ASSERT_EQ(n.findUnusedColor(3), 0);

    n.colorNeighbor(4, 2); // same color as 3
    ASSERT_EQ(n.getAdjacentColorCount(), 1);
}

TEST(XRLF, NodeOrder) {
    boost::heap::fibonacci_heap<std::shared_ptr<xrlf::Node>, boost::heap::compare<xrlf::node_compare>> nodes;
    auto n1 = std::make_shared<xrlf::Node>(0, 5);
    auto n2 = std::make_shared<xrlf::Node>(1, 4);

    nodes.push(n1);
    nodes.push(n2);

    ASSERT_EQ(nodes.top()->getNodeID(), 0);

    auto n3 = std::make_shared<xrlf::Node>(2, 1);
    n3->colorNeighbor(5, 0);
    nodes.push(n3);
    ASSERT_EQ(nodes.top()->getNodeID(), 2);

    auto n4 = std::make_shared<xrlf::Node>(3, 3);
    n4->colorNeighbor(2, 1);
    n4->colorNeighbor(2, 2);
    auto n4Handle = nodes.push(n4);

    ASSERT_EQ(nodes.top()->getNodeID(), 3);

    bool decreased = n4->uncolorNeighbor(2, 2);
    if (decreased) {
        nodes.decrease(n4Handle);
    } else {
        nodes.increase(n4Handle);
    }

    ASSERT_EQ(nodes.top()->getNodeID(), 3);
}

TEST(XRLF, ColouringStateInit) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);
    std::unordered_set<NodeID> nodesToBeRemoved;
    nodesToBeRemoved.insert(3);
    nodesToBeRemoved.insert(5);
    s.removeNodes(nodesToBeRemoved);

    xrlf::ColouringState cs(s);
    ASSERT_EQ(cs.getUsedColors(), 0);
    ASSERT_EQ(cs.uncoloredNodeCount(), s.getNumberOfNodes());

    std::shared_ptr<xrlf::Node> n1 = cs.getBestNode();
    NodeID bestNode = n1->getNodeID();    
    ASSERT_EQ(bestNode, 1);
    cs.colorNode(bestNode, 0, true);
    ASSERT_EQ(cs.uncoloredNodeCount(), s.getNumberOfNodes() - 1);
    ASSERT_EQ(cs.getUsedColors(), 1);

    graph_colouring::Colouring c = cs.getColouring();
    for (NodeID n = 0; n < s.getNumberOfNodes(); n++) {
        if (n != bestNode) {
            ASSERT_EQ(c[n], std::numeric_limits<NodeID>::max());
        } else {
            ASSERT_EQ(c[n], 0);
        }
    }    

    bestNode = cs.getBestNode()->getNodeID();
    ASSERT_EQ(bestNode, 0);
}

TEST(XRLF, FindOptimalColouringSubgraph) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);
    std::unordered_set<NodeID> nodesToBeRemoved;
    nodesToBeRemoved.insert(3);
    nodesToBeRemoved.insert(5);
    s.removeNodes(nodesToBeRemoved);

    graph_colouring::Colouring s_init(G.number_of_nodes(), std::numeric_limits<NodeID>::max());
    Color offset = 0;
    xrlf::findOptimalColouring(s, s_init, offset);
    NodeID uncoloredNodes = graph_colouring::numberOfUncolouredNodes(s_init);

    ASSERT_EQ(s_init.size(), G.number_of_nodes());
    ASSERT_EQ(graph_colouring::numberOfConflictingEdges(G, s_init), 0);
    ASSERT_EQ(uncoloredNodes, nodesToBeRemoved.size());
    ASSERT_EQ(G.number_of_nodes() - uncoloredNodes, s.getNumberOfNodes());
}

TEST(XRLF, ExhaustiveSearch) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);
    
    std::unordered_set<NodeID> content;
    content.insert(0);
    content.insert(1);
    content.insert(5); // Note that all neighbors of C are ineligible for W
    xrlf::RandomAccessSet<NodeID> W(content);
    std::unordered_set<NodeID> C;
    C.insert(3);

    auto bestSet = xrlf::exhaustiveSearch(W, s, C);

    ASSERT_EQ(bestSet.size(), 1);
    NodeID beg = *bestSet.begin();
    ASSERT_TRUE(beg == 1 || beg == 5);
}

TEST(XRLF, ExhaustiveSearch3) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);
    
    std::unordered_set<NodeID> content;
    content.insert(0);
    content.insert(3);
    xrlf::RandomAccessSet<NodeID> W(content);
    std::unordered_set<NodeID> C;
    C.insert(1);
    C.insert(5);

    auto bestSet = xrlf::exhaustiveSearch(W, s, C);

    ASSERT_EQ(bestSet.size(), 2);
    ASSERT_EQ(*bestSet.begin(), 3);
    ASSERT_EQ(*(++bestSet.begin()), 0);
}

TEST(XRLF, ExhaustiveSearch2) {
    graph_access G;
    G.start_construction(8, 24);
    for (NodeID i = 0; i < 8; i++) {
        G.new_node();
    }
    G.new_edge(0, 1);
    G.new_edge(0, 5);
    G.new_edge(0, 6);
    G.new_edge(1, 0);
    G.new_edge(1, 2);
    G.new_edge(1, 3);
    G.new_edge(1, 4);
    G.new_edge(2, 1);
    G.new_edge(2, 4);
    G.new_edge(2, 6);
    G.new_edge(2, 7);
    G.new_edge(3, 1);
    G.new_edge(3, 4);
    G.new_edge(3, 5);
    G.new_edge(4, 1);
    G.new_edge(4, 2);
    G.new_edge(4, 3);
    G.new_edge(4, 7);
    G.new_edge(5, 0);
    G.new_edge(5, 3);
    G.new_edge(6, 0);
    G.new_edge(6, 2);
    G.new_edge(7, 2);
    G.new_edge(7, 4);

    G.finish_construction();
    xrlf::Subgraph s(G);
    
    std::unordered_set<NodeID> content;
    content.insert(1);
    content.insert(2);
    content.insert(4);
    content.insert(6);
    content.insert(7);
    
    xrlf::RandomAccessSet<NodeID> W(content);
    std::unordered_set<NodeID> C;
    C.insert(5);

    auto bestSet = xrlf::exhaustiveSearch(W, s, C);
    ASSERT_EQ(bestSet.size(), 3);

    for (auto b = bestSet.begin(); b != bestSet.end(); ++b) {
        NodeID n = *b;
        ASSERT_TRUE(n == 6 || n == 7 || n == 1);
    }
}

TEST(XRLF, RandomAccessSet) {
    std::unordered_set<NodeID> content;
    content.insert(2);
    content.insert(3);
    xrlf::RandomAccessSet<NodeID> w(content);

    ASSERT_EQ(w.size(), 2);

    for (typename std::vector<NodeID>::size_type n = 0; n < w.size(); n++) {
        ASSERT_TRUE(w.getElementAt(n) == 2 || w.getElementAt(n) == 3);
    }

    w.erase(2);
    ASSERT_EQ(w.size(), 1);
    ASSERT_EQ(w.getElementAt(0), 3);
    ASSERT_TRUE(w.contains(3));
    ASSERT_FALSE(w.contains(2));

    w.insert(5);
    ASSERT_EQ(w.size(), 2);
    ASSERT_EQ(w.getElementAt(1), 5);
    ASSERT_TRUE(w.contains(5));    
}


TEST(XRLF, SubgraphInit) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);
    
    ASSERT_EQ(s.getNumberOfNodes(), G.number_of_nodes());
    ASSERT_EQ(s.getNodes().size(), G.number_of_nodes());
    auto nodes = s.getNodes();
    NodeID minDegree = std::numeric_limits<NodeID>::max();
    NodeID maxDegree = 0;
    for (auto n_ = nodes.begin(); n_ != nodes.end(); ++n_) {
        NodeID node = *n_;
        ASSERT_TRUE(node < G.number_of_nodes());
        ASSERT_TRUE(node >= 0);

        ASSERT_EQ(s.getNodeDegree(node), G.getNodeDegree(node));
        ASSERT_EQ((*s.getNeighbors(node)).size(), G.getNodeDegree(node));
        minDegree = std::min(minDegree, G.getNodeDegree(node));
        maxDegree = std::max(maxDegree, G.getNodeDegree(node));
        
        auto neighbors = G.neighbours(node);
        auto subgraph_neighbors = *s.getNeighbors(node);
        for (auto neighbor_ = neighbors.begin(); neighbor_ != neighbors.end(); ++neighbor_) {
            ASSERT_TRUE(subgraph_neighbors.find(*neighbor_) != subgraph_neighbors.end());
        }
    }

    ASSERT_EQ(s.getMinDegree(), minDegree);
    auto maxNode = s.randomMaxDegreeNode();
    NodeID deg = G.getNodeDegree(maxNode);
    ASSERT_EQ(deg, maxDegree);
}

TEST(XRLF, SubgraphMinDegree) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);
    
    ASSERT_EQ(s.getMinDegree(), 2);
    std::unordered_set<NodeID> nodesToBeRemoved;
    nodesToBeRemoved.insert(1);

    s.removeNodes(nodesToBeRemoved);
    ASSERT_EQ(s.getMinDegree(), 1);

    std::unordered_set<NodeID> nextNodesToBeRemoved;
    nextNodesToBeRemoved.insert(2);
    s.removeNodes(nextNodesToBeRemoved);

    ASSERT_EQ(s.getMinDegree(), 1);
}

TEST(XRLF, SubgraphGetNodes) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);
    
    ASSERT_EQ(s.getMinDegree(), 2);
    std::unordered_set<NodeID> nodesToBeRemoved;
    nodesToBeRemoved.insert(1);
    nodesToBeRemoved.insert(3);
    s.removeNodes(nodesToBeRemoved);

    auto nodes = s.getNodes();
    ASSERT_EQ(nodes.size(), 4);
    for (auto n_ = nodes.begin(); n_ != nodes.end(); ++n_) {
        ASSERT_NE(*n_, 1);
        ASSERT_NE(*n_, 3);
    }
}

TEST(XRLF, SubgraphMaxDegreeNodes) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);

    auto maxDeg = s.getMaxDegreeNodes();
    ASSERT_EQ(maxDeg.size(), 2);    
    ASSERT_EQ(s.getNodeDegree(maxDeg.getElementAt(0)), 3);
    ASSERT_EQ(s.getNodeDegree(maxDeg.getElementAt(1)), 3);

    std::unordered_set<NodeID> nodesToBeRemoved;
    nodesToBeRemoved.insert(4);
    s.removeNodes(nodesToBeRemoved);

    maxDeg = s.getMaxDegreeNodes();
    ASSERT_EQ(maxDeg.size(), 1);   
    ASSERT_EQ(maxDeg.getElementAt(0), 1); 
    ASSERT_EQ(s.getNodeDegree(maxDeg.getElementAt(0)), 3);

    std::unordered_set<NodeID> nextNodesToBeRemoved;
    nextNodesToBeRemoved.insert(1);
    s.removeNodes(nextNodesToBeRemoved);

    maxDeg = s.getMaxDegreeNodes();
    ASSERT_EQ(maxDeg.size(), 4);    
    for (NodeID i = 0; i < maxDeg.size(); i++) {
        ASSERT_EQ(s.getNodeDegree(maxDeg.getElementAt(i)), 1);
    }
}

TEST(XRLF, SubgraphNeighbors) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);

    auto neighbors = *s.getNeighbors(5);    

    ASSERT_EQ(neighbors.size(), 3);
    for (auto n_ = neighbors.begin(); n_ != neighbors.end(); ++n_) {
        NodeID neighbor = *n_;
        ASSERT_TRUE(neighbor == 0 || neighbor == 1 || neighbor == 4);
    }

    std::unordered_set<NodeID> nodeToBeRemoved;
    nodeToBeRemoved.insert(0);
    s.removeNodes(nodeToBeRemoved);

    neighbors = *s.getNeighbors(5);
    ASSERT_EQ(neighbors.size(), 2);
    for (auto n_ = neighbors.begin(); n_ != neighbors.end(); ++n_) {
        NodeID neighbor = *n_;
        ASSERT_TRUE(neighbor == 1 || neighbor == 4);
    }
    
}

TEST(XRLF, SubgraphNodeDegree) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);

    NodeID degree = s.getNodeDegree(5);
    ASSERT_EQ(degree, 3);

    std::unordered_set<NodeID> nodesToBeRemoved;
    nodesToBeRemoved.insert(0);
    nodesToBeRemoved.insert(1);
    s.removeNodes(nodesToBeRemoved);

    degree = s.getNodeDegree(5);
    ASSERT_EQ(degree, 1);    
}

TEST(XRLF, SubgraphRandomMaxDegreeNode) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);

    ASSERT_EQ(s.getMaxDegreeNodes().size(), 2);
    NodeID maxNode = s.randomMaxDegreeNode();

    ASSERT_TRUE(maxNode == 5 || maxNode == 1);

    std::unordered_set<NodeID> nodesToBeRemoved;
    nodesToBeRemoved.insert(5);
    nodesToBeRemoved.insert(1);
    s.removeNodes(nodesToBeRemoved);
    
    maxNode = s.randomMaxDegreeNode();
    ASSERT_TRUE(maxNode == 1 || maxNode == 2 || maxNode == 3);
    
}

TEST(XRLF, SubgraphDeconstruct) {
    graph_access G;
    std::string graph_filename = "../../input/simple.graph";
    graph_io::readGraphWeighted(G, graph_filename);
    xrlf::Subgraph s(G);

    std::unordered_set<NodeID> removedNodes;
    NodeID nodeCount = G.number_of_nodes();
    while (nodeCount > 0) {
        std::unordered_set<NodeID> nodesToBeRemoved;
        auto currentNodes = s.getNodes();
        nodesToBeRemoved.insert(*currentNodes.begin());
        nodesToBeRemoved.insert(*(++currentNodes.begin()));
        removedNodes.insert(nodesToBeRemoved.begin(), nodesToBeRemoved.end());
        nodeCount -= 2;
        s.removeNodes(nodesToBeRemoved);

        ASSERT_EQ(s.getNumberOfNodes(), nodeCount);
        ASSERT_EQ(s.getNodes().size(), nodeCount);

        auto nodes = s.getNodes();
        NodeID minDegree = std::numeric_limits<NodeID>::max();
        NodeID maxDegree = 0;

        for (auto n_ = nodes.begin(); n_ != nodes.end(); ++n_) {
            NodeID n = *n_;
            auto neighbors = *s.getNeighbors(n);
            for (auto neighbor_ = neighbors.begin(); neighbor_ != neighbors.end(); ++neighbor_) {
                NodeID neighbor = *neighbor_;
                // check that no neighor of subgraph is in the nodesToBeRemoved
                ASSERT_TRUE(nodesToBeRemoved.find(neighbor) == nodesToBeRemoved.end()); 
            }

            auto oldNeighbors = G.neighbours(n);
            NodeID nodeDegree = 0;
            for (auto neighbor_ = oldNeighbors.begin(); neighbor_ != oldNeighbors.end(); ++neighbor_) {
                NodeID neighbor = *neighbor_;
                if (removedNodes.find(neighbor) == removedNodes.end()) {
                    nodeDegree++;
                }
            }
            minDegree = std::min(minDegree, nodeDegree);
            maxDegree = std::max(maxDegree, nodeDegree);
            ASSERT_EQ(s.getNodeDegree(n), nodeDegree);
        }

        if (s.getNumberOfNodes() == 0) {
            minDegree = 0;
            maxDegree = 0;
        }
        ASSERT_EQ(s.getMinDegree(), minDegree);
        if (s.getNumberOfNodes() > 0) {
            NodeID maxNode = s.randomMaxDegreeNode();
            ASSERT_EQ(s.getNodeDegree(maxNode), maxDegree);
        }
    }
}