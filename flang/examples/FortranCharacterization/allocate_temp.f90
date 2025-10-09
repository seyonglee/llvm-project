! RUN: %python %S/test_errors.py %s %flang_fc1
! Check for semantic errors in ALLOCATE statements

subroutine C933_a(ca3, ca4)
    character(len=:), allocatable :: ca1, ca2(:)
    character(len=*), allocatable :: ca3, ca4(:)
    character(len=2), allocatable :: ca5, ca6(:)
    character(len=5) mold
  
end subroutine

! /Program -> ProgramUnit -> SubroutineSubprogram
! | SubroutineStmt
! | | Name = 'c933_a'
! | | DummyArg -> Name = 'ca3'
! | | DummyArg -> Name = 'ca4'
! | SpecificationPart
! | | ImplicitPart -> 
! | | DeclarationConstruct -> SpecificationConstruct -> TypeDeclarationStmt
! | | | DeclarationTypeSpec -> IntrinsicTypeSpec -> Character
! | | | | CharSelector -> LengthSelector -> TypeParamValue -> Deferred
! | | | AttrSpec -> Allocatable
! | | | EntityDecl
! | | | | Name = 'ca1'
! | | | EntityDecl
! | | | | Name = 'ca2'
! | | | | ArraySpec -> DeferredShapeSpecList -> int
! | | DeclarationConstruct -> SpecificationConstruct -> TypeDeclarationStmt
! | | | DeclarationTypeSpec -> IntrinsicTypeSpec -> Character
! | | | | CharSelector -> LengthSelector -> TypeParamValue -> Star
! | | | AttrSpec -> Allocatable
! | | | EntityDecl
! | | | | Name = 'ca3'
! | | | EntityDecl
! | | | | Name = 'ca4'
! | | | | ArraySpec -> DeferredShapeSpecList -> int
! | | DeclarationConstruct -> SpecificationConstruct -> TypeDeclarationStmt
! | | | DeclarationTypeSpec -> IntrinsicTypeSpec -> Character
! | | | | CharSelector -> LengthSelector -> TypeParamValue -> Scalar -> Integer -> Expr = '2_4'
! | | | | | LiteralConstant -> IntLiteralConstant = '2'
! | | | AttrSpec -> Allocatable
! | | | EntityDecl
! | | | | Name = 'ca5'
! | | | EntityDecl
! | | | | Name = 'ca6'
! | | | | ArraySpec -> DeferredShapeSpecList -> int
! | | DeclarationConstruct -> SpecificationConstruct -> TypeDeclarationStmt
! | | | DeclarationTypeSpec -> IntrinsicTypeSpec -> Character
! | | | | CharSelector -> LengthSelector -> TypeParamValue -> Scalar -> Integer -> Expr = '5_4'
! | | | | | LiteralConstant -> IntLiteralConstant = '5'
! | | | EntityDecl
! | | | | Name = 'mold'
! | ExecutionPart -> Block
! | EndSubroutineStmt -> 