<?php
    $con = mysqli_connect("localhost", "tracking", "system", "tracking");
    mysqli_query($con,'SET NAMES utf8');

    $UserID = $_POST["UserID"];
    $UserPassword = $_POST["UserPassword"];
    $UserName = $_POST["UserName"];
    $UserPhone = $_POST["UserPhone"];
    $UserNumber = $_POST["UserNumber"];
       

    $statement = mysqli_prepare($con, "INSERT INTO tracking.user VALUES (?,?,?,?,?)");
    mysqli_stmt_bind_param($statement, "sssii", $UserID, $UserPassword, $UserName, $UserPhone, $UserNumber);
    mysqli_stmt_execute($statement);


    $response = array();
    $response["success"] = true;


    echo json_encode($response);



?>
