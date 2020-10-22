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
#include "llvm/Transforms/IPO/Attributor.h"
#include "Log.h"

using namespace llvm;
/// Type for the log of a single optimization pass
using OptLog = std::vector<std::string>;

enum ParsingState{
  Running,
  SkipToNextRunning,
  InLog,
  InModule
};

inline bool isFromLog(const std::string& Line){
  std::vector<std::string> PossibleStarts {"Creating",
      "Removing", "Moving", "Replacing"};
  for(auto& Start : PossibleStarts){
    if(Line.rfind(Start, 0) == 0)
      return true;
  }
  return false;
}

inline bool isRunning(const std::string& Line){
  return Line.rfind("Running:", 0) == 0;
}

inline bool isModuleStart(const std::string& Line){
  return Line.rfind("; ModuleID",0) == 0;
}

inline std::unique_ptr<Module>
generateModule(std::string ModuleString, 
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
  //bool InModule = true; //a log starts with a module
  //bool ModuleHeader = false;
  int CurrentLog = 0;
  std::string Line;
  std::string LastRunningLine;
  std::string ModuleString;
  std::vector<OptLog> Logs;
  ParsingState State = ParsingState::InModule;
  
  std::getline(F,Line); //discard first line
  while(std::getline(F, Line)){
    switch(State){
    case(ParsingState::Running):
      if(isRunning(Line)){
	LastRunningLine = Line;
      }
      else if(isFromLog(Line)){
	Logs.push_back(OptLog());
	Logs[CurrentLog].push_back(LastRunningLine);
	Logs[CurrentLog].push_back(Line);
	State = ParsingState::InLog;
      }
      else if(isModuleStart(Line)){
	//the transformation made no changes, go to next Running entry
        State = ParsingState::SkipToNextRunning;
      }
      else{
	//signal error
	assert(false && "Error while parsing running");
      }
      break;
    case(ParsingState::InLog):
      if(isFromLog(Line)){
	Logs[CurrentLog].push_back(Line);
      }
      else if(isModuleStart(Line)){
	ModuleString += Line;
	CurrentLog++;
	State = ParsingState::InModule;	
      }
      else{
	assert(false && "Error while parsing Log");
      }  
      break;
    case(ParsingState::InModule):
      if(isRunning(Line)){
	Modules.push_back(generateModule(ModuleString, Ctx));
	ModuleString = "";    
	LastRunningLine = Line;
	State = ParsingState::Running;
      }
      else{
	ModuleString += "\n" + Line;
      }	
      break;
    case(ParsingState::SkipToNextRunning):
      if(isRunning(Line)){
	LastRunningLine = Line;
	State = ParsingState::Running;
      }
      break;
    }//end switch
  }

  //generate last module
  if(State == ParsingState::InModule)
    Modules.push_back(generateModule(ModuleString, Ctx));

  //generates Log objects and fill the vector with them
  for(auto LogStr : Logs){
    TrueLog.emplace_back(LogStr);
  }
}
