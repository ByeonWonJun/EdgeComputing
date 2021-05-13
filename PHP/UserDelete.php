<?php
    $con = mysqli_connect("localhost", "tracking", "system", "tracking");
    mysqli_query($con,'SET NAMES utf8');

    $UserID = $_POST["UserID"];

    $IsUser = mysqli_query($con, "Select UserID FROM user WHERE UserID='$UserID'");
    $User_count = mysqli_num_rows($IsUser);

    if($User_count == 1){

	$User_statement = mysqli_query($con, "Select Distinct DeviceID FROM mapping WHERE UserID='$UserID'");

	while($User_row=mysqli_fetch_assoc($User_statement)){

		$User_Device = $User_row["DeviceID"];
		$count_statement = mysqli_query($con, "Select Distinct FileID FROM tracking WHERE DeviceID = '$User_Device'");
		$count = mysqli_num_rows($count_statement);
		
		$response = array();
	
		if($count >= 1){
			$NFS_statement = mysqli_query($con, "Select Distinct FileID FROM tracking WHERE DeviceID = '$User_Device'");
			while($row=mysqli_fetch_assoc($NFS_statement)){
				$command = $row["FileID"];
				system("sudo /home/tracking/Deletion/Deletion '$command'");
			}
			$statement = mysqli_query($con, "DELETE FROM tracking WHERE DeviceID = '$User_Device'");
		}
	}
	$response["success"] = true;
    }

    else{
	$response["success"] = false;
    }

    if($response["success"] == true){
	$statement = mysqli_query($con, "DELETE FROM mapping WHERE UserID = '$UserID'");
	$statement = mysqli_query($con, "DELETE FROM user WHERE UserID = '$UserID'");
    }

    echo json_encode($response);
    mysqli_close($con);
?>
