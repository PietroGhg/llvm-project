#include <fstream>
#include <string>
#include <vector>
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"

using namespace llvm;
/// Type for the log of a single optimization pass
using OptLog = std::vector<std::string>;

inline bool isFromLog(const std::string& Line){
  std::vector<std::string> PossibleStarts {"Creating",
      "Removing", "Moving", "Replacing"};
  for(auto& Start : PossibleStarts){
    if(Line.rfind(Start, 0) == 0)
      return true;
  }
  return false;
}

inline std::unique_ptr<Module>
generateModule(const std::string& ModuleString,
	       LLVMContext& Ctx){
  SMDiagnostic Err;
  auto Buffer = MemoryBuffer::getMemBuffer(ModuleString);
  return parseIR(Buffer->getMemBufferRef(), Err, Ctx);
}
  

inline void parseLog(std::fstream& F, LLVMContext& Ctx){
  bool InModule = true; //a log starts with a module
  int CurrentLog = 0;
  std::string Line;
  std::string ModuleString;
  std::vector<OptLog> Logs;
  Logs.push_back(OptLog());
  std::vector<std::unique_ptr<Module>> Modules;

  while(std::getline(F, Line)){
    if(InModule && isFromLog(Line)){
      Modules.push_back(generateModule(ModuleString, Ctx));
      ModuleString = "";
      InModule = false;
      Logs[CurrentLog].push_back(Line);
    }
    else if(InModule && !isFromLog(Line)){
      ModuleString += "\n" + Line;
    }
    else if(!InModule && isFromLog(Line)){
      Logs[CurrentLog].push_back(Line);
    }
    else if(!InModule && !isFromLog(Line)){
      CurrentLog++;
      Logs.push_back(OptLog());
      ModuleString += "\n" + Line;
      InModule = true;
    }
  }

  errs() << "there are " << Modules.size() << " modules\n";
  for(auto& M : Modules) {
    errs() << *(M.get());
    errs() << "\n";
  }

  for(auto&& Log : Logs){
    errs() << "---------\n";
    for(auto& Entry : Log)
      errs() << Entry << "\n";
  }
}
