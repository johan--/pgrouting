///////////////////////////////////////////////////////////////////////////////
///  YenTopKShortestPathsAlg.cpp
///  The implementation of Yen's algorithm to get the top k shortest paths
///  connecting a pair of vertices in a graph.
///
///  @remarks <TODO: insert remarks here>
///
///  @author Yan Qi @date 7/10/2010
///
///  $Id: YenTopKShortestPathsAlg.cpp 65 2010-09-08 06:48:36Z yan.qi.asu $
///
///////////////////////////////////////////////////////////////////////////////

#include <deque>
#include <algorithm>
#include "GraphElements.h"
#include "DijkstraShortestPathAlg.h"
#include "YenTopKShortestPathsAlg.h"


void YenTopKShortestPathsAlg::clear() {
        DijkstraShortestPathAlg::clear();
        m_ResultList.clear();
        m_Heap.clear();
}

void YenTopKShortestPathsAlg::_init() {
        clear();
        BasePath shortestPath = Dijkstra(m_Source_id, m_Target_id);
        DijkstraShortestPathAlg::clear();
         if (!shortestPath.isEmpty()) { 
            m_ResultList.push_back(shortestPath);
        }
}


std::deque<BasePath> YenTopKShortestPathsAlg::Yen(int  source, int  sink, int K) {
        clear();
        if (source !=sink && K > 0 && exist_vertex(source) && exist_vertex(sink)) {
             m_Source_id = source;
             m_Target_id = sink;
             sourceID = find_vertex(source);
             targetID = find_vertex(sink);
             get_shortest_paths(find_vertex(source), find_vertex(sink), K);
        }
        return m_ResultList;
}


BasePath YenTopKShortestPathsAlg::get_shortest_path(POS  sourcePos, POS  targetPos) {
        int source_id = m_vtVertices[ sourcePos ].getOriginalID();
        int target_id = m_vtVertices[ targetPos ].getOriginalID();
        return Dijkstra(source_id,  target_id);
}

void YenTopKShortestPathsAlg::next() {
        POS currPathId =  m_ResultList.size()-1;
        BasePath curr_result_path = m_ResultList[ currPathId ];


        POS spurNode;
        BasePath rootPath;
        for (POS i = 0; i < curr_result_path.size(); ++i) {
            BaseEdge spurEdge (curr_result_path[i]);
            spurNode = spurEdge.getStart();
            curr_result_path.subPath(rootPath, i);


               // for each path p in A:
               //   if rootPath == p.nodes(0, i):
               //     Remove the links that are part of the previous shortest paths which share the same root path.
               //     remove p.edge(i, i + 1) from Graph;

            POS edgeToBeRemoved;
            for (POS j=0; j < m_ResultList.size(); j++) {
               BasePath workingPath = m_ResultList[j];  // TODO(vicky): can be placed inside the condition
               if (rootPath.isEqual(workingPath)) {
                   if ( i < workingPath.size()) { 
                      edgeToBeRemoved = workingPath[i].getNID();
                      remove_edge(edgeToBeRemoved);
                   }
               }
            }

            removeNodes(rootPath);

            BasePath spurPath;
            spurPath = DijkstraShortestPathAlg::Dijkstra(spurNode , targetID, true);
            DijkstraShortestPathAlg::clear();

            if (spurPath.size() > 0) {
                // Entire path is made up of the root path and spur path.
                rootPath.append(spurPath);  // this should also update the cost of the totalPath

                // Add the potential k-shortest path to the heap.
                if (rootPath.FromTo(sourceID, targetID)) {
                   m_Heap.push_back(rootPath);
                }
           }
        }
}

void YenTopKShortestPathsAlg::semiOrderHeap() {
   if (m_Heap.size()==0) return;
   for (POS i = m_Heap.size()-1; i > 0 ; i--) {
       if ( m_Heap[i].Weight() <  m_Heap[i-1].Weight() )
           std::swap(m_Heap[i],  m_Heap[i-1]);
   }           

}

void YenTopKShortestPathsAlg::get_shortest_paths(POS source_id, POS target_id, int K) {
          _init();  // get the best using Dijkstra
          if (m_ResultList.size() == 0) return; //no path found
          while ( m_ResultList.size() < (unsigned int) K ) {
                next();
                if ( m_Heap.size() == 0 ) break;
// TODO(Vicky): do the sorting
                semiOrderHeap();
                m_ResultList.push_back(m_Heap[0]);
                m_Heap.erase(m_Heap.begin());
          }
}
