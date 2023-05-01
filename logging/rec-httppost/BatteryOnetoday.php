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


    $query = "SELECT t1.TimeStamp, t1.Voltage0, t2.Voltage1 FROM" .
    " (SELECT TimeStamp, Voltage as Voltage0" .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)) t1" .
    " LEFT JOIN" .
    " (SELECT TimeStamp, Voltage as Voltage1" .
    " FROM BatteryOne" .
    " WHERE BatNum = 1" .
    " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)) t2" .
    " ON t1.TimeStamp = t2.TimeStamp";

    // echo ($query);
    
    $aresult = $con->query($query);

    // $con = new mysqli($servername, $username, $password, $dbname);

    // if ($con->connect_error) {
    //     die("Connection failed: " . $con->connect_error);
    // }
    // else
    // {
    //     // echo ("Connect Successfully\n");
    // }
    // $query5 = "SELECT TimeStamp, SolarToday, ImportToday, ExportToday, (SolarToday+ImportToday+ExportToday) as ConsumedToday  FROM SolarData WHERE TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE) AND TimeStamp < '2025-01-01 00:00:00'";
    // // echo "$query<br>\n";
    // $aresult5 = $con->query($query5);

?>
<html>
  <head>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['corechart']});
      google.charts.setOnLoadCallback(drawChart);

      function drawChart() {

        var data = new google.visualization.DataTable();
        data.addColumn('datetime', 'TimeStamp');
        data.addColumn('number', 'Voltage0');
        data.addColumn('number', 'Voltage1');

        data.addRows([
                <?php
                  $row = mysqli_fetch_assoc($aresult);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Voltage0"].", ".$row["Voltage1"]."]";
                  while($row = mysqli_fetch_assoc($aresult)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Voltage0"].", ".$row["Voltage1"]."]";
                    }
                ?>
               ])

        var options = {
          title: 'Voltage',
          legend: { position: 'bottom' }//,
          //vAxis: { viewWindow: { min: 20, max: 60} }
          // vAxis: { ticks: [15,20,25,30,35,40,45,50,55,60,65] }
        };


        var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));

        chart.draw(data, options);
  
      }
    </script>
  </head>
  <body>
    <div id="curve_chart" style="width: 1800px; height: 900px"></div>
  </body>
</html>


  