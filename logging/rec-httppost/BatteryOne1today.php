  <?php
    include "BatteryOneinsertdataqt.php";
    $servername = "jstulen.netfirmsmysql.com";
    $username = "timluser";
    $password = "userML";  //your database password
    $dbname = "battery_data";  //your database name

    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully");
    }
    $query = " SELECT TimeStamp, Voltage as Voltage1, VoltageMin as VoltageMin1, VoltageMax as VoltageMax1" .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)"; 

    //  echo ($query);
    $resultV = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully\n");
    }
    $query =" SELECT TimeStamp, Current as Current1, " . 
    " DischargeLimit as DischargeLimit1, " .
    " ChargeLimit as ChargeLimit1 " .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)";

    //  echo ($query);
    $resultAllC = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully\n");
    }
    $query =" SELECT TimeStamp, Current as Current1, CurrentMin as CurrentMin1, CurrentMax as CurrentMax1" .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)";

    //  echo ($query);
    $resultC = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully\n");
    }
    $query =" SELECT TimeStamp, ChargeLimit as ChargeLimit1, ChargeLimitMin as ChargeLimitMin1, ChargeLimitMax as ChargeLimitMax1" .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)";

    //  echo ($query);
    $resultCL = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully\n");
    }
    $query =" SELECT TimeStamp, Current as Current1, CurrentMin as CurrentMin1, CurrentMax as CurrentMax1," . 
    " DischargeLimit as DischargeLimit1, DischargeLimitMin as DischargeLimitMin1, DischargeLimitMax as DischargeLimitMax1," .
    " ChargeLimit as ChargeLimit1, ChargeLimitMin as ChargeLimitMin1, ChargeLimitMax as ChargeLimitMax1" .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)";

    //  echo ($query);
    $resultDCL = $con->query($query);
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
    " AND TimeStamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)";

    //  echo ($query);
    $resultSE = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------

?>
<html>
  <head>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['corechart']});
      google.charts.setOnLoadCallback(drawChart);

      function drawChart() {
         //--------------------------------------------------------------------------------------------------------------
         var dataAllC = new google.visualization.DataTable();
        dataAllC.addColumn('datetime', 'TimeStamp');
        dataAllC.addColumn('number', 'Current1');
        dataAllC.addColumn('number', 'ChargeLimit1');
        dataAllC.addColumn('number', 'DischargeLimit1');

        dataAllC.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultAllC);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current1"].", ".$row["ChargeLimit1"].", ".$row["DischargeLimit1"]."]";
                  while($row = mysqli_fetch_assoc($resultAllC)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current1"].", ".$row["ChargeLimit1"].", ".$row["DischargeLimit1"]."]";
                    }
                ?>
               ])
        var optionsAllC = {
          title: 'Current Values Summary (Amps)',
          legend: { position: 'bottom' }//,
          //vAxis: { viewWindow: { min: 20, max: 60} }
          // vAxis: { ticks: [15,20,25,30,35,40,45,50,55,60,65] }
        };
        var chartAllC = new google.visualization.LineChart(document.getElementById('curve_chartAllC'));
        chartAllC.draw(dataAllC, optionsAllC);
         //--------------------------------------------------------------------------------------------------------------
        var dataV = new google.visualization.DataTable();
        dataV.addColumn('datetime', 'TimeStamp');
        dataV.addColumn('number', 'Voltage1');
        dataV.addColumn('number', 'VoltageMin1');
        dataV.addColumn('number', 'VoltageMax1');

        dataV.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultV);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Voltage1"].", ".$row["VoltageMin1"].", ".$row["VoltageMax1"]."]";
                  while($row = mysqli_fetch_assoc($resultV)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Voltage1"].", ".$row["VoltageMin1"].", ".$row["VoltageMax1"]."]";
                    }
                ?>
               ])
        var optionsV = {
          title: 'Voltage (V)',
          legend: { position: 'bottom' }//,
          //vAxis: { viewWindow: { min: 20, max: 60} }
          // vAxis: { ticks: [15,20,25,30,35,40,45,50,55,60,65] }
        };
        var chartV = new google.visualization.LineChart(document.getElementById('curve_chartV'));
        chartV.draw(dataV, optionsV);
         //--------------------------------------------------------------------------------------------------------------
         var dataC = new google.visualization.DataTable();
        dataC.addColumn('datetime', 'TimeStamp');
        dataC.addColumn('number', 'Current1');
        dataC.addColumn('number', 'CurrentMin1');
        dataC.addColumn('number', 'CurrentMax1');

        dataC.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultC);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current1"].", ".$row["CurrentMin1"].", ".$row["CurrentMax1"]."]";
                  while($row = mysqli_fetch_assoc($resultC)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current1"].", ".$row["CurrentMin1"].", ".$row["CurrentMax1"]."]";
                    }
                ?>
               ])
        var optionsC = {
          title: 'Current (A)',
          legend: { position: 'bottom' }//,
        };
        var chartC = new google.visualization.LineChart(document.getElementById('curve_chartC'));
        chartC.draw(dataC, optionsC);
         //--------------------------------------------------------------------------------------------------------------
         var dataCL = new google.visualization.DataTable();
        dataCL.addColumn('datetime', 'TimeStamp');
        dataCL.addColumn('number', 'ChargeLimit1');
        dataCL.addColumn('number', 'ChargeLimitMin1');
        dataCL.addColumn('number', 'ChargeLimitMax1');

        dataCL.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultCL);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["ChargeLimit1"].", ".$row["ChargeLimitMin1"].", ".$row["ChargeLimitMax1"]."]";
                  while($row = mysqli_fetch_assoc($resultCL)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["ChargeLimit1"].", ".$row["ChargeLimitMin1"].", ".$row["ChargeLimitMax1"]."]";
                    }
                ?>
               ])
        var optionsCL = {
          title: 'Charge Current Limit (A)',
          legend: { position: 'bottom' }//,
        };
        var chartCL = new google.visualization.LineChart(document.getElementById('curve_chartCL'));
        chartCL.draw(dataCL, optionsCL);
         //--------------------------------------------------------------------------------------------------------------
         var dataDCL = new google.visualization.DataTable();
        dataDCL.addColumn('datetime', 'TimeStamp');
        dataDCL.addColumn('number', 'DischargeLimit1');
        dataDCL.addColumn('number', 'DischargeLimitMin1');
        dataDCL.addColumn('number', 'DischargeLimitMax1');

        dataDCL.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultDCL);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["DischargeLimit1"].", ".$row["DischargeLimitMin1"].", ".$row["DischargeLimitMax1"]."]";
                  while($row = mysqli_fetch_assoc($resultDCL)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["DischargeLimit1"].", ".$row["DischargeLimitMin1"].", ".$row["DischargeLimitMax1"]."]";
                    }
                ?>
               ])
        var optionsDCL = {
          title: 'Discharge Current Limit (A)',
          legend: { position: 'bottom' }//,
        };
        var chartDCL = new google.visualization.LineChart(document.getElementById('curve_chartDCL'));
        chartDCL.draw(dataDCL, optionsDCL);
         //--------------------------------------------------------------------------------------------------------------
         var dataSE = new google.visualization.DataTable();
        dataSE.addColumn('datetime', 'TimeStamp');
        dataSE.addColumn('number', 'StoredEnergy');
        dataSE.addColumn('number', 'SOCPercent');
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
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["StoredEnergy"].", ".$row["SOCPercent"].", ".$row["Temperature"]."]";
                  while($row = mysqli_fetch_assoc($resultSE)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["StoredEnergy"].", ".$row["SOCPercent"].", ".$row["Temperature"]."]";
                    }
                ?>
               ])
        var optionsSE = {
          title: 'Stored Energy (kWh)  SOC (%)  Temperature (degC)',
          legend: { position: 'bottom' }//,
        };
        var chartSE = new google.visualization.LineChart(document.getElementById('curve_chartSE'));
        chartSE.draw(dataSE, optionsSE);
         //--------------------------------------------------------------------------------------------------------------
  
      }
    </script>
  </head>
  <body>
    <div id="curve_chartAllC" style="width: 1100px; height: 500px"></div>
    <div id="curve_chartSE" style="width: 1100px; height: 500px"></div>
    <div id="curve_chartV" style="width: 1100px; height: 500px"></div>
    <div id="curve_chartC" style="width: 1100px; height: 500px"></div>
    <div id="curve_chartCL" style="width: 1100px; height: 500px"></div>
    <div id="curve_chartDCL" style="width: 1100px; height: 500px"></div>
  </body>
</html>


  