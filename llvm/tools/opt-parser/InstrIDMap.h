#pragma once
#include <map>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "Log.h"

using namespace std;
using namespace llvm;

using InstrIDMap_t = map<Instruction*, unsigned long>;
using IDInstrMap_t = map<unsigned long, vector<Instruction*>>;
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
	  Res[I.getIDInt()].push_back(&I);
	}
      }
    }
  }
  return Res;
}

inline void updateIDInstrMap(IDInstrMap_t& Map, Module* M){
  for(auto& F : *M){
    for(auto& BB : F){
      for(auto& I : BB){
	if(I.hasID()){
	  Map[I.getIDInt()].push_back(&I);
	}
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
      auto InstrsBefore = IDInstrBefore[Entry.getInstID1()];
      for(Instruction *IBefore : InstrsBefore)
        Map[IBefore].push_back(Entry);	
  }
}
 

inline void printModule(Module* M, InstrEntryMap_t& InstrEntry){
  const string Green = "\033[32m";
  const string Red = "\033[31m";
  const string Reset = "\033[0m";
  string Color;
  
  auto Map = getInstrIDmap(M);
  for(auto& F : *M){
    errs() << F.getName() << "\n";
    for(auto& BB : F){
      errs() << "\t" << BB.getName() << "\n";
      for(auto& I : BB){
	Color = Reset;
	for(auto& Entry : InstrEntry[&I]){
	  if(Entry.getKind() == EntryKind::Create)
	    Color = Green;
	  else if(Entry.getKind() == EntryKind::Remove)
	    Color = Red;
	}
	errs() << "\t\t" << Color << I << Reset << " " << Map[&I] << " ";
	for(auto& Entry : InstrEntry[&I]){
	  if(Entry.getKind() != EntryKind::Create &&
	     Entry.getKind() != EntryKind::Remove)
	    errs() << Entry.toString() << " ";
	}
	errs() << "\n";
      }
    }
  }
}

/// Returns the HTML print of a module before the logged transformation in applied
/// Mark in red removed instructions
inline string htmlBefore(const Log& Log,
			 Module* M){
  const string Red = "<span style=\"color:red;margin-bottom:30px\">";
  const string P = "<span>";
  const string Reset = "</span>";
  string Color;
  string S;
  raw_string_ostream Out(S);

  InstrEntryMap_t InstrEntry;
  updateInstEntryMap(M, Log, InstrEntry);
  Out << "<html><head></head><body><pre>";

  auto Map = getInstrIDmap(M);
  if(M){
    for(auto& F : *M){
      Out << F.getName() << "<br>\n";
      for(auto& BB : F){
	Out << "&#9;" << BB.getName() << "<br>\n";
	for(auto& I : BB){
	  Color = P;
	  for(auto& Entry : InstrEntry[&I]){
	    if(Entry.getKind() == EntryKind::Remove)
	      Color = Red;
	  }
	  Out  << Color << "&#9; &#9;" << I << " " << Map[&I] << " ";
	  for(auto& Entry : InstrEntry[&I]){
	    if(Entry.getKind() != EntryKind::Create &&
	       Entry.getKind() != EntryKind::Remove)
	      Out << Entry.toString() << " ";
	  }
	  Out << Reset << "\n";
	}
      }
    }
  }

   
  Out << "</pre></body></html>";
  return Out.str();  
}

/// Returns the HTML print of a module before the logged transformation in applied
/// Mark in red removed instructions
inline string htmlAfter(const Log& Log,
			 Module* M){
  const string Green = "<span style=\"color:green;margin-bottom:30px\">";
  const string P = "<span>";
  const string Reset = "</span>";
  string Color;
  string S;
  raw_string_ostream Out(S);

  InstrEntryMap_t InstrEntry;
  updateInstEntryMap(M, Log, InstrEntry);
  Out << "<html><head></head><body><pre>";
  if(M){
    auto Map = getInstrIDmap(M);
    for(auto& F : *M){
      Out << F.getName() << "<br>\n";
      for(auto& BB : F){
	Out << "&#9;" << BB.getName() << "<br>\n";
	for(auto& I : BB){
	  Color = P;
	  for(auto& Entry : InstrEntry[&I]){
	    if(Entry.getKind() == EntryKind::Create)
	      Color = Green;
	  }
	  Out  << Color << "&#9; &#9;" << I << " " << Map[&I] << " ";
	  for(auto& Entry : InstrEntry[&I]){
	    if(Entry.getKind() != EntryKind::Create &&
	       Entry.getKind() != EntryKind::Remove)
	      Out << Entry.toString() << " ";
	  }
	  Out << Reset << "\n";
	}
      }
    }
  }

   
  Out << "</pre></body></html>";
  return Out.str();  
}

inline void printTransform(const Log& Log,
			   Module* Before,
			   Module* After,
			   string Filename){
  string HTMLpre = htmlBefore(Log, Before);
  string HTMLafter = htmlAfter(Log, After);


  const char* Part1 = R"V0G0N(<html>
  <head>
    <style>
      .box{
	  width:100vw
	  float:left;
	  margin-right:30px;
      }
      .clear{
	  clear:both;
      }
      </style>
  </head>
  <body>
    <div class="box">
      <iframe src=")V0G0N";
  const char* Part2 = R"V0G0N(" id="before" frameborder="1" scrolling="yes" width="48%" height="100%" align="left"> </iframe>
    </div>
    
    <div class="box">
      <iframe src=")V0G0N";
  const char* Part3 = R"V0G0N(" id="after" frameborder="1" scrolling="yes" width="48%" height="100%" align="top"></iframe>
    </div>

  </body>
</html>)V0G0N";
  string PathBefore = Filename+"_before.html";
  string PathAfter = Filename+"_after.html";
  ofstream FB(PathBefore);
  ofstream FA(PathAfter);
  ofstream F(Filename + ".html");
  F << Part1 << PathBefore << Part2 << PathAfter << Part3;
  FB << HTMLpre;
  FA << HTMLafter;
  F.close();
  FA.close();
  FB.close();
}

inline void printModuleHTML(Module* M, InstrEntryMap_t& InstrEntry){
  const string Green = "<span style=\"color:green;margin-bottom:30px\">";
  const string Red = "<span style=\"color:red;margin-bottom:30px\">";
  const string P = "<span>";
  const string Reset = "</span>";
  string Color;
  string S;
  raw_string_ostream Out(S);
  Out << "<html><head></head><pre>";
  
  auto Map = getInstrIDmap(M);
  for(auto& F : *M){
    Out << F.getName() << "<br>\n";
    for(auto& BB : F){
      Out << "&#9;" << BB.getName() << "<br>\n";
      for(auto& I : BB){
	Color = P;
	for(auto& Entry : InstrEntry[&I]){
	  if(Entry.getKind() == EntryKind::Create)
	    Color = Green;
	  else if(Entry.getKind() == EntryKind::Remove)
	    Color = Red;
	}
	Out  << Color << "&#9; &#9;" << I << " " << Map[&I] << " ";
	for(auto& Entry : InstrEntry[&I]){
	  if(Entry.getKind() != EntryKind::Create &&
	     Entry.getKind() != EntryKind::Remove)
	    Out << Entry.toString() << " ";
	}
	Out << Reset << "\n";
      }
    }
  }
  Out << "</pre></html>";
  errs() << Out.str();
}


 


      


