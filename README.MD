# BNRS - Bipart Network Recommemder System 1.0 (Apr 2015)

### Example
* `valgrind --tool=memcheck --leak-check=full -v --track-origins=yes --log-file=./rs.log ./run -i ~/Downloads/movielens_2c.txt -m`
* `./run -i ~/Downloads/movielens_3c.txt -S`

### OPTION
* OPTION common:
	*  -h:  help
	*  -o logfilename:
		*       File the log system will output log to

* OPTION privated to Algorithm:
	*  -m:  Calculate the result of mass algorithm
	*  -e:  Calculate the result of heats algorithm
	*  -H:  Calculate the result of hybrid algorithm
	*  --hybrid-rate doubleValue:  
		*       Rate used in hybrid algorithm.
		*       only valid when -H option is used, otherwize this arg will be ignored.
		*       default: 0.2
	*  -N:  Calculate the result of HNBI algorithm
	*  --HNBI-rate doubleValue:  
		*       Rate used in HNBI algorithm.
		*       only valid when -N option is used, otherwize this arg will be ignored.
		*       default: -0.8
	*  -D:  Calculate the result of mass degree algorithm
	*  --mass-degree-rate doubleValue:  
		*       Rate used in mass score algorithm.
		*       only valid when -D option is used, otherwize this arg will be ignored.
		*       default: -0.8
	*  -S:  Calculate the result of mass score algorithm
	*  --mass-score-rate doubleValue:  
		*       Rate used in mass score algorithm.
		*       only valid when -S option is used, otherwize this arg will be ignored.
		*       default: -0.8
	*  -w:  Calculate the result of mass score (only the third step) algorithm
	*  --mass-score-third-rate doubleValue:  
		*       Rate used in mass score (only the third step algorithm.
		*       only valid when -w option is used, otherwize this arg will be ignored.
		*       default: -0.8

* OPTION common to Algorithms:
	*  -i filename:
		*       File containing full dataset
		*       if -i is used, then -T and -t will be ignored.
		*       else, -T and -t both have to be present.
	*  -T filename:
		*       File containing train dataset
	*  -t filename:
		*       File containing test dataset
	*  -u filename:  
		*       File containing extra attribute of the recommending objects
	*  -d doubleValue:  
		*       Rate used to divide full dataset to train and test dataset
		*       only valid when -i option is used
	*  -l intValue:  
		*       Number of times which the algorthim calculation need to be performed
		*       in order to get reasonable average result
	*  -L intValue:  
		*       Number of the recommended objects which will be used to computer metrics
	*  -s unsignedlongValue: 
		*       Random seed
