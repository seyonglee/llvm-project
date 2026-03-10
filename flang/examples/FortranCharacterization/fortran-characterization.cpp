// TODO: create a template where we check for a given feature in the map by
// providing const char* argument instead of creating a new function every time

// attributes of interest
#include "fortran-characterization.h"
#include "flang/Common/indirection.h"
#include "flang/Parser/parse-tree.h"
#include <iostream>
#include <unordered_set>
#include <variant>
#include <vector>

// Convert inputString to lowercases and store in a variable lName
#define CONVERT2LOWERCASE(inputString, lName) \
  auto lName{inputString}; \
  std::transform(lName.begin(), lName.end(), lName.begin(), \
      [](unsigned char c) { return std::tolower(c); })

std::unordered_map<const char *, bool> FeatureCharacterization::features{
    /* Fortran 95's New Features */
    {"Forall statements", false}, {"Forall constructs", false},
    {"Enhancements to WHERE", false},
    {"Initialization of pointers with NULL function", false},
    {"Default initialization of derived types", false},
    {"Pure procedures", false}, {"Elemental procedures", false},
    {"Automatic deallocation of allocatable arrays", false},
    {"New and enhanced intrinsic procedures", false},
    /* Fortran 2003's New Features */
    {"Procedure pointers", false}, {"Parameterized derived types", false},
    {"Finalization", false},
    {"Procedures bound by name to a type (type-bound procedures)", false},
    {"The PASS attribute", false},
    {"Procedures bound to a type as operators", false},
    {"Type extension", false}, {"Overriding a type-bound procedure", false},
    {"Enumerations", false}, {"ASSOCIATE construct", false},
    {"Polymorphic entities", false}, {"SELECT TYPE construct", false},
    {"Deferred bindings and abstract types", false},
    {"Structure constructors", false},
    {"The allocate statement (allocate with SOURCE)", false},
    {"Allocatable scalars", false}, {"Allocatable character length", false},
    {"Assignment to an allocatable array", false},
    {"Transferring an allocation", false},
    {"More control of access from a module", false},
    {"Renaming operators on the USE statement", false},
    {"Pointer assignment (rank remapping)", false}, {"Pointer INTENT", false},
    {"VOLATILE attribute", false}, {"The IMPORT statement", false},
    {"Intrinsic modules", false},
    {"Access to the computing environment (Command line processing)", false},
    {"Support for international character sets", false},
    {"Binary, octal and hex constants", false},
    {"Lengths of names and statements", false},
    {"Array constructor syntax", false},
    {"Specification and initialization expressions", false},
    {"Complex constants", false}, {"Changes to intrinsic functions", false},
    {"Controlling IEEE underflow", false}, {"Another IEEE class value", false},
    {"Derived type I/O", false}, {"Asynchronous I/O", false},
    {"FLUSH statement", false}, {"IOMSG= spcifier", false},
    {"Stream access I/O", false}, {"ROUND= spcifier", false},
    {"DECIMAL= spcifier", false}, {"SIGN= spcifier", false},
    {"Kind type parameters of integer specifiers", false},
    {"Recursive I/O", false},
    {"Intrinsic function for newline character", false},
    {"I/O of IEEE exceptional values", false},
    {"Comma after a P edit descriptor", false},
    {"Interoperability of intrinsic types", false},
    {"Interoperability with C pointers", false},
    {"Interoperability of derived types", false},
    {"Interoperability of variables", false},
    {"Interoperability of procedures", false},
    {"Interoperability of global data", false},
    /* Fortran 2008's New Features */
    {"Submodules", false}, {"Coarrays", false}, {"do concurrent", false},
    {"Contiguous attribute", false},
    {"Simply contiguous arrays rank remapping to rank>1 target", false},
    {"Maximum rank", false}, {"Long integers", false},
    {"Allocatable components of recursive type", false},
    {"Implied-shape array", false},
    {"Pointer initialization with SAVE attribute", false},
    {"Kind of a forall index", false},
    {"Type statement for intrinsic types", false},
    {"Declaring type-bound procedures", false},
    {"Extensions to value attribute", false},
    {"Omitting an allocatable component in a structure constructor", false},
    {"Multiple allocations with source=", false},
    {"Allocating a polymorphic variable using MOLD", false},
    {"Copying bounds of source array in ALLOCATE", false},
    {"Polymorphic assignment", false},
    {"Accessing real and imaginary parts", false}, {"Pointer functions", false},
    {"Elemental dummy argument restrictions lifted", false},
    {"Finding a unit when opening a file (newunit=u)", false},
    {"g0 edit descriptor", false}, {"Unlimited format item", false},
    {"Recursive I/O for an external unit", false},
    {"The BLOCK construct", false},
    {"Exit statement allowed in almost any construct", false},
    {"STOP code", false}, {"Bit sequence comparison", false},
    {"Combined shifting", false}, {"Counting bits", false},
    {"Masking bits", false}, {"Shifting bits", false}, {"Merging bits", false},
    {"Bit transformational functions", false}, {"Storage size", false},
    {"Optional argument radix added to selected real kind", false},
    {"Extensions to trigonometric and hyperbolic intrinsic functions", false},
    {"Bessel functions", false}, {"Error and gamma functions", false},
    {"Euclidean vector norms", false}, {"Parity", false},
    {"Execute command line", false},
    {"Optional back argument added to maxloc and minloc", false},
    {"Find location in an array", false}, {"String comparison", false},
    {"Constants in ISO_FORTRAN_ENV", false},
    {"Compiler information in ISO_FORTRAN_ENV", false},
    {"Function for C sizeof", false},
    {"Optional argument for ieee_selected_real_kind", false},
    {"Save attribute for module and submodule data", false},
    {"Empty contains part", false},
    {"Form of the end statement for an internal or module procedure", false},
    {"Internal procedure as an actual argument", false},
    {"Null pointer or unallocated allocatable as an absent dummy argument",
        false},
    {"Non-pointer actual for pointer dummy argument", false},
    {"Impure elemental procedures", false},
    {"Generic resolution by procedureness", false},
    {"Generic resolution by pointer vs. allocatable", false},
    /* Fortran 2018's New Features */
    {"C descriptors", false}, {"Attribute codes", false},
    {"The type CFI_dim_t", false}, {"Assumed rank", false},
    {"SELECT RANK", false}, {"Assumed-size arrays", false},
    {"Assumed type", false},
    {"Contiguous attribute for assumed-rank arrays", false},
    {"Default accessibility for entities accessed from a module", false},
    {"Implicit none enhancement", false},
    {"Changes to Intrinsics that access the computing environment", false},
    {"New reduction intrinsic reduce", false},
    {"Intrinsic function coshape", false},
    {"Intrinsic subroutine random_init", false},
    {"Kind of the do variable in implied do", false},
    {"Locality clauses in do concurrent", false},
    {"Control of host association", false},
    {"The value attribute for an argument of a defined operation or assignment",
        false},
    {"Removal of anomalies regarding pure procedures", false},
    {"Simplification of calls of the intrinsic cmplx", false},
    {"Removal of the restriction on argument dim of many intrinsic functions",
        false},
    {"Recursive and non-recursive procedures", false}
    /* Add other Fortran 2018 Features */
};

std::vector<std::string> FeatureCharacterization::Fortran_intrinsic_modules{
    "iso_fortran_env", "ieee_arithmetic", "ieee_exceptions", "ieee_features",
    "iso_c_binding", "iso_varying_string", "ieee_control_type",
    "ieee_status_type"};

std::unordered_set<std::string> FeatureCharacterization::used_modules;

std::unordered_set<std::string>
    FeatureCharacterization::computing_environment_intrinsics{"get_command",
        "get_command_argument", "command_argument_count",
        "get_environment_variable"};

std::unordered_set<std::string> FeatureCharacterization::user_defined_operators;

std::unordered_set<std::string>
    FeatureCharacterization::user_defined_assignment;

std::unordered_set<std::string> FeatureCharacterization::dummy_arguments;

std::unordered_set<std::string>
    FeatureCharacterization::pure_value_dummy_arguments;

std::unordered_set<std::string>
    FeatureCharacterization::optional_dummy_arguments;

// Fortran 2008 allows the following executable constructs to have an exit
// statement, in addition to DO: IF, SELECT CASE, SELECT RANK, SELECT TYPE,
// ASSOCIATE, BLOCK, WHERE
std::unordered_set<std::string> FeatureCharacterization::executable_constructs;

bool FeatureCharacterization::use_iso_Fortran_env = false;

bool FeatureCharacterization::is_in_c_binding_procedure = false;

bool FeatureCharacterization::is_in_pure_procedure = false;

bool FeatureCharacterization::is_in_module = false;

bool FeatureCharacterization::is_in_user_defined_operator_function = false;

bool FeatureCharacterization::is_in_user_defined_assignment_subroutine = false;

void FeatureCharacterization::Post(const parser::Name &name) {
  if (use_iso_Fortran_env) {
    CONVERT2LOWERCASE(name.ToString(), nameString);
    if (nameString.compare("iostat_inquire_internal_unit") == 0) {
      features["Constants in ISO_FORTRAN_ENV"] = true;
    } else {
      for (const auto &constName :
          Fortran2008_iso_Fortran_env_constant_arrays) {
        if (nameString.compare(constName) == 0) {
          features["Constants in ISO_FORTRAN_ENV"] = true;
          break;
        }
      }
    }
  }
}
///////////////////////////////
// Fortran 95's New Features //
///////////////////////////////
// R1055 forall-stmt -> FORALL concurrent-header forall-assignment-stmt
void FeatureCharacterization::Post(const parser::ForallStmt &) {
  features["Forall statements"] = true;
}
// R1050 forall-construct ->
//         forall-construct-stmt [forall-body-construct]... end-forall-stmt
void FeatureCharacterization::Post(const parser::ForallConstruct &) {
  features["Forall constructs"] = true;
}
// R1044 where-body-construct ->
//         where-assignment-stmt | where-stmt | where-construct
void FeatureCharacterization::Post(const parser::WhereBodyConstruct &wB) {
  if (std::get_if<Statement<WhereStmt>>(&wB.u)) {
    features["Enhancements to WHERE"] = true;
  } else if (std::get_if<common::Indirection<WhereConstruct>>(&wB.u)) {
    features["Enhancements to WHERE"] = true;
  }
}
// R806 null-init -> function-reference     {constrained to be NULL()}
void FeatureCharacterization::Post(const parser::NullInit &nI) {
  features["Initialization of pointers with NULL function"] = true;
}
// R739 component-decl ->
//       component-name [( component-array-spec )]
//       [lbracket coarray-spec rbracket] [* char-length]
//       [component-initialization]
void FeatureCharacterization::Post(const parser::ComponentDecl &compDecl) {
  const auto &compInit{std::get<std::optional<Initialization>>(compDecl.t)};
  if (compInit.has_value()) {
    features["Default initialization of derived types"] = true;
  }
}
// R1527 prefix-spec ->
//         declaration-type-spec | ELEMENTAL | IMPURE | MODULE |
//         NON_RECURSIVE | PURE | RECURSIVE |
// (CUDA)  ATTRIBUTES ( (DEVICE | GLOBAL | GRID_GLOBAL | HOST)... )
//         LAUNCH_BOUNDS(expr-list) | CLUSTER_DIMS(expr-list)
void FeatureCharacterization::Post(const parser::PrefixSpec &pSpec) {
  if (std::get_if<parser::PrefixSpec::Pure>(&pSpec.u)) {
    features["Pure procedures"] = true;
    is_in_pure_procedure = true;
  } else if (std::get_if<parser::PrefixSpec::Elemental>(&pSpec.u)) {
    features["Elemental procedures"] = true;
  } else if (std::get_if<parser::PrefixSpec::Impure>(&pSpec.u)) {
    features["Impure elemental procedures"] = true;
  } else if (std::get_if<parser::PrefixSpec::Non_Recursive>(&pSpec.u)) {
    features["Recursive and non-recursive procedures"] = true;
  }
}

/////////////////////////////////
// Fortran 2003's New Features //
/////////////////////////////////
std::vector<std::string>
    FeatureCharacterization::Fortran2003_interop_c_procedures{"c_ptr",
        "c_funptr", "c_null_funptr", "c_loc", "c_funloc", "c_associated",
        "c_f_pointer", "c_f_procpointer"};
std::vector<std::string>
    FeatureCharacterization::Fortran2003_interop_c_intrinsictypes{"c_int",
        "c_short", "c_long", "c_long_long", "c_signed_char", "c_size_t",
        "c_int8_t", "c_int16_t", "c_int32_t", "c_int64_t", "c_int_least8_t",
        "c_int_least16_t", "c_int_least32_t", "c_int_least64_t",
        "c_int_fast8_t", "c_int_fast16_t", "c_int_fast32_t", "c_int_fast64_t",
        "c_intmax_t", "c_intptr_t", "c_float", "c_double", "c_long_double",
        "c_float_complex", "c_double_complex", "c_long_double_complex",
        "c_bool", "c_char"};
// R1512 procedure-declaration-stmt ->
//         PROCEDURE ( [proc-interface] ) [[, proc-attr-spec]... ::]
//         proc-decl-list
void FeatureCharacterization::Post(
    const parser::ProcedureDeclarationStmt &pds) {
  const auto &procAttrSpec{std::get<std::list<parser::ProcAttrSpec>>(pds.t)};
  for (const parser::ProcAttrSpec &procAttr : procAttrSpec) {
    if (std::get_if<parser::Pointer>(&procAttr.u)) {
      features["Procedure pointers"] = true;
    }
    break;
  }
}
void FeatureCharacterization::Post(const parser::TypeParamDefStmt &) {
  features["Parameterized derived types"] = true;
}
void FeatureCharacterization::Post(const parser::TypeParamSpec &) {
  features["Parameterized derived types"] = true;
}
void FeatureCharacterization::Post(const parser::FinalProcedureStmt &node) {
  features["Finalization"] = true;
}
void FeatureCharacterization::Post(const parser::Pass &) {
  features["The PASS attribute"] = true;
}
void FeatureCharacterization::Post(const parser::TypeBoundGenericStmt &tbgs) {
  const auto &genericSpec{std::get<Indirection<GenericSpec>>(tbgs.t)};
  if (std::get_if<parser::DefinedOperator>(&genericSpec.value().u)) {
    features["Procedures bound to a type as operators"] = true;
  } else if (std::get_if<GenericSpec::Assignment>(&genericSpec.value().u)) {
    features["Procedures bound to a type as operators"] = true;
  }
}
void FeatureCharacterization::Post(const parser::TypeAttrSpec &tas) {
  if (std::get_if<parser::TypeAttrSpec::Extends>(&tas.u)) {
    features["Type extension"] = true;
  } else if (std::get_if<parser::Abstract>(&tas.u)) {
    features["Deferred bindings and abstract types"] = true;
  }
}
void FeatureCharacterization::Post(const parser::EnumDef &ed) {
  features["Enumerations"] = true;
}
void FeatureCharacterization::Post(const parser::AssociateConstruct &) {
  features["ASSOCIATE construct"] = true;
}
// R703 declaration-type-spec ->
//        intrinsic-type-spec | TYPE ( intrinsic-type-spec ) |
//        TYPE ( derived-type-spec ) | CLASS ( derived-type-spec ) |
//        CLASS ( * ) | TYPE ( * )
// Legacy extension: RECORD /struct/
void FeatureCharacterization::checkDeclarationTypeSpec(
    const parser::DeclarationTypeSpec &dts, bool is_poa) {
  if (std::get_if<parser::DeclarationTypeSpec::Class>(&dts.u)) {
    features["Polymorphic entities"] = true;
  } else if (std::get_if<parser::DeclarationTypeSpec::ClassStar>(&dts.u)) {
    features["Polymorphic entities"] = true;
  } else if (std::get_if<parser::DeclarationTypeSpec::TypeStar>(&dts.u)) {
    features["Assumed type"] = true;
  } else if (const auto *const typeT{
                 std::get_if<parser::DeclarationTypeSpec::Type>(&dts.u)}) {
    const auto &tname{std::get<Name>(typeT->derived.t)};
    CONVERT2LOWERCASE(tname.ToString(), tnString);
    if ((tnString.compare("c_funptr") == 0) ||
        (tnString.compare("c_ptr") == 0)) {
      features["Interoperability with C pointers"] = true;
    }
  } else if (const auto *const intrT{
                 std::get_if<parser::IntrinsicTypeSpec>(&dts.u)}) {
    auto check_interop_intrinsic_types = [&](KindSelector const &kindS) {
      if (const auto *const cExpr{
              std::get_if<parser::ScalarIntConstantExpr>(&kindS.u)}) {
        const auto &kindExp{cExpr->thing.thing.thing.value()};
        if (const auto *const dsn{
                std::get_if<Indirection<Designator>>(&kindExp.u)}) {
          if (const auto *const dref{std::get_if<DataRef>(&dsn->value().u)}) {
            if (const auto *const tname{std::get_if<Name>(&dref->u)}) {
              CONVERT2LOWERCASE(tname->ToString(), tnString);
              auto it = std::find(Fortran2003_interop_c_intrinsictypes.begin(),
                  Fortran2003_interop_c_intrinsictypes.end(), tnString);
              if (it != Fortran2003_interop_c_intrinsictypes.end()) {
                features["Interoperability of intrinsic types"] = true;
                if (!is_poa) {
                  features["Interoperability of variables"] = true;
                }
              }
              auto it2 = std::find(
                  Fortran2008_iso_Fortran_env_constant_scalars.begin(),
                  Fortran2008_iso_Fortran_env_constant_scalars.end(), tnString);
              if (it2 != Fortran2008_iso_Fortran_env_constant_scalars.end()) {
                if (use_iso_Fortran_env) {
                  features["Constants in ISO_FORTRAN_ENV"] = true;
                }
              }
            } else if (const auto *const ae{
                           std::get_if<Indirection<ArrayElement>>(&dref->u)}) {
              if (const auto *const aname{
                      std::get_if<Name>(&ae->value().base.u)}) {
                CONVERT2LOWERCASE(aname->ToString(), anString);
                auto it2 = std::find(
                    Fortran2008_iso_Fortran_env_constant_arrays.begin(),
                    Fortran2008_iso_Fortran_env_constant_arrays.end(),
                    anString);
                if (it2 != Fortran2008_iso_Fortran_env_constant_arrays.end()) {
                  if (use_iso_Fortran_env) {
                    features["Constants in ISO_FORTRAN_ENV"] = true;
                  }
                }
              }
            }
          }
        } else if (const auto *const fr{
                       std::get_if<Indirection<FunctionReference>>(
                           &kindExp.u)}) {
          // FIXME: Parser incorrectly creates FunctionReference instead of
          // ArrayElement. To fix this, semantic analysis should
          // convert FunctionReference to ArrayElement.
          // Temporarily, this check is handled by Post(const parser::Name &).
        }
      }
    };
    if (const auto *const realT{
            std::get_if<parser::IntrinsicTypeSpec::Real>(&intrT->u)}) {
      if (realT->kind.has_value()) {
        check_interop_intrinsic_types(realT->kind.value());
      }
    } else if (const auto *const intT{
                   std::get_if<parser::IntegerTypeSpec>(&intrT->u)}) {
      if (intT->v.has_value()) {
        check_interop_intrinsic_types(intT->v.value());
      }
    } else if (const auto *const compT{
                   std::get_if<parser::IntrinsicTypeSpec::Complex>(
                       &intrT->u)}) {
      if (compT->kind.has_value()) {
        check_interop_intrinsic_types(compT->kind.value());
      }
    } else if (const auto *const logicT{
                   std::get_if<parser::IntrinsicTypeSpec::Logical>(
                       &intrT->u)}) {
      if (logicT->kind.has_value()) {
        check_interop_intrinsic_types(logicT->kind.value());
      }
    } else if (const auto *const charT{
                   std::get_if<parser::IntrinsicTypeSpec::Character>(
                       &intrT->u)}) {
      if (charT->selector.has_value()) {
        if (const auto *const ls{std::get_if<parser::LengthSelector>(
                &charT->selector.value().u)}) {
          if (const auto *const tpv{
                  std::get_if<parser::TypeParamValue>(&ls->u)}) {
            if (const auto *const sie{
                    std::get_if<parser::ScalarIntExpr>(&tpv->u)}) {
              const auto &kindExp{sie->thing.thing.value()};
              if (const auto *const dsn{
                      std::get_if<Indirection<Designator>>(&kindExp.u)}) {
                if (const auto *const dref{
                        std::get_if<DataRef>(&dsn->value().u)}) {
                  if (const auto *const tname{std::get_if<Name>(&dref->u)}) {
                    CONVERT2LOWERCASE(tname->ToString(), tnString);
                    if (tnString.compare("c_char")) {
                      features["Interoperability of intrinsic types"] = true;
                      if (!is_poa) {
                        features["Interoperability of variables"] = true;
                      }
                    }
                  }
                }
              }
            } else if (const auto *const str{
                           std::get_if<parser::Star>(&tpv->u)}) {
              if (is_in_c_binding_procedure) {
                features["C descriptors"] = true;
                features["Attribute codes"] = true;
                features["The type CFI_dim_t"] = true;
              }
            }
          }
        } else if (const auto *const lk{
                       std::get_if<parser::CharSelector::LengthAndKind>(
                           &charT->selector.value().u)}) {
          if (lk->length.has_value()) {
            if (const auto *const sie{std::get_if<parser::ScalarIntExpr>(
                    &lk->length.value().u)}) {
              const auto &lenExp{sie->thing.thing.value()};
              if (const auto *const lc{
                      std::get_if<parser::LiteralConstant>(&lenExp.u)}) {
                if (const auto *const ilc{
                        std::get_if<parser::IntLiteralConstant>(&lc->u)}) {
                }
              }
            } else if (const auto *const str{
                           std::get_if<parser::Star>(&lk->length.value().u)}) {
              if (is_in_c_binding_procedure) {
                features["C descriptors"] = true;
                features["Attribute codes"] = true;
                features["The type CFI_dim_t"] = true;
              }
            }
          }
          const auto &kindExp{lk->kind.thing.thing.thing.value()};
          if (const auto *const dsn{
                  std::get_if<Indirection<Designator>>(&kindExp.u)}) {
            if (const auto *const dref{std::get_if<DataRef>(&dsn->value().u)}) {
              if (const auto *const tname{std::get_if<Name>(&dref->u)}) {
                CONVERT2LOWERCASE(tname->ToString(), tnString);
                if (tnString.compare("c_char")) {
                  features["Interoperability of intrinsic types"] = true;
                  if (!is_poa) {
                    features["Interoperability of variables"] = true;
                  }
                }
              } else if (const auto *const ae{
                             std::get_if<common::Indirection<ArrayElement>>(
                                 &dref->u)}) {
                if (const auto *const aname{
                        std::get_if<Name>(&ae->value().base.u)}) {
                  CONVERT2LOWERCASE(aname->ToString(), anString);
                  if (use_iso_Fortran_env && (anString == "character_kinds")) {
                    features["Constants in ISO_FORTRAN_ENV"] = true;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
// FIXME We dont need this if checkDeclarationTypeSpec() is called in all places
// containing DeclarationTypeSpec.
void FeatureCharacterization::Post(const parser::DeclarationTypeSpec &dts) {
  checkDeclarationTypeSpec(dts, true);
}
// R1153 select-type-construct ->
//           select-type stmt [type-guard-stmt block]...end-select-type-stmt
void FeatureCharacterization::Post(const parser::SelectTypeConstruct &) {
  features["SELECT TYPE construct"] = true;
}
void FeatureCharacterization::Post(const parser::BindAttr &ba) {
  if (std::get_if<parser::BindAttr::Deferred>(&ba.u)) {
    features["Deferred bindings and abstract types"] = true;
  }
}
// FIXME need to check other statement containing DeclarationTypeSpec
void FeatureCharacterization::Post(const parser::TypeDeclarationStmt &tds) {
  const auto &dts{std::get<parser::DeclarationTypeSpec>(tds.t)};
  const auto &attrSpecList{std::get<std::list<parser::AttrSpec>>(tds.t)};
  const auto &entityDeclList{std::get<std::list<parser::EntityDecl>>(tds.t)};

  // check AttrSpecList to see if Allocatable or Pointer is in there
  bool allocatableAttr{false};
  bool pointerAttr{false};
  bool arraySpecAttr{false};
  bool coarraySpecAttr{false};
  bool saveAttr{false};
  bool targetAttr{false};
  bool contiguousAttr{false};
  bool assumedRankAttr{false};
  bool valueAttr{false};
  bool optionalAttr{false};
  for (const parser::AttrSpec &attrSpec : attrSpecList) {
    if (std::holds_alternative<parser::Allocatable>(attrSpec.u)) {
      allocatableAttr = true;
      if (is_in_c_binding_procedure) {
        features["C descriptors"] = true;
        features["Attribute codes"] = true;
        features["The type CFI_dim_t"] = true;
      }
    } else if (std::holds_alternative<parser::Pointer>(attrSpec.u)) {
      pointerAttr = true;
      if (is_in_c_binding_procedure) {
        features["C descriptors"] = true;
        features["Attribute codes"] = true;
        features["The type CFI_dim_t"] = true;
      }
    } else if (std::holds_alternative<parser::ArraySpec>(attrSpec.u)) {
      arraySpecAttr = true;
      auto const *arraySpec = std::get_if<parser::ArraySpec>(&attrSpec.u);
      if (std::holds_alternative<std::list<AssumedShapeSpec>>(arraySpec->u)) {
        if (is_in_c_binding_procedure) {
          features["C descriptors"] = true;
          features["Attribute codes"] = true;
          features["The type CFI_dim_t"] = true;
        }
      } else if (std::holds_alternative<parser::DeferredShapeSpecList>(
                     arraySpec->u)) {
        if (is_in_c_binding_procedure) {
          features["C descriptors"] = true;
          features["Attribute codes"] = true;
          features["The type CFI_dim_t"] = true;
        }
      } else if (std::holds_alternative<parser::AssumedRankSpec>(
                     arraySpec->u)) {
        features["Assumed rank"] = true;
        assumedRankAttr = true;
        if (contiguousAttr) {
          features["Contiguous attribute for assumed-rank arrays"] = true;
        }
      }
    } else if (std::holds_alternative<parser::CoarraySpec>(attrSpec.u)) {
      coarraySpecAttr = true;
    } else if (std::holds_alternative<parser::LanguageBindingSpec>(
                   attrSpec.u)) {
      features["Interoperability of global data"] = true;
    } else if (std::holds_alternative<parser::Save>(attrSpec.u)) {
      saveAttr = true;
    } else if (std::holds_alternative<parser::Target>(attrSpec.u)) {
      targetAttr = true;
    } else if (std::holds_alternative<parser::Contiguous>(attrSpec.u)) {
      contiguousAttr = true;
      if (assumedRankAttr) {
        features["Contiguous attribute for assumed-rank arrays"] = true;
      }
    } else if (std::holds_alternative<parser::Value>(attrSpec.u)) {
      valueAttr = true;
      if (is_in_user_defined_operator_function ||
          is_in_user_defined_assignment_subroutine) {
        features["The value attribute for an argument of a defined operation "
                 "or assignment"] = true;
      }
    } else if (std::holds_alternative<parser::Optional>(attrSpec.u)) {
      optionalAttr = true;
    }
  }
  if (saveAttr && targetAttr) {
    features["Pointer initialization with SAVE attribute"] = true;
  }
  checkDeclarationTypeSpec(dts, pointerAttr || allocatableAttr);
  for (const parser::EntityDecl &ed : entityDeclList) {
    auto const &entityName{std::get<ObjectName>(ed.t)};
    CONVERT2LOWERCASE(entityName.ToString(), enString);
    if (valueAttr && is_in_pure_procedure &&
        (dummy_arguments.find(enString) != dummy_arguments.end())) {
      pure_value_dummy_arguments.insert(enString);
    }
    if (optionalAttr &&
        (dummy_arguments.find(enString) != dummy_arguments.end())) {
      optional_dummy_arguments.insert(enString);
    }
    if (std::get<std::optional<parser::ArraySpec>>(ed.t).has_value()) {
      auto const &arraySpec{
          std::get<std::optional<parser::ArraySpec>>(ed.t).value()};
      if (is_in_c_binding_procedure) {
        if (std::holds_alternative<std::list<AssumedShapeSpec>>(arraySpec.u)) {
          features["C descriptors"] = true;
          features["Attribute codes"] = true;
          features["The type CFI_dim_t"] = true;
        } else if (std::holds_alternative<parser::DeferredShapeSpecList>(
                       arraySpec.u)) {
          features["C descriptors"] = true;
          features["Attribute codes"] = true;
          features["The type CFI_dim_t"] = true;
        }
      }
      if (std::holds_alternative<parser::AssumedRankSpec>(arraySpec.u)) {
        features["Assumed rank"] = true;
        if (contiguousAttr) {
          features["Contiguous attribute for assumed-rank arrays"] = true;
        }
      }
    }
  }
  if (allocatableAttr) {
    // if there is an arrayspec, it's not a scalar
    if (arraySpecAttr || coarraySpecAttr) {
      return;
    }
    for (const parser::EntityDecl &ed : entityDeclList) {
      if (std::get<std::optional<parser::ArraySpec>>(ed.t).has_value()) {
        return;
      }
      if (std::get<std::optional<parser::CoarraySpec>>(ed.t).has_value()) {
        return;
      }
    }

    if (const auto *const its{std::get_if<parser::IntrinsicTypeSpec>(&dts.u)}) {
      if (std::holds_alternative<parser::IntrinsicTypeSpec::Character>(
              its->u)) {
        features["Allocatable character length"] = true;
      } else {
        features["Allocatable scalars"] = true;
      }
    } else {
      // All non-array types are scalar type.
      features["Allocatable scalars"] = true;
    }
  }
}
void FeatureCharacterization::Post(const parser::DataComponentDefStmt &dcd) {
  const auto &dts{std::get<parser::DeclarationTypeSpec>(dcd.t)};
  const auto &attrSpecList{std::get<std::list<ComponentAttrSpec>>(dcd.t)};
  const auto &compFillList{std::get<std::list<ComponentOrFill>>(dcd.t)};

  // check AttrSpecList to see if Allocatable or Pointer is in there
  bool allocatableAttr{false};
  bool pointerAttr{false};
  bool compArraySpecAttr{false};
  bool coarraySpecAttr{false};
  for (const parser::ComponentAttrSpec &attrSpec : attrSpecList) {
    if (std::holds_alternative<parser::Allocatable>(attrSpec.u)) {
      allocatableAttr = true;
    }
    if (std::holds_alternative<parser::Pointer>(attrSpec.u)) {
      pointerAttr = true;
    }
    if (std::holds_alternative<parser::ComponentArraySpec>(attrSpec.u)) {
      compArraySpecAttr = true;
    }
    if (std::holds_alternative<parser::CoarraySpec>(attrSpec.u)) {
      coarraySpecAttr = true;
    }
  }
  checkDeclarationTypeSpec(dts, pointerAttr || allocatableAttr);
  if (allocatableAttr) {
    // if there is an arrayspec, it's not a scalar
    if (compArraySpecAttr || coarraySpecAttr) {
      return;
    }
    for (const parser::ComponentOrFill &cf : compFillList) {
      if (const auto *const cd{std::get_if<parser::ComponentDecl>(&cf.u)}) {
        const auto &as{
            std::get<std::optional<parser::ComponentArraySpec>>(cd->t)};
        if (as.has_value()) {
          return;
        }
        const auto &cas{std::get<std::optional<parser::CoarraySpec>>(cd->t)};
        if (cas.has_value()) {
          return;
        }
      }
    }

    if (const auto *const its{std::get_if<parser::IntrinsicTypeSpec>(&dts.u)}) {
      if (std::holds_alternative<parser::IntrinsicTypeSpec::Character>(
              its->u)) {
        features["Allocatable character length"] = true;
      } else {
        features["Allocatable scalars"] = true;
      }
    } else {
      // All non-array types are scalar type.
      features["Allocatable scalars"] = true;
    }
  }
}
void FeatureCharacterization::Post(const parser::AllocateStmt &allocateStmt) {
  const auto &allocOptions{
      std::get<std::list<parser::AllocOpt>>(allocateStmt.t)};
  if (allocOptions.size() > 0) {
    for (const parser::AllocOpt &allocOption : allocOptions) {
      if (std::get_if<parser::AllocOpt::Source>(&allocOption.u)) {
        features["The allocate statement (allocate with SOURCE)"] = true;
      } else if (std::get_if<parser::AllocOpt::Mold>(&allocOption.u)) {
        features["Allocating a polymorphic variable using MOLD"] = true;
      }
    }
  }
  /*if (auto info{CheckAllocateOptions(allocateStmt, context_)}) {
    for (const parser::Allocation &allocation :
        std::get<std::list<parser::Allocation>>(allocateStmt.t)) {
      AllocationCheckerHelper ach{allocation, *info};
      if (ach.checkScalar()) {
        features["Allocatable scalars"] = true;
      }
      if (ach.checkAllocatableCharacter())
        features["Allocatable character length"] = true;
    }
    if (info->gotSource)
      features["The allocate statement (allocate with SOURCE)"] = true;
  }*/
}
// R829 allocatable-stmt -> ALLOCATABLE [::] allocatable-decl-list
void FeatureCharacterization::Post(
    const parser::AllocatableStmt &allocatableStmt) {
  for (const parser::ObjectDecl &od : allocatableStmt.v) {
    const parser::ObjectName &name{std::get<parser::ObjectName>(od.t)};
    const Symbol *ultimate_{
        name.symbol ? &name.symbol->GetUltimate() : nullptr};
    if (ultimate_) {
      if (ultimate_->Rank() == 0) {
        out_ << name.ToString() << "\n";
        if (ultimate_->GetType()->category() ==
            DeclTypeSpec::Category::Character) {
          features["Allocatable character length"] = true;
        } else {
          features["Allocatable scalars"] = true;
        }
      }
    }
  }
}
// R807 access-spec -> PUBLIC | PRIVATE
void FeatureCharacterization::Post(const parser::AccessSpec &as) {
  features["More control of access from a module"] = true;
}
void FeatureCharacterization::Post(const parser::Protected &as) {
  features["More control of access from a module"] = true;
}
// R1409 use-stmt ->
//         USE [[, module-nature] ::] module-name [, rename-list] |
//         USE [[, module-nature] ::] module-name , ONLY : [only-list]
// R1410 module-nature -> INTRINSIC | NON_INTRINSIC
void FeatureCharacterization::Post(const parser::UseStmt &us) {
  CONVERT2LOWERCASE(us.moduleName.ToString(), modString);
  auto it = std::find(Fortran_intrinsic_modules.begin(),
      Fortran_intrinsic_modules.end(), modString);
  if (it != Fortran_intrinsic_modules.end()) {
    if (us.nature.value_or(parser::UseStmt::ModuleNature::Non_Intrinsic) !=
        parser::UseStmt::ModuleNature::Non_Intrinsic) {
      features["Intrinsic modules"] = true;
      if (modString == "iso_fortran_env") {
        use_iso_Fortran_env = true;
      }
    }
  }
  if (const auto *const renameList{std::get_if<std::list<Rename>>(&us.u)}) {
    if (renameList->begin() != renameList->end()) {
      features["Renaming operators on the USE statement"] = true;
    }
  }
  if (const auto *const onlyList{std::get_if<std::list<Only>>(&us.u)}) {
    for (const Only &o : *onlyList) {
      if (std::holds_alternative<Rename>(o.u)) {
        features["Renaming operators on the USE statement"] = true;
      }
    }
  }
  used_modules.insert(modString);
}
void FeatureCharacterization::Post(const parser::PointerAssignmentStmt &pas) {
  const auto &bounds{std::get<PointerAssignmentStmt::Bounds>(pas.t)};
  if (const auto *const brList{
          std::get_if<std::list<BoundsRemapping>>(&bounds.u)}) {
    if (!brList->empty()) {
      features["Pointer assignment (rank remapping)"] = true;
    }
  }
}
void FeatureCharacterization::Post(const parser::ImportStmt &is) {
  features["The IMPORT statement"] = true;
  if (is.kind != common::ImportKind::Default) {
    features["Control of host association"] = true;
  }
}
// R1520 function-reference -> procedure-designator ( [actual-arg-spec-list] )
// R1521 call-stmt -> CALL procedure-designator [( [actual-arg-spec-list] )]
void FeatureCharacterization::Post(const parser::Call &c) {
  const auto &actArgSpecs{std::get<std::list<ActualArgSpec>>(c.t)};
  const auto &pd{std::get<ProcedureDesignator>(c.t)};
  if (const auto *const name{std::get_if<Name>(&pd.u)}) {
    CONVERT2LOWERCASE(name->ToString(), fnName);
    if (fnName == "null") {
      features["New and enhanced intrinsic procedures"] = true;
      features["Initialization of pointers with NULL function"] = true;
    } else if (fnName == "cpu_time") {
      features["New and enhanced intrinsic procedures"] = true;
    } else if (fnName == "ceiling" || fnName == "floor") {
      for (const auto &arg : actArgSpecs) {
        const auto &optKeyword = std::get<std::optional<Keyword>>(arg.t);
        if (optKeyword.has_value()) {
          CONVERT2LOWERCASE(optKeyword.value().v.ToString(), kw);
          if (kw == "kind") {
            features["New and enhanced intrinsic procedures"] = true;
            break;
          }
        }
      }
    } else if (fnName == "selected_int_kind") {
      for (const auto &arg : actArgSpecs) {
        const auto &argSpec = std::get<ActualArg>(arg.t);
        if (const auto *argExpr =
                std::get_if<common::Indirection<Expr>>(&argSpec.u)) {
          if (const auto *const lc{
                  std::get_if<LiteralConstant>(&argExpr->value().u)}) {
            if (const auto *const ilc{
                    std::get_if<IntLiteralConstant>(&lc->u)}) {
              const auto &ilcStr = std::get<CharBlock>(ilc->t);
              if (ilcStr.ToString() == "18") {
                features["Long integers"] = true;
              }
            }
          }
        }
      }
    } else if (fnName == "storage_size") {
      features["Storage size"] = true;
    } else if ((fnName == "compiler_options") ||
        (fnName == "compiler_version")) {
      features["Compiler information in ISO_FORTRAN_ENV"] = true;
    } else if (fnName == "c_sizeof") {
      features["Function for C sizeof"] = true;
    } else if (fnName == "rank") {
      features["Assumed rank"] = true;
    } else if (fnName == "cmplx") {
      bool found_kind = false;
      for (const auto &arg : actArgSpecs) {
        const auto &optKeyword = std::get<std::optional<Keyword>>(arg.t);
        if (optKeyword.has_value()) {
          CONVERT2LOWERCASE(optKeyword.value().v.ToString(), kw);
          if (kw == "kind") {
            found_kind = true;
            break;
          }
        }
      }
      if (!found_kind) {
        features["Simplification of calls of the intrinsic cmplx"] = true;
      }
    } else if (fnName == "reduce") {
      features["New reduction intrinsic reduce"] = true;
    } else if (fnName == "coshape") {
      features["Intrinsic function coshape"] = true;
    } else if (fnName == "random_init") {
      features["Intrinsic subroutine random_init"] = true;
    } else if (fnName == "all" || fnName == "any" || fnName == "norm2" ||
        fnName == "parity") {
      if (actArgSpecs.size() == 2) {
        const auto &argSpec = std::get<ActualArg>(actArgSpecs.back().t);
        if (const auto *argExprPtr =
                std::get_if<common::Indirection<Expr>>(&argSpec.u)) {
          const auto &argExpr = argExprPtr->value();
          if (const auto *const dsn =
                  std::get_if<Indirection<Designator>>(&argExpr.u)) {
            if (const auto *const dref =
                    std::get_if<DataRef>(&dsn->value().u)) {
              if (const auto *const tname{std::get_if<Name>(&dref->u)}) {
                CONVERT2LOWERCASE(tname->ToString(), tnameStr);
                if (optional_dummy_arguments.find(tnameStr) !=
                    optional_dummy_arguments.end()) {
                  features["Removal of the restriction on argument dim of many "
                           "intrinsic functions"] = true;
                }
              }
            }
          }
        }
      }
    } else if (fnName == "this_image") {
      if (actArgSpecs.size() > 1) {
        int tIndex = 0;
        bool keyword_used = false;
        bool found_dim = false;
        for (const auto &arg : actArgSpecs) {
          const auto &optKeyword = std::get<std::optional<Keyword>>(arg.t);
          if (optKeyword.has_value()) {
            keyword_used = true;
            CONVERT2LOWERCASE(optKeyword.value().v.ToString(), kw);
            if (kw == "dim") {
              found_dim = true;
            }
          } else {
            keyword_used = false;
          }
          if (found_dim || (!keyword_used && (tIndex == 1))) {
            const auto &argSpec = std::get<ActualArg>(arg.t);
            if (const auto *argExprPtr =
                    std::get_if<common::Indirection<Expr>>(&argSpec.u)) {
              const auto &argExpr = argExprPtr->value();
              if (const auto *const dsn =
                      std::get_if<Indirection<Designator>>(&argExpr.u)) {
                if (const auto *const dref =
                        std::get_if<DataRef>(&dsn->value().u)) {
                  if (const auto *const tname{std::get_if<Name>(&dref->u)}) {
                    CONVERT2LOWERCASE(tname->ToString(), tnameStr);
                    if (optional_dummy_arguments.find(tnameStr) !=
                        optional_dummy_arguments.end()) {
                      features
                          ["Removal of the restriction on argument dim of many "
                           "intrinsic functions"] = true;
                    }
                  }
                }
              }
            }
            break;
          }
          tIndex++;
        }
      }
    } else if (fnName == "iall" || fnName == "iany" || fnName == "iparity" ||
        fnName == "maxval" || fnName == "minval" || fnName == "product" ||
        fnName == "sum" || fnName == "maxloc" || fnName == "minloc") {
      if (actArgSpecs.size() > 1) {
        int tIndex = 0;
        bool keyword_used = false;
        bool found_dim = false;
        for (const auto &arg : actArgSpecs) {
          const auto &optKeyword = std::get<std::optional<Keyword>>(arg.t);
          if (optKeyword.has_value()) {
            keyword_used = true;
            CONVERT2LOWERCASE(optKeyword.value().v.ToString(), kw);
            if (kw == "dim") {
              found_dim = true;
            } else if (kw == "back") {
              if (fnName == "maxloc" || fnName == "minloc") {
                features["Optional back argument added to maxloc and minloc"] =
                    true;
              }
            }
          } else {
            keyword_used = false;
            if (fnName == "maxloc" || fnName == "minloc") {
              if (tIndex == 4) {
                features["Optional back argument added to maxloc and minloc"] =
                    true;
              } else if (tIndex == 3) {
                //[FIXME] If this is the last argument and its type is logical,
                // then it is the back argument.
                // features["Optional back argument added to maxloc and minloc"]
                // = true;
              }
            }
          }
          //[FIXME] If keyword is not used, we need to check the type of the
          // second argument to see whether it is dim or not.
          // if (found_dim || (!keyword_used && (tIndex == 1)))
          if (found_dim) {
            const auto &argSpec = std::get<ActualArg>(arg.t);
            if (const auto *argExprPtr =
                    std::get_if<common::Indirection<Expr>>(&argSpec.u)) {
              const auto &argExpr = argExprPtr->value();
              if (const auto *const dsn =
                      std::get_if<Indirection<Designator>>(&argExpr.u)) {
                if (const auto *const dref =
                        std::get_if<DataRef>(&dsn->value().u)) {
                  if (const auto *const tname{std::get_if<Name>(&dref->u)}) {
                    CONVERT2LOWERCASE(tname->ToString(), tnameStr);
                    if (optional_dummy_arguments.find(tnameStr) !=
                        optional_dummy_arguments.end()) {
                      features
                          ["Removal of the restriction on argument dim of many "
                           "intrinsic functions"] = true;
                    }
                  }
                }
              }
            }
            break;
          }
          tIndex++;
        }
      }
    } else if (fnName == "findloc") {
      if (actArgSpecs.size() > 2) {
        int tIndex = 0;
        bool keyword_used = false;
        bool found_dim = false;
        for (const auto &arg : actArgSpecs) {
          const auto &optKeyword = std::get<std::optional<Keyword>>(arg.t);
          if (optKeyword.has_value()) {
            keyword_used = true;
            CONVERT2LOWERCASE(optKeyword.value().v.ToString(), kw);
            if (kw == "dim") {
              found_dim = true;
            }
          } else {
            keyword_used = false;
          }
          //[FIXME] If keyword is not used, we need to check the type of the
          // third argument to see whether it is dim or not.
          // if (found_dim || (!keyword_used && (tIndex == 2)))
          if (found_dim) {
            const auto &argSpec = std::get<ActualArg>(arg.t);
            if (const auto *argExprPtr =
                    std::get_if<common::Indirection<Expr>>(&argSpec.u)) {
              const auto &argExpr = argExprPtr->value();
              if (const auto *const dsn =
                      std::get_if<Indirection<Designator>>(&argExpr.u)) {
                if (const auto *const dref =
                        std::get_if<DataRef>(&dsn->value().u)) {
                  if (const auto *const tname{std::get_if<Name>(&dref->u)}) {
                    CONVERT2LOWERCASE(tname->ToString(), tnameStr);
                    if (optional_dummy_arguments.find(tnameStr) !=
                        optional_dummy_arguments.end()) {
                      features
                          ["Removal of the restriction on argument dim of many "
                           "intrinsic functions"] = true;
                    }
                  }
                }
              }
            }
            break;
          }
          tIndex++;
        }
      }
    } else if (computing_environment_intrinsics.find(fnName) !=
        computing_environment_intrinsics.end()) {
      features
          ["Access to the computing environment (Command line processing)"] =
              true;
      for (const auto &arg : actArgSpecs) {
        const auto &optKeyword = std::get<std::optional<Keyword>>(arg.t);
        if (optKeyword.has_value()) {
          CONVERT2LOWERCASE(optKeyword.value().v.ToString(), kw);
          if (kw == "errmsg") {
            features["Changes to Intrinsics that access the computing "
                     "environment"] = true;
            break;
          }
        }
      }
    } else {
      for (const auto &p : Fortran2003_interop_c_procedures) {
        if (fnName == p) {
          features["Interoperability with C pointers"] = true;
          break;
        }
      }
    }
  }
  if (std::get_if<ProcComponentRef>(&pd.u)) {
    features["Procedures bound by name to a type (type-bound procedures)"] =
        true;
  }
}
void FeatureCharacterization::Post(const parser::IoControlSpec &iocs) {
  if (const auto *const format{std::get_if<Format>(&iocs.u)}) {
    if (const auto *const expr{std::get_if<Expr>(&format->u)}) {
      if (const auto *const ls{std::get_if<LiteralConstant>(&expr->u)}) {
        if (const auto *const cls{std::get_if<CharLiteralConstant>(&ls->u)}) {
          std::string fmtString{cls->GetString()};
          if (fmtString.find("DT") != std::string::npos ||
              fmtString.find("dt") != std::string::npos ||
              fmtString.find("dT") != std::string::npos ||
              fmtString.find("Dt") != std::string::npos) {
            features["Derived type I/O"] = true;
          }
        }
      }
    }
  }
}
// R748 type-bound-proc-binding ->
//        type-bound-procedure-stmt | type-bound-generic-stmt |
//        final-procedure-stmt
void FeatureCharacterization::Post(const parser::TypeBoundProcBinding &tbpb) {
  features["Procedures bound by name to a type (type-bound procedures)"] = true;
  if (const auto *const tbgs{std::get_if<TypeBoundGenericStmt>(&tbpb.u)}) {
    const auto &genericSpec{std::get<Indirection<GenericSpec>>(tbgs->t)};
    if (const auto *definedOp{
            std::get_if<parser::DefinedOperator>(&genericSpec.value().u)}) {
      if (std::get_if<parser::DefinedOperator::IntrinsicOperator>(
              &(definedOp->u))) {
        const auto fNameList{std::get<std::list<parser::Name>>(tbgs->t)};
        for (const auto &name : fNameList) {
          CONVERT2LOWERCASE(name.ToString(), nameString);
          user_defined_operators.insert(nameString);
        }
      }
    } else if (std::get_if<parser::GenericSpec::Assignment>(
                   &genericSpec.value().u)) {
      const auto fNameList{std::get<std::list<parser::Name>>(tbgs->t)};
      for (const auto &name : fNameList) {
        CONVERT2LOWERCASE(name.ToString(), nameString);
        user_defined_assignment.insert(nameString);
      }
    } else if (std::get_if<parser::GenericSpec::ReadFormatted>(
                   &genericSpec.value().u) ||
        std::get_if<parser::GenericSpec::ReadUnformatted>(
            &genericSpec.value().u) ||
        std::get_if<parser::GenericSpec::WriteFormatted>(
            &genericSpec.value().u) ||
        std::get_if<parser::GenericSpec::WriteUnformatted>(
            &genericSpec.value().u)) {
      features["Derived type I/O"] = true;
    }
  }
}
void FeatureCharacterization::Post(const parser::InterfaceBlock &ib) {
  const auto &ispeclist{
      std::get<std::list<parser::InterfaceSpecification>>(ib.t)};
  const auto &is{
      std::get<parser::Statement<parser::InterfaceStmt>>(ib.t).statement};
  if (const auto *genericSpec{
          std::get_if<std::optional<parser::GenericSpec>>(&is.u)}) {
    if (genericSpec->has_value()) {
      if (std::get_if<parser::GenericSpec::ReadFormatted>(
              &genericSpec->value().u) ||
          std::get_if<parser::GenericSpec::ReadUnformatted>(
              &genericSpec->value().u) ||
          std::get_if<parser::GenericSpec::WriteFormatted>(
              &genericSpec->value().u) ||
          std::get_if<parser::GenericSpec::WriteUnformatted>(
              &genericSpec->value().u)) {
        features["Derived type I/O"] = true;
      } else if (const auto *definedOp{std::get_if<parser::DefinedOperator>(
                     &genericSpec->value().u)}) {
        if (std::get_if<parser::DefinedOperator::IntrinsicOperator>(
                &(definedOp->u))) {
          for (const auto &ispec : ispeclist) {
            const auto *ps{
                std::get_if<parser::Statement<parser::ProcedureStmt>>(
                    &ispec.u)};
            const auto &nameList{
                std::get<std::list<parser::Name>>(ps->statement.t)};
            for (const auto &name : nameList) {
              CONVERT2LOWERCASE(name.ToString(), nameString);
              user_defined_operators.insert(nameString);
            }
          }
        }
      } else if (std::get_if<parser::GenericSpec::Assignment>(
                     &genericSpec->value().u)) {
        for (const auto &ispec : ispeclist) {
          const auto *ps{
              std::get_if<parser::Statement<parser::ProcedureStmt>>(&ispec.u)};
          const auto &nameList{
              std::get<std::list<parser::Name>>(ps->statement.t)};
          for (const auto &name : nameList) {
            CONVERT2LOWERCASE(name.ToString(), nameString);
            user_defined_assignment.insert(nameString);
          }
        }
      }
    }
  }
}
void FeatureCharacterization::Post(const parser::TypeAttrSpec::BindC &) {
  features["Interoperability of derived types"] = true;
}
void FeatureCharacterization::Post(const parser::ProcAttrSpec &pas) {
  if (std::get_if<parser::LanguageBindingSpec>(&pas.u)) {
    features["Interoperability of procedures"] = true;
  }
}
void FeatureCharacterization::Post(const parser::Suffix &sfx) {
  if (sfx.binding.has_value()) {
    features["Interoperability of procedures"] = true;
  }
}
void FeatureCharacterization::Post(const parser::SubroutineStmt &sts) {
  const auto &lbinding{std::get<std::optional<LanguageBindingSpec>>(sts.t)};
  if (lbinding.has_value()) {
    features["Interoperability of procedures"] = true;
    is_in_c_binding_procedure = true;
  } else {
    is_in_c_binding_procedure = false;
  }
  const auto &fName{std::get<parser::Name>(sts.t)};
  CONVERT2LOWERCASE(fName.ToString(), nameString);
  if (user_defined_assignment.find(nameString) !=
      user_defined_assignment.end()) {
    is_in_user_defined_assignment_subroutine = true;
  }
  const auto &dummyArgs{std::get<std::list<parser::DummyArg>>(sts.t)};
  for (const auto &dummyArg : dummyArgs) {
    const auto *dummyArgName{std::get_if<parser::Name>(&dummyArg.u)};
    CONVERT2LOWERCASE(dummyArgName->ToString(), dummyArgString);
    dummy_arguments.insert(dummyArgString);
  }
}
void FeatureCharacterization::Post(const parser::EndSubroutineStmt &ests) {
  is_in_c_binding_procedure = false;
  is_in_pure_procedure = false;
  is_in_user_defined_assignment_subroutine = false;
  dummy_arguments.clear();
  pure_value_dummy_arguments.clear();
  optional_dummy_arguments.clear();
}
void FeatureCharacterization::Post(const parser::FunctionStmt &fts) {
  const auto &sufx{std::get<std::optional<Suffix>>(fts.t)};
  if (sufx.has_value() && sufx.value().binding.has_value()) {
    features["Interoperability of procedures"] = true;
    is_in_c_binding_procedure = true;
  } else {
    is_in_c_binding_procedure = false;
  }
  const auto &fName{std::get<parser::Name>(fts.t)};
  CONVERT2LOWERCASE(fName.ToString(), nameString);
  if (user_defined_operators.find(nameString) != user_defined_operators.end()) {
    is_in_user_defined_operator_function = true;
  }
  const auto &dummyArgs{std::get<std::list<parser::Name>>(fts.t)};
  for (const auto &dummyArg : dummyArgs) {
    CONVERT2LOWERCASE(dummyArg.ToString(), dummyArgString);
    dummy_arguments.insert(dummyArgString);
  }
}
void FeatureCharacterization::Post(const parser::EndFunctionStmt &efts) {
  is_in_c_binding_procedure = false;
  is_in_pure_procedure = false;
  is_in_user_defined_operator_function = false;
  dummy_arguments.clear();
  pure_value_dummy_arguments.clear();
  optional_dummy_arguments.clear();
}
void FeatureCharacterization::Post(const parser::BindStmt &) {
  features["Interoperability of global data"] = true;
}

/////////////////////////////////
// Fortran 2008's New Features //
/////////////////////////////////
std::vector<std::string>
    FeatureCharacterization::Fortran2008_iso_Fortran_env_constant_arrays{
        "character_kinds", "integer_kinds", "real_kinds", "logical_kinds"};
std::vector<std::string>
    FeatureCharacterization::Fortran2008_iso_Fortran_env_constant_scalars{
        "int8", "int16", "int32", "int64", "real32", "real64", "real128",
        "iostat_inquire_internal_unit"};
void FeatureCharacterization::Post(const parser::SubmoduleStmt &) {
  features["Submodules"] = true;
}
void FeatureCharacterization::Post(const parser::DoConstruct &node) {
  features["do concurrent"] = node.IsDoConcurrent();
}
void FeatureCharacterization::Post(const parser::AttrSpec &at) {
  if (std::get_if<parser::Contiguous>(&at.u)) {
    features["Contiguous attribute"] = true;
  }
}
void FeatureCharacterization::Post(const parser::ComponentAttrSpec &cat) {
  if (std::get_if<parser::Contiguous>(&cat.u)) {
    features["Contiguous attribute"] = true;
  }
}
void FeatureCharacterization::Post(const parser::ContiguousStmt &) {
  features["Contiguous attribute"] = true;
}
void FeatureCharacterization::Post(const parser::ConcurrentHeader &ch) {
  const auto &intTypeSpec{std::get<std::optional<IntegerTypeSpec>>(ch.t)};
  if (intTypeSpec.has_value()) {
    if (intTypeSpec.value().v.has_value()) {
      const auto &kindSpec{intTypeSpec.value().v.value()};
      if (std::get_if<parser::ScalarIntConstantExpr>(&kindSpec.u)) {
        features["Kind of a forall index"] = true;
      }
    }
  }
}
void FeatureCharacterization::Post(const parser::ConnectSpec::Newunit &) {
  features["Finding a unit when opening a file (newunit=u)"] = true;
}
void FeatureCharacterization::Post(const parser::BlockStmt &bs) {
  features["The BLOCK construct"] = true;
  if (bs.v.has_value()) {
    const auto &blockName{bs.v.value()};
    CONVERT2LOWERCASE(blockName.ToString(), blockNameString);
    executable_constructs.insert(blockNameString);
  }
}
void FeatureCharacterization::Post(const parser::EndBlockStmt &ebs) {
  if (ebs.v.has_value()) {
    const auto &blockName{ebs.v.value()};
    CONVERT2LOWERCASE(blockName.ToString(), blockNameString);
    executable_constructs.erase(blockNameString);
  }
}
void FeatureCharacterization::Post(const parser::IfThenStmt &its) {
  const auto &ifConstructName{std::get<std::optional<parser::Name>>(its.t)};
  if (ifConstructName.has_value()) {
    const auto &name{ifConstructName.value()};
    CONVERT2LOWERCASE(name.ToString(), nameString);
    executable_constructs.insert(nameString);
  }
}
void FeatureCharacterization::Post(const parser::EndIfStmt &eifs) {
  if (eifs.v.has_value()) {
    const auto &ifConstructName{eifs.v.value()};
    CONVERT2LOWERCASE(ifConstructName.ToString(), ifConstructNameString);
    executable_constructs.erase(ifConstructNameString);
  }
}
void FeatureCharacterization::Post(const parser::AssociateStmt &as) {
  const auto &associateStmtName{std::get<std::optional<parser::Name>>(as.t)};
  if (associateStmtName.has_value()) {
    const auto &name{associateStmtName.value()};
    CONVERT2LOWERCASE(name.ToString(), nameString);
    executable_constructs.insert(nameString);
  }
}
void FeatureCharacterization::Post(const parser::EndAssociateStmt &eas) {
  if (eas.v.has_value()) {
    const auto &associateStmtName{eas.v.value()};
    CONVERT2LOWERCASE(associateStmtName.ToString(), nameString);
    executable_constructs.erase(nameString);
  }
}
void FeatureCharacterization::Post(const parser::WhereConstructStmt &wcs) {
  const auto &whereConstructName{std::get<std::optional<parser::Name>>(wcs.t)};
  if (whereConstructName.has_value()) {
    const auto &name{whereConstructName.value()};
    CONVERT2LOWERCASE(name.ToString(), nameString);
    executable_constructs.insert(nameString);
  }
}
void FeatureCharacterization::Post(const parser::EndWhereStmt &ews) {
  if (ews.v.has_value()) {
    const auto &whereConstructName{ews.v.value()};
    CONVERT2LOWERCASE(whereConstructName.ToString(), nameString);
    executable_constructs.erase(nameString);
  }
}
void FeatureCharacterization::Post(const parser::SelectCaseStmt &scs) {
  const auto &constructName{std::get<std::optional<parser::Name>>(scs.t)};
  if (constructName.has_value()) {
    const auto &name{constructName.value()};
    CONVERT2LOWERCASE(name.ToString(), nameString);
    executable_constructs.insert(nameString);
  }
}
void FeatureCharacterization::Post(const parser::SelectTypeStmt &sts) {
  const auto &constructName = std::get<0>(sts.t);
  if (constructName.has_value()) {
    const auto &name{constructName.value()};
    CONVERT2LOWERCASE(name.ToString(), nameString);
    executable_constructs.insert(nameString);
  }
}
void FeatureCharacterization::Post(const parser::EndSelectStmt &ess) {
  if (ess.v.has_value()) {
    const auto &constructName{ess.v.value()};
    CONVERT2LOWERCASE(constructName.ToString(), nameString);
    executable_constructs.erase(nameString);
  }
}
void FeatureCharacterization::Post(const parser::ExitStmt &es) {
  if (es.v.has_value()) {
    const auto &constructName{es.v.value()};
    CONVERT2LOWERCASE(constructName.ToString(), nameString);
    if (executable_constructs.find(nameString) != executable_constructs.end()) {
      features["Exit statement allowed in almost any construct"] = true;
    }
  }
}

/////////////////////////////////
// Fortran 2018's New Features //
/////////////////////////////////
void FeatureCharacterization::Post(const parser::SelectRankStmt &srs) {
  features["SELECT RANK"] = true;
  const auto &constructName = std::get<0>(srs.t);
  if (constructName.has_value()) {
    const auto &name{constructName.value()};
    CONVERT2LOWERCASE(name.ToString(), nameString);
    executable_constructs.insert(nameString);
  }
}
void FeatureCharacterization::Post(const parser::AssumedSizeSpec &) {
  features["Assumed-size arrays"] = true;
}
void FeatureCharacterization::Post(const parser::ImpliedShapeSpec &iss) {
  auto &assumedImpliedShapeSpecs = iss.v;
  if (assumedImpliedShapeSpecs.size() == 1) {
    features["Assumed-size arrays"] = true;
  }
}
void FeatureCharacterization::Post(const parser::ImplicitStmt &is) {
  if (const auto *const inns{
          std::get_if<std::list<ImplicitStmt::ImplicitNoneNameSpec>>(&is.u)}) {
    if (!inns->empty()) {
      features["Implicit none enhancement"] = true;
    }
  }
}
void FeatureCharacterization::Post(const parser::AcImpliedDoControl &aic) {
  const auto &its{std::get<std::optional<IntegerTypeSpec>>(aic.t)};
  if (its.has_value()) {
    features["Kind of the do variable in implied do"] = true;
  }
}
void FeatureCharacterization::Post(const parser::DataImpliedDo &did) {
  const auto &its{std::get<std::optional<IntegerTypeSpec>>(did.t)};
  if (its.has_value()) {
    features["Kind of the do variable in implied do"] = true;
  }
}
void FeatureCharacterization::Post(const parser::LocalitySpec &ls) {
  features["Locality clauses in do concurrent"] = true;
}
void FeatureCharacterization::Post(const parser::StopStmt &ss) {
  if (is_in_pure_procedure) {
    const auto &kind{std::get<parser::StopStmt::Kind>(ss.t)};
    if (kind == parser::StopStmt::Kind::ErrorStop) {
      features["Removal of anomalies regarding pure procedures"] = true;
    }
  }
}
void FeatureCharacterization::Post(const parser::ModuleStmt &ls) {
  is_in_module = true;
}
void FeatureCharacterization::Post(const parser::EndModuleStmt &ls) {
  is_in_module = false;
  used_modules.clear();
}
void FeatureCharacterization::Post(const parser::AccessStmt &as) {
  if (is_in_module) {
    const auto &accessIdList{std::get<std::list<parser::AccessId>>(as.t)};
    for (const auto &accessId : accessIdList) {
      const auto *aName{std::get_if<parser::Name>(&accessId.v.value().u)};
      if (aName != nullptr) {
        CONVERT2LOWERCASE(aName->ToString(), accessIdString);
        if (used_modules.find(accessIdString) != used_modules.end()) {
          features
              ["Default accessibility for entities accessed from a module"] =
                  true;
          break;
        }
      }
    }
  }
}
void FeatureCharacterization::Post(const parser::AssignmentStmt &as) {
  const auto &var{std::get<parser::Variable>(as.t)};
  if (const auto *dsn{
          std::get_if<common::Indirection<parser::Designator>>(&var.u)}) {
    const auto &designator{dsn->value()};
    if (const auto *dr{std::get_if<parser::DataRef>(&designator.u)}) {
      if (const auto *name{std::get_if<parser::Name>(&dr->u)}) {
        CONVERT2LOWERCASE(name->ToString(), nameString);
        if (pure_value_dummy_arguments.find(nameString) !=
            pure_value_dummy_arguments.end()) {
          features["Removal of anomalies regarding pure procedures"] = true;
        }
      } else if (const auto *ar{
                     std::get_if<common::Indirection<parser::ArrayElement>>(
                         &dr->u)}) {
        const auto &base{ar->value().base};
        if (const auto *name{std::get_if<parser::Name>(&base.u)}) {
          CONVERT2LOWERCASE(name->ToString(), nameString);
          if (pure_value_dummy_arguments.find(nameString) !=
              pure_value_dummy_arguments.end()) {
            features["Removal of anomalies regarding pure procedures"] = true;
          }
        }
      }
    } else if (const auto *substr{
                   std::get_if<parser::Substring>(&designator.u)}) {
      const auto &dref{std::get<parser::DataRef>(substr->t)};
      if (const auto *name{std::get_if<parser::Name>(&dref.u)}) {
        CONVERT2LOWERCASE(name->ToString(), nameString);
        if (pure_value_dummy_arguments.find(nameString) !=
            pure_value_dummy_arguments.end()) {
          features["Removal of anomalies regarding pure procedures"] = true;
        }
      }
    }
  }
}

void FeatureCharacterization::Post(const parser::ReadStmt &rs) {
  for (const auto &titem : rs.items) {
    if (const auto *const var{std::get_if<parser::Variable>(&titem.u)}) {
      if (const auto *dsn{
              std::get_if<common::Indirection<parser::Designator>>(&var->u)}) {
        if (const auto *dr{std::get_if<parser::DataRef>(&dsn->value().u)}) {
          if (const auto *name{std::get_if<parser::Name>(&dr->u)}) {
            CONVERT2LOWERCASE(name->ToString(), nameString);
            if (pure_value_dummy_arguments.find(nameString) !=
                pure_value_dummy_arguments.end()) {
              features["Removal of anomalies regarding pure procedures"] = true;
            }
          } else if (const auto *ar{
                         std::get_if<common::Indirection<parser::ArrayElement>>(
                             &dr->u)}) {
            const auto &base{ar->value().base};
            if (const auto *name{std::get_if<parser::Name>(&base.u)}) {
              CONVERT2LOWERCASE(name->ToString(), nameString);
              if (pure_value_dummy_arguments.find(nameString) !=
                  pure_value_dummy_arguments.end()) {
                features["Removal of anomalies regarding pure procedures"] =
                    true;
              }
            }
          }
        } else if (const auto *substr{
                       std::get_if<parser::Substring>(&dsn->value().u)}) {
          const auto &dref{std::get<parser::DataRef>(substr->t)};
          if (const auto *name{std::get_if<parser::Name>(&dref.u)}) {
            CONVERT2LOWERCASE(name->ToString(), nameString);
            if (pure_value_dummy_arguments.find(nameString) !=
                pure_value_dummy_arguments.end()) {
              features["Removal of anomalies regarding pure procedures"] = true;
            }
          }
        }
      }
    }
  }
}

void FeatureCharacterization::Post(const parser::WriteStmt &ws) {
  if (ws.iounit.has_value()) {
    const auto &tunit{ws.iounit.value()};
    if (const auto *var{std::get_if<parser::Variable>(&tunit.u)}) {
      if (const auto *dsn{
              std::get_if<common::Indirection<parser::Designator>>(&var->u)}) {
        if (const auto *dr{std::get_if<parser::DataRef>(&dsn->value().u)}) {
          if (const auto *name{std::get_if<parser::Name>(&dr->u)}) {
            CONVERT2LOWERCASE(name->ToString(), nameString);
            if (pure_value_dummy_arguments.find(nameString) !=
                pure_value_dummy_arguments.end()) {
              features["Removal of anomalies regarding pure procedures"] = true;
            }
          } else if (const auto *ar{
                         std::get_if<common::Indirection<parser::ArrayElement>>(
                             &dr->u)}) {
            const auto &base{ar->value().base};
            if (const auto *name{std::get_if<parser::Name>(&base.u)}) {
              CONVERT2LOWERCASE(name->ToString(), nameString);
              if (pure_value_dummy_arguments.find(nameString) !=
                  pure_value_dummy_arguments.end()) {
                features["Removal of anomalies regarding pure procedures"] =
                    true;
              }
            }
          }
        } else if (const auto *substr{
                       std::get_if<parser::Substring>(&dsn->value().u)}) {
          const auto &dref{std::get<parser::DataRef>(substr->t)};
          if (const auto *name{std::get_if<parser::Name>(&dref.u)}) {
            CONVERT2LOWERCASE(name->ToString(), nameString);
            if (pure_value_dummy_arguments.find(nameString) !=
                pure_value_dummy_arguments.end()) {
              features["Removal of anomalies regarding pure procedures"] = true;
            }
          }
        }
      }
    }
  }
}

///////////////////////
// Utility Functions //
///////////////////////

void FeatureCharacterization::checkMap(const char *key, bool addComma) {
  auto itr = features.find(key);
  out_ << "\t\"" << key << "\": ";
  if (itr != features.end() && itr->second) {
    out_ << "true";
  } else {
    out_ << "false";
  }

  if (addComma) {
    out_ << ",";
  }
  out_ << "\n";
}
void FeatureCharacterization::setMap(const char *key, bool val) {
  auto itr = features.find(key);
  if (itr != features.end()) {
    features[key] = val;
  }
}

void FeatureCharacterization::checkAllFeatures() {
  out_ << "Fortran 95's New Features {\n";
  checkMap("Forall statements");
  checkMap("Forall constructs");
  checkMap("Enhancements to WHERE");
  checkMap("Initialization of pointers with NULL function");
  checkMap("Default initialization of derived types");
  checkMap("Pure procedures");
  checkMap("Elemental procedures");
  // checkMap("Automatic deallocation of allocatable arrays");
  checkMap("New and enhanced intrinsic procedures");
  out_ << "}\n";

  out_ << "Fortran 2003's New Features {\n";
  checkMap("Procedure pointers");
  checkMap("Parameterized derived types");
  checkMap("Finalization");
  checkMap("Procedures bound by name to a type (type-bound procedures)");
  checkMap("The PASS attribute");
  checkMap("Procedures bound to a type as operators");
  checkMap("Type extension");
  // discuss why we need semantics here and why we won't be including it
  // anymore checkMap("Overriding a type-bound procedure");
  checkMap("Enumerations");
  checkMap("ASSOCIATE construct");
  checkMap("Polymorphic entities");
  checkMap("SELECT TYPE construct");
  checkMap("Deferred bindings and abstract types");
  checkMap("Structure constructors");
  checkMap("The allocate statement (allocate with SOURCE)");
  checkMap("Allocatable scalars"); // Semantics
  checkMap("Allocatable character length"); // Semantics
  // checkMap("Assignment to an allocatable array");
  // checkMap("Transferring an allocation");
  checkMap("More control of access from a module");
  checkMap("Renaming operators on the USE statement");
  checkMap("Pointer assignment (rank remapping)");
  // checkMap("Pointer INTENT");
  // checkMap("VOLATILE attribute");
  checkMap("The IMPORT statement");
  checkMap("Intrinsic modules");
  checkMap("Access to the computing environment (Command line processing)");
  // checkMap("Support for international character sets");
  // checkMap("Binary, octal and hex constants");
  // checkMap("Lengths of names and statements")
  // checkMap("Array constructor syntax");
  // checkMap("Specification and initialization expressions");
  // checkMap("Complex constants");
  // checkMap("Changes to intrinsic functions");
  // checkMap("Controlling IEEE underflow");
  // checkMap("Another IEEE class value");
  checkMap("Derived type I/O");
  // checkMap("Asynchronous I/O");
  // checkMap("FLUSH statement");
  // checkMap("IOMSG= spcifier");
  // checkMap("Stream access I/O");
  // checkMap("ROUND= spcifier");
  // checkMap("DECIMAL= spcifier");
  // checkMap("SIGN= spcifier");
  // checkMap("Kind type parameters of integer specifiers");
  // checkMap("Recursive I/O");
  // checkMap("Intrinsic function for newline character");
  // checkMap("I/O of IEEE exceptional values");
  // checkMap("Comma after a P edit descriptor");
  checkMap("Interoperability of intrinsic types");
  checkMap("Interoperability with C pointers");
  checkMap("Interoperability of derived types");
  checkMap("Interoperability of variables");
  checkMap("Interoperability of procedures");
  checkMap("Interoperability of global data");
  out_ << "}\n";

  out_ << "Fortran 2008's New Features {\n";
  checkMap("Submodules");
  // checkMap("Coarrays");
  checkMap("do concurrent");
  checkMap("Contiguous attribute");
  // checkMap("Simply contiguous arrays rank remapping to rank>1 target");
  // checkMap("Maximum rank");
  checkMap("Long integers");
  // checkMap("Allocatable components of recursive type");
  // checkMap("Implied-shape array");
  checkMap("Pointer initialization with SAVE attribute");
  checkMap("Kind of a forall index");
  // checkMap("Type statement for intrinsic types");
  // checkMap("Declaring type-bound procedures");
  // checkMap("Extensions to value attribute");
  // checkMap("Omitting an allocatable component in a structure constructor");
  // checkMap("Multiple allocations with source=");
  checkMap("Allocating a polymorphic variable using MOLD");
  // checkMap("Copying bounds of source array in ALLOCATE");
  // checkMap("Polymorphic assignment");
  // checkMap("Accessing real and imaginary parts");
  // checkMap("Pointer functions");
  // checkMap("Elemental dummy argument restrictions lifted");
  checkMap("Finding a unit when opening a file (newunit=u)");
  // checkMap("g0 edit descriptor");
  // checkMap("Unlimited format item");
  // checkMap("Recursive I/O for an external unit");
  checkMap("The BLOCK construct");
  checkMap("Exit statement allowed in almost any construct");
  // checkMap("STOP code");
  // checkMap("Bit sequence comparison");
  // checkMap("Combined shifting");
  // checkMap("Counting bits");
  // checkMap("Masking bits");
  // checkMap("Shifting bits");
  // checkMap("Merging bits");
  // checkMap("Bit transformational functions");
  checkMap("Storage size");
  // checkMap("Optional argument radix added to selected real kind");
  // checkMap("Extensions to trigonometric and hyperbolic intrinsic
  // functions"); checkMap("Bessel functions"); checkMap("Error and gamma
  // functions"); checkMap("Euclidean vector norms"); checkMap("Parity");
  // checkMap("Execute command line");
  checkMap("Optional back argument added to maxloc and minloc");
  // checkMap("Find location in an array");
  // checkMap("String comparison");
  checkMap("Constants in ISO_FORTRAN_ENV");
  checkMap("Compiler information in ISO_FORTRAN_ENV");
  checkMap("Function for C sizeof");
  // checkMap("Optional argument for ieee_selected_real_kind");
  // checkMap("Save attribute for module and submodule data");
  // checkMap("Empty contains part");
  // checkMap("Form of the end statement for an internal or module
  // procedure"); checkMap("Internal procedure as an actual argument");
  // checkMap(
  //    "Null pointer or unallocated allocatable as an absent dummy
  //    argument");
  // checkMap("Non-pointer actual for pointer dummy argument");
  checkMap("Impure elemental procedures");
  // checkMap("Generic resolution by procedureness");
  // checkMap("Generic resolution by pointer vs. allocatable");
  out_ << "}\n";

  out_ << "Fortran 2018's New Features {\n";
  checkMap("C descriptors");
  checkMap("Attribute codes");
  checkMap("The type CFI_dim_t");
  checkMap("Assumed rank");
  checkMap("SELECT RANK");
  checkMap("Assumed-size arrays");
  checkMap("Assumed type");
  checkMap("Contiguous attribute for assumed-rank arrays");
  checkMap("Default accessibility for entities accessed from a module");
  checkMap("Implicit none enhancement");
  checkMap("Changes to Intrinsics that access the computing environment");
  checkMap("New reduction intrinsic reduce");
  checkMap("Intrinsic function coshape");
  checkMap("Intrinsic subroutine random_init");
  checkMap("Kind of the do variable in implied do");
  checkMap("Locality clauses in do concurrent");
  checkMap("Control of host association");
  checkMap("The value attribute for an argument of a defined operation or "
           "assignment");
  checkMap("Removal of anomalies regarding pure procedures");
  checkMap("Simplification of calls of the intrinsic cmplx");
  checkMap(
      "Removal of the restriction on argument dim of many intrinsic functions");
  checkMap("Recursive and non-recursive procedures");
  out_ << "}\n";
}
