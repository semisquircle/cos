import processing.serial.*;

Serial myPort;
float yaw = 0, pitch = 0, roll = 0;
float yawOffset = 0, pitchOffset = 0, rollOffset = 0;
boolean calibrated = false;

void setup() {
  size(500, 500, P3D);
  myPort = new Serial(this, "/dev/cu.usbmodem142301", 115200);
  myPort.bufferUntil('\n');
}

void keyPressed() {
  if (key == ' ') { // Press spacebar to calibrate
    yawOffset = yaw;
    pitchOffset = pitch;
    rollOffset = roll;
    calibrated = true;
    println("Calibrated: X Offset = " + yawOffset + ", Y Offset = " + pitchOffset + ", Z Offset = " + rollOffset);
  }
}

void serialEvent(Serial myPort) {
  String inString = myPort.readStringUntil('\n');
  if (inString != null) {    
    JSONObject json = parseJSONObject(inString);
    JSONObject imu = json.getJSONObject("imu");
    if (imu != null) {
      yaw = imu.getFloat("yaw");
      pitch = imu.getFloat("pitch");
      roll = -1.0 * imu.getFloat("roll");
    }
  }
}

void draw() {
  background(50);
  lights();
  
  translate(width / 2, height / 2);
  rotateY(radians(yaw - yawOffset));
  rotateX(radians(pitch - pitchOffset));
  rotateZ(radians(roll - rollOffset));
  
  fill(0, 200, 255);
  stroke(255);
  box(200);
}
