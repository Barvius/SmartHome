<?php
header('Access-Control-Allow-Origin: *');
header('Content-Type: application/json');
include './class/system.php';
include './class/db.php';
include './config.php';
$DB = New DB();

if (isset($_GET['act'])) {
  switch ($_GET['act']) {
    case 'login':
      if (isset($_POST['user']) and isset($_POST['password'])) {
        $DB->UserLogin($_POST['user'],$_POST['password']);
      } else {
        System::JsonInput(404);
      }
      break;
    case 'check_tooken':

      break;
    default:
      echo json_encode(array('code' => '404'));
      break;
  }
}
