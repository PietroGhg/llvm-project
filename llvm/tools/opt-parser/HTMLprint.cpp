#include "HTMLprint.h"
#include <algorithm>
#include <vector>


const char* CSS = R"V0G0N(
body {
  font-family: Arial, Helvetica, sans-serif;
}

.navbar {
  overflow: hidden;
  background-color: #333;
}

.navbar a {
  float: left;
  font-size: 16px;
  color: white;
  text-align: center;
  padding: 14px 16px;
  text-decoration: none;
}

.dropdown {
  float: left;
  overflow: hidden;
}

.dropdown .dropbtn {
  font-size: 16px;  
  border: none;
  outline: none;
  color: white;
  padding: 14px 16px;
  background-color: inherit;
  font-family: inherit;
  margin: 0;
}

.navbar a:hover, .dropdown:hover .dropbtn {
  background-color: red;
}

.dropdown-content {
  display: none;
  position: absolute;
  background-color: #f9f9f9;
  min-width: 160px;
  box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);
  z-index: 1;
}

.dropdown-content a {
  float: none;
  color: black;
  padding: 12px 16px;
  text-decoration: none;
  display: block;
  text-align: left;
}

.dropdown-content a:hover {
  background-color: #ddd;
}

.dropdown:hover .dropdown-content {
  display: block;
}

.box{
  width:100vw
  float:left;
  margin-right:30px;
}
.clear{
  clear:both;
})V0G0N";


/// Prints module to errs()
void printModule(Module* M, InstrEntryMap_t& InstrEntry){
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
string htmlBefore(const Log& Log,
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
string htmlAfter(const Log& Log,
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

void removeChar(string& Original, char C){
  Original.erase(std::remove(Original.begin(), Original.end(), C),
		 Original.end());
}
  

string encode(const string& Original){
  string Res = "";
  for(auto C : Original){
    if(C == ' ')
      Res += "%20";
    else
      Res += C;
  }
  return Res;
}

void printTransform(const Log& Log,
		    Module* Before,
		    Module* After,
		    const string& NavBar,
		    const string& OutputDir,
		    const string& Filename){
  string HTMLpre = htmlBefore(Log, Before);
  string HTMLafter = htmlAfter(Log, After);


  const char* Part1 = R"V0G0N(<html>
  <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <link rel="stylesheet" href="style.css"></link>
  </head>
  <body>)V0G0N";
  const char* Part2 = R"V0G0N(
    <div class="box">
      <iframe src=")V0G0N";
  const char* Part3 = R"V0G0N(" id="before" frameborder="1" scrolling="yes" width="48%" height="100%" align="left"> </iframe>
    </div>
    
    <div class="box">
      <iframe src=")V0G0N";
  const char* Part4 = R"V0G0N(" id="after" frameborder="1" scrolling="yes" width="48%" height="100%" align="top"></iframe>
    </div>

  </body>
</html>)V0G0N";

  string FilenameBefore = Filename + "_before.html";
  string FilenameAfter = Filename + "_after.html";
  string PathBefore = OutputDir + "/" + FilenameBefore;
  string PathAfter = OutputDir + "/" + FilenameAfter;
  ofstream FB(PathBefore);
  ofstream FA(PathAfter);
  ofstream F(OutputDir + "/" + Filename + ".html");
  F << Part1 << NavBar << Part2 <<
    encode(FilenameBefore) << Part3 << encode(FilenameAfter) << Part4;
  FB << HTMLpre;
  FA << HTMLafter;
  F.close();
  FA.close();
  FB.close();
}

void printModuleHTML(Module* M, InstrEntryMap_t& InstrEntry){
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

      
string generateNavBar(const vector<string>& Links){
  string Result;
  string HRefs = "";
  const char* Part1 = R"V0G0N(

<div class="navbar">
  <div class="dropdown">
    <button class="dropbtn">Transformations 
      <i class="fa fa-caret-down"></i>
    </button>
    <div class="dropdown-content">)V0G0N";

  const char * Part2 = R"V0G0N(
    </div>
  </div> 
</div>
)V0G0N";

  for(auto& El : Links)
    HRefs += "<a href=\"" + encode(El) + ".html\">" + El + "</a>\n";

  return Part1 + HRefs + Part2;  
}

void generateIndex(const string& OutputDir,
		   const string& NavBar){
  ofstream F(OutputDir + "/" + "index.html");
  F << R"V0G0N(
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="style.css"></link>
</head>
<body>
)V0G0N" << NavBar <<
R"V0G0N(
<h3>Transformations</h3>
<p>Hover over the "Transformations" link to see the dropdown menu.</p>

</body>
</html>
)V0G0N";
  F.close();
}

void printLogs(const vector<Log>& Logs,
	       const vector<std::unique_ptr<Module>>& Modules,
	       const string& OutputDir){
  vector<string> Links; //the links to the transformation pages
  
  //creates CSS file
  ofstream CSSfile(OutputDir + "/" + "style.css");
  CSSfile << CSS;
  CSSfile.close();

  //generate NavBar
  unsigned long I = 0;
  for(auto& Log : Logs){
    if(Log.getEntries().empty())
      continue;
    string Filename = to_string(I) + " " + Log.getPassName();
    removeChar(Filename, '/');
    Links.push_back(Filename);
    I++;
  }
  string NavBar = generateNavBar(Links);
    

  //generate transform's html files
  I = 0;
  for(auto& Log : Logs){
    if(Log.getEntries().empty())
      continue;
    
    string Filename = to_string(I)+ " " + Log.getPassName();
    removeChar(Filename, '/');
    printTransform(Log,
		   Modules[I].get(),
		   Modules[I+1].get(),
		   NavBar,
		   OutputDir,
		   Filename);
    I++;
  }

  generateIndex(OutputDir, NavBar);

}
