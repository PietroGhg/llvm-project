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

  auto G = getRepGraph(Logs, Map);
  printRG(G);
  propagateLocations(G);
  errs() << "\n";
  printRG(G);
    
  int I = 0;
  for(auto It = Modules.begin(); It != Modules.end(); It++){
    InstrEntryMap_t IEM;

    if(It != Modules.end() - 1){
      updateInstEntryMap(It->get(), Logs[I], IEM);
      if(I-1 >= 0)
	updateInstEntryMap(It->get(), Logs[I-1], IEM);
    }
    else{
      if(I-1 >= 0)
	updateInstEntryMap(It->get(), Logs[I-1], IEM);
    }
      
    
    printModule(It->get(), IEM);
    errs() << "\n\n";
    I++;
  }

}
