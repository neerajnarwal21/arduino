<?php
error_reporting(E_ALL);//show any errors if there is any
ini_set('display_errors', '1');
if(isset($_GET['t'])) { 
    // Check if temperature is present or not
    $str = '';     // intialize empty string
    if(file_exists('tempLightDataenc.xml')) { 
        // if the xml file already exists then 
        // read it
        $str = file_get_contents('tempLightDataenc.xml'); // get all the current data     
    }
    if(strlen($str) == 0) {
        // initialize the variable to the empty xml if 
        // there is no old content
        $str = "<?xml version='1.0' encoding='UTF-8'?> \n<kittemp></kittemp>";     
    }
    
    // create a new text for appending to the file
    $newData = "\n<record>
	<temperature>". $_GET['t']. "</temperature>
	<light>". $_GET['l']. "</light>
	<date>". date('d-m-Y H:i:s') . "</date>
	</record>\n</kittemp>"; 
    $str = str_replace("</kittemp>", $newData, $str);
    file_put_contents('tempLightDataenc.xml', $str); // write the file back to the server
    echo'1'; // return success
}
else 
    echo'0';// return failure
?>