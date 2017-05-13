# BNR - Bipartite Network Recommemdation

## Overview

Bnr is a command-line tool for bipartite network recommendation.

libbnr is the library bnr is using to do its job.

Algorithms included currently:

* [mass](https://journals.aps.org/pre/pdf/10.1103/PhysRevE.76.046115)
* [heat](https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.99.154301)
* [hybrid](http://www.pnas.org/content/107/10/4511.short)
* HNBI
* mass degree
* mass score 1
* mass score 2

## What it can do

TODO

## Installation

1. git clone https://github.com/RuiOAIX/BNR
2. cd BNR && make
3. The command *bnr* lives in *bin* folder.
4. The header of *libbnr* lives in *include* folder, and the static library *libbnr.a* lives in *lib* folder.

## Usage Example

1. `./run -i ~/Downloads/movielens_3c.txt -S`

## Options
* Common options:
	* -h: help
	* -o logfilename:
		* Setting the log file

* Options privated to Algorithm:
	* -m: Calculate the result of mass algorithm
	* -e: Calculate the result of heats algorithm
	* -H: Calculate the result of hybrid algorithm
	* --hybrid-rate doubleValue:
		* Rate used in hybrid algorithm.
		* only valid when -H option is used, otherwize this arg will be ignored.
		* default: 0.2
	* -N: Calculate the result of HNBI algorithm
	* --HNBI-rate doubleValue:
		* Rate used in HNBI algorithm.
		* only valid when -N option is used, otherwize this arg will be ignored.
		* default: -0.8
	* -D: Calculate the result of mass degree algorithm
	* --mass-degree-rate doubleValue:
		* Rate used in mass score algorithm.
		* only valid when -D option is used, otherwize this arg will be ignored.
		* default: -0.8
	* -S: Calculate the result of mass score algorithm
	* --mass-score-rate doubleValue:
		* Rate used in mass score algorithm.
		* only valid when -S option is used, otherwize this arg will be ignored.
		* default: -0.8
	* -w: Calculate the result of mass score (only the third step) algorithm
	* --mass-score-third-rate doubleValue:
		* Rate used in mass score (only the third step algorithm.
		* only valid when -w option is used, otherwize this arg will be ignored.
		* default: -0.8

* Options related to dataset & file:
	* -i filename:
		* File containing full dataset
		* if -i is used, then -T and -t will be ignored.
		* else, -T and -t both have to be present.
	* -T filename:
		* File containing train dataset
	* -t filename:
		* File containing test dataset
	* -u filename:
		* File containing extra attribute of the recommending objects
	* -d doubleValue:
		* Rate used to divide full dataset to train and test dataset
		* only valid when -i option is used

* Other options:
	* -l intValue:
		* Number of times which the algorthim calculation need to be performed
		* in order to get reasonable average result
	* -L intValue:
		* Number of the recommended objects which will be used to computer metrics
	* -s unsignedlongValue:
		* Random seed
