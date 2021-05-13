<?php
    $con = mysqli_connect("localhost", "tracking", "system", "tracking");
    mysqli_query($con,'SET NAMES utf8');

    $UserID = $_POST["UserID"];
    $UserPassword = $_POST["UserPassword"];
    
    $statement = mysqli_prepare($con, "SELECT * FROM tracking.user WHERE UserID = ? AND UserPassword = ? ");
    mysqli_stmt_bind_param($statement, "ss", $UserID, $UserPassword);
    mysqli_stmt_execute($statement);


    mysqli_stmt_store_result($statement);
    mysqli_stmt_bind_result($statement, $UserID, $UserPassword,$UserName, $UserPhone, $UserNumber);

    $response = array();
    $response2 = array();

    $response["success"] = false;
 
      while(mysqli_stmt_fetch($statement)) {
        $response["success"] = true;
        $response["UserID"] = $UserID;
        $response["UserPassword"] = $UserPassword;
        $response["UserName"] = $UserName;
        $response["UserPhone"] = $UserPhone;
        $response["UserNumber"] = $UserNumber;
      }

echo json_encode($response);

?>
