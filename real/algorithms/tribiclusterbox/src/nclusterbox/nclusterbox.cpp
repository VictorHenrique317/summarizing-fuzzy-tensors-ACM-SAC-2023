// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include <boost/program_options.hpp>
#include "sysexits.h"

#include "../common/PatternFileReader.h"
#include "../common/DenseRoughTensor.h"
#include "../common/OrderPatterns.h"
#include "ModifiedPattern.h"

using namespace boost::program_options;

int main(int argc, char* argv[])
{
  AbstractRoughTensor* roughTensor;
  bool isVerbose;
  bool isRSSPrinted;
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
	("grow,g", "remove nothing from the input patterns")
	("intermediary,i", "keep intermediary patterns")
	("ns", "neither select nor order patterns")
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
	("prss", "print residual sum of squares of truncated output")
	("ps", "print sizes in output data")
	("sp", value<string>()->default_value(" : "), "set string prefixing sizes in output data")
	("ss", value<string>()->default_value(" "), "set string separating sizes in output data")
	("pa", "print areas in output data")
	("ap", value<string>()->default_value(" : "), "set string prefixing area in output data");
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
	  cout << "Usage: nclusterbox [options] [pattern-file]\n" << generic << mandatory << basicConfig << io << "\nReport bugs to: lcerf@dcc.ufmg.br\n";
	  return EX_OK;
	}
      if (vm.count("version"))
	{
	  cout << "nclusterbox version 0.27\nCopyright (C) 2022 Loïc Cerf.\nLicense GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\n";
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
      if (!vm.count("file"))
	{
	  throw UsageException("file option is mandatory!");
	}
      if (vm["density"].as<float>() < 0 || vm["density"].as<float>() > 1)
	{
	  throw UsageException("density option should provide a float in [0, 1]!");
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
      AbstractRoughTensor::setOutput(outputFileName.c_str(), vm["ods"].as<string>().c_str(), vm["oes"].as<string>().c_str(), "", "", vm["sp"].as<string>().c_str(), vm["ss"].as<string>().c_str(), vm["ap"].as<string>().c_str(), vm.count("pl"), vm.count("ps"), vm.count("pa"), vm.count("ns"));
      if (AbstractRoughTensor::isDirectOutput())
	{
	  roughTensor->setNoSelection();
	}
#ifdef DETAILED_TIME
      steady_clock::time_point startingPoint = steady_clock::now();
#endif
      ModifiedPattern::setContext(roughTensor, tensor, vm.count("intermediary"));
      PatternFileReader patternFileReader(patternFileName.c_str(), vm["pds"].as<string>().c_str(), vm["pes"].as<string>().c_str(), AbstractRoughTensor::getIds2Labels());
      if (isVerbose)
	{
	  if (vm.count("grow"))
	    {
	      for (vector<vector<unsigned int>> nSet = patternFileReader.next(); !nSet.empty(); nSet = patternFileReader.next())
		{
		  cout << "\rMaximizing g growing pattern in line " << patternFileReader.getLineNb() << '.' << flush;
		  ModifiedPattern::grow(nSet);
		}
	      cout << "\rMaximizing g growing every input pattern: done.\n";
	    }
	  else
	    {
	      for (vector<vector<unsigned int>> nSet = patternFileReader.next(); !nSet.empty(); nSet = patternFileReader.next())
		{
		  cout << "\rMaximizing g modifying pattern in line " << patternFileReader.getLineNb() << '.' << flush;
		  ModifiedPattern::modify(nSet);
		}
	      cout << "\rMaximizing g modifying every input pattern: done.\n";
	    }
	}
      else
	{
	  if (vm.count("grow"))
	    {
	      for (vector<vector<unsigned int>> nSet = patternFileReader.next(); !nSet.empty(); nSet = patternFileReader.next())
		{
		  ModifiedPattern::grow(nSet);
		}
	    }
	  else
	    {
	      for (vector<vector<unsigned int>> nSet = patternFileReader.next(); !nSet.empty(); nSet = patternFileReader.next())
		{
		  ModifiedPattern::modify(nSet);
		}
	    }
	}
      ModifiedPattern::clearAndFree();
#ifdef DETAILED_TIME
#ifdef GNUPLOT
      cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count();
#else
      cout << "g maximization time: " << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "s\n";
#endif
#endif
      isRSSPrinted = vm.count("prss");
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
  if (AbstractRoughTensor::getNbOfCandidateVariables())
    {
      OrderPatterns::order(AbstractRoughTensor::getNbOfCandidateVariables(), roughTensor, isVerbose, isRSSPrinted);
    }
  else
    {
#if defined NUMERIC_PRECISION || defined NB_OF_PATTERNS || defined AICC || defined DETAILED_TIME
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
