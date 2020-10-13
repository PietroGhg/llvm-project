#pragma once
#include <cassert>
#include <memory>
#include <string>
#include <vector>
using namespace std;

/// Virtual class for an entry in the log
enum EntryKind{
  Create,
  Remove,
  Move,
  Replace,
  RepOperand
};
  

class Entry{
private:
  EntryKind Kind;
  unsigned long Where;
  unsigned long InstID1;
  unsigned long InstID2;
  string Value;
  bool ReplaceWithValue;
  void makeOneInstrEntry(const string& LogStr);
  void makeMoveEntry(const string& LogStr);
  void makeReplaceEntry(const string& LogStr);
  void makeRepOperandEntry(const string& LogStr);
public:
  Entry(EntryKind Kind, const string& LogStr);
  string toString() const;
  EntryKind getKind() const { return Kind; }
  unsigned long getWhere() const {
    assert(Kind == EntryKind::RepOperand);
    return Where;
  }
  unsigned long getInstID1() const {    return InstID1; }
  unsigned long getInstID2() const {
    assert(Kind == EntryKind::Move ||
	   (Kind == EntryKind::RepOperand && !ReplaceWithValue) ||
	   (Kind == EntryKind::Replace && !ReplaceWithValue));
    return InstID2;
  }
  bool isReplaceWithValue() const { return ReplaceWithValue; }
  const string& getValue() const {
    assert(ReplaceWithValue);
    return Value;
  }
};


class Log{
 private:
  string PassName;
  vector<Entry> Entries;
 public:
  Log(const vector<string>& LogStrings, const string& Name = "unknown");
  const vector<Entry>& getEntries() const { return Entries; }
  vector<Entry> getEntries(const EntryKind Kind) const;
  const string& getPassName() { return PassName; }
  string toString();
};
