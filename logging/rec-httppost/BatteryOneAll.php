  <?php
    // ini_set('display_errors', 1);
    // ini_set('display_startup_errors', 1);
    // error_reporting(E_ALL);

    include "BatteryOneinsertdataqt.php";
    $servername = "jstulen.netfirmsmysql.com";
    $username = "timluser";
    $password = "userML";  //your database password
    $dbname = "battery_data";  //your database name
    //--------------------------------------------------------------------------------------------------------------

    // 2 hours
    $timerange = " Timestamp > DATE_ADD( CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+11:00'), INTERVAL -2 HOUR)";
    $t0timerange = " t0.Timestamp > DATE_ADD( CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+11:00'), INTERVAL -2 HOUR)";
    $time = $_GET['time'];
    // today
    if ($time == "td") {
      $timerange = " Timestamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+11:00') as DATE)";
      $t0timerange = "t0.Timestamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+11:00') as DATE)";
    } 
    // yesterday
    else if ($time == "yd") {
      $timerange = " TimeStamp > DATE_ADD(CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+11:00') as DATE), INTERVAL -1 DAY) AND TimeStamp < CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+11:00') as DATE)";
      $t0timerange = "t0.TimeStamp > DATE_ADD(CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+11:00') as DATE), INTERVAL -1 DAY) AND t0.TimeStamp < CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+11:00') as DATE)";
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
    $query =" SELECT TimeStamp, Current, " . 
    " (Current * Voltage * 0.001) as Power, " .
    " SOCPercent " .
    // " DischargeCurrentLimit, " .
    // " ChargeCurrentLimit " .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND " . $timerange;

    //  echo ($query);
    $resultSOCP = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully\n");
    }
    $query =" SELECT TimeStamp, StoredEnergy, SOCPercent, Temperature" .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND " . $timerange;

    //  echo ($query);
    $resultSE = $con->query($query);

$time = $_GET['time'];
if ($time != "yd" and $time != "td") {
    echo '<h1 style="font-size:2em">Charts for Last 2 Hours</h1>';
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
         var dataSOCP = new google.visualization.DataTable();
        dataSOCP.addColumn('datetime', 'TimeStamp');
        // dataSOCP.addColumn('number', 'Current');
        dataSOCP.addColumn('number', 'SOCPercent');
        dataSOCP.addColumn('number', 'Power');

        dataSOCP.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultSOCP);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  // echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current"].", ".$row["SOCPercent"].", ".$row["Power"]."]";
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["SOCPercent"].", ".$row["Power"]."]";
                  while($row = mysqli_fetch_assoc($resultSOCP)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        // echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current"].", ".$row["SOCPercent"].", ".$row["Power"]."]";
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["SOCPercent"].", ".$row["Power"]."]";
                      }
                ?>
               ])
        var optionsSOCP = {
          title: 'SOC (%), Power (kW)',
          legend: { position: 'bottom' },
          vAxes: {
            0: {viewWindow: { min: 0, max: 100}, ticks: [0,25,50,75,100] },
            1: {viewWindow: { min: -20, max: 20}, ticks: [-20,-10,0,10,20] }
          },
          series: {
            0:{targetAxisIndex:0},
            1:{targetAxisIndex:1}
          }



          // vAxis: { viewWindow: { min: -30, max: 30} }//,
          // vAxis: { ticks: [-15,-10,-5,0,5,10,15] }

        };
        var chartAllC = new google.visualization.LineChart(document.getElementById('curve_chartSOCP'));
        chartAllC.draw(dataSOCP, optionsSOCP);

        //--------------------------------------------------------------------------------------------------------------
         var dataSE = new google.visualization.DataTable();
        dataSE.addColumn('datetime', 'TimeStamp');
        dataSE.addColumn('number', 'StoredEnergy');
        // dataSE.addColumn('number', 'SOCPercent');
        dataSE.addColumn('number', 'Temperature');

        dataSE.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultSE);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  // echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["StoredEnergy"].", ".$row["SOCPercent"].", ".$row["Temperature"]."]";
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["StoredEnergy"].", ".$row["Temperature"]."]";
                  while($row = mysqli_fetch_assoc($resultSE)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        // echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["StoredEnergy"].", ".$row["SOCPercent"].", ".$row["Temperature"]."]";
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["StoredEnergy"].", ".$row["Temperature"]."]";
                    }
                ?>
               ])
        var optionsSE = {
          title: 'Stored Energy (kWh), Temperature (degC)',
          legend: { position: 'bottom' }//,
//           backgroundColor: '#000',
//           legendTextStyle: { color: '#FFF' },
//           titleTextStyle: { color: '#FFF' },
//           hAxis: {
// 			textStyle:{color: '#FFF'}
// 		  },
// 	      vAxis: {
// 			textStyle:{color: '#FFF'}
// 		  }

        };
        var chartSE = new google.visualization.LineChart(document.getElementById('curve_chartSE'));
        chartSE.draw(dataSE, optionsSE);
         //--------------------------------------------------------------------------------------------------------------
  
      }
    </script>
  </head>
  <body>
    <div id="curve_chartSOCP" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartSE" style="width: 1000px; height: 500px"></div>
  </body>
</html>


  