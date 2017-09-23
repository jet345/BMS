
#include "/usr/include/mysql/mysql.h"
#include "/home/pi/cooking/libraries/arduPiLoRa/arduPiLoRa.h"
#include "function.h"
#include "smart.h"
#include <math.h>
#include <time.h>

void rx_loop();
void tx_loop(int lora_addr);
void db_setup();
float THI(int temp, int hum);

int e;
char my_packet[100];
char msg[10];

fd_set status;
MESSAGE m_message;
char m_userName[20];

int sock;
struct sockaddr_in echoServAddr;
unsigned short echoServPort; 
char *servIP = "192.168.0.14"; 
char *echoString;
char buffer[100];

time_t stTempTime;
int iRet;

/*mysql variable*/
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
char query[100];
int query_stat;
char *first_floor_table;
char *second_floor_table;
/****************/

/*time variable*/
time_t timer;
struct tm *t;
int time_date[5];
/****************/

/*aircon control variable*/
float THI_val;
int sensor[4];

void db_setup(){
    conn = mysql_init(NULL);

    if(!conn) {
            printf("error");
            exit(0);
    }

    conn= mysql_real_connect(conn,"35.185.138.8","root","rudal45","buildingmanager",0,NULL,0);
    if(conn) {
            printf("DB CONNECT!\n");
    }
    //mysql_close(conn_ptr);
}

void tcp_setup(){
  sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
  if( sock < 0 )
  {
    printf( "Socket Function Failed!\n" );
    exit(1);
  }

  memset( &echoServAddr, 0, sizeof( echoServAddr ) ); 
  echoServAddr.sin_family = AF_INET; 
  echoServAddr.sin_addr.s_addr = inet_addr( servIP );
  echoServAddr.sin_port = htons( 5000 );

  iRet = connect( sock, ( struct sockaddr* )&echoServAddr, sizeof( echoServAddr ) );
    
  if( iRet < 0 )  
  {
    printf( "Connect Function Failed!\n" );
    //close(sock);
    exit(1);
  }
  else {
    printf("Connect Success\n\n");
  }

}

void setup(){  // Print a start message
  printf("SX1272 module and Raspberry Pi: receive packets without ACK\n");  
  // Power ON the module
  e = sx1272.ON();
  printf("Setting power ON: state %d\n", e);  
  // Set transmission mode
  e |= sx1272.setMode(4);
  printf("Setting Mode: state %d\n", e);  
  // Set header
  e |= sx1272.setHeaderON();
  printf("Setting Header ON: state %d\n", e);  
  // Select frequency channel
  e |= sx1272.setChannel(CH_08_900);
  printf("Setting Channel: state %d\n", e);  
  // Set CRC
  e |= sx1272.setCRC_ON();
  printf("Setting CRC ON: state %d\n", e);  
  // Select output power (Max, High or Low)
  e |= sx1272.setPower('H');
  printf("Setting Power: state %d\n", e);  
  // Set the node address
  e |= sx1272.setNodeAddress(8);
  printf("Setting Node address: state %d\n", e);
  
  // Print a success message
  if (e == 0){
    printf("SX1272 successfully configured\n");
    tcp_setup();
    db_setup();
  }
  else
    printf("SX1272 initialization failed\n");
  
  delay(1000);
  
}

float THI(int temp, int hum){
    float THI_val;
    THI_val = 1.8*temp-((0.55*(1-hum*0.01)*(1.8*temp-26)))+32;

    return THI_val;
}

void rx_loop() {
  
  int lora_id;
  //int sensor[4];
  float sensor_f;
  int count=-1;
  printf("rx_loop start\n");
  e = sx1272.receivePacketTimeout(10000);
  if ( e == 0 )
  {
    timer = time(NULL); // 현재 시각을 초 단위로 얻기
    t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기
    time_date[0]=t->tm_year+1900;
    time_date[1]=t->tm_mon+1;
    time_date[2]=t->tm_mday;
    time_date[3]=t->tm_hour;
    time_date[4]=t->tm_min;

    //printf("Receive TIME : %d %d %d %d %d",time_date[0],time_date[1],time_date[2],time_date[3],time_date[4]);

    printf("Receive packet, state %d\n",e);
    for (unsigned int i = 0; i <= sx1272.packet_received.length; i++)
    {
      my_packet[i] = (char)sx1272.packet_received.data[i];
    }
    // Seperate message array
    char *test = strtok(my_packet, "/");

    while(test != NULL) {
      printf("%s ", test);
      if(count==-1){ //맨처음은 무조건 lora id
        lora_id = atoi(test);
        printf("lora_id : %d\n",lora_id);
        count++;
      }
      else{//count가 0~3까지
        if(lora_id==22)
        {
          sensor_f = atof(test);
        }
        sensor[count] = atoi(test);
        //printf("sensor[%d]:%d\n",count,sensor[count]);
        count++;
      }
      test = strtok(NULL, "/");
    }
    if(lora_id == 11){ //1층에서 온 메세지라면 sensor table에 저장
        //맨마지막 카운터센서 값까지 db에 저장한다.
        sprintf(query,"insert into sensor values('%d','%d','%d','%d','%d','%d','%d','%d','%d')",time_date[0],time_date[1],time_date[2],time_date[3],time_date[4],sensor[2],sensor[1],sensor[0],sensor[3]);
        /*m_message.m_temp = sensor[2];
        m_message.m_hum = sensor[1];
        unistd::write( sock, &m_message, sizeof(m_message) );*/ //aircon_judge를 위해 짯던 부분.       
        query_stat = mysql_query(conn,query);
        if(query_stat){
            printf("MYSQL INSERT &SENSOR& ERROR");
        }
        //THI_val를 전역으로 선언하고, 희망온도와 비교할 수 있도록 main의 tcp수신파트로 보낸다.
        THI_val = THI(sensor[2],sensor[1]);
    }
    else if(lora_id == 22){
    	//전력 테이블에 5분단위로 저장하고, 전력량 테이블을 업데이트 한다. (5분이니까 1/12시간, 전력*시간=전력량(현재는 Wh임))
        float elec5 = sensor_f/12;
        elec5 = roundf(elec5 * 100) / 100;

		sprintf(query,"INSERT INTO electricity VALUES ('%d','%d','%f') ON DUPLICATE KEY UPDATE amount=amount+'%f'",time_date[0],time_date[1], elec5, elec5);

        query_stat = mysql_query(conn,query);
        if(query_stat){
            printf("MYSQL INSERT &ELECTRICITY& ERROR");
        }

		sprintf(query,"insert into power values('%d','%d','%d','%d','%d','%f')",time_date[0],time_date[1],time_date[2],time_date[3],time_date[4],sensor_f);
        query_stat = mysql_query(conn,query);
        if(query_stat){
            printf("MYSQL INSERT &POWER& ERROR");
        }
    }
	
    else if(lora_id == 33){//2층에서 온 메세지라면 sensor table에 저장
        sprintf(query,"insert into sensor2 values ('%d','%d','%d','%d','%d','%d','%d','%d')",time_date[0],time_date[1],time_date[2],time_date[3],time_date[4],sensor[2],sensor[1],sensor[0]);
        query_stat = mysql_query(conn,query);
        if(query_stat){
            printf("MYSQL INSERT $SENSOR2$ ERROR");
        }
    }

    //printf("Message: %s\n", my_packet);
    printf("%d\n",sx1272.packet_received.src);
  }
  else {
    printf("Receive packet, state %d\n",e);
  }

}

void tx_loop(int lora_addr){
 
  printf("tx_loop start\n");
  //delay(2000);
  e = sx1272.sendPacketTimeout(lora_addr, msg);
  printf("Packet sent, state %d\n",e);
  //delay(4000);

}


int main (){
 
  int index;
  int data;
  int i,rc;
  int result[2];
  int ascii;
  int ascii_2;
  setup();

  while(1){

    FD_ZERO( &status );
    FD_SET( 0, &status );
    FD_SET( sock, &status );
    iRet = select( sock+1, &status, 0, 0, 0 );

    //server.cpp에서 tcp로 제어명령이 온 경우 처리 
    if(1 == FD_ISSET( sock, &status ) ){
      printf("RECEIVE\n");
      unistd::read(sock, &m_message, sizeof(m_message));
      strcpy(buffer,m_message.m_buffer);
      ascii = buffer[0];
      printf("%d",ascii);
      //a,b,c,d 인 경우는 1층 스위치제어
      //아스키코드범위 97~100
      if(ascii>=97 && ascii <=100){
        printf("1st floor switch\n");
        switch(ascii){
            case 97:
            //a
            strcpy(msg,"a ");
            tx_loop(11);
            printf("a");
            break;
            case 98:
            //b
            strcpy(msg,"b ");
            tx_loop(11);
            printf("b");
            break;

            case 99:
            //c
            strcpy(msg,"c ");
            tx_loop(11);
            printf("c");
            break;

            case 100:
            //d
            strcpy(msg,"d ");
            tx_loop(11);
            printf("d");
            break;
        }
        putc('\n',stdout);

      }

      //e,f,g,h 인 경우는 2층 스위치제어
      //아스키코드범위 101~104
      if(ascii>=101 && ascii<=104){
        printf("2st floor switch\n");
        switch(ascii){
            case 101:
            //e
            strcpy(msg,"e ");
            tx_loop(33);
            printf("e");
            break;
            case 102:
            //f
            strcpy(msg,"f ");
            tx_loop(33);
            printf("f");
            break;

            case 103:
            //g
            strcpy(msg,"g ");
            tx_loop(33);
            printf("g");
            break;

            case 104:
            //h
            strcpy(msg,"h ");
            tx_loop(33);
            printf("h");
            break;
        }
        putc('\n',stdout);
      }

      //얼굴인식 완료되었을때
      //출입가능 : o(소문자)
      if(ascii == 111){
        strcpy(msg,"o ");
        tx_loop(22);
		    printf("o");
      }

      //에어컨 제어 (희망온도를 보낸다.)
      //On :11
      //Off :10
      //강 : 12
      //약 : 13
      //온도 20~27
      if(ascii>=112 && ascii<=121){
        switch(ascii){
            case 112: //off
              strcpy(msg,"11 ");
              tx_loop(11);
              
            break;

            case 113://20
              if(sensor[2] < 20 || THI_val > 68){
                strcpy(msg,"20 ");
                tx_loop(11);
              }
            break;

            case 114:
              if(sensor[2] < 21 || THI_val > 68){
                strcpy(msg,"21 ");
                tx_loop(11);
              }

            break;

            case 115:
                if(sensor[2] < 22 || THI_val > 68){
                  strcpy(msg,"22 ");
                  tx_loop(11);
                }
            break;
            case 116:
                if(sensor[2] < 23 || THI_val > 68){
                  strcpy(msg,"23 ");
                  tx_loop(11);
                }
            break;
            case 117:
                if(sensor[2] < 24 || THI_val > 68){
                  strcpy(msg,"24 ");
                  tx_loop(11);
                }
            break;

            case 119:
                if(sensor[2] < 25 || THI_val > 68){
                  strcpy(msg,"25 ");
                  tx_loop(11);
                }
            break;

            case 120:
                if(sensor[2] < 26 || THI_val > 68){
                  strcpy(msg,"26 ");
                  tx_loop(11);
                }
            break;
            case 121:
                if(sensor[2] < 27 || THI_val > 68){
                  strcpy(msg,"27 ");
                  tx_loop(11);
                }
            break;
        }
      }

      
    }

    else if(1 != FD_ISSET(sock, &status)){
      rx_loop();
      putc('\n',stdout);
    }
  }
  
  return (0);
}
