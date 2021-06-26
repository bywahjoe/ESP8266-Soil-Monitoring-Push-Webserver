<?php

$servername = "localhost";
$database   = "bywahjoe_soil";
$userID     = "bywahjoe_soil";
$pass       = "Wahyu123-";
$myAPI="sendIP4";
$apiKEY= $d1= $d2= $d3="";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $apiKEY = test_input($_POST["apiKEY"]);
    if($apiKEY == $myAPI) {
        $d1 = test_input($_POST["d1"]);
        $d2 = test_input($_POST["d2"]);
        $d3 = test_input($_POST["d3"]);
        
        // Create connection
        $conn = new mysqli($servername, $userID, $pass, $database);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO sensor (SOIL,DISTANCE,RELAY) VALUES ('" . $d1 . "','" . $d2 . "','" . $d3 . "') ";
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
?>