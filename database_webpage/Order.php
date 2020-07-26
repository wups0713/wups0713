<?php
    session_start ();    
?>
<!DOCTYPE html>
<html>
    <head>
        <title>Order</title>
        <link type="text/css" rel="stylesheet" href="user_management.css">
    </head>
    <body>
        <?php
            $db_host = "dbhome.cs.nctu.edu.tw";
                $user_id = $_SESSION ["id"];
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
                if (($select = @$_POST["favorite"]) == "我的最愛")      //@ 不輸出警告訊息
                    echo "<script>window.location.href = './Favorite.php'</script>";
                if (($select = @$_POST["homepage"]) == "首頁")      //@ 不輸出警告訊息
                    echo "<script>window.location.href = './Homepage.php'</script>";
                ?>
                <table class = "members members1">
                <form method = "post" action = "Order.php">
                    <div float = "left" text-align = "left"> 
                    <h1>Your Houses
                    <input type = "submit" class = "button button5" name = "homepage" value = "首頁">
                    <input type = "submit" class = "button button4" name = "logout" value = "登出">
                    <?php
                        echo " "."<input type = 'submit' class = 'button button3' name = 'favorite' value = '我的最愛'><br>";
                    ?>
                    </div>
                    </h1>
                    <br>
                    <tr>
                        <th width = "5%" height = "25%">ID</th>
                        <th width = "10.5%" height = "25%">Name</th>
                        <th width = "10.5%" height = "25%">Price</th>
                        <th width = "10.5%" height = "25%">Location</th>
                        <th width = "10.5%" height = "25%">Time</th>
                        <th width = "10.5%" height = "25%">Information</th>
                        <th width = "10.5%" height = "25%">Owner</th>
                        <th width = "10.5%" height = "25%">Check-in</th>
                        <th width = "10.5%" height = "25%">Check-out</th>
                        <th width = "10.5%" height = "25%">Option</th>
                    </tr>
                </form>
                </table>
                <?php
            $sql = $db->query ("SELECT DISTINCT house_order.id, house.house_id , house.name AS 'house_name', house.price, location_table.location, house.time, user_data.name AS 'user_name', 
                                house_order.check_in, house_order.check_out
                                FROM house 
                                INNER JOIN location_table ON location_table.location_id = house.location_id 
                                INNER JOIN house_order ON house_order.house_id = house.house_id
                                INNER JOIN user_data ON house.owner_id = user_data.user_id
                                WHERE house_order.user_id = '$user_id'");
            $id_ = 0;
            $check = 0;
            $data = $sql->fetch_object ();
            do
            {   
                if ($check == 0 && empty ($data))
                    {   
                        ?><table class = "members members3"><td><b><font size = "6"><?php echo "您尚未預訂任何房屋";?></font></b></td></table><?php
                        break;                    
                    }
                $id_ = ($id_> $data->id)?$id_:$data->id;
                $check++;
                ?>
                <table class ="members members2">
                    <tr>
                        <td width = "5%" height = "25%"scope="row"><?php echo $data->house_id;?></td>
                        <td width = "10.5%" height = "25%"><?php echo $data->house_name;?></td>
                        <td width = "10.5%" height = "25%"><?php echo $data->price;?></td>
                        <td width = "10.5%" height = "25%"><?php
                        if (empty ($data->location))
                            echo "未知";
                        else
                            echo $data->location;?></td>
                        <td width = "10.5%" height = "25%"><?php echo $data->time;
                        $sql_ = $db->query ("SELECT information_table.information
                                            FROM information
                                            INNER JOIN information_table ON information_table.information_id = information.information_id
                                            WHERE house_id = '$data->house_id'");?>
                        <td width = "10.5%" height = "25%"><?php
                        while ($information = $sql_->fetch_object ()){
                                echo $information->information."<br>";}?></td>    
                        </td>
                        <td width = "10.5%" height = "25%"><?php echo $data->user_name;?></td>
                        <td width = "10.5%" height = "25%"><?php echo $data->check_in;?></td>
                        <td width = "10.5%" height = "25%"><?php echo $data->check_out;?></td>
                    <?php
                    $edit_ = "edit".$id_;
                    $delete_ = "delete".$id_;
                    ?>
                    <td width = "10.5%" height = "25%"><?php 
                        echo "<form method = 'post' action = 'Edit_order.php'>";
                        echo "<input type = 'submit' class = 'button' name = '".$edit_."' value = '編輯'>";
                        echo "<input type = 'hidden' name = 'check_in' value = '$data->check_in'>";
                        echo "<input type = 'hidden' name = 'check_out' value = '$data->check_out'>";
                        echo "<input type = 'hidden' name = 'first_time' value = '1'>";
                        echo "<input type = 'hidden' name = 'id' value = '$data->id'>";
                        echo "</form><br>";
                        echo "<form method = 'post' action = 'Order.php'>";
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
                    echo "<script>window.location.href = './Edit_order.php'</script>";
                }
                if (isset ($_POST ["delete".$id_]))
                {
                    $sql = $db->query ("DELETE FROM house_order WHERE id = '$id_'");
                    echo "<script>window.location.href = './Order.php'</script>";
                }
                $id_--;
            }  
            ?>               

    </body>
</html>