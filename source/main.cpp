// download-mgr.cpp : Defines the entry point for the console application.
//

#include "download-mgr.h"
#include <iostream>
#include <iomanip>


namespace dl_mgr {
    //TODO: add recursive directory sorting
    /**
     * \brief Runs the program.
     * \param config The configuration to be used during execution.
     * \param path If provided, overrides the source directory read from the config.
     */
    void run(config_t &config, const std::string &path = "")
        {
        for (auto &p : dir_it(path.empty()
                                  ? config.SourceDir
                                  : path))
            if (is_regular_file(p))
                rename(p, config.
                          Map.resolve_tgt(p));
        }

    /**
     * \brief Checks which category a command line argument belongs to.
     *  Additionally, resolves the value of the argument and stores
     *  it in `arg`.
     * \param[in/out] arg A string containing the option to check.
     * \return A value from the `options` enum.
     */
    options check_option(const std::string &arg)
        {
        if (arg.empty()) return NONE;
        if (arg == "-h" || arg == "--help" || arg == "/?"
            )
            return HELP;
        if (arg == "cfg" || arg == "configure") return CFG;
        if (arg == "add") return ADD;
        if (arg == "--src") return SRC;
        /*if (arg.find("--src=") == 0)
            {
            arg = arg.substr(5);
            return SRC;
            }*/
        return NONE;
        }

    /**
     * \brief Prints the help information to the console.
     */
    void show_help()
        {
        std::cout << "Usage:" << std::endl
            << "download-mgr [--src=<dir>]" << std::endl
            << "download-mgr cfg [--src=<dir>]" << std::endl
            << "download-mgr add <extension> <tgt>" << std::
            endl
            << std::endl << "Options:" << std::endl
            << std::setw(15) << "--src=<dir>"
            << "Path to source directory." << std::endl;
        }

    /**
     * \brief Finishes up execution of the program.
     * \param wait Wait for user input to end the program.
     * \param help Display help information before ending the program.
     * \param err Print an error message before ending the program, with
     *  an optional detail message afterwards.
     * \param err_msg The error detail message to print if `err` is `true`.
     * \return An integer indicating whether or not the program completed without errors.
     */
    int finish
        (const bool wait = false, const bool help = false,
         const bool err = false,
         const std::string &err_msg = "")
        {
        if (err)
            std::cout << application_name <<
                " encountered an error executing the command."
                << std::endl << (err_msg.empty()
                                     ? "."
                                     : ": \n" + err_msg) <<
                std::endl;
        if (help) show_help();
        if (wait)
            {
            std::cout << "Press any key to continue...";
            getchar();
            }
        return err;
        }
}


int main(const size_t argc, const char **argv)
    {
    using namespace dl_mgr;

    if (!exists(fs::current_path() / config_file))
        return finish(true, true, true,
                      "Config file not found.");

    program prog;

    if (argc == 1)
        {
        prog.run();
        return finish(true);
        }

    switch (check_option(argv[1]))
        {
        case HELP:
            return finish(false, true);
        case CFG:
            if (argc == 4 && check_option(argv[2]) == SRC)
                prog.set_src_dir(argv[3]);
            else
                return finish(false, true, true,
                              "Incorrect number of arguments.");
            break;
        case ADD:
            if (argc == 4) prog.edit_ext_entry(
                argv[2], argv[3]);
            else
                return finish(false, true, true,
                              "Incorrect number of arguments.");
            break;
        case SRC:
            prog.run(argv[2]);
            break;
        default:
            return finish();
        }
    return finish(true);
    }
