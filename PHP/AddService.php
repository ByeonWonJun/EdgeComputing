<?php
    $con = mysqli_connect("localhost", "tracking", "system", "tracking");
    mysqli_query($con,'SET NAMES utf8');

    $UserID = $_POST["UserID"];
    $DeviceID = $_POST["DeviceID"];
    $ServiceID = $_POST["ServiceID"];

    $isNull = mysqli_query($con, " SELECT ServiceID FROM tracking.mapping WHERE DeviceID = '$DeviceID' AND UserID = '$UserID'");
    $response = array();

    if($isNull == null){
        $statement = mysqli_prepare($con, " UPDATE tracking.mapping SET ServiceID= ? WHERE DeviceID= ? AND UserID = ?");
        mysqli_stmt_bind_param($statement, "sss", $ServiceID, $DeviceID, $UserID);
        mysqli_stmt_execute($statement);
        $response["success"] = true;
    }else if($isNull != null){
        $statement = mysqli_prepare($con, " INSERT INTO tracking.mapping VALUES(?,?,?)");
        mysqli_stmt_bind_param($statement, "sss", $UserID,$DeviceID, $ServiceID );
        mysqli_stmt_execute($statement); 
        $response["success"] = true;
    }else{
        $response["success"] = false;
    }
    
    echo json_encode($response);

    mysqli_close($con);


?>