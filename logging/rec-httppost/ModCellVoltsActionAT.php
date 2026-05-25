  <?php
    session_start();

    ini_set('display_errors', 'On');
    error_reporting(E_ALL | E_STRICT);
    date_default_timezone_set("Pacific/Auckland");

    $servername = "jstulen.netfirmsmysql.com";
    $username = "timluser";
    $password = "userML";  //your database password
    $dbname = "battery_data";  //your database name


    $con1 = new mysqli($servername, $username, $password, $dbname);

    if ($con1->connect_error) {
        die("Connection failed: " . $con1->connect_error);
    }
    else
    {
        //echo ("Connect Successfully");
    }

    $syr = $_POST["StartYR"];
    $smo = $_POST["StartMO"];
    $sday = $_POST["StartDAY"];
    $shr = $_POST["StartHR"];
    $smin = $_POST["StartMIN"];
    $startdt = new datetime;
    $startdt->setDate( $syr, $smo, $sday);
    $startdt->setTime( $shr, $smin);
  
    // save the actual time from the index page
    $_SESSION['starttext'] = $startdt->format('Y-m-d H:i:s');
  
    // adjust the time for fucking daylight savings time
    //$startdt->sub(new DateInterval('PT1H')); // sub 1 hour to convert to NZST during summer
  
    $eyr = $_POST["EndYR"];
    $emo = $_POST["EndMO"];
    $eday = $_POST["EndDAY"];
    $ehr = $_POST["EndHR"];
    $emin = $_POST["EndMIN"];
    $enddt = new datetime;
    $enddt->setDate( $eyr, $emo, $eday);
    $enddt->setTime( $ehr, $emin);
  
    // save the actual time from the index page
    $_SESSION['endtext'] = $enddt->format('Y-m-d H:i:s');

    // adjust the time for fucking daylight savings time
    //$enddt->sub(new DateInterval('PT1H')); // sub 1 hour to convert to NZST during summer

    $starttext = $startdt->format('Y-m-d H:i:s');
    $endtext = $enddt->format('Y-m-d H:i:s');
  
    // echo ($starttext);
    // echo ($endtext);


    //--------------------------------------------------------------------------------------------------------------

    // Anytime
    $timerange = " Timestamp >= '" . $starttext . "' AND Timestamp <= '" . $endtext . "'";
    $t0timerange = " t0.Timestamp >= '" . $starttext . "' AND t0.Timestamp <= '" . $endtext . "'";

    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully");
    }
    $query = " SELECT TimeStamp, CellVolt1, CellVolt2, CellVolt3, CellVolt4, CellVolt5, CellVolt6" .
    " FROM CellVoltage" .
    " WHERE Module = 1" .
    " AND " . $timerange; 

    //  echo ($query);
    $resultV = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------

?>
<html>
<!-------------------------------------------------------------------------------------------------------------->
<head>
<style> 
*{ Color : #fff; 
  Background : #000;} 
</style>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['corechart']});
      google.charts.setOnLoadCallback(drawCharts);

      function drawCharts() {
         //--------------------------------------------------------------------------------------------------------------
        var dataV = new google.visualization.DataTable();
        dataV.addColumn('datetime', 'TimeStamp');
        dataV.addColumn('number', 'CellVolt1');
        dataV.addColumn('number', 'CellVolt2');
        dataV.addColumn('number', 'CellVolt3');
        dataV.addColumn('number', 'CellVolt4');
        dataV.addColumn('number', 'CellVolt5');
        dataV.addColumn('number', 'CellVolt6');

        dataV.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultV);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  $sec = substr($dt,17,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min.",".$sec."), ".$row["CellVolt1"].", ".$row["CellVolt2"].", ".$row["CellVolt3"].", ".$row["CellVolt4"].", ".$row["CellVolt5"].", ".$row["CellVolt6"]."]";
                  while($row = mysqli_fetch_assoc($resultV)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        $sec = substr($dt,17,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min.",".$sec."), ".$row["CellVolt1"].", ".$row["CellVolt2"].", ".$row["CellVolt3"].", ".$row["CellVolt4"].", ".$row["CellVolt5"].", ".$row["CellVolt6"]."]";
                    }
                ?>
               ])
        var optionsV = {
          title: 'All Cell Voltages (V)',
          legend: { position: 'bottom' },
          legendTextStyle: { color: '#FFF' },
          titleTextStyle: { color: '#FFF' },
          hAxis: { textStyle:{color: '#FFF'} },
          vAxis: { viewWindow: { min: 3.0, max: 4.0}, textStyle:{color: '#FFF'} },
          backgroundColor: { fill: "#000000" },
          'chartArea': {
                'backgroundColor': {
                'fill': '#000000',
                'opacity': 1
            },
           }
        };
        var chartV = new google.visualization.LineChart(document.getElementById('curve_chartV'));
        chartV.draw(dataV, optionsV);
         //--------------------------------------------------------------------------------------------------------------

        // Individual charts for each CellVolt
        drawIndividualChart(dataV, 1, 'curve_chartV1');
        drawIndividualChart(dataV, 2, 'curve_chartV2');
        drawIndividualChart(dataV, 3, 'curve_chartV3');
        drawIndividualChart(dataV, 4, 'curve_chartV4');
        drawIndividualChart(dataV, 5, 'curve_chartV5');
        drawIndividualChart(dataV, 6, 'curve_chartV6');
      }

      function drawIndividualChart(fullData, cellVoltIndex, elementId) {
        var data = new google.visualization.DataTable();
        data.addColumn('datetime', 'TimeStamp');
        data.addColumn('number', 'CellVolt' + cellVoltIndex);

        var numRows = fullData.getNumberOfRows();
        for (var i = 0; i < numRows; i++) {
          data.addRows([[fullData.getValue(i, 0), fullData.getValue(i, cellVoltIndex)]]);
        }

        var options = {
          title: 'Cell Voltage ' + cellVoltIndex + ' (V)',
          legend: { position: 'bottom' },
          legendTextStyle: { color: '#FFF' },
          titleTextStyle: { color: '#FFF' },
          hAxis: { textStyle:{color: '#FFF'} },
          vAxis: { viewWindow: { min: 3.0, max: 4.0}, textStyle:{color: '#FFF'} },
          pointSize: 2,
          lineWidth: 1,
          backgroundColor: { fill: "#000000" },
          'chartArea': {
                'backgroundColor': {
                'fill': '#000000',
                'opacity': 1
            },
           }
        };

        var chart = new google.visualization.LineChart(document.getElementById(elementId));
        chart.draw(data, options);
      }
    </script>
  </head>
  <body>
    <form action="ModCellVoltsIndexAT.php">
    <input type="Submit" value="RETURN" style="background-color: white; color: black;">
    </form>
    <div id="curve_chartV" style="width: 1500px; height: 600px"></div>

    <div id="curve_chartV1" style="width: 1500px; height: 600px"></div>
    <div id="curve_chartV2" style="width: 1500px; height: 600px"></div>
    <div id="curve_chartV3" style="width: 1500px; height: 600px"></div>
    <div id="curve_chartV4" style="width: 1500px; height: 600px"></div>
    <div id="curve_chartV5" style="width: 1500px; height: 600px"></div>
    <div id="curve_chartV6" style="width: 1500px; height: 600px"></div>

  </body>
</html>


  