  <?php
    include "BatteryOneinsertdataqt.php";
    $servername = "jstulen.netfirmsmysql.com";
    $username = "timluser";
    $password = "userML";  //your database password
    $dbname = "battery_data";  //your database name

    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully");
    }
    $query = "SELECT t1.TimeStamp, t1.Voltage1, t2.Voltage2 FROM" .
    " (SELECT TimeStamp, Voltage as Voltage1" .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)) t1" .
    " LEFT JOIN" .
    " (SELECT TimeStamp, Voltage as Voltage2" .
    " FROM BatteryOne" .
    " WHERE BatNum = 1" .
    " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)) t2" .
    " ON t1.TimeStamp = t2.TimeStamp";

    $resultV = $con->query($query);

   //  $con = new mysqli($servername, $username, $password, $dbname);
   //  if ($con->connect_error) {
   //      die("Connection failed: " . $con->connect_error);
   //  }
   //  else
   //  {
   //      // echo ("Connect Successfully\n");
   //  }
   //  $query = "SELECT t1.TimeStamp, t1.Voltage0, t2.Voltage1 FROM" .
   //  " (SELECT TimeStamp, Current as Current1" .
   //  " FROM BatteryOne" .
   //  " WHERE BatNum = 0" .
   //  " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)) t1" .
   //  " LEFT JOIN" .
   //  " (SELECT TimeStamp, Current as Current2" .
   //  " FROM BatteryOne" .
   //  " WHERE BatNum = 1" .
   //  " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)) t2" .
   //  " ON t1.TimeStamp = t2.TimeStamp";
   //  $resultC = $con->query($query);

?>
<html>
  <head>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['corechart']});
      google.charts.setOnLoadCallback(drawChart);

      function drawChart() {
         //--------------------------------------------------------------------------------------------------------------
        var dataV = new google.visualization.DataTable();
        dataV.addColumn('datetime', 'TimeStamp');
        dataV.addColumn('number', 'Voltage1');
        dataV.addColumn('number', 'Voltage2');

        dataV.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultV);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Voltage1"].", ".$row["Voltage2"]."]";
                  while($row = mysqli_fetch_assoc($resultV)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Voltage1"].", ".$row["Voltage2"]."]";
                    }
                ?>
               ])
        var optionsV = {
          title: 'Voltage',
          legend: { position: 'bottom' }//,
          //vAxis: { viewWindow: { min: 20, max: 60} }
          // vAxis: { ticks: [15,20,25,30,35,40,45,50,55,60,65] }
        };
        var chartV = new google.visualization.LineChart(document.getElementById('curve_chartV'));
        chartV.draw(dataV, optionsV);
         //--------------------------------------------------------------------------------------------------------------
  
      }
    </script>
  </head>
  <body>
    <div id="curve_chartV" style="width: 1800px; height: 900px"></div>
  </body>
</html>


  