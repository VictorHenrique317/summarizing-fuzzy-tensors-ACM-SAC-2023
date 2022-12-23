// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

/* NCLUSTERBOX_UPDATES_SUMS makes nclusterbox update (rather than compute from scratch) the sums of the membership degrees on all elements after each addition/remotion step. */
#define NCLUSTERBOX_UPDATES_SUMS

/* NCLUSTERBOX_REMEMBERS makes nclusterbox store every pattern that it reaches to avoid redundant computation if it is reached again. */
#define NCLUSTERBOX_REMEMBERS

/* VERBOSE_PARSER turns on the output (on the standard output) of information when the input data are parsed. */
/* #define VERBOSE_PARSER */

/* DEBUG_NCLUSTERBOX turns on the output (on the standard output) of information during the execution of nclusterbox.  This option may be enabled by who wishes to understand how nclusterbox modifies a small number of patterns. */
/* #define DEBUG_NCLUSTERBOX */

/* DEBUG_GROW turns on the output (on the standard output) of information during the growing of the patterns.  This option may be enabled by who wishes to understand how a small number of patterns grow. */
/* #define DEBUG_GROW */

/* DEBUG_AMALGAMATE turns on the output (on the standard output) of information during the amalgamation of the patterns.  This option may be enabled by who wishes to understand how a small number of patterns are amalgamated. */
/* #define DEBUG_AMALGAMATE */

/* DEBUG_SELECT turns on the output (on the standard output) of information during the selection of the patterns.  This option may be enabled by who wishes to understand how a small number of patterns are selected. */
/* #define DEBUG_SELECT */

/* NUMERIC_PRECISION turns ou the output (on the standard output) of the maximal possible round-off error made when internally storing a membership degree for growing and amalgamating and, then, for selecting. */
#define NUMERIC_PRECISION

/* NB_OF_PATTERNS turns on the output (on the standard output) of the numbers (in this order) of: */
/* - density-maximized patterns */
/* - grown patterns */
/* - patterns candidates for selection */
/* - selected patterns */
#define NB_OF_PATTERNS

/* AICC turns on the output (on the standard output) of the AICc of the selected model. */
#define AICC

/* TIME turns on the output (on the standard output) of the run time of paf. */
#define TIME

/* DETAILED_TIME turns on the output (on the standard output) of a more detailed analysis of how the time is spent.  It lists (in this order): */
/* - the tensor parsing time */
/* - the tensor shifting time */
/* - the inital density maximization time */
/* - the inital sub-pattern discarding time */
/* - the further growing time */
/* - the amalgamation initialization time */
/* - the amalgamation time */
/* - the tensor reduction time */
/* - the selection time */
#define DETAILED_TIME

/* GNUPLOT modifies the outputs of NUMERIC_PRECISION, NB_OF_PATTERNS, AICC, DETAILED_TIME and TIME.  They become tab separated values.  The output order is: */
/* - tensor parsing time (#ifdef DETAILED_TIME) */
/* - numeric precision for growing and amalgamating (#ifdef NUMERIC_PRECISION) */
/* - tensor shifting time (#ifdef DETAILED_TIME) */
/* - inital density maximization time (#ifdef DETAILED_TIME) */
/* - inital sub-pattern discarding time (#ifdef DETAILED_TIME) */
/* - number of density-maximized patterns (#ifdef NB_OF_PATTERNS) */
/* - further growing time (#ifdef DETAILED_TIME) */
/* - number of grown patterns (#ifdef NB_OF_PATTERNS) */
/* - amalgamation initialization time (#ifdef DETAILED_TIME) */
/* - amalgamation time (#ifdef DETAILED_TIME) */
/* - number of patterns candidates for selection (#ifdef NB_OF_PATTERNS) */
/* - numeric precision for selecting (#ifdef NUMERIC_PRECISION) */
/* - tensor reduction time (#ifdef DETAILED_TIME) */
/* - number of selected patterns (#ifdef NB_OF_PATTERNS) */
/* - AICc (#ifdef AICC) */
/* - selection time (#ifdef DETAILED_TIME) */
/* - total time (#ifdef TIME) */
/* #define GNUPLOT */

// Assert
/* ASSERT is used to check, after every iteration of nclusterbox, the correctness of the area and of the sums of membership degrees on every element and over the whole pattern. */
/* #define ASSERT */

#endif /*PARAMETERS_H_*/
