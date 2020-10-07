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

Entry::Entry(EntryKind Kind, const string& LogStr): Kind(Kind){
  switch(Kind){
  case(EntryKind::Create):
  case(EntryKind::Remove):
    makeOneInstrEntry(LogStr);
    break;
  case(EntryKind::Move):
    makeMoveEntry(LogStr);
    break;
  case(EntryKind::Replace):
    makeReplaceEntry(LogStr);
    break;
  case(EntryKind::RepOperand):
    makeRepOperandEntry(LogStr);
    break;
  } 
}
    

void Entry::makeOneInstrEntry(const string& LogStr){
  //split the string to retrieve the id of the instruction and set it
  //the LogStr looks like TYPE i64 ID, so the ID will be at Tokens[2].
  vector<string> Tokens;
  boost::algorithm::split(Tokens, LogStr, boost::algorithm::is_any_of(" "));
  InstID1 = stoi(Tokens[2]);
}

void Entry::makeMoveEntry(const string& LogStr){
  //the LogStr looks like Moving i64 IDOld to i64 IDNew
  //IDOld is at Tokens[2], IDNew at Tokens[5]
  vector<string> Tokens;
  boost::algorithm::split(Tokens, LogStr, boost::algorithm::is_any_of(" "));
  InstID1 = stoi(Tokens[2]);
  InstID2 = stoi(Tokens[5]);
}

void Entry::makeReplaceEntry(const string& LogStr){
  //the LogStr either looks like:
  //Replacing i64 IDOld with i64 IDNew
  //Replacing i64 IDOld with Value type name
  //first set the id
  //then disambiguate the value and set either the oldID or the value
  vector<string> Tokens;
  boost::algorithm::split(Tokens, LogStr, boost::algorithm::is_any_of(" "));
  InstID1 = stoi(Tokens[2]);
  if(Tokens[4] == "i64"){
    InstID2 = stoi(Tokens[5]);
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

void Entry::makeRepOperandEntry(const string& LogStr){
  //the LogStr either looks like:
  //Replacing operand in i64 WHERE from i64 OLD with i64 NEW
  //Replacing operand in i64 WHERE from i64 OLD with value i64 blabla
  vector<string> Tokens;
  boost::algorithm::split(Tokens, LogStr, boost::algorithm::is_any_of(" "));
  InstID1 = stoi(Tokens[7]);
  Where = stoi(Tokens[4]);
  if(Tokens[9] == "i64"){
    InstID2 = stoi(Tokens[10]);
    ReplaceWithValue = false;
  }
  else if(Tokens[9] == "value"){
    ReplaceWithValue = true;
    Value = Tokens[11];
  }
  else{
    assert(false && "Replace with operand malformed");
  } 
}

string Entry::toString() const {
  switch(Kind){
  case(EntryKind::Create):
    return "Creating " + to_string(InstID1);
  case(EntryKind::Remove):
    return "Removing " + to_string(InstID1);
  case(EntryKind::Move):
    return "Moving " + to_string(InstID1) + " to " + to_string(InstID2);
  case(EntryKind::Replace):
    return "Replacing " + to_string(InstID1) + " " +
      (ReplaceWithValue ? "Value " + Value : to_string(InstID2));
  case(EntryKind::RepOperand):
    return "Replacing operand in " + to_string(Where) + " from " +
      to_string(InstID1) + " to " +
      (ReplaceWithValue ? "Value " + Value : to_string(InstID2));
  }
  assert(false && "Opsie\n");
}

  

Log::Log(const vector<string>& LogStrings, const string& Name): PassName(Name){
  for(auto LogStr : LogStrings){
    if(LogStr.rfind("Creating", 0) == 0){
      Entries.emplace_back(EntryKind::Create, LogStr);
    }
    else if(LogStr.rfind("Removing", 0) == 0){
      Entries.emplace_back(EntryKind::Remove, LogStr);
    }
    else if(LogStr.rfind("Moving", 0) == 0){
      Entries.emplace_back(EntryKind::Move, LogStr);
    }
    else if(LogStr.rfind("Replacing", 0) == 0){
      if(LogStr.rfind("operand", 10) == 10)
	Entries.emplace_back(EntryKind::RepOperand, LogStr);
      else
	Entries.emplace_back(EntryKind::Replace, LogStr);
    }
    else{
      assert(false && "Malformed Log Entry\n");
    }
  }
}


string Log::toString(){
  string Res = "";
  for(auto& Entry : Entries){
    Res += Entry.toString() + "\n";
  }
  return Res;
}
