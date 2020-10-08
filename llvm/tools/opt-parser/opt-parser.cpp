#include <iostream>
#include <fstream>
#include "llvm/IR/LLVMContext.h"
#include "Log.h"
#include "LogParser.h"
#include "InstrIDMap.h"
#include "RepGraph.h"

using namespace std;

int main(int argc, char* argv[]){
  llvm::LLVMContext Ctx;
  fstream F(argv[1], fstream::in);
  vector<unique_ptr<llvm::Module>> Modules;
  vector<Log> Logs;
  parseLog(F, Ctx, Modules, Logs);
  IDInstrMap_t Map;
  for(auto& M : Modules){
    updateIDInstrMap(Map, M.get());
  }
  for(auto el : Map){
    errs() << el.first << ": ";
    for (Instruction *elel : el.second) {
      errs() << *elel << " ";
    }
    errs() << "\n";
  }
  getRepGraph(Logs, Map);
    
  /*int I = 0;
  for(auto It = Modules.begin(); It != Modules.end(); It++){
    InstrEntryMap_t IEM;

    if(It != Modules.end() - 1){
      updateInstEntryMap(It->get(), Logs[I], IEM);
      for(auto Entry : Logs[I].getEntries()){
	if(Entry.getKind() == EntryKind::Create){
	  errs() << Entry.toString() << "\n";
	}
      }
    }
    
    printModule(It->get(), IEM);
    I++;
  }*/
}
