// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include <boost/program_options.hpp>
#include "sysexits.h"

#include "../common/DenseRoughTensor.h"
#include "../common/NonSink.h"
#include "Pattern.h"
#include "DendrogramNode.h"

using namespace boost::program_options;

#if defined NUMERIC_PRECISION || defined NB_OF_PATTERNS || defined AICC || defined DETAILED_TIME
void printZerosForNoSelection()
{
#ifdef GNUPLOT
#ifdef NB_OF_PATTERNS
  cout << "\t0\t0";
#endif
#ifdef NUMERIC_PRECISION
  cout << "\t0";
#endif
#ifdef AICC
  cout << "\t0";
#endif
#ifdef DETAILED_TIME
  cout << "\t0";
#endif
#else
#ifdef NB_OF_PATTERNS
  cout << "Nb of patterns candidates for selection: 0\n";
#endif
#ifdef NUMERIC_PRECISION
  cout << "Numeric precision: 0\n";
#endif
#ifdef AICC
  cout << "AICc: 0\n";
#endif
#ifdef NB_OF_PATTERNS
  cout << "Nb of selected patterns: 0\n";
#endif
#ifdef DETAILED_TIME
  cout << "Selecting time: 0s\n";
#endif
#endif
}
#endif

int main(int argc, char* argv[])
{
  AbstractRoughTensor* roughTensor;
  unsigned int nbOfGrownPatterns;
  bool isOnlyGrow;
  bool isVerbose;
  bool isHierarchyPrinted = false;
  // Parsing the command line and the option file
  try
    {
      string tensorFileName;
      string patternFileName;
      string outputFileName;
      options_description generic("Generic options");
      generic.add_options()
	("help,h", "produce help message")
	("version,V", "display version information and exit")
	("opt", value<string>(), "set the option file name (by default [pattern-file].opt if present)");
      options_description mandatory("Mandatory option (on the command line or in the option file)");
      mandatory.add_options()
	("file,f", value<string>(&tensorFileName), "set tensor file");
      options_description basicConfig("Basic configuration (on the command line or in the option file)");
      basicConfig.add_options()
	("shift,s", value<float>(), "shift memberhip degrees by constant in argument (by default, density of input tensor)")
	("expectation,e", "shift every memberhip degree by the max density of the slices covering it")
	("density,d", value<float>()->default_value(0), "set threshold between 0 (completely dense storage) and 1 (minimization of memory usage) to trigger a dense storage of the tensor")
	("grow,g", value<unsigned int>(&nbOfGrownPatterns), "grow until getting the specified number of patterns and do not amalgamate")
	("max,m", "no input pattern is sub-pattern of another one")
	("ni", "while growing, discard intermediary patterns")
	("amalgamate,a", value<unsigned int>(&nbOfGrownPatterns), "grow until getting the specified number of patterns and amalgamate")
	("ns", "do not select patterns")
	("verbose,v", "verbose output")
	("out,o", value<string>(&outputFileName)->default_value("-"), "set output file name");
      options_description io("Input/Output format (on the command line or in the option file)");
      io.add_options()
	("ids", value<string>()->default_value(" "), "set any character separating two dimensions in input tensor")
	("ies", value<string>()->default_value(","), "set any character separating two elements in input tensor")
	("pds", value<string>()->default_value(" "), "set any character separating two dimensions in input patterns")
	("pes", value<string>()->default_value(","), "set any character separating two elements in input patterns")
	("ods", value<string>()->default_value(" "), "set string separating two dimensions in output patterns")
	("oes", value<string>()->default_value(","), "set string separating two elements in output patterns")
	("pl", "print the parameters of the model instead of densities")
	("ps", "print sizes in output data")
	("sp", value<string>()->default_value(" : "), "set string prefixing sizes in output data")
	("ss", value<string>()->default_value(" "), "set string separating sizes in output data")
	("pa", "print areas in output data")
	("ap", value<string>()->default_value(" : "), "set string prefixing area in output data")
	("ph", "print direct subpatterns in output data")
	("hp", value<string>()->default_value(" : "), "set string prefixing direct subpatterns in output data")
	("hs", value<string>()->default_value(" "), "set string separating direct subpatterns in output data");
      options_description hidden("Hidden options");
      hidden.add_options()
	("pattern-file", value<string>(&patternFileName)->default_value("/dev/stdin"), "set pattern file");
      positional_options_description p;
      p.add("pattern-file", -1);
      options_description commandLineOptions;
      commandLineOptions.add(generic).add(mandatory).add(basicConfig).add(io).add(hidden);
      variables_map vm;
      store(command_line_parser(argc, argv).options(commandLineOptions).positional(p).run(), vm);
      notify(vm);
      if (vm.count("help"))
	{
	  cout << "Usage: paf [options] [pattern-file]\n" << generic << mandatory << basicConfig << io << "\nReport bugs to: lcerf@dcc.ufmg.br\n";
	  return EX_OK;
	}
      if (vm.count("version"))
	{
	  cout << "paf version 0.27\nCopyright (C) 2022 Loïc Cerf.\nLicense GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\n";
	  return EX_OK;
	}
      ifstream optionFile;
      if (vm.count("opt"))
	{
	  if (vm["opt"].as<string>() == "-")
	    {
	      optionFile.open("/dev/stdin");
	    }
	  else
	    {
	      optionFile.open(vm["opt"].as<string>());
	      if (!optionFile)
		{
		  throw NoInputException(vm["opt"].as<string>().c_str());
		}
	    }
	}
      else
	{
	  if (vm.count("pattern-file"))
	    {
	      optionFile.open((patternFileName + ".opt").c_str());
	    }
	}
      options_description config;
      config.add(mandatory).add(basicConfig).add(io).add(hidden);
      store(parse_config_file(optionFile, config), vm);
      optionFile.close();
      notify(vm);
      if (vm.count("grow") || vm.count("amalgamate"))
	{
	  if (vm.count("grow") && vm.count("amalgamate"))
	    {
	      throw UsageException("grow and amalgamate options exclude each other!");
	    }
	}
      else
	{
	  nbOfGrownPatterns = numeric_limits<unsigned int>::max();
	}
      if (!vm.count("file"))
	{
	  throw UsageException("file option is mandatory!");
	}
      if (vm["density"].as<float>() < 0 || vm["density"].as<float>() > 1)
	{
	  throw UsageException("density option should provide a float in [0, 1]!");
	}
      if (vm.count("ph"))
	{
	  if (vm.count("ns"))
	    {
	      cerr << "Warning: in conjunction with the ns option, ph option is ignored\n";
	    }
	  else
	    {
	      isHierarchyPrinted = true;
	    }
	}
      if (tensorFileName == "-")
	{
	  tensorFileName = "/dev/stdin";
	}
      isVerbose = vm.count("verbose");
      if (vm.count("expectation"))
      	{
      	  if (vm.count("shift"))
      	    {
      	      throw UsageException("shift and expectation options are mutually exclusive!");
      	    }
      	  if (vm["density"].as<float>() != 0)
      	    {
      	      cerr << "Warning: expectation option triggers a dense storage of the tensor, despite the use of the density option\n";
      	    }
      	  roughTensor = new DenseRoughTensor(tensorFileName.c_str(), vm["ids"].as<string>().c_str(), vm["ies"].as<string>().c_str(), isVerbose);
      	}
      else
      	{
      	  if (vm.count("shift"))
      	    {
	      if (vm["shift"].as<float>() < 0 || vm["shift"].as<float>() >= 1)
		{
		  throw UsageException("shift option should provide a float in [0, 1[!");
		}
      	      roughTensor = AbstractRoughTensor::makeRoughTensor(tensorFileName.c_str(), vm["ids"].as<string>().c_str(), vm["ies"].as<string>().c_str(), vm["density"].as<float>(), vm["shift"].as<float>(), isVerbose);
      	    }
      	  else
      	    {
      	      roughTensor = AbstractRoughTensor::makeRoughTensor(tensorFileName.c_str(), vm["ids"].as<string>().c_str(), vm["ies"].as<string>().c_str(), vm["density"].as<float>(), isVerbose);
      	    }
      	}
      if (patternFileName == "-")
	{
	  patternFileName = "/dev/stdin";
	}
      if (outputFileName == "-")
	{
	  outputFileName = "/dev/stdout";
	}
      Trie tensor(roughTensor->getTensor());
      if (isVerbose)
	{
	  cout << "\rShifting fuzzy tensor: done.\n";
	}
      AbstractRoughTensor::setOutput(outputFileName.c_str(), vm["ods"].as<string>().c_str(), vm["oes"].as<string>().c_str(), vm["hp"].as<string>().c_str(), vm["hs"].as<string>().c_str(), vm["sp"].as<string>().c_str(), vm["ss"].as<string>().c_str(), vm["ap"].as<string>().c_str(), vm.count("pl"), vm.count("ps"), vm.count("pa"), vm.count("ns"));
      if (AbstractRoughTensor::isDirectOutput())
	{
	  roughTensor->setNoSelection();
	}
      Pattern::grow(patternFileName.c_str(), vm["pds"].as<string>().c_str(), vm["pes"].as<string>().c_str(), roughTensor, tensor, nbOfGrownPatterns, vm.count("max"), !vm.count("ni"), isVerbose);
      isOnlyGrow = !vm.count("amalgamate");
    }
  catch (unknown_option& e)
    {
      cerr << "Unknown option!\n";
      return EX_USAGE;
    }
  catch (UsageException& e)
    {
      cerr << e.what() << '\n';
      return EX_USAGE;
    }
  catch (NoInputException& e)
    {
      cerr << e.what() << '\n';
      return EX_NOINPUT;
    }
  catch (DataFormatException& e)
    {
      cerr << e.what() << '\n';
      return EX_DATAERR;
    }
  catch (NoOutputException& e)
    {
      cerr << e.what() << '\n';
      return EX_CANTCREAT;
    }
  if (isOnlyGrow)
    {
#ifdef DETAILED_TIME
#ifdef GNUPLOT
      cout << "\t0\t0";
#else
      cout << "Amalgamation initialization time: 0s\nAmalgamating time: 0s\n";
#endif
#endif
      if (AbstractRoughTensor::isDirectOutput())
	{
#if defined NUMERIC_PRECISION || defined NB_OF_PATTERNS || defined AICC || defined DETAILED_TIME
	  printZerosForNoSelection();
#endif
	  Pattern::outputPatterns();
	}
      else
	{
	  NonSink::forwardSelect(Pattern::setPatternsAsCandidateVariables(), roughTensor, isVerbose, isHierarchyPrinted);
	}
    }
  else
    {
      DendrogramNode::setLeavesAndTensor(Pattern::getPatterns(), roughTensor, Pattern::getTensor());
      DendrogramNode::amalgamate(isVerbose);
      if (!AbstractRoughTensor::isDirectOutput())
	{
	  NonSink::forwardSelect(1, roughTensor, isVerbose, isHierarchyPrinted);
	}
#if defined NUMERIC_PRECISION || defined NB_OF_PATTERNS || defined AICC || defined DETAILED_TIME
      else
	{
	  printZerosForNoSelection();
	}
#endif
    }
  delete roughTensor;
#ifdef TIME
  AbstractRoughTensor::printCurrentDuration();
#endif
#if defined GNUPLOT && (defined NUMERIC_PRECISION || defined NB_OF_PATTERNS || defined AICC || defined DETAILED_TIME)
  cout << '\n';
#endif
  return EX_OK;
}
