<?php

/*ArgumentManager*/
/*- checks for arguments */
/*- checks for correct format of arguments */
final class ArgumentManager_test{

    public $directory = '';
    public $scripts_directory = '';
    public $recursive = false;
    public $parse_script = '';
    public $int_script = '';
    public $parse_only = false;
    public $int_only = false;
    public $jexamxml_path = '/pub/courses/ipp/jexamxml/jexamxml.jar'; //default merlin xml comparison tool for Merlin

    function __construct($argc, $argv){

        if ($argc == 2 && $argv[1] == '--help'){
            fprintf(STDERR, "PRINTING HELP MESSAGE...");
            exit(0);
        }

        $this->scripts_directory = getcwd();

        for ($i = 1; $i < count($argv); $i++){
            switch ($argv[$i]){

                case '--recursive':
                    $this->recursive = true;
                    break;

                case '--parse-only':
                    $this->parse_only = true;
                    break;

                case '--int-only':
                    $this->int_only = true;
                    break;

                default:

                    if (preg_match('/^--directory=*/', $argv[$i])){
                        $this->directory = substr($argv[$i], 12);

                    } else if (preg_match('/^--parse-script=*/', $argv[$i])){
                        $this->parse_script = substr($argv[$i], 15);

                    } else if (preg_match('/^--int-script=*/', $argv[$i])){
                        $this->int_script = substr($argv[$i], 13);

                    } else if (preg_match('/^--jexamxml=*/', $argv[$i])){
                        $this->jexamxml_path = substr($argv[$i], 11);

                    } else {
                        fprintf(STDERR, "Unknown parameter or forbidden use of --help...".EOL);
                        exit(10); //unknown parameter
                    }
            }
        }

        if ($this->directory == '') $this->directory = getcwd();
        if ($this->parse_script == '') $this->parse_script = $this->scripts_directory."/parse.php";
        if ($this->int_script == '') $this->int_script = $this->scripts_directory."/interpret.py";
    }

    function print_values(){
        fprintf(STDERR, "DIRECTORY: ".$this->directory.EOL);
        fprintf(STDERR, "recursive: ".$this->recursive.EOL);
        fprintf(STDERR, "PARSE SCRIPT: ".$this->parse_script.EOL);
        fprintf(STDERR, "INT SCRIPT: ".$this->int_script.EOL);
        fprintf(STDERR, "PARSE_ONLY: ".$this->parse_only.EOL);
        fprintf(STDERR, "INT_ONLY: ".$this->int_only.EOL);
        fprintf(STDERR, "JEXAMXML: ".$this->jexamxml_path.EOL);
    }
}