<?php
$user = "timluser";
$password = "userML";
$database = "battery_data";
$hostname = "jstulen.netfirmsmysql.com";

// Create a mysqli connection
$connection = new mysqli($hostname, $user, $password, $database);

// Check the connection
if ($connection->connect_error) {
    die("Connection failed: " . $connection->connect_error);
}
//echo "Connected successfully<br>";

// Define the insert statement
$insert = "INSERT INTO `BatteryOne`(`TimeStamp`, `BatNum`, `CellVoltageMin`, `CellVoltageMax`, 
            `Voltage`, `VoltageMin`, `VoltageMax`, `Current`, `CurrentMin`, `CurrentMax`, 
            `SOCPercent`, `ChargeCurrentLimit`, `ChargeCurrentLimitMin`, `ChargeCurrentLimitMax`, 
            `DischargeCurrentLimit`, `DischargeCurrentLimitMin`, `DischargeCurrentLimitMax`, 
            `StoredEnergy`, `Temperature`, `TemperatureMin`, `TemperatureMax`) VALUES ";

// Read the source file
$file = 'BatteryOneDataLog.txt';
if (file_exists($file)) {
    $lines = file($file);
    $dcount = count($lines);

    if ($dcount > 0) {
        for ($x = 0; $x < $dcount; $x++) {
            // Prepare the query for each line 
            $query = $insert . '(' . $lines[$x] . ')';
            //echo "$query<br>\n";

            // Execute the query using mysqli
            if (!$connection->query($query)) {
                //echo "Error: " . $connection->error . "<br>\n";
            }
        }

        // Delete the source file after processing
        if (unlink($file)) {
            //echo "Deleted $file<br>\n";
        } else {
            //echo "Error deleting $file<br>\n";
        }
    } else {
        //echo "No new data found.<br>\n";
    }
} else {
    //echo "File not found.<br>\n";
}

// Close the connection
$connection->close();
//echo "Done.";
?>
