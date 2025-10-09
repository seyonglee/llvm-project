add_llvm_example_library(fortranCharacterization
    MODULE
    fortran-characterization.cpp

    DEPENDS
    #acc_gen
    flangFrontend
    #omp_gen
    #FortranSemantics
)

target_include_directories(fortranCharacterization
    PUBLIC
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/../flang/lib/Semantics>
		$<INSTALL_INTERFACE:include/flang/Semantics> 
)

# I don't actually need this apparently
# Even including it doesn't do break anything...
#target_link_libraries(fortranCharacterization
#    FortranSemantics
#)
