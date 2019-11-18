#include <iostream>
#include <boost/program_options.hpp>
#include <core.h>
#include "settings.h"
#include "process.h"
#include "player.h"
#include "arbiter.h"

game_settings parse_cmd_line(int argc, char** argv) {
    using namespace boost::program_options;
    using std::chrono::milliseconds;

    game_settings gs = {};
    gs.valid = false;
    try {
        options_description options;
        options.add_options()
                ("help,h", "print usage message")
                ("white-exec", value<std::string>()->required(), "[REQUIRED] path to engine executable that will play with white pieces")
                ("white-input", value<std::string>()->default_value(""), "path to white options input file")
                ("white-init-time", value<int>()->default_value(0), "initial time for white in milliseconds")
                ("white-move-time", value<int>()->default_value(0), "move time for white in milliseconds")
                ("white-time-increment", value<int>()->default_value(0), "time increment per move for white in milliseconds")
                ("black-exec", value<std::string>()->required(), "[REQUIRED] path to engine executable that will play with black pieces")
                ("black-input", value<std::string>()->default_value(""), "path to black options input file")
                ("black-init-time", value<int>()->default_value(0), "initial time for black in milliseconds")
                ("black-move-time", value<int>()->default_value(0), "move time for white in milliseconds")
                ("black-time-increment", value<int>()->default_value(0), "time increment per move for black in milliseconds")
                ("output", value<std::string>()->default_value(""), "location of pgn output file")
                ("initial-pos", value<std::string>()->default_value("startpos"),
                 "string representing initial position either as 'startpos moves e2e4 e7e5 ...' or as 'fen fen-value moves m1 m2'")
                ("verbose", "enables verbose mode");
        variables_map variables;
        store(parse_command_line(argc, argv, options), variables);
        if (variables.count("help")) {
            std::cout << options << std::endl;
            std::exit(0);
        }
        if (variables.count("white-exec") == 0 || variables.count("black-exec") == 0)
            throw std::logic_error("white-exec and black-exec parameters are required");
        gs.verbose = variables.count("verbose") > 0;
        gs.output_location = variables["output"].as<std::string>();
        gs.initial_position = variables["initial-pos"].as<std::string>();
        gs.white_settings.executable = variables["white-exec"].as<std::string>();
        gs.white_settings.input_filename = variables["white-input"].as<std::string>();
        gs.white_settings.initial_time = milliseconds(variables["white-init-time"].as<int>());
        gs.white_settings.move_time = milliseconds(variables["white-move-time"].as<int>());
        gs.white_settings.time_increment = milliseconds(variables["white-time-increment"].as<int>());

        gs.black_settings.executable = variables["black-exec"].as<std::string>();
        gs.black_settings.input_filename = variables["black-input"].as<std::string>();
        gs.black_settings.initial_time = milliseconds(variables["black-init-time"].as<int>());
        gs.black_settings.move_time = milliseconds(variables["black-move-time"].as<int>());
        gs.black_settings.time_increment = milliseconds(variables["black-time-increment"].as<int>());

        gs.valid = true;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "run  `chessarbiter --help` for more info" << std::endl;
    }
    return gs;
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
    std::cout << "     --output                  = " << gs.output_location << std::endl;
    std::cout << "     --initial-pos             = " << gs.initial_position << std::endl;
}

std::string file_contents(const std::string& filepath) {
    if (filepath.empty()) return "";
    std::ifstream t(filepath);
    return std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

int main(int argc, char** argv) {
    chess::core::init();
    const game_settings& settings = parse_cmd_line(argc, argv);
    if (!settings.valid) return 1;
    if (settings.verbose) print_settings(settings);

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
