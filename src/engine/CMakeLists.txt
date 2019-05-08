set(engine_src
        Board.h Board.cpp
        BitBoard.cpp BitBoard.h
        SquarePosition.cpp SquarePosition.h
        Move.cpp Move.h
        Color.cpp Color.h
        Piece.h)
add_library(engine STATIC ${engine_src})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})