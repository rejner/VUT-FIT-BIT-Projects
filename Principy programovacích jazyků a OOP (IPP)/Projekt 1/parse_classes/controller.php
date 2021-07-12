<?php
/*
* IPP 2019 - Project 1
* File: controller.php
* Author: Michal Rein (xreinm00)
*/

/*Controller*/
/*- initialization of argument manager and scanner class*/
/*- identification of scanner return code*/
final class Controller{

    public function __construct($argc, $argv){

        //new instance of class ArgumentManager which parses arguments
        $args = new ArgumentManager_parse($argc, $argv);

        //init scanner
        $scan = new Scanner();
        $scan_err = $scan->run_scanner($args); //runs scanner, error code is returned, otherwise 0

        // identifying returned error code...
        if ($scan_err != 0){
            switch($scan_err){
                case 1:
                    fprintf(STDERR, "ERROR: Header not found or recognized!\n");
                    exit(21);
                    break;

                case 2:
                    fprintf(STDERR, "ERROR: (line $scan->line_counter) Instruction has not been recognized!\n");
                    exit(22);
                    break;

                case 3:
                    fprintf(STDERR, "ERROR: (line $scan->line_counter) Lexical or syntax error found in the source code!\n");
                    exit(23);
                    break;

                default:
                    fprintf(STDERR, "This error code has not been implemented yet!\n");
            }
        }
    }
}