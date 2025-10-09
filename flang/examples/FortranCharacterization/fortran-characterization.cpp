// TODO: create a template where we check for a given feature in the map by
// providing const char* argument instead of creating a new function every time

// attributes of interest
#include "fortran-characterization.h"

std::unordered_map<const char *, bool> FeatureCharacterization::features{
    {"Parameterized derived types", false}, {"Procedure pointers", false},
    {"Finalization", false},
    {"Procedures bound by name to a type (type-bound procedures)", false},
    {"The PASS attribute", false},
    {"Procedures bound to a type as operators", false},
    {"Type extension", false}, {"Overriding a type-bound procedure", false},
    {"Enumerations", false}, {"ASSOCIATE construct", false},
    {"Polymorphic entities", false}, {"SELECT TYPE construct", false},
    {"Deferred bindings and abstract types", false},
    {"Allocatable scalars", false}, {"Allocatable character length", false},
    {"The allocate statement (allocate with SOURCE)", false},
    {"More control of access from a module", false},
    {"Renaming operators on the USE statement", false},
    {"Pointer assignment (rank remapping)", false},
    {"The IMPORT statement", false},
    {"Access to the computing environment (Command line processing)", false},
    {"Array constructor syntax", false},
    {"Specification and initialization expressions", false},
    {"Interoperability of intrinsic types", false},
    {"Interoperability with C pointers", false},
    {"Interoperability of derived types", false},
    {"Interoperability of variables", false},
    {"Interoperability of procedures", false},
    {"Interoperability of global data", false}, {"Submodules", false},
    {"do concurrent", false}, {"Contiguous attribute", false},
    {"Long integers", false}, {"Pointer initialization", false},
    {"Allocating a polymorphic variable (e.g. using MOLD= or SOURCE=)", false},
    {"Simply contiguous arrays rank remapping to rank>1 target", false},
    {"Copying the properties of an object in an ALLOCATE statement", false},
    {"MOLD= specifier for ALLOCATE", false},
    {"Copying bounds of source array in ALLOCATE", false},
    {"Finding a unit when opening a file (newunit=u)", false},
    {"The BLOCK construct", false},
    {"Exit statement allowed in almost any construct", false},
    {"Storage size", false},
    {"Optional back argument added to maxloc and minloc", false},
    {"Constants in ISO_FORTRAN_ENV", false}, {"Function for C sizeof", false},
    {"Save attribute for module and submodule data", false},
    {"Internal procedure as an actual argument", false},
    {"Generic resolution by procedureness", false},
    {"Generic resolution by pointer vs. allocatable", false},
    {"Impure elemental procedures", false}, {"Derived type I/O", false}};

void FeatureCharacterization::Post(const parser::Pass &) {
  features["The PASS attribute"] = true;
}
void FeatureCharacterization::Post(const parser::DoConstruct &node) {
  features["do concurrent"] = node.IsDoConcurrent();
}
void FeatureCharacterization::Post(const parser::FinalProcedureStmt &node) {
  features["Finalization"] = true;
}

void FeatureCharacterization::Post(const parser::AssociateConstruct &) {
  features["ASSOCIATE construct"] = true;
}

void FeatureCharacterization::Post(const parser::SelectTypeConstruct &) {
  features["SELECT TYPE construct"] = true;
}

void FeatureCharacterization::Post(const parser::TypeParamDefStmt &) {
  features["Parameterized derived types"] = true;
}
void FeatureCharacterization::Post(const parser::TypeParamSpec &) {
  features["Parameterized derived types"] = true;
}

void FeatureCharacterization::Post(
    const parser::ProcedureDeclarationStmt &pds) {
  const auto &procAttrSpec{std::get<std::list<parser::ProcAttrSpec>>(pds.t)};
  for (const parser::ProcAttrSpec &procAttr : procAttrSpec) {
    // if (std::get_if<parser::Pointer>(&procAttr.u))
    if (std::get_if<parser::Pointer>(&procAttr.u))
      features["Procedure pointers"] = true;
    break;
  }
}

void FeatureCharacterization::Post(const parser::TypeAttrSpec &tas) {
  if (std::get_if<parser::TypeAttrSpec::Extends>(&tas.u))
    features["Type extension"] = true;
  else if (std::get_if<parser::Abstract>(&tas.u))
    features["Deferred bindings and abstract types"] = true;
}

void FeatureCharacterization::Post(const parser::BindAttr &ba) {
  if (std::get_if<parser::BindAttr::Deferred>(&ba.u))
    features["Deferred bindings and abstract types"] = true;
}

void FeatureCharacterization::Post(const parser::TypeBoundGenericStmt &tbgs) {
  const auto &genericSpec{std::get<Indirection<GenericSpec>>(tbgs.t)};
  if (std::get_if<parser::DefinedOperator>(&genericSpec.value().u)) {
    features["Procedures bound to a type as operators"] = true;
  } else if (std::get_if<GenericSpec::Assignment>(&genericSpec.value().u))
    features["Procedures bound to a type as operators"] = true;
}

void FeatureCharacterization::Post(const parser::EnumDef &ed) {
  features["Enumerations"] = true;
}

void FeatureCharacterization::Post(const parser::DeclarationTypeSpec &dts) {
  if (const auto *dtsClass{
          std::get_if<parser::DeclarationTypeSpec::Class>(&dts.u)})
    features["Polymorphic entities"] = true;
  else if (const auto *dtsClass{
               std::get_if<parser::DeclarationTypeSpec::ClassStar>(&dts.u)})
    features["Polymorphic entities"] = true;
}

void FeatureCharacterization::Post(const parser::TypeDeclarationStmt &tds) {
  const auto &dts{std::get<parser::DeclarationTypeSpec>(tds.t)};
  const auto &attrSpecList{std::get<std::list<parser::AttrSpec>>(tds.t)};
  const auto &entityDeclList{std::get<std::list<parser::EntityDecl>>(tds.t)};

  // check AttrSpecList to see if Allocatable is in there
  bool allocatableSpec{false};
  for (const parser::AttrSpec &attrSpec : attrSpecList) {
    if (std::holds_alternative<parser::Allocatable>(attrSpec.u)) {
      allocatableSpec = true;
      break;
    }
  }
  if (!allocatableSpec)
    return;

  // if there is an arrayspec, it's not a scalar
  for (const parser::EntityDecl &ed : entityDeclList) {
    // const auto &arrSpec{std::get<parser::ArraySpec>(ed)};
    if (std::get<std::optional<parser::ArraySpec>>(ed.t).has_value())
      return;
  }

  // check DeclarationTypeSpec to see if it's character type. we don't care
  // about that here.
  if (const auto &its{std::get_if<parser::IntrinsicTypeSpec>(&dts.u)}) {
    if (std::holds_alternative<parser::IntrinsicTypeSpec::Character>(its->u)) {
      features["Allocatable character length"] = true;
    } else {
      features["Allocatable scalars"] = true;
    }
  }
}

void FeatureCharacterization::Post(const parser::AllocateStmt &allocateStmt) {
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

void FeatureCharacterization::Post(const parser::ImportStmt &is) {
  features["The IMPORT statement"] = true;
}

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
            DeclTypeSpec::Category::Character)
          features["Allocatable character length"] = true;
        else {
          features["Allocatable scalars"] = true;
        }
      }
    }
  }
}

void FeatureCharacterization::Post(const parser::UseStmt &us) {
  if (const auto &renameList{std::get_if<std::list<Rename>>(&us.u)})
    if (renameList->begin() != renameList->end())
      features["Renaming operators on the USE statement"] = true;
  if (const auto &onlyList{std::get_if<std::list<Only>>(&us.u)}) {
    for (const Only &o : *onlyList) {
      if (std::holds_alternative<Rename>(o.u))
        features["Renaming operators on the USE statement"] = true;
    }
  }
}

void FeatureCharacterization::Post(const parser::PointerAssignmentStmt &pas) {
  const auto &bounds{std::get<PointerAssignmentStmt::Bounds>(pas.t)};
  if (const auto &brList{std::get_if<std::list<BoundsRemapping>>(&bounds.u)})
    if (!brList->empty())
      features["Pointer assignment (rank remapping)"] = true;
}

void FeatureCharacterization::Post(const parser::Call &c) {
  const auto &pd{std::get<ProcedureDesignator>(c.t)};
  if (const auto &name{std::get_if<Name>(&pd.u)}) {
    std::string fnName{name->ToString()};
    if (fnName == "get_command_argument" ||
        fnName == "command_argument_count" ||
        fnName == "get_environment_variable" || fnName == "get_command") {
      features
          ["Access to the computing environment (Command line processing)"] =
              true;
    }
  }
}

void FeatureCharacterization::Post(const parser::IoControlSpec &iocs) {
  if (const auto &format{std::get_if<Format>(&iocs.u)}) {
    if (const auto &expr{std::get_if<Expr>(&format->u)}) {
      if (const auto &ls{std::get_if<LiteralConstant>(&expr->u)}) {
        if (const auto &cls{std::get_if<CharLiteralConstant>(&ls->u)}) {
          std::string fmtString{cls->GetString()};
          if (fmtString.find("DT") != std::string::npos ||
              fmtString.find("dt") != std::string::npos ||
              fmtString.find("dT") != std::string::npos ||
              fmtString.find("Dt") != std::string::npos)
            features["Derived type I/O"] = true;
        }
      }
    }
  }
}

void FeatureCharacterization::Post(const parser::TypeBoundProcBinding &tbpb) {
  if (const auto &tbgs{std::get_if<TypeBoundGenericStmt>(&tbpb.u)}) {
    const auto &genericSpec{std::get<Indirection<GenericSpec>>(tbgs->t)};
    // if (const auto *definedOp{
    //         std::get_if<parser::DefinedOperator>(&genericSpec.value().u)})
    //   if (const auto *intrinsicOp{
    //           std::get_if<parser::DefinedOperator::IntrinsicOperator>(
    //               &(definedOp->u))})
    // }
    if (std::get_if<parser::GenericSpec::ReadFormatted>(
            &genericSpec.value().u) ||
        std::get_if<parser::GenericSpec::ReadUnformatted>(
            &genericSpec.value().u) ||
        std::get_if<parser::GenericSpec::WriteFormatted>(
            &genericSpec.value().u) ||
        std::get_if<parser::GenericSpec::WriteUnformatted>(
            &genericSpec.value().u))
      features["Derived type I/O"] = true;
  }
}

void FeatureCharacterization::Post(const parser::InterfaceStmt &is) {
  if (const auto &genericSpec{
          std::get_if<std::optional<parser::GenericSpec>>(&is.u)}) {
    if (genericSpec->has_value() &&
        (std::get_if<parser::GenericSpec::ReadFormatted>(
             &genericSpec->value().u) ||
            std::get_if<parser::GenericSpec::ReadUnformatted>(
                &genericSpec->value().u) ||
            std::get_if<parser::GenericSpec::WriteFormatted>(
                &genericSpec->value().u) ||
            std::get_if<parser::GenericSpec::WriteUnformatted>(
                &genericSpec->value().u)))
      features["Derived type I/O"] = true;
  }
}

void FeatureCharacterization::Post(const parser::Name &name) {
  auto n{name.ToString()};
  std::transform(n.begin(), n.end(), n.begin(),
      [](unsigned char c) { return std::tolower(c); });
  std::vector<std::string> possibilities{"c_ptr", "c_funptr", "c_null_funptr",
      "c_loc", "c_funloc", "c_associated", "c_f_pointer", "c_f_procpointer"};
  // if (n == "c_ptr" || "c_funptr" || "c_null_ptr")
  for (const auto &p : possibilities) {
    if (n == p)
      features["Interoperability with C pointers"] = true;
  }
}

void FeatureCharacterization::Post(const parser::TypeAttrSpec::BindC &) {
  features["Interoperability of derived types"] = true;
}

void FeatureCharacterization::Post(const parser::LanguageBindingSpec &) {
  features["Interoperability of derived types"] = true;
}

void FeatureCharacterization::checkMap(const char *key, bool addComma) {
  auto itr = features.find(key);
  out_ << "\t\"" << key << "\": ";
  if (itr != features.end() && itr->second)
    out_ << "true";
  else
    out_ << "false";

  if (addComma)
    out_ << ",";
  out_ << "\n";
}
void FeatureCharacterization::setMap(const char *key, bool val) {
  auto itr = features.find(key);
  if (itr != features.end())
    features[key] = val;
}

void FeatureCharacterization::checkAllFeatures() {
  out_ << "{\n";

  checkMap("The PASS attribute");
  checkMap("do concurrent");
  checkMap("Finalization");
  checkMap("ASSOCIATE construct");
  checkMap("SELECT TYPE construct");
  checkMap("Parameterized derived types");
  checkMap("Procedure pointers");
  checkMap("Type extension");
  checkMap("Deferred bindings and abstract types");
  checkMap("Procedures bound to a type as operators");
  checkMap("Enumerations");
  checkMap("Polymorphic entities");
  // checkMap("Allocatable scalars"); semantics
  // checkMap("Allocatable character length"); semantics
  // checkMap("The allocate statement (allocate with SOURCE)"); We could do this
  // one
  checkMap("The IMPORT statement");
  checkMap("Renaming operators on the USE statement");
  checkMap("Pointer assignment (rank remapping)");
  checkMap("Access to the computing environment (Command line processing)");
  checkMap("Derived type I/O");
  checkMap("Interoperability with C pointers", false);
  // discuss why we need semantics here and why we won't be including it anymore
  /* checkMap("Overriding a type-bound procedure");
  //checkMap(
  //    "Procedures bound by a name to a type (type-bound procedures)",
  false);*/

  out_ << "}\n";
}