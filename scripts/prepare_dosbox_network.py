#!/usr/bin/env python3
"""Stage hash-pinned libslirp with a narrow Amiga portability patch."""
import hashlib,json,tarfile,urllib.request
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SRC=ROOT/'amiga/dosbox/network'
OUT=ROOT/'.context/amiga/dosbox-network'
def prepare(destination=None):
    pin=json.loads((SRC/'upstream.json').read_text())['libslirp']
    OUT.mkdir(parents=True,exist_ok=True)
    archive=OUT/'libslirp.tar.bz2'
    if not archive.exists():
        archive.write_bytes(urllib.request.urlopen(pin['url'],timeout=45).read())
    if hashlib.sha256(archive.read_bytes()).hexdigest()!=pin['sha256']:
        raise ValueError('libslirp archive hash mismatch')
    dest=destination or OUT/'slirp';dest.mkdir(parents=True,exist_ok=True)
    with tarfile.open(archive) as tar:
        for m in tar.getmembers():
            parts=Path(m.name).parts
            if not m.isfile() or len(parts)<2:continue
            rel=Path(*parts[1:])
            if '..' in rel.parts or rel.is_absolute():raise ValueError('unsafe archive path')
            if rel.parts[0]!='src' and rel.name not in ('COPYRIGHT','COPYING','LICENSE'):continue
            p=dest/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(tar.extractfile(m).read())
    inc=dest/'src'
    p=inc/'libslirp-version.h.in';s=p.read_text()
    for k,v in {'MAJOR_VERSION':'4','MINOR_VERSION':'9','MICRO_VERSION':'5','VERSION_STRING':'"4.9.5-zz-amiga"'}.items():s=s.replace('@SLIRP_'+k+'@',v)
    (inc/'libslirp-version.h').write_text(s)
    # Fragment queue overlays can sit at four-byte offsets on 64-bit hosts.
    # Preserve their size while making accesses explicitly unaligned-safe.
    for name,struct in [('ip.h','qlink'),('misc.h','slirp_quehead')]:
        p=inc/name;s=p.read_text();a=s.index('struct '+struct+' {');b=s.index('};',a)
        s=s[:b]+'} SLIRP_PACKED_END;'+s[b+2:];p.write_text(s)
    p=inc/'ip.h';s=p.read_text();a=s.index('struct ipovly {');b=s.index('} SLIRP_PACKED_END;',a)+len('} SLIRP_PACKED_END;')
    s=s[:b]+'''\nG_STATIC_ASSERT(sizeof(struct mbuf_ptr)==8);
G_STATIC_ASSERT(sizeof(struct ipovly)==20);
G_STATIC_ASSERT(offsetof(struct ipovly,ih_pr)==9);
G_STATIC_ASSERT(offsetof(struct ipovly,ih_src)==12);
'''+s[b:];p.write_text(s)
    # Never allow guest-forwarding to spawn host commands, on either test host.
    p=inc/'misc.c';s=p.read_text();a=s.index('static int slirp_socketpair_with_oob');b=s.index('int open_unix',a)
    s=s[:a]+'int fork_exec(struct socket *so,const char *ex) { (void)so;(void)ex;return 0; }\n\n'+s[b:];p.write_text(s)
    p=inc/'util.h';s=p.read_text().replace('#include "libslirp.h"','#include "libslirp.h"\n#ifdef __amigaos__\n#include "amiga_socket.h"\n#endif')
    s=s.replace('#include <netinet/tcp.h>', '#ifndef __amigaos__\n#include <netinet/tcp.h>\n#endif')
    s=s.replace('#define closesocket(s) close(s)\n#define ioctlsocket(s, r, v) ioctl(s, r, v)', '#ifdef __amigaos__\n#define closesocket(s) CloseSocket(s)\n#define ioctlsocket(s,r,v) IoctlSocket(s,r,v)\n#else\n#define closesocket(s) close(s)\n#define ioctlsocket(s,r,v) ioctl(s,r,v)\n#endif');p.write_text(s)
    p=inc/'util.c';s=p.read_text().replace('void slirp_set_nonblock(slirp_os_socket fd)\n{','void slirp_set_nonblock(slirp_os_socket fd)\n{\n#ifdef __amigaos__\n unsigned long opt=1; IoctlSocket(fd,FIONBIO,(char*)&opt);\n#else')
    # Close the conditional inside the function, before its final brace.
    end=s.index('\nstatic void slirp_set_cloexec');prev=s.rfind('\n}',0,end);s=s[:prev]+'\n#endif'+s[prev:]
    a=s.index('static void slirp_set_cloexec');b=s.index('\nslirp_os_socket slirp_socket',a)
    old=s[a:b];s=s[:a]+'#ifdef __amigaos__\nstatic void slirp_set_cloexec(slirp_os_socket fd) { (void)fd; }\n#else\n'+old+'#endif\n'+s[b:];p.write_text(s)
    p=inc/'slirp.c';s=p.read_text();a=s.index('#if defined(_WIN32)',s.index('#define TIMEOUT_DEFAULT'));b=s.index('\nstatic void slirp_init_once',a)
    old=s[a:b];s=s[:a]+'''#ifdef __amigaos__
extern struct in_addr zz_net_dns;
int get_dns_addr(struct in_addr *a,uint16_t *p) { if(!zz_net_dns.s_addr)return -1;*a=zz_net_dns;*p=htons(53);return 0; }
int get_dns6_addr(struct in6_addr *a,uint16_t *p,uint32_t *scope) { (void)a;(void)p;(void)scope;return -1; }
#else
'''+old+'#endif\n'+s[b:]
    s=s.replace('#include <net/if.h>', '#include <net/if.h>\n#ifdef __amigaos__\n#undef if_mtu\n#endif')
    # Upstream reserves ~64 KiB on every Ethernet send for maximum-size IP.
    # This single-session backend fixes IP MTU at 1500 and caps frames at 1518.
    # Keep upstream's length guard; never consume the Amiga's whole 64 KiB stack.
    if s.count('uint8_t buf[IF_MTU_MAX + 100];')!=1:
        raise ValueError('libslirp Ethernet stack buffer changed upstream')
    s=s.replace('#include "slirp.h"','#include "slirp.h"\n#include "backend.h"')
    s=s.replace('uint8_t buf[IF_MTU_MAX + 100];','uint8_t buf[ZZ_NET_MTU + 2];')
    p.write_text(s)
    return inc

def prepare_ne2000(output):
    """Adapt only the pinned NE2000 device, keeping DOSBox-X/Bochs notices."""
    pin=json.loads((SRC/'upstream.json').read_text())['ne2000']
    OUT.mkdir(parents=True,exist_ok=True)
    for source in ['src/hardware/ne2000.cpp','include/ne2000.h']:
        cache=OUT/('upstream-'+Path(source).name)
        if not cache.exists():
            url='https://raw.githubusercontent.com/joncampbell123/dosbox-x/'+pin['revision']+'/'+source
            cache.write_bytes(urllib.request.urlopen(url,timeout=45).read())
        if hashlib.sha256(cache.read_bytes()).hexdigest()!=pin['sha256'][source]:
            raise ValueError('NE2000 upstream hash mismatch: '+source)
        s=cache.read_text()
        if source.endswith('.cpp'):
            s='#include <stdarg.h>\n'+s
            s=s.replace('# if C_IPX','# if 0 /* Separate DOSBox IPX driver, no Ethernet redirection. */')
            s=s.replace('LOG(LOG_MISC,LOG_DEBUG)', 'LOG(LOG_MISC,LOG_NORMAL)')
            s=s.replace('#include "hardware.h"','').replace('#include "ethernet.h"','#include "ethernet_arm.h"')
            s='\n'.join(line for line in s.splitlines() if 'CaptureState & CAPTURE_NETWORK' not in line)+'\n'
            a=s.index('\t\tSection_prop * section=');b=s.index('\t\t// create the bochs NIC class',a)
            s=s[:a]+'''        load_success=true;
        if(!db_net_enabled()){load_success=false;addne2k=false;return;}
        ethernet=new EthernetConnection();
        const Bitu irq=3,base=0x300;
        const uint8_t mac[6]={2,0x5a,0x5a,0,0,1};
'''+s[b:]
            s=s.replace('\t\tif (addne2k) VFILE_Remove("NE2000.COM","SYSTEM");','')
            a=s.index('void NE2K_OnReset(')
            s=s[:a]+'''void db_net_device_open(){
 if(test)E_Exit("NE2000 already open");
 test=new NE2K(nullptr);
 if(!test->load_success){delete test;test=nullptr;}
}
void db_net_device_close(){NE2K_ShutDown(nullptr);}
'''
            s=s.replace('bx_ne2k_c::write_cr(uint32_t value)\n{','''bx_ne2k_c::write_cr(uint32_t value)
{
  if ((value & 4) && (s.tx_page_start*256u < BX_NE2K_MEMSTART ||
      s.tx_page_start*256u >= BX_NE2K_MEMEND || s.tx_bytes > 1518u ||
      s.tx_bytes > BX_NE2K_MEMEND-s.tx_page_start*256u))
    E_Exit("NE2000 TX bounds");
''')
            s=s.replace('bx_ne2k_c::rx_frame(const void *buf, unsigned io_len)\n{','''bx_ne2k_c::rx_frame(const void *buf, unsigned io_len)
{
  if(io_len<14 || io_len>1518 || s.page_start*256u<BX_NE2K_MEMSTART ||
     s.page_stop*256u>BX_NE2K_MEMEND || s.page_start>=s.page_stop ||
     s.curr_page<s.page_start || s.curr_page>=s.page_stop ||
     s.bound_ptr<s.page_start || s.bound_ptr>=s.page_stop)return;
''')
        (output/Path(source).name).write_text(s)
    return output/'ne2000.cpp'
if __name__=='__main__':print(prepare())
