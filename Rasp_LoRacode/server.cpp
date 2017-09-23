#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include "function.h"

int main(){
  MESSAGE m_message;
  fd_set status;
  
  int servSock;  // 서버 소켓번호는 정수형으로 이루어져 있다.
  int clntSock[3];  // 클라이언트 소켓
  int tempSock;

  int iMaxSock;  // 
  
  unsigned int uiUser;

  int iCount;
  int i;
  
  struct sockaddr_in echoServAddr;  //  서버 소켓 구조체(신버전)
  struct sockaddr_in echoClntAddr;  //  클라이언트 주소...
  
  unsigned short echoServPort;  // 서버 포트.... 9999...
  unsigned int clntLen;  // 

  time_t stTempTime;
  
  int iRet;  // 잡동사니를 넣어두는 변수.

  unsigned char ucBuffer[500];  // 클라이이언트로부터 메시지를 저장하는 버퍼.
  int tmp_hum;
  int tmp_temp;

  echoServPort = 5000;  // 사용할 포트 번호.

  servSock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );  // Socket을 생성한다.
  if( servSock < 0 )  // 위의 구문에서 에러가 발생했다면
  {
    printf( "Socket Function Error!\n" );
    return( 0 );
  }
  memset( &echoServAddr, 0, sizeof( echoServAddr ) );  //해당 메모리를 전부 0으로 초기화.
  echoServAddr.sin_family = PF_INET;  // Internet Address Family
  echoServAddr.sin_addr.s_addr = htonl( INADDR_ANY ); 
  echoServAddr.sin_port = htons( echoServPort );

  iRet = bind( servSock, ( struct sockaddr * )&echoServAddr, sizeof( echoServAddr ) );
  
  if( iRet < 0 )  // 위의 구문에서 에러가 발생했다면
  {
    close( servSock );  // 소켓을 연 이후 에러가 났으므로 닫아주도록 한다.
    printf( "Bind Failed Error!\n" );
    return( 0 );
  }

  iRet = listen( servSock, 3 ); // MAX PENDING : 3
  if( iRet < 0 )  // 위의 구문에서 에러가 발생했다면...
  {  
    close( servSock );  // 소켓을 연 이후 에러가 났으므로 닫아주도록 한다.
    printf( "Listen  Failed Error!\n" );
    return( 0 );
  }
  clntLen = sizeof( echoClntAddr );    
  iMaxSock = servSock+1;  
  uiUser = 0;

  while(1){
    FD_ZERO( &status );  // 변수 초기화(0으로)
    FD_SET( 0, &status );  // 0번째 값을 1로 바꾼다(stdin) -> 키보드 감지.
    FD_SET( servSock, &status );  // 랑데부 소켓과 키보드만 감지한다.

    for( i=0; i<uiUser; i++ )
    {
      FD_SET( clntSock[i], &status );
      
      if( iMaxSock <= clntSock[i] )
      {
        iMaxSock = clntSock[i]+1;
      }
      
    }
    
    iRet = select( iMaxSock, &status, 0, 0, 0 );
    if( iRet < 0 )
    {
      printf( "Select Function Error!\n" );
      break;
    }
    
    if( 1 == FD_ISSET( servSock, &status ) )  // 랑데부. 새로운 클라이언트가 접속했을 때.
    {
      tempSock = accept( servSock, ( struct sockaddr * )&echoClntAddr, &clntLen );
      // 들어오는 사람의 IP 및 정보가 요구되므로 2번째 인자가 클라이언트 정보.
      // accept는 구 자료형 형식의 함수이기 때문에 강제 캐스팅
      // clntLen : 크기.
      // accept에서는 접속이 될 때 까지 그대로 대기상태가 된다. 블로킹 함수.
      // servSock : 랑데부 소켓(외부에서 받아들이기만 한다)
      // clntSock : 커뮤니케이션 소켓. 내용을 처리할 때 쓰인다.
  
      printf( "Socket Number : %d\n", tempSock );
      
      if( tempSock < 0 )
      {
        printf( "Accept Function Error!\n" );
        continue;
      }
    
      printf( "클라이언트 접속 IP : %s\n", inet_ntoa( echoClntAddr.sin_addr ) );
      // Network to ASCII Code(수치로만 되어있는 IP주소를 사용자가 알아볼 수 있게 문자열로 바꾸어준다.)
  
      printf( "클라이언트 접속 PORT : %d\n", ntohs( echoClntAddr.sin_port ) );
      // Big-Endian으로 저장된 클라이언트의 포트 정보를 서버에 맞게 Little-Endian으로 변환하여 출력한다.

      if( 3 <= uiUser )  // 사람이 다 찼는데 또 들어올 경우.
      {
        close( tempSock );
        continue;  // 와일 문 정상화.
      }

      clntSock[uiUser] = tempSock;  // tempSock으로 받은 커뮤니케이션 소켓을 넣어준다.
      uiUser = uiUser + 1;  // 접속 인원 + 1

      printf( "현재 접속자 수는 총 %d명 입니다.\n", uiUser );
      
    }
    //서버->클라이언트, 서버의 키보드 입력이있을때
    //궂이 필요한 부분은 아님
    else if(1 == FD_ISSET(0, &status)){
      iRet = read(0, m_message.m_buffer, sizeof(m_message.m_buffer));
      //strcpy(For_Server_Problem, m_message.m_buffer); 
      m_message.m_buffer[iRet - 1] = 0;
      //strcpy(m_message.m_userName, "PROBLEM");

      for (iCount = 0; iCount<uiUser; iCount++) 
      {
        write(clntSock[iCount], &m_message, sizeof(m_message));       

      }
    }

    //클라이언트로부터 받은 메세지가 있을 때
    //node js 웹서버로부터 받은 메세지가 있을때 처리될 부분
    //웹에서 전달받은 메세지를 라즈베리파이로 바로 전송한다.
    //node js 웹서버가 항상 첫번째 client가 될것이기 때문에 clntSock[0]는 nodejs
    //두번째 client는 라즈베리파이 clntSock[1].
    //세번째 client는 구글서버에 돌아갈 에어컨 판단코드 client[2].

    else{
       if (1 == FD_ISSET(clntSock[0], &status)){
          iRet = read(clntSock[0], &m_message, sizeof(ucBuffer) - 1);

          //m_message안의 변수를 읽어서 웹서버가 요청한 동작에대한 내용을 라즈베리파이로 전송
          //웹서버가 요청하는 동작 -> led밝기조절, 에어컨 희망온도
          if (iRet != 0){
            printf("receive message");
            write(clntSock[1], &m_message, sizeof(m_message));
            //write(clntSock[2], &m_message, sizeof(m_message));            
          }
        }

       //3_km_rx_lora.cpp 파일에서 로라로 수신한 온도,습도 값을 aircon_judge.js로 보낼것임
       /*if (1 == FD_ISSET(clntSock[1], &status)){
          iRet = read(clntSock[1], &m_message, sizeof(ucBuffer) - 1);
        
          if (iRet != 0){
            tmp_temp = m_message.m_temp;
            tmp_hum = m_message.m_hum;
            
            write(clntSock[2], &m_message, sizeof(m_message));           
            
          }
        }

        //aircon_judge.js(clntSock[2])가 불쾌지수가 68보다 낮다고 판단했을때, 3_km_rx_lora.cpp(clntSock[1])파일로 에어컨을 1도낮추라고 명령
        if (1 == FD_ISSET(clntSock[2], &status)){
            iRet = read(clntSock[2], &m_message, sizeof(ucBuffer)-1);

            if(iRet != 0){
                write(clntSock[1], &m_message, sizeof(m_message));
            }

        }*/
    }
   

  

  }
}