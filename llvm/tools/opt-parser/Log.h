#include <string>
#include <vector>
using namespace std;

/// Virtual class for an entry in the log
class Entry{
 public:
  virtual string toString() = 0;
};

/// Virtual class for entries with one instruction
class OneInstrEntry : public Entry{
 protected:
  unsigned long InstID;
 public:
  OneInstrEntry(const string& LogStr);
  unsigned long getInstID() { return InstID; }
  virtual string toString() override = 0;
};

class CreateEntry : public OneInstrEntry {
 public:
 CreateEntry(const string& LogStr): OneInstrEntry(LogStr){};
  string toString() override { return "Creating " + to_string(InstID); }
};

class RemoveEntry : public OneInstrEntry {
 public:
 RemoveEntry(const string& LogStr): OneInstrEntry(LogStr){};
  string toString() override  { return "Removing " + to_string(InstID); }
};

class MoveEntry : public Entry {
 private:
  unsigned int InstIDOld;
  unsigned int InstIDNew;
 public:
  MoveEntry(const string& LogStr);
  unsigned int getInstIDOld(){ return InstIDOld; }
  unsigned int getInstIDNew() { return InstIDNew; }
  string toString() override {
    return "Moving " + to_string(InstIDOld) + " to " + to_string(InstIDNew);
  }
};

class ReplaceEntry : public Entry {
 private:
  bool ReplaceWithValue; /// True if it's a replace with value
  unsigned int InstIDOld;
  unsigned int InstIDNew;
  string Value;
 public:
  ReplaceEntry(const string& LogStr);
  string toString() override {
    return "Replacing " + to_string(InstIDOld) + " " +
      (ReplaceWithValue ? "Value " + Value : to_string(InstIDNew));
  }
};

class Log{
 private:
  string PassName;
  vector<Entry*> Entries;
 public:
  Log(const vector<string>& LogStrings, const string& Name = "unknown");
  ~Log();
  vector<Entry*>& getEntries() { return Entries; }
  const string& getPassName() { return PassName; }
  string toString();
};
