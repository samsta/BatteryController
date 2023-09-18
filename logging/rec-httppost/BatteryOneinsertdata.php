<?
 $user="timluser";
 $password="userML";
 $database="battery_data";
 $hostname="jstulen.netfirmsmysql.com";
mysql_connect($hostname,$user,$password);
@mysql_select_db($database) or die( "Unable to select database");
$insert = "INSERT INTO `BatteryOne`(`TimeStamp`,`BatNum`,`Voltage`,`VoltageMin`,`VoltageMax`,`Current`,`CurrentMin`,`CurrentMax`," .
          "`SOCPercent`,`ChargeCurrentLimit`,`ChargeCurrentLimitMin`,`ChargeCurrentLimitMax`,`DischargeCurrentLimit`,`DischargeCurrentLimitMin`,`DischargeCurrentLimitMax`," .
          "`StoredEnergy`,`Temperature`,`TemperatureMin`,`TemperatureMax`) VALUES ";

$file = 'BatteryOneDataLog.txt';
// read the source file
if (file_exists($file)) {
	$lines = file($file);
	$dcount = count($lines);
  // echo "$dcount<br>\n";
	// echo "$lines<br>\n";
	if ($dcount > 0)
	{
		for($x = 0; $x<$dcount; $x=$x+1)
		{
			$query = $insert . '(' . $lines[$x] . ')';
			echo "$query<br>\n";
			//insert each line into the database
			mysql_query($query);
		}
		// delete the source file
		if (unlink($file))	{
			echo ("deleted $file<br>\n");
		}
		else {
			echo ("error deleting $file<br>\n");
		}
	}
	else
	{
		echo "No new data found.<br>\n";
	}
}
else {
	echo "File not found.<br>\n";
}
mysql_close();
echo "Done.";
?>

