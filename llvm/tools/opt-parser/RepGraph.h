#pragma once
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
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
      auto Kind = Entry.getKind();
      if((Kind == EntryKind::Replace || Kind == EntryKind::RepOperand)
	 && !Entry.isReplaceWithValue()){
	//Add edge
	errs() << "Making edge for entry " << Entry.toString() << "\n";
	add_edge(Entry.getInstID1(), Entry.getInstID2(), G);
      }
    }
  }
  printRG(G);
  errs() << "\n";

  //Add debug locations
  for(auto V : make_iterator_range(vertices(G))){
    for(auto* I : Map[V]){
      if(auto Loc = I->getDebugLoc()){
	G[V].Locations.insert(SourceLocation(Loc.get()));
      }
    }
  }
  printRG(G);
  return G;
}
