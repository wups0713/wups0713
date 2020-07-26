<?php
    session_start ();    
?>
<!DOCTYPE html>
<html>
<head>
    <title>User_management</title>
    <link type="text/css" rel="stylesheet" href="user_management.css">
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
        if (($select = @$_POST["homepage"]) == "首頁")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './Homepage.php'</script>";
        if (($select = @$_POST["new_user"]) == "新增使用者")      //@ 不輸出警告訊息
            echo "<script>window.location.href = './New_user.php'</script>";
        if ($admin)
            $identity = "admin";
        else
            $identity = "user";
        $sql = $db->query ("SELECT * FROM user_data WHERE user_id = '$user_id'");
        $data = $sql->fetch_object ();
        $account = $data->account;
        $name = $data->name;
        $email = $data->email;
        ?>
        <table class = "members members1">
            <form method = "post" action = "User_management.php">
                <div float = "left" text-align = "left"> 
                <h1>Profile
                <input type = "submit" class = "button button4" name = "logout" value = "登出">
                <input type = "submit" class = "button button5" name = "homepage" value = "首頁"><?php
                if ($_SESSION ["admin"] == 1)
                     echo " "."<input type = 'submit' class = 'button button3' name = 'new_user' value = '新增使用者'><br>";
                ?>
                </div>
                </h1>
                <br>
                <tr>
                    <th width = "25%" height = "25%">Account</th>
                    <th width = "25%" height = "25%">Name</th>
                    <th width = "25%" height = "25%">Email</th>
                    <th width = "25%" height = "25%">Identity</th>
                </tr>
                <tr>
                    <td width = "25%" height = "25%"scope="row"><?php echo $account;?></td>
                    <td width = "25%" height = "25%"><?php echo $name;?></td>
                    <td width = "25%" height = "25%"><?php echo $email;?></td>
                    <td width = "25%" height = "25%"><?php echo $identity;?></td>
                </tr>
            </form>
    </table>
        <?php
        if ($admin == 1)
        {
            $id = 0;
            $sql = $db->query ("SELECT * FROM user_data");
            while ($data = $sql->fetch_object ())
            {
                if ($data->user_id != $user_id)
                {
                    while ($data->user_id != $id)
                        $id++;
                    if ($data->admin)
                        $identity = "admin";
                    else
                        $identity = "user";?>
                        <table class ="members members2">
                        <tr>
                            <td width = "25%" height = "25%"scope="row"><?php echo $data->account;?></td>
                            <td width = "25%" height = "25%"><?php echo $data->name;?></td>
                            <td width = "25%" height = "25%"><?php echo $data->email;?></td>
                            <?php
                    if ($data->admin)
                    {
                        $checked_user = '';
                        $checked_admin = "checked";        
                    }
                    else
                    {
                        $checked_user = "checked";
                        $checked_admin = '';
                    }
                    $complete_ = "complete".$id;
                    $delete_ = "delete".$id;?>
                    <td width = "25%" height = "25%"><?php 
                    echo "<form method = 'post' action = 'User_management.php'>";?>
                    <div align="left"><?php
                    echo $identity."<div align='right'>"."<input type = 'submit' class = 'button' name = '".$complete_."' value = '變更'>";
                    echo " "."<input type = 'submit' class = 'button'  name = '".$delete_."' value = '刪除'>";
                        ?>
                    </div><?php
                    echo "</form>";?>
                    </td>
                    </tr>
                </table><?php
                }
            }
            while ($id)
            {
                if (isset ($_POST ["complete".$id]))
                {
                    $sql = $db->query ("SELECT user_id, admin FROM user_data WHERE user_id = '$id'");
                    $data_update = $sql->fetch_object ();
                    $update = ($data_update->admin)?0:1;
                    $sql = $db->query ("UPDATE user_data SET admin = '$update' WHERE user_id = '$id'");
                    echo "<script>window.location.href = './User_management.php'</script>";
                }
                if (isset ($_POST ["delete".$id]))
                {
                    $sql = $db->query ("DELETE FROM house WHERE owner_id = '$id'");
                    $sql = $db->query ("DELETE FROM user_data WHERE user_id = '$id'");
                    echo "<script>window.location.href = './User_management.php'</script>";
                }
                $id--;
            }
        }
    ?>
</body>
</html>