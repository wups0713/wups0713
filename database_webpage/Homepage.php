<?php
    session_start ();
?>
<html>
<head>
    <title>Homepage</title>
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
        $db_password = "aa066889";
        $db = new mysqli ($db_host, $db_user, $db_password, $db_name);
        if (($select = @$_POST["logout"]) == "登出")      //@ 不輸出警告訊息
        {
            unset ($_SESSION["id"]);
            unset ($_SESSION["admin"]);
            echo "<script>window.location.href = './Login.php'</script>";
        }
        if (($select = @$_POST["location_management"]) == "地點管理")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './Location_management.php'</script>";
        if (($select = @$_POST["information_management"]) == "資訊管理")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './Information_management.php'</script>";
        if (($select = @$_POST["user_management"]) == "會員管理")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './User_management.php'</script>";
        if (($select = @$_POST["favorite"]) == "我的最愛")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './Favorite.php'</script>";
        if (($select = @$_POST["house_management"]) == "房屋管理")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './House_management.php'</script>";
        if (($select = @$_POST["order_page"]) == "訂房管理")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './Order.php'</script>";
        if (($select = @$_POST["landlord"]) == "租用狀態")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './Landlord.php'</script>";
    ?>
        <form method = "post" action = "Homepage.php">
            <div float = "left" text-align = "left"><p><br></p><h1>Houses<?php
            if ($_SESSION ["admin"] == 1)
            {
                ?>
                <input type = 'submit' class = 'button button10' name = 'logout' value = '登出'>
                <input type = 'submit' class = 'button button9' name = 'location_management' value = '地點管理'>
                <input type = 'submit' class = 'button button8' name = 'information_management' value = '資訊管理'>
                <input type = 'submit' class = 'button button7' name = 'user_management' value = '會員管理'>
                <input type = 'submit' class = 'button button6' name = 'favorite' value = '我的最愛'>
                <input type = 'submit' class = 'button button5' name = 'house_management' value = '房屋管理'><?php
            }
            else
            {   
                ?>
                <input type = 'submit' class = 'button button7' name = 'logout' value = '登出'>
                <input type = 'submit' class = 'button button6' name = 'favorite' value = '我的最愛'>
                <input type = 'submit' class = 'button button5' name = 'house_management' value = '房屋管理'><?php
            }
            ?>
            <input type = 'submit' class = 'button button4' name = 'order_page' value = '訂房管理'>
            <input type = 'submit' class = 'button button3' name = 'landlord' value = '租用狀態'></h1></div>
            <br>
    <?php
        if (empty ($_POST["page"]))
            $page = 1;
        else
            $page = $_POST["page"];
        if (empty ($_POST["id"]))
            $id_temp = "";
        else
            $id_temp = $_POST["id"];
        if (empty ($_POST["name"]))
            $name_temp = "";
        else
            $name_temp = $_POST["name"];
        if (empty ($_POST["price"]))
            $price_temp = '-1';
        else
            switch ($_POST["price"])
            {
                case -1:
                    $price_temp = '-1';
                    break;
                case 1:
                    $price_temp = '1';
                    $lowerbound = '0';
                    $upperbound = '500';
                    break;
                case 2:
                    $price_temp = '2';
                    $lowerbound = '500';
                    $upperbound = '1000';
                    break;
                case 3:
                    $price_temp = '3';
                    $lowerbound = '1000';
                    $upperbound = '2000';
                    break;
                case 4:
                    $price_temp = '4';
                    $lowerbound = '2000';
                    $upperbound = '100000';
                    break;
                default:
                    $price_temp = '-1';
                    break;
            }
        if (empty ($_POST["location"]))
            $location_temp = '-1';
        else
            if ($_POST["location"] == '-1')
                $location_temp = '-1';
            else
                $location_temp = $_POST["location"];
        if (empty ($_POST["check_in"]))
            $check_in_temp = "";
        else
            $check_in_temp = $_POST["check_in"];
        if (empty ($_POST["check_out"]))
            $check_out_temp = "";
        else
            $check_out_temp = $_POST["check_out"];
        if (empty ($_POST["owner"]))
            $owner_temp = "";
        else
            $owner_temp = $_POST["owner"];
        $sort = " ORDER BY house.house_id ASC";
        $sort_ = "";
        if (@$_POST["submit"])
        {
            $sort_ = "";
        }
        else
        {
            if (!empty ($_POST["sort_"]))
            {
                if (@$_POST["sort_"] == '▲price')
                {
                    $sort_ = '▲price';
                    $sort = " ORDER BY price ASC";
                }
                elseif (@$_POST["sort_"] == '▼price')
                {
                    $sort_ = '▼price';
                    $sort = " ORDER BY price DESC";
                }
                elseif (@$_POST["sort_"] == '▲time')
                {
                    $sort_ = '▲time';
                    $sort = " ORDER BY time ASC";                
                }
                elseif (@$_POST["sort_"] == '▼time')
                {
                    $sort_ = '▼time';
                    $sort = " ORDER BY time DESC";                
                }
            }
        }
        $sql = $db->query ("SELECT count(*) AS c
                            FROM information_table");
        $data = $sql->fetch_object ()->c;
        $information = array ();
        for ($i = 0; $i <= $data; $i++)
            array_push ($information, "0");
        $array = array ();
        $count = 0;
        for ($i = 1; $i <= $data; $i++)
        {
            if (empty ($_POST["information".$i]))
                $information [$i] = "";
            else
            {
                $count++;
                $information [$i] = $i;
                array_push ($array, "'".$information[$i]."'");
            }
        }
        //echo "<form method = 'post' action = 'Homepage.php'>";
            echo "<input type = 'text' name = 'id' placeholder = 'id' value = '$id_temp'>";
            echo "<input type = 'text' name = 'name' placeholder = 'name' value = '$name_temp'>";
            echo "<select name = 'price'>";
                echo "<option value = '-1' "; if($price_temp == '-1') {echo("selected");} echo " disabled hidden> ---- </option>";
                echo "<option value = '0' "; if($price_temp == '0') {echo("selected");} if ($price_temp == '-1')echo " hidden"; echo "> 0 ~ </option>";
                echo "<option value = '1' "; if($price_temp == '1') {echo("selected");} echo "> 0 ~ 500 </option>";
                echo "<option value = '2' "; if($price_temp == '2') {echo("selected");} echo "> 500 ~ 1000 </option>";
                echo "<option value = '3' "; if($price_temp == '3') {echo("selected");} echo "> 1000 ~ 2000 </option>";
                echo "<option value = '4' "; if($price_temp == '4') {echo("selected");} echo "> 2000 ~ </option>";
            echo "</select>";
            $sql = $db->query ("SELECT *
                                FROM location_table
                                ORDER BY location ASC");
            echo "<select name = 'location'>";
                echo "<option value = '-1' "; if ($location_temp == '-1') {echo "selected ";} echo " disabled hidden> ---- </option>";
            while ($data = $sql->fetch_object ())
            {
                echo "<option value = ".$data->location_id." "; if ($location_temp == $data->location_id) {echo ("selected ");} echo "> ".$data->location."</option>";
            }
            echo "</select>";
            echo "<input type = 'date' name = 'check_in' placeholder = 'check-in time' value = '$check_in_temp'>";
            echo "<input type = 'date' name = 'check_out' placeholder = 'check-out time' value = '$check_out_temp'>";
            echo "<input type = 'text' name = 'owner' placeholder = 'owner' value = '$owner_temp'><br>";
            $sql = $db->query ("SELECT *
                                FROM information_table
                                ORDER BY information_id ASC");
            while ($data = $sql->fetch_object ())
            {
                echo "<input type = 'checkbox' value = '".$data->information."' name = 'information".$data->information_id."' "; if ($information [$data->information_id] == $data->information_id) {echo "checked";} echo "> ".$data->information;
            }
            echo "<input type = 'submit' class = 'button button3' name = 'submit' value = 'submit'>";
            echo "<input type = 'hidden' name = 'sort_' value = '".$sort_."'>";
            echo "<br><br>";
    ?>
        <table class = "members members1">
                <tr>
                    <br>
                    <th width = "12.5%" height = "25%">ID</th>
                    <th width = "12.5%" height = "25%">Name</th>
                    <th width = "12.5%" height = "25%"><?php echo "<button type = 'submit' class = 'button2' name = 'sort_' value = '▲price'>▲</button>";?>Price
                                                    <?php echo "<button type = 'submit' class = 'button2' name = 'sort_' value = '▼price'>▼</button>";?></th>
                    <th width = "12.5%" height = "25%">Location</th>
                    <th width = "12.5%" height = "25%"><?php echo "<button type = 'submit' class = 'button2' name = 'sort_' value = '▲time'>▲</button>";?>Time
                                                    <?php echo "<button type = 'submit' class = 'button2' name = 'sort_' value = '▼time'>▼</button>";?></th>                                 
                    <th width = "12.5%" height = "25%">Owner</th>
                    <th width = "12.5%" height = "25%">Information</th>
                    <th width = "12.5%" height = "25%">Option</th>
                </tr>
        </table>
    <?php
        if ($count)
        {
            $text = " (".implode (",", $array).")";
            $information_sql = "
            house.house_id IN
            (
                SELECT house_id
                FROM information
                WHERE information_id IN ".$text."
                GROUP BY house_id
                Having COUNT(*) = ".$count."
            )";
        }
        else
            $information_sql = " 1";
        $id_sql = "%".$id_temp."%";
        $name_sql = "%".$name_temp."%";
        if ($price_temp == '-1')
            $price_sql = " 1 " ;
        else
            $price_sql = " house.price BETWEEN ".$lowerbound." AND ".$upperbound." ";
        if ($location_temp == '-1')
            $location_sql = " 1 ";
        else
            $location_sql = " house.location_id = ".$location_temp." ";
        $owner_sql = "%".$owner_temp."%";
        $time_empty = 0;
        if (empty ($_POST["check_in"]))
        {
            $time_empty = 1;
            $check_in = "0";            
        }
        else
            $check_in = $check_in_temp;
        if (empty ($_POST["check_out"]))
        {
            $time_empty = 1;
            $check_out = "0";   
        }
        else
            $check_out = $check_out_temp;
        $time_error = 0;
        $time_sql = " 1 ";
        if ($time_empty == 0)
            if ($check_out <= $check_in || $check_in <= date("Y-m-d"))
                $time_error = 1;
            else
                $time_sql = " house.house_id NOT IN (
                    SELECT house.house_id
                    FROM house_order
                    INNER JOIN house ON house_order.house_id = house.house_id
                    WHERE
                    (house_order.check_in <= '".$check_in."' AND '".$check_in."' < house_order.check_out) OR
                    (house_order.check_in < '".$check_out."' AND '".$check_out."' <= house_order.check_out) OR
                    ('".$check_in."' < house_order.check_in AND house_order.check_out < '".$check_out."')
                )";
        $rows = 0;
        if ($check_in == "0"){
            ?><div style = "text-align:center;"><p style="font-size:15px"><?php echo "請輸入訂房時間"?></p></div><?php
        }
        elseif ($time_error){
            ?><div style = "text-align:center;"><p style="font-size:15px"><?php echo "訂房時間錯誤"?></p></div><?php
        }
        elseif ($time_empty){
            ?><div style = "text-align:center;"><p style="font-size:15px"><?php echo "訂房時間不可為空"?></p></div><?php
        }
        else
        {
            $sql = $db->prepare ("SELECT DISTINCT house.house_id, house.name, house.price, location_table.location, house.time, house.owner_id
                                FROM house
                                LEFT JOIN location_table ON house.location_id = location_table.location_id
                                INNER JOIN user_data ON house.owner_id = user_data.user_id
                                WHERE
                                ".$information_sql." AND
                                house.house_id LIKE ? AND
                                house.name LIKE ? AND
                                ".$price_sql." AND
                                ".$location_sql." AND
                                ".$time_sql." AND
                                user_data.name LIKE ?
                                ".$sort);
            $sql->bind_param ("sss", $id_sql, $name_sql, $owner_sql);
            $sql->execute ();
            $result = $sql->get_result ();
            $rows = mysqli_num_rows($result);
            if ($rows == 0)
                echo "無";
            else
            {
                for ($i = ($page - 1) * 5; $i > 0; $i--)
                    $data = $result->fetch_object();
                $i = 0;
                while ($data = $result->fetch_object())
                {
                    if ($i == 5)
                        break;
                    $i++;
                    ?>
                    <table class ="members members2">
                        <tr>
                            <td width = "12.5%" height = "25%"scope="row"><?php echo $data->house_id;?></td>
                            <td width = "12.5%" height = "25%"><?php echo $data->name;?></td>
                            <td width = "12.5%" height = "25%"><?php echo $data->price;?></td>
                            <td width = "12.5%" height = "25%"><?php 
                            if (empty ($data->location))
                                echo "未知";
                            else
                                echo $data->location;?></td>
                            <td width = "12.5%" height = "25%"><?php echo $data->time;?></td>
                        <?php  
                            $sql = $db->query ("SELECT name
                                                FROM user_data
                                                WHERE user_id = '$data->owner_id'");
                            $owner = $sql->fetch_object ();?>
                            <td width = "12.5%" height = "25%"><?php echo $owner->name;?></td>
                        <?php
                            $sql = $db->query ("SELECT information_table.information
                                                FROM information
                                                INNER JOIN information_table ON information.information_id = information_table.information_id
                                                WHERE house_id = '$data->house_id'");?>
                            <td width = "12.5%" height = "25%"><?php
                            while ($information = $sql->fetch_object ())
                                echo $information->information."<br>";?></td>
                        <?php
                            $id = $data->house_id;
                            $sql = $db->query ("SELECT *
                                                FROM favorite
                                                WHERE user_id = '$user_id' AND house_id = '$id'");?>
                            <td width = "25%" height = "25%"><?php 
                            if ($favorite = $sql->fetch_object ()){?>
                                <button class="button disabled">已在我的最愛內</button><?php
                            }
                            else
                                echo "<button type = 'submit' class = 'button' name = 'favorite_add' value = '".$id."'>加到我的最愛</button>";
                            if ($_SESSION ["admin"])
                                echo "<br><br><button type = 'submit' class = 'button' name = 'delete' value = '".$id."'>刪除</button>";
                            echo "<button type = 'submit' class = 'button' name = 'order' value = '".$id."'>訂房</button>";
                            ?>
                            </td>
                        </tr>
                    </table>
                    <?php
                }
            }
        }
        if ($rows)
        {
            for ($i = 1; $rows > 0; $rows = $rows - 5)
            {
                if (@$_POST["page"] == $i || ($page == 1 && $i == 1))
                    echo "<button class='button disabled'>".$i."</button>";
                else
                    echo "<input type = 'submit' class = 'button' name = 'page' value = '".$i."'>";
                $i++;
            }
        }
        if (isset ($_POST ["favorite_add"]))
        {
            $id_ = $_POST ["favorite_add"];
            $sql = $db->query ("INSERT INTO favorite (user_id, house_id) VALUES ('$user_id', '$id_')");
            echo "<script>window.location.href = './Homepage.php'</script>";
        }
        if (isset ($_POST ["delete"]))
        {
            $id_ = $_POST ["delete"];
            $sql = $db->query ("DELETE FROM house WHERE id = '$id_'");
            $sql = $db->query ("DELETE FROM information WHERE house_id = '$id_'");
            $sql = $db->query ("DELETE FROM favorite WHERE house_id = '$id_'");
            $sql = $db->query ("DELETE FROM house_order WHERE house_id = '$id_'");
            echo "<script>window.location.href = './Homepage.php'</script>";
        }
        if (isset ($_POST ["order"]))
        {
            $id_ = $_POST ["order"];
            $sql = $db->query ("INSERT INTO house_order (house_id, user_id, check_in, check_out) VALUES ('$id_', '$user_id', '$check_in', '$check_out')");
            echo "<script>window.location.href = './Homepage.php'</script>";
        }
        echo "</form>";
    ?>
</body>