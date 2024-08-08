<?php
	if(isset($_GET['rfid']) && $_GET['rfid'] == '15-a1-1-d2') {
        file_put_contents('dummy.txt', "extra file to test");
		echo 'a';
	}else {
		echo '0';
	}
	// 15-a1-1-d2 Lab 6
	// 5-a0-0-d2 Lab 7
?>
