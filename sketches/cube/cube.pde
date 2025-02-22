import processing.serial.*;

Serial myPort;
float xAngle = 0, yAngle = 0, zAngle = 0;

void setup() {
  size(500, 500, P3D);
  String portName = "/dev/cu.usbmodem142301";
  myPort = new Serial(this, portName, 115200);
  myPort.bufferUntil('\n');
}

void draw() {
  background(50);
  lights();
  
  translate(width/2, height/2);
  rotateX(radians(xAngle));
  rotateY(radians(yAngle));
  rotateZ(radians(zAngle));
  
  fill(0, 200, 255);
  stroke(255);
  box(200);
}

void serialEvent(Serial myPort) {
  String inString = myPort.readStringUntil('\n');
  if (inString != null) {
    String[] values = trim(split(inString, ","));
    if (values.length == 3) {
      xAngle = 2 * float(values[0]);
      yAngle = 2 * float(values[1]);
      zAngle = 2 * float(values[2]);
    }
  }
}
