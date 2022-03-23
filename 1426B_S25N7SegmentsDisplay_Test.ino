/**********************************************************/
/* Example of using a hardware timer on                   */
/*   an STM32 microcontroller.                            */
/**********************************************************/
#include  <HardwareTimer.h>
#define  TXD1 PA9
#define  RXD1 PA10
HardwareSerial Serial1( RXD1, TXD1 ); 
uint8_t numCode[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x4F};
//uint8_t numCode[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
uint8_t number[4];
uint8_t row[8]={PD0,PD1,PD2,PD3,PD4,PD5,PD6,PD7};
uint8_t col[3]={PC10,PC11,PC12};
int pos;

HardwareTimer timer( TIM2 );

void datawrite(int data);
void led6(int data);
void led789(int data);
void column(int num);
void updateSystemTimer();

void setup()
{
  for(int i=0;i<=7;i++){
    pinMode(row[i],OUTPUT);
  }
  for(int i1=0;i1<=2;i1++){
    pinMode(col[i1],OUTPUT);
  }
  
  Serial1.begin( 115200UL );
  while( !Serial1 ) { delay( 20 ); }
  Serial1.println( "STM32 Hardware Timer Demo." );
  delay( 100UL );

  // timer initialize.
  timer.pause();
  timer.setPrescaleFactor( 90UL );
    /* 1Mhz timer clock. F103=72,F401=84,F446=90,L053=32 */
  timer.setOverflow( 1 * 1000000UL );  /* 1ms interrupt */
  timer.attachInterrupt( updateSystemTimer );
  timer.refresh();
  timer.resume();
}
void loop() 
{
  static int count = 0;
  static uint32_t baseTim = millis();
  if( (millis() - baseTim) >= 1000UL )
  {
    baseTim = millis();
    datawrite( count );
    if( ++count >= 2000 ) count = 0;
    Serial1.print("count = ");
    Serial1.println(count);
  }
}

void datawrite(int data){
  number[0]=data/1000;
  number[1]=(data%1000)/100;
  number[2]=(data%100)/10;
  number[3]=((data%1000)%100)%10;
}

void led6(int data){
  if(data==1){
    digitalWrite(row[0],HIGH);
    digitalWrite(row[1],HIGH);
  }
  else{
    digitalWrite(row[0],LOW);
    digitalWrite(row[1],LOW);
  }
}

void led789(int data){
  for(int i=0;i<=7;i++){
    int e = pow(2,i);
    digitalWrite(row[i],(numCode[data]&e)>>i);
  }
}

void column(int num){
  for(int i=0;i<=2;i++){
    int e = pow(2,i);
    digitalWrite(col[i],(num&e)>>i);
  }
}

void updateSystemTimer(){
  uint8_t val=number[pos];
  if(pos==0) {led6(val);}
  else if(pos>=1&&pos<=3) {led789(val);}
  column(pos);
  if( ++pos > (int)(sizeof(number) / sizeof(number[0])) ) pos = 0;
}
