#------------------------------------------------------------------------------#
# Distributed under the OSI-approved Apache License, Version 2.0.  See
# accompanying file Copyright.txt for details.
#------------------------------------------------------------------------------#

add_executable(julea_heatTransfer_queryAdios
  queryAdios.cpp 
  QueryPrintDataStep.h 
  QuerySettings.cpp
)
target_link_libraries(julea_heatTransfer_queryAdios adios2::cxx11_mpi MPI::MPI_C)
