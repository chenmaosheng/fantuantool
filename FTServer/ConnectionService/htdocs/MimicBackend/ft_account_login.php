<?php

$name=$_POST["name"]; 
$password=$_POST["password"];

$encryptPass="Fantuan";
$method="aes128";
$iv="1234567812345678";

$encryptString = openssl_encrypt($password, $method, $encryptPass, true, $iv);

$con=mysql_connect("localhost:3306", "root", "passw0rD");
if (!$con)
{
	echo "-1";
	exit();
}

mysql_select_db("account", $con);

$result = mysql_query("select * from account where accountName='".$name."' and password='".$encryptString."'", $con);
if (!$result || mysql_num_rows($result) == 0)
{
	echo "-2";
	exit();
}

$token = $name.";".$encryptString;
echo $token;
exit();

?>