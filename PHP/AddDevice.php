<?php
    $con = mysqli_connect("localhost", "tracking", "system", "tracking");
    mysqli_query($con,'SET NAMES utf8');

    $DeviceID = $_POST["DeviceID"];
    $UserID = $_POST["UserID"];

    $statement = mysqli_prepare($con, " INSERT INTO tracking.mapping (UserID, DeviceID) VALUES (?, ?)");
    mysqli_stmt_bind_param($statement, "ss", $UserID, $DeviceID);
    mysqli_stmt_execute($statement);

    $response = array();
    $response["success"] = true;

    echo json_encode($response);
    mysqli_close($con);

?>
