# cmd
## set
* 0xCEA0 - set mode (bool) (1-auto/0-manual)
* 0xCEA1 - set on temp (int)
* 0xCEA2 - set off temp (int)
* 0xCEF - set state (bool) (1-on/0-off)
* 0xCEB0 - alarm (bool) (1-on/0-off)
* 0xCEB1 - alarm temp (int)

## get
* 0xCFA0 - get mode
* 0xCFA1 - get temp on
* 0xCFA2 - get temp off
* 0xCFF - get pump state
* 0xCFB0 - get alarm mode
* 0xCFB1 - get alarm temp
* 0xCFC0 - get external temperature
* 0xCFC1 - get external humidity
* 0xCFE - get system tempearature
