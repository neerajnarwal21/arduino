<?php
    error_reporting(E_ALL);
    //show any errors if there is any
    ini_set('display_errors', '1');
    include '../kitiotheader.php';				//include the canvasjs libraries (modified for KIT317/417)
?>
<!--<div id="chartContainer"></div>-->
<?php
if(file_exists('../../dummy.txt')) {
?>
<h1>Maintenance</h1>
<?php
}else{
    $tempPoints = array();
    $ldrpoints = array();
    
    if(isset($_GET['startdate']) && isset($_GET['enddate'])) {
        $startdate = $_GET['startdate'];
        $enddate   = $_GET['enddate'];
        if($startdate <= $enddate) {
            $xml_arr = '';  // intialize empty string
            if(file_exists('../../tempLightDataenc.xml')) {
                // if the xml file already exists then read it  
                $xml_arr = simplexml_load_file('../../tempLightDataenc.xml'); 
                // get all the current data  
                //print_r($xml_arr);  
    ?>
        <div id="chartContainer"></div>
    <?php
                foreach($xml_arr->record as $r) {
                    if(strtotime($startdate) <= strtotime($r->date) && strtotime($r->date) <= strtotime($enddate)) {
                        // push the data into the array for the displaygraph
                        array_push($tempPoints,   array("x" => strtotime($r->date) * 1000, "y" => $r->temperature . ""));
                        array_push($ldrpoints,  array("x" => strtotime($r->date) * 1000, "y" => $r->light. ""));
				
                    }
                }
                print_r($tempPoints);
            }else {
                echo "No File Found";
            }
        }else {
            echo "Date Incorrect";
        }
    }else {
        echo "No Date provided";
    }
}
?>

<script type="text/javascript">

    $(function () {
        var chart = new CanvasJS.Chart("chartContainer", {
            title: {
		text: "Temparature and Light Graph"
            },
            animationEnabled: true,
            toolTip: {
		shared: true,
		content: "<span style='\"'color: {color};'\"'><strong>{name}</strong></span> <span style='\"'color: dimgrey;'\"'>{y}</span> "
            },
            
            axisY2: {
                title: "Light",
                includeZero: false,
                suffix: " lux",
                lineColor: "#C24642"
            },
            axisY: {
                title: "Temparature",
                includeZero: false,
                suffix: " C",
                lineColor: "#369EAD"
            },
            axisX: {
                title: "Date/Time",
            },
            data: [
            {
                type: "line",
                name: "Temparature",
		xValueType: "dateTime",
                dataPoints: <?php echo json_encode($tempPoints, JSON_NUMERIC_CHECK); ?>
            },
            {
//                type: "splineArea",
//                axisYType: "secondary",
//		xValueType: "dateTime",
//                name: "Light",
//                dataPoints: <?php echo json_encode($ldrpoints, JSON_NUMERIC_CHECK); ?>
                
                type: "line",
                axisYType: "secondary",
		xValueType: "dateTime",
                name: "Light",
                dataPoints: <?php echo json_encode($ldrpoints, JSON_NUMERIC_CHECK); ?>
            }
            ]
        });

        chart.render();

    });
</script>