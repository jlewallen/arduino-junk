import processing.serial.*;

int axisCylinderSides = 5;
float axisCylinderRadius = 5;
float axisCylinderH = 200;
int lf = 10;
float x = 0.0;
float y = 0.0;
float z = 0.0;
Serial serial;

void drawCylinder(int sides, float r1, float r2, float h) {
    float angle = 360 / sides;
    float halfHeight = h / 2;
    // top
    beginShape();
    for (int i = 0; i < sides; i++) {
        float x = cos(radians(i * angle)) * r1;
        float y = sin(radians(i * angle)) * r1;
        vertex(x, y, -halfHeight);
    }
    endShape(CLOSE);
    // bottom
    beginShape();
    for (int i = 0; i < sides; i++) {
        float x = cos(radians(i * angle)) * r2;
        float y = sin(radians(i * angle)) * r2;
        vertex(x, y, halfHeight);
    }
    endShape(CLOSE);
    // draw body
    beginShape(TRIANGLE_STRIP);
    for (int i = 0; i < sides + 1; i++) {
        float x1 = cos(radians(i * angle)) * r1;
        float y1 = sin(radians(i * angle)) * r1;
        float x2 = cos(radians(i * angle)) * r2;
        float y2 = sin(radians(i * angle)) * r2;
        vertex(x1, y1, -halfHeight);
        vertex(x2, y2, halfHeight);
    }
    endShape(CLOSE);
}

void setup() {
  size(640, 480, P3D); 
  serial = new Serial(this, "/dev/tty.usbmodemfa141", 115200);
  serial.clear();
}

void draw() {
  background(245, 238, 184);

  pushMatrix();
  translate(640 / 2, 360, 0);
  rotateX(radians(90));
  rotateX(x);
  rotateY(y);
  rotateZ(z);
  
  pushMatrix();
  translate(0, 0, axisCylinderH / 2);
  fill(255, 128, 128);
  stroke(153);
  drawCylinder(axisCylinderSides, axisCylinderRadius, axisCylinderRadius, axisCylinderH);
  popMatrix();

  pushMatrix();
  rotateY(radians(90));
  translate(0, 0, axisCylinderH / 2);
  fill(128, 255, 128);
  drawCylinder(axisCylinderSides, axisCylinderRadius, axisCylinderRadius, axisCylinderH);
  popMatrix();

  pushMatrix();
  rotateX(radians(90));
  translate(0, 0, -axisCylinderH / 2);
  fill(128, 128, 255);
  drawCylinder(axisCylinderSides, axisCylinderRadius, axisCylinderRadius, axisCylinderH);
  popMatrix();
  
  popMatrix();
 
  if (serial.available() > 0) {
    String row = serial.readStringUntil(lf);
    if (row != null) {
        String[][] m = matchAll(row, "\\!\\s+(\\d+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)");
        if (m != null && m.length == 1) {
          x = radians(float(m[0][2]));
          y = radians(float(m[0][3]));
          z = radians(float(m[0][4]));
        }
    }
  }  
} 

