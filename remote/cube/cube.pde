import processing.serial.*;

Serial myPort;
float xAngle = 0, yAngle = 0, zAngle = 0;
float xOffset = 0, yOffset = 0, zOffset = 0;
boolean calibrated = false;

void setup() {
  size(500, 500, P3D);
  myPort = new Serial(this, "/dev/cu.usbmodem142301", 115200);
  myPort.bufferUntil('\n');
}

void keyPressed() {
  if (key == ' ') { // Press spacebar to calibrate
    xOffset = xAngle;
    yOffset = yAngle;
    zOffset = zAngle;
    calibrated = true;
    println("Calibrated: X Offset = " + xOffset + ", Y Offset = " + yOffset + ", Z Offset = " + zOffset);
  }
}

void serialEvent(Serial myPort) {
  String inString = myPort.readStringUntil('\n');
  if (inString != null) {    
    JSONObject json = parseJSONObject(inString);
    JSONObject imu = json.getJSONObject("imu");
    if (imu != null) {
      xAngle = imu.getFloat("pitch");
      yAngle = imu.getFloat("yaw");
      zAngle = -1.0 * imu.getFloat("roll");
    }
  }
}

void draw() {
  background(50);
  lights();
  
  translate(width / 2, height / 2);
  rotateX(radians(xAngle - xOffset));
  rotateY(radians(yAngle - yOffset));
  rotateZ(radians(zAngle - zOffset));
  
  fill(0, 200, 255);
  stroke(255);
  box(200);
}
