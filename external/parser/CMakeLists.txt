add_library(parser
        parser/Bchart.cpp
        parser/BchartSm.cpp
        parser/Bst.cpp
        parser/ChartBase.cpp
        parser/ClassRule.cpp
        parser/CntxArray.cpp
        parser/CombineBests.cpp
        parser/ECArgs.cpp
        parser/Edge.cpp
        parser/EdgeHeap.cpp
        parser/edgeSubFns.cpp
        parser/EgsFromTree.cpp
        parser/ewDciTokStrm.cpp
        parser/extraMain.cpp
        parser/FBinaryArray.cpp
        parser/Feat.cpp
        parser/Feature.cpp
        parser/FeatureTree.cpp
        parser/fhSubFns.cpp
        parser/Field.cpp
        parser/FullHist.cpp
        parser/GotIter.cpp
        parser/headFinder.cpp
        parser/headFinderCh.cpp
        parser/InputTree.cpp
        parser/Item.cpp
        parser/Link.cpp
        parser/MeChart.cpp
        parser/Params.cpp
        parser/ParseStats.cpp
        parser/SentRep.cpp
        parser/Term.cpp
        parser/TimeIt.cpp
        parser/UnitRules.cpp
        parser/utils.cpp
        parser/ValHeap.cpp)

target_include_directories(parser PUBLIC
        .)
