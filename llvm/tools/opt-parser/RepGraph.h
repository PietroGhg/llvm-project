#pragma once
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <iterator>
#include <vector>
#include "llvm/IR/DebugInfoMetadata.h"
#include "Log.h"
#include "InstrIDMap.h"
#include "SourceLocation.h"

using namespace boost;
using namespace llvm;
//!Struct for the vertex data, a delay for each vertex
struct RepVertex {
  unsigned long InstID;
  std::set<SourceLocation> Locations;
};


struct RepEdge {
};

//! Data type for a graph that represents the "Replaced By" relation
typedef adjacency_list<vecS, 
		       vecS,
		       bidirectionalS,
		       RepVertex,
		       RepEdge> RepGraph;

using Vertex = graph_traits<RepGraph>::vertex_descriptor;


inline void printRG(const RepGraph& G){
  for(auto V : make_iterator_range(vertices(G))){
    errs() << G[V].InstID << ": ";
    for(auto E : make_iterator_range(out_edges(V,G))){
      errs() << G[source(E, G)].InstID << ", " << G[target(E,G)].InstID;
    }
    for (const SourceLocation& Loc : G[V].Locations) {
      errs() << " Loc: " << Loc.toString() << " ";
    }
    errs() << "\n";
  }
}

//MYTODO: deal with instructions that are created and removed in the same pass
inline RepGraph getRepGraph(const std::vector<Log> Logs, IDInstrMap_t& Map){
  RepGraph G(Map.rbegin()->first);
  
  //set id i to node i
  for(auto V : make_iterator_range(vertices(G)))
    G[V].InstID = V;
  
  for(auto& Log : Logs){
    for(auto& Entry : Log.getEntries()){
      switch(Entry.getKind()){
      case(EntryKind::RepOperand):
      case(EntryKind::Replace):
      case(EntryKind::Move):
	if(!Entry.isReplaceWithValue())
	  add_edge(Entry.getInstID1(), Entry.getInstID2(), G);
	break;
      case(EntryKind::Create):
      case(EntryKind::Remove):
	break;;
      }
    }
  }

  //Add debug locations
  for(auto V : make_iterator_range(vertices(G))){
    for(auto* I : Map[V]){
      if(auto Loc = I->getDebugLoc()){
	G[V].Locations.insert(SourceLocation(Loc.get()));
      }
    }
  }

  return G;
}

inline void propagateLocations(RepGraph& G){
  //Topological sort the graph
  std::deque<Vertex> Sorted;
  topological_sort(G, std::front_inserter(Sorted));

  //Following the topological sort, insert the locations
  //of the source in the target.
  for(auto V : Sorted){
    for(auto E : make_iterator_range(out_edges(V,G)))
      for(auto& Loc : G[V].Locations)
	G[target(E,G)].Locations.insert(Loc);
  }
}

