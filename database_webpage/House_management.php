<?php
    session_start ();    
?>
<!DOCTYPE html>
<html>
<head>
    <title>House_management</title>
    <link type="text/css" rel="stylesheet" href="house_management.css">
</head>
<body>
    <?php
        if (empty ($_SESSION ["id"]))
            echo "<script>window.location.href = './Login.php'</script>";
        $user_id = $_SESSION ["id"];
        $admin = $_SESSION ["admin"];
        $db_host = "dbhome.cs.nctu.edu.tw";
        $db_name = "wups0713_cs_HW3";
        $db_user = "wups0713_cs";
        $db_password = "aa066889";
        $db = new mysqli ($db_host, $db_user, $db_password, $db_name);
        if (($select = @$_POST["logout"]) == "登出")      //@ 不輸出警告訊息
        {
            unset ($_SESSION["id"]);
            unset ($_SESSION["admin"]);
            echo "<script>window.location.href = './Login.php'</script>";
        }
        if (($select = @$_POST["new_house"]) == "新增")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './New_house.php'</script>";
        if (($select = @$_POST["homepage"]) == "首頁")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './Homepage.php'</script>";
        ?>
        <table class = "members members1">
        <form method = "post" action = "House_management.php">
            <div float = "left" text-align = "left"> 
            <h1>Your Houses
            <input type = "submit" class = "button button5" name = "homepage" value = "首頁">
            <input type = "submit" class = "button button4" name = "logout" value = "登出">
            <?php
                 echo " "."<input type = 'submit' class = 'button button3' name = 'new_house' value = '新增'><br>";
                 
            ?>
            </div>
            </h1>
            <br>
            <tr>
                <th width = "12.5%" height = "25%">ID</th>
                <th width = "12.5%" height = "25%">Name</th>
                <th width = "12.5%" height = "25%">Price</th>
                <th width = "12.5%" height = "25%">Location</th>
                <th width = "12.5%" height = "25%">Time</th>
                <th width = "12.5%" height = "25%">Owner</th>
                <th width = "12.5%" height = "25%">Information</th>
                <th width = "12.5%" height = "25%">Option</th>
            </tr>
        </form>
        </table>
        <?php
        $sql = $db->query ("SELECT DISTINCT house.house_id AS 'id', house.name AS 'name', house.price AS 'price', location_table.location AS 'location', house.location_id AS 'location_id', house.time AS 'time',house.owner_id
                            FROM house 
                            INNER JOIN user_data ON house.owner_id = user_data.user_id
                            LEFT JOIN location_table ON location_table.location_id = house.location_id
                            WHERE house.owner_id = '$user_id'");
        $id_ = 0;
        $check = 0;
        $data = $sql->fetch_object ();
        do
        {   
            if ($check == 0 && empty ($data))
                {   
                    ?><table class = "members members3"><td><b><font size = "6"><?php echo "您尚未擁有任何房屋";?></font></b></td></table><?php
                    break;                    
                }
            $id_ = ($id_> $data->id)?$id_:$data->id;
            $check++;
            ?>
           <table class ="members members2">
                <tr>
                    <td width = "12.5%" height = "25%"scope="row"><?php echo $data->id;?></td>
                    <td width = "12.5%" height = "25%"><?php echo $data->name;?></td>
                    <td width = "12.5%" height = "25%"><?php echo $data->price;?></td>
                    <td width = "12.5%" height = "25%"><?php
                    if (empty ($data->location))
                        echo "未知";
                    else
                        echo $data->location;?></td>
                    <td width = "12.5%" height = "25%"><?php echo $data->time;?></td>
                <?php
                $sql_ = $db->query ("SELECT name
                                    FROM user_data
                                    WHERE user_data.user_id = '$data->owner_id'");
                $owner = $sql_->fetch_object ();?>
                    <td width = "12.5%" height = "25%"><?php echo $owner->name;?></td>
                <?php
                $edit_ = "edit".$id_;
                $delete_ = "delete".$id_;
                $sql_ = $db->query ("SELECT information_table.information AS 'information'
                                    FROM information
                                    INNER JOIN information_table ON information.information_id = information_table.information_id
                                    WHERE house_id = '$data->id'");?>
                <td width = "12.5%" height = "25%"><?php
                while ($information = $sql_->fetch_object ()){
                     echo $information->information."<br>";}?></td>
                <td width = "25%" height = "25%"><?php 
                    echo "<form method = 'post' action = 'Edit_house.php'>";
                    echo "<input type = 'submit' class = 'button' name = '".$edit_."' value = '編輯'>";
                    echo "<input type = 'hidden' name = 'id' value = '$data->id'>";
                    echo "<input type = 'hidden' name = 'name' value = '$data->name'>";
                    echo "<input type = 'hidden' name = 'price' value = '$data->price'>";
                    echo "<input type = 'hidden' name = 'location' value = '$data->location_id'>";
                    echo "<input type = 'hidden' name = 'time' value = '$data->time'>";
                    echo "<input type = 'hidden' name = 'first_time' value = '1'>";
                    echo "</form><br>";
                    echo "<form method = 'post' action = 'House_management.php'>";
                    echo " "."<input type = 'submit' class = 'button'  name = '".$delete_."' value = '刪除'>";
                    echo "</form>";?>
                </td>
                </tr>
            </table>
        <?php
        }while($data = $sql->fetch_object ());
        while ($id_)
        {   
            if (isset ($_POST ["edit".$id_]))
            {
                echo "<script>window.location.href = './Edit_house.php'</script>";
            }
            if (isset ($_POST ["delete".$id_]))
            {
                $sql = $db->query ("DELETE FROM house WHERE house_id = '$id_'");
                $sql = $db->query ("DELETE FROM information WHERE house_id = '$id_'");
                echo "<script>window.location.href = './House_management.php'</script>";
            }
            $id_--;
        }  
        ?>                   
</body>
</html>