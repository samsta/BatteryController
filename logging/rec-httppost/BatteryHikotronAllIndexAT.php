<?php
session_start();

ini_set('display_errors', 'On');
error_reporting(E_ALL | E_STRICT);

$start = new datetime("NOW", new DateTimeZone('Pacific/Auckland'));
$end = new datetime("NOW", new DateTimeZone('Pacific/Auckland'));
$start->sub(new DateInterval('PT24H')); // subtract 24 hours
//echo "START = " . $start->format('Y-m-d H:i:s (e)') . "<br>";
//echo "END   = " . $end->format('Y-m-d H:i:s (e)') . "<br>";

if( isset( $_SESSION['starttext'] ) ) {
  $st = $_SESSION['starttext'];
  $et = $_SESSION['endtext'];
  //echo "IS SET <br>";
}
else {
  $st = $start->format('Y-m-d H:i:s');
  $et = $end->format('Y-m-d H:i:s');
  //echo "Not SET <br>";

}    

//echo "#st = " . $st . "<br>";
//echo "#et = " . $et . "<br>";

$syr = substr($st,0,4);
$smo = substr($st,5,2);
$sday = substr($st,8,2);
$shr = substr($st,11,2);
$smin = substr($st,14,2);
$eyr = substr($et,0,4);
$emo = substr($et,5,2);
$eday = substr($et,8,2);
$ehr = substr($et,11,2);
$emin = substr($et,14,2);

?>

<html>
<BODY bgColor="#FFFFFF" text="#000000" link="#CCCFFF" vlink="#FFCCFF">
<font face="helvetica" color="#000000">
<?php
date_default_timezone_set("Pacific/Auckland");
echo "ALL Graphs Anytime" . "<br><br>";
echo "The time is " . date("H:i:s") . "<br><br>";
?>
<form autocomplete="off" action="BatteryOneAllActionAT.php"  method="post">
Start Time
<input name="StartYR" type="text" size="4" value="<?=$syr?>">
-<input name="StartMO" type="text" size="2" value="<?=$smo?>">
-<input name="StartDAY" type="text" size="2" value="<?=$sday?>">
 <input name="StartHR" type="text" size="2" value="<?=$shr?>">
:<input name="StartMIN" type="text" size="2" value="<?=$smin?>"><br>
  End Time
 <input name="EndYR" type="text" size="4" value="<?=$eyr?>">
-<input name="EndMO" type="text" size="2" value="<?=$emo?>">
-<input name="EndDAY" type="text" size="2" value="<?=$eday?>">
  <input name="EndHR" type="text" size="2" value="<?=$ehr?>">
  :<input name="EndMIN" type="text" size="2" value="<?=$emin?>"><br>
<br>
<input type="Submit" value="Draw Chart">
</form>
</font>
<br><A HREF="https://jimster.ca/BatteryOne/index.php" >Main Menu</A>
</body>
</html>



