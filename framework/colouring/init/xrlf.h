#pragma once

#include "colouring/graph_colouring.h"
#include <unordered_set>
#include <unordered_map>
#include <boost/heap/fibonacci_heap.hpp>

namespace graph_colouring {

    /**
     * This version ignores the given color count.
     *
     */
    Colouring initByXRLFIgnored(const graph_access &G,
                                    const ColorCount _);
    /**
     * This version randomly colors the remaining nodes after reaching k colors. Note that this method
     * does not guarantee to return a valid coloring.
     * The random colors are between 0 and k - 1
     */
    Colouring initByXRLFRandom(const graph_access &G,
                                     const ColorCount k);
    /**
     * This version uses at most k colors and leaves all remaining nodes uncolored.
     */
    Colouring initByXRLFUncolored(const graph_access &G,
                                     const ColorCount k);

};

namespace xrlf {
    extern std::random_device randomNumberDevice;
    extern std::mt19937 rngGenerator;


    enum XRLFMode {
        IGNORE_COLORCOUNT,
        RANDOM_COLOR_REMAINNIG,
        UNCOLOR_REMAINING
    };
    struct XRLFParameters {
        NodeID EXACTLIM ;
        NodeID TRIALNUM;
        NodeID SETLIM;
        NodeID CANDNUM;
        XRLFMode MODE;
        graph_colouring::ColorCount COLORCOUNT;
        XRLFParameters(): EXACTLIM(50), TRIALNUM(128), SETLIM(30), CANDNUM(50), MODE(XRLFMode::IGNORE_COLORCOUNT), COLORCOUNT(0) {}
    };

    /**
     * This function is called by the methods in graph_colouring 
     */
    graph_colouring::Colouring initByXRLF(const graph_access &G, XRLFParameters parameters);

    bool isIndependentSet(std::unordered_set<NodeID> set, const graph_access &G);
        
    template <class data_type> class RandomAccessSet {
        public:
            RandomAccessSet(std::unordered_set<data_type> content);
            RandomAccessSet();
            void erase(data_type element);
            data_type getElementAt(typename std::vector<data_type>::size_type position) const;
            typename std::vector<data_type>::size_type size() const;
            bool contains(data_type date) const;
            void insert(data_type date);
            const std::vector<data_type> getData() const;
        private:
            std::vector<data_type> data;
            std::unordered_map<data_type, typename std::vector<data_type>::size_type> positions;
    };

    template <class data_type> const std::vector<data_type> RandomAccessSet<data_type>::getData() const {
        return data;
    }

    template <class data_type> RandomAccessSet<data_type>::RandomAccessSet() {
    }

    template <class data_type> void RandomAccessSet<data_type>::insert(data_type date) {
        if (positions.find(date) != positions.end()) {
            return;
        }
        data.emplace_back(date);
        positions[date] = data.size() - 1;
    }

    template <class data_type> bool RandomAccessSet<data_type>::contains(data_type date) const {
        return positions.find(date) != positions.end();
    }
    template <class data_type> RandomAccessSet<data_type>::RandomAccessSet(std::unordered_set<data_type> content) {
        data.reserve(content.size());
        typename std::vector<data_type>::size_type i = 0;
        for (auto n_ = content.begin(); n_ != content.end(); ++n_) {
            data_type n = *n_;
            data.push_back(n);
            positions[n] = i;
            i++;
        }

        assert(size() == content.size());
    }

    template <class data_type> data_type RandomAccessSet<data_type>::getElementAt(typename std::vector<data_type>::size_type position) const {
        return data.at(position);
    }

    template <class data_type> typename std::vector<data_type>::size_type RandomAccessSet<data_type>::size() const {
        return data.size();
    }

    template <class data_type> void RandomAccessSet<data_type>::erase(data_type element) {
        if (positions.find(element) == positions.end()) {
            return;
        }
        if (data.size() > 1) {
            data_type lastElement = data.back();
            typename std::vector<data_type>::size_type position = positions[element];
            data[position] = lastElement;
            positions[lastElement] = position;
        }
        positions.erase(element);
        data.pop_back();
    }

    struct NodeData {
        NodeID nodeId;
        NodeID neighborCount;

        NodeData(NodeID id, NodeID neighbors): nodeId(id), neighborCount(neighbors) {}

        bool operator< (NodeData const &rhs) const {
            return neighborCount < rhs.neighborCount;
        }
    };

    class Subgraph {
        public:
            Subgraph(const graph_access& G);
            void removeNodes(std::unordered_set<NodeID> nodes);
            NodeID getNodeDegree(NodeID node) const;
            NodeID randomMaxDegreeNode() const;
            NodeID getMinDegree() const;
            NodeID getNumberOfNodes() const;
            std::shared_ptr<std::unordered_set<NodeID>> getNeighbors(NodeID node) const;
            std::unordered_set<NodeID> getNodes() const;
            std::unordered_set<NodeID>& getNodesRef() ;
            RandomAccessSet<NodeID> getMaxDegreeNodes() const;
            NodeID getTotalNumberOfNodes();
        private:
            RandomAccessSet<NodeID> maxDegreeNodes;
            boost::heap::fibonacci_heap<NodeData> nodeDegrees;
            std::unordered_map<NodeID, typename boost::heap::fibonacci_heap<NodeData>::handle_type> handles;
            NodeID nodeCount;
            NodeID originalGraphNodeCount;
            NodeID minDegree;
            NodeID maxDegree;
            std::unordered_set<NodeID> remainingNodes;
            std::unordered_map<NodeID, std::shared_ptr<std::unordered_set<NodeID>>> neighbors;
    };

    class Node {
        public:
            bool colorNeighbor(NodeID node, Color k);
            bool uncolorNeighbor(NodeID node, Color k);
            Color getAdjacentColorCount() const;
            NodeID getUncoloredNodeCount() const;
            NodeID getNodeID() const;
            Color findUnusedColor(Color usedColors);
            Node(NodeID node, NodeID degree);
            bool operator< (const Node& rhs) const;
            bool isAdjacentTo(Color k);
        private:
            NodeID nodeId;
            std::unordered_map<Color, NodeID> adjacentColors;
            NodeID uncoloredNodeCount;
    };

    struct node_compare {
        bool operator() (const std::shared_ptr<xrlf::Node>& lhs, const std::shared_ptr<xrlf::Node>& rhs) const {
            bool result = *lhs < *rhs;
            return result;
        }
    };

    class ColouringState {
        private:
            graph_colouring::Colouring colouring;
            Color usedColors;
            boost::heap::fibonacci_heap<std::shared_ptr<xrlf::Node>, boost::heap::compare<node_compare>> uncoloredNodes;
            std::unordered_map<NodeID, typename boost::heap::fibonacci_heap<std::shared_ptr<xrlf::Node>, boost::heap::compare<node_compare>>::handle_type> handles;
            std::unordered_map<NodeID, std::shared_ptr<Node>> coloredNodes;
            xrlf::Subgraph& g;

        public:
            xrlf::Subgraph& getSubgraph();
            void colorNode(NodeID u, Color k, bool isNewColor);
            void uncolorNode(NodeID u, Color k, bool wasNewColor);
            std::shared_ptr<xrlf::Node> getBestNode();
            NodeID uncoloredNodeCount();
            Color getUsedColors();
            graph_colouring::Colouring& getColouring();
            ColouringState(xrlf::Subgraph &G);
    };

    typedef std::pair<std::unordered_set<NodeID>, NodeID> SubsetPair;
    std::unordered_set<NodeID> exhaustiveSearch(const RandomAccessSet<NodeID> &W, const xrlf::Subgraph& s, std::unordered_set<NodeID> const& C);
    std::unordered_set<NodeID> calculateIndependentSet(xrlf::Subgraph &subgraph, XRLFParameters parameters);    
    void findOptimalColouring(xrlf::Subgraph &G, graph_colouring::Colouring& colouring, Color& offset);
};