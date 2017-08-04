#include "octree.h"

namespace oct {
    leafData g_leafData;

    OctNode g_octNode[max_octnodes];
    std::mutex g_octNode_mut;

    OctNode g_rootNode;

};