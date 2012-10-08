<?php
	
$name=$_POST["Account"]; 
$password=$_POST["Password"];

if (!$name || !$password)
{
	echo "-1";
	exit();
}

$url="mimicBackend/ft_account_login.php?name=".$name."&password=".$password;
$result = file_get_contents($url);

if ($result == false)
{
	echo "-1";
	exit();
}

echo $result;

?>