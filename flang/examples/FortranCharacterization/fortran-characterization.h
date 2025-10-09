/*
 * What do I want out of this?
 *
 * I want to be able to look at parse tree NODEZs in the same way as
 * FeatureList.cpp
 * - Is it possible to re-use parts of this?
 * - inherit from dump-parse-tree.h
 * - I will need to overwrite the Pre and Post functions though
 */

//#include "check-declarations.cpp"
//#include "check-declarations.h"
//////#include "check-allocate-anthony.h" // TODO: make cleaner
//////#include "check-allocate.h"
//////#include "check.h"
//#include "pointer-assignment-anthony.h" // TODO: make cleaner
#include "flang/Support/Fortran.h"
#include "flang/Common/idioms.h"
#include "flang/Common/indirection.h"
#include "flang/Frontend/CompilerInstance.h"
#include "flang/Frontend/FrontendActions.h"
#include "flang/Frontend/FrontendPluginRegistry.h"
#include "flang/Parser/format-specification.h"
#include "flang/Parser/parse-tree-visitor.h"
#include "flang/Parser/parse-tree.h"
#include "flang/Parser/parsing.h"
#include "flang/Parser/tools.h"
#include "flang/Parser/unparse.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
#include <type_traits>
#include <unordered_map>

using namespace Fortran::semantics;
using namespace Fortran::common;
using namespace Fortran::frontend;
using namespace Fortran::parser;
using namespace Fortran;

//////namespace Fortran::semantics {
//////class CheckHelperAnthony : public CheckHelper {
//////public:
//////  CheckHelperAnthony(SemanticsContext &c) : CheckHelper(c){};
//////
//////private:
//////};
//////} // namespace Fortran::semantics

class FeatureCharacterization { // this is a visitor
public:
  explicit FeatureCharacterization(
      llvm::raw_ostream &out) //, SemanticsContext &context)
      : out_(out) /*, context_(context)*/ {}

  void checkMap(const char *key, bool addComma = true);
  void setMap(const char *key, bool val);

  template <typename T> bool Pre(const T &x) { return true; }

  template <typename T> void Post(const T &x) {}

  void checkAllFeatures();

  // feature stuff
  void Post(const parser::Pass &);
  void Post(const parser::DoConstruct &node);
  void Post(const parser::FinalProcedureStmt &);
  void Post(const parser::AssociateConstruct &);
  // R1153 select-type-construct ->
  //           select-type stmt [type-guard-stmt block]...end-select-type-stmt
  void Post(const parser::SelectTypeConstruct &);
  // - parameterized derived types
  void Post(const parser::TypeParamDefStmt &);
  void Post(const parser::TypeParamSpec &);
  // - procedure pointers
  void Post(const parser::ProcedureDeclarationStmt &);
  // - type extension and abstract types
  void Post(const parser::TypeAttrSpec &);
  // - deferred bindings
  void Post(const parser::BindAttr &);
  // - procedures bound to a type as operators
  void Post(const parser::TypeBoundGenericStmt &);
  // - Enumerations
  void Post(const parser::EnumDef &);
  // - Polymorphic Entities
  void Post(const parser::DeclarationTypeSpec &);
  // - Allocatable scalar and
  // - allocate and initialize with source keyword
  void Post(const parser::TypeDeclarationStmt &);
  void Post(const parser::AllocateStmt &allocateStmt);
  /////// account for Allocatable stmt hopefully
  void Post(const parser::AllocatableStmt &allocatableStmt);
  // - import statement
  void Post(const parser::ImportStmt &is);
  // - Renaming operators on the USE staement
  void Post(const parser::UseStmt &us);
  // - Pointer assignment (rank remapping)
  void Post(const parser::PointerAssignmentStmt &us);
  // - Access to the computing environment
  //   (Command line processing)
  void Post(const parser::Call &cs);
  // - Derived type I/O
  void Post(const parser::IoControlSpec &iocs);
  void Post(const parser::TypeBoundProcBinding &tbpb);
  void Post(const parser::InterfaceStmt &is);
  // - Interoperability of intrinsic types
  void Post(const parser::Name &name);
  // - Interoperability of derived types
  void Post(const parser::TypeAttrSpec::BindC &);
  void Post(const parser::LanguageBindingSpec &);

protected:
private:
  llvm::raw_ostream &out_;
  // const AnalyzedObjectsAsFortran *const asFortran_;
  static std::unordered_map<const char *, bool> features;
  // SemanticsContext &context_;
};

/*static void checkOverride(CheckHelperAnthony &ch) {
  if (ch.getOverridePresent())
    llvm::outs() << "TRUE: ";
  else
    llvm::outs() << "FALSE: ";
  llvm::outs() << "Overriding a type-bound procedure\n";
}

static void checkTypeBoundProc(CheckHelperAnthony &ch) {
  if (ch.getTypeBoundProcPresent())
    llvm::outs() << "TRUE: ";
  else
    llvm::outs() << "FALSE: ";
  llvm::outs()
      << "Procedures bound by a name to a type (type-bound procedures)\n";
}*/

class FeatureListAction : public PluginParseTreeAction {
  void executeAction() override {
    // Fortran::frontend::CompilerInstance &ci = this->getInstance();
    // auto &semanticsCtx{ci.getSemanticsContext()};

    FeatureCharacterization visitor{llvm::outs()}; //, semanticsCtx};
    auto &pt = getParsing().parseTree();
    Fortran::parser::Walk(pt, visitor);

    // Fortran::semantics::CheckHelperAnthony ch{semanticsCtx};
    //  initialize ch with checking to see if stuff is present
    //  checks for
    //  - override
    //  - type bound proc

    // remove and discuss
    /*auto &scope = semanticsCtx.globalScope();
    ch.Check(scope);

    // checkOverride(ch);
    visitor.setMap(
        "Overriding a type-bound procedure", ch.getOverridePresent());
    // checkTypeBoundProc(ch);
    visitor.setMap(
        "Procedures bound by a name to a type (type-bound procedures)",
        ch.getTypeBoundProcPresent());*/

    visitor.checkAllFeatures();

    /* visitor.setMap(
         "Interoperability with C pointers", ch.getCPtrInteropPresent());
    visitor.checkMap("Interoperability with C pointers");*/

    /*for (auto &pair : scope) {
      llvm::outs() << pair.first << "\nYOOOO\n";
      //if (!ch.findTypeBoundProcOverrides(*pair.second))
      //  llvm::outs() << "FALSE: Overriding a type-bound procedure\n";
      for (const Scope &child : scope.children()) {

      }

    }*/

    // Fortran::semantics::AllocateChecker ac{semanticsCtx};

    // for all features in the map:
    //   print key: value
  }

  // don't need to override beginSourceFileAction because it already does
  // what we want
  bool beginSourceFileAction() override {
    return runPrescan() &&
        runParse(/*emitMessages=*/true); // &&
                                         // runSemanticChecks();
  }
};

static Fortran::frontend::FrontendPluginRegistry::Add<FeatureListAction> X(
    "anthony-feature-list", "List program features");
