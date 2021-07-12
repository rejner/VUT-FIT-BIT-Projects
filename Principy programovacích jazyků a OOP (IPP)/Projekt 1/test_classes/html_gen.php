<?php

/*HTMLGenerator*/
/*- generates results of tests in HTML 5 format */
final class HTMLGenerator{

    /*Generates basic structure of final output*/
    public function generate_header($output){

        $output = "<!DOCTYPE html>\n<html>\n<head>\n<style>\n";
        $output .= "table, th, td {\nborder: 1px solid black; \nborder-collapse: collapse; \ntext-align: center;\n}\n";
        $output .= "</style>\n</head>\n";
        $output .= "<body>\n<h2>Tests results:</h2>\n";
        $output .= "<table>\n";
        $output .= "<tr>\n<th>TEST</th>\n<th>RC</th>\n<th>EXPECTED RC</th>\n<th>DIFFER</th>\n<th>RESULT</th>\n";
        $output .= "</tr>\n";

        return $output;
    }

    /*Generates new record into final output*/
    public function generate_record($output, $test_path, $rc, $exp_rc, $differ){

        if ($rc == $exp_rc && $rc != 0) $differ = 'OK';

        $output .= "<tr>\n";
        $output .= "<td>" . $test_path . "</td>\n";
        $output .= "<td>" . $rc . "</td>\n";
        $output .= "<td>" . $exp_rc . "</td>\n";
        $output .= "<td>" . $differ . "</td>\n";

        if ($rc == $exp_rc && $differ == "OK"){
            $output .= "<td bgcolor='#adff2f'>PASSED</td>\n";
        } else {
            $output .= "<td bgcolor='red'>FAILED</td>\n";
        }

        return $output;
    }

    /*Generates end of output*/
    public function generate_end($output){

        $output .= "</table>\n</body>\n</html>\n";
        return $output;
    }


}