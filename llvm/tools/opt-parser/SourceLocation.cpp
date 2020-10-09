#include "SourceLocation.h"
#include "llvm/IR/DebugInfoMetadata.h"

SourceLocation::SourceLocation(unsigned Line,
			       unsigned Column,
			       const std::string& File):
  Line(Line), Column(Column), File(File){}

SourceLocation::SourceLocation(llvm::DILocation* Loc){
  assert(Loc && "Location must not be null");
  llvm::DILocalScope *SubProg = Loc->getScope();
  llvm::DIFile *DIFile = SubProg->getFile();
  llvm::StringRef Name = DIFile->getFilename();
  llvm::StringRef Dir = DIFile->getDirectory();
  std::string FullName = Dir.str() + "/" + Name.str();
  Line = Loc->getLine();
  Column = Loc->getColumn();
  File = FullName;
}

bool SourceLocation::operator==(const SourceLocation& Other) const {
  return Line == Other.getLine() &&
    Column == Other.getColumn() &&
    File == Other.getFile();
}

bool SourceLocation::operator<(const SourceLocation& Other) const {
  if(File < Other.getFile())
    return true;
  if(Line < Other.getLine())
    return true;
  if(Column < Other.getColumn())
    return true;
  return false;
}

std::string SourceLocation::toString() const {
  return "Line: " + std::to_string(Line) + " Col: " +
    std::to_string(Column) + " File: " + File;
}
