<?php
/**
 *
 */
class System {

    public function JsonInput($code,$data=NULL){
      echo json_encode(array('code'=>$code,'data'=>$data),JSON_NUMERIC_CHECK);
    }

    public function Gpio($port,$state){
      exec("echo $state > /sys/devices/platform/gpio_sw.16/gpio_sw/$port/data");
    }

    public function GpioRead($port){
      self::JsonInput(200,array('port' => $port, 'state' => exec("cat /sys/devices/platform/gpio_sw.16/gpio_sw/$port/data")));
    }
}


 ?>
