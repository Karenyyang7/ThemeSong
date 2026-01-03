/* 
Credit: IMA Creative Computing with Ellen Nickles

Modified from:
https://github.com/ITPNYU/physcomp/tree/main/Labs/P5SerialLabs/P5ReadSerial/readSerialWebSerial

https://itp.nyu.edu/physcomp/labs/labs-serial-communication/lab-webserial-input-to-p5-js/
*/

//create a new instance of the p5.WebSerial library
//--------------- FACE + MOOD -----------------
let faceMesh;
let video;
let faces = [];
let options = { maxFaces: 1, refineLandmarks: false, flipHorizontal: false };
let mood = "neutral";

// ----------------- SERIAL + FSR + SOUND -----------------
let serial;
let portButton;
let fsrValue1 = 0;
let fsrValue2 = 0;
let threshold = 50;

let bedSong;
let ballSong;

// ----------------- VISUAL VIDEOS -----------------
let bedVid;
let ballVid;
let happyVid;
let sadVid;

let currentSong = null;
let currentVisual = null;

// ----------------- PRELOAD -----------------
function preload() {
  // songs
  bedSong = loadSound("FromTheStart.mp3");
  ballSong = loadSound("WantsandNeeds.mp3");

  // faceMesh
  faceMesh = ml5.faceMesh(options);
}

// ----------------- SETUP -----------------
function setup() {
  createCanvas(1400, 900);

  // webcam for faceMesh
  video = createCapture(VIDEO);
  video.size(640, 480);
  video.hide();
  faceMesh.detectStart(video, gotFaces);

  // visual videos  (CHANGE FILE NAMES TO YOURS)
  bedVid   = createVideo("bedVisual.mp4");
  ballVid  = createVideo("ballVisual.mp4");
  happyVid = createVideo("happyVisual.mp4");
  sadVid   = createVideo("sadVisual.mp4");

  // hide HTML video elements
  bedVid.hide();
  ballVid.hide();
  happyVid.hide();
  sadVid.hide();

  // WebSerial
  serial = new p5.WebSerial();
  setupSerial();
}

// ----------------- DRAW -----------------
function draw() {
  background(10);

  // update mood from face
  detectMood();

  // decide what should be active
  let pressed1 = fsrValue1 > threshold;   // object 1 picked up
  let pressed2 = fsrValue2 > threshold;   // object 2 picked up

  let nextVisual = null;
  let nextSong = null;

  if (pressed2) {
    nextVisual = bedVid;
    nextSong   = bedSong;
  } else if (pressed1) {
    nextVisual = ballVid;
    nextSong   = ballSong;
  } else if (mood === "happy") {
    nextVisual = happyVid;
    nextSong   = bedSong;
  } else if (mood === "sad") {
    nextVisual = sadVid;
    nextSong   = ballSong;
  } // neutral → nothing

  updateVisual(nextVisual);
  updateSong(nextSong);

  // draw visual video (full background)
  if (currentVisual) {
    push();
    imageMode(CENTER);
    image(currentVisual, width / 2, height / 2, width, height);
    pop(); // restores imageMode to default (CORNER)
  }

  // draw webcam mirror in center
  drawMirror();
  // debug text
  fill(255);
  textSize(14);
  textAlign(LEFT, TOP);
  text("mood: " + mood, 10, 10);
  text("FSR1: " + fsrValue1, 10, 30);
  text("FSR2: " + fsrValue2, 10, 50);
}

// ----------------- MIRROR (rectangle) -----------------
function drawMirror() {
  let w = 320, h = 240;
  let x = width / 2 - w / 2;
  let y = height / 2 - h / 2;

  // frame
  fill(255, 255, 255, 40);
  stroke(200);
  strokeWeight(6);
  rect(x - 8, y - 8, w + 16, h + 16, 20);

  // video (mirrored)
  push();
  translate(x + w, y);
  scale(-1, 1);
  image(video, 0, 0, w, h);
  pop();
}

// ----------------- MOOD DETECTION -----------------
function detectMood() {
  if (faces.length > 0 && faces[0].lips && faces[0].lips.keypoints.length > 0) {
    let lips = faces[0].lips;
    let pts = lips.keypoints;

    let left = pts[0];
    let right = pts[0];

    for (let p of pts) {
      if (p.x < left.x) left = p;
      if (p.x > right.x) right = p;
    }

    let w = right.x - left.x;
    if (w === 0) {
      mood = "neutral";
      return;
    }

    let sumDiff = 0;
    let count = 0;

    for (let p of pts) {
      let t = (p.x - left.x) / w;
      if (t < 0 || t > 1) continue;

      let baseY = left.y + t * (right.y - left.y);
      let diff = baseY - p.y;
      sumDiff += diff;
      count++;
    }

    if (count === 0) {
      mood = "neutral";
      return;
    }

    let avgDiff = sumDiff / count;
    let thresholdLips = lips.height * 0.12;

    if (avgDiff < -thresholdLips) {
      mood = "happy";
    } else if (avgDiff > thresholdLips * 0.4) {
      mood = "sad";
    } else {
      mood = "neutral";
    }
  }
}

// ----------------- VISUAL / SONG HELPERS -----------------
function updateVisual(next) {
  if (next !== currentVisual) {
    [bedVid, ballVid, happyVid, sadVid].forEach(v => {
      if (v) v.stop();
    });
    currentVisual = next;
    if (currentVisual) {
      currentVisual.loop();
    }
  }
}

function updateSong(next) {
  if (next !== currentSong) {
    if (bedSong) bedSong.stop();
    if (ballSong) ballSong.stop();
    currentSong = next;
    if (currentSong) {
      currentSong.loop();
    }
  }
}

// ----------------- FACE MESH CALLBACK -----------------
function gotFaces(results) {
  faces = results;
}

// ----------------- SERIAL DATA -----------------
function onSerialData() {
  let line = serial.readStringUntil("\n");
  if (!line) return;

  line = line.trim();
  if (line.length === 0) return;

  let parts = line.split(",");
  if (parts.length === 2) {
    fsrValue1 = int(parts[0]);
    fsrValue2 = int(parts[1]);
  }
}

// ----------------- SERIAL SETUP -----------------
function setupSerial() {
  serial.on("noport", makePortButton);
  serial.on("portavailable", openPort);
  serial.on("requesterror", portError);
  serial.on("data", onSerialData);
  serial.on("close", makePortButton);

  navigator.serial.addEventListener("connect", portConnect);
  navigator.serial.addEventListener("disconnect", portDisconnect);

  serial.getPorts();
}

function makePortButton() {
  if (!portButton) {
    portButton = createButton("choose port");
    portButton.position(10, 10);
    portButton.mousePressed(choosePort);
  }
  portButton.show();
}

function choosePort() {
  serial.requestPort();
}

function openPort() {
  serial.open({ baudRate: 9600 }).then(() => {
    console.log("port open");
    if (portButton) portButton.hide();
  });
}

function portError(err) {
  console.error("Serial port error:", err);
}

function portConnect() {
  console.log("port connected");
  serial.getPorts();
}

function portDisconnect() {
  console.log("port disconnected");
  if (portButton) portButton.show();
}

