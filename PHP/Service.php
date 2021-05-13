<?php
    $con = mysqli_connect("localhost", "tracking", "system", "tracking");
    mysqli_query($con,'SET NAMES utf8');

    $DeviceID = $_POST["DeviceID"];
    
    $statement = mysqli_prepare($con, "SELECT DISTINCT ServiceID FROM mapping WHERE DeviceID= ?; ");
    mysqli_stmt_bind_param($statement, "s", $DeviceID);
    mysqli_stmt_execute($statement);


    mysqli_stmt_store_result($statement);
    mysqli_stmt_bind_result($statement, $ServiceID);

    $response = array();
    $response2 = array();
    $response["success"] = false;
 
    while(mysqli_stmt_fetch($statement)) {
        $response["success"] = true;
        $response["ServiceID"] = $ServiceID;
        $response2[] = $response;
    }

    echo json_encode($response2);



?>
