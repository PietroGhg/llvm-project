#pragma once
#include <string>
#include "InstrIDMap.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <filesystem>

void printModule(Module* M, InstrEntryMap_t& InstrEntry);
string htmlBefore(const Log& Log, Module* M);
string htmlAfter(const Log& Log, Module* M);
void printTransform(const Log& Log,
		    Module* Before,
		    Module* After,
		    const string& NavBar,
		    const string& OutputDir,
		    const string& Filename);
void printLogs(const vector<Log>& Logs,
	       const vector<std::unique_ptr<Module>>& Modules,
	       const string& OutputDir);
void printModuleHTML(Module* M, InstrEntryMap_t& InstrEntry);
