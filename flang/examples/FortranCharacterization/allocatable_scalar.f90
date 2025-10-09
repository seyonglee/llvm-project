program test_allocated
  !use m06
  integer :: i = 100 

  ! error! allocatable object cannot be initialized
  !integer, allocatable :: j = 4

  !real(4), allocatable :: x_array(:)
  !if (.not. allocated(x_array)) allocate(x_array(i))

  real, allocatable :: x
  if (.not. allocated(x)) allocate(x)

end program test_allocated

!! array
! Program -> ProgramUnit -> MainProgram
! | ProgramStmt -> Name = 'test_allocated'
! | SpecificationPart
! | | ImplicitPart -> 
! | | DeclarationConstruct -> SpecificationConstruct -> TypeDeclarationStmt
! | | | DeclarationTypeSpec -> IntrinsicTypeSpec -> IntegerTypeSpec -> 
! | | | EntityDecl
! | | | | Name = 'i'
! | | | | Initialization -> Constant -> Expr = '100_4'
! | | | | | LiteralConstant -> IntLiteralConstant = '100'
! | | DeclarationConstruct -> SpecificationConstruct -> TypeDeclarationStmt
! | | | DeclarationTypeSpec -> IntrinsicTypeSpec -> Real
! | | | | KindSelector -> Scalar -> Integer -> Constant -> Expr = '4_4'
! | | | | | LiteralConstant -> IntLiteralConstant = '4'
! | | | AttrSpec -> Allocatable
! | | | EntityDecl
! | | | | Name = 'x_array'
! | | | | ArraySpec -> DeferredShapeSpecList -> int
! | ExecutionPart -> Block
! | | ExecutionPartConstruct -> ExecutableConstruct -> ActionStmt -> IfStmt
! | | | Scalar -> Logical -> Expr = '.NOT.allocated(x_array)'
! | | | | NOT -> Expr = 'allocated(x_array)'
! | | | | | FunctionReference -> Call
! | | | | | | ProcedureDesignator -> Name = 'allocated'
! | | | | | | ActualArgSpec
! | | | | | | | ActualArg -> Expr = 'x_array'
! | | | | | | | | Designator -> DataRef -> Name = 'x_array'
! | | | ActionStmt -> AllocateStmt
! | | | | Allocation
! | | | | | AllocateObject = 'x_array'
! | | | | | | Name = 'x_array'
! | | | | | AllocateShapeSpec
! | | | | | | Scalar -> Integer -> Expr = 'i'
! | | | | | | | Designator -> DataRef -> Name = 'i'
! | EndProgramStmt -> Name = 'test_allocated'

!! scalar
! Program -> ProgramUnit -> MainProgram
! | ProgramStmt -> Name = 'test_allocated'
! | SpecificationPart
! | | ImplicitPart -> 
! | | DeclarationConstruct -> SpecificationConstruct -> TypeDeclarationStmt
! | | | DeclarationTypeSpec -> IntrinsicTypeSpec -> IntegerTypeSpec -> 
! | | | EntityDecl
! | | | | Name = 'i'
! | | | | Initialization -> Constant -> Expr = '100_4'
! | | | | | LiteralConstant -> IntLiteralConstant = '100'
! | | DeclarationConstruct -> SpecificationConstruct -> TypeDeclarationStmt
! | | | DeclarationTypeSpec -> IntrinsicTypeSpec -> Real
! | | | AttrSpec -> Allocatable
! | | | EntityDecl
! | | | | Name = 'x'
! | ExecutionPart -> Block
! | | ExecutionPartConstruct -> ExecutableConstruct -> ActionStmt -> IfStmt
! | | | Scalar -> Logical -> Expr = '.NOT.allocated(x)'
! | | | | NOT -> Expr = 'allocated(x)'
! | | | | | FunctionReference -> Call
! | | | | | | ProcedureDesignator -> Name = 'allocated'
! | | | | | | ActualArgSpec
! | | | | | | | ActualArg -> Expr = 'x'
! | | | | | | | | Designator -> DataRef -> Name = 'x'
! | | | ActionStmt -> AllocateStmt
! | | | | Allocation
! | | | | | AllocateObject = 'x'
! | | | | | | Name = 'x'
! | EndProgramStmt -> Name = 'test_allocated' 