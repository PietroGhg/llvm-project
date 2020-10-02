#include "Log.h"
#include <cassert>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#define BOOST_NO_EXCEPTIONS
#include <boost/throw_exception.hpp>
//this is done to prevent a linking error
void boost::throw_exception(std::exception const & E){
//do nothing
  exit(-1);
}

OneInstrEntry::OneInstrEntry(const string& LogStr){
  //split the string to retrieve the id of the instruction and set it
  //the LogStr looks like TYPE i64 ID, so the ID will be at Tokens[2].
  vector<string> Tokens;
  boost::algorithm::split(Tokens, LogStr, boost::algorithm::is_any_of(" "));
  InstID = stoi(Tokens[2]);
}

MoveEntry::MoveEntry(const string& LogStr){
  //the LogStr looks like Moving i64 IDOld to i64 IDNew
  //IDOld is at Tokens[2], IDNew at Tokens[5]
  vector<string> Tokens;
  boost::algorithm::split(Tokens, LogStr, boost::algorithm::is_any_of(" "));
  InstIDOld = stoi(Tokens[2]);
  InstIDNew = stoi(Tokens[5]);
}

ReplaceEntry::ReplaceEntry(const string& LogStr){
  //the LogStr either looks like:
  //Replacing i64 IDOld with i64 IDNew
  //Replacing i64 IDOld with Value type name
  //first set the id
  //then disambiguate the value and set either the oldID or the value
  vector<string> Tokens;
  boost::algorithm::split(Tokens, LogStr, boost::algorithm::is_any_of(" "));
  InstIDOld = stoi(Tokens[2]);
  if(Tokens[4] == "i64"){
    InstIDNew = stoi(Tokens[5]);
    ReplaceWithValue = false;
  }
  else if(Tokens[4] == "value"){
    ReplaceWithValue = true;
    Value = Tokens[6];
  }
  else{
    assert(false && "Replace with value malformed");
  } 
}

  

Log::Log(const vector<string>& LogStrings, const string& Name): PassName(Name){
  for(auto LogStr : LogStrings){
    if(LogStr.rfind("Creating", 0) == 0){
      Entries.push_back(new CreateEntry(LogStr));
    }
    else if(LogStr.rfind("Removing", 0) == 0){
      Entries.push_back(new RemoveEntry(LogStr));
    }
    else if(LogStr.rfind("Moving", 0) == 0){
      Entries.push_back(new MoveEntry(LogStr));
    }
    else if(LogStr.rfind("Replacing", 0) == 0){
      Entries.push_back(new ReplaceEntry(LogStr));
    }
    else{
      assert(false && "Malformed Log Entry\n");
    }
  }
}

Log::~Log(){
  for(auto& Entry : Entries){
    free(Entry);
  }
}

string Log::toString(){
  string Res = "";
  for(auto& Entry : Entries){
    Res += Entry->toString() + "\n";
  }
  return Res;
}
