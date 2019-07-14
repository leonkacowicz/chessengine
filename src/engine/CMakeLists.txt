set(engine_src
        Board.h Board.cpp
        BitBoard.cpp BitBoard.h
        Square.cpp Square.h
        Move.cpp Move.h
        ../chess/color.cpp ../chess/color.h
        Piece.h)
add_library(engine STATIC ${engine_src})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})