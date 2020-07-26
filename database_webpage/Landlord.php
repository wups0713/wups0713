<?php
    session_start ();    
?>
<!DOCTYPE html>
<html>
    <head>
        <title>Landlord</title>
        <link type="text/css" rel="stylesheet" href="house_management.css">
    </head>
    <body>
    <?php
        if (empty ($_SESSION ["id"]))
            echo "<script>window.location.href = './Login.php'</script>";
        $user_id = $_SESSION ["id"];
        $db_host = "dbhome.cs.nctu.edu.tw";
        $db_name = "wups0713_cs_HW3";
        $db_user = "wups0713_cs";
        $db_password = "aa066889";
        $db = new mysqli ($db_host, $db_user, $db_password, $db_name);
        if (($select = @$_POST["homepage"]) == "首頁")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './Homepage.php'</script>";
        if (($select = @$_POST["logout"]) == "登出")      //@ 不輸出警告訊息
        {
            unset ($_SESSION["id"]);
            unset ($_SESSION["admin"]);
            echo "<script>window.location.href = './Login.php'</script>";
        }
        ?>
        <table class = "members members1">
        <form method = "post" action = "House_management.php">
            <div float = "left" text-align = "left"> 
            <h1>Landlord
            <input type = "submit" class = "button button4" name = "homepage" value = "首頁">
            <input type = "submit" class = "button button3" name = "logout" value = "登出">
            </div>
            </h1>
            <br>
            <tr>
                <th width = "25%" height = "25%">House</th>
                <th width = "25%" height = "25%">Check-in</th>
                <th width = "25%" height = "25%">Check-out</th>
                <th width = "25%" height = "25%">Vistor</th>
            </tr>
        </form>
        </table>
        <?php
        $sql = $db->query ("SELECT DISTINCT house.name AS 'name', house_order.check_in AS 'check_in', house_order.check_out AS 'check_out', user_data.name AS 'user_name'
                            FROM house
                            INNER JOIN house_order ON house_order.house_id = house.house_id
                            INNER JOIN user_data ON user_data.user_id = house_order.user_id
                            WHERE house.owner_id = '$user_id'");
        $data = $sql->fetch_object ();
        $check = 0;
        do
        {   
            if ($check == 0 && empty ($data))
                {   
                    ?><table class = "members members3"><td><b><font size = "6"><?php echo "尚未有房屋被預訂";?></font></b></td></table><?php
                    break;                    
                }
            $check++;
            ?>
            <table class ="members members2">
                    <tr>
                        <td width = "25%" height = "25%"scope="row"><?php echo $data->name;?></td>
                        <td width = "25%" height = "25%"><?php echo $data->check_in;?></td>
                        <td width = "25%" height = "25%"><?php echo $data->check_out;?></td>
                        <td width = "25%" height = "25%"><?php echo $data->user_name;?></td>
                    </tr>
            </table>
            <?php
        }while($data = $sql->fetch_object ());
        ?>
    </body>
</html>