<PIC_RunQueue()>:
   5d47c: e59f316c     	ldr	r3, [pc, #0x16c]        @ 0x5d5f0 <PIC_RunQueue()+0x174>
   5d480: e3a00000     	mov	r0, #0
   5d484: e59f1168     	ldr	r1, [pc, #0x168]        @ 0x5d5f4 <PIC_RunQueue()+0x178>
   5d488: e59f2168     	ldr	r2, [pc, #0x168]        @ 0x5d5f8 <PIC_RunQueue()+0x17c>
   5d48c: e08f3003     	add	r3, pc, r3
   5d490: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   5d494: ed2d8b02     	vpush	{d8}
   5d498: e7936001     	ldr	r6, [r3, r1]
   5d49c: e7937002     	ldr	r7, [r3, r2]
   5d4a0: e5962000     	ldr	r2, [r6]
   5d4a4: e5971000     	ldr	r1, [r7]
   5d4a8: e5870000     	str	r0, [r7]
   5d4ac: e0822001     	add	r2, r2, r1
   5d4b0: e1520000     	cmp	r2, r0
   5d4b4: e5862000     	str	r2, [r6]
   5d4b8: da00003f     	ble	0x5d5bc <PIC_RunQueue()+0x140> @ imm = #0xfc
   5d4bc: e59f1138     	ldr	r1, [pc, #0x138]        @ 0x5d5fc <PIC_RunQueue()+0x180>
   5d4c0: e7938001     	ldr	r8, [r3, r1]
   5d4c4: e59f1134     	ldr	r1, [pc, #0x134]        @ 0x5d600 <PIC_RunQueue()+0x184>
   5d4c8: e5983000     	ldr	r3, [r8]
   5d4cc: e08f1001     	add	r1, pc, r1
   5d4d0: e2811a02     	add	r1, r1, #8192
   5d4d4: e5914004     	ldr	r4, [r1, #0x4]
   5d4d8: e59f1124     	ldr	r1, [pc, #0x124]        @ 0x5d604 <PIC_RunQueue()+0x188>
   5d4dc: e1540000     	cmp	r4, r0
   5d4e0: e3a00001     	mov	r0, #1
   5d4e4: e08f1001     	add	r1, pc, r1
   5d4e8: e5c10000     	strb	r0, [r1]
   5d4ec: 0a00003d     	beq	0x5d5e8 <PIC_RunQueue()+0x16c> @ imm = #0xf4
   5d4f0: e59f5110     	ldr	r5, [pc, #0x110]        @ 0x5d608 <PIC_RunQueue()+0x18c>
   5d4f4: e0432002     	sub	r2, r3, r2
   5d4f8: e59f910c     	ldr	r9, [pc, #0x10c]        @ 0x5d60c <PIC_RunQueue()+0x190>
   5d4fc: ee082a10     	vmov	s16, r2
   5d500: e08f5005     	add	r5, pc, r5
   5d504: eeb88ac8     	vcvt.f32.s32	s16, s16
   5d508: e2855a02     	add	r5, r5, #8192
   5d50c: e08f9009     	add	r9, pc, r9
   5d510: ea00000b     	b	0x5d544 <PIC_RunQueue()+0xc8> @ imm = #0x2c
   5d514: e594200c     	ldr	r2, [r4, #0xc]
   5d518: e9940009     	ldmib	r4, {r0, r3}
   5d51c: ed897a00     	vstr	s14, [r9]
   5d520: e5852004     	str	r2, [r5, #0x4]
   5d524: e12fff33     	blx	r3
   5d528: e5953000     	ldr	r3, [r5]
   5d52c: e584300c     	str	r3, [r4, #0xc]
   5d530: e5854000     	str	r4, [r5]
   5d534: e5954004     	ldr	r4, [r5, #0x4]
   5d538: e3540000     	cmp	r4, #0
   5d53c: 0a000023     	beq	0x5d5d0 <PIC_RunQueue()+0x154> @ imm = #0x8c
   5d540: e5983000     	ldr	r3, [r8]
   5d544: ee073a90     	vmov	s15, r3
   5d548: ed947a00     	vldr	s14, [r4]
   5d54c: eef87ae7     	vcvt.f32.s32	s15, s15
   5d550: ee677a87     	vmul.f32	s15, s15, s14
   5d554: eef47ac8     	vcmpe.f32	s15, s16
   5d558: eef1fa10     	vmrs	APSR_nzcv, fpscr
   5d55c: 9affffec     	bls	0x5d514 <PIC_RunQueue()+0x98> @ imm = #-0x50
   5d560: edd87a00     	vldr	s15, [r8]
   5d564: e3a01000     	mov	r1, #0
   5d568: e59f30a0     	ldr	r3, [pc, #0xa0]         @ 0x5d610 <PIC_RunQueue()+0x194>
   5d56c: e5962000     	ldr	r2, [r6]
   5d570: eef87ae7     	vcvt.f32.s32	s15, s15
   5d574: e08f3003     	add	r3, pc, r3
   5d578: e5c31000     	strb	r1, [r3]
   5d57c: ee178a87     	vnmls.f32	s16, s15, s14
   5d580: eefd7ac8     	vcvt.s32.f32	s15, s16
   5d584: ee173a90     	vmov	r3, s15
   5d588: e1530001     	cmp	r3, r1
   5d58c: 03a03001     	moveq	r3, #1
   5d590: e1520003     	cmp	r2, r3
   5d594: c0421003     	subgt	r1, r2, r3
   5d598: c1a02003     	movgt	r2, r3
   5d59c: e59f3070     	ldr	r3, [pc, #0x70]         @ 0x5d614 <PIC_RunQueue()+0x198>
   5d5a0: e5872000     	str	r2, [r7]
   5d5a4: e5861000     	str	r1, [r6]
   5d5a8: e08f3003     	add	r3, pc, r3
   5d5ac: e5933000     	ldr	r3, [r3]
   5d5b0: e3530000     	cmp	r3, #0
   5d5b4: 1a000002     	bne	0x5d5c4 <PIC_RunQueue()+0x148> @ imm = #0x8
   5d5b8: e3a00001     	mov	r0, #1
   5d5bc: ecbd8b02     	vpop	{d8}
   5d5c0: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   5d5c4: ebfffdd5     	bl	0x5cd20 <PIC_runIRQs()> @ imm = #-0x8ac
   5d5c8: e3a00001     	mov	r0, #1
   5d5cc: eafffffa     	b	0x5d5bc <PIC_RunQueue()+0x140> @ imm = #-0x18
   5d5d0: e59f3040     	ldr	r3, [pc, #0x40]         @ 0x5d618 <PIC_RunQueue()+0x19c>
   5d5d4: e5962000     	ldr	r2, [r6]
   5d5d8: e08f3003     	add	r3, pc, r3
   5d5dc: e5c34000     	strb	r4, [r3]
   5d5e0: e3a01000     	mov	r1, #0
   5d5e4: eaffffec     	b	0x5d59c <PIC_RunQueue()+0x120> @ imm = #-0x50
   5d5e8: e5c14000     	strb	r4, [r1]
   5d5ec: eafffffb     	b	0x5d5e0 <PIC_RunQueue()+0x164> @ imm = #-0x14
   5d5f0: b4 89 05 00  	.word	0x000589b4
   5d5f4: 3c 7f 00 00  	.word	0x00007f3c
   5d5f8: 08 81 00 00  	.word	0x00008108
   5d5fc: ec 7f 00 00  	.word	0x00007fec
   5d600: 54 fe 38 00  	.word	0x0038fe54
   5d604: 38 fe 38 00  	.word	0x0038fe38
   5d608: 20 fe 38 00  	.word	0x0038fe20
   5d60c: 0c fe 38 00  	.word	0x0038fe0c
   5d610: a8 fd 38 00  	.word	0x0038fda8
   5d614: 80 1d 39 00  	.word	0x00391d80
   5d618: 44 fd 38 00  	.word	0x0038fd44
