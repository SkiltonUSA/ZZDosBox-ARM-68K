/* GPL-3.0-or-later. Built-in IPXNET shell command, using the MUI endpoint. */
static void ipx_print(const char*s){Bit16u n=strlen(s);DOS_WriteFile(STDOUT,(Bit8u*)s,&n);}
bool db_ipx_command(char*line){
 while(*line==' '||*line=='\t')++line;
 if(strncasecmp(line,"IPXNET",6)|| (line[6]&&line[6]!=' '&&line[6]!='\t'))return false;
 line+=6;while(*line==' '||*line=='\t')++line;
 dos.return_code=0;
 if(!db_ipx_enabled()){ipx_print("IPX is disabled. Select IPX wrapper in the launcher.\r\n");dos.return_code=1;return true;}
 if(!strcasecmp(line,"CONNECT")){
  if(incomingPacket.connected){ipx_print("IPX already connected.\r\n");return true;}
  if(ConnectToServer("configured")){db_ipx_connected(1);ipx_print("IPX connected to configured server.\r\n");}
  else{ipx_print("IPX connection failed. Check server address and UDP port.\r\n");dos.return_code=1;}
 }else if(!strcasecmp(line,"DISCONNECT")){DisconnectFromServer(false);ipx_print("IPX disconnected.\r\n");}
 else if(!strcasecmp(line,"STATUS")){ipx_print(incomingPacket.connected?"IPX client: connected (UDP tunnel).\r\n":"IPX client: disconnected.\r\n");}
 else if(!strcasecmp(line,"PING")){
  if(!incomingPacket.connected){ipx_print("IPX is not connected.\r\n");dos.return_code=1;return true;}
  TIMER_DelTickHandler(&IPX_ClientLoop);pingSend();Bit32u start=PIC_Ticks;unsigned count=0;IPXHeader response;
  while(PIC_Ticks-start<1500){CALLBACK_Idle();if(pingCheck(&response)&&SDLNet_Read16(response.dest.socket)==2){count++;ipx_print("IPX peer replied.\r\n");}}
  TIMER_AddTickHandler(&IPX_ClientLoop);if(!count)ipx_print("No IPX peers replied (the relay itself need not reply).\r\n");
 }else{ipx_print("IPXNET CONNECT | DISCONNECT | STATUS | PING\r\nServer address/port: set in MUI. Relay hosting is not included.\r\n");dos.return_code=1;}
 return true;
}
