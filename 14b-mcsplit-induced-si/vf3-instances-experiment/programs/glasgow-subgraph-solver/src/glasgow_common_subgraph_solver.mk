TARGET := glasgow_common_subgraph_solver

SOURCES := \
    glasgow_common_subgraph_solver.cc

TGT_PREREQS := libcommon.a
ifeq ($(shell uname -s), Linux)
TGT_LDLIBS := libcommon.a $(boost_ldlibs) -lstdc++fs
else
TGT_LDLIBS := libcommon.a $(boost_ldlibs)
endif

