<?php
    $con = mysqli_connect("localhost", "tracking", "system", "tracking");
    mysqli_query($con,'SET NAMES utf8');

    $userID = $_POST["UserID"];
    
    $statement = mysqli_prepare($con, "SELECT DISTINCT DeviceID FROM mapping WHERE UserID = ? ");
    mysqli_stmt_bind_param($statement, "s", $userID);
    mysqli_stmt_execute($statement);

    mysqli_stmt_store_result($statement);
    mysqli_stmt_bind_result($statement, $DeviceID);

    $response = array();
    $response2 = array();
 
    while(mysqli_stmt_fetch($statement)) {
        $response["DeviceID"] = $DeviceID;
        $response2[] = $response;
    }

    echo json_encode($response2);
?>
