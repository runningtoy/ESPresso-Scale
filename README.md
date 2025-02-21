 **ESPresso Scale**  
Fully open source (s/w & h/w) scale project.
  
  
**Fork of the great project of jousis9** [https://github.com/jousis9/ESPresso-Scale]  
Main work is done by him! Thanks

  
![Scale](Hardware/scale.jpg)  


**Features**
- Open source firmware, software and hardware
- ESP32 (wroom module) MCU
- ADS1232 24-bit ADC from TI
- Lipo and USB powered (I connected a wireless charging pad)
- 10SPS and 80SPS sampling speed with various options regarding filtering/smoothing
- Auto calibration function (selectable weight)
- Touch buttons , using ESP32 pins or external touch module (TTP223).
- 3d printed case (fusion360 files provided)
- Auto power off and consumes no power when "sleep" to extend battery life
- enable WiFi on demand (press both buttons during startup ) -> ```credentials.h```
- OTA support (if WiFi enabled) -> ```http://SCALEIP/update```
- UDP Logging (if WiFi enabled) to ```UDP_SERVER_IP```



**What is provided ?**
- [PCB assembly guide](./Hardware/Documents)
- [Schematics (PDF)](./Hardware/Schematics)
- [PCB easyEDA project export](./Hardware/easyEDA)
- [Fusion 360 project file for the cases](./Hardware/case)
- [Bill of materials (BOM)](./Hardware/Documents) *wip*
- [Firmware (Platform.io IDE project)](./Software/Firmware)


  

**Documentation**  
*UDP LOGGING Bash*  
Command for UDP Logging  
```bash
nc -u -l -p 44444
```  

  
**Load Cell**  
- [x] [Mavin NA1](https://www.mavin.cn/na1-oiml-c3-approved-load-cell-high-accuracy-single-point-sensor_p15.html) and 
- [x] [Mavin MV-NA27-2KG](https://www.mavin.cn/kitchen-scale-load-cell-low-capacity-weight-sensor-na27_p28.html)
<img src="Hardware/case/images/XCut2.PNG" width="350px">

**known Pitfall**  
[Repeatability](https://github.com/jousis9/ESPresso-Scale/issues/2#issue-1459375448)  
To avoid it I used an aluminum beam as a carrier and glued the top part of the case on the beam  and one at the bottom.
<p>
<img src="Hardware/case/images/XCut.PNG" width="350px">
<img src="Hardware/case/images/Beam.png" width="350px">
  
[Case and mounting]()  
due to sensibility of the scale use the most flexible cables and smallest possible diamater. Stiff cables can lead to extra force on the load cell which makes it useless. I used [AWG30 silicon cables](https://www.amazon.de/Fermerry-Litzendraht-verzinnte-Kupferdrahtspule-Elektrokabel-Kit/dp/B09BFQMWL4/ref=sr_1_3?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&keywords=AWG30&qid=1674220803&sr=8-3)


**Consider contributing**  

  
**License**  
<!-- ![OSHW Certification](Hardware/OSHW_mark_GR000002_medium.png)   -->
Software under [GPL v3](https://gitlab.com/jousis/espresso-scale/blob/master/LICENSE)  
Hardware under [CERN Open Hardware Licence v1.2](https://gitlab.com/jousis/espresso-scale/blob/master/hw-LICENSE)  
Documentation under [CC BY-SA license 2.0](https://creativecommons.org/licenses/by-sa/2.0/)
