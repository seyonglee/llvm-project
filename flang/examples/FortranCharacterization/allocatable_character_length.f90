function greet(b)

  logical, intent(in) :: b
  
  character(:), allocatable :: greet
  
  !! Manual allocation of character variable. This could be necessary on old or buggy compilers.
  
  if(b) then
    allocate(character(5) :: greet)
    greet = 'hello'
  else
    allocate(character(3) :: greet)
    greet = 'bye'
  endif
  
end function greet

! Program -> ProgramUnit -> FunctionSubprogram
! | FunctionStmt
! | | Name = 'greet'
! | | Name = 'b'
! | SpecificationPart
! | | ImplicitPart -> 
! | | DeclarationConstruct -> SpecificationConstruct -> TypeDeclarationStmt
! | | | DeclarationTypeSpec -> IntrinsicTypeSpec -> Logical
! | | | AttrSpec -> IntentSpec -> Intent = In
! | | | EntityDecl
! | | | | Name = 'b'
! | | DeclarationConstruct -> SpecificationConstruct -> TypeDeclarationStmt
! | | | DeclarationTypeSpec -> IntrinsicTypeSpec -> Character
! | | | | CharSelector -> LengthSelector -> TypeParamValue -> Deferred
! | | | AttrSpec -> Allocatable
! | | | EntityDecl
! | | | | Name = 'greet'
! | ExecutionPart -> Block
! | | ExecutionPartConstruct -> ExecutableConstruct -> IfConstruct
! | | | IfThenStmt
! | | | | Scalar -> Logical -> Expr = 'b'
! | | | | | Designator -> DataRef -> Name = 'b'
! | | | Block
! | | | | ExecutionPartConstruct -> ExecutableConstruct -> ActionStmt -> AllocateStmt
! | | | | | TypeSpec -> IntrinsicTypeSpec -> Character
! | | | | | | CharSelector -> LengthSelector -> TypeParamValue -> Scalar -> Integer -> Expr = '5_4'
! | | | | | | | LiteralConstant -> IntLiteralConstant = '5'
! | | | | | Allocation
! | | | | | | AllocateObject = 'greet'
! | | | | | | | Name = 'greet'
! | | | | ExecutionPartConstruct -> ExecutableConstruct -> ActionStmt -> AssignmentStmt = 'greet="hello"'
! | | | | | Variable = 'greet'
! | | | | | | Designator -> DataRef -> Name = 'greet'
! | | | | | Expr = '"hello"'
! | | | | | | LiteralConstant -> CharLiteralConstant
! | | | | | | | string = 'hello'
! | | | ElseBlock
! | | | | ElseStmt -> 
! | | | | Block
! | | | | | ExecutionPartConstruct -> ExecutableConstruct -> ActionStmt -> AllocateStmt
! | | | | | | TypeSpec -> IntrinsicTypeSpec -> Character
! | | | | | | | CharSelector -> LengthSelector -> TypeParamValue -> Scalar -> Integer -> Expr = '3_4'
! | | | | | | | | LiteralConstant -> IntLiteralConstant = '3'
! | | | | | | Allocation
! | | | | | | | AllocateObject = 'greet'
! | | | | | | | | Name = 'greet'
! | | | | | ExecutionPartConstruct -> ExecutableConstruct -> ActionStmt -> AssignmentStmt = 'greet="bye"'
! | | | | | | Variable = 'greet'
! | | | | | | | Designator -> DataRef -> Name = 'greet'
! | | | | | | Expr = '"bye"'
! | | | | | | | LiteralConstant -> CharLiteralConstant
! | | | | | | | | string = 'bye'
! | | | EndIfStmt -> 
! | EndFunctionStmt -> Name = 'greet'