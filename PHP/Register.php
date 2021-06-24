<?php
    $con = mysqli_connect("localhost", "tracking", "system", "tracking");
    mysqli_query($con,'SET NAMES utf8');

    $UserID = $_POST["UserID"];
    $UserPassword = $_POST["UserPassword"];
    $UserName = $_POST["UserName"];
    $UserPhone = $_POST["UserPhone"];
    $UserNumber = $_POST["UserNumber"];

    $salted = "sejongWonjun".$UserPassword."sejongHanwul";
     
    $hashed = hash('sha512', $salted);
       

    $statement = mysqli_prepare($con, "INSERT INTO tracking.user VALUES (?,?,?,?,?)");
    mysqli_stmt_bind_param($statement, "sssii", $UserID, $hashed, $UserName, $UserPhone, $UserNumber);
    mysqli_stmt_execute($statement);


    $response = array();
    $response["success"] = true;


    echo json_encode($response);

    echo $hashed;



?>
