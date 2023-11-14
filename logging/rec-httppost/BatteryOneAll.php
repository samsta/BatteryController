  <?php
    include "BatteryOneinsertdataqt.php";
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
      $timerange = " Timestamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)";
      $t0timerange = "t0.Timestamp > CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)";
    } 
    // yesterday
    else if ($time == "yd") {
      $timerange = " TimeStamp > DATE_ADD(CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE), INTERVAL -1 DAY) AND TimeStamp < CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)";
      $t0timerange = "t0.TimeStamp > DATE_ADD(CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE), INTERVAL -1 DAY) AND t0.TimeStamp < CAST(CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+12:00') as DATE)";
    }

    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully");
    }
    $query = " SELECT TimeStamp, Voltage, VoltageMin, VoltageMax " .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND " . $timerange; 

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
    $query =" SELECT TimeStamp, Current, " . 
    " (Current * Voltage * 0.001) as Power, " .
    " DischargeCurrentLimit, " .
    " ChargeCurrentLimit " .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND " . $timerange;

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
    // $query =" SELECT TimeStamp, Current as Current1, CurrentMin as CurrentMin1, CurrentMax as CurrentMax1" .
    // " FROM BatteryOne" .
    // " WHERE BatNum = 0" .
    // " AND " . $timerange;
    $query = "SELECT t0.TimeStamp, t0.Current as CurrentTotal," .
    " t1.TimeStamp, t1.Current as Current1," .
    " t2.TimeStamp, t2.Current as Current2," .
    " t3.TimeStamp, t3.Current as Current3" .
    " FROM BatteryOne as t0" .
    " join BatteryOne as t1 ON t0.TimeStamp = t1.TimeStamp" .
    " join BatteryOne as t2 ON t0.TimeStamp = t2.TimeStamp" .
    " join BatteryOne as t3 ON t0.TimeStamp = t3.TimeStamp" .
    " WHERE" .
    " t0.BatNum = 0 AND" .
    " t1.BatNum = 1 AND" .
    " t2.BatNum = 2 AND" .
    " t3.BatNum = 3 AND " .$t0timerange;

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
    $query =" SELECT TimeStamp, ChargeCurrentLimit, ChargeCurrentLimitMin, ChargeCurrentLimitMax" .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND " . $timerange;

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
    $query =" SELECT TimeStamp, DischargeCurrentLimit, DischargeCurrentLimitMin, DischargeCurrentLimitMax" .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND " . $timerange;

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
    " AND " . $timerange;

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
    $query =" SELECT TimeStamp, Current, CurrentMin, CurrentMax" .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND " . $timerange;

    //  echo ($query);
    $resultCC = $con->query($query);
    //--------------------------------------------------------------------------------------------------------------
    $con = new mysqli($servername, $username, $password, $dbname);
    if ($con->connect_error) {
        die("Connection failed: " . $con->connect_error);
    }
    else
    {
        // echo ("Connect Successfully\n");
    }
    $query =" SELECT TimeStamp, CellVoltageMin, CellVoltageMax" .
    " FROM BatteryOne" .
    " WHERE BatNum = 0" .
    " AND " . $timerange;

    //  echo ($query);
    $resultCV = $con->query($query);
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
    " WHERE BatNum = 0" .
    " AND Timestamp > DATE_ADD( CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00'), INTERVAL -1 HOUR) ORDER BY TimeStamp DESC LIMIT 1";

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
    " WHERE BatNum = 3" .
    " AND Timestamp > DATE_ADD( CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00'), INTERVAL -1 HOUR) ORDER BY TimeStamp DESC LIMIT 1";

    $resultTBL = $con->query($query);
    $rows = mysqli_fetch_assoc($resultTBL);
    $SOC3 = $rows["SOCPercent"];
    $StoredEnergy3 = $rows["StoredEnergy"];
    $Current3 = $rows["Current"];
    $DischargeCurrentLimit3 = $rows["DischargeCurrentLimit"];
    $ChargeCurrentLimit3 = $rows["ChargeCurrentLimit"];
    $Voltage3 = $rows["Voltage"];
    $Temperature3 = $rows["Temperature"];
    $DTm3 = $rows["TimeStamp"];
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
    " WHERE BatNum = 2" .
    " AND Timestamp > DATE_ADD( CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00'), INTERVAL -1 HOUR) ORDER BY TimeStamp DESC LIMIT 1";

    $resultTBL = $con->query($query);
    $rows = mysqli_fetch_assoc($resultTBL);
    $SOC2 = $rows["SOCPercent"];
    $StoredEnergy2 = $rows["StoredEnergy"];
    $Current2 = $rows["Current"];
    $DischargeCurrentLimit2 = $rows["DischargeCurrentLimit"];
    $ChargeCurrentLimit2 = $rows["ChargeCurrentLimit"];
    $Voltage2 = $rows["Voltage"];
    $Temperature2 = $rows["Temperature"];
    $DTm2 = $rows["TimeStamp"];
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
    " WHERE BatNum = 1" .
    " AND Timestamp > DATE_ADD( CONVERT_TZ(UTC_TIMESTAMP ,'+00:00','+13:00'), INTERVAL -1 HOUR) ORDER BY TimeStamp DESC LIMIT 1";

    $resultTBL = $con->query($query);
    $rows = mysqli_fetch_assoc($resultTBL);
    $SOC1 = $rows["SOCPercent"];
    $StoredEnergy1 = $rows["StoredEnergy"];
    $Current1 = $rows["Current"];
    $DischargeCurrentLimit1 = $rows["DischargeCurrentLimit"];
    $ChargeCurrentLimit1 = $rows["ChargeCurrentLimit"];
    $Voltage1 = $rows["Voltage"];
    $Temperature1 = $rows["Temperature"];
    $DTm1 = $rows["TimeStamp"];
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
  <tr>
    <th>Bat</th>
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
    <td>Big</p></td>
    <td><p><?=round($SOC,1)?> %</p></td>
    <td><p><?=round($StoredEnergy,1)?> kWh</p></td>
    <td><p><?=round($Current,1)?> A</p></td>
    <td><p><?=round($Voltage*$Current/1000,1)?> kW</p></td>
    <td><p><?=round($ChargeCurrentLimit,1)?> A</p></td>
    <td><p><?=round($DischargeCurrentLimit,1)?> A</p></td>
    <td><p><?=round($Voltage,1)?> V</p></td>
    <td><p><?=round($Temperature,1)?> C</p></td>
  </tr>
  <tr>
    <td>1</p></td>
    <td><p><?=round($SOC1,1)?> %</p></td>
    <td><p><?=round($StoredEnergy1,1)?> kWh</p></td>
    <td><p><?=round($Current1,1)?> A</p></td>
    <td><p><?=round($Voltage1*$Current1/1000,1)?> kW</p></td>
    <td><p><?=round($ChargeCurrentLimit1,1)?> A</p></td>
    <td><p><?=round($DischargeCurrentLimit1,1)?> A</p></td>
    <td><p><?=round($Voltage1,1)?> V</p></td>
    <td><p><?=round($Temperature1,1)?> C</p></td>
  </tr>
  <tr>
    <td>2</p></td>
    <td><p><?=round($SOC2,1)?> %</p></td>
    <td><p><?=round($StoredEnergy2,1)?> kWh</p></td>
    <td><p><?=round($Current2,1)?> A</p></td>
    <td><p><?=round($Voltage2*$Current2/1000,1)?> kW</p></td>
    <td><p><?=round($ChargeCurrentLimit2,1)?> A</p></td>
    <td><p><?=round($DischargeCurrentLimit2,1)?> A</p></td>
    <td><p><?=round($Voltage2,1)?> V</p></td>
    <td><p><?=round($Temperature2,1)?> C</p></td>
  </tr>
  <tr>
    <td>3</p></td>
    <td><p><?=round($SOC3,1)?> %</p></td>
    <td><p><?=round($StoredEnergy3,1)?> kWh</p></td>
    <td><p><?=round($Current3,1)?> A</p></td>
    <td><p><?=round($Voltage3*$Current3/1000,1)?> kW</p></td>
    <td><p><?=round($ChargeCurrentLimit3,1)?> A</p></td>
    <td><p><?=round($DischargeCurrentLimit3,1)?> A</p></td>
    <td><p><?=round($Voltage3,1)?> V</p></td>
    <td><p><?=round($Temperature3,1)?> C</p></td>
  </tr>
</table>
</body>
<!-------------------------------------------------------------------------------------------------------------------------->
<head>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['corechart']});
      google.charts.setOnLoadCallback(drawChart);

      function drawChart() {
         //--------------------------------------------------------------------------------------------------------------
         var dataAllC = new google.visualization.DataTable();
        dataAllC.addColumn('datetime', 'TimeStamp');
        dataAllC.addColumn('number', 'Current');
        dataAllC.addColumn('number', 'ChargeCurrentLimit');
        dataAllC.addColumn('number', 'Power');
        dataAllC.addColumn('number', 'DischargeCurrentLimit');

        dataAllC.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultAllC);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current"].", ".$row["ChargeCurrentLimit"].", ".$row["Power"].", ".$row["DischargeCurrentLimit"]."]";
                  while($row = mysqli_fetch_assoc($resultAllC)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current"].", ".$row["ChargeCurrentLimit"].", ".$row["Power"].", ".$row["DischargeCurrentLimit"]."]";
                    }
                ?>
               ])
        var optionsAllC = {
          title: 'Current(A), Power(kW) and Current Limits(A)',
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
        dataV.addColumn('number', 'Voltage');
        dataV.addColumn('number', 'VoltageMin');
        dataV.addColumn('number', 'VoltageMax');

        dataV.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultV);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Voltage"].", ".$row["VoltageMin"].", ".$row["VoltageMax"]."]";
                  while($row = mysqli_fetch_assoc($resultV)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Voltage"].", ".$row["VoltageMin"].", ".$row["VoltageMax"]."]";
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
        dataC.addColumn('number', 'CurrentTotal');
        dataC.addColumn('number', 'Current1');
        dataC.addColumn('number', 'Current2');
        dataC.addColumn('number', 'Current3');
        // dataC.addColumn('number', 'CurrentMin0');
        // dataC.addColumn('number', 'CurrentMax0');
        
        dataC.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultC);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["CurrentTotal"].", ".$row["Current1"].", ".$row["Current2"].", ".$row["Current3"]."]";
                  // echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current0"].", ".$row["Current1"].", ".$row["Current2"].", ".$row["Current3"].", ".$row["CurrentMin0"].", ".$row["CurrentMax0"]."]";
                  while($row = mysqli_fetch_assoc($resultC)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["CurrentTotal"].", ".$row["Current1"].", ".$row["Current2"].", ".$row["Current3"]."]";
                        // echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current0"].", ".$row["Current1"].", ".$row["Current2"].", ".$row["Current3"].", ".$row["CurrentMin0"].", ".$row["CurrentMax0"]."]";
                    }
                ?>
               ])
        var optionsC = {
          title: 'Current(A) For Each Battery with Total',
          legend: { position: 'bottom' }//,
        };
        var chartC = new google.visualization.LineChart(document.getElementById('curve_chartC'));
        chartC.draw(dataC, optionsC);
         //--------------------------------------------------------------------------------------------------------------
         var dataCL = new google.visualization.DataTable();
        dataCL.addColumn('datetime', 'TimeStamp');
        dataCL.addColumn('number', 'ChargeCurrentLimit');
        dataCL.addColumn('number', 'ChargeCurrentLimitMin');
        dataCL.addColumn('number', 'ChargeCurrentLimitMax');

        dataCL.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultCL);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["ChargeCurrentLimit"].", ".$row["ChargeCurrentLimitMin"].", ".$row["ChargeCurrentLimitMax"]."]";
                  while($row = mysqli_fetch_assoc($resultCL)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["ChargeCurrentLimit"].", ".$row["ChargeCurrentLimitMin"].", ".$row["ChargeCurrentLimitMax"]."]";
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
         var dataCC = new google.visualization.DataTable();
        dataCC.addColumn('datetime', 'TimeStamp');
        dataCC.addColumn('number', 'Current');
        dataCC.addColumn('number', 'CurrentMin');
        dataCC.addColumn('number', 'CurrentMax');

        dataCC.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultCC);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current"].", ".$row["CurrentMin"].", ".$row["CurrentMax"]."]";
                  while($row = mysqli_fetch_assoc($resultCC)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["Current"].", ".$row["CurrentMin"].", ".$row["CurrentMax"]."]";
                    }
                ?>
               ])
        var optionsCC = {
          title: 'Current (A) with Min and Max',
          legend: { position: 'bottom' }//,
        };
        var chartCC = new google.visualization.LineChart(document.getElementById('curve_chartCC'));
        chartCC.draw(dataCC, optionsCC);
         //--------------------------------------------------------------------------------------------------------------
         var dataDCL = new google.visualization.DataTable();
        dataDCL.addColumn('datetime', 'TimeStamp');
        dataDCL.addColumn('number', 'DischargeCurrentLimit');
        dataDCL.addColumn('number', 'DischargeCurrentLimitMin');
        dataDCL.addColumn('number', 'DischargeCurrentLimitMax');

        dataDCL.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultDCL);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["DischargeCurrentLimit"].", ".$row["DischargeCurrentLimitMin"].", ".$row["DischargeCurrentLimitMax"]."]";
                  while($row = mysqli_fetch_assoc($resultDCL)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["DischargeCurrentLimit"].", ".$row["DischargeCurrentLimitMin"].", ".$row["DischargeCurrentLimitMax"]."]";
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
         var dataCV = new google.visualization.DataTable();
        dataCV.addColumn('datetime', 'TimeStamp');
        dataCV.addColumn('number', 'CellVoltageMin');
        dataCV.addColumn('number', 'CellVoltageMax');

        dataCV.addRows([
                <?php
                  $row = mysqli_fetch_assoc($resultCV);
                  $dt = $row["TimeStamp"];
                  $yr = substr($dt,0,4);
                  $mo = substr($dt,5,2);
                  $day = substr($dt,8,2);
                  $hr = substr($dt,11,2);
                  $min = substr($dt,14,2);
                  echo "[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["CellVoltageMin"].", ".$row["CellVoltageMax"]."]";
                  while($row = mysqli_fetch_assoc($resultCV)){
                        $dt = $row["TimeStamp"];
                        $yr = substr($dt,0,4);
                        $mo = substr($dt,5,2);
                        $day = substr($dt,8,2);
                        $hr = substr($dt,11,2);
                        $min = substr($dt,14,2);
                        echo ",[new Date(".$yr.",".$mo."-1,".$day.",".$hr.",".$min."), ".$row["CellVoltageMin"].", ".$row["CellVoltageMax"]."]";
                    }
                ?>
               ])
        var optionsCV = {
          title: 'Cell Voltages Min/Max (V)',
          legend: { position: 'bottom' }//,
        };
        var chartCV = new google.visualization.LineChart(document.getElementById('curve_chartCV'));
        chartCV.draw(dataCV, optionsCV);
         //--------------------------------------------------------------------------------------------------------------
  
      }
    </script>
  </head>
  <body>
    <div id="curve_chartAllC" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartSE" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartV" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartC" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartCC" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartCV" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartCL" style="width: 1000px; height: 500px"></div>
    <div id="curve_chartDCL" style="width: 1000px; height: 500px"></div>
  </body>
</html>


  