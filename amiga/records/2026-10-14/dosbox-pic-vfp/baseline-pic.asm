<PIC_RunQueue()>:
   5d4fc: e59f317c     	ldr	r3, [pc, #0x17c]        @ 0x5d680 <PIC_RunQueue()+0x184>
   5d500: e3a00000     	mov	r0, #0
   5d504: e59f2178     	ldr	r2, [pc, #0x178]        @ 0x5d684 <PIC_RunQueue()+0x188>
   5d508: e59f1178     	ldr	r1, [pc, #0x178]        @ 0x5d688 <PIC_RunQueue()+0x18c>
   5d50c: e08f3003     	add	r3, pc, r3
   5d510: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   5d514: e793a002     	ldr	r10, [r3, r2]
   5d518: e24dd00c     	sub	sp, sp, #12
   5d51c: e7939001     	ldr	r9, [r3, r1]
   5d520: e59a2000     	ldr	r2, [r10]
   5d524: e58a0000     	str	r0, [r10]
   5d528: e5998000     	ldr	r8, [r9]
   5d52c: e0888002     	add	r8, r8, r2
   5d530: e1580000     	cmp	r8, r0
   5d534: e5898000     	str	r8, [r9]
   5d538: da000043     	ble	0x5d64c <PIC_RunQueue()+0x150> @ imm = #0x10c
   5d53c: e59f2148     	ldr	r2, [pc, #0x148]        @ 0x5d68c <PIC_RunQueue()+0x190>
   5d540: e7936002     	ldr	r6, [r3, r2]
   5d544: e3a02001     	mov	r2, #1
   5d548: e59f3140     	ldr	r3, [pc, #0x140]        @ 0x5d690 <PIC_RunQueue()+0x194>
   5d54c: e5965000     	ldr	r5, [r6]
   5d550: e08f3003     	add	r3, pc, r3
   5d554: e2833a02     	add	r3, r3, #8192
   5d558: e5934004     	ldr	r4, [r3, #0x4]
   5d55c: e59f3130     	ldr	r3, [pc, #0x130]        @ 0x5d694 <PIC_RunQueue()+0x198>
   5d560: e1540000     	cmp	r4, r0
   5d564: e08f3003     	add	r3, pc, r3
   5d568: e5c32000     	strb	r2, [r3]
   5d56c: 0a000040     	beq	0x5d674 <PIC_RunQueue()+0x178> @ imm = #0x100
   5d570: e59fb120     	ldr	r11, [pc, #0x120]       @ 0x5d698 <PIC_RunQueue()+0x19c>
   5d574: e0450008     	sub	r0, r5, r8
   5d578: e59f811c     	ldr	r8, [pc, #0x11c]        @ 0x5d69c <PIC_RunQueue()+0x1a0>
   5d57c: fa013e1d     	blx	0xacdf8 <__floatsisf>   @ imm = #0x4f874
   5d580: e08fb00b     	add	r11, pc, r11
   5d584: e1a07000     	mov	r7, r0
   5d588: e28bba02     	add	r11, r11, #8192
   5d58c: e08f8008     	add	r8, pc, r8
   5d590: ea00000b     	b	0x5d5c4 <PIC_RunQueue()+0xc8> @ imm = #0x2c
   5d594: e594200c     	ldr	r2, [r4, #0xc]
   5d598: e9940009     	ldmib	r4, {r0, r3}
   5d59c: e5885000     	str	r5, [r8]
   5d5a0: e58b2004     	str	r2, [r11, #0x4]
   5d5a4: e12fff33     	blx	r3
   5d5a8: e59b3000     	ldr	r3, [r11]
   5d5ac: e584300c     	str	r3, [r4, #0xc]
   5d5b0: e58b4000     	str	r4, [r11]
   5d5b4: e59b4004     	ldr	r4, [r11, #0x4]
   5d5b8: e3540000     	cmp	r4, #0
   5d5bc: 0a000029     	beq	0x5d668 <PIC_RunQueue()+0x16c> @ imm = #0xa4
   5d5c0: e5965000     	ldr	r5, [r6]
   5d5c4: e1a00005     	mov	r0, r5
   5d5c8: e5945000     	ldr	r5, [r4]
   5d5cc: fa013e09     	blx	0xacdf8 <__floatsisf>   @ imm = #0x4f824
   5d5d0: e1a01005     	mov	r1, r5
   5d5d4: fa013e31     	blx	0xacea0 <__mulsf3>      @ imm = #0x4f8c4
   5d5d8: e1a01007     	mov	r1, r7
   5d5dc: fa013f03     	blx	0xad1f0 <__aeabi_fcmple> @ imm = #0x4fc0c
   5d5e0: e2503000     	subs	r3, r0, #0
   5d5e4: 1affffea     	bne	0x5d594 <PIC_RunQueue()+0x98> @ imm = #-0x58
   5d5e8: e59f20b0     	ldr	r2, [pc, #0xb0]         @ 0x5d6a0 <PIC_RunQueue()+0x1a4>
   5d5ec: e5960000     	ldr	r0, [r6]
   5d5f0: e58d3004     	str	r3, [sp, #0x4]
   5d5f4: e08f2002     	add	r2, pc, r2
   5d5f8: e5998000     	ldr	r8, [r9]
   5d5fc: e5c23000     	strb	r3, [r2]
   5d600: fa013dfc     	blx	0xacdf8 <__floatsisf>   @ imm = #0x4f7f0
   5d604: e1a01005     	mov	r1, r5
   5d608: fa013e24     	blx	0xacea0 <__mulsf3>      @ imm = #0x4f890
   5d60c: e1a01007     	mov	r1, r7
   5d610: fa013d9d     	blx	0xacc8c <__subsf3>      @ imm = #0x4f674
   5d614: fa013f04     	blx	0xad22c <__fixsfsi>     @ imm = #0x4fc10
   5d618: e2502000     	subs	r2, r0, #0
   5d61c: 03a02001     	moveq	r2, #1
   5d620: e1580002     	cmp	r8, r2
   5d624: ca00000a     	bgt	0x5d654 <PIC_RunQueue()+0x158> @ imm = #0x28
   5d628: e59d0004     	ldr	r0, [sp, #0x4]
   5d62c: e59f3070     	ldr	r3, [pc, #0x70]         @ 0x5d6a4 <PIC_RunQueue()+0x1a8>
   5d630: e58a8000     	str	r8, [r10]
   5d634: e5890000     	str	r0, [r9]
   5d638: e08f3003     	add	r3, pc, r3
   5d63c: e5933000     	ldr	r3, [r3]
   5d640: e3530000     	cmp	r3, #0
   5d644: 1a000005     	bne	0x5d660 <PIC_RunQueue()+0x164> @ imm = #0x14
   5d648: e3a00001     	mov	r0, #1
   5d64c: e28dd00c     	add	sp, sp, #12
   5d650: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   5d654: e0480002     	sub	r0, r8, r2
   5d658: e1a08002     	mov	r8, r2
   5d65c: eafffff2     	b	0x5d62c <PIC_RunQueue()+0x130> @ imm = #-0x38
   5d660: ebfffdae     	bl	0x5cd20 <PIC_runIRQs()> @ imm = #-0x948
   5d664: eafffff7     	b	0x5d648 <PIC_RunQueue()+0x14c> @ imm = #-0x24
   5d668: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x5d6a8 <PIC_RunQueue()+0x1ac>
   5d66c: e5998000     	ldr	r8, [r9]
   5d670: e08f3003     	add	r3, pc, r3
   5d674: e3a00000     	mov	r0, #0
   5d678: e5c34000     	strb	r4, [r3]
   5d67c: eaffffea     	b	0x5d62c <PIC_RunQueue()+0x130> @ imm = #-0x58
   5d680: b4 89 05 00  	.word	0x000589b4
   5d684: 08 81 00 00  	.word	0x00008108
   5d688: 3c 7f 00 00  	.word	0x00007f3c
   5d68c: ec 7f 00 00  	.word	0x00007fec
   5d690: d0 fd 38 00  	.word	0x0038fdd0
   5d694: b8 fd 38 00  	.word	0x0038fdb8
   5d698: a0 fd 38 00  	.word	0x0038fda0
   5d69c: 8c fd 38 00  	.word	0x0038fd8c
   5d6a0: 28 fd 38 00  	.word	0x0038fd28
   5d6a4: f0 1c 39 00  	.word	0x00391cf0
   5d6a8: ac fc 38 00  	.word	0x0038fcac
