<?php

/*ArgumentManager*/
/*- checks for arguments, sets flags for logging options */
/*- checks for correct format of arguments */
final class ArgumentManager_parse{

    public $stats = false;
    public $loc = false;
    public $labels = false;
    public $jumps = false;
    public $comments = false;
    public $stats_file;
    public $stats_order = array();

    public function __construct($argc, $argv){

        if ($argc == 2){

            //check for help
            if ($argv[1] == "--help"){
                print("Usage: parse.php [OPTIONS] < IPPCODE20".EOL);
                print("- This script performs lexical and syntax analysis of code written in IPPcode20 language.".EOL);
                print("Options:".EOL);
                print("--help       | - prints help message".EOL);
                print("--stats=FILE | - turns on gathering of statistics into given file".EOL);
                print("Parameters for usage with --stats:".EOL);
                print("--loc      | - counts number of lines with containing instruction".EOL);
                print("--comments | - counts amount of lines containing comment".EOL);
                print("--labels   | - counts amount of defined labels".EOL);
                print("--jumps    | - counts amount of instructions for (un)conditional jumps".EOL);
                exit (0);

            } else if (preg_match('/^--stats=*/', $argv[1])){ //only stats were passed -> blank file
                $this->stats = true;
                $this->stats_file = substr($argv[1], 8);
            } else {
                exit (10); //cannot be combined with any other parameters
            }

        } else { //more arguments passed, analyze them

            for ($i = 1; $i < count($argv); $i++){
                switch ($argv[$i]){

                    case "--loc":
                        $this->loc = true;
                        array_push($this->stats_order, 'LOC');
                        break;

                    case "--comments":
                        $this->comments = true;
                        array_push($this->stats_order, 'COM');
                        break;

                    case "--jumps":
                        $this->jumps = true;
                        array_push($this->stats_order, 'JUM');
                        break;

                    case "--labels":
                        $this->labels = true;
                        array_push($this->stats_order, 'LAB');
                        break;

                    default:

                        if (preg_match("/^--stats=*/", $argv[$i])){
                            $this->stats = true;
                            $this->stats_file = substr($argv[$i], 8);
                        } else {
                            fprintf(STDERR, "Unknown parameter or forbidden use of --help...".EOL);
                            exit(10); //unknown parameter or --help with other parameters
                        }
                        break;
                }
            }

            if (($this->jumps || $this->loc || $this->comments || $this->labels) && !$this->stats){
                fprintf(STDERR, "Missing stats parameter...".EOL);
                exit(10);
            }
        }
    }
}