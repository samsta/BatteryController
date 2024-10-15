  <?php
    // ini_set('display_errors', 1);
    // ini_set('display_startup_errors', 1);
    // error_reporting(E_ALL);

    include "PowerOneinsertdataOEMSqt.php";
    $servername = "jstulen.netfirmsmysql.com";
    $username = "timluser";
    $password = "userML";  //your database password
    $dbname = "battery_data";  //your database name
    //--------------------------------------------------------------------------------------------------------------

    // 2 hours
    $timerange = " Timestamp > DATE_ADD( CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00'), INTERVAL -2 HOUR)";
    $t0timerange = " t0.Timestamp > DATE_ADD( CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00'), INTERVAL -2 HOUR)";
    $time = $_GET['time'];
    // today
    if ($time == "td") {
      $timerange = " Timestamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00') as DATE)";
      $t0timerange = "t0.Timestamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00') as DATE)";
    } 
    // yesterday
    else if ($time == "yd") {
      $timerange = " TimeStamp > DATE_ADD(CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00') as DATE), INTERVAL -1 DAY) AND TimeStamp < CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00') as DATE)";
      $t0timerange = "t0.TimeStamp > DATE_ADD(CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00') as DATE), INTERVAL -1 DAY) AND t0.TimeStamp < CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00') as DATE)";
    }

    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully\n");
    }
    $query =" SELECT TimeStamp, " . 
    "PowerL1Avg, PowerL2Avg, PowerL3Avg, InvPowerAvg " .

    " FROM PowerOne" .
     " WHERE " . $timerange;
    //  echo ($query);
    $resultPAvg = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully\n");
    }
    $query =" SELECT TimeStamp, " . 
    "PowerL1Avg, PowerL1Min, PowerL1Max " .
    " FROM PowerOne" .
     " WHERE " . $timerange;


    //  "PowerL1Avg + PowerL2Avg + PowerL3Avg as PowerCons, " .
    //  "PowerL1Avg + PowerL2Avg + PowerL3Avg - InvPowerAvg as PowerNet " .
    // echo ($query);

    $resultP1mm = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------
$time = $_GET['time'];
if ($time != "yd" and $time != "td") {
    echo '<h1 style="font-size:2em">Charts for Last 3 Hours</h1>';
}
elseif ($time == "yd") {
    echo '<h1 style="font-size:2em">Charts for Yesterday</h1>';
}
elseif ($time == "td") {
  echo '<h1 style="font-size:2em">Charts for Today</h1>';
}
?>
<html>
<!-------------------------------------------------------------------------------------------------------------->
<head>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['corechart']});
      google.charts.setOnLoadCallback(drawChart);

      function drawChart() {
         //--------------------------------------------------------------------------------------------------------------
         var dataPAvg = new google.visualization.DataTable();
        dataPAvg.addColumn('datetime', 'TimeStamp');
        dataPAvg.addColumn('number', 'PowerL1Avg');
        dataPAvg.addColumn('number', 'PowerL2Avg');
        dataPAvg.addColumn('number', 'PowerL3Avg');
        // dataPAvg.addColumn('number', 'PowerCons');
        // dataPAvg.addColumn('number', 'PowerNet');
        dataPAvg.addColumn('number', 'InvPowerAvg');

        dataPAvg.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultPAvg);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerL1Avg"].", ".$row["PowerL2Avg"].", ".$row["PowerL3Avg"].", ".$row["InvPowerAvg"]."]";
                    // echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerL1Avg"].", ".$row["PowerL2Avg"].", ".$row["PowerL3Avg"].", ".$row["PowerCons"].", "
                    // .$row["PowerNet"].", ".$row["InvPowerAvg"]."]";
                  while($row = mysqli_fetch_assoc($resultPAvg)){
                    $dt = $row["TimeStamp"];
                    $yr = substr($dt,0,4);
                    $mo = substr($dt,5,2);
                    $day = substr($dt,8,2);
                    $hr = substr($dt,11,2);
                    $min = substr($dt,14,2);
                    echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerL1Avg"].", ".$row["PowerL2Avg"].", ".$row["PowerL3Avg"].", ".$row["InvPowerAvg"]."]";
                    // echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerL1Avg"].", ".$row["PowerL2Avg"].", ".$row["PowerL3Avg"].", ".$row["PowerCons"]
                    // .", ".$row["PowerNet"].", ".$row["InvPowerAvg"]."]";
                  }
                ?>
               ])
        var optionsPAvg = {
          title: 'Power (kW)',
          legend: { position: 'bottom' },

        };
        var chartPAvg = new google.visualization.LineChart(document.getElementById('curve_chartPAvg'));
        chartPAvg.draw(dataPAvg, optionsPAvg);
         //--------------------------------------------------------------------------------------------------------------
         var dataP1mm = new google.visualization.DataTable();
        dataP1mm.addColumn('datetime', 'TimeStamp');
        dataP1mm.addColumn('number', 'PowerL1Avg');
        dataP1mm.addColumn('number', 'PowerL1Min');
        dataP1mm.addColumn('number', 'PowerL1Max');

        dataP1mm.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultP1mm);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerL1Avg"].", ".$row["PowerL1Min"].", ".$row["PowerL1Max"]."]";

                  while($row = mysqli_fetch_assoc($resultP1mm)){
                    $dt = $row["TimeStamp"];
                    $yr = substr($dt,0,4);
                    $mo = substr($dt,5,2);
                    $day = substr($dt,8,2);
                    $hr = substr($dt,11,2);
                    $min = substr($dt,14,2);
                    echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerL1Avg"].", ".$row["PowerL1Min"].", ".$row["PowerL1Max"]."]";

                  }
                ?>
               ])
        var optionsP1mm = {
          title: 'Power (kW)',
          legend: { position: 'bottom' },

        };
        var chartP1mm = new google.visualization.LineChart(document.getElementById('curve_chartP1mm'));
        chartP1mm.draw(dataP1mm, optionsP1mm);

        //--------------------------------------------------------------------------------------------------------------
  
      }
    </script>
  </head>
  <body>
    <div id="curve_chartPAvg" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartP1mm" style="width: 1000px; height: 500px"></div>

  </body>
</html>


  