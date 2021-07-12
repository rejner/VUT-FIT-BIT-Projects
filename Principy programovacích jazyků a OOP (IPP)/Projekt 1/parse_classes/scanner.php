<?php

/*Scanner*/
/* - performs basic lexical analysis of input*/
final class Scanner{

    private $input;
    public $instructions;
    public $types;
    public $line_counter = 0;
    public function __construct(){

        //reads input data, stores them into an array
        $this->input = fopen( 'php://stdin', 'r' );

        /* all possible instructions that might occur in input stored in an associative array,
         * values represent expected arguments */
        /* possible values: */
        /* VAR - instruction takes only one argument of type variable
         * SYMB - instruction takes only one argument of type symbol
         * LABEL - takes only one argument of type label
         * VAR_SYMB - 2 arguments, first of type variable, second of type symbol
         * VAR_TYPE - 2 arguments, first of type variable, second of type 'type'
         * VAR_SYMB_SYMB - 3 arguments, (1)variable -> (2)symbol -> (3)symbol
         * LABEL_SYMB_SYMB - 3 arguments, (1)label -> (2)symbol -> (3)symbol
         * NONE - takes no argument at all
         * */
        $this->instructions = array(
           '' => "NONE", "MOVE" => "VAR_SYMB", "WRITE" => "SYMB", "DEFVAR" => "VAR", "CREATEFRAME"=> "NONE",
            "PUSHFRAME" => "NONE", "POPFRAME" => "NONE", "CALL" => "LABEL", "RETURN" => "NONE", "PUSHS" => "SYMB",
            "POPS" => "VAR", "ADD" => "VAR_SYMB_SYMB", "SUB" => "VAR_SYMB_SYMB", "MUL" => "VAR_SYMB_SYMB",
            "IDIV" => "VAR_SYMB_SYMB", "LT" => "VAR_SYMB_SYMB", "GT" => "VAR_SYMB_SYMB", "EQ" => "VAR_SYMB_SYMB",
            "AND" => "VAR_SYMB_SYMB", "OR" => "VAR_SYMB_SYMB", "NOT" => "VAR_SYMB", "INT2CHAR" => "VAR_SYMB",
            "STRI2INT" => "VAR_SYMB_SYMB", "READ" => "VAR_TYPE", "CONCAT" => "VAR_SYMB_SYMB", "STRLEN" => "VAR_SYMB",
            "GETCHAR" => "VAR_SYMB_SYMB", "SETCHAR" => "VAR_SYMB_SYMB", "TYPE" => "VAR_SYMB", "LABEL" => "LABEL",
            "JUMP" => "LABEL", "JUMPIFEQ" => "LABEL_SYMB_SYMB", "JUMPIFNEQ" => "LABEL_SYMB_SYMB", "EXIT" => "SYMB",
            "DPRINT" => "SYMB", "BREAK" => "NONE"
        );

        $this->types = array(
            EOL => "BLANK",
            "MOVE" => "I_MOVE",
            "WRITE" => "I_WRITE",

        );

    }

    public function run_scanner($arguments){

        //init parser
        $parser = new Parser($arguments);

        $header = false; //set bool variable for header search

        while($line = fgets($this->input)){ //for each line in our input
            $this->line_counter++; //line counter increment

            //replace '#' characters with ' #' pattern, so we can easily find all possible comments
            if (preg_match('/^.*#.*/', $line)){
                $line = preg_replace('/#/', ' #', $line);
            }

            //remove all white space characters from the beginning to the first occurrence of any character
            $line = ltrim($line);
            $line = trim(preg_replace('/\s\s+/', ' ', $line)); //remove EOL from line

            //splits line into separate words with space delimiter
            $split_line = explode(" ", $line);

            //check for header
            if (!$header){
                if (!$header && $line == ''){
                    continue;
                } elseif (!$header && strtoupper($split_line[0]) == ".IPPCODE20") {
                    $header = true;
                    continue;

                } elseif (preg_match('/^#/', $line)){
                    $parser->comment_count++;
                    continue;

                } else {
                    return 1; # 1 -> bad header
                }
            }

            //if comment or blank line has been found, continue
             if (preg_match('/^#/', $line)) {
                 $parser->comment_count++;
                 continue;

             }elseif ($line == ''){ //ignoring blank line
                 continue;
             }

            //searches whether instruction found on a new line is legit or not
            if (array_key_exists(strtoupper($split_line[0]), $this->instructions)) {

                $split_line[0] = strtoupper($split_line[0]); // this allows case-insensitivity for operation codes

                //sends command to parser with expected arguments type and format included
                if ($parser->receive_command($split_line, $this->instructions[$split_line[0]])){
                    continue;
                }else{
                    return 3;
                }

            } else {
                //invalid command -> error
                return 2;
            }

        }
        $parser->print_output();
        if ($arguments->stats){
            $parser->print_stats($arguments->stats_file);
        }

        return 0;
    }
}
