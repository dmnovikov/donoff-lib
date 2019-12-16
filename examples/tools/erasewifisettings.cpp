/* 

this tiny tool erase wifi settings AND put esp to circle reboot mode (it's need),
after once flash this sript (erasing settings), flash it with comment line
//*((int*) 0) = 0;
to prevent circlying reboots
*/

void setup() {
  Serial.begin(9600);
  Serial.println("Starting.. erse.");  

  ESP.eraseConfig();

  Serial.println("Craching by WDT")

  *((int*) 0) = 0;

}


void loop() {
}