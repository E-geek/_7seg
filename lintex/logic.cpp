// lvl0 0 - часы, 1 - буд1, 2 - буд2
// lvl1 0 - none, 1-... - value
int8_t lvl0, lvl1;

bool shift, change;

void save() {
  lvl0 = lvl1 = 0;
}

void onNext(Event*) {
  clog("next press");
  if(lvl1 > 0) {
    if(lvl0 == 0) {
      if(++lvl1 == 4) {
        save();
        return;
      }
      if(lvl1 == 2) {
        flash = B1100;
      } else if (lvl1 == 3) {
        flash = B0011;    
      } else {
        uint8_t d = read_i2c_register(0x03);
        print(weekDay0[d], weekDay1[d], 11, 11);
        flash = B1100;
      }
      return;
    }
  }
  if(++lvl0 == 3) {
    lvl0 = 0;
    return;
  }
  actualAlarmTime(lvl0 == 1);
};

void onEnter(Event* e) {
  clog("enter press");
  if(lvl1++ == 0) {
    onNext(e);
    return;
  }
};

void onChangeDown() {
  clog("change down");
  change = true;
}

void onChangePress() {
  clog("change press");
  change = false;
}

void onShiftDown(Event*) {
  clog("shift down");
  if ((lvl0 | lvl1) == 0) {
    if (change) {
      printRTCState();
      return;
    }
    // print day week and temp
    lvl1 = -1;
    uint8_t d = read_i2c_register(0x03);
    uint8_t t = read_i2c_register(0x11);
    print(weekDay0[d], -weekDay1[d], leftDec(t), rightDec(t));
    return;
  }
};

void onShiftPress(Event*) {
  clog("shift press");
  if (lvl0 == 0 && lvl1 == -1) {
    lvl1 = 0;
    return;
  }
}