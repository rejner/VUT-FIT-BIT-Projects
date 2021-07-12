<?php
/*
* IPP 2019 - Project 1
* File: parse.php
* Author: Michal Rein (xreinm00)
*/

//OS check for correct end of line characters, accessible everywhere via EOL const. variable
if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN'){
    define("EOL", "\r\n");
} else {
    define("EOL", "\n");
}

include "parse_classes/arguments_parse.php";
include  "parse_classes/parser.php";
include "parse_classes/controller.php";
include "parse_classes/scanner.php";

//new instance of controller class which controls program flow
$control = new Controller($argc, $argv);
