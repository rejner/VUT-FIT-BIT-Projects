<?php

/*Parser*/
/* - checks syntax of code and generates its XML representation*/
final class Parser{

    private $output_file;
    private $var_re = '/^(GF|LF|TF)@([a-zA-Z]|_|[-$&%*!?])+[a-zA-Z0-9]*$/';
    private $symb_int_re = '/^int@([-+]|[0-9])[0-9]*$/';
    private $symb_bool_re = '/^bool@(true|false)$/';
    private $symb_nill_re = '/^nil@nil$/';
    private $symb_string_re = '/^string@([a-zA-Z0-9]|\p{L}|\p{M}|\p{S}|\p{P}|\p{Z}|\p{C}|\\\{1}[0-9]{3})*$/u';
    private $label_re = '/^([a-zA-Z]|[-$&%*!?_])+[a-zA-Z0-9]*$/';
    private $type_re = '/^(string|bool|int)$/';
    private $xw; //xml-writer object
    private $order = 0;

    //counters for STATP add-on
    public $loc_count = 0;
    public $comment_count = 0;
    public $labels_count = 0;
    public $jumps_count = 0;

    //access to arguments
    private $args;

    public function __construct($arguments){

        //configure xml-writer for generating XML output
        $this->xw = xmlwriter_open_memory();
        xmlwriter_set_indent($this->xw, 1);
        $res = xmlwriter_set_indent_string($this->xw, "\t");

        //generate basic structure
        xmlwriter_start_document($this->xw, '1.0', 'UTF-8');
        xmlwriter_start_element($this->xw, 'program');
        xmlwriter_start_attribute($this->xw, 'language');
        xmlwriter_text($this->xw, 'IPPcode20');
        xmlwriter_end_attribute($this->xw);

        $this->args = $arguments;

    }

    /*Receive command from Scanner*/
    public function receive_command($line, $arg_type){

        $this->order++;

        if ($this->args->loc) $this->loc_count++;

        //generates instruction XML element
        xmlwriter_start_element($this->xw, 'instruction');
        xmlwriter_start_attribute($this->xw, 'order');
        xmlwriter_text($this->xw, $this->order);
        xmlwriter_start_attribute($this->xw, 'opcode');
        xmlwriter_text($this->xw, $line[0]);
        xmlwriter_end_attribute($this->xw);

        // based on required argument type and order, select case for syntax analysis
        switch($arg_type){

            case "VAR":

                if($this->check_var($line, 1, 1)){
                    $this->generate_argument($line, "VAR", 1);
                    xmlwriter_end_element($this->xw);
                    if (count($line) >= 3 && preg_match('/^#/', $line[2])) $this->comment_count++; //comment check
                    return true;
                }
                break;

            case "SYMB":

                if ($this->check_symbol($line, 1, 1)){
                    $this->generate_argument($line, "SYMB", 1);
                    xmlwriter_end_element($this->xw);
                    if (count($line) >= 3 && preg_match('/^#/', $line[2])) $this->comment_count++; //comment check
                    return true;
                }
                break;

            case "LABEL":

                if ($line[0] == "LABEL") $this->labels_count++;
                if ($line[0] == "JUMP" || $line[0] == "CALL") $this->jumps_count++;

                if ($this->check_label($line, 1, 1)){
                    $this->generate_argument($line, "LABEL", 1);
                    xmlwriter_end_element($this->xw);
                    if (count($line) >= 3 && preg_match('/^#/', $line[2])) $this->comment_count++; //comment check
                    return true;
                }
                break;

            case "VAR_SYMB":

                if ($this->check_var($line, 1, 2)){
                    if ($this->check_symbol($line, 2, 2)){
                        $this->generate_argument($line, "VAR", 1);
                        $this->generate_argument($line, "SYMB", 2);
                        xmlwriter_end_element($this->xw);
                        if (count($line) >= 4 && preg_match('/^#/', $line[3])) $this->comment_count++; //comment check
                        return true;
                    }
                }
                break;

            case "VAR_TYPE":

                if ($this->check_var($line, 1, 2)){
                    if ($this->check_type($line, 2, 2)){
                        $this->generate_argument($line, "VAR", 1);
                        $this->generate_argument($line, "TYPE", 2);
                        xmlwriter_end_element($this->xw);
                        if (count($line) >= 4 && preg_match('/^#/', $line[3])) $this->comment_count++; //comment check
                        return true;
                    }
                }
                break;

            case "VAR_SYMB_SYMB":

                if ($this->check_var($line, 1, 3)){
                    if ($this->check_symbol($line, 2, 3)){
                        if ($this->check_symbol($line, 3, 3)){
                            $this->generate_argument($line, "VAR", 1);
                            $this->generate_argument($line, "SYMB", 2);
                            $this->generate_argument($line, "SYMB", 3);
                            xmlwriter_end_element($this->xw);
                            if (count($line) >= 5 && preg_match('/^#/', $line[4])) $this->comment_count++; //comment check
                            return true;
                        }
                    }
                }
                break;

            case "LABEL_SYMB_SYMB":

                if (preg_match('/^JUMP*/', $line[0])) $this->jumps_count++;

                if ($this->check_label($line, 1, 3)){
                    if ($this->check_symbol($line, 2, 3)){
                        if ($this->check_symbol($line, 3, 3)){
                            $this->generate_argument($line, "LABEL", 1);
                            $this->generate_argument($line, "SYMB", 2);
                            $this->generate_argument($line, "SYMB", 3);
                            xmlwriter_end_element($this->xw);
                            if (count($line) >= 5 && preg_match('/^#/', $line[4])) $this->comment_count++; //comment check
                            return true;
                        }
                    }
                }
                break;

            case "NONE":

                if ($this->check_none($line)){
                    if ($line[0] == "RETURN") $this->jumps_count++; //jump check
                    xmlwriter_end_element($this->xw);
                    if (count($line) >= 2 && preg_match('/^#/', $line[1])) $this->comment_count++; //comment check
                    return true;
                }
                break;

            default:
                break;

        }

        return false; //something failed

    }

    /*Checks correct format of variable written in IPPcode20 language*/
    public function check_var($line, $order, $num_of_args){

        //argument is valid, command might be executable
        if (count($line) >= $num_of_args + 1 && preg_match($this->var_re, $line[$order])){

            //if this is last argument or next argument starts with #, then command is valid
            if (count($line) == $num_of_args + 1 || preg_match('/^#/', $line[$num_of_args + 1])){
                return true; //command is valid
            }
        }

        return false;
    }

    public function generate_argument($line, $type, $rank){

        switch($type){

            case "VAR":
                xmlwriter_start_element($this->xw, 'arg'.$rank);
                xmlwriter_start_attribute($this->xw, 'type');
                xmlwriter_text($this->xw, 'var');
                xmlwriter_end_attribute($this->xw);
                xmlwriter_text($this->xw, $line[$rank]);
                xmlwriter_end_element($this->xw);

                break;

            case "SYMB":
                xmlwriter_start_element($this->xw, 'arg'.$rank);
                xmlwriter_start_attribute($this->xw, 'type');
                xmlwriter_text($this->xw, $this->check_arg_type($line[$rank]));
                xmlwriter_end_attribute($this->xw);
                xmlwriter_text($this->xw, $this->extract_symbol($line[$rank]));
                xmlwriter_end_element($this->xw);

                break;

            case "TYPE":
                xmlwriter_start_element($this->xw, 'arg'.$rank);
                xmlwriter_start_attribute($this->xw, 'type');
                xmlwriter_text($this->xw, 'type');
                xmlwriter_end_attribute($this->xw);
                xmlwriter_text($this->xw, $line[$rank]);
                xmlwriter_end_element($this->xw);

                break;

            case "LABEL":
                xmlwriter_start_element($this->xw, 'arg'.$rank);
                xmlwriter_start_attribute($this->xw, 'type');
                xmlwriter_text($this->xw, 'label');
                xmlwriter_end_attribute($this->xw);
                xmlwriter_text($this->xw, $line[$rank]);
                xmlwriter_end_element($this->xw);

                break;
        }
    }

    /*Checks correct format of symbol written in IPPcode20 language*/
    public function check_symbol($line, $order, $num_of_args){

        //argument is valid, command might be executable
        if (count($line) >= $num_of_args + 1 && (preg_match($this->symb_int_re, $line[$order]) ||
                preg_match($this->symb_string_re, $line[$order]) ||
                preg_match($this->symb_bool_re, $line[$order]) ||
                preg_match($this->symb_nill_re, $line[$order]) ||
                preg_match($this->var_re, $line[$order]))) {

            if (count($line) == $num_of_args + 1 || preg_match('/^#/', $line[$num_of_args + 1])) {
                return true; //command is valid

            }
        }
        return false;
    }

    /*Checks correct format of label written in IPPcode20 language*/
    public function check_label($line, $order, $num_of_args){

        if (count($line) >= $num_of_args + 1 && (preg_match($this->label_re, $line[$order]))){
            if (count($line) == $num_of_args + 1 || preg_match('/^#/', $line[$num_of_args + 1])) {
                return true; //command is valid
            }
        }
        return false;
    }

    /*Checks correct format of 'type' written in IPPcode20 language*/
    public function check_type($line, $order, $num_of_args){

        //argument is valid, command might be executable
        if (count($line) >= $num_of_args + 1 && preg_match($this->type_re, $line[$order])){

            //if this is last argument or next argument starts with #, then command is valid
            if (count($line) == $num_of_args + 1 || preg_match('/^#/', $line[$order + 1])){
                return true; //command is valid
            }
        }
        return false;
    }

    /*Checks correct format of 'none' written in IPPcode20 language*/
    public function check_none($line){

        //argument is valid, command might be executable
        if (count($line) >= 1){

            //if this is last argument or next argument starts with #, then command is valid
            if (count($line) == 1 || preg_match('/^#/', $line[1])){
                return true; //command is valid
            }
        }
        return false;
    }

    /*Check type of symbol given and return that type*/
    public function check_arg_type($param){

        if (preg_match($this->var_re, $param)){ //var
            return "var";
        } elseif (preg_match($this->symb_int_re, $param)){ //int
            return "int";
        } elseif (preg_match($this->symb_string_re, $param)){ //string
            return "string";
        } elseif (preg_match($this->symb_bool_re, $param)){ //bool
            return "bool";
        } elseif (preg_match($this->symb_nill_re, $param)){ //nill
            return "nil";
        } elseif (preg_match($this->label_re, $param)){ //label
            return "label";
        } elseif (preg_match($this->type_re, $param)){ //type
            return "type";
        } else {
            return "unknown";
        }
    }

    /*Extracts data from symbol (excluding variable type) and return data after '@' character*/
    /*Necessary for generating of valid XML*/
    public function extract_symbol($symb){
        if (preg_match($this->var_re, $symb)){
            return $symb;
        } else {
            return substr($symb, strpos($symb, '@') + 1);
        }
    }

    /*Prints output generated by XMLWriter*/
    public function print_output(){
        xmlwriter_end_document($this->xw);
        echo xmlwriter_output_memory($this->xw);
    }

    /*Writes stats if legal --stats parameter has been given*/
    public function print_stats($filename){

        //open required file
        $fp = fopen($filename, 'w+');

        //iterate through given order of statistics and write them into file specified by --stats=FILE parameter
        foreach ($this->args->stats_order as $information){

            switch ($information){
                case "LOC":
                    fprintf ($fp,$this->loc_count.EOL);
                    break;
                case "COM":
                    fprintf ($fp,$this->comment_count.EOL);
                    break;
                case "JUM":
                    fprintf ($fp,$this->jumps_count.EOL);
                    break;
                case "LAB":
                    fprintf ($fp,$this->labels_count.EOL);
                    break;
            }
        }

        fclose($fp);
    }
}
