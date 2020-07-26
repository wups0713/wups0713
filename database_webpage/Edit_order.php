<?php
    session_start ();    
?>
<!DOCTYPE html>
<html>
<head>
	<title>Edit_order</title>
	<style>
		.error {color: #FF0000;}
	</style>
	<link type="text/css" rel="stylesheet" href="edit_house.css">
</head>
<body>
	<?php
		if (empty ($_SESSION ["id"]) || empty ($_POST["id"]))
            echo "<script>window.location.href = './Login.php'</script>";
        $db_host = "dbhome.cs.nctu.edu.tw";
        $db_name = "wups0713_cs_HW3";
        $db_user = "wups0713_cs";
        $db_password = "aa066889";
        $check_in_temp = "";
        $check_out_temp = "";
        $error_text = "";
        $db = new mysqli ($db_host, $db_user, $db_password, $db_name);
        if ($_SERVER["REQUEST_METHOD"] == "POST")
        {
            $id = $_POST["id"];
            $sql = $db->query ("SELECT house_id
                                FROM house_order
                                WHERE id = '$id'");
            $house_id = $sql->fetch_object()->house_id;
            if (@$_POST ["back"] == "返回")
                echo "<script>window.location.href = './Order.php'</script>";
            if (isset ($_POST["first_time"])) //由Order進入 設定預設選項
            {
                $sql = $db->query ("SELECT check_in, check_out
                                    FROM house_order
                                    WHERE id = '$id'");
                $data = $sql->fetch_object();
                $check_in_temp = $data->check_in;
                $check_out_temp = $data->check_out;
            }
            else //輸入有誤 回復之前選項
            {
                $check_in_temp = $_POST["check_in"];
                $check_out_temp = $_POST["check_out"];
            }
            $test = 1;
            if ($check_out_temp <= $check_in_temp || $check_in_temp <= date("Y-m-d"))
            {
                $test = 0;
                $error_text = "輸入時間有誤";
            }
            else
            {
                $sql = $db->query ("SELECT check_in, check_out
                                    FROM house_order
                                    WHERE 
                                    house_id = '$house_id' AND
                                    id != '$id'
                                    ");
                while ($data = $sql->fetch_object())
                {
                    if ($check_in_temp <= $data->check_in && $check_out_temp > $data->check_in)
                    {
                        $test = 0;
                        if ($check_out_temp < $data->check_out)
                            $error_text = $error_text.$data->check_in."~".$check_out_temp." ";
                        else
                            $error_text = $error_text.$data->check_in."~".$data->check_out." ";
                    }
                    elseif ($check_in_temp >= $data->check_in && $check_in_temp < $data->check_out)
                    {
                        $test = 0;
                        if ($check_out_temp < $data->check_out)
                            $error_text = $error_text.$check_in_temp."~".$check_out_temp." ";
                        else
                            $error_text = $error_text.$check_in_temp."~".$data->check_out." ";
                    }
                }
                if ($test == 0)
                    $error_text = $error_text."重複";
            }
            if ($test && @$_POST["complete"] == "修改")
            {
                $sql = $db->query ("UPDATE house_order
                                    SET check_in = '$check_in_temp', check_out = '$check_out_temp'
                                    WHERE id = '$id'");
                echo "<script>window.location.href = './Order.php'</script>";
            }
		}
	?>
	<div class = "board1">
	<form method = "post" action = "Edit_order.php">
		check_in:<br>
		<input type = "date" name = "check_in" value = <?php echo "'".$check_in_temp."'"; ?>>
		<br>
        check_out:<br>
		<input type = "date" name = "check_out" value = <?php echo "'".$check_out_temp."'"; ?>>
		<span class = "error"><?php echo $error_text;?></span>
		<br><br>
        <input type = "hidden" name = "id" value = <?php echo "'".$id."'"?>>
		<input type = "submit" class = "button" name = "complete" value = "修改">
        <input type = "submit" class = "button" name = "back" value = "返回">
		<br>
	</form>
	</div>
</body>
</html>