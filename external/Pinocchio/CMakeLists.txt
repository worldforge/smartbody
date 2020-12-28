add_library(pinocchio
        Pinocchio/attachment.cpp
        Pinocchio/discretization.cpp
        Pinocchio/embedding.cpp
        Pinocchio/graphutils.cpp
        Pinocchio/indexer.cpp
        Pinocchio/intersector.cpp
        Pinocchio/lsqSolver.cpp
        Pinocchio/matrix.cpp
        Pinocchio/mesh.cpp
        Pinocchio/Pinocchio.cpp
        Pinocchio/pinocchioApi.cpp
        Pinocchio/refinement.cpp
        Pinocchio/skeleton.cpp
        Pinocchio/PinnocchioCallback.cpp)

target_include_directories(pinocchio PUBLIC
        .)