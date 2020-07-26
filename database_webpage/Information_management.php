<?php
    session_start ();    
?>
<!DOCTYPE html>
<html>
    <head>
        <title>Information_management</title>
        <link type="text/css" rel="stylesheet" href="house_management.css">
    </head>
    <body>
        <?php   
            $information_error = "";
            $check_error = 1;
            if (empty ($_SESSION ["id"]))
                echo "<script>window.location.href = './Login.php'</script>";
            if (!$_SESSION ["admin"])
                echo "<script>window.location.href = './Homepage.php'</script>";
            $db_host = "dbhome.cs.nctu.edu.tw";
            $db_name = "wups0713_cs_HW3";
            $db_user = "wups0713_cs";
            $db_password = "aa066889";
            $db = new mysqli ($db_host, $db_user, $db_password, $db_name);
            if (($select = @$_POST["homepage"]) == "首頁")      //@ 不輸出警告訊息
                echo "<script>window.location.href = './Homepage.php'</script>";
            if (($select = @$_POST["add"]) == "新增"){
                if (empty ($_POST ["information_"])){
                    $information_error = "資訊不能為空";
                    $check_error = 0; 
                }
                else{
                    $sql = $db->prepare ("SELECT * FROM information_table WHERE information = ?");
                    $sql->bind_param ("s", $_POST ["information_"]);
                    $sql->execute ();
                    $result = $sql->get_result ();
                    if ($check = $result->fetch_object())
                        if (preg_match ("/\b$check->information\b/", $_POST ["information_"]))
                        {   
                            $information_error = "此資訊已存在";
                            $check_error = 0; 
                        }
                }
                if($check_error){
                    $information = $_POST ["information_"];
                    $sql = $db->prepare ("INSERT INTO information_table (information)  VALUES (?)");
                    $sql->bind_param("s", $information);
                    $sql->execute ();
                    echo "<script>window.location.href = './Information_management.php'</script>";
                } 
            } 
            ?>
            <table class = "members members1">
            <form method = "post" action = "Information_management.php">
                <div float = "center" text-align = "center"> 
                <h1>Information
                <input type = "submit" class = "button button3" name = "homepage" value = "首頁"> 
                </h1>
                <br>
                <font size=4>Add Information:</font>
                <input type = "text" name = "information_" placeholder = "information..">
                <span class = "error"><?php echo $information_error;?></span>
                <input type = "submit" class = "button" name = "add" value = "新增">
                </div>
                
                <br>
                <tr>
                    <th width = "10%" height = "25%">ID</th>
                    <th width = "80%" height = "25%">Information</th>
                    <th width = "10%" height = "25%"></th>
                </tr>
            </form>
            </table>
            <?php
            $sql = $db->query ("SELECT DISTINCT information_id , information 
                                FROM information_table");
            $data = $sql->fetch_object ();
            $id_ = 0;
            $check = 0;
            do
            {   
                if ($check == 0 && empty ($data))
                    {   
                        ?><table class = "members members3"><td><b><font size = "6"><?php echo "尚未有任何資訊";?></font></b></td></table><?php
                        break;                    
                    }
                $id_ = ($id_> $data->information_id)?$id_:$data->information_id;
                $check++;
                ?>
            <table class ="members members2">
                    <tr>
                        <td width = "10%" height = "25%"scope="row"><?php echo $data->information_id;?></td>
                        <td width = "80%" height = "25%"><?php echo $data->information;?></td>
                    <?php
                    $delete_ = "delete".$id_;
                    ?>
                    <td width = "10%" height = "25%"><?php 
                        echo "</form>";
                        echo "<form method = 'post' action = 'Information_management.php'>";
                        echo " "."<input type = 'submit' class = 'button'  name = '".$delete_."' value = '刪除'>";
                        echo "</form>";?>
                    </td>
                    </tr>
                </table>
                <?php      
            }while($data = $sql->fetch_object ());
            while ($id_)
            {   
                if (isset ($_POST ["delete".$id_]))
                {
                    $sql = $db->query ("DELETE FROM information_table WHERE information_id  = '$id_'");
                    echo "<script>window.location.href = './Information_management.php'</script>";
                }
                $id_--;
            }   
        ?>
    </body>
</html>