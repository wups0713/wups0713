<?php
    session_start ();    
?>
<!DOCTYPE html>
<html>
<head>
	<title>New_user</title>
	<style>
		.error {color: #FF0000;}
	</style>
	<link type="text/css" rel="stylesheet" href="new_user.css">
</head>
<body>
	<?php
		if (empty ($_SESSION ["id"]))
			echo "<script>window.location.href = './Login.php'</script>";
		$account_error = $password_error = $password_confirmation_error = $name_error = $email_error = "";
		$check_error = 0;
		$admin_check = $_SESSION ["admin"];
		if ($admin_check == 1)
		{
			if ($_SERVER["REQUEST_METHOD"] == "POST")
			{
				$db_host = "dbhome.cs.nctu.edu.tw";
				$db_name = "wups0713_cs_HW3";
				$db_user = "wups0713_cs";
				$db_password = "********";
				$db = new mysqli ($db_host, $db_user, $db_password, $db_name);
				if (@$_POST ["back"] == "返回")
					echo "<script>window.location.href = './User_management.php'</script>";
				//testing account start
				$test = 1;
				if (empty ($_POST ["account_"])){
					$account_error = "*帳號不能為空";
					$test = 0;
				}
				else	
					if (preg_match ("/\s/", $_POST ["account_"])){
							// \s 空格 or 換行
						$account_error = "*帳號格式錯誤";
						$test = 0;
					}	
					else
					{
						$sql = $db->prepare ("SELECT * FROM user_data WHERE account = ?");
						$sql->bind_param ("s", $_POST ["account_"]);
						$sql->execute ();
						$result = $sql->get_result ();
						if ($check = $result->fetch_object())
							if (preg_match ("/\b$check->account\b/", $_POST ["account_"]))
							{
								$account_error = "*此帳號已被使用";
								$test = 0;
							}
					}
				if ($test)
					$check_error++;
				//testing account end
				//testing password start
				$password_check = $_POST ["password_confirmation"];
				if (empty ($_POST ["password_"]))
					$password_error = "*密碼不能為空";
				else
					if (!preg_match ("/\b$password_check\b/", $_POST ["password_"]))
						$password_confirmation_error = "*密碼驗證錯誤";
					else
						$check_error++;
				//testing password end
				//testing name start
				if (empty ($_POST ["name_"]))
					$name_error = "*名字不能為空";
				else
					$check_error++;
				//testing name end
				//testing email start
				if (empty ($_POST ["email_"]))
					$email_error = "*信箱不能為空";
				else
					if (!preg_match ("/(^[a-zA-Z0-9]+(\.[a-zA-Z0-9]+)*@[a-zA-Z0-9]+(\.[a-zA-Z0-9]+)+$)/", $_POST ["email_"]))
					{
						//^[a-zA-Z0-9]+ 以至少一個(英文or數字)開頭
						//(\.[a-zA-Z0-9]+)* 可加上多個(.英文or數字)
						//@ 一個@
						//[a-zA-Z0-9]+ 至少一個(英文or數字)
						//(\.[a-zA-Z0-9]+)+$ 以至少一個(.英文or數字)結尾
						$email_error = "*信箱格式錯誤";
					}
					else
						$check_error++;
				//testing email end
				if ($check_error == 4)
				{
					$account = $_POST ["account_"];
					$password = hash ("sha256", $_POST ["password_"]);
					$name = $_POST ["name_"];
					$email = $_POST ["email_"];
					$user_type = ($_POST ["select_"] == "user")?'0':'1';
					$sql = $db->prepare ("INSERT INTO user_data (account, password, name, email, admin) VALUES (?, ?, ?, ?, $user_type)");
					$sql->bind_param("ssss", $account, $password, $name, $email);
					$sql->execute ();
					echo "<script>window.location.href = './User_management.php'</script>";
				}
			}
		}
		else
			echo "<script>window.location.href = './Login.php'</script>";
	?>
	<div class = "board">
	<form method = "post" action = "New_user.php">
		account:<br>
		<input type = "text" name = "account_">
		<span class = "error"><?php echo $account_error;?></span>
		<br>
		password:<br>
		<input type = "password" name = "password_">
		<span class = "error"><?php echo $password_error;?></span>
		<br>
		password confirmation:<br>
		<input type = "password" name = "password_confirmation">
		<span class = "error"><?php echo $password_confirmation_error;?></span>
		<br>
		name:<br>
		<input type = "text" name = "name_">
		<span class = "error"><?php echo $name_error;?></span>
		<br>
		email:<br>
		<input type = "text" name = "email_">
		<span class = "error"><?php echo $email_error;?></span>
		<br>
        <input type = "radio" name = "select_" value = "user" checked>user
        <input type = "radio" name = "select_" value = "admin">admin
        <br>
		<input type = "submit" class = "button" value = "新增">
        <input type = "submit" class = "button" name = "back" value = "返回">
		<br>
	</form>
	</div>
</body>
</html>