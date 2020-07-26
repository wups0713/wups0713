<?php
    session_start ();
?>
<!DOCTYPE html>
<html>
<head>
	<title>Login</title>
    <style>
        .error {color: #FF0000;}
    </style>
    <link type="text/css" rel="stylesheet" href="login.css">
</head>
<body>
    <?php
        $input_error = "";
        if ($_SERVER["REQUEST_METHOD"] == "POST")
        {
            if (($select = $_POST["select_"]) == "註冊")
                echo "<script>window.location.href = './Register.php'</script>";
            else
            {
                $db_host = "dbhome.cs.nctu.edu.tw";
                $db_name = "wups0713_cs_HW3";
                $db_user = "wups0713_cs";
                $db_password = "aa066889";
                $db = new mysqli ($db_host, $db_user, $db_password, $db_name);
                $account = $_POST ["account_"];
                $password = hash ("sha256", $_POST ["password_"]);
                $sql = $db->prepare ("SELECT * FROM user_data WHERE account = ?");
                $sql->bind_param ("s", $account);
                $sql->execute ();
                $result = $sql->get_result ();
                if($check = $result->fetch_object())
                    if (preg_match ("/\b$check->account\b/", $account) && preg_match ("/\b$check->password\b/", $password))
                    {
                        $_SESSION ["id"] = $check->user_id;
                        $_SESSION ["admin"] = $check->admin;
                        echo "<script>window.location.href = './Homepage.php'</script>";
                        exit ();
                    }
                $input_error = "*帳號或密碼錯誤";
            }
        }
    ?>
    <div class = "board">
        <form method = "post" action = "Login.php">
            Account:<br>
            <input type = "text" name = "account_" placeholder = "Your account..">
            <br><br>
            Password:<br>
            <input type = "password" name = "password_" placeholder = "Your password..">
            <br><br>
            <input type = "submit" class = "button" name = "select_" value = "登入">
            <input type = "submit" class = "button" name = "select_" value = "註冊">
            <br><br>
            <span class = "error"><?php echo $input_error;?></span>
            <br>
        </form>
    </div>
</body>
</html>