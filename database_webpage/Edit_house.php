<?php
    session_start ();    
?>
<!DOCTYPE html>
<html>
<head>
	<title>Edit_house</title>
	<style>
		.error {color: #FF0000;}
	</style>
	<link type="text/css" rel="stylesheet" href="edit_house.css">
</head>
<body>
	<?php
        $name_error = $price_error = $location_error = $owner_id_error = "";
		$check_error = 0;
        date_default_timezone_set('Asia/Taipei');
		if (empty ($_SESSION ["id"]) || empty ($_POST["id"]))
			echo "<script>window.location.href = './Login.php'</script>";
        if ($_SERVER["REQUEST_METHOD"] == "POST")
        {
            $db_host = "dbhome.cs.nctu.edu.tw";
            $db_name = "wups0713_cs_HW3";
            $db_user = "wups0713_cs";
            $db_password = "********";
            $db = new mysqli ($db_host, $db_user, $db_password, $db_name);
            $id = $_POST["id"];
            $sql = $db->query ("SELECT max(information_id) AS c
                                FROM information_table");
            $information_count = $sql->fetch_object ()->c;
            $information = array ();
            for ($i = 0; $i <= $information_count; $i++)
                array_push ($information, "0");
            $test = 0;            
            $db = new mysqli ($db_host, $db_user, $db_password, $db_name);
            if (@$_POST ["back"] == "返回")
                echo "<script>window.location.href = './House_management.php'</script>";
            if (isset ($_POST["first_time"])) //由House_management進入 設定預設選項
            {
                $sql = $db->query ("SELECT information_id
                                    FROM information
                                    WHERE house_id = '$id'");
                while ($data = $sql->fetch_object ())
                    $information[$data->information_id] = "1";
            }
            else //輸入有誤 回復之前選項
            {
                for ($i = 1; $i <= $information_count; $i++)
                    if (isset ($_POST["information".$i]))
                        $information[$i] = 1;
            }
            $name = $_POST["name"];
            $price = $_POST["price"];
            $location = $_POST["location"];
            $name_error = "";
            $price_error = "";
            $location_error = "";
            $time_error = "";
            //testing name start
            if (empty ($name))
                $name_error = "*name 不能為空";
            else
                if (preg_match ("/^\s$/", $name))
                    $name_error = "*name 格式錯誤";
                else
                    $test++;
            //testing name end
            //testing price start
            if (empty ($price))
                $price_error = "*price 不能為空";
            else
                if (!preg_match ("/^[0-9]+$/", $price))
                    $price_error = "*price 格式錯誤";
                else
                    $test++;
            //testing price end
            if ($test == 2 && @$_POST["complete"] == "修改")
            {
                $sql = $db->prepare ("UPDATE house
                                    SET name = ?, price = ?, location_id = '$location'
                                    WHERE house_id = '$id'");
                $sql->bind_param("ss", $name, $price);
                $sql->execute ();
                $sql = $db->query ("DELETE FROM information
                                    WHERE house_id = '$id'");
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
	<form method = "post" action = "Edit_house.php">
		Name:<br>
		<input type = "text" name = "name" value = <?php echo "'".$name."'"; ?>>
		<span class = "error"><?php echo $name_error;?></span>
		<br>
		Price:<br>
		<input type = "text" name = "price" value = <?php echo "'".$price."'"; ?>>
		<span class = "error"><?php echo $price_error;?></span>
		<br>
		Location:<br>
		<?php
            echo "</select>";
            $sql = $db->query ("SELECT *
                                FROM location_table
                                ORDER BY location ASC");
            echo "<select name = 'location'>";
            while ($data = $sql->fetch_object ())
            {
                echo "<option value = ".$data->location_id." "; if ($location == $data->location_id) {echo ("selected ");} echo "> ".$data->location."</option>";
            }
            echo "</select>";
        ?>
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
        <input type = "hidden" name = "id" value = <?php echo "'".$id."'"?>>
        <input type = "submit" class = "button" name = "complete" value = "修改">
        <input type = "submit" class = "button" name = "back" value = "返回">
		<br>
	</form>
	</div>
</body>
</html>