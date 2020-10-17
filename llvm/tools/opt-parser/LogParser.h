#pragma once
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "Log.h"

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
  
inline std::string reachNextLog(std::fstream& F){
  std::string Line;
  while(std::getline(F, Line)){
    if(isFromLog(Line))
      break;
  }
  return Line;
}
 
inline void parseLog(std::fstream& F,
		     LLVMContext& Ctx,
		     std::vector<std::unique_ptr<llvm::Module>>& Modules, //out
		     std::vector<Log>& TrueLog /*out*/){ 
  bool InModule = true; //a log starts with a module
  bool ModuleHeader = false;
  int CurrentLog = 0;
  std::string Line;
  std::string ModuleString;
  std::vector<OptLog> Logs;

  while(std::getline(F, Line)){
    if(ModuleHeader && Line.rfind("; ModuleID",0) == 0)
      Line = reachNextLog(F);
    
    if(InModule && isFromLog(Line)){
      //if in module, find line from log, the module has ended
      //generare module given the string and push it in the vector
      Modules.push_back(generateModule(ModuleString, Ctx)); 
      ModuleString = ""; //reset module string
      InModule = false; //no more in module
      ModuleHeader = true;
      Logs.push_back(OptLog()); //push back a new vector of strings for the log
      Logs[CurrentLog].push_back(Line); //push back the current line in the log
    }
    else if(InModule && !isFromLog(Line)){
      //line of a module, just add it the the whole module string
      ModuleHeader = true;
      ModuleString += "\n" + Line;
    }
    else if(!InModule && isFromLog(Line)){
      //log line, just push in into the log
      Logs[CurrentLog].push_back(Line);
    }
    else if(!InModule && !isFromLog(Line)){
      //while reading the log, find a line not from the log
      //assume it's from a module, start adding to the module string
      CurrentLog++;
      ModuleString += "\n" + Line;
      InModule = true;
    }
  }
  //generate last module
  auto M = generateModule(ModuleString, Ctx);
  if(!M)
    errs() << "RIP\n";
  Modules.push_back(generateModule(ModuleString, Ctx));

  //generates Log objects and fill che vector with them
  for(auto LogStr : Logs){
    TrueLog.emplace_back(LogStr);
  }
}
