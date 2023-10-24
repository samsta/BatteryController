<?php
// Check if the data has been sent via POST method
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
  // Get the data sent in the request body
  $data = file_get_contents('php://input');
  
  // Do something with the data
  // For example, you can save it to a file or a database
  file_put_contents('TESTDataLog.txt', $data, FILE_APPEND | LOCK_EX );
  // file_put_contents('BatteryOneDataLog.txt', $data.PHP_EOL, FILE_APPEND | LOCK_EX );
  
  // Send a response back to the client
  // echo nl2br("Server said: Data received successfully.");
}
?>
