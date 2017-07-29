<?php
/**
 * gpio class
 */
class Gpio {
  public function on($value=''){
    $mysql_results = $DB->pdo->prepare("INSERT INTO `gpio_logs`(`gpio_num`, `time`, `ip`, `user`, `action`) VALUES (:gpio_num, :time, :ip, :user, 'on')");
    $mysql_results->bindParam(':gpio_num', $value);
    $mysql_results->bindParam(':time', date_timestamp_get(date_create()));
    $mysql_results->bindParam(':ip', $_SERVER['REMOTE_ADDR']);
    $mysql_results->bindParam(':user',  User::UserName());
    $mysql_results->execute();
    json_input(200);
  }
  public function off($value=''){
    $mysql_results = $this->pdo->prepare("INSERT INTO `gpio_logs`(`gpio_num`, `time`, `ip`, `user`, `action`) VALUES (:gpio_num, :time, :ip, :user, 'off')");
    $mysql_results->bindParam(':gpio_num', $value);
    $mysql_results->bindParam(':time', date_timestamp_get(date_create()));
    $mysql_results->bindParam(':ip', $_SERVER['REMOTE_ADDR']);
    $mysql_results->bindParam(':user',  User::UserName());
    $mysql_results->execute();
    json_input(200);
  }
}

 ?>
