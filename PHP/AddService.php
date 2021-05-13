<?php
    $con = mysqli_connect("localhost", "tracking", "system", "tracking");
    mysqli_query($con,'SET NAMES utf8');

    $UserID = $_POST["UserID"];
    $DeviceID = $_POST["DeviceID"];
    $ServiceID = $_POST["ServiceID"];

    $statement = mysqli_prepare($con, " UPDATE tracking.mapping SET ServiceID= ? WHERE DeviceID= ? AND UserID = ?");
    mysqli_stmt_bind_param($statement, "sss", $ServiceID, $UserID, $DeviceID);
    mysqli_stmt_execute($statement);


    $response = array();
    $response["success"] = true;


    echo json_encode($response);

    mysqli_close($con);


?>
