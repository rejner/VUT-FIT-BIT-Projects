<?php
/*
* IPP 2019 - Project 1
* File: test.php
* Author: Michal Rein (xreinm00)
*/

//OS check for correct end of line characters, accessible everywhere via EOL const. variable
if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN'){
    define("EOL", "\r\n");
} else {
    define("EOL", "\n");
}

define("DEBUG", False);

include "test_classes/arguments_test.php";
include "test_classes/html_gen.php";


function print_log($msg){
    if (DEBUG){
        fwrite(STDERR, $msg);
    }
}


function iterate_directory($dir_path, $args, $final_output, $html_gen){

    $files = scandir($dir_path);
    $results[] = '';

    foreach ($files as $file) {
        $path = realpath($dir_path . DIRECTORY_SEPARATOR . $file);
        if (!is_dir($path)) {

            // we found a file
            $info = pathinfo($path);

            // if we already have file with the same name (extensions may differ), continue
            if (array_search($info['filename'], $results)){
                continue;
            } else {
                // add filename into list of files
                $results[] = $info['filename'];
            }

        // if we found a directory
        } else if ($file != "." && $file != "..") {

            // if recursion is allowed, call recursively iterate_directory()
            if ($args->recursive){
                $final_output = iterate_directory($path."/", $args,  $final_output, $html_gen);
            }

        }
    }

    // we know all files which are present in current directory, we will start executing tests
    print_log(EOL."Executing tests in directory: " . $dir_path . EOL);
    $tests_passed = 0;
    $tests_failed = 0;
    foreach ($results as $result){

        // first element in array will always be ''
        if ($result == '') continue;

        // prepare paths to files we expect to find in present directory
        $src = $dir_path.$result.".src"; // source file
        $out = $dir_path.$result.".out"; // reference output file
        $rc = $dir_path.$result.".rc";   // expected return code
        $in = $dir_path.$result.".in";   // input file
        $expected_rc = ''; // variable for storing expected return code

        // we must check if the file $filename.rc even exists, if not found, expect 0 as return code
        if (file_exists($rc)){
            $rc_f = fopen($rc, 'r');
            $expected_rc = fgets($rc_f);
            fclose($rc_f);
        } else {
            $rc_f = fopen($rc, "w+");
            fwrite($rc_f, "0");
            fclose($rc_f);
            $expected_rc = 0;
        }

        if (!file_exists($in)){
            $in = '';
            touch($in);
        }

        if (!file_exists($out)){
            touch($out);
        }

        // PARSE ONLY
        if ($args->parse_only){

            // test if test source file exists, then execute parse.php with this file
            if (file_exists($src)){

                print_log("EXECUTING".EOL);

                $output = '';   // output
                $parse_rc = ''; // return code
                exec("php7.4 " . $args->parse_script . " < " . $src, $output, $parse_rc);

                $output = implode("\n", $output); // string from returned array

                // DEBUG information
                print_log("OUTPUT: ".EOL);
                print_log($output.EOL);
                print_log("RETURN CODE: " . $parse_rc . EOL.EOL);

                // create/open temporary file for output
                $tf = fopen('tmp_diff.txt', 'w+');
                fwrite($tf, $output);
                fclose($tf);

                $jxml_output = '';  // output of jexamxml tool
                $jxml_rc = '';      // return code of jexamxml tool, 0 = success, otherwise failed
                exec("java -jar " . $args->jexamxml_path . " " . $args->scripts_directory . "/tmp_diff.txt " . $out, $jxml_output, $jxml_rc);
                print_log("JXML return code: " . $jxml_rc . EOL);

                $differ = 'DIFFER';
                if ($jxml_rc == 0){
                    $differ = 'OK';
                    $tests_passed++;
                }else{
                    $tests_failed++;
                }

                // all important information was extracted, generate HTML table row
                $final_output = $html_gen->generate_record($final_output, $src, $parse_rc, $expected_rc, $differ);

            } else {
                print_log("File not found..."."(".$src.")".EOL.EOL);
            }

        // INT ONLY
        } elseif ($args->int_only){
            if (file_exists($src)){
                print_log("EXECUTING: " . $src . EOL);

                $int_output = '';
                $int_rc = '';
                exec("python3.8 " . $args->int_script . " --source=". $src . " --input=" . $in . " > tmp_int.txt", $int_output, $int_rc);

                if (file_exists($out) and $int_rc == 0){

                    $diff_output = '';
                    $diff_rc = '';
                    exec("diff ". $out . " tmp_int.txt", $diff_output, $diff_rc);
                    $differ = 'DIFFER';

                    if ($diff_rc == 0){
                        $differ = 'OK';
                        $tests_passed++;
                    } else {
                        fwrite(STDERR, "FAILED: ". implode($diff_output).EOL);
                        $tests_failed++;
                    }

                } elseif (!file_exists($out) and $int_rc == 0){
                    $differ = "NOT FOUND";
                    $tests_failed++;
                } elseif ($int_rc == $expected_rc){
                    $differ = 'OK';
                    $tests_passed++;
                } else {
                    $differ = 'DIFFER';
                    $tests_failed++;
                }
                // all important information was extracted, generate HTML table row
                $final_output = $html_gen->generate_record($final_output, $src, $int_rc, $expected_rc, $differ);

            } else {
                print_log("File not found..."."(".$src.")".EOL.EOL);
            }

        // BOTH
        } elseif (!$args->parse_only and !$args->int_only){

            if (file_exists($src)){
                print_log("Executing: " . $src . EOL);
                $parse_output = ''; // parser output
                $parse_rc = ''; // parser return code
                exec("php7.4 " . $args->parse_script . " < " . $src, $parse_output, $parse_rc);
                $parse_output = implode("\n", $parse_output); // string from returned array

                // DEBUG information
                print_log("OUTPUT: ".EOL);
                print_log($parse_output.EOL);
                print_log("RETURN CODE: " . $parse_rc . EOL.EOL);

                // create/open temporary file for output
                $tf = fopen('tmp_parse.txt', 'w+');
                fwrite($tf, $parse_output);
                fclose($tf);

                $int_output = ''; // interpret output
                $int_rc = ''; // interpret return code
                // execute interpret with generated
                exec("python3.8 " . $args->int_script . " --source=tmp_parse.txt" . " --input=" . $in . " > tmp_int.txt", $int_output, $int_rc);

                $differ = 'OK';
                if (file_exists($out) and $int_rc == 0){

                    $diff_output = '';
                    $diff_rc = '';
                    exec("diff ". $out . " tmp_int.txt", $diff_output, $diff_rc);
                    $differ = 'DIFFER';

                    if ($diff_rc == 0){
                        $differ = 'OK';
                        $tests_passed++;
                    } else {
                        fwrite(STDERR, "FAILED: ". implode($diff_output).EOL);
                        $tests_failed++;
                    }

                } elseif (!file_exists($out) and $int_rc == 0){
                    $differ = "NOT FOUND";
                    $tests_failed++;
                } elseif ($int_rc == $expected_rc){
                    $tests_passed++;
                } else {
                    $tests_failed++;
                }
                // all important information was extracted, generate HTML table row
                $final_output = $html_gen->generate_record($final_output, $src, $int_rc, $expected_rc, $differ);

            } else {
                print_log("File not found..."."(".$src.")".EOL.EOL);
            }
        }
    }
    $html_gen->passed = $html_gen->passed + $tests_passed;
    $html_gen->failed = $html_gen->failed + $tests_failed;

    return $final_output;

}

/*------------MAIN-------------*/

//check arguments, fill argument manager with information
$args = new ArgumentManager_test($argc, $argv);

//set path for tests
$tests_path = '';
if ($args->directory != ''){
    $tests_path = $args->directory;
} else {
    $tests_path = $args->scripts_directory;
}

print($tests_path);

$html_gen = new HTMLGenerator();
$final_output = '';
$final_output = $html_gen->generate_header($final_output);

$final_output = iterate_directory($tests_path, $args, $final_output, $html_gen);

// all tests were executed, generate end of HTML file
$final_output = $html_gen->generate_end($final_output);

print($final_output);
?>
