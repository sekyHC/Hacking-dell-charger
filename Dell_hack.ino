#define turnOn10 PORTB|=B00000100
#define turnOff10 PORTB&=B11111011

#define output10 DDRB|=B00000100
#define input10  DDRB&=B11111011

byte CRC=0xFB;

byte data[3] = { 0x30, 0x39, 0x30};
boolean restarted=false;
unsigned long start=0;
#define restart_time 450

boolean read10(){
  byte nkj=PINB;
  nkj&=B00000100;
  if(nkj != 0){
  return 1;
  }else{
    return 0;
  }
}

void setup() {
noInterrupts();
DDRB &= B11000000; //reset port
PORTB &= B11000000; //reset port to disable all pull-ups
}

void loop() {

if(!restarted){ //waiting for restart

if(read10() == 0 && start == 0){ //pin goes low and we are not measuring yet
start=micros();
}
if(read10() != 0 && start != 0){ //pin goes high and we are waiting for that
if(micros()-start >= restart_time){ //we have a restart
  restarted=true; 
  delayMicroseconds(30);
  output10;
  turnOff10;
  delayMicroseconds(120);
  turnOn10;  
  input10;
}else{ //we dont have restat yet
start=0;
}
}
}else{ //we have passed restart state
//read crap
for(int k=0; k < 4; k++){ //read skip_rom, readmem, 2 za adresu
for(int i=0; i < 8; i++){
while(read10() != 0);
while(read10() == 0);
}
}

//write CRC
for(int i=0; i<8; i++){
if(CRC & 1<<i){ //bit is 1
     while(read10() !=0); //wait zero
     while(read10() ==0); //wait 1
     delayMicroseconds(20);     
  }else{ //bit is 0
    while(read10() !=0);
     output10;
     turnOff10;
     delayMicroseconds(20);
     turnOn10;
     input10;
   }
}
//write data
for(int i=0; i<3; i++){
  for(int j=0; j<8; j++){
  if(data[i] & 1<<j){ //bit is 1
    while(read10() !=0); //wait 0
     while(read10() ==0); //wait 1
     delayMicroseconds(20);
  }else{ //bit is 0
     while(read10() !=0);
     output10;
     turnOff10;     
     delayMicroseconds(20);
    turnOn10;
    input10;    
  }
  }
}
restarted=false; //expect next
} //else !restarted
} //loop

