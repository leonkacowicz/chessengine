#include <iostream>
#include <bitboard.h>
#include <magic_bitboard.h>
#include "settings.h"
#include "process.h"
#include "player.h"
#include "arbiter.h"

game_settings parse_cmd_line(int argc, char ** argv) {
    game_settings gs = {};
    gs.valid = false;

    for (int i = 1; i < argc - 1; i++) {
        std::string current(argv[i]);
        std::string next(argv[i + 1]);

        try {
            if (current == "--white-exec") gs.white_settings.executable = next;
            else if (current == "--white-input") gs.white_settings.input_filename = next;
            else if (current == "--white-init-time") gs.white_settings.initial_time = std::chrono::milliseconds(std::stoi(next));
            else if (current == "--white-time-increment") gs.white_settings.time_increment = std::chrono::milliseconds(std::stoi(next));
            else if (current == "--white-move-time") gs.white_settings.move_time = std::chrono::milliseconds(std::stoi(next));
            else if (current == "--black-exec") gs.black_settings.executable = next;
            else if (current == "--black-input") gs.black_settings.input_filename = next;
            else if (current == "--black-init-time") gs.black_settings.initial_time = std::chrono::milliseconds(std::stoi(next));
            else if (current == "--black-time-increment") gs.black_settings.time_increment = std::chrono::milliseconds(std::stoi(next));
            else if (current == "--black-move-time") gs.black_settings.move_time = std::chrono::milliseconds(std::stoi(next));
            else if (current == "--initial-pos") gs.initial_position = next;
            else if (current == "--verbose") { gs.verbose = true; continue; }
            else return gs;
            i++;
        } catch (std::invalid_argument & e) {
            return gs;
        }
    }
    gs.valid = true;
    return gs;
}

void print_usage() {
    std::cout << "Chess Arbiter" << std::endl;
    std::cout << "Usage: chessarbiter OPTIONS " << std::endl << std::endl;
    std::cout << "     --white-exec                path to white pieces engine executable" << std::endl;
    std::cout << "     --white-input               [optional] path to white engine input options file" << std::endl;
    std::cout << "     --white-init-time           [optional] initial white time in milliseconds" << std::endl;
    std::cout << "     --white-time-increment      [optional] white time increment per move in milliseconds" << std::endl;
    std::cout << "     --white-move-time           [optional] white time per move in milliseconds" << std::endl;
    std::cout << "     --black-exec                path to black pieces engine executable" << std::endl;
    std::cout << "     --black-input               [optional] path to black engine input options file" << std::endl;
    std::cout << "     --black-init-time           [optional] initial black time in milliseconds" << std::endl;
    std::cout << "     --black-time-increment      [optional] black time increment per move in milliseconds" << std::endl;
    std::cout << "     --black-move-time           [optional] black time per move in milliseconds" << std::endl;
    std::cout << "     --initial-pos               [optional] initial position in fen notation or in list of moves since start position" << std::endl;
    std::cout << "     --verbose                   [optional] enables verbose mode" << std::endl;
    std::cout << std::endl;
}

void print_settings(const game_settings& gs) {
    std::cout << "Using settings:" << std::endl;
    std::cout << "     --white-exec              = " << gs.white_settings.executable << std::endl;
    std::cout << "     --white-input             = " << gs.white_settings.input_filename << std::endl;
    std::cout << "     --white-init-time         = " << gs.white_settings.initial_time.count() << std::endl;
    std::cout << "     --white-time-increment    = " << gs.white_settings.time_increment.count() << std::endl;
    std::cout << "     --white-move-time         = " << gs.white_settings.move_time.count() << std::endl;
    std::cout << "     --black-exec              = " << gs.black_settings.executable << std::endl;
    std::cout << "     --black-input             = " << gs.black_settings.input_filename << std::endl;
    std::cout << "     --black-init-time         = " << gs.black_settings.initial_time.count() << std::endl;
    std::cout << "     --black-time-increment    = " << gs.black_settings.time_increment.count() << std::endl;
    std::cout << "     --black-move-time         = " << gs.black_settings.move_time.count() << std::endl;
    std::cout << "     --initial-pos             = " << gs.initial_position << std::endl;
}

std::string file_contents(const std::string & filepath) {
    if (filepath.empty()) return "";
    std::ifstream t(filepath);
    return std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

int main(int argc, char ** argv) {
    chess::core::init_bitboards();
    chess::core::init_magic_bitboards();

    const game_settings& settings = parse_cmd_line(argc, argv);
    if (!settings.valid) {
        print_usage();
        return 1;
    }

    if (settings.verbose) {
        print_settings(settings);
    }

    process white_proc(settings.white_settings.executable);
    process black_proc(settings.black_settings.executable);
    player white(WHITE, white_proc.stdin, white_proc.stdout);
    player black(BLACK, black_proc.stdin, black_proc.stdout);
    //arbiter arb(white, black, settings.white_settings.initial_time, settings.white_settings.time_increment, settings.verbose);
    arbiter arb(white, black, settings);

    std::string white_options = file_contents(settings.white_settings.input_filename);
    std::string black_options = file_contents(settings.black_settings.input_filename);

    arb.start_players(white_options, black_options);
    arb.start_game();
}
