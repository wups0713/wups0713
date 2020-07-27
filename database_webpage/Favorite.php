<?php
    session_start ();    
?>
<!DOCTYPE html>
<html>
<head>
    <title>Favorite</title>
    <link type="text/css" rel="stylesheet" href="house_management.css">
</head>
<body>
    <?php
        if (empty ($_SESSION ["id"]))
            echo "<script>window.location.href = './Login.php'</script>";
        $account = $name = $email = $identity = "";
        $user_id = $_SESSION ["id"];
        $admin = $_SESSION ["admin"];
        $db_host = "dbhome.cs.nctu.edu.tw";
        $db_name = "wups0713_cs_HW3";
        $db_user = "wups0713_cs";
        $db_password = "********";
        $db = new mysqli ($db_host, $db_user, $db_password, $db_name);
        if (($select = @$_POST["logout"]) == "登出")      //@ 不輸出警告訊息
        {
            unset ($_SESSION["id"]);
            unset ($_SESSION["admin"]);
            echo "<script>window.location.href = './Login.php'</script>";
        }
        if (($select = @$_POST["back"]) == "首頁")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './Homepage.php'</script>";
        ?>
        <table class = "members members1">
            <form method = "post" action = "Favorite.php">
                <div float = "left" text-align = "left"> 
                <h1>Your Favorite
                <input type = "submit" class = "button button3" name = "logout" value = "登出">
                <input type = "submit" class = "button button4" name = "back" value = "首頁"><?php
                ?>
                </div>
                </h1>
                <br>
                <tr>
                    <th width = "12.5%" height = "25%">Id</th>
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
            $id_ = 0;
            $sql = $db->query ("SELECT favorite.favorite_id AS 'id_', house.house_id AS 'id', house.name AS 'name'
                                , house.price AS 'price', location_table.location AS 'location', house.time AS 'time'
                                , house.owner_id AS 'owner_id' 
                                FROM favorite
                                INNER JOIN house ON favorite.house_id = house.house_id
                                LEFT JOIN location_table ON house.location_id = location_table.location_id 
                                WHERE favorite.user_id = '$user_id'
                                ORDER BY id ASC");
            $check = 0;
            $data = $sql->fetch_object ();
            do
            {
                if ($check == 0 && empty ($data))
                {
                    ?><table class = "members members3"><td><b><font size = "6"><?php echo "您尚未有我的最愛";?></font></b></td></table><?php
                    break;                    
                }
                $id_ = ($id_ > $data->id_)?$id_:$data->id_;                                
                $check++;
                ?>
                <table class ="members members2">
                <tr>
                    <td width = "12.5%" height = "25%"scope="row"><?php echo $data->id;?></td>
                    <td width = "12.5%" height = "25%"><?php echo $data->name;?></td>
                    <td width = "12.5%" height = "25%"><?php echo $data->price;?></td>
                    <td width = "12.5%" height = "25%"><?php if (empty ($data->location)) echo "未知"; else echo $data->location;?></td>
                    <td width = "12.5%" height = "25%"><?php echo $data->time;?></td>
                <?php
                $sql_ = $db->query ("SELECT name
                                    FROM user_data
                                    WHERE user_id = '$data->owner_id'");
                $owner = $sql_->fetch_object ();?>
                <td width = "12.5%" height = "25%"><?php echo $owner->name;?></td>
                <?php
                $sql_ = $db->query ("SELECT information
                                    FROM information
                                    INNER JOIN information_table ON information_table.information_id = information.information_id
                                    WHERE house_id = '$data->id'");?>
                <td width = "12.5%" height = "25%"><?php
                while ($information = $sql_->fetch_object ())
                    echo $information->information."<br>";?></td>
                <td width = "25%" height = "25%"><?php
                $delete_ = "delete".$data->id_;
                echo "<form method = 'post' action = 'Favorite.php'>";
                echo "<input type = 'submit' class = 'button' name = '".$delete_."' value = '刪除'>";
                echo "</form><br>";?>
                </td>
                </tr>
            </table>
        <?php
            } while ($data = $sql->fetch_object ());
            while ($id_)
            {
                if (isset ($_POST ["delete".$id_]))
                {
                    $sql = $db->query ("DELETE FROM favorite WHERE favorite_id = '$id_'");
                    echo "<script>window.location.href = './Favorite.php'</script>";
                }
                $id_--;
            }
    ?>
</body>
</html>