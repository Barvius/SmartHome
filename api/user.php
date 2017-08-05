<?php
header('Access-Control-Allow-Origin: *');
header('Content-Type: application/json');
header('Access-Control-Allow-Methods: GET, POST, PATCH, PUT, DELETE, OPTIONS');
header('Access-Control-Allow-Headers: Origin, Content-Type, X-Auth-Token');
include './class/system.php';
include './class/db.php';
include './config.php';
$DB = New SmartHome();

if (isset($_GET['act'])) {
  switch ($_GET['act']) {
    case 'login':
    if ($_SERVER['REQUEST_METHOD'] == 'POST'){
    $data = json_decode(file_get_contents("php://input"));
      $DB->UserLogin($data->username,$data->password);
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
