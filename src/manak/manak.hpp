/**
 * @file manak.hpp
 * @author Sumedh Ghaisas
 *
 * Standalone header for end user.
 */
#ifndef MANAK_HPP_INCLUDED
#define MANAK_HPP_INCLUDED

#include <functional>

#include <manak/benchmark_suit/set_env.hpp>
#include <manak/benchmark_suit/benchmark_suite.hpp>
#include <manak/benchmark_suit/run_tree.hpp>

#include <manak/util/cli.hpp>

namespace manak /** C++ Unit Benchmarking Library. **/
{

//! This function runs benchmarking module with given function. If main is set
//! to auto, this function will be called else it has to be called manually.
//! Returns status as int
//!
//! \return int
//!
int manak_benchmarking_main(std::function<bool()> init_func, int argc, char* argv[] )
{
  bool output_format_html = true;
  bool compare = false;

  if(manak::utils::cli::CLI::cmdOptionExists(argv, argv + argc, "-h"))
  {
    std::cout << "-h          : For help" << std::endl;
    std::cout << "-r regex    : Only the cases satisfying the given regex will run." << std::endl;
    std::cout << "              Check out guide for supported regex expressions." << std::endl;
    std::cout << "-o filename : Save output to given file." << std::endl;
    std::cout << "-s filename : Save the result for comparison." << std::endl;
    std::cout << "-c filename : Load the given file for comparison." << std::endl;
    std::cout << "-f HTML     : Save the output as HTML(Default)" << std::endl;
    std::cout << "   TXT      : Save the output as text document" << std::endl;
    exit(0);
  }

  if(manak::utils::cli::CLI::cmdOptionExists(argv, argv + argc, "-v"))
  {
    std::cout << manak::GetVersionInfo() << std::endl;
    exit(0);
  }


  init_func();
  std::string pattern = "";
  if(manak::utils::cli::CLI::cmdOptionExists(argv, argv + argc, "-r"))
  {
    char* c_pattern = manak::utils::cli::CLI::getCmdOption(argv,
                                                           argv + argc,
                                                           "-r");
    if(c_pattern)
    {
      pattern = std::string(c_pattern);
    }
    else
    {
      std::cout << "No pattern given!" << std::endl;
      exit(1);
    }
  }

  std::ofstream* stream = NULL;

  if(manak::utils::cli::CLI::cmdOptionExists(argv, argv + argc, "-f"))
  {
    std::string format = manak::utils::cli::CLI::getCmdOption(argv, argv + argc, "-f");
    if(format == "TXT" || format == "txt")
      output_format_html = false;
    else if(format == "HTML" || format == "html");
    else
    {
      std::cerr << "Unrecognized format \"" << format << "\"" << std::endl;
      std::cerr << "For supported formats pass \"-h\"" << std::endl;
      exit(1);
    }
  }

  if(manak::utils::cli::CLI::cmdOptionExists(argv, argv + argc, "-o"))
  {
    std::string filename(manak::utils::cli::CLI::getCmdOption(argv, argv + argc, "-o"));
    stream = new std::ofstream(filename);
  }
  else
  {
    std::string fname = MANAK_STRINGIZE(MANAK_DEFAULT_OUT_FILENAME);

    if(output_format_html)
      fname += ".html";
    else fname += ".txt";

    stream = new std::ofstream(fname);
  }

  BenchmarkSuite::GetMasterSuite().Run("", pattern, compare);

  RunTree::GlobalRunTree().Run();

  if(manak::utils::cli::CLI::cmdOptionExists(argv, argv + argc, "-c"))
  {
    std::string filename(manak::utils::cli::CLI::getCmdOption(argv, argv + argc, "-c"));
    std::ifstream com_file(filename);
    RunTree::GlobalRunTree().LoadForComparison(com_file);
  }

  if(output_format_html)
    RunTree::GlobalRunTree().PrintHTML(*stream);
  else
    RunTree::GlobalRunTree().PrintTXT(*stream);

//  std::ofstream s_html("test.html");
//  RunTree::GlobalRunTree().PrintHTML(s_html);

  if(manak::utils::cli::CLI::cmdOptionExists(argv, argv + argc, "-s"))
  {
    std::string filename(manak::utils::cli::CLI::getCmdOption(argv, argv + argc, "-s"));

    std::ofstream ss(filename);
    RunTree::GlobalRunTree().SaveForComparison(ss);
    ss.close();
  }

  stream->close();
  delete stream;
  return 0;
}

}; // namespace manak

//! If main generation is set to auto following function will be added
#ifdef MANAK_AUTO_BENCHMARK_MAIN

int main(int argc, char* argv[])
{
  return manak::manak_benchmarking_main(&MANAK_INIT_FUNCTION, argc, argv);
}

#endif // MANAK_AUTO_BENCHMARK_MAIN

#endif // MANAK_HPP_INCLUDED
