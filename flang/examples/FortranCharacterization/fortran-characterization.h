/*
 * What do I want out of this?
 *
 * I want to be able to look at parse tree NODEZs in the same way as
 * FeatureList.cpp
 * - Is it possible to re-use parts of this?
 * - inherit from dump-parse-tree.h
 * - I will need to overwrite the Pre and Post functions though
 */

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
#include "flang/Semantics/semantics.h"
#include "flang/Semantics/symbol.h"
#include "flang/Semantics/tools.h"
#include "flang/Support/Fortran.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

using namespace Fortran::semantics;
using namespace Fortran::common;
using namespace Fortran::frontend;
using namespace Fortran::parser;
using namespace Fortran;

class FeatureCharacterization { // this is a visitor
public:
  explicit FeatureCharacterization(
      llvm::raw_ostream &out) //, SemanticsContext &context)
      : out_(out) /*, context_(context)*/ {}

  void checkMap(const char *key, bool addComma = true);
  void setMap(const char *key, bool val);
  std::string symbolKindString(const semantics::Symbol *sym);
  void dumpSymbol(const semantics::Symbol *sym);
  std::string dumpExactSymbolType(const Symbol *sym);
  bool IsCInteroperableObject(const semantics::Symbol &sym);
  bool IsLocalVariable(const semantics::Symbol &sym);

  template <typename T> bool Pre(const T &x) { return true; }

  template <typename T> void Post(const T &x) {}

  void checkAllFeatures();

  static std::vector<std::string> Fortran_intrinsic_modules;

  static std::unordered_set<std::string> used_modules;

  static std::unordered_set<std::string> computing_environment_intrinsics;

  static std::unordered_set<std::string> user_defined_operators;

  static std::unordered_set<std::string> user_defined_assignment;

  static std::unordered_set<std::string> dummy_arguments;

  static std::unordered_set<std::string> pure_value_dummy_arguments;

  static std::unordered_set<std::string> optional_dummy_arguments;

  static std::unordered_set<std::string> executable_constructs;

  static bool use_iso_Fortran_env;

  static bool is_in_c_binding_procedure;

  static bool is_in_pure_procedure;

  static bool is_in_module;

  static bool is_in_submodule;

  static bool is_in_function;

  static bool is_in_subroutine;

  static bool is_in_user_defined_operator_function;

  static bool is_in_user_defined_assignment_subroutine;

  static bool is_in_initialization;

  static bool is_in_type_declaration_stmt;

  // - Constants in ISO_FORTRAN_ENV (Fortran 2008)
  void Post(const parser::Name &);
  ///////////////////////////////
  // Fortran 95's New Features //
  ///////////////////////////////
  // - Forall statements
  void Post(const parser::ForallStmt &);
  // - Forall constructs
  void Post(const parser::ForallConstruct &);
  // - Enhancements to WHERE
  void Post(const parser::WhereBodyConstruct &);
  // - Initialization of pointers with NULL function
  void Post(const parser::NullInit &);
  // - Default initialization of derived types
  void Post(const parser::ComponentDecl &);
  // - Pure procedures
  // - Elemental procedures
  // - Impure elemental procedures
  // - Recursive and non-recursive procedures (Fortran 2018)
  void Post(const parser::PrefixSpec &);

  /////////////////////////////////
  // Fortran 2003's New Features //
  /////////////////////////////////
  static std::vector<std::string> Fortran2003_interop_c_procedures;
  static std::vector<std::string> Fortran2003_interop_c_intrinsictypes;
  // - Array constructor syntax
  bool Pre(const parser::Expr &);
  // - Specification and initialization expressions
  bool Pre(const parser::Initialization &init);
  void Post(const parser::Initialization &init);
  void Post(const parser::StructureConstructor &init);
  bool Pre(const parser::TypeDeclarationStmt &init);
  // - procedure pointers
  void Post(const parser::ProcedureDeclarationStmt &);
  // - parameterized derived types
  void Post(const parser::TypeParamDefStmt &);
  void Post(const parser::TypeParamSpec &);
  void Post(const parser::FinalProcedureStmt &);
  // - Procedures bound by name to a type (type-bound procedures)
  // void Post(const parser::TypeBoundProcBinding &node);
  void Post(const parser::Pass &);
  // - type extension and abstract types
  void Post(const parser::TypeAttrSpec &);
  // - Enumerations
  void Post(const parser::EnumDef &);
  void Post(const parser::AssociateConstruct &);
  // - Polymorphic Entities
  // - Interoperability of intrinsic types
  // - Interoperability with C pointers
  // - Interoperability of variables
  // - C descriptors (Fortran 2018)
  // - Attribute codes (Fortran 2018)
  // - The type CFI_dim_t (Fortran 2018)
  // - Assumed type (Fortran 2018)
  void checkDeclarationTypeSpec(const parser::DeclarationTypeSpec &, bool);
  void Post(const parser::DeclarationTypeSpec &);
  // - SELECT TYPE construct
  void Post(const parser::SelectTypeConstruct &);
  // - deferred bindings
  void Post(const parser::BindAttr &);
  // - Allocatable scalars
  // - Allocatable character length
  // - Interoperability of global data
  // - Pointer initialization with SAVE attribute (Fortran 2008)
  // - C descriptors (Fortran 2018)
  // - Attribute codes (Fortran 2018)
  // - The type CFI_dim_t (Fortran 2018)
  // - Assumed rank (Fortran 2018)
  // - Allocatable dummy arguments of intent out (Fortran 2018)
  // - Contiguous attribute for assumed-rank arrays (Fortran 2018)
  // - Automatic deallocation of allocatable arrays (Fortran 95)
  void Post(const parser::TypeDeclarationStmt &);
  // - Allocatable scalars
  // - Allocatable character length
  void Post(const parser::DataComponentDefStmt &);
  // - Allocatable character length
  // - Allocatable scalars
  void Post(const parser::AllocatableStmt &allocatableStmt);
  // - More control of access from a module
  void Post(const parser::AccessSpec &as);
  void Post(const parser::Protected &pt);
  // - Renaming operators on the USE staement
  // - Intrinsic modules
  void Post(const parser::UseStmt &us);
  // - Pointer assignment (rank remapping)
  // - Internal procedure as an actual argument
  // - Simply contiguous arrays rank remapping to rank>1 target (Fortran 2008)
  void Post(const parser::PointerAssignmentStmt &pas);
  // - import statement
  // - Control of host association
  void Post(const parser::ImportStmt &is);
  // - Access to the computing environment
  //   (Command line processing)
  // - Long integers (Fortran 2008)
  // - Storage size (Fortran 2008)
  // - Optional back argument added to maxloc and minloc (Fortran 2008)
  // - Initialization of pointers with NULL function
  // - New and enhanced intrinsic procedures
  //   TODO: handle SIGN() function.
  // - Compiler information in ISO_FORTRAN_ENV (Fortran 2008)
  // - Function for C sizeof (Fortran 2008)
  // - Assumed rank (Fortran 2018)
  // - Simplification of calls of the intrinsic cmplx (Fortran 2018)
  // - Changes to Intrinsics that access the computing environment (Fortran
  // 2018)
  // - New reduction intrinsic reduce (Fortran 2018)
  // - Intrinsic function coshape (Fortran 2018)
  // - Intrinsic subroutine random_init (Fortran 2018)
  // - Removal of the restriction on argument dim of many intrinsic functions
  // (Fortran 2018)
  // - Internal procedure as an actual argument
  // - Overriding a type-bound procedure
  // - Changes to procedures in the iso_c_binding module (Fortran 2018)
  // - Specification and initialization expressions
  void Post(const parser::Call &cs);
  // - Derived type I/O
  void Post(const parser::IoControlSpec &iocs);
  // - procedures bound to a type as operators
  // - The value attribute for an argument of a defined operation or assignment
  // (Fortran 2018)
  void Post(const parser::TypeBoundProcBinding &tbpb);
  // - The value attribute for an argument of a defined operation or assignment
  // (Fortran 2018)
  void Post(const parser::InterfaceBlock &ib);
  // - Interoperability of derived types
  void Post(const parser::TypeAttrSpec::BindC &);
  // - Interoperability of procedures
  void Post(const parser::ProcAttrSpec &);
  void Post(const parser::Suffix &);
  void Post(const parser::SubroutineStmt &);
  void Post(const parser::EndSubroutineStmt &);
  void Post(const parser::FunctionStmt &);
  void Post(const parser::EndFunctionStmt &);
  // - Interoperability of global data
  void Post(const parser::BindStmt &);

  /////////////////////////////////
  // Fortran 2008's New Features //
  /////////////////////////////////
  static std::vector<std::string> Fortran2008_iso_Fortran_env_constant_arrays;
  static std::vector<std::string> Fortran2008_iso_Fortran_env_constant_scalars;
  // - Submodules
  // - Save attribute for module and submodule data
  void Post(const parser::SubmoduleStmt &);
  // - Save attribute for module and submodule data
  void Post(const parser::EndSubmoduleStmt &);
  // - Coarrays
  // - do concurrent
  void Post(const parser::DoConstruct &);
  // - Contiguous attribute
  void Post(const parser::AttrSpec &);
  void Post(const parser::ComponentAttrSpec &);
  void Post(const parser::ContiguousStmt &);
  // - Simply contiguous arrays rank remapping to rank>1 target
  // - Maximum rank
  // - Allocatable components of recursive type
  // - Implied-shape array
  // - Pointer initialization with SAVE attribute
  // - Kind of a forall index
  void Post(const parser::ConcurrentHeader &);
  // - Allocating a polymorphic variable using MOLD
  void Post(const parser::AllocateStmt &allocateStmt);
  // - Type statement for intrinsic types
  // - Declaring type-bound procedures
  // - Extensions to value attribute
  // - Finding a unit when opening a file (newunit=u)
  void Post(const parser::ConnectSpec::Newunit &);
  // - The BLOCK construct
  // - Exit statement allowed in almost any construct
  void Post(const parser::BlockStmt &);
  // - Exit statement allowed in almost any construct
  void Post(const parser::EndBlockStmt &);
  // - Exit statement allowed in almost any construct
  void Post(const parser::IfThenStmt &);
  // - Exit statement allowed in almost any construct
  void Post(const parser::EndIfStmt &);
  // - Exit statement allowed in almost any construct
  void Post(const parser::AssociateStmt &);
  // - Exit statement allowed in almost any construct
  void Post(const parser::EndAssociateStmt &);
  // - Exit statement allowed in almost any construct
  void Post(const parser::WhereConstructStmt &);
  // - Exit statement allowed in almost any construct
  void Post(const parser::EndWhereStmt &);
  // - Exit statement allowed in almost any construct
  void Post(const parser::SelectCaseStmt &);
  // - Exit statement allowed in almost any construct
  void Post(const parser::SelectTypeStmt &);
  // - Exit statement allowed in almost any construct
  // - SELECT RANK (Fortran 2018)
  void Post(const parser::SelectRankStmt &);
  // - Exit statement allowed in almost any construct
  void Post(const parser::EndSelectStmt &);
  // - Exit statement allowed in almost any construct
  void Post(const parser::ExitStmt &);
  // - Generic resolution by procedureness
  // - Generic resolution by pointer vs. allocatable
  // - Rules for generic procedures (Fortran 2018)
  void Post(const parser::GenericSpec &);

  /////////////////////////////////
  // Fortran 2018's New Features //
  /////////////////////////////////
  // - Assumed-size arrays
  void Post(const parser::AssumedSizeSpec &);
  // - Assumed-size arrays
  void Post(const parser::ImpliedShapeSpec &);
  // - Implicit none enhancement
  void Post(const parser::ImplicitStmt &);
  // - Kind of the do variable in implied do
  void Post(const parser::AcImpliedDoControl &);
  // - Kind of the do variable in implied do
  void Post(const parser::DataImpliedDo &);
  // - Locality clauses in do concurrent
  void Post(const parser::LocalitySpec &);
  // - Removal of anomalies regarding pure procedures
  void Post(const parser::StopStmt &);
  // - Default accessibility for entities accessed from a module
  void Post(const parser::ModuleStmt &);
  // - Default accessibility for entities accessed from a module
  void Post(const parser::EndModuleStmt &);
  // - Default accessibility for entities accessed from a module
  void Post(const parser::AccessStmt &);
  // - Removal of anomalies regarding pure procedures
  // - Assignment to an allocatable array (Fortran 2003)
  void Post(const parser::AssignmentStmt &);
  // - Removal of anomalies regarding pure procedures
  void Post(const parser::ReadStmt &);
  // - Removal of anomalies regarding pure procedures
  void Post(const parser::WriteStmt &);

protected:
private:
  llvm::raw_ostream &out_;
  static std::unordered_map<const char *, bool> features;
};

class FeatureListAction : public PluginParseTreeAction {
  void executeAction() override {
    // Fortran::frontend::CompilerInstance &ci = this->getInstance();
    // auto &semanticsCtx{ci.getSemanticsContext()};

    FeatureCharacterization visitor{llvm::outs()}; //, semanticsCtx};
    auto &pt = getParsing().parseTree();
    Fortran::parser::Walk(pt, visitor);

    visitor.checkAllFeatures();
  }

  // don't need to override beginSourceFileAction because it already does
  // what we want
  bool beginSourceFileAction() override {
    return runPrescan() && runParse(/*emitMessages=*/true) &&
        runSemanticChecks();
  }
};

static Fortran::frontend::FrontendPluginRegistry::Add<FeatureListAction> X(
    "fortran-feature-list", "List program features");
