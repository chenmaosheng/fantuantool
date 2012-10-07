<?php

$name=$_POST["Account"]; 
$password=$_POST["Password"];
$repassword=$_POST["RePassword"];

if ($password != $repassword)
{
	echo "Password must be the same";
	exit();
}

if (!$name || !$password)
{
	echo "All columns must be filled";
	exit();
}

$encryptPass="Fantuan";
$method="aes128";
$iv="1234567812345678";

$encryptString = openssl_encrypt($password, $method, $encryptPass, true, $iv);

$con=mysql_connect("localhost:3306", "root", "passw0rD");
if (!$con)
{
	echo "Connect to DB failed";
	exit();
}

mysql_select_db("account", $con);

$result = mysql_query("select * from account where accountName='".$name."'", $con);
if ($result && mysql_num_rows($result) != 0)
{
	echo "The account exists";
	exit();
}

mysql_query("insert into account(accountName, password) values('".$name."','".$encryptString."')", $con);

echo "The account is created, please step in game";
exit();

?>