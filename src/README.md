# DEMO-git para sa Kontroler Pad

Ito ang panukalang kalipunan ng mga kodigo para sa Kontroler Pad ng LundayHangin.

### Mga Funsiyong Naipatupad
`checkIICPeripheral()` 
`printPeripheralStat()`
`scanIICPeripheral()`
`debugPrint()`
___________

* `checkIICPeripheral()`

Tiyakin na ang lahat ng mga IIC periperal na nakakonekta patungo sa MCU. Ang lahat ng IIC periperal ay gumagamit ng IIC protokol at nakakonekta sa mga bulos ng SDA at SCL.

* `debugPrint()`

Ipakita sa Serial Monitor ang mga debug messages habang pinauunlad ang mulaang kodigo. Palitan ang X ng 1 o kaya 0. Ang 1 ay para mapagana ang debug messages at 0 ang hindi. 

``` cpp
#define DEBUG X
```
Narito ang output sa monitor kung pagaganahin ito.
``` bash
DEBUGGING: 
    'checkIICPeripheral()'
    Sisimulang magtsek ng mga IIC periperal.
```

* `printPeripheralStat()`

Ipakita sa Serial Monitor ang katayuan ng lahat ng periperal, IIC o SPI man. Narito sa ibaba ang halimbawa.

``` bash
        +---------------------------------------------------+
        |                   MGA PERIPERAL                   |
        +--------+----------+-----------+----------+--------+
        | Ngalan | Protokol |   Adres   |   Tsip   | Estado |
        +--------+----------+-----------+----------+--------+
        | GPIOXL | I2C      | 0x20      | MCP23017 | OK     |
        | GPIOXR | I2C      | 0x21      | MCP23017 | OK     |
        | EEPROM | I2C      | 0x57      | ATMHK218 | OK     |
        | RTC    | I2C      | 0x68      | DS3231   | OK     |
        | IMU    | I2C      | 0x69      | MPU6050  | OK     |
        | MSC    | SPI      | 0x21-GPA7 | AIPA347  | Wala   |
        +--------+----------+-----------+----------+--------+
```

* `scanIICPeripheral()`

Suriin ang mga IIC address ng mga IIC periperal na nakalista sa listahan.

___

### Mga Funsiyong Isasaayos
`checkSPIPeripheral()`