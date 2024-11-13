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


    $query = "SELECT " .
    "(SELECT EnergyConsumed " .
    " FROM PowerOne " .
    " WHERE Timestamp >= '2024-10-16 10:00:00' " .
    " ORDER BY Timestamp ASC " .
    " LIMIT 1) - " .
    "(SELECT EnergyConsumed " .
    " FROM PowerOne " .
    " WHERE Timestamp >= '2024-10-16 08:00:00' " .
    " ORDER BY Timestamp ASC " .
    " LIMIT 1) AS EnergyConsumedDifference, " .
    "(SELECT EnergyProduced " .
    " FROM PowerOne " .
    " WHERE Timestamp >= '2024-10-16 16:30:00' " .
    " ORDER BY Timestamp ASC " .
    " LIMIT 1) - " .
    "(SELECT EnergyProduced " .
    " FROM PowerOne " .
    " WHERE Timestamp >= '2024-10-16 07:00:00' " .
    " ORDER BY Timestamp ASC " .
    " LIMIT 1) AS EnergyProducedDifference;";

    // SELECT 
    // -- EnergyConsumed Difference
    // (SELECT EnergyConsumed 
    //  FROM PowerOne 
    //  WHERE Timestamp >= '2024-10-15 22:00:00' 
    //  ORDER BY Timestamp ASC 
    //  LIMIT 1) - 
    // (SELECT EnergyConsumed 
    //  FROM PowerOne 
    //  WHERE Timestamp >= '2024-10-15 0:00:00' 
    //  ORDER BY Timestamp ASC 
    //  LIMIT 1) AS EnergyConsumedDifference,

    // -- EnergyProduced Difference
    // (SELECT EnergyProduced 
    //  FROM PowerOne 
    //  WHERE Timestamp >= '2024-10-15 22:00:00' 
    //  ORDER BY Timestamp ASC 
    //  LIMIT 1) - 
    // (SELECT EnergyProduced 
    //  FROM PowerOne 
    //  WHERE Timestamp >= '2024-10-15 0:00:00' 
    //  ORDER BY Timestamp ASC 
    //  LIMIT 1) AS EnergyProducedDifference;


    // $query = " SELECT TimeStamp, Voltage, VoltageMin, VoltageMax " .
    // " FROM BatteryOne" .
    // " WHERE BatNum = 0" .
    // " AND " . $timerange; 

    //  echo ($query);
    $resultE = $con->query($query);
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
    // $con = new mysqli($servername, $username, $password, $dbname);
    // if ($con->connect_error) {
    //     die("Connection failed: " . $con->connect_error);
    // }
    // else
    // {
    //     // echo ("Connect Successfully\n");
    // }
    // $query =" SELECT TimeStamp, " . 
    //  "PowerL1Avg + PowerL2Avg + PowerL3Avg as PowerCons, " .
    //  "PowerL1Avg + PowerL2Avg + PowerL3Avg - InvPowerAvg as PowerNet, InvPowerAvg " .
    //  " FROM PowerOne" .
    //  " WHERE " . $timerange;
    // // echo ($query);
    // $resultPC = $con->query($query);
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
    // echo ($query);
    $resultP1mm = $con->query($query);
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
    "PowerL2Avg, PowerL2Min, PowerL2Max " .
    " FROM PowerOne" .
     " WHERE " . $timerange;
    // echo ($query);
    $resultP2mm = $con->query($query);
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
    "PowerL3Avg, PowerL3Min, PowerL3Max " .
    " FROM PowerOne" .
     " WHERE " . $timerange;

    // echo ($query);

    $resultP3mm = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------
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
          title: 'Power All Avg (W)',
          legend: { position: 'bottom' },

        };
        var chartPAvg = new google.visualization.LineChart(document.getElementById('curve_chartPAvg'));
        chartPAvg.draw(dataPAvg, optionsPAvg);
         //--------------------------------------------------------------------------------------------------------------
        //  var dataPC = new google.visualization.DataTable();
        // dataPC.addColumn('datetime', 'TimeStamp');
        // dataPC.addColumn('number', 'PowerCons');
        // dataPC.addColumn('number', 'PowerNet');
        // dataPC.addColumn('number', 'InvPowerAvg');

        // dataPC.addRows([
        //         <?php
        //           $row = mysqli_fetch_assoc($resultPC);
        //           $dt = $row["TimeStamp"];
        //           $yr = substr($dt,0,4);
        //           $mo = substr($dt,5,2);
        //           $day = substr($dt,8,2);
        //           $hr = substr($dt,11,2);
        //           $min = substr($dt,14,2);
        //           echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerCons"].", ".$row["PowerNet"].", ".$row["InvPowerAvg"]."]";
        //             // echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerL1Avg"].", ".$row["PowerL2Avg"].", ".$row["PowerL3Avg"].", ".$row["PowerCons"].", "
        //             // .$row["PowerNet"].", ".$row["InvPowerAvg"]."]";
        //           while($row = mysqli_fetch_assoc($resultPC)){
        //             $dt = $row["TimeStamp"];
        //             $yr = substr($dt,0,4);
        //             $mo = substr($dt,5,2);
        //             $day = substr($dt,8,2);
        //             $hr = substr($dt,11,2);
        //             $min = substr($dt,14,2);
        //           echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerCons"].", ".$row["PowerNet"].", ".$row["InvPowerAvg"]."]";
        //           }
        //         ?>
        //        ])
        // var optionsPC = {
        //   title: 'Power All Avg (W)',
        //   legend: { position: 'bottom' },

        // };
        // var chartPC = new google.visualization.LineChart(document.getElementById('curve_chartPC'));
        // chartPC.draw(dataPC, optionsPC);
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
          title: 'Power L1 (W)',
          legend: { position: 'bottom' },

        };
        var chartP1mm = new google.visualization.LineChart(document.getElementById('curve_chartP1mm'));
        chartP1mm.draw(dataP1mm, optionsP1mm);
         //--------------------------------------------------------------------------------------------------------------
         var dataP2mm = new google.visualization.DataTable();
        dataP2mm.addColumn('datetime', 'TimeStamp');
        dataP2mm.addColumn('number', 'PowerL2Avg');
        dataP2mm.addColumn('number', 'PowerL2Min');
        dataP2mm.addColumn('number', 'PowerL2Max');

        dataP2mm.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultP2mm);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerL2Avg"].", ".$row["PowerL2Min"].", ".$row["PowerL2Max"]."]";

                  while($row = mysqli_fetch_assoc($resultP2mm)){
                    $dt = $row["TimeStamp"];
                    $yr = substr($dt,0,4);
                    $mo = substr($dt,5,2);
                    $day = substr($dt,8,2);
                    $hr = substr($dt,11,2);
                    $min = substr($dt,14,2);
                    echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerL2Avg"].", ".$row["PowerL2Min"].", ".$row["PowerL2Max"]."]";

                  }
                ?>
               ])
        var optionsP2mm = {
          title: 'Power L2 (W)',
          legend: { position: 'bottom' },

        };
        var chartP2mm = new google.visualization.LineChart(document.getElementById('curve_chartP2mm'));
        chartP2mm.draw(dataP2mm, optionsP2mm);

         //--------------------------------------------------------------------------------------------------------------
         var dataP3mm = new google.visualization.DataTable();
        dataP3mm.addColumn('datetime', 'TimeStamp');
        dataP3mm.addColumn('number', 'PowerL3Avg');
        dataP3mm.addColumn('number', 'PowerL3Min');
        dataP3mm.addColumn('number', 'PowerL3Max');

        dataP3mm.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultP3mm);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerL3Avg"].", ".$row["PowerL3Min"].", ".$row["PowerL3Max"]."]";

                  while($row = mysqli_fetch_assoc($resultP3mm)){
                    $dt = $row["TimeStamp"];
                    $yr = substr($dt,0,4);
                    $mo = substr($dt,5,2);
                    $day = substr($dt,8,2);
                    $hr = substr($dt,11,2);
                    $min = substr($dt,14,2);
                    echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["PowerL3Avg"].", ".$row["PowerL3Min"].", ".$row["PowerL3Max"]."]";

                  }
                ?>
               ])
        var optionsP3mm = {
          title: 'Power L3(W)',
          legend: { position: 'bottom' },

        };
        var chartP3mm = new google.visualization.LineChart(document.getElementById('curve_chartP3mm'));
        chartP3mm.draw(dataP3mm, optionsP3mm);

        //--------------------------------------------------------------------------------------------------------------
  
      }
    </script>
  </head>
  <body>
  <form action="PowerOneIndexAT.php">
    <input type="Submit" value="RETURN">
    </form>
    <br><A HREF="https://jimster.ca/BatteryOne/index.php" >Main Menu</A>
  
    <div id="curve_chartPAvg" style="width: 1000px; height: 500px"></div>
    <!-- <div id="curve_chartPC" style="width: 1000px; height: 500px"></div> -->
    <div id="curve_chartP1mm" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartP2mm" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartP3mm" style="width: 1000px; height: 500px"></div>
  </body>
</html>


  