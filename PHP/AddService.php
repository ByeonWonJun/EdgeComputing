<?php
    $con = mysqli_connect("localhost", "tracking", "system", "tracking");
    mysqli_query($con,'SET NAMES utf8');

    $UserID = $_POST["UserID"];
    $DeviceID = $_POST["DeviceID"];
    $ServiceID = $_POST["ServiceID"];

    $response = array();
    $OK = false;

    $isNull = mysqli_query($con, "SELECT ServiceID FROM mapping WHERE DeviceID = '$DeviceID' AND UserID = '$UserID'");

    while($row=mysqli_fetch_assoc($isNull)){
	$command=$row["ServiceID"];
    	
	if($command == NULL){
	        $statement = mysqli_query($con, "UPDATE mapping SET ServiceID='$ServiceID' WHERE DeviceID= '$DeviceID' AND UserID = '$UserID'");
		$OK = true;
	}
    }

    if($OK == false)
	$statement = mysqli_query($con, "insert into mapping values('$UserID', '$DeviceID', '$ServiceID')");

    $response["success"] = true;

    echo json_encode($response);
    mysqli_close($con);
?>
