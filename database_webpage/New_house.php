<?php
    session_start ();    
?>
<!DOCTYPE html>
<html>
<head>
	<title>New_house</title>
	<style>
		.error {color: #FF0000;}
	</style>
	<link type="text/css" rel="stylesheet" href="new_user.css">
</head>
<body>
	<?php
		$name_error = $price_error = $location_error = $owner_id_error = "";
		$check_error = 0;
        date_default_timezone_set('Asia/Taipei');
        $db_host = "dbhome.cs.nctu.edu.tw";
        $db_name = "wups0713_cs_HW3";
        $db_user = "wups0713_cs";
        $db_password = "********";
        $db = new mysqli ($db_host, $db_user, $db_password, $db_name);
        if ($_SERVER["REQUEST_METHOD"] == "POST")
        {
            if (@$_POST ["back"] == "返回")
                echo "<script>window.location.href = './House_management.php'</script>";
            $test = 1;
            if (empty ($_POST ["name_"])){
                $name_error = "*房屋名稱不能為空";
                $test = 0;
            }
            if($test)
                $check_error++;
            //testing name end
            //testing price start
            if (empty ($_POST ["price_"]))
                $price_error = "*價格不能為空";
            else{
                if(!preg_match("/^[0-9]*$/", $_POST ["price_"]))
                    $price_error = "*請輸入阿拉伯數字";
                else
                    $check_error++;
            }
            //testing price end
            if ($check_error == 2)
            {
                $name = $_POST ["name_"];
                $price = $_POST ["price_"];
                $location = $_POST ["location_"];
                $time = date("Y-m-d");
                $owner_id =  $_SESSION ["id"];
                $sql = $db->prepare ("INSERT INTO house (name, price, location_id, time, owner_id) VALUES (?, ?, '$location', '$time', $owner_id)");
                $sql->bind_param("ss", $name, $price);
                $sql->execute ();
                $sql = $db->query ("SELECT max(information_id) AS c
                                    FROM information_table");
                $information_count = $sql->fetch_object ()->c;
                $information = array ();
                for ($i = 0; $i <= $information_count; $i++)
                    array_push ($information, "0");
                for ($i = 1; $i <= $information_count; $i++)
                {
                    if (empty ($_POST["information".$i]))
                        $information [$i] = "0";
                    else
                        $information [$i] = "1";
                }
                $sql = $db->query ("SELECT max(house_id) AS 'id'
                                    FROM house");
                $id = $sql->fetch_object()->id;
                for ($i = 1; $i <= $information_count; $i++)
                {
                    if ($information[$i] == "1"){
                       $sql = $db->query("INSERT INTO information(information_id,house_id) VALUES ('$i',$id)");
                    }
                }
                echo "<script>window.location.href = './House_management.php'</script>";
            }
        }
	?>
	<div class = "board">
	<form method = "post" action = "New_house.php">
		Name:<br>
		<input type = "text" name = "name_">
		<span class = "error"><?php echo $name_error;?></span>
		<br>
		Price:<br>
		<input type = "text" name = "price_">
		<span class = "error"><?php echo $price_error;?></span>
		<br>
		Location:<br>
		<?php
            echo "</select>";
            $sql = $db->query ("SELECT *
                                FROM location_table
                                ORDER BY location ASC");
            echo "<select name = 'location_'>";
                echo "<option value = '-1' "; if ($location_temp == '-1') {echo "selected ";} echo " disabled hidden> ---- </option>";
            while ($data = $sql->fetch_object ())
            {
                echo "<option value = ".$data->location_id." "; if ($location_temp == $data->location_id) {echo ("selected ");} echo "> ".$data->location."</option>";
            }
            echo "</select>";
        ?>
		<span class = "error"><?php echo $location_error;?></span>
        <br><?php
            $sql = $db->query ("SELECT *
                                FROM information_table
                                ORDER BY information_id ASC");
            while ($data = $sql->fetch_object ())
            {
                echo "<input type = 'checkbox' value = '".$data->information."' name = 'information".$data->information_id."' "; if ($information [$data->information_id] == "1") {echo "checked";} echo "> ".$data->information;
            }
            ?>
        <br>
        <input type = "submit" class = "button" value = "新增">
        <input type = "submit" class = "button" name = "back" value = "返回">
		<br>
	</form>
	</div>
</body>
</html>