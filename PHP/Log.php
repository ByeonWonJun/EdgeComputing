<?php
    $con = mysqli_connect("localhost", "tracking", "system", "tracking");
    mysqli_query($con,'SET NAMES utf8');

    $DeviceID = $_POST["DeviceID"];
    $Date = $_POST["Date"];
    $ServiceID = $_POST["ServiceID"];
    
    $statement = mysqli_prepare($con, "SELECT * FROM tracking WHERE DeviceID = ? AND Date = ? AND ServiceID = ? ORDER BY Time");
    mysqli_stmt_bind_param($statement, "sss", $DeviceID, $Date, $ServiceID);
    mysqli_stmt_execute($statement);


    mysqli_stmt_store_result($statement);
    mysqli_stmt_bind_result($statement,$id, $Date, $AgentID, $DeviceID, $ServiceID, $KeyID, $FileID, $IO, $Time);

    $response = array();
    $response2 = array();
 
    while(mysqli_stmt_fetch($statement)) {
        $response["id"] = $id;
        $response["Date"] = $Date;
        $response["AgentID"] = $AgentID;
        $response["DeviceID"] = $DeviceID;
        $response["ServiceID"] = $ServiceID;
        $response["KeyID"] = $KeyID;
        $response["FileID"] = $FileID;
        $response["IO"] = $IO;
        $response["Time"] = $Time;
        $response2[] = $response;
    }

    echo json_encode($response2);
?>
