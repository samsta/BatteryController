  <?php
    include "BatteryOneinsertdataqt.php";
    $servername = "jstulen.netfirmsmysql.com";
    $username = "timluser";
    $password = "userML";  //your database password
    $dbname = "battery_data";  //your database name

    $timestart = "DATE_ADD( CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00')";
    $timeinterval = ", INTERVAL -2 HOUR)";
    $batnum = 1;
    $wherebatnum = " WHERE BatNum = " . $batnum;

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
    $wherebatnum .
    " AND Timestamp > " . $timestart . $timeinterval; 

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
    " (Current * Voltage * 0.001) as Power1, " .
    " DischargeCurrentLimit as DischargeCurrentLimit1, " .
    " ChargeCurrentLimit as ChargeCurrentLimit1 " .
    " FROM BatteryOne" .
    $wherebatnum .
    " AND Timestamp > " . $timestart . $timeinterval;

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
    $wherebatnum .
    " AND Timestamp > " . $timestart . $timeinterval;

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
    $query =" SELECT TimeStamp, ChargeCurrentLimit as ChargeCurrentLimit1, ChargeCurrentLimitMin as ChargeCurrentLimitMin1, ChargeCurrentLimitMax as ChargeCurrentLimitMax1" .
    " FROM BatteryOne" .
    $wherebatnum .
    " AND Timestamp > " . $timestart . $timeinterval;

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
    " DischargeCurrentLimit as DischargeCurrentLimit1, DischargeCurrentLimitMin as DischargeCurrentLimitMin1, DischargeCurrentLimitMax as DischargeCurrentLimitMax1," .
    " ChargeCurrentLimit as ChargeCurrentLimit1, ChargeCurrentLimitMin as ChargeCurrentLimitMin1, ChargeCurrentLimitMax as ChargeCurrentLimitMax1" .
    " FROM BatteryOne" .
    $wherebatnum .
    " AND Timestamp > " . $timestart . $timeinterval;

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
    $wherebatnum .
    " AND Timestamp > " . $timestart . $timeinterval;

    //  echo ($query);
    $resultSE = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully\n");
    }
    $query =" SELECT TimeStamp, SOCPercent, StoredEnergy, Current, DischargeCurrentLimit, ChargeCurrentLimit, Voltage, Temperature" .
    " FROM BatteryOne" .
    $wherebatnum .
    " AND Timestamp > ". $timestart . ", INTERVAL -10 MINUTE) ORDER BY TimeStamp DESC LIMIT 1";

    $resultTBL = $con->query($query);
    $rows = mysqli_fetch_assoc($resultTBL);
    $SOC = $rows["SOCPercent"];
    $StoredEnergy = $rows["StoredEnergy"];
    $Current = $rows["Current"];
    $DischargeCurrentLimit = $rows["DischargeCurrentLimit"];
    $ChargeCurrentLimit = $rows["ChargeCurrentLimit"];
    $Voltage = $rows["Voltage"];
    $Temperature = $rows["Temperature"];
    $DTm = $rows["TimeStamp"];
    //--------------------------------------------------------------------------------------------------------------
  	date_default_timezone_set("Pacific/Auckland");
	  // $thetimeis = "<h4>The time is " . date("H:i:s") . "</h4>"
	  $thetimeis = "Newest Data: " . $DTm;

?>
<html>
<h4><p><?=$thetimeis?></p></h4>

<head>
<style>
table, th, td {
  border: 1px solid black;
  border-collapse: collapse;
}
<meta charset="UTF-8">
    <title>Battery One</title>
    <style>
        table,
        td,
        th {
            border: 1px solid;
            padding: 20px;
        }
 
        table {
            text-align: center;
        }
    </style>
</style>
</head>

<body>
<table style="width: 70%;">
  <!-- <tbody style="font-size: 12px";> -->
  <tr>
    <th>SOC</th>
    <th>Energy</th>
    <th>Current</th>
    <th>Power</th>
    <th>Chg.Cur.Lmt</th>
    <th>DChg.Cur.Lmt</th>
    <th>Voltage</th>
    <th>Temperature</th>
  </tr>
  <tr>
    <td><p><?=round($SOC,1)?> %</p></td>
    <td><p><?=round($StoredEnergy,1)?> kWh</p></td>
    <td><p><?=round($Current,1)?> A</p></td>
    <td><p><?=round($Voltage*$Current/1000,1)?> kW</p></td>
    <td><p><?=round($ChargeCurrentLimit,1)?> A</p></td>
    <td><p><?=round($DischargeCurrentLimit,1)?> A</p></td>
    <td><p><?=round($Voltage,1)?> V</p></td>
    <td><p><?=round($Temperature,1)?> C</p></td>
  </tr>
</table>
</body>
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
        dataAllC.addColumn('number', 'ChargeCurrentLimit1');
        dataAllC.addColumn('number', 'Power1');
        dataAllC.addColumn('number', 'DischargeCurrentLimit1');

        dataAllC.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultAllC);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current1"].", ".$row["ChargeCurrentLimit1"].", ".$row["Power1"].", ".$row["DischargeCurrentLimit1"]."]";
                  while($row = mysqli_fetch_assoc($resultAllC)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current1"].", ".$row["ChargeCurrentLimit1"].", ".$row["Power1"].", ".$row["DischargeCurrentLimit1"]."]";
                    }
                ?>
               ])
        var optionsAllC = {
          title: 'Current Values Summary (Amps)',
          legend: { position: 'bottom' }//,
          //vAxis: { viewWindow: { min: 20, max: 60} }
          // vAxis: { ticks: [15,20,25,30,35,40,45,50,55,60,65] }
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
        dataCL.addColumn('number', 'ChargeCurrentLimit1');
        dataCL.addColumn('number', 'ChargeCurrentLimitMin1');
        dataCL.addColumn('number', 'ChargeCurrentLimitMax1');

        dataCL.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultCL);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["ChargeCurrentLimit1"].", ".$row["ChargeCurrentLimitMin1"].", ".$row["ChargeCurrentLimitMax1"]."]";
                  while($row = mysqli_fetch_assoc($resultCL)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["ChargeCurrentLimit1"].", ".$row["ChargeCurrentLimitMin1"].", ".$row["ChargeCurrentLimitMax1"]."]";
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
        dataDCL.addColumn('number', 'DischargeCurrentLimit1');
        dataDCL.addColumn('number', 'DischargeCurrentLimitMin1');
        dataDCL.addColumn('number', 'DischargeCurrentLimitMax1');

        dataDCL.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultDCL);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["DischargeCurrentLimit1"].", ".$row["DischargeCurrentLimitMin1"].", ".$row["DischargeCurrentLimitMax1"]."]";
                  while($row = mysqli_fetch_assoc($resultDCL)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["DischargeCurrentLimit1"].", ".$row["DischargeCurrentLimitMin1"].", ".$row["DischargeCurrentLimitMax1"]."]";
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
    <div id="curve_chartAllC" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartSE" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartV" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartC" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartCL" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartDCL" style="width: 1000px; height: 500px"></div>
  </body>
</html>


  