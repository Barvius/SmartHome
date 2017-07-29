<?php
/**
 * db
 */
 class SmartHome {
   private $dbServer;
   private $dbUser;
   private $dbPassword;
   private $dbName;
   private $timezone;

   public $pdo;

   public function __construct(){
     $this->dbServer = dbServer;
     $this->dbUser = dbUser;
     $this->dbPassword = dbPassword;
     $this->dbName = dbName;
     $this->pdo = new PDO("mysql:host=$this->dbServer;dbname=$this->dbName;charset=utf8", $this->dbUser, $this->dbPassword);
   }

   public function __destruct(){
     $this->pdo = null;
   }

   public function CheckTooken($tooken){
     $mysql_results = $this->pdo->prepare("SELECT * FROM users WHERE tooken=:tooken");
     $mysql_results->bindParam(':tooken', $tooken);
     $mysql_results->execute();
     $rr = $mysql_results->fetch(PDO::FETCH_ASSOC);
     if(hash('sha256', $rr['password'] + $_SERVER['REMOTE_ADDR']) == $tooken){
       return 1;
     }
   }

   public function UserLogin($username,$password){
     $mysql_results = $this->pdo->prepare("SELECT * FROM users WHERE name=:user AND password=:password");
     $mysql_results->bindParam(':user', $username);
     $mysql_results->bindParam(':password', $password);
     $mysql_results->execute();
     if($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)){
         $mysql_results = $this->pdo->prepare("UPDATE `users` SET tooken=:tooken WHERE id=:id");
         $mysql_results->bindParam(':tooken', hash('sha256', $rr['password'] + $_SERVER['REMOTE_ADDR']));
         $mysql_results->bindParam(':id', $rr['id']);
         $mysql_results->execute();
         System::JsonInput(200,array('username' => $username, 'user_level'=>$rr['user_level'], 'tooken' => hash('sha256', $rr['password'] + $_SERVER['REMOTE_ADDR'])));
       } else {
         System::JsonInput(403);
       }

   }

   public function GetUserLevel(){
     $mysql_results = $this->pdo->prepare("SELECT * FROM users WHERE tooken=:tooken");
     $mysql_results->bindParam(':tooken', $_GET['tooken']);
     $mysql_results->execute();
     $rr = $mysql_results->fetch(PDO::FETCH_ASSOC);
     return $rr['user_level'];
   }

   public function GetUserName(){
     $mysql_results = $this->pdo->prepare("SELECT * FROM users WHERE tooken=:tooken");
     $mysql_results->bindParam(':tooken', $_GET['tooken']);
     $mysql_results->execute();
     $rr = $mysql_results->fetch(PDO::FETCH_ASSOC);
     return $rr['name'];
   }

   public function GetUserInfo(){
     $mysql_results = $this->pdo->prepare("SELECT * FROM users WHERE tooken=:tooken");
     $mysql_results->bindParam(':tooken', $_GET['tooken']);
     $mysql_results->execute();
     $rr = $mysql_results->fetch(PDO::FETCH_ASSOC);
     System::JsonInput(200,array('name' => $rr['name'], 'level' => $rr['user_level']));
   }

   public function CheckFunctionality($value){
     $mysql_results = $this->pdo->query("SELECT * FROM sys_functionality WHERE id = 1");
     $rr = $mysql_results->fetch(PDO::FETCH_ASSOC);
     return $rr[$value];
   }

   public function GetFunctionality(){
     $mysql_results = $this->pdo->query("SELECT * FROM sys_functionality WHERE id = 1");
     $rr = $mysql_results->fetch(PDO::FETCH_ASSOC);
     System::JsonInput(200, $rr);
   }

   public function MainLogWrite($type,$zone,$msg){
     $mysql_results = $this->pdo->prepare("INSERT INTO `main_log`(`time`, `type`, `zone`, `msg`) VALUES (:time, :type, :zone, :msg)");
     $mysql_results->bindParam(':time', date_timestamp_get(date_create()));
     $mysql_results->bindParam(':type', $type);
     $mysql_results->bindParam(':zone', $zone);
     $mysql_results->bindParam(':msg',  $msg);
     $mysql_results->execute();
   }

   public function MainLogRead(){
     $mysql_results = $this->pdo->query("SELECT * FROM `main_log` order by time desc limit 10");
     while($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)) {
       $data[] = array($rr['time']*1000,$rr['type'],$rr['zone'],$rr['msg']);
     }
     System::JsonInput(200,$data);
   }

   public function GpioLogWrite($port,$action,$ip="",$user=""){
     if (!$ip) {
       $ip = $_SERVER['REMOTE_ADDR'];
     }
     if (!$user) {
       $user=$this->GetUserName();
     }
     $mysql_results = $this->pdo->prepare("INSERT INTO `gpio_logs`(`gpio_num`, `time`, `ip`, `user`, `action`) VALUES (:gpio_num, :time, :ip, :user, :action)");
     $mysql_results->bindParam(':gpio_num', $port);
     $mysql_results->bindParam(':time', date_timestamp_get(date_create()));
     $mysql_results->bindParam(':ip', $ip);
     $mysql_results->bindParam(':user', $user);
     $mysql_results->bindParam(':action',  $action);
     $mysql_results->execute();
   }

   public function GpioLogRead($port){
     $mysql_results = $this->pdo->prepare("SELECT * FROM `gpio_logs` where `gpio_num`=:gpio_num order by time desc limit 15");
     $mysql_results->bindParam(':gpio_num', $port);
     $mysql_results->execute();
     while($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)) {
       $data[] = array($rr['time']*1000,$rr['ip'],$rr['user'],$rr['action']);
     }
     System::JsonInput(200,$data);
   }

   public function GpioSetAtribute($port,$name,$functionality,$inverse,$driver){
     $mysql_results = $this->pdo->prepare("UPDATE `gpio_aliases` SET `name`=:name, `functionality`=:functionality, `inverse`=:inverse, `driver`=:driver WHERE port=:port");
     $mysql_results->bindParam(':port', $port);
     $mysql_results->bindParam(':name', $name);
     $mysql_results->bindParam(':functionality', $functionality);
     $mysql_results->bindParam(':inverse', $inverse);
     $mysql_results->bindParam(':driver', $driver);
     $mysql_results->execute();
     System::JsonInput(200);
   }

   public function GpioList(){
     $mysql_results = $this->pdo->query("SELECT * FROM `gpio_aliases`");
     while($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)) {
       $data[] = $rr['port'];
       $name[$rr['port']] = array($rr['name'],$rr['functionality'],$rr['inverse'],$rr['driver']);
       if($rr['functionality'] == 'light'){
         $render[] = $rr['port'];
       }
     }
     System::JsonInput(200,array('id' => $data,'render_ids' => $render, 'atr'=> $name));
   }

   public function SensorSetAtribute($ids,$name,$prefix,$location,$enable,$inindex,$narodmon){
     $mysql_results = $this->pdo->prepare("SELECT `id` FROM `name_aliases` WHERE `sensor_id`=:sensor_id");
     $mysql_results->bindParam(':sensor_id', $ids);
     $mysql_results->execute();
     $id = $mysql_results->fetch(PDO::FETCH_ASSOC);
     if(!$id) {
       $mysql_results = $this->pdo->prepare("INSERT INTO `name_aliases`(`sensor_id`, `name`, `prefix`, `location`, `enable`, `inindex`, `narodmon`) VALUES (:sensor_id, :name, :prefix, :location, :enable, :inindex, :narodmon)");
       $mysql_results->bindParam(':sensor_id', $ids);
       $mysql_results->bindParam(':name', $name);
       $mysql_results->bindParam(':prefix', $prefix);
       $mysql_results->bindParam(':location', $location);
       $mysql_results->bindParam(':enable', $enable);
       $mysql_results->bindParam(':inindex', $inindex);
       $mysql_results->bindParam(':narodmon', $narodmon);
       $mysql_results->execute();
       System::JsonInput(200);
     } else {
       $mysql_results = $this->pdo->prepare("UPDATE `name_aliases` SET `sensor_id`=:sensor_id,`name`=:name, `prefix`=:prefix, `location`=:location, `enable`=:enable, `inindex`=:inindex , `narodmon`=:narodmon WHERE id=:id");
       $mysql_results->bindParam(':id', $id['id']);
       $mysql_results->bindParam(':sensor_id', $ids);
       $mysql_results->bindParam(':name', $name);
       $mysql_results->bindParam(':prefix', $prefix);
       $mysql_results->bindParam(':location', $location);
       $mysql_results->bindParam(':enable', $enable);
       $mysql_results->bindParam(':inindex', $inindex);
       $mysql_results->bindParam(':narodmon', $narodmon);
       $mysql_results->execute();
       System::JsonInput(200);
     }
   }

   public function SensorList(){
    //  $mysql_results = $this->pdo->query("SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name = 'stats' AND table_schema = '$this->dbName' AND column_name LIKE '%\_%'");
    //  while($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)) {
    //    $data[] = $rr['COLUMN_NAME'];
    //  }
     $mysql_results = $this->pdo->query("SELECT * FROM `name_aliases`");
     while($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)) {
       $name[] = array('id' => $rr['sensor_id'], 'name' => $rr['name'], 'prefix' => $rr['prefix'], 'location' => $rr['location'], 'en' => $rr['enable'], 'ii' => $rr['inindex'], 'nm' => $rr['narodmon']);
      //  if($rr['enable']){
      //    $render[$rr['location']][] = $rr['sensor_id'];
      //  }
      //  if($rr['inindex']){
      //    $inindex[] = $rr['sensor_id'];
      //  }
      //  if($rr['narodmon']){
      //    $narodmon[] = $rr['sensor_id'];
      //  }
     }
    //  $mysql_results = $this->pdo->query("SELECT * FROM `sensor_zone`");
    //  while($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)) {
    //    $zone[] = array($rr['id'],$rr['name']);
    //  }

     return $name;
   }

   public function SensorGet($id){

     $mysql_results = $this->pdo->prepare("SELECT * FROM `name_aliases` where `sensor_id` = :sensor_id");
     $mysql_results->bindParam(':sensor_id', $id);
     $mysql_results->execute();
     while($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)) {
       $name = array('id' => $rr['sensor_id'], 'name' => $rr['name'], 'prefix' => $rr['prefix'], 'location' => $rr['location'], 'en' => $rr['enable'], 'ii' => $rr['inindex'], 'nm' => $rr['narodmon']);
     }
     return $name;
   }

   public function SensorCheck($id){
     $mysql_results = $this->pdo->query("SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name = 'stats' AND table_schema = '$this->dbName' AND column_name LIKE '%\_%'");
     while($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)) {
       $sens_list[] = $rr['COLUMN_NAME'];
     }
     if (in_array($id, $sens_list)) {
       return 1;
     }
   }

   public function SensorGetNowData($id){
     $mysql_results = $this->pdo->query("SELECT {$id} FROM `stats` WHERE id=(SELECT MAX(id) FROM stats)");
     $rr = $mysql_results->fetch(PDO::FETCH_ASSOC);
     return array('id' => $id, 'data' => round($rr[$id],1));
   }

   public function SensorGetData($id,$limit){
     $mysql_results = $this->pdo->query("SELECT {$id} ,date FROM `stats` order by `id` desc limit {$limit}");
     while($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)) {
       if ($rr[$id]) {
         $data[] = array( ($rr['date']+10800)*1000, $rr[$id]);
       }
     }
     $temp = array();
     foreach ($data as $key => $row){
       $temp[$key] = $row[0];
     }
     array_multisort($temp, SORT_ASC, $data);
     System::JsonInput(200, array('id' => $id, 'data' => $data));
   }

   public function SensorGetMinifidData($id,$limit){
     $mysql_results = $this->pdo->query("SELECT {$id} ,date FROM `stats` order by `id` desc limit {$limit}");
     while($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)) {
       if ($rr[$id]) {
         $data[] = array( ($rr['date']+10800)*1000, $rr[$id]);
       }
     }
     $temp = array();
      foreach ($data as $key => $row){
        $temp[$key] = $row[0];
      }
     array_multisort($temp, SORT_DESC,$data);
     for ($i=0; $i < count($data); $i+=6) {
       $min = 125;
       $max = -55;
       $sr = 0;
       for ($j=0; $j < 6; $j++) {
         if ($data[$i+$j][1] > $max) {
           $max = $data[$i+$j][1];
         }
         if ($data[$i+$j][1] < $min) {
           $min = $data[$i+$j][1];
         }
         $sr += $data[$i+$j][1];
       }
       $arr['range'][] = array($data[$i][0],round($min,1),round($max,1));
       $arr['avg'][] = array($data[$i][0],round($sr/6,1));
     }

     System::JsonInput(200, array('id' => $id, 'data' => array('range' => $arr['range'], 'avg' => $arr['avg'])));
   }

   public function SensorGetAllData($id){
     $mysql_results = $this->pdo->query("SELECT {$id} ,date FROM `stats` order by `id`");
     while($rr = $mysql_results->fetch(PDO::FETCH_ASSOC)) {
       $data[] = array( ($rr['date']+10800)*1000, $rr[$id]);
     }
     System::JsonInput(200, array('id' => $id, 'data' => $data));
   }

   public function GetHeatingSystemConfig(){
     $mysql_results = $this->pdo->query("SELECT * FROM `heating_system` WHERE `id`=1");
     $rr = $mysql_results->fetch(PDO::FETCH_ASSOC);
     $config = array('max_temp' => $rr['max_temp'], 'min_temp' => $rr['min_temp'], 'mode' => $rr['mode'], 'port' => $rr['port'], 'sensor' => $rr['sensor']);
     return $config;
   }

   public function SetHeatingSystemMode($mode){
     $mysql_results = $this->pdo->prepare("UPDATE `heating_system` SET `mode`=:mode WHERE `id`=1");
     $mysql_results->bindParam(':mode', $mode);
     $mysql_results->execute();
     System::JsonInput(200);
   }

   public function SetHeatingSystemTemperature($max_temp, $min_temp){
     $mysql_results = $this->pdo->prepare("UPDATE `heating_system` SET `max_temp`=:max_temp, `min_temp`=:min_temp WHERE `id`=1");
     $mysql_results->bindParam(':max_temp', $max_temp);
     $mysql_results->bindParam(':min_temp', $min_temp);
     $mysql_results->execute();
     System::JsonInput(200);
   }

   public function SetHeatingSystemConfig($port, $sensor){
     $mysql_results = $this->pdo->prepare("UPDATE `heating_system` SET `port`=:port, `sensor`=:sensor WHERE `id`=1");
     $mysql_results->bindParam(':port', $port);
     $mysql_results->bindParam(':sensor', $sensor);
     $mysql_results->execute();
     System::JsonInput(200);
   }

   public function GpioRead($port){
     $state=NULL;
     $mysql_results = $this->pdo->prepare("SELECT * FROM `gpio_aliases` WHERE `port`=:port");
     $mysql_results->bindParam(':port', $port);
     $mysql_results->execute();
     $rr = $mysql_results->fetch(PDO::FETCH_ASSOC);
     switch ($rr['driver']) {
       case 'gpio_standart':
         $state = exec("cat /sys/class/gpio/gpio$port/value");
         if ($rr['inverse']) {
           $state = $state ? 0 : 1;
         }
         break;
       case 'gpio_sunxi':
         $state = exec("cat /sys/devices/platform/gpio_sw.16/gpio_sw/$port/data");
         if ($rr['inverse']) {
           $state = $state ? 0 : 1;
         }
         break;
     }
     return array('port' => $port, 'state' => $state);
   }

   public function GpioWrite($port,$state){
     $mysql_results = $this->pdo->prepare("SELECT * FROM `gpio_aliases` WHERE `port`=:port");
     $mysql_results->bindParam(':port', $port);
     $mysql_results->execute();
     $rr = $mysql_results->fetch(PDO::FETCH_ASSOC);
     switch ($rr['driver']) {
       case 'gpio_standart':
         if ($rr['inverse']) {
           $state = $state ? 0 : 1;
         }
         exec("echo $state > /sys/class/gpio/gpio$port/value");
         break;
       case 'gpio_sunxi':
         if ($rr['inverse']) {
           $state = $state ? 0 : 1;
         }
         exec("echo $state > /sys/devices/platform/gpio_sw.16/gpio_sw/$port/data");
         break;
     }
   }

   public function AddSensorZone($name){
     $mysql_results = $this->pdo->prepare("INSERT INTO `sensor_zone`(`name`) VALUES (:name)");
     $mysql_results->bindParam(':name', $name);
     $mysql_results->execute();
     System::JsonInput(200);
   }

   public function RemoveSensorZone($name){
     $mysql_results = $this->pdo->prepare("DELETE FROM `sensor_zone` WHERE `name`=:name");
     $mysql_results->bindParam(':name', $name);
     $mysql_results->execute();
     System::JsonInput(200);
   }

 }

 ?>
