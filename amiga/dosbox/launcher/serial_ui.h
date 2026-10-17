/* GPL-3.0-or-later. COM1 physical-device controls and passive USB detection. */
#include <exec/execbase.h>
extern struct ExecBase *SysBase;
enum {ID_SERIAL_CHANGE=46,ID_SERIAL_INTERNAL,ID_SERIAL_USB,ID_SERIAL_DETECT};
static Object *serial_device,*serial_unit,*serial_baud,*serial_bits,*serial_parity,*serial_stop,*serial_flow,*serial_usb,*serial_detection;
static char serial_usb_name[64];
static const char *serial_bit_names[]={"7","8",NULL},*serial_parity_names[]={"None","Odd","Even","Mark","Space",NULL},*serial_stop_names[]={"1","2",NULL},*serial_flow_names[]={"None","RTS/CTS (7-wire)",NULL};
static void serial_controls(void){
 unsigned m=val(serial_mode,MUIA_Cycle_Active);int physical=m==3,manual=val(serial_baud,MUIA_String_Integer)!=0;
 set(serial_host,MUIA_Disabled,m!=1&&m!=2);set(serial_port,MUIA_Disabled,m!=1&&m!=2);
 set(serial_device,MUIA_Disabled,!physical);set(serial_unit,MUIA_Disabled,!physical);set(serial_baud,MUIA_Disabled,!physical);set(serial_flow,MUIA_Disabled,!physical);
 set(serial_bits,MUIA_Disabled,!physical||!manual);set(serial_parity,MUIA_Disabled,!physical||!manual);set(serial_stop,MUIA_Disabled,!physical||!manual);
}
static void serial_show(const struct db_serial_config*c){
 set(serial_device,MUIA_String_Contents,(ULONG)c->device);set(serial_unit,MUIA_String_Integer,c->unit);set(serial_baud,MUIA_String_Integer,c->baud);set(serial_bits,MUIA_Cycle_Active,c->bits-7);set(serial_parity,MUIA_Cycle_Active,c->parity);set(serial_stop,MUIA_Cycle_Active,c->stop-1);set(serial_flow,MUIA_Cycle_Active,c->flow);serial_controls();
}
static int serial_store(struct db_serial_config*c){
 struct db_serial_config next;db_serial_defaults(&next);snprintf(next.device,sizeof(next.device),"%s",(char*)val(serial_device,MUIA_String_Contents));next.unit=val(serial_unit,MUIA_String_Integer);next.baud=val(serial_baud,MUIA_String_Integer);next.bits=7+val(serial_bits,MUIA_Cycle_Active);next.parity=val(serial_parity,MUIA_Cycle_Active);next.stop=1+val(serial_stop,MUIA_Cycle_Active);next.flow=val(serial_flow,MUIA_Cycle_Active);
 if(!db_serial_config_valid(&next)){note("Serial: check device, unit 0-255 and baud 0 or 50-115200.");return 0;}*c=next;return 1;
}
static void serial_detect(void){
 static const char *drivers[]={"serialpl2303.device","usbmodem.device","serialft232r.device",NULL};unsigned i;int usb;
 /* Poseidon drivers publish Exec devices dynamically. No port is opened here. */
 serial_usb_name[0]=0;Forbid();usb=FindName(&SysBase->LibList,(STRPTR)"poseidon.library")!=NULL;
 if(usb)for(i=0;drivers[i];i++)if(FindName(&SysBase->DeviceList,(STRPTR)drivers[i])){strcpy(serial_usb_name,drivers[i]);break;}Permit();
 set(serial_usb,MUIA_Disabled,!serial_usb_name[0]);
 set(serial_detection,MUIA_Text_Contents,(ULONG)(!usb?"USB: Poseidon not detected":!*serial_usb_name?"USB: Poseidon detected; no serial driver registered":serial_usb_name));
}
static Object *serial_page(void){Object *internal,*detect;Object *page=VGroup,UI_PAGE,
 Child,ColGroup(2),UI_PANEL("COM1"),
 Child,fieldlabel("Connection"),Child,serial_mode=cycle(serial_modes),
 Child,fieldlabel("Server IPv4"),Child,serial_host=str(16),
 Child,fieldlabel("TCP port"),Child,serial_port=str(6),End,
 Child,VGroup,UI_PANEL("Physical serial - Experimental"),
 Child,HGroup,Child,internal=ui_button("Built-in"),Child,serial_usb=ui_button("USB (Experimental)"),Child,detect=ui_button("Detect USB"),End,
 Child,ColGroup(2),Child,fieldlabel("Device / expansion"),Child,serial_device=str(64),
 Child,fieldlabel("Unit"),Child,serial_unit=str(4),
 Child,fieldlabel("Baud (0 = DOS controlled)"),Child,serial_baud=str(7),
 Child,fieldlabel("Data / parity / stop"),Child,HGroup,Child,serial_bits=cycle(serial_bit_names),Child,serial_parity=cycle(serial_parity_names),Child,serial_stop=cycle(serial_stop_names),End,
 Child,fieldlabel("Flow control"),Child,serial_flow=cycle(serial_flow_names),End,
 Child,serial_detection=label("USB: not checked"),End,
 Child,VGroup,UI_PANEL("Connection details"),
 Child,label("COM1: 3F8h / IRQ 4. Embedded DOS; NE2000 / IPX off."),
 Child,label("Physical mode passes modem AT commands to your device."),
 Child,label("RTS/DTR are driver-managed. Disable shared-port MIDI."),End,
 Child,VSpace(0),End;
 set(internal,MUIA_UserData,ID_SERIAL_INTERNAL);set(serial_usb,MUIA_UserData,ID_SERIAL_USB);set(detect,MUIA_UserData,ID_SERIAL_DETECT);
 set(serial_device,MUIA_ShortHelp,(ULONG)"serial.device for built-in; enter the installed IOExtSer-compatible USB/expansion driver name. Unit selects its port.");
 set(serial_baud,MUIA_ShortHelp,(ULONG)"0 follows DOS UART baud and framing (7/8 bits). A nonzero baud fixes physical framing to the controls below. Driver limits apply.");
 set(serial_usb,MUIA_ShortHelp,(ULONG)"Experimental. Enabled when Poseidon and a known serial device are registered. Detect USB refreshes after attachment. Driver presence does not prove a usable unit.");
 return page;
}
