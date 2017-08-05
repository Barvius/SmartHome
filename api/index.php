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

if (isset($_GET['tooken'])) {
  if ($DB->CheckTooken($_GET['tooken'])) {
    //основной иф авторизации
    if (isset($_GET['system'])) {
      switch ($_GET['system']) {
        case 'functionality':
          $DB->GetFunctionality();
          break;

        case 'log':
          $DB->MainLogRead();
          break;

        default:
          # code...
          break;
      }
    };

    if (isset($_GET['user'])) {
      switch ($_GET['user']) {
        case 'info':
          $DB->GetUserInfo();
          break;

        case 'check':
          System::JsonInput(200);
          break;
        default:
          # code...
          break;
      }
    };

    if (isset($_GET['heating_system'])) {
      switch ($_GET['heating_system']) {
        case 'get_config':
          System::JsonInput(200,$DB->GetHeatingSystemConfig());
          break;

        case 'set_mode':
          if (isset($_POST['mode'])) {
            $DB->SetHeatingSystemMode($_POST['mode']);
          }
          break;

        case 'set_temperature':
          if (isset($_POST['max_temp']) and isset($_POST['min_temp'])) {
            $DB->SetHeatingSystemTemperature($_POST['max_temp'],$_POST['min_temp']);
          }
          break;

        case 'set_config':
          if (isset($_POST['port']) and isset($_POST['sensor'])) {
            $DB->SetHeatingSystemConfig($_POST['port'],$_POST['sensor']);
          }
          break;

        case 'check':
          System::JsonInput(200);
          break;
        default:
          # code...
          break;
      }
    };

    if (isset($_GET['gpio']) and $DB->CheckFunctionality('gpio')) {
      switch ($_GET['gpio']) {
        case 'set_atr':
          if (isset($_GET['port'])) {
            if($DB->GetUserLevel == 0){
              $DB->GpioSetAtribute($_GET['port'],$_POST['name'],$_POST['functionality'],$_POST['inverse'],$_POST['driver']);
            } else {
              System::JsonInput(403);
            }
          }
          break;
        case 'list':
          $DB->GpioList();
          break;
        case 'st':
          if (isset($_GET['port'])) {
            System::JsonInput(200,$DB->GpioRead($_GET['port']));
          }
          break;
        case 'on':
          if (isset($_GET['port'])) {
            $DB->GpioLogWrite($_GET['port'], 'on');
            $DB->GpioWrite($_GET['port'],1);
          }
          break;
        case 'off':
          if (isset($_GET['port'])) {
            $DB->GpioLogWrite($_GET['port'], 'off');
            $DB->GpioWrite($_GET['port'],0);
          }
          break;
        case 'log':
          if (isset($_GET['port'])) {
            $DB->GpioLogRead($_GET['port']);
          }
          break;
        default:
          System::JsonInput(404);
          break;
      };
    };

    if (isset($_GET['sensor']) and $DB->CheckFunctionality('sensors')) {
      switch ($_GET['sensor']) {
        case 'update':
        if ($_SERVER['REQUEST_METHOD'] == 'POST'){
        $data = json_decode(file_get_contents("php://input"));
        print($data->id);
        $DB->SensorSetAtribute($data->id,$data->cmd,$data->name,$data->prefix,$data->location,$data->en,$data->ii,$data->nm);
        }
          break;
        case 'get_all':
          System::JsonInput(200,$DB->SensorList());
          break;

        case 'get':
        if (isset($_GET['id'])) {
          if ($DB->SensorCheck($_GET['id'])) {
              System::JsonInput(200,$DB->SensorGet($_GET['id']));
          }
        }
          break;
          case 'stats':
          if (isset($_GET['id']) and isset($_GET['period'])) {
            if ($DB->SensorCheck($_GET['id'])) {
              switch ($_GET['period']) {
                case 'all':
                  $DB->SensorGetAllData($_GET['id']);
                  break;
                case '1d':
                  $DB->SensorGetData($_GET['id'],'288');
                  break;
                case 'now':
                System::JsonInput(200, $DB->SensorGetNowData($_GET['id']));
                  break;
                case 'minifid':
                  $DB->SensorGetMinifidData($_GET['id'],288);
                  break;

                default:
                  System::JsonInput(404);
                  break;
              }
            }
          }
            break;
          //

        default:
          System::JsonInput(404);
          break;
      };
    };
  // закончить выдачу ошибок,

    //основной иф авторизации
  } else {
    System::JsonInput(403);
  }
}


?>
