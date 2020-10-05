#pragma once
#include <map>
#include <iostream>
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"
#include "Log.h"

using namespace std;
using namespace llvm;

using InstrIDMap_t = map<Instruction*, unsigned long>;
using IDInstrMap_t = map<unsigned long, Instruction*>;
using InstrEntryMap_t = map<Instruction*, vector<Entry>>;

inline InstrIDMap_t getInstrIDmap(Module* M){
  InstrIDMap_t Res;
  for(auto& F : *M){
    for(auto& BB : F){
      for(auto& I : BB){
	if(I.hasID()){
	  Res[&I] = I.getIDInt();
	}
      }
    }
  }
  return Res;
}

inline IDInstrMap_t getIDInstrMap(Module* M){
  IDInstrMap_t Res;
  for(auto& F : *M){
    for(auto& BB : F){
      for(auto& I : BB){
	if(I.hasID()){
	  Res[I.getIDInt()] = &I;
	}
      }
    }
  }
  return Res;
}

inline void printModule(Module* M, InstrEntryMap_t& InstrEntry){
  auto Map = getInstrIDmap(M);
  for(auto& F : *M){
    errs() << F.getName() << "\n";
    for(auto& BB : F){
      errs() << BB.getName().str() << "\n";
      for(auto& I : BB){
	errs() << I << " " << Map[&I] << " ";
	for(auto& Entry : InstrEntry[&I])
	  errs() << Entry.toString() << " ";
	errs() << "\n";
      }
    }
  }
}


inline void updateInstEntryMap(Module* M,
			       const Log& L,
			       InstrEntryMap_t& Map){

  IDInstrMap_t IDInstrBefore = getIDInstrMap(M);
  InstrIDMap_t InstrIDIDBefore = getInstrIDmap(M);

  for(auto& Entry : L.getEntries()){
    //if the module is before the log, do not mark with the create entries
    if(Entry.getKind() != EntryKind::Create){
      Instruction* IBefore = IDInstrBefore[Entry.getInstID1()];
      Map[IBefore].push_back(Entry);
    }	
  }
}
  


      


