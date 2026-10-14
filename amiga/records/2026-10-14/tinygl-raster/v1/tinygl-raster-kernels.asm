
.context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-pic-vfp-matrix-bench-tinygl-internal-raster/109-ztriangle.arm.o:	file format elf32-littlearm

Disassembly of section .text.ZB_fillTriangleFlat_DT0_DW0:

00000000 <ZB_fillTriangleFlat_DT0_DW0>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a09000     	mov	r9, r0
       8: ed2d8b02     	vpush	{d8}
       c: e1a08003     	mov	r8, r3
      10: e24dd01c     	sub	sp, sp, #28
      14: e592c004     	ldr	r12, [r2, #0x4]
      18: e5910004     	ldr	r0, [r1, #0x4]
      1c: e593e004     	ldr	lr, [r3, #0x4]
      20: e15c0000     	cmp	r12, r0
      24: a1a04000     	movge	r4, r0
      28: b1a0400c     	movlt	r4, r12
      2c: a1a0000c     	movge	r0, r12
      30: b1a05002     	movlt	r5, r2
      34: a1a05001     	movge	r5, r1
      38: b1a03001     	movlt	r3, r1
      3c: a1a03002     	movge	r3, r2
      40: e15e0004     	cmp	lr, r4
      44: e1a0c004     	mov	r12, r4
      48: e58d5008     	str	r5, [sp, #0x8]
      4c: ba00000b     	blt	0x80 <ZB_fillTriangleFlat_DT0_DW0+0x80> @ imm = #0x2c
      50: e15e0000     	cmp	lr, r0
      54: e1a02008     	mov	r2, r8
      58: a1a0c000     	movge	r12, r0
      5c: b1a0c00e     	movlt	r12, lr
      60: a1a0000e     	movge	r0, lr
      64: a58d3008     	strge	r3, [sp, #0x8]
      68: a1a08005     	movge	r8, r5
      6c: a1a0e004     	movge	lr, r4
      70: a1a03002     	movge	r3, r2
      74: b1a0e004     	movlt	lr, r4
      78: b59d8008     	ldrlt	r8, [sp, #0x8]
      7c: b58d2008     	strlt	r2, [sp, #0x8]
      80: e5986000     	ldr	r6, [r8]
      84: e040000e     	sub	r0, r0, lr
      88: e5932000     	ldr	r2, [r3]
      8c: ee070a90     	vmov	s15, r0
      90: e5934018     	ldr	r4, [r3, #0x18]
      94: e0422006     	sub	r2, r2, r6
      98: eef87ae7     	vcvt.f32.s32	s15, s15
      9c: e5997004     	ldr	r7, [r9, #0x4]
      a0: ee082a10     	vmov	s16, r2
      a4: e04c200e     	sub	r2, r12, lr
      a8: e1a04444     	asr	r4, r4, #8
      ac: ee072a10     	vmov	s14, r2
      b0: e59d2008     	ldr	r2, [sp, #0x8]
      b4: e2044cff     	and	r4, r4, #65280
      b8: eeb87ac7     	vcvt.f32.s32	s14, s14
      bc: eeb88ac8     	vcvt.f32.s32	s16, s16
      c0: e5922000     	ldr	r2, [r2]
      c4: e0422006     	sub	r2, r2, r6
      c8: ee288a07     	vmul.f32	s16, s16, s14
      cc: ee072a10     	vmov	s14, r2
      d0: e5992020     	ldr	r2, [r9, #0x20]
      d4: eeb87ac7     	vcvt.f32.s32	s14, s14
      d8: e00e0e92     	mul	lr, r2, lr
      dc: e5d3201e     	ldrb	r2, [r3, #0x1e]
      e0: ee178a27     	vnmls.f32	s16, s14, s15
      e4: e1844002     	orr	r4, r4, r2
      e8: e5932014     	ldr	r2, [r3, #0x14]
      ec: e087710e     	add	r7, r7, lr, lsl #2
      f0: e20228ff     	and	r2, r2, #16711680
      f4: e1844002     	orr	r4, r4, r2
      f8: eeb58a40     	vcmp.f32	s16, #0
      fc: eef1fa10     	vmrs	APSR_nzcv, fpscr
     100: 1ef77a00     	vmovne.f32	s15, #1.000000e+00
     104: 1e878a88     	vdivne.f32	s16, s15, s16
     108: eeb58ac0     	vcmpe.f32	s16, #0
     10c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     110: da000083     	ble	0x324 <ZB_fillTriangleFlat_DT0_DW0+0x324> @ imm = #0x20c
     114: e5931004     	ldr	r1, [r3, #0x4]
     118: e1a02003     	mov	r2, r3
     11c: e59d0008     	ldr	r0, [sp, #0x8]
     120: e58d0004     	str	r0, [sp, #0x4]
     124: e5985004     	ldr	r5, [r8, #0x4]
     128: e0411005     	sub	r1, r1, r5
     12c: e04cb005     	sub	r11, r12, r5
     130: e3510000     	cmp	r1, #0
     134: da00000a     	ble	0x164 <ZB_fillTriangleFlat_DT0_DW0+0x164> @ imm = #0x28
     138: e5922000     	ldr	r2, [r2]
     13c: e58d300c     	str	r3, [sp, #0xc]
     140: e0422006     	sub	r2, r2, r6
     144: e1a00802     	lsl	r0, r2, #16
     148: ebfffffe     	bl	0x148 <ZB_fillTriangleFlat_DT0_DW0+0x148> @ imm = #-0x8
     14c: e1a03840     	asr	r3, r0, #16
     150: e6ffa070     	uxth	r10, r0
     154: e2832001     	add	r2, r3, #1
     158: e58d3000     	str	r3, [sp]
     15c: e59d300c     	ldr	r3, [sp, #0xc]
     160: ea000002     	b	0x170 <ZB_fillTriangleFlat_DT0_DW0+0x170> @ imm = #0x8
     164: e3a02001     	mov	r2, #1
     168: e3a0a000     	mov	r10, #0
     16c: e58da000     	str	r10, [sp]
     170: e59d0004     	ldr	r0, [sp, #0x4]
     174: e58db004     	str	r11, [sp, #0x4]
     178: e3a0b000     	mov	r11, #0
     17c: e58d3014     	str	r3, [sp, #0x14]
     180: e5901004     	ldr	r1, [r0, #0x4]
     184: e0411005     	sub	r1, r1, r5
     188: e3a05000     	mov	r5, #0
     18c: e3510000     	cmp	r1, #0
     190: e5983000     	ldr	r3, [r8]
     194: d3a08000     	movle	r8, #0
     198: da000008     	ble	0x1c0 <ZB_fillTriangleFlat_DT0_DW0+0x1c0> @ imm = #0x20
     19c: e5900000     	ldr	r0, [r0]
     1a0: e58d2010     	str	r2, [sp, #0x10]
     1a4: e58d300c     	str	r3, [sp, #0xc]
     1a8: e0400003     	sub	r0, r0, r3
     1ac: e1a00800     	lsl	r0, r0, #16
     1b0: ebfffffe     	bl	0x1b0 <ZB_fillTriangleFlat_DT0_DW0+0x1b0> @ imm = #-0x8
     1b4: e59d300c     	ldr	r3, [sp, #0xc]
     1b8: e1a08000     	mov	r8, r0
     1bc: e59d2010     	ldr	r2, [sp, #0x10]
     1c0: e58db00c     	str	r11, [sp, #0xc]
     1c4: e1a0b803     	lsl	r11, r3, #16
     1c8: e59d3004     	ldr	r3, [sp, #0x4]
     1cc: e3530000     	cmp	r3, #0
     1d0: da00002d     	ble	0x28c <ZB_fillTriangleFlat_DT0_DW0+0x28c> @ imm = #0xb4
     1d4: e59de004     	ldr	lr, [sp, #0x4]
     1d8: e1a0c00b     	mov	r12, r11
     1dc: e58db010     	str	r11, [sp, #0x10]
     1e0: e066084c     	rsb	r0, r6, r12, asr #16
     1e4: e0873106     	add	r3, r7, r6, lsl #2
     1e8: e3500002     	cmp	r0, #2
     1ec: e24ee001     	sub	lr, lr, #1
     1f0: da000010     	ble	0x238 <ZB_fillTriangleFlat_DT0_DW0+0x238> @ imm = #0x40
     1f4: e283b010     	add	r11, r3, #16
     1f8: e1a01000     	mov	r1, r0
     1fc: e1a0300b     	mov	r3, r11
     200: e2411004     	sub	r1, r1, #4
     204: e5034010     	str	r4, [r3, #-0x10]
     208: e3510002     	cmp	r1, #2
     20c: e503400c     	str	r4, [r3, #-0xc]
     210: e5034008     	str	r4, [r3, #-0x8]
     214: e2833010     	add	r3, r3, #16
     218: e5034014     	str	r4, [r3, #-0x14]
     21c: cafffff7     	bgt	0x200 <ZB_fillTriangleFlat_DT0_DW0+0x200> @ imm = #-0x24
     220: e2401003     	sub	r1, r0, #3
     224: e2400004     	sub	r0, r0, #4
     228: e1a03121     	lsr	r3, r1, #2
     22c: e3c11003     	bic	r1, r1, #3
     230: e0400001     	sub	r0, r0, r1
     234: e08b3203     	add	r3, r11, r3, lsl #4
     238: e3500000     	cmp	r0, #0
     23c: a0830100     	addge	r0, r3, r0, lsl #2
     240: a2433004     	subge	r3, r3, #4
     244: ba000002     	blt	0x254 <ZB_fillTriangleFlat_DT0_DW0+0x254> @ imm = #0x8
     248: e5a34004     	str	r4, [r3, #0x4]!
     24c: e1500003     	cmp	r0, r3
     250: 1afffffc     	bne	0x248 <ZB_fillTriangleFlat_DT0_DW0+0x248> @ imm = #-0x10
     254: e085500a     	add	r5, r5, r10
     258: e3550000     	cmp	r5, #0
     25c: c2455801     	subgt	r5, r5, #65536
     260: c0866002     	addgt	r6, r6, r2
     264: d59d3000     	ldrle	r3, [sp]
     268: d0866003     	addle	r6, r6, r3
     26c: e5993020     	ldr	r3, [r9, #0x20]
     270: e35e0000     	cmp	lr, #0
     274: e08cc008     	add	r12, r12, r8
     278: e0877103     	add	r7, r7, r3, lsl #2
     27c: 1affffd7     	bne	0x1e0 <ZB_fillTriangleFlat_DT0_DW0+0x1e0> @ imm = #-0xa4
     280: e59db010     	ldr	r11, [sp, #0x10]
     284: e59d3004     	ldr	r3, [sp, #0x4]
     288: e02bb893     	mla	r11, r3, r8, r11
     28c: e59d300c     	ldr	r3, [sp, #0xc]
     290: e3530001     	cmp	r3, #1
     294: 1a000002     	bne	0x2a4 <ZB_fillTriangleFlat_DT0_DW0+0x2a4> @ imm = #0x8
     298: e28dd01c     	add	sp, sp, #28
     29c: ecbd8b02     	vpop	{d8}
     2a0: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     2a4: e59d3014     	ldr	r3, [sp, #0x14]
     2a8: eeb58ac0     	vcmpe.f32	s16, #0
     2ac: eef1fa10     	vmrs	APSR_nzcv, fpscr
     2b0: e5931004     	ldr	r1, [r3, #0x4]
     2b4: e59d3008     	ldr	r3, [sp, #0x8]
     2b8: e5933004     	ldr	r3, [r3, #0x4]
     2bc: e0411003     	sub	r1, r1, r3
     2c0: e3a03001     	mov	r3, #1
     2c4: e0810003     	add	r0, r1, r3
     2c8: e58d0004     	str	r0, [sp, #0x4]
     2cc: c59d0014     	ldrgt	r0, [sp, #0x14]
     2d0: c1a0b003     	movgt	r11, r3
     2d4: c59d8008     	ldrgt	r8, [sp, #0x8]
     2d8: caffffab     	bgt	0x18c <ZB_fillTriangleFlat_DT0_DW0+0x18c> @ imm = #-0x154
     2dc: e59d2008     	ldr	r2, [sp, #0x8]
     2e0: e3510000     	cmp	r1, #0
     2e4: e5926000     	ldr	r6, [r2]
     2e8: da000011     	ble	0x334 <ZB_fillTriangleFlat_DT0_DW0+0x334> @ imm = #0x44
     2ec: e58d3000     	str	r3, [sp]
     2f0: e59d3014     	ldr	r3, [sp, #0x14]
     2f4: e5930000     	ldr	r0, [r3]
     2f8: e0400006     	sub	r0, r0, r6
     2fc: e1a00800     	lsl	r0, r0, #16
     300: ebfffffe     	bl	0x300 <ZB_fillTriangleFlat_DT0_DW0+0x300> @ imm = #-0x8
     304: e59d3000     	ldr	r3, [sp]
     308: e6ffa070     	uxth	r10, r0
     30c: e59d500c     	ldr	r5, [sp, #0xc]
     310: e58d300c     	str	r3, [sp, #0xc]
     314: e1a03840     	asr	r3, r0, #16
     318: e2832001     	add	r2, r3, #1
     31c: e58d3000     	str	r3, [sp]
     320: eaffffa8     	b	0x1c8 <ZB_fillTriangleFlat_DT0_DW0+0x1c8> @ imm = #-0x160
     324: e59d2008     	ldr	r2, [sp, #0x8]
     328: e1a0100c     	mov	r1, r12
     32c: e58d3004     	str	r3, [sp, #0x4]
     330: eaffff7b     	b	0x124 <ZB_fillTriangleFlat_DT0_DW0+0x124> @ imm = #-0x214
     334: e59d3004     	ldr	r3, [sp, #0x4]
     338: e3530001     	cmp	r3, #1
     33c: 1affffd5     	bne	0x298 <ZB_fillTriangleFlat_DT0_DW0+0x298> @ imm = #-0xac
     340: e59da00c     	ldr	r10, [sp, #0xc]
     344: e59d2004     	ldr	r2, [sp, #0x4]
     348: e1a0500a     	mov	r5, r10
     34c: e58da000     	str	r10, [sp]
     350: e58d200c     	str	r2, [sp, #0xc]
     354: eaffff9e     	b	0x1d4 <ZB_fillTriangleFlat_DT0_DW0+0x1d4> @ imm = #-0x188

Disassembly of section .text.ZB_fillTriangleFlat_DT0_DW1:

00000000 <ZB_fillTriangleFlat_DT0_DW1>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a04003     	mov	r4, r3
       8: ed2d8b02     	vpush	{d8}
       c: e24dd04c     	sub	sp, sp, #76
      10: e592c004     	ldr	r12, [r2, #0x4]
      14: e58d0018     	str	r0, [sp, #0x18]
      18: e5910004     	ldr	r0, [r1, #0x4]
      1c: e593e004     	ldr	lr, [r3, #0x4]
      20: e15c0000     	cmp	r12, r0
      24: a1a05000     	movge	r5, r0
      28: b1a0500c     	movlt	r5, r12
      2c: a1a0000c     	movge	r0, r12
      30: a1a03001     	movge	r3, r1
      34: b1a03002     	movlt	r3, r2
      38: a1a01002     	movge	r1, r2
      3c: e15e0005     	cmp	lr, r5
      40: e1a0c005     	mov	r12, r5
      44: e58d1034     	str	r1, [sp, #0x34]
      48: ba000006     	blt	0x68 <ZB_fillTriangleFlat_DT0_DW1+0x68> @ imm = #0x18
      4c: e15e0000     	cmp	lr, r0
      50: aa0000e6     	bge	0x3f0 <ZB_fillTriangleFlat_DT0_DW1+0x3f0> @ imm = #0x398
      54: e1a02004     	mov	r2, r4
      58: e1a0c00e     	mov	r12, lr
      5c: e1a04003     	mov	r4, r3
      60: e1a0e005     	mov	lr, r5
      64: e1a03002     	mov	r3, r2
      68: e59d2034     	ldr	r2, [sp, #0x34]
      6c: e040000e     	sub	r0, r0, lr
      70: e5949000     	ldr	r9, [r4]
      74: ee070a90     	vmov	s15, r0
      78: e5922000     	ldr	r2, [r2]
      7c: eeb87ae7     	vcvt.f32.s32	s14, s15
      80: e59d1034     	ldr	r1, [sp, #0x34]
      84: e0422009     	sub	r2, r2, r9
      88: e5945008     	ldr	r5, [r4, #0x8]
      8c: e59d7018     	ldr	r7, [sp, #0x18]
      90: ee072a90     	vmov	s15, r2
      94: e04c200e     	sub	r2, r12, lr
      98: e1a0b005     	mov	r11, r5
      9c: ee062a90     	vmov	s13, r2
      a0: e5932000     	ldr	r2, [r3]
      a4: eef87ae7     	vcvt.f32.s32	s15, s15
      a8: eef86ae6     	vcvt.f32.s32	s13, s13
      ac: e0422009     	sub	r2, r2, r9
      b0: e5970004     	ldr	r0, [r7, #0x4]
      b4: ee062a10     	vmov	s12, r2
      b8: ee268aa7     	vmul.f32	s16, s13, s15
      bc: eeb86ac6     	vcvt.f32.s32	s12, s12
      c0: e5912008     	ldr	r2, [r1, #0x8]
      c4: ee168a07     	vnmls.f32	s16, s12, s14
      c8: e0422005     	sub	r2, r2, r5
      cc: eeb58a40     	vcmp.f32	s16, #0
      d0: eef1fa10     	vmrs	APSR_nzcv, fpscr
      d4: 1ef75a00     	vmovne.f32	s11, #1.000000e+00
      d8: 1e858a88     	vdivne.f32	s16, s11, s16
      dc: ee052a90     	vmov	s11, r2
      e0: e5932008     	ldr	r2, [r3, #0x8]
      e4: eef85ae5     	vcvt.f32.s32	s11, s11
      e8: e0422005     	sub	r2, r2, r5
      ec: ee052a10     	vmov	s10, r2
      f0: e5972020     	ldr	r2, [r7, #0x20]
      f4: eeb85ac5     	vcvt.f32.s32	s10, s10
      f8: e0020e92     	mul	r2, r2, lr
      fc: e1a0e001     	mov	lr, r1
     100: e5911018     	ldr	r1, [r1, #0x18]
     104: ee686a26     	vmul.f32	s13, s16, s13
     108: ee687a27     	vmul.f32	s15, s16, s15
     10c: e0800102     	add	r0, r0, r2, lsl #2
     110: ee287a07     	vmul.f32	s14, s16, s14
     114: e1a01441     	asr	r1, r1, #8
     118: ee286a06     	vmul.f32	s12, s16, s12
     11c: ee666aa5     	vmul.f32	s13, s13, s11
     120: e2011cff     	and	r1, r1, #65280
     124: ee677a85     	vmul.f32	s15, s15, s10
     128: e58d0004     	str	r0, [sp, #0x4]
     12c: e5970000     	ldr	r0, [r7]
     130: ee576a05     	vnmls.f32	s13, s14, s10
     134: ee567a25     	vnmls.f32	s15, s12, s11
     138: eeb58ac0     	vcmpe.f32	s16, #0
     13c: e0802082     	add	r2, r0, r2, lsl #1
     140: eef1fa10     	vmrs	APSR_nzcv, fpscr
     144: e58d2000     	str	r2, [sp]
     148: e5de201e     	ldrb	r2, [lr, #0x1e]
     14c: eebd7ae6     	vcvt.s32.f32	s14, s13
     150: eefd7ae7     	vcvt.s32.f32	s15, s15
     154: e1811002     	orr	r1, r1, r2
     158: e59e2014     	ldr	r2, [lr, #0x14]
     15c: ee176a10     	vmov	r6, s14
     160: e20228ff     	and	r2, r2, #16711680
     164: eef08a67     	vmov.f32	s17, s15
     168: e1818002     	orr	r8, r1, r2
     16c: e1a02106     	lsl	r2, r6, #2
     170: e58d2020     	str	r2, [sp, #0x20]
     174: da0000cf     	ble	0x4b8 <ZB_fillTriangleFlat_DT0_DW1+0x4b8> @ imm = #0x33c
     178: e59e1004     	ldr	r1, [lr, #0x4]
     17c: e1a0000e     	mov	r0, lr
     180: e1a0a003     	mov	r10, r3
     184: e5947004     	ldr	r7, [r4, #0x4]
     188: e0411007     	sub	r1, r1, r7
     18c: e04c2007     	sub	r2, r12, r7
     190: e3510000     	cmp	r1, #0
     194: da000014     	ble	0x1ec <ZB_fillTriangleFlat_DT0_DW1+0x1ec> @ imm = #0x50
     198: e5900000     	ldr	r0, [r0]
     19c: e1a0b005     	mov	r11, r5
     1a0: e58d300c     	str	r3, [sp, #0xc]
     1a4: e58d2008     	str	r2, [sp, #0x8]
     1a8: e0400009     	sub	r0, r0, r9
     1ac: e1a00800     	lsl	r0, r0, #16
     1b0: ebfffffe     	bl	0x1b0 <ZB_fillTriangleFlat_DT0_DW1+0x1b0> @ imm = #-0x8
     1b4: e6ff2070     	uxth	r2, r0
     1b8: e1a03840     	asr	r3, r0, #16
     1bc: e58d201c     	str	r2, [sp, #0x1c]
     1c0: ee182a90     	vmov	r2, s17
     1c4: e58d302c     	str	r3, [sp, #0x2c]
     1c8: e0222693     	mla	r2, r3, r6, r2
     1cc: e2833001     	add	r3, r3, #1
     1d0: e58d3024     	str	r3, [sp, #0x24]
     1d4: e0863002     	add	r3, r6, r2
     1d8: e58d2030     	str	r2, [sp, #0x30]
     1dc: e58d3028     	str	r3, [sp, #0x28]
     1e0: e59d2008     	ldr	r2, [sp, #0x8]
     1e4: e59d300c     	ldr	r3, [sp, #0xc]
     1e8: ea000008     	b	0x210 <ZB_fillTriangleFlat_DT0_DW1+0x210> @ imm = #0x20
     1ec: ee180a90     	vmov	r0, s17
     1f0: e3a01000     	mov	r1, #0
     1f4: e086c000     	add	r12, r6, r0
     1f8: e3a00001     	mov	r0, #1
     1fc: e58d101c     	str	r1, [sp, #0x1c]
     200: e58dc028     	str	r12, [sp, #0x28]
     204: edcd8a0c     	vstr	s17, [sp, #48]
     208: e58d0024     	str	r0, [sp, #0x24]
     20c: e58d102c     	str	r1, [sp, #0x2c]
     210: e59a1004     	ldr	r1, [r10, #0x4]
     214: e1a05002     	mov	r5, r2
     218: e58d3044     	str	r3, [sp, #0x44]
     21c: e1a0300a     	mov	r3, r10
     220: e1a0a009     	mov	r10, r9
     224: e1a0900b     	mov	r9, r11
     228: e0411007     	sub	r1, r1, r7
     22c: e3a07000     	mov	r7, #0
     230: e1a0b007     	mov	r11, r7
     234: e3510000     	cmp	r1, #0
     238: e5944000     	ldr	r4, [r4]
     23c: d3a03000     	movle	r3, #0
     240: d58d3014     	strle	r3, [sp, #0x14]
     244: da000004     	ble	0x25c <ZB_fillTriangleFlat_DT0_DW1+0x25c> @ imm = #0x10
     248: e5932000     	ldr	r2, [r3]
     24c: e0422004     	sub	r2, r2, r4
     250: e1a00802     	lsl	r0, r2, #16
     254: ebfffffe     	bl	0x254 <ZB_fillTriangleFlat_DT0_DW1+0x254> @ imm = #-0x8
     258: e58d0014     	str	r0, [sp, #0x14]
     25c: e1a04804     	lsl	r4, r4, #16
     260: e1a0000b     	mov	r0, r11
     264: e58d7038     	str	r7, [sp, #0x38]
     268: e3550000     	cmp	r5, #0
     26c: da000059     	ble	0x3d8 <ZB_fillTriangleFlat_DT0_DW1+0x3d8> @ imm = #0x164
     270: e1a07005     	mov	r7, r5
     274: e1a0b004     	mov	r11, r4
     278: e58d403c     	str	r4, [sp, #0x3c]
     27c: e58d5040     	str	r5, [sp, #0x40]
     280: e59d3004     	ldr	r3, [sp, #0x4]
     284: e06a484b     	rsb	r4, r10, r11, asr #16
     288: e2477001     	sub	r7, r7, #1
     28c: e3540002     	cmp	r4, #2
     290: e083e10a     	add	lr, r3, r10, lsl #2
     294: e59d3000     	ldr	r3, [sp]
     298: e083c08a     	add	r12, r3, r10, lsl #1
     29c: e1a03009     	mov	r3, r9
     2a0: da000029     	ble	0x34c <ZB_fillTriangleFlat_DT0_DW1+0x34c> @ imm = #0xa4
     2a4: e2445003     	sub	r5, r4, #3
     2a8: e28c2008     	add	r2, r12, #8
     2ac: e1a05125     	lsr	r5, r5, #2
     2b0: e28ee010     	add	lr, lr, #16
     2b4: e28cc010     	add	r12, r12, #16
     2b8: e58d200c     	str	r2, [sp, #0xc]
     2bc: e1a03185     	lsl	r3, r5, #3
     2c0: e1a0100e     	mov	r1, lr
     2c4: e58d0010     	str	r0, [sp, #0x10]
     2c8: e08cc003     	add	r12, r12, r3
     2cc: e58d3008     	str	r3, [sp, #0x8]
     2d0: e1a03009     	mov	r3, r9
     2d4: e0830006     	add	r0, r3, r6
     2d8: e1a03723     	lsr	r3, r3, #14
     2dc: e5018010     	str	r8, [r1, #-0x10]
     2e0: e2822008     	add	r2, r2, #8
     2e4: e14231b0     	strh	r3, [r2, #-16]
     2e8: e0803006     	add	r3, r0, r6
     2ec: e1a00720     	lsr	r0, r0, #14
     2f0: e501800c     	str	r8, [r1, #-0xc]
     2f4: e2811010     	add	r1, r1, #16
     2f8: e14200be     	strh	r0, [r2, #-14]
     2fc: e0830006     	add	r0, r3, r6
     300: e1a03723     	lsr	r3, r3, #14
     304: e5018018     	str	r8, [r1, #-0x18]
     308: e14230bc     	strh	r3, [r2, #-12]
     30c: e1a03720     	lsr	r3, r0, #14
     310: e5018014     	str	r8, [r1, #-0x14]
     314: e14230ba     	strh	r3, [r2, #-10]
     318: e15c0002     	cmp	r12, r2
     31c: e0803006     	add	r3, r0, r6
     320: 1affffeb     	bne	0x2d4 <ZB_fillTriangleFlat_DT0_DW1+0x2d4> @ imm = #-0x54
     324: e59d3008     	ldr	r3, [sp, #0x8]
     328: e2444004     	sub	r4, r4, #4
     32c: e59d200c     	ldr	r2, [sp, #0xc]
     330: e0444105     	sub	r4, r4, r5, lsl #2
     334: e59d0010     	ldr	r0, [sp, #0x10]
     338: e08ee205     	add	lr, lr, r5, lsl #4
     33c: e083c002     	add	r12, r3, r2
     340: e59d2020     	ldr	r2, [sp, #0x20]
     344: e0893002     	add	r3, r9, r2
     348: e0233295     	mla	r3, r5, r2, r3
     34c: e3540000     	cmp	r4, #0
     350: a08e4104     	addge	r4, lr, r4, lsl #2
     354: a24ee004     	subge	lr, lr, #4
     358: ba000005     	blt	0x374 <ZB_fillTriangleFlat_DT0_DW1+0x374> @ imm = #0x14
     35c: e1a02723     	lsr	r2, r3, #14
     360: e5ae8004     	str	r8, [lr, #0x4]!
     364: e154000e     	cmp	r4, lr
     368: e0833006     	add	r3, r3, r6
     36c: e0cc20b2     	strh	r2, [r12], #2
     370: 1afffff9     	bne	0x35c <ZB_fillTriangleFlat_DT0_DW1+0x35c> @ imm = #-0x1c
     374: e59d301c     	ldr	r3, [sp, #0x1c]
     378: e0800003     	add	r0, r0, r3
     37c: e3500000     	cmp	r0, #0
     380: da000021     	ble	0x40c <ZB_fillTriangleFlat_DT0_DW1+0x40c> @ imm = #0x84
     384: e59d3024     	ldr	r3, [sp, #0x24]
     388: e2400801     	sub	r0, r0, #65536
     38c: e08aa003     	add	r10, r10, r3
     390: e59d3028     	ldr	r3, [sp, #0x28]
     394: e0899003     	add	r9, r9, r3
     398: e59d3014     	ldr	r3, [sp, #0x14]
     39c: e3570000     	cmp	r7, #0
     3a0: e59d2004     	ldr	r2, [sp, #0x4]
     3a4: e08bb003     	add	r11, r11, r3
     3a8: e59d3018     	ldr	r3, [sp, #0x18]
     3ac: e5933020     	ldr	r3, [r3, #0x20]
     3b0: e0822103     	add	r2, r2, r3, lsl #2
     3b4: e58d2004     	str	r2, [sp, #0x4]
     3b8: e59d2000     	ldr	r2, [sp]
     3bc: e0823083     	add	r3, r2, r3, lsl #1
     3c0: e58d3000     	str	r3, [sp]
     3c4: 1affffad     	bne	0x280 <ZB_fillTriangleFlat_DT0_DW1+0x280> @ imm = #-0x14c
     3c8: e59d403c     	ldr	r4, [sp, #0x3c]
     3cc: e59d5040     	ldr	r5, [sp, #0x40]
     3d0: e59d3014     	ldr	r3, [sp, #0x14]
     3d4: e0244395     	mla	r4, r5, r3, r4
     3d8: e59d3038     	ldr	r3, [sp, #0x38]
     3dc: e3530001     	cmp	r3, #1
     3e0: 1a00000e     	bne	0x420 <ZB_fillTriangleFlat_DT0_DW1+0x420> @ imm = #0x38
     3e4: e28dd04c     	add	sp, sp, #76
     3e8: ecbd8b02     	vpop	{d8}
     3ec: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     3f0: e1a0c000     	mov	r12, r0
     3f4: e58d4034     	str	r4, [sp, #0x34]
     3f8: e1a0000e     	mov	r0, lr
     3fc: e1a04003     	mov	r4, r3
     400: e1a0e005     	mov	lr, r5
     404: e1a03001     	mov	r3, r1
     408: eaffff16     	b	0x68 <ZB_fillTriangleFlat_DT0_DW1+0x68> @ imm = #-0x3a8
     40c: e59d302c     	ldr	r3, [sp, #0x2c]
     410: e08aa003     	add	r10, r10, r3
     414: e59d3030     	ldr	r3, [sp, #0x30]
     418: e0899003     	add	r9, r9, r3
     41c: eaffffdd     	b	0x398 <ZB_fillTriangleFlat_DT0_DW1+0x398> @ imm = #-0x8c
     420: e59d3034     	ldr	r3, [sp, #0x34]
     424: e3a07001     	mov	r7, #1
     428: e59d2044     	ldr	r2, [sp, #0x44]
     42c: eeb58ac0     	vcmpe.f32	s16, #0
     430: e5933004     	ldr	r3, [r3, #0x4]
     434: e5921004     	ldr	r1, [r2, #0x4]
     438: eef1fa10     	vmrs	APSR_nzcv, fpscr
     43c: e0431001     	sub	r1, r3, r1
     440: e0815007     	add	r5, r1, r7
     444: c59d4044     	ldrgt	r4, [sp, #0x44]
     448: c1a0b000     	movgt	r11, r0
     44c: c59d3034     	ldrgt	r3, [sp, #0x34]
     450: caffff77     	bgt	0x234 <ZB_fillTriangleFlat_DT0_DW1+0x234> @ imm = #-0x224
     454: e59d3044     	ldr	r3, [sp, #0x44]
     458: e3510000     	cmp	r1, #0
     45c: e593a000     	ldr	r10, [r3]
     460: da000018     	ble	0x4c8 <ZB_fillTriangleFlat_DT0_DW1+0x4c8> @ imm = #0x60
     464: e59d3034     	ldr	r3, [sp, #0x34]
     468: e5930000     	ldr	r0, [r3]
     46c: e040000a     	sub	r0, r0, r10
     470: e1a00800     	lsl	r0, r0, #16
     474: ebfffffe     	bl	0x474 <ZB_fillTriangleFlat_DT0_DW1+0x474> @ imm = #-0x8
     478: e6ff3070     	uxth	r3, r0
     47c: e1a02840     	asr	r2, r0, #16
     480: e59d0038     	ldr	r0, [sp, #0x38]
     484: e58d301c     	str	r3, [sp, #0x1c]
     488: e59d3044     	ldr	r3, [sp, #0x44]
     48c: e58d202c     	str	r2, [sp, #0x2c]
     490: e58d7038     	str	r7, [sp, #0x38]
     494: e5939008     	ldr	r9, [r3, #0x8]
     498: ee183a90     	vmov	r3, s17
     49c: e0233692     	mla	r3, r2, r6, r3
     4a0: e2822001     	add	r2, r2, #1
     4a4: e58d2024     	str	r2, [sp, #0x24]
     4a8: e58d3030     	str	r3, [sp, #0x30]
     4ac: e0863003     	add	r3, r6, r3
     4b0: e58d3028     	str	r3, [sp, #0x28]
     4b4: eaffff6b     	b	0x268 <ZB_fillTriangleFlat_DT0_DW1+0x268> @ imm = #-0x254
     4b8: e59da034     	ldr	r10, [sp, #0x34]
     4bc: e1a0100c     	mov	r1, r12
     4c0: e1a00003     	mov	r0, r3
     4c4: eaffff2e     	b	0x184 <ZB_fillTriangleFlat_DT0_DW1+0x184> @ imm = #-0x348
     4c8: e3550001     	cmp	r5, #1
     4cc: 1affffc4     	bne	0x3e4 <ZB_fillTriangleFlat_DT0_DW1+0x3e4> @ imm = #-0xf0
     4d0: e59d2038     	ldr	r2, [sp, #0x38]
     4d4: e5939008     	ldr	r9, [r3, #0x8]
     4d8: ee183a90     	vmov	r3, s17
     4dc: e0861003     	add	r1, r6, r3
     4e0: e1a00002     	mov	r0, r2
     4e4: e58d201c     	str	r2, [sp, #0x1c]
     4e8: e58d202c     	str	r2, [sp, #0x2c]
     4ec: e58d5038     	str	r5, [sp, #0x38]
     4f0: e58d1028     	str	r1, [sp, #0x28]
     4f4: edcd8a0c     	vstr	s17, [sp, #48]
     4f8: e58d5024     	str	r5, [sp, #0x24]
     4fc: eaffff5b     	b	0x270 <ZB_fillTriangleFlat_DT0_DW1+0x270> @ imm = #-0x294

Disassembly of section .text.ZB_fillTriangleFlat_DT1_DW0:

00000000 <ZB_fillTriangleFlat_DT1_DW0>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a04003     	mov	r4, r3
       8: ed2d8b02     	vpush	{d8}
       c: e24dd05c     	sub	sp, sp, #92
      10: e592c004     	ldr	r12, [r2, #0x4]
      14: e58d0028     	str	r0, [sp, #0x28]
      18: e5910004     	ldr	r0, [r1, #0x4]
      1c: e593e004     	ldr	lr, [r3, #0x4]
      20: e15c0000     	cmp	r12, r0
      24: a1a05000     	movge	r5, r0
      28: b1a0500c     	movlt	r5, r12
      2c: a1a0000c     	movge	r0, r12
      30: b1a03002     	movlt	r3, r2
      34: a1a03001     	movge	r3, r1
      38: b1a09001     	movlt	r9, r1
      3c: a1a09002     	movge	r9, r2
      40: e15e0005     	cmp	lr, r5
      44: e1a0c005     	mov	r12, r5
      48: ba00000b     	blt	0x7c <ZB_fillTriangleFlat_DT1_DW0+0x7c> @ imm = #0x2c
      4c: e15e0000     	cmp	lr, r0
      50: e1a02004     	mov	r2, r4
      54: a1a0c000     	movge	r12, r0
      58: a1a04003     	movge	r4, r3
      5c: a1a0000e     	movge	r0, lr
      60: a1a03009     	movge	r3, r9
      64: b1a0c00e     	movlt	r12, lr
      68: b1a04003     	movlt	r4, r3
      6c: a1a0e005     	movge	lr, r5
      70: a1a09002     	movge	r9, r2
      74: b1a0e005     	movlt	lr, r5
      78: b1a03002     	movlt	r3, r2
      7c: e594a000     	ldr	r10, [r4]
      80: e040000e     	sub	r0, r0, lr
      84: e5992000     	ldr	r2, [r9]
      88: ee070a90     	vmov	s15, r0
      8c: eeb87ae7     	vcvt.f32.s32	s14, s15
      90: e042200a     	sub	r2, r2, r10
      94: e5945008     	ldr	r5, [r4, #0x8]
      98: e59d0028     	ldr	r0, [sp, #0x28]
      9c: ee072a90     	vmov	s15, r2
      a0: e04c200e     	sub	r2, r12, lr
      a4: e5991018     	ldr	r1, [r9, #0x18]
      a8: e1a0b005     	mov	r11, r5
      ac: ee062a90     	vmov	s13, r2
      b0: e5932000     	ldr	r2, [r3]
      b4: eef87ae7     	vcvt.f32.s32	s15, s15
      b8: eef86ae6     	vcvt.f32.s32	s13, s13
      bc: e1a01441     	asr	r1, r1, #8
      c0: e042200a     	sub	r2, r2, r10
      c4: e2011cff     	and	r1, r1, #65280
      c8: ee062a10     	vmov	s12, r2
      cc: ee268aa7     	vmul.f32	s16, s13, s15
      d0: eeb86ac6     	vcvt.f32.s32	s12, s12
      d4: e5992008     	ldr	r2, [r9, #0x8]
      d8: ee168a07     	vnmls.f32	s16, s12, s14
      dc: e0422005     	sub	r2, r2, r5
      e0: eeb58a40     	vcmp.f32	s16, #0
      e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
      e8: 1ef75a00     	vmovne.f32	s11, #1.000000e+00
      ec: 1e858a88     	vdivne.f32	s16, s11, s16
      f0: ee052a90     	vmov	s11, r2
      f4: e5932008     	ldr	r2, [r3, #0x8]
      f8: eef85ae5     	vcvt.f32.s32	s11, s11
      fc: e0422005     	sub	r2, r2, r5
     100: ee052a10     	vmov	s10, r2
     104: e5902020     	ldr	r2, [r0, #0x20]
     108: eeb85ac5     	vcvt.f32.s32	s10, s10
     10c: e0020e92     	mul	r2, r2, lr
     110: e1a0e000     	mov	lr, r0
     114: e5900004     	ldr	r0, [r0, #0x4]
     118: ee686a26     	vmul.f32	s13, s16, s13
     11c: ee687a27     	vmul.f32	s15, s16, s15
     120: ee287a07     	vmul.f32	s14, s16, s14
     124: e0800102     	add	r0, r0, r2, lsl #2
     128: ee286a06     	vmul.f32	s12, s16, s12
     12c: ee666aa5     	vmul.f32	s13, s13, s11
     130: ee677a85     	vmul.f32	s15, s15, s10
     134: e58d0004     	str	r0, [sp, #0x4]
     138: e59e0000     	ldr	r0, [lr]
     13c: ee576a05     	vnmls.f32	s13, s14, s10
     140: ee567a25     	vnmls.f32	s15, s12, s11
     144: eeb58ac0     	vcmpe.f32	s16, #0
     148: e0802082     	add	r2, r0, r2, lsl #1
     14c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     150: e58d2008     	str	r2, [sp, #0x8]
     154: e5d9201e     	ldrb	r2, [r9, #0x1e]
     158: eebd7ae6     	vcvt.s32.f32	s14, s13
     15c: eefd7ae7     	vcvt.s32.f32	s15, s15
     160: e1811002     	orr	r1, r1, r2
     164: e5992014     	ldr	r2, [r9, #0x14]
     168: ee178a10     	vmov	r8, s14
     16c: e20228ff     	and	r2, r2, #16711680
     170: eef08a67     	vmov.f32	s17, s15
     174: e1817002     	orr	r7, r1, r2
     178: e1a02108     	lsl	r2, r8, #2
     17c: e58d2030     	str	r2, [sp, #0x30]
     180: da0000da     	ble	0x4f0 <ZB_fillTriangleFlat_DT1_DW0+0x4f0> @ imm = #0x368
     184: e5991004     	ldr	r1, [r9, #0x4]
     188: e1a00009     	mov	r0, r9
     18c: e58d300c     	str	r3, [sp, #0xc]
     190: e5946004     	ldr	r6, [r4, #0x4]
     194: e0411006     	sub	r1, r1, r6
     198: e04c2006     	sub	r2, r12, r6
     19c: e3510000     	cmp	r1, #0
     1a0: da000014     	ble	0x1f8 <ZB_fillTriangleFlat_DT1_DW0+0x1f8> @ imm = #0x50
     1a4: e5900000     	ldr	r0, [r0]
     1a8: e1a0b005     	mov	r11, r5
     1ac: e58d3014     	str	r3, [sp, #0x14]
     1b0: e58d2010     	str	r2, [sp, #0x10]
     1b4: e040000a     	sub	r0, r0, r10
     1b8: e1a00800     	lsl	r0, r0, #16
     1bc: ebfffffe     	bl	0x1bc <ZB_fillTriangleFlat_DT1_DW0+0x1bc> @ imm = #-0x8
     1c0: e6ff2070     	uxth	r2, r0
     1c4: e1a03840     	asr	r3, r0, #16
     1c8: e58d202c     	str	r2, [sp, #0x2c]
     1cc: ee182a90     	vmov	r2, s17
     1d0: e58d303c     	str	r3, [sp, #0x3c]
     1d4: e0222893     	mla	r2, r3, r8, r2
     1d8: e2833001     	add	r3, r3, #1
     1dc: e58d3034     	str	r3, [sp, #0x34]
     1e0: e0883002     	add	r3, r8, r2
     1e4: e58d2040     	str	r2, [sp, #0x40]
     1e8: e58d3038     	str	r3, [sp, #0x38]
     1ec: e59d2010     	ldr	r2, [sp, #0x10]
     1f0: e59d3014     	ldr	r3, [sp, #0x14]
     1f4: ea000008     	b	0x21c <ZB_fillTriangleFlat_DT1_DW0+0x21c> @ imm = #0x20
     1f8: ee180a90     	vmov	r0, s17
     1fc: e3a01000     	mov	r1, #0
     200: e088c000     	add	r12, r8, r0
     204: e3a00001     	mov	r0, #1
     208: e58d102c     	str	r1, [sp, #0x2c]
     20c: e58dc038     	str	r12, [sp, #0x38]
     210: edcd8a10     	vstr	s17, [sp, #64]
     214: e58d0034     	str	r0, [sp, #0x34]
     218: e58d103c     	str	r1, [sp, #0x3c]
     21c: e59d000c     	ldr	r0, [sp, #0xc]
     220: e1a0c008     	mov	r12, r8
     224: e3a05000     	mov	r5, #0
     228: e1a0800a     	mov	r8, r10
     22c: e58d9054     	str	r9, [sp, #0x54]
     230: e1a0a00b     	mov	r10, r11
     234: e5901004     	ldr	r1, [r0, #0x4]
     238: e1a09005     	mov	r9, r5
     23c: e58d3050     	str	r3, [sp, #0x50]
     240: e1a0b00c     	mov	r11, r12
     244: e1a03000     	mov	r3, r0
     248: e0411006     	sub	r1, r1, r6
     24c: e3510000     	cmp	r1, #0
     250: e5944000     	ldr	r4, [r4]
     254: d3a03000     	movle	r3, #0
     258: d58d3024     	strle	r3, [sp, #0x24]
     25c: da000006     	ble	0x27c <ZB_fillTriangleFlat_DT1_DW0+0x27c> @ imm = #0x18
     260: e5933000     	ldr	r3, [r3]
     264: e58d200c     	str	r2, [sp, #0xc]
     268: e0433004     	sub	r3, r3, r4
     26c: e1a00803     	lsl	r0, r3, #16
     270: ebfffffe     	bl	0x270 <ZB_fillTriangleFlat_DT1_DW0+0x270> @ imm = #-0x8
     274: e59d200c     	ldr	r2, [sp, #0xc]
     278: e58d0024     	str	r0, [sp, #0x24]
     27c: e1a04804     	lsl	r4, r4, #16
     280: e1a0c009     	mov	r12, r9
     284: e1a09007     	mov	r9, r7
     288: e58d5048     	str	r5, [sp, #0x48]
     28c: e58d4044     	str	r4, [sp, #0x44]
     290: e1a04002     	mov	r4, r2
     294: e3540000     	cmp	r4, #0
     298: da000060     	ble	0x420 <ZB_fillTriangleFlat_DT1_DW0+0x420> @ imm = #0x180
     29c: e59d7044     	ldr	r7, [sp, #0x44]
     2a0: e1a06004     	mov	r6, r4
     2a4: e58d404c     	str	r4, [sp, #0x4c]
     2a8: e59d2008     	ldr	r2, [sp, #0x8]
     2ac: e068e847     	rsb	lr, r8, r7, asr #16
     2b0: e59d3004     	ldr	r3, [sp, #0x4]
     2b4: e2466001     	sub	r6, r6, #1
     2b8: e35e0002     	cmp	lr, #2
     2bc: e0821088     	add	r1, r2, r8, lsl #1
     2c0: e1a0200a     	mov	r2, r10
     2c4: e0833108     	add	r3, r3, r8, lsl #2
     2c8: da00002f     	ble	0x38c <ZB_fillTriangleFlat_DT1_DW0+0x38c> @ imm = #0xbc
     2cc: e24e4003     	sub	r4, lr, #3
     2d0: e58dc014     	str	r12, [sp, #0x14]
     2d4: e1a04124     	lsr	r4, r4, #2
     2d8: e2835010     	add	r5, r3, #16
     2dc: e2813008     	add	r3, r1, #8
     2e0: e58d8018     	str	r8, [sp, #0x18]
     2e4: e1a00184     	lsl	r0, r4, #3
     2e8: e2811010     	add	r1, r1, #16
     2ec: e1a02005     	mov	r2, r5
     2f0: e58d3010     	str	r3, [sp, #0x10]
     2f4: e0811000     	add	r1, r1, r0
     2f8: e58d000c     	str	r0, [sp, #0xc]
     2fc: e1a0000a     	mov	r0, r10
     300: e58da01c     	str	r10, [sp, #0x1c]
     304: e58d6020     	str	r6, [sp, #0x20]
     308: e153a0b8     	ldrh	r10, [r3, #-8]
     30c: e080800b     	add	r8, r0, r11
     310: e088600b     	add	r6, r8, r11
     314: e2833008     	add	r3, r3, #8
     318: e086c00b     	add	r12, r6, r11
     31c: e2822010     	add	r2, r2, #16
     320: e15a0720     	cmp	r10, r0, lsr #14
     324: e08c000b     	add	r0, r12, r11
     328: 95029020     	strls	r9, [r2, #-0x20]
     32c: e153a0be     	ldrh	r10, [r3, #-14]
     330: e15a0728     	cmp	r10, r8, lsr #14
     334: 9502901c     	strls	r9, [r2, #-0x1c]
     338: e15380bc     	ldrh	r8, [r3, #-12]
     33c: e1580726     	cmp	r8, r6, lsr #14
     340: 95029018     	strls	r9, [r2, #-0x18]
     344: e15360ba     	ldrh	r6, [r3, #-10]
     348: e156072c     	cmp	r6, r12, lsr #14
     34c: 95029014     	strls	r9, [r2, #-0x14]
     350: e1510003     	cmp	r1, r3
     354: 1affffeb     	bne	0x308 <ZB_fillTriangleFlat_DT1_DW0+0x308> @ imm = #-0x54
     358: e59d300c     	ldr	r3, [sp, #0xc]
     35c: e24ee004     	sub	lr, lr, #4
     360: e59d2010     	ldr	r2, [sp, #0x10]
     364: e04ee104     	sub	lr, lr, r4, lsl #2
     368: e59d0030     	ldr	r0, [sp, #0x30]
     36c: e59da01c     	ldr	r10, [sp, #0x1c]
     370: e0831002     	add	r1, r3, r2
     374: e59dc014     	ldr	r12, [sp, #0x14]
     378: e59d8018     	ldr	r8, [sp, #0x18]
     37c: e0853204     	add	r3, r5, r4, lsl #4
     380: e08a2000     	add	r2, r10, r0
     384: e59d6020     	ldr	r6, [sp, #0x20]
     388: e0222094     	mla	r2, r4, r0, r2
     38c: e35e0000     	cmp	lr, #0
     390: a081e08e     	addge	lr, r1, lr, lsl #1
     394: a2411002     	subge	r1, r1, #2
     398: ba000006     	blt	0x3b8 <ZB_fillTriangleFlat_DT1_DW0+0x3b8> @ imm = #0x18
     39c: e1f100b2     	ldrh	r0, [r1, #2]!
     3a0: e2833004     	add	r3, r3, #4
     3a4: e1500722     	cmp	r0, r2, lsr #14
     3a8: e082200b     	add	r2, r2, r11
     3ac: 95039004     	strls	r9, [r3, #-0x4]
     3b0: e15e0001     	cmp	lr, r1
     3b4: 1afffff8     	bne	0x39c <ZB_fillTriangleFlat_DT1_DW0+0x39c> @ imm = #-0x20
     3b8: e59d302c     	ldr	r3, [sp, #0x2c]
     3bc: e08cc003     	add	r12, r12, r3
     3c0: e35c0000     	cmp	r12, #0
     3c4: da00001b     	ble	0x438 <ZB_fillTriangleFlat_DT1_DW0+0x438> @ imm = #0x6c
     3c8: e59d3034     	ldr	r3, [sp, #0x34]
     3cc: e24cc801     	sub	r12, r12, #65536
     3d0: e0888003     	add	r8, r8, r3
     3d4: e59d3038     	ldr	r3, [sp, #0x38]
     3d8: e08aa003     	add	r10, r10, r3
     3dc: e59d3024     	ldr	r3, [sp, #0x24]
     3e0: e3560000     	cmp	r6, #0
     3e4: e59d2004     	ldr	r2, [sp, #0x4]
     3e8: e0877003     	add	r7, r7, r3
     3ec: e59d3028     	ldr	r3, [sp, #0x28]
     3f0: e5933020     	ldr	r3, [r3, #0x20]
     3f4: e0822103     	add	r2, r2, r3, lsl #2
     3f8: e58d2004     	str	r2, [sp, #0x4]
     3fc: e59d2008     	ldr	r2, [sp, #0x8]
     400: e0823083     	add	r3, r2, r3, lsl #1
     404: e58d3008     	str	r3, [sp, #0x8]
     408: 1affffa6     	bne	0x2a8 <ZB_fillTriangleFlat_DT1_DW0+0x2a8> @ imm = #-0x168
     40c: e59d3044     	ldr	r3, [sp, #0x44]
     410: e59d404c     	ldr	r4, [sp, #0x4c]
     414: e59d2024     	ldr	r2, [sp, #0x24]
     418: e0233294     	mla	r3, r4, r2, r3
     41c: e58d3044     	str	r3, [sp, #0x44]
     420: e59d3048     	ldr	r3, [sp, #0x48]
     424: e3530001     	cmp	r3, #1
     428: 1a000007     	bne	0x44c <ZB_fillTriangleFlat_DT1_DW0+0x44c> @ imm = #0x1c
     42c: e28dd05c     	add	sp, sp, #92
     430: ecbd8b02     	vpop	{d8}
     434: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     438: e59d303c     	ldr	r3, [sp, #0x3c]
     43c: e0888003     	add	r8, r8, r3
     440: e59d3040     	ldr	r3, [sp, #0x40]
     444: e08aa003     	add	r10, r10, r3
     448: eaffffe3     	b	0x3dc <ZB_fillTriangleFlat_DT1_DW0+0x3dc> @ imm = #-0x74
     44c: e59d3054     	ldr	r3, [sp, #0x54]
     450: e3a05001     	mov	r5, #1
     454: eeb58ac0     	vcmpe.f32	s16, #0
     458: eef1fa10     	vmrs	APSR_nzcv, fpscr
     45c: e5931004     	ldr	r1, [r3, #0x4]
     460: e59d3050     	ldr	r3, [sp, #0x50]
     464: e5933004     	ldr	r3, [r3, #0x4]
     468: e0411003     	sub	r1, r1, r3
     46c: e0814005     	add	r4, r1, r5
     470: da000005     	ble	0x48c <ZB_fillTriangleFlat_DT1_DW0+0x48c> @ imm = #0x14
     474: e1a02004     	mov	r2, r4
     478: e1a07009     	mov	r7, r9
     47c: e59d4050     	ldr	r4, [sp, #0x50]
     480: e1a0900c     	mov	r9, r12
     484: e59d3054     	ldr	r3, [sp, #0x54]
     488: eaffff6f     	b	0x24c <ZB_fillTriangleFlat_DT1_DW0+0x24c> @ imm = #-0x244
     48c: e59d3050     	ldr	r3, [sp, #0x50]
     490: e3510000     	cmp	r1, #0
     494: e5938000     	ldr	r8, [r3]
     498: da000018     	ble	0x500 <ZB_fillTriangleFlat_DT1_DW0+0x500> @ imm = #0x60
     49c: e59d3054     	ldr	r3, [sp, #0x54]
     4a0: e5930000     	ldr	r0, [r3]
     4a4: e0400008     	sub	r0, r0, r8
     4a8: e1a00800     	lsl	r0, r0, #16
     4ac: ebfffffe     	bl	0x4ac <ZB_fillTriangleFlat_DT1_DW0+0x4ac> @ imm = #-0x8
     4b0: e6ff2070     	uxth	r2, r0
     4b4: e1a03840     	asr	r3, r0, #16
     4b8: e59dc048     	ldr	r12, [sp, #0x48]
     4bc: e58d202c     	str	r2, [sp, #0x2c]
     4c0: e59d2050     	ldr	r2, [sp, #0x50]
     4c4: e58d303c     	str	r3, [sp, #0x3c]
     4c8: e58d5048     	str	r5, [sp, #0x48]
     4cc: e592a008     	ldr	r10, [r2, #0x8]
     4d0: ee182a90     	vmov	r2, s17
     4d4: e0222b93     	mla	r2, r3, r11, r2
     4d8: e2833001     	add	r3, r3, #1
     4dc: e58d3034     	str	r3, [sp, #0x34]
     4e0: e08b3002     	add	r3, r11, r2
     4e4: e58d2040     	str	r2, [sp, #0x40]
     4e8: e58d3038     	str	r3, [sp, #0x38]
     4ec: eaffff68     	b	0x294 <ZB_fillTriangleFlat_DT1_DW0+0x294> @ imm = #-0x260
     4f0: e1a0100c     	mov	r1, r12
     4f4: e1a00003     	mov	r0, r3
     4f8: e58d900c     	str	r9, [sp, #0xc]
     4fc: eaffff23     	b	0x190 <ZB_fillTriangleFlat_DT1_DW0+0x190> @ imm = #-0x374
     500: e3540001     	cmp	r4, #1
     504: 1affffc8     	bne	0x42c <ZB_fillTriangleFlat_DT1_DW0+0x42c> @ imm = #-0xe0
     508: e59d2048     	ldr	r2, [sp, #0x48]
     50c: e593a008     	ldr	r10, [r3, #0x8]
     510: ee183a90     	vmov	r3, s17
     514: e08b1003     	add	r1, r11, r3
     518: e1a0c002     	mov	r12, r2
     51c: e58d202c     	str	r2, [sp, #0x2c]
     520: e58d203c     	str	r2, [sp, #0x3c]
     524: e58d4048     	str	r4, [sp, #0x48]
     528: e58d1038     	str	r1, [sp, #0x38]
     52c: edcd8a10     	vstr	s17, [sp, #64]
     530: e58d4034     	str	r4, [sp, #0x34]
     534: eaffff58     	b	0x29c <ZB_fillTriangleFlat_DT1_DW0+0x29c> @ imm = #-0x2a0

Disassembly of section .text.ZB_fillTriangleFlat_DT1_DW1:

00000000 <ZB_fillTriangleFlat_DT1_DW1>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a05002     	mov	r5, r2
       8: ed2d8b02     	vpush	{d8}
       c: e24dd05c     	sub	sp, sp, #92
      10: e1a04003     	mov	r4, r3
      14: e592c004     	ldr	r12, [r2, #0x4]
      18: e58d0028     	str	r0, [sp, #0x28]
      1c: e5910004     	ldr	r0, [r1, #0x4]
      20: e593e004     	ldr	lr, [r3, #0x4]
      24: e15c0000     	cmp	r12, r0
      28: a1a06000     	movge	r6, r0
      2c: b1a0600c     	movlt	r6, r12
      30: a1a0000c     	movge	r0, r12
      34: a1a02001     	movge	r2, r1
      38: b1a03001     	movlt	r3, r1
      3c: a1a03005     	movge	r3, r5
      40: e15e0006     	cmp	lr, r6
      44: e1a0c006     	mov	r12, r6
      48: ba00000b     	blt	0x7c <ZB_fillTriangleFlat_DT1_DW1+0x7c> @ imm = #0x2c
      4c: e15e0000     	cmp	lr, r0
      50: e1a01004     	mov	r1, r4
      54: a1a0c000     	movge	r12, r0
      58: a1a04002     	movge	r4, r2
      5c: a1a0000e     	movge	r0, lr
      60: a1a02003     	movge	r2, r3
      64: b1a0c00e     	movlt	r12, lr
      68: b1a04002     	movlt	r4, r2
      6c: a1a0e006     	movge	lr, r6
      70: a1a03001     	movge	r3, r1
      74: b1a0e006     	movlt	lr, r6
      78: b1a02001     	movlt	r2, r1
      7c: e040000e     	sub	r0, r0, lr
      80: e5931000     	ldr	r1, [r3]
      84: e5948008     	ldr	r8, [r4, #0x8]
      88: ee070a90     	vmov	s15, r0
      8c: e5940000     	ldr	r0, [r4]
      90: eeb87ae7     	vcvt.f32.s32	s14, s15
      94: e1a0a008     	mov	r10, r8
      98: e59d5028     	ldr	r5, [sp, #0x28]
      9c: e0411000     	sub	r1, r1, r0
      a0: e58d0000     	str	r0, [sp]
      a4: ee071a90     	vmov	s15, r1
      a8: e04c100e     	sub	r1, r12, lr
      ac: ee061a90     	vmov	s13, r1
      b0: e5921000     	ldr	r1, [r2]
      b4: eef87ae7     	vcvt.f32.s32	s15, s15
      b8: eef86ae6     	vcvt.f32.s32	s13, s13
      bc: e0411000     	sub	r1, r1, r0
      c0: e5930018     	ldr	r0, [r3, #0x18]
      c4: ee061a10     	vmov	s12, r1
      c8: ee268aa7     	vmul.f32	s16, s13, s15
      cc: eeb86ac6     	vcvt.f32.s32	s12, s12
      d0: e1a00440     	asr	r0, r0, #8
      d4: e5931008     	ldr	r1, [r3, #0x8]
      d8: e2000cff     	and	r0, r0, #65280
      dc: ee168a07     	vnmls.f32	s16, s12, s14
      e0: e0411008     	sub	r1, r1, r8
      e4: eeb58a40     	vcmp.f32	s16, #0
      e8: eef1fa10     	vmrs	APSR_nzcv, fpscr
      ec: 1ef75a00     	vmovne.f32	s11, #1.000000e+00
      f0: 1e858a88     	vdivne.f32	s16, s11, s16
      f4: ee051a90     	vmov	s11, r1
      f8: e5921008     	ldr	r1, [r2, #0x8]
      fc: eef85ae5     	vcvt.f32.s32	s11, s11
     100: e0411008     	sub	r1, r1, r8
     104: ee051a10     	vmov	s10, r1
     108: e5951020     	ldr	r1, [r5, #0x20]
     10c: eeb85ac5     	vcvt.f32.s32	s10, s10
     110: e0010e91     	mul	r1, r1, lr
     114: e595e004     	ldr	lr, [r5, #0x4]
     118: ee686a26     	vmul.f32	s13, s16, s13
     11c: ee687a27     	vmul.f32	s15, s16, s15
     120: e08ee101     	add	lr, lr, r1, lsl #2
     124: ee287a07     	vmul.f32	s14, s16, s14
     128: ee286a06     	vmul.f32	s12, s16, s12
     12c: ee666aa5     	vmul.f32	s13, s13, s11
     130: ee677a85     	vmul.f32	s15, s15, s10
     134: e58de008     	str	lr, [sp, #0x8]
     138: e595e000     	ldr	lr, [r5]
     13c: ee576a05     	vnmls.f32	s13, s14, s10
     140: ee567a25     	vnmls.f32	s15, s12, s11
     144: eeb58ac0     	vcmpe.f32	s16, #0
     148: e08e1081     	add	r1, lr, r1, lsl #1
     14c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     150: e58d1004     	str	r1, [sp, #0x4]
     154: e5d3101e     	ldrb	r1, [r3, #0x1e]
     158: eebd7ae6     	vcvt.s32.f32	s14, s13
     15c: eefd7ae7     	vcvt.s32.f32	s15, s15
     160: e1800001     	orr	r0, r0, r1
     164: e5931014     	ldr	r1, [r3, #0x14]
     168: ee17ba10     	vmov	r11, s14
     16c: e20118ff     	and	r1, r1, #16711680
     170: eef08a67     	vmov.f32	s17, s15
     174: e1806001     	orr	r6, r0, r1
     178: e1a0110b     	lsl	r1, r11, #2
     17c: e58d1030     	str	r1, [sp, #0x30]
     180: da0000f0     	ble	0x548 <ZB_fillTriangleFlat_DT1_DW1+0x548> @ imm = #0x3c0
     184: e5931004     	ldr	r1, [r3, #0x4]
     188: e1a00003     	mov	r0, r3
     18c: e1a07002     	mov	r7, r2
     190: e5945004     	ldr	r5, [r4, #0x4]
     194: e0411005     	sub	r1, r1, r5
     198: e04cc005     	sub	r12, r12, r5
     19c: e3510000     	cmp	r1, #0
     1a0: e58dc044     	str	r12, [sp, #0x44]
     1a4: da000015     	ble	0x200 <ZB_fillTriangleFlat_DT1_DW1+0x200> @ imm = #0x54
     1a8: e58d300c     	str	r3, [sp, #0xc]
     1ac: e1a0a008     	mov	r10, r8
     1b0: e59d3000     	ldr	r3, [sp]
     1b4: e5900000     	ldr	r0, [r0]
     1b8: e58d2010     	str	r2, [sp, #0x10]
     1bc: e0400003     	sub	r0, r0, r3
     1c0: e1a00800     	lsl	r0, r0, #16
     1c4: ebfffffe     	bl	0x1c4 <ZB_fillTriangleFlat_DT1_DW1+0x1c4> @ imm = #-0x8
     1c8: e6ff2070     	uxth	r2, r0
     1cc: e1a03840     	asr	r3, r0, #16
     1d0: e58d202c     	str	r2, [sp, #0x2c]
     1d4: ee182a90     	vmov	r2, s17
     1d8: e58d303c     	str	r3, [sp, #0x3c]
     1dc: e0222b93     	mla	r2, r3, r11, r2
     1e0: e2833001     	add	r3, r3, #1
     1e4: e58d3034     	str	r3, [sp, #0x34]
     1e8: e08b3002     	add	r3, r11, r2
     1ec: e58d2040     	str	r2, [sp, #0x40]
     1f0: e58d3038     	str	r3, [sp, #0x38]
     1f4: e59d2010     	ldr	r2, [sp, #0x10]
     1f8: e59d300c     	ldr	r3, [sp, #0xc]
     1fc: ea000008     	b	0x224 <ZB_fillTriangleFlat_DT1_DW1+0x224> @ imm = #0x20
     200: ee180a90     	vmov	r0, s17
     204: e3a01000     	mov	r1, #0
     208: e08bc000     	add	r12, r11, r0
     20c: e3a00001     	mov	r0, #1
     210: e58d102c     	str	r1, [sp, #0x2c]
     214: e58dc038     	str	r12, [sp, #0x38]
     218: edcd8a10     	vstr	s17, [sp, #64]
     21c: e58d0034     	str	r0, [sp, #0x34]
     220: e58d103c     	str	r1, [sp, #0x3c]
     224: e5971004     	ldr	r1, [r7, #0x4]
     228: e3a09000     	mov	r9, #0
     22c: e59d8000     	ldr	r8, [sp]
     230: e58d2054     	str	r2, [sp, #0x54]
     234: e1a02007     	mov	r2, r7
     238: e0411005     	sub	r1, r1, r5
     23c: e1a0500a     	mov	r5, r10
     240: e1a0a006     	mov	r10, r6
     244: e3a06000     	mov	r6, #0
     248: e3510000     	cmp	r1, #0
     24c: e5944000     	ldr	r4, [r4]
     250: d3a02000     	movle	r2, #0
     254: d58d2024     	strle	r2, [sp, #0x24]
     258: da000006     	ble	0x278 <ZB_fillTriangleFlat_DT1_DW1+0x278> @ imm = #0x18
     25c: e5920000     	ldr	r0, [r2]
     260: e58d3000     	str	r3, [sp]
     264: e0400004     	sub	r0, r0, r4
     268: e1a00800     	lsl	r0, r0, #16
     26c: ebfffffe     	bl	0x26c <ZB_fillTriangleFlat_DT1_DW1+0x26c> @ imm = #-0x8
     270: e59d3000     	ldr	r3, [sp]
     274: e58d0024     	str	r0, [sp, #0x24]
     278: e1a04804     	lsl	r4, r4, #16
     27c: e1a00005     	mov	r0, r5
     280: e1a01009     	mov	r1, r9
     284: e58d6048     	str	r6, [sp, #0x48]
     288: e1a05008     	mov	r5, r8
     28c: e1a07003     	mov	r7, r3
     290: e59d3044     	ldr	r3, [sp, #0x44]
     294: e3530000     	cmp	r3, #0
     298: da000074     	ble	0x470 <ZB_fillTriangleFlat_DT1_DW1+0x470> @ imm = #0x1d0
     29c: e58d7050     	str	r7, [sp, #0x50]
     2a0: e1a08000     	mov	r8, r0
     2a4: e59d9044     	ldr	r9, [sp, #0x44]
     2a8: e1a00005     	mov	r0, r5
     2ac: e59d7008     	ldr	r7, [sp, #0x8]
     2b0: e1a05001     	mov	r5, r1
     2b4: e58d404c     	str	r4, [sp, #0x4c]
     2b8: e58d4000     	str	r4, [sp]
     2bc: e59d3000     	ldr	r3, [sp]
     2c0: e0872100     	add	r2, r7, r0, lsl #2
     2c4: e2499001     	sub	r9, r9, #1
     2c8: e0604843     	rsb	r4, r0, r3, asr #16
     2cc: e59d3004     	ldr	r3, [sp, #0x4]
     2d0: e3540002     	cmp	r4, #2
     2d4: e083e080     	add	lr, r3, r0, lsl #1
     2d8: e1a03008     	mov	r3, r8
     2dc: da000038     	ble	0x3c4 <ZB_fillTriangleFlat_DT1_DW1+0x3c4> @ imm = #0xe0
     2e0: e2446003     	sub	r6, r4, #3
     2e4: e2821010     	add	r1, r2, #16
     2e8: e1a06126     	lsr	r6, r6, #2
     2ec: e28e3008     	add	r3, lr, #8
     2f0: e28ee010     	add	lr, lr, #16
     2f4: e58d300c     	str	r3, [sp, #0xc]
     2f8: e1a02186     	lsl	r2, r6, #3
     2fc: e1a0c001     	mov	r12, r1
     300: e58d7010     	str	r7, [sp, #0x10]
     304: e08ee002     	add	lr, lr, r2
     308: e58d2008     	str	r2, [sp, #0x8]
     30c: e1a02008     	mov	r2, r8
     310: e58d5014     	str	r5, [sp, #0x14]
     314: e58d0018     	str	r0, [sp, #0x18]
     318: e58d801c     	str	r8, [sp, #0x1c]
     31c: e58d9020     	str	r9, [sp, #0x20]
     320: e15390b8     	ldrh	r9, [r3, #-8]
     324: e1a08722     	lsr	r8, r2, #14
     328: e082500b     	add	r5, r2, r11
     32c: e085000b     	add	r0, r5, r11
     330: e1a05725     	lsr	r5, r5, #14
     334: e1590008     	cmp	r9, r8
     338: 950ca010     	strls	r10, [r12, #-0x10]
     33c: e080200b     	add	r2, r0, r11
     340: 914380b8     	strhls	r8, [r3, #-8]
     344: e1a00720     	lsr	r0, r0, #14
     348: e15380b6     	ldrh	r8, [r3, #-6]
     34c: e1a07722     	lsr	r7, r2, #14
     350: e2833008     	add	r3, r3, #8
     354: e082200b     	add	r2, r2, r11
     358: e28cc010     	add	r12, r12, #16
     35c: e1580005     	cmp	r8, r5
     360: 950ca01c     	strls	r10, [r12, #-0x1c]
     364: 914350be     	strhls	r5, [r3, #-14]
     368: e15350bc     	ldrh	r5, [r3, #-12]
     36c: e1550000     	cmp	r5, r0
     370: 950ca018     	strls	r10, [r12, #-0x18]
     374: 914300bc     	strhls	r0, [r3, #-12]
     378: e15300ba     	ldrh	r0, [r3, #-10]
     37c: e1500007     	cmp	r0, r7
     380: 950ca014     	strls	r10, [r12, #-0x14]
     384: 914370ba     	strhls	r7, [r3, #-10]
     388: e15e0003     	cmp	lr, r3
     38c: 1affffe3     	bne	0x320 <ZB_fillTriangleFlat_DT1_DW1+0x320> @ imm = #-0x74
     390: e59d3008     	ldr	r3, [sp, #0x8]
     394: e28d0018     	add	r0, sp, #24
     398: e59d200c     	ldr	r2, [sp, #0xc]
     39c: e2444004     	sub	r4, r4, #4
     3a0: e8900301     	ldm	r0, {r0, r8, r9}
     3a4: e0444106     	sub	r4, r4, r6, lsl #2
     3a8: e083e002     	add	lr, r3, r2
     3ac: e0812206     	add	r2, r1, r6, lsl #4
     3b0: e59d1030     	ldr	r1, [sp, #0x30]
     3b4: e59d7010     	ldr	r7, [sp, #0x10]
     3b8: e59d5014     	ldr	r5, [sp, #0x14]
     3bc: e0883001     	add	r3, r8, r1
     3c0: e0233196     	mla	r3, r6, r1, r3
     3c4: e3540000     	cmp	r4, #0
     3c8: a08e4084     	addge	r4, lr, r4, lsl #1
     3cc: a24ee002     	subge	lr, lr, #2
     3d0: ba000008     	blt	0x3f8 <ZB_fillTriangleFlat_DT1_DW1+0x3f8> @ imm = #0x20
     3d4: e1fec0b2     	ldrh	r12, [lr, #2]!
     3d8: e1a01723     	lsr	r1, r3, #14
     3dc: e2822004     	add	r2, r2, #4
     3e0: e083300b     	add	r3, r3, r11
     3e4: e15c0001     	cmp	r12, r1
     3e8: 9502a004     	strls	r10, [r2, #-0x4]
     3ec: 91ce10b0     	strhls	r1, [lr]
     3f0: e154000e     	cmp	r4, lr
     3f4: 1afffff6     	bne	0x3d4 <ZB_fillTriangleFlat_DT1_DW1+0x3d4> @ imm = #-0x28
     3f8: e59d302c     	ldr	r3, [sp, #0x2c]
     3fc: e0855003     	add	r5, r5, r3
     400: e3550000     	cmp	r5, #0
     404: da00001f     	ble	0x488 <ZB_fillTriangleFlat_DT1_DW1+0x488> @ imm = #0x7c
     408: e59d3034     	ldr	r3, [sp, #0x34]
     40c: e2455801     	sub	r5, r5, #65536
     410: e0800003     	add	r0, r0, r3
     414: e59d3038     	ldr	r3, [sp, #0x38]
     418: e0888003     	add	r8, r8, r3
     41c: e59d3000     	ldr	r3, [sp]
     420: e3590000     	cmp	r9, #0
     424: e59d2024     	ldr	r2, [sp, #0x24]
     428: e0833002     	add	r3, r3, r2
     42c: e59d2004     	ldr	r2, [sp, #0x4]
     430: e58d3000     	str	r3, [sp]
     434: e59d3028     	ldr	r3, [sp, #0x28]
     438: e5933020     	ldr	r3, [r3, #0x20]
     43c: e0877103     	add	r7, r7, r3, lsl #2
     440: e0823083     	add	r3, r2, r3, lsl #1
     444: e58d3004     	str	r3, [sp, #0x4]
     448: 1affff9b     	bne	0x2bc <ZB_fillTriangleFlat_DT1_DW1+0x2bc> @ imm = #-0x194
     44c: e59d404c     	ldr	r4, [sp, #0x4c]
     450: e1a01005     	mov	r1, r5
     454: e59d3024     	ldr	r3, [sp, #0x24]
     458: e1a05000     	mov	r5, r0
     45c: e59d2044     	ldr	r2, [sp, #0x44]
     460: e1a00008     	mov	r0, r8
     464: e58d7008     	str	r7, [sp, #0x8]
     468: e59d7050     	ldr	r7, [sp, #0x50]
     46c: e0244392     	mla	r4, r2, r3, r4
     470: e59d3048     	ldr	r3, [sp, #0x48]
     474: e3530001     	cmp	r3, #1
     478: 1a000007     	bne	0x49c <ZB_fillTriangleFlat_DT1_DW1+0x49c> @ imm = #0x1c
     47c: e28dd05c     	add	sp, sp, #92
     480: ecbd8b02     	vpop	{d8}
     484: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     488: e59d303c     	ldr	r3, [sp, #0x3c]
     48c: e0800003     	add	r0, r0, r3
     490: e59d3040     	ldr	r3, [sp, #0x40]
     494: e0888003     	add	r8, r8, r3
     498: eaffffdf     	b	0x41c <ZB_fillTriangleFlat_DT1_DW1+0x41c> @ imm = #-0x84
     49c: e59d3054     	ldr	r3, [sp, #0x54]
     4a0: e3a06001     	mov	r6, #1
     4a4: e5972004     	ldr	r2, [r7, #0x4]
     4a8: eeb58ac0     	vcmpe.f32	s16, #0
     4ac: e5933004     	ldr	r3, [r3, #0x4]
     4b0: eef1fa10     	vmrs	APSR_nzcv, fpscr
     4b4: e0423003     	sub	r3, r2, r3
     4b8: e0832006     	add	r2, r3, r6
     4bc: e58d2044     	str	r2, [sp, #0x44]
     4c0: da000007     	ble	0x4e4 <ZB_fillTriangleFlat_DT1_DW1+0x4e4> @ imm = #0x1c
     4c4: e1a09001     	mov	r9, r1
     4c8: e1a08005     	mov	r8, r5
     4cc: e1a01003     	mov	r1, r3
     4d0: e59d4054     	ldr	r4, [sp, #0x54]
     4d4: e1a05000     	mov	r5, r0
     4d8: e1a03007     	mov	r3, r7
     4dc: e1a02007     	mov	r2, r7
     4e0: eaffff58     	b	0x248 <ZB_fillTriangleFlat_DT1_DW1+0x248> @ imm = #-0x2a0
     4e4: e59d2054     	ldr	r2, [sp, #0x54]
     4e8: e3530000     	cmp	r3, #0
     4ec: e5925000     	ldr	r5, [r2]
     4f0: da000018     	ble	0x558 <ZB_fillTriangleFlat_DT1_DW1+0x558> @ imm = #0x60
     4f4: e5970000     	ldr	r0, [r7]
     4f8: e1a01003     	mov	r1, r3
     4fc: e0400005     	sub	r0, r0, r5
     500: e1a00800     	lsl	r0, r0, #16
     504: ebfffffe     	bl	0x504 <ZB_fillTriangleFlat_DT1_DW1+0x504> @ imm = #-0x8
     508: e6ff2070     	uxth	r2, r0
     50c: e1a03840     	asr	r3, r0, #16
     510: e59d1048     	ldr	r1, [sp, #0x48]
     514: e58d202c     	str	r2, [sp, #0x2c]
     518: e59d2054     	ldr	r2, [sp, #0x54]
     51c: e58d303c     	str	r3, [sp, #0x3c]
     520: e58d6048     	str	r6, [sp, #0x48]
     524: e5920008     	ldr	r0, [r2, #0x8]
     528: ee182a90     	vmov	r2, s17
     52c: e0222b93     	mla	r2, r3, r11, r2
     530: e2833001     	add	r3, r3, #1
     534: e58d3034     	str	r3, [sp, #0x34]
     538: e08b3002     	add	r3, r11, r2
     53c: e58d2040     	str	r2, [sp, #0x40]
     540: e58d3038     	str	r3, [sp, #0x38]
     544: eaffff51     	b	0x290 <ZB_fillTriangleFlat_DT1_DW1+0x290> @ imm = #-0x2bc
     548: e1a0100c     	mov	r1, r12
     54c: e1a00002     	mov	r0, r2
     550: e1a07003     	mov	r7, r3
     554: eaffff0d     	b	0x190 <ZB_fillTriangleFlat_DT1_DW1+0x190> @ imm = #-0x3cc
     558: e59d3044     	ldr	r3, [sp, #0x44]
     55c: e3530001     	cmp	r3, #1
     560: 1affffc5     	bne	0x47c <ZB_fillTriangleFlat_DT1_DW1+0x47c> @ imm = #-0xec
     564: e59d1048     	ldr	r1, [sp, #0x48]
     568: e5920008     	ldr	r0, [r2, #0x8]
     56c: ee182a90     	vmov	r2, s17
     570: e08bc002     	add	r12, r11, r2
     574: e58d102c     	str	r1, [sp, #0x2c]
     578: e58d103c     	str	r1, [sp, #0x3c]
     57c: e58d3048     	str	r3, [sp, #0x48]
     580: e58dc038     	str	r12, [sp, #0x38]
     584: edcd8a10     	vstr	s17, [sp, #64]
     588: e58d3034     	str	r3, [sp, #0x34]
     58c: eaffff42     	b	0x29c <ZB_fillTriangleFlat_DT1_DW1+0x29c> @ imm = #-0x2f8

Disassembly of section .text.ZB_fillTriangleFlatNOBLEND_DT0_DW0:

00000000 <ZB_fillTriangleFlatNOBLEND_DT0_DW0>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a08003     	mov	r8, r3
       8: ed2d8b02     	vpush	{d8}
       c: e1a09000     	mov	r9, r0
      10: e24dd01c     	sub	sp, sp, #28
      14: e5933018     	ldr	r3, [r3, #0x18]
      18: e5920004     	ldr	r0, [r2, #0x4]
      1c: e598c004     	ldr	r12, [r8, #0x4]
      20: e1a04443     	asr	r4, r3, #8
      24: e5913004     	ldr	r3, [r1, #0x4]
      28: e2044cff     	and	r4, r4, #65280
      2c: e1500003     	cmp	r0, r3
      30: b1a0e000     	movlt	lr, r0
      34: a1a0e003     	movge	lr, r3
      38: a1a03000     	movge	r3, r0
      3c: e5d8001e     	ldrb	r0, [r8, #0x1e]
      40: b1a05002     	movlt	r5, r2
      44: a1a05001     	movge	r5, r1
      48: b1a0b001     	movlt	r11, r1
      4c: a1a0b002     	movge	r11, r2
      50: e1844000     	orr	r4, r4, r0
      54: e5980014     	ldr	r0, [r8, #0x14]
      58: e15c000e     	cmp	r12, lr
      5c: e58d5008     	str	r5, [sp, #0x8]
      60: e20008ff     	and	r0, r0, #16711680
      64: e1844000     	orr	r4, r4, r0
      68: e1a0000e     	mov	r0, lr
      6c: ba00000b     	blt	0xa0 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0xa0> @ imm = #0x2c
      70: e15c0003     	cmp	r12, r3
      74: e1a02008     	mov	r2, r8
      78: a1a00003     	movge	r0, r3
      7c: b1a0000c     	movlt	r0, r12
      80: a1a0300c     	movge	r3, r12
      84: a58db008     	strge	r11, [sp, #0x8]
      88: a1a08005     	movge	r8, r5
      8c: a1a0c00e     	movge	r12, lr
      90: a1a0b002     	movge	r11, r2
      94: b1a0c00e     	movlt	r12, lr
      98: b59d8008     	ldrlt	r8, [sp, #0x8]
      9c: b58d2008     	strlt	r2, [sp, #0x8]
      a0: e043300c     	sub	r3, r3, r12
      a4: e5986000     	ldr	r6, [r8]
      a8: e5997004     	ldr	r7, [r9, #0x4]
      ac: ee073a90     	vmov	s15, r3
      b0: e59b3000     	ldr	r3, [r11]
      b4: eef87ae7     	vcvt.f32.s32	s15, s15
      b8: e0433006     	sub	r3, r3, r6
      bc: ee083a10     	vmov	s16, r3
      c0: e040300c     	sub	r3, r0, r12
      c4: ee073a10     	vmov	s14, r3
      c8: e59d3008     	ldr	r3, [sp, #0x8]
      cc: eeb87ac7     	vcvt.f32.s32	s14, s14
      d0: eeb88ac8     	vcvt.f32.s32	s16, s16
      d4: e5933000     	ldr	r3, [r3]
      d8: e0433006     	sub	r3, r3, r6
      dc: ee288a07     	vmul.f32	s16, s16, s14
      e0: ee073a10     	vmov	s14, r3
      e4: e5993020     	ldr	r3, [r9, #0x20]
      e8: eeb87ac7     	vcvt.f32.s32	s14, s14
      ec: e00c0c93     	mul	r12, r3, r12
      f0: ee178a27     	vnmls.f32	s16, s14, s15
      f4: e087710c     	add	r7, r7, r12, lsl #2
      f8: eeb58a40     	vcmp.f32	s16, #0
      fc: eef1fa10     	vmrs	APSR_nzcv, fpscr
     100: 1ef77a00     	vmovne.f32	s15, #1.000000e+00
     104: 1e878a88     	vdivne.f32	s16, s15, s16
     108: eeb58ac0     	vcmpe.f32	s16, #0
     10c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     110: da000082     	ble	0x320 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x320> @ imm = #0x208
     114: e59b1004     	ldr	r1, [r11, #0x4]
     118: e1a0200b     	mov	r2, r11
     11c: e59d3008     	ldr	r3, [sp, #0x8]
     120: e5985004     	ldr	r5, [r8, #0x4]
     124: e0411005     	sub	r1, r1, r5
     128: e0400005     	sub	r0, r0, r5
     12c: e3510000     	cmp	r1, #0
     130: e58d0004     	str	r0, [sp, #0x4]
     134: da00000a     	ble	0x164 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x164> @ imm = #0x28
     138: e5922000     	ldr	r2, [r2]
     13c: e58d300c     	str	r3, [sp, #0xc]
     140: e0422006     	sub	r2, r2, r6
     144: e1a00802     	lsl	r0, r2, #16
     148: ebfffffe     	bl	0x148 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x148> @ imm = #-0x8
     14c: e1a03840     	asr	r3, r0, #16
     150: e6ffa070     	uxth	r10, r0
     154: e2832001     	add	r2, r3, #1
     158: e58d3000     	str	r3, [sp]
     15c: e59d300c     	ldr	r3, [sp, #0xc]
     160: ea000002     	b	0x170 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x170> @ imm = #0x8
     164: e3a02001     	mov	r2, #1
     168: e3a0a000     	mov	r10, #0
     16c: e58da000     	str	r10, [sp]
     170: e5931004     	ldr	r1, [r3, #0x4]
     174: e1a00003     	mov	r0, r3
     178: e58db014     	str	r11, [sp, #0x14]
     17c: e3a0b000     	mov	r11, #0
     180: e0411005     	sub	r1, r1, r5
     184: e3a05000     	mov	r5, #0
     188: e3510000     	cmp	r1, #0
     18c: e5983000     	ldr	r3, [r8]
     190: d3a08000     	movle	r8, #0
     194: da000008     	ble	0x1bc <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x1bc> @ imm = #0x20
     198: e5900000     	ldr	r0, [r0]
     19c: e58d2010     	str	r2, [sp, #0x10]
     1a0: e58d300c     	str	r3, [sp, #0xc]
     1a4: e0400003     	sub	r0, r0, r3
     1a8: e1a00800     	lsl	r0, r0, #16
     1ac: ebfffffe     	bl	0x1ac <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x1ac> @ imm = #-0x8
     1b0: e59d300c     	ldr	r3, [sp, #0xc]
     1b4: e1a08000     	mov	r8, r0
     1b8: e59d2010     	ldr	r2, [sp, #0x10]
     1bc: e58db00c     	str	r11, [sp, #0xc]
     1c0: e1a0b803     	lsl	r11, r3, #16
     1c4: e59d3004     	ldr	r3, [sp, #0x4]
     1c8: e3530000     	cmp	r3, #0
     1cc: da00002d     	ble	0x288 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x288> @ imm = #0xb4
     1d0: e59de004     	ldr	lr, [sp, #0x4]
     1d4: e1a0c00b     	mov	r12, r11
     1d8: e58db010     	str	r11, [sp, #0x10]
     1dc: e066084c     	rsb	r0, r6, r12, asr #16
     1e0: e0873106     	add	r3, r7, r6, lsl #2
     1e4: e3500002     	cmp	r0, #2
     1e8: e24ee001     	sub	lr, lr, #1
     1ec: da000010     	ble	0x234 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x234> @ imm = #0x40
     1f0: e283b010     	add	r11, r3, #16
     1f4: e1a01000     	mov	r1, r0
     1f8: e1a0300b     	mov	r3, r11
     1fc: e2411004     	sub	r1, r1, #4
     200: e5034010     	str	r4, [r3, #-0x10]
     204: e3510002     	cmp	r1, #2
     208: e503400c     	str	r4, [r3, #-0xc]
     20c: e5034008     	str	r4, [r3, #-0x8]
     210: e2833010     	add	r3, r3, #16
     214: e5034014     	str	r4, [r3, #-0x14]
     218: cafffff7     	bgt	0x1fc <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x1fc> @ imm = #-0x24
     21c: e2401003     	sub	r1, r0, #3
     220: e2400004     	sub	r0, r0, #4
     224: e1a03121     	lsr	r3, r1, #2
     228: e3c11003     	bic	r1, r1, #3
     22c: e0400001     	sub	r0, r0, r1
     230: e08b3203     	add	r3, r11, r3, lsl #4
     234: e3500000     	cmp	r0, #0
     238: a0830100     	addge	r0, r3, r0, lsl #2
     23c: a2433004     	subge	r3, r3, #4
     240: ba000002     	blt	0x250 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x250> @ imm = #0x8
     244: e5a34004     	str	r4, [r3, #0x4]!
     248: e1500003     	cmp	r0, r3
     24c: 1afffffc     	bne	0x244 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x244> @ imm = #-0x10
     250: e085500a     	add	r5, r5, r10
     254: e3550000     	cmp	r5, #0
     258: c2455801     	subgt	r5, r5, #65536
     25c: c0866002     	addgt	r6, r6, r2
     260: d59d3000     	ldrle	r3, [sp]
     264: d0866003     	addle	r6, r6, r3
     268: e5993020     	ldr	r3, [r9, #0x20]
     26c: e35e0000     	cmp	lr, #0
     270: e08cc008     	add	r12, r12, r8
     274: e0877103     	add	r7, r7, r3, lsl #2
     278: 1affffd7     	bne	0x1dc <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x1dc> @ imm = #-0xa4
     27c: e59db010     	ldr	r11, [sp, #0x10]
     280: e59d3004     	ldr	r3, [sp, #0x4]
     284: e02bb893     	mla	r11, r3, r8, r11
     288: e59d300c     	ldr	r3, [sp, #0xc]
     28c: e3530001     	cmp	r3, #1
     290: 1a000002     	bne	0x2a0 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x2a0> @ imm = #0x8
     294: e28dd01c     	add	sp, sp, #28
     298: ecbd8b02     	vpop	{d8}
     29c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     2a0: e59d3014     	ldr	r3, [sp, #0x14]
     2a4: eeb58ac0     	vcmpe.f32	s16, #0
     2a8: eef1fa10     	vmrs	APSR_nzcv, fpscr
     2ac: e5931004     	ldr	r1, [r3, #0x4]
     2b0: e59d3008     	ldr	r3, [sp, #0x8]
     2b4: e5933004     	ldr	r3, [r3, #0x4]
     2b8: e0411003     	sub	r1, r1, r3
     2bc: e3a03001     	mov	r3, #1
     2c0: e0810003     	add	r0, r1, r3
     2c4: e58d0004     	str	r0, [sp, #0x4]
     2c8: c59d0014     	ldrgt	r0, [sp, #0x14]
     2cc: c1a0b003     	movgt	r11, r3
     2d0: c59d8008     	ldrgt	r8, [sp, #0x8]
     2d4: caffffab     	bgt	0x188 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x188> @ imm = #-0x154
     2d8: e59d2008     	ldr	r2, [sp, #0x8]
     2dc: e3510000     	cmp	r1, #0
     2e0: e5926000     	ldr	r6, [r2]
     2e4: da000011     	ble	0x330 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x330> @ imm = #0x44
     2e8: e58d3000     	str	r3, [sp]
     2ec: e59d3014     	ldr	r3, [sp, #0x14]
     2f0: e5930000     	ldr	r0, [r3]
     2f4: e0400006     	sub	r0, r0, r6
     2f8: e1a00800     	lsl	r0, r0, #16
     2fc: ebfffffe     	bl	0x2fc <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x2fc> @ imm = #-0x8
     300: e59d3000     	ldr	r3, [sp]
     304: e6ffa070     	uxth	r10, r0
     308: e59d500c     	ldr	r5, [sp, #0xc]
     30c: e58d300c     	str	r3, [sp, #0xc]
     310: e1a03840     	asr	r3, r0, #16
     314: e2832001     	add	r2, r3, #1
     318: e58d3000     	str	r3, [sp]
     31c: eaffffa8     	b	0x1c4 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x1c4> @ imm = #-0x160
     320: e59d2008     	ldr	r2, [sp, #0x8]
     324: e1a01000     	mov	r1, r0
     328: e1a0300b     	mov	r3, r11
     32c: eaffff7b     	b	0x120 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x120> @ imm = #-0x214
     330: e59d3004     	ldr	r3, [sp, #0x4]
     334: e3530001     	cmp	r3, #1
     338: 1affffd5     	bne	0x294 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x294> @ imm = #-0xac
     33c: e59da00c     	ldr	r10, [sp, #0xc]
     340: e59d2004     	ldr	r2, [sp, #0x4]
     344: e1a0500a     	mov	r5, r10
     348: e58da000     	str	r10, [sp]
     34c: e58d200c     	str	r2, [sp, #0xc]
     350: eaffff9e     	b	0x1d0 <ZB_fillTriangleFlatNOBLEND_DT0_DW0+0x1d0> @ imm = #-0x188

Disassembly of section .text.ZB_fillTriangleFlatNOBLEND_DT0_DW1:

00000000 <ZB_fillTriangleFlatNOBLEND_DT0_DW1>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a04003     	mov	r4, r3
       8: ed2d8b02     	vpush	{d8}
       c: e24dd04c     	sub	sp, sp, #76
      10: e591c004     	ldr	r12, [r1, #0x4]
      14: e5933018     	ldr	r3, [r3, #0x18]
      18: e58d0018     	str	r0, [sp, #0x18]
      1c: e5920004     	ldr	r0, [r2, #0x4]
      20: e594e004     	ldr	lr, [r4, #0x4]
      24: e1a03443     	asr	r3, r3, #8
      28: e150000c     	cmp	r0, r12
      2c: b1a05000     	movlt	r5, r0
      30: a1a0500c     	movge	r5, r12
      34: a1a0c000     	movge	r12, r0
      38: e5d4001e     	ldrb	r0, [r4, #0x1e]
      3c: e2033cff     	and	r3, r3, #65280
      40: e1833000     	orr	r3, r3, r0
      44: e5940014     	ldr	r0, [r4, #0x14]
      48: e20008ff     	and	r0, r0, #16711680
      4c: e1838000     	orr	r8, r3, r0
      50: a1a03001     	movge	r3, r1
      54: b1a03002     	movlt	r3, r2
      58: a1a01002     	movge	r1, r2
      5c: e15e0005     	cmp	lr, r5
      60: e1a00005     	mov	r0, r5
      64: e58d1034     	str	r1, [sp, #0x34]
      68: ba000006     	blt	0x88 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x88> @ imm = #0x18
      6c: e15e000c     	cmp	lr, r12
      70: aa0000de     	bge	0x3f0 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x3f0> @ imm = #0x378
      74: e1a02004     	mov	r2, r4
      78: e1a0000e     	mov	r0, lr
      7c: e1a04003     	mov	r4, r3
      80: e1a0e005     	mov	lr, r5
      84: e1a03002     	mov	r3, r2
      88: e59d2034     	ldr	r2, [sp, #0x34]
      8c: e04cc00e     	sub	r12, r12, lr
      90: e5949000     	ldr	r9, [r4]
      94: ee07ca90     	vmov	s15, r12
      98: e5922000     	ldr	r2, [r2]
      9c: eeb87ae7     	vcvt.f32.s32	s14, s15
      a0: e59dc034     	ldr	r12, [sp, #0x34]
      a4: e0422009     	sub	r2, r2, r9
      a8: e5945008     	ldr	r5, [r4, #0x8]
      ac: e59d1018     	ldr	r1, [sp, #0x18]
      b0: ee072a90     	vmov	s15, r2
      b4: e040200e     	sub	r2, r0, lr
      b8: e1a0b005     	mov	r11, r5
      bc: ee062a90     	vmov	s13, r2
      c0: e5932000     	ldr	r2, [r3]
      c4: eef87ae7     	vcvt.f32.s32	s15, s15
      c8: eef86ae6     	vcvt.f32.s32	s13, s13
      cc: e0422009     	sub	r2, r2, r9
      d0: ee062a10     	vmov	s12, r2
      d4: ee268aa7     	vmul.f32	s16, s13, s15
      d8: eeb86ac6     	vcvt.f32.s32	s12, s12
      dc: e59c2008     	ldr	r2, [r12, #0x8]
      e0: ee168a07     	vnmls.f32	s16, s12, s14
      e4: e0422005     	sub	r2, r2, r5
      e8: eeb58a40     	vcmp.f32	s16, #0
      ec: eef1fa10     	vmrs	APSR_nzcv, fpscr
      f0: 1ef75a00     	vmovne.f32	s11, #1.000000e+00
      f4: 1e858a88     	vdivne.f32	s16, s11, s16
      f8: ee052a90     	vmov	s11, r2
      fc: e5932008     	ldr	r2, [r3, #0x8]
     100: eef85ae5     	vcvt.f32.s32	s11, s11
     104: e0422005     	sub	r2, r2, r5
     108: ee052a10     	vmov	s10, r2
     10c: e5912020     	ldr	r2, [r1, #0x20]
     110: eeb85ac5     	vcvt.f32.s32	s10, s10
     114: e00e0e92     	mul	lr, r2, lr
     118: e5912004     	ldr	r2, [r1, #0x4]
     11c: ee686a26     	vmul.f32	s13, s16, s13
     120: ee687a27     	vmul.f32	s15, s16, s15
     124: e082210e     	add	r2, r2, lr, lsl #2
     128: ee287a07     	vmul.f32	s14, s16, s14
     12c: ee286a06     	vmul.f32	s12, s16, s12
     130: ee666aa5     	vmul.f32	s13, s13, s11
     134: ee677a85     	vmul.f32	s15, s15, s10
     138: e58d2000     	str	r2, [sp]
     13c: e5912000     	ldr	r2, [r1]
     140: ee576a05     	vnmls.f32	s13, s14, s10
     144: ee567a25     	vnmls.f32	s15, s12, s11
     148: eeb58ac0     	vcmpe.f32	s16, #0
     14c: e082208e     	add	r2, r2, lr, lsl #1
     150: eef1fa10     	vmrs	APSR_nzcv, fpscr
     154: e58d2004     	str	r2, [sp, #0x4]
     158: eebd7ae6     	vcvt.s32.f32	s14, s13
     15c: eefd7ae7     	vcvt.s32.f32	s15, s15
     160: ee176a10     	vmov	r6, s14
     164: e1a02106     	lsl	r2, r6, #2
     168: eef08a67     	vmov.f32	s17, s15
     16c: e58d2020     	str	r2, [sp, #0x20]
     170: da0000d0     	ble	0x4b8 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x4b8> @ imm = #0x340
     174: e59c1004     	ldr	r1, [r12, #0x4]
     178: e1a0a003     	mov	r10, r3
     17c: e5947004     	ldr	r7, [r4, #0x4]
     180: e0411007     	sub	r1, r1, r7
     184: e0402007     	sub	r2, r0, r7
     188: e3510000     	cmp	r1, #0
     18c: da000014     	ble	0x1e4 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x1e4> @ imm = #0x50
     190: e59c0000     	ldr	r0, [r12]
     194: e1a0b005     	mov	r11, r5
     198: e58d300c     	str	r3, [sp, #0xc]
     19c: e58d2008     	str	r2, [sp, #0x8]
     1a0: e0400009     	sub	r0, r0, r9
     1a4: e1a00800     	lsl	r0, r0, #16
     1a8: ebfffffe     	bl	0x1a8 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x1a8> @ imm = #-0x8
     1ac: e6ff2070     	uxth	r2, r0
     1b0: e1a03840     	asr	r3, r0, #16
     1b4: e58d201c     	str	r2, [sp, #0x1c]
     1b8: ee182a90     	vmov	r2, s17
     1bc: e58d302c     	str	r3, [sp, #0x2c]
     1c0: e0222693     	mla	r2, r3, r6, r2
     1c4: e2833001     	add	r3, r3, #1
     1c8: e58d3024     	str	r3, [sp, #0x24]
     1cc: e0863002     	add	r3, r6, r2
     1d0: e58d2030     	str	r2, [sp, #0x30]
     1d4: e58d3028     	str	r3, [sp, #0x28]
     1d8: e59d2008     	ldr	r2, [sp, #0x8]
     1dc: e59d300c     	ldr	r3, [sp, #0xc]
     1e0: ea000008     	b	0x208 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x208> @ imm = #0x20
     1e4: ee180a90     	vmov	r0, s17
     1e8: e3a01000     	mov	r1, #0
     1ec: e086c000     	add	r12, r6, r0
     1f0: e3a00001     	mov	r0, #1
     1f4: e58d101c     	str	r1, [sp, #0x1c]
     1f8: e58dc028     	str	r12, [sp, #0x28]
     1fc: edcd8a0c     	vstr	s17, [sp, #48]
     200: e58d0024     	str	r0, [sp, #0x24]
     204: e58d102c     	str	r1, [sp, #0x2c]
     208: e59a1004     	ldr	r1, [r10, #0x4]
     20c: e1a0000a     	mov	r0, r10
     210: e1a0c00b     	mov	r12, r11
     214: e1a05002     	mov	r5, r2
     218: e1a0b009     	mov	r11, r9
     21c: e1a02000     	mov	r2, r0
     220: e0411007     	sub	r1, r1, r7
     224: e3a07000     	mov	r7, #0
     228: e1a0a007     	mov	r10, r7
     22c: e1a0900c     	mov	r9, r12
     230: e58d3044     	str	r3, [sp, #0x44]
     234: e3510000     	cmp	r1, #0
     238: e5944000     	ldr	r4, [r4]
     23c: d3a03000     	movle	r3, #0
     240: d58d3014     	strle	r3, [sp, #0x14]
     244: da000004     	ble	0x25c <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x25c> @ imm = #0x10
     248: e5922000     	ldr	r2, [r2]
     24c: e0422004     	sub	r2, r2, r4
     250: e1a00802     	lsl	r0, r2, #16
     254: ebfffffe     	bl	0x254 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x254> @ imm = #-0x8
     258: e58d0014     	str	r0, [sp, #0x14]
     25c: e1a04804     	lsl	r4, r4, #16
     260: e1a0000a     	mov	r0, r10
     264: e58d7038     	str	r7, [sp, #0x38]
     268: e3550000     	cmp	r5, #0
     26c: da000059     	ble	0x3d8 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x3d8> @ imm = #0x164
     270: e1a0a005     	mov	r10, r5
     274: e1a07004     	mov	r7, r4
     278: e58d503c     	str	r5, [sp, #0x3c]
     27c: e58d4040     	str	r4, [sp, #0x40]
     280: e59d3000     	ldr	r3, [sp]
     284: e06b4847     	rsb	r4, r11, r7, asr #16
     288: e24aa001     	sub	r10, r10, #1
     28c: e3540002     	cmp	r4, #2
     290: e083e10b     	add	lr, r3, r11, lsl #2
     294: e59d3004     	ldr	r3, [sp, #0x4]
     298: e083c08b     	add	r12, r3, r11, lsl #1
     29c: e1a03009     	mov	r3, r9
     2a0: da000029     	ble	0x34c <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x34c> @ imm = #0xa4
     2a4: e2445003     	sub	r5, r4, #3
     2a8: e28c2008     	add	r2, r12, #8
     2ac: e1a05125     	lsr	r5, r5, #2
     2b0: e28ee010     	add	lr, lr, #16
     2b4: e28cc010     	add	r12, r12, #16
     2b8: e58d200c     	str	r2, [sp, #0xc]
     2bc: e1a03185     	lsl	r3, r5, #3
     2c0: e1a0100e     	mov	r1, lr
     2c4: e58d0010     	str	r0, [sp, #0x10]
     2c8: e08cc003     	add	r12, r12, r3
     2cc: e58d3008     	str	r3, [sp, #0x8]
     2d0: e1a03009     	mov	r3, r9
     2d4: e0830006     	add	r0, r3, r6
     2d8: e1a03723     	lsr	r3, r3, #14
     2dc: e5018010     	str	r8, [r1, #-0x10]
     2e0: e2822008     	add	r2, r2, #8
     2e4: e14231b0     	strh	r3, [r2, #-16]
     2e8: e0803006     	add	r3, r0, r6
     2ec: e1a00720     	lsr	r0, r0, #14
     2f0: e501800c     	str	r8, [r1, #-0xc]
     2f4: e2811010     	add	r1, r1, #16
     2f8: e14200be     	strh	r0, [r2, #-14]
     2fc: e0830006     	add	r0, r3, r6
     300: e1a03723     	lsr	r3, r3, #14
     304: e5018018     	str	r8, [r1, #-0x18]
     308: e14230bc     	strh	r3, [r2, #-12]
     30c: e1a03720     	lsr	r3, r0, #14
     310: e5018014     	str	r8, [r1, #-0x14]
     314: e14230ba     	strh	r3, [r2, #-10]
     318: e15c0002     	cmp	r12, r2
     31c: e0803006     	add	r3, r0, r6
     320: 1affffeb     	bne	0x2d4 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x2d4> @ imm = #-0x54
     324: e59d3008     	ldr	r3, [sp, #0x8]
     328: e2444004     	sub	r4, r4, #4
     32c: e59d200c     	ldr	r2, [sp, #0xc]
     330: e0444105     	sub	r4, r4, r5, lsl #2
     334: e59d0010     	ldr	r0, [sp, #0x10]
     338: e08ee205     	add	lr, lr, r5, lsl #4
     33c: e083c002     	add	r12, r3, r2
     340: e59d2020     	ldr	r2, [sp, #0x20]
     344: e0893002     	add	r3, r9, r2
     348: e0233295     	mla	r3, r5, r2, r3
     34c: e3540000     	cmp	r4, #0
     350: a08e4104     	addge	r4, lr, r4, lsl #2
     354: a24ee004     	subge	lr, lr, #4
     358: ba000005     	blt	0x374 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x374> @ imm = #0x14
     35c: e1a02723     	lsr	r2, r3, #14
     360: e5ae8004     	str	r8, [lr, #0x4]!
     364: e154000e     	cmp	r4, lr
     368: e0833006     	add	r3, r3, r6
     36c: e0cc20b2     	strh	r2, [r12], #2
     370: 1afffff9     	bne	0x35c <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x35c> @ imm = #-0x1c
     374: e59d301c     	ldr	r3, [sp, #0x1c]
     378: e0800003     	add	r0, r0, r3
     37c: e3500000     	cmp	r0, #0
     380: da000021     	ble	0x40c <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x40c> @ imm = #0x84
     384: e59d3024     	ldr	r3, [sp, #0x24]
     388: e2400801     	sub	r0, r0, #65536
     38c: e08bb003     	add	r11, r11, r3
     390: e59d3028     	ldr	r3, [sp, #0x28]
     394: e0899003     	add	r9, r9, r3
     398: e59d3014     	ldr	r3, [sp, #0x14]
     39c: e35a0000     	cmp	r10, #0
     3a0: e59d2000     	ldr	r2, [sp]
     3a4: e0877003     	add	r7, r7, r3
     3a8: e59d3018     	ldr	r3, [sp, #0x18]
     3ac: e5933020     	ldr	r3, [r3, #0x20]
     3b0: e0822103     	add	r2, r2, r3, lsl #2
     3b4: e58d2000     	str	r2, [sp]
     3b8: e59d2004     	ldr	r2, [sp, #0x4]
     3bc: e0823083     	add	r3, r2, r3, lsl #1
     3c0: e58d3004     	str	r3, [sp, #0x4]
     3c4: 1affffad     	bne	0x280 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x280> @ imm = #-0x14c
     3c8: e59d503c     	ldr	r5, [sp, #0x3c]
     3cc: e59d4040     	ldr	r4, [sp, #0x40]
     3d0: e59d3014     	ldr	r3, [sp, #0x14]
     3d4: e0244395     	mla	r4, r5, r3, r4
     3d8: e59d3038     	ldr	r3, [sp, #0x38]
     3dc: e3530001     	cmp	r3, #1
     3e0: 1a00000e     	bne	0x420 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x420> @ imm = #0x38
     3e4: e28dd04c     	add	sp, sp, #76
     3e8: ecbd8b02     	vpop	{d8}
     3ec: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     3f0: e1a0000c     	mov	r0, r12
     3f4: e58d4034     	str	r4, [sp, #0x34]
     3f8: e1a0c00e     	mov	r12, lr
     3fc: e1a04003     	mov	r4, r3
     400: e1a0e005     	mov	lr, r5
     404: e1a03001     	mov	r3, r1
     408: eaffff1e     	b	0x88 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x88> @ imm = #-0x388
     40c: e59d302c     	ldr	r3, [sp, #0x2c]
     410: e08bb003     	add	r11, r11, r3
     414: e59d3030     	ldr	r3, [sp, #0x30]
     418: e0899003     	add	r9, r9, r3
     41c: eaffffdd     	b	0x398 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x398> @ imm = #-0x8c
     420: e59d3034     	ldr	r3, [sp, #0x34]
     424: e3a07001     	mov	r7, #1
     428: e59d2044     	ldr	r2, [sp, #0x44]
     42c: eeb58ac0     	vcmpe.f32	s16, #0
     430: e5933004     	ldr	r3, [r3, #0x4]
     434: e5921004     	ldr	r1, [r2, #0x4]
     438: eef1fa10     	vmrs	APSR_nzcv, fpscr
     43c: e0431001     	sub	r1, r3, r1
     440: e0815007     	add	r5, r1, r7
     444: c59d4044     	ldrgt	r4, [sp, #0x44]
     448: c1a0a000     	movgt	r10, r0
     44c: c59d2034     	ldrgt	r2, [sp, #0x34]
     450: caffff77     	bgt	0x234 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x234> @ imm = #-0x224
     454: e59d3044     	ldr	r3, [sp, #0x44]
     458: e3510000     	cmp	r1, #0
     45c: e593b000     	ldr	r11, [r3]
     460: da000018     	ble	0x4c8 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x4c8> @ imm = #0x60
     464: e59d3034     	ldr	r3, [sp, #0x34]
     468: e5930000     	ldr	r0, [r3]
     46c: e040000b     	sub	r0, r0, r11
     470: e1a00800     	lsl	r0, r0, #16
     474: ebfffffe     	bl	0x474 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x474> @ imm = #-0x8
     478: e6ff3070     	uxth	r3, r0
     47c: e1a02840     	asr	r2, r0, #16
     480: e59d0038     	ldr	r0, [sp, #0x38]
     484: e58d301c     	str	r3, [sp, #0x1c]
     488: e59d3044     	ldr	r3, [sp, #0x44]
     48c: e58d202c     	str	r2, [sp, #0x2c]
     490: e58d7038     	str	r7, [sp, #0x38]
     494: e5939008     	ldr	r9, [r3, #0x8]
     498: ee183a90     	vmov	r3, s17
     49c: e0233692     	mla	r3, r2, r6, r3
     4a0: e2822001     	add	r2, r2, #1
     4a4: e58d2024     	str	r2, [sp, #0x24]
     4a8: e58d3030     	str	r3, [sp, #0x30]
     4ac: e0863003     	add	r3, r6, r3
     4b0: e58d3028     	str	r3, [sp, #0x28]
     4b4: eaffff6b     	b	0x268 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x268> @ imm = #-0x254
     4b8: e59da034     	ldr	r10, [sp, #0x34]
     4bc: e1a01000     	mov	r1, r0
     4c0: e1a0c003     	mov	r12, r3
     4c4: eaffff2c     	b	0x17c <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x17c> @ imm = #-0x350
     4c8: e3550001     	cmp	r5, #1
     4cc: 1affffc4     	bne	0x3e4 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x3e4> @ imm = #-0xf0
     4d0: e59d2038     	ldr	r2, [sp, #0x38]
     4d4: e5939008     	ldr	r9, [r3, #0x8]
     4d8: ee183a90     	vmov	r3, s17
     4dc: e0861003     	add	r1, r6, r3
     4e0: e1a00002     	mov	r0, r2
     4e4: e58d201c     	str	r2, [sp, #0x1c]
     4e8: e58d202c     	str	r2, [sp, #0x2c]
     4ec: e58d5038     	str	r5, [sp, #0x38]
     4f0: e58d1028     	str	r1, [sp, #0x28]
     4f4: edcd8a0c     	vstr	s17, [sp, #48]
     4f8: e58d5024     	str	r5, [sp, #0x24]
     4fc: eaffff5b     	b	0x270 <ZB_fillTriangleFlatNOBLEND_DT0_DW1+0x270> @ imm = #-0x294

Disassembly of section .text.ZB_fillTriangleFlatNOBLEND_DT1_DW0:

00000000 <ZB_fillTriangleFlatNOBLEND_DT1_DW0>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a04003     	mov	r4, r3
       8: ed2d8b02     	vpush	{d8}
       c: e24dd05c     	sub	sp, sp, #92
      10: e591c004     	ldr	r12, [r1, #0x4]
      14: e5933018     	ldr	r3, [r3, #0x18]
      18: e58d0028     	str	r0, [sp, #0x28]
      1c: e5920004     	ldr	r0, [r2, #0x4]
      20: e594e004     	ldr	lr, [r4, #0x4]
      24: e1a03443     	asr	r3, r3, #8
      28: e150000c     	cmp	r0, r12
      2c: b1a05000     	movlt	r5, r0
      30: a1a0500c     	movge	r5, r12
      34: a1a0c000     	movge	r12, r0
      38: e5d4001e     	ldrb	r0, [r4, #0x1e]
      3c: e2033cff     	and	r3, r3, #65280
      40: b1a09001     	movlt	r9, r1
      44: a1a09002     	movge	r9, r2
      48: e1833000     	orr	r3, r3, r0
      4c: e5940014     	ldr	r0, [r4, #0x14]
      50: e20008ff     	and	r0, r0, #16711680
      54: e1837000     	orr	r7, r3, r0
      58: b1a03002     	movlt	r3, r2
      5c: a1a03001     	movge	r3, r1
      60: e15e0005     	cmp	lr, r5
      64: e1a00005     	mov	r0, r5
      68: ba00000b     	blt	0x9c <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x9c> @ imm = #0x2c
      6c: e15e000c     	cmp	lr, r12
      70: e1a02004     	mov	r2, r4
      74: a1a0000c     	movge	r0, r12
      78: a1a04003     	movge	r4, r3
      7c: a1a0c00e     	movge	r12, lr
      80: a1a03009     	movge	r3, r9
      84: b1a0000e     	movlt	r0, lr
      88: b1a04003     	movlt	r4, r3
      8c: a1a0e005     	movge	lr, r5
      90: a1a09002     	movge	r9, r2
      94: b1a0e005     	movlt	lr, r5
      98: b1a03002     	movlt	r3, r2
      9c: e594a000     	ldr	r10, [r4]
      a0: e04cc00e     	sub	r12, r12, lr
      a4: e5992000     	ldr	r2, [r9]
      a8: ee07ca90     	vmov	s15, r12
      ac: eeb87ae7     	vcvt.f32.s32	s14, s15
      b0: e042200a     	sub	r2, r2, r10
      b4: e5945008     	ldr	r5, [r4, #0x8]
      b8: e59d1028     	ldr	r1, [sp, #0x28]
      bc: ee072a90     	vmov	s15, r2
      c0: e040200e     	sub	r2, r0, lr
      c4: e1a0b005     	mov	r11, r5
      c8: ee062a90     	vmov	s13, r2
      cc: e5932000     	ldr	r2, [r3]
      d0: eef87ae7     	vcvt.f32.s32	s15, s15
      d4: eef86ae6     	vcvt.f32.s32	s13, s13
      d8: e042200a     	sub	r2, r2, r10
      dc: ee062a10     	vmov	s12, r2
      e0: ee268aa7     	vmul.f32	s16, s13, s15
      e4: eeb86ac6     	vcvt.f32.s32	s12, s12
      e8: e5992008     	ldr	r2, [r9, #0x8]
      ec: ee168a07     	vnmls.f32	s16, s12, s14
      f0: e0422005     	sub	r2, r2, r5
      f4: eeb58a40     	vcmp.f32	s16, #0
      f8: eef1fa10     	vmrs	APSR_nzcv, fpscr
      fc: 1ef75a00     	vmovne.f32	s11, #1.000000e+00
     100: 1e858a88     	vdivne.f32	s16, s11, s16
     104: ee052a90     	vmov	s11, r2
     108: e5932008     	ldr	r2, [r3, #0x8]
     10c: eef85ae5     	vcvt.f32.s32	s11, s11
     110: e0422005     	sub	r2, r2, r5
     114: ee052a10     	vmov	s10, r2
     118: e5912020     	ldr	r2, [r1, #0x20]
     11c: eeb85ac5     	vcvt.f32.s32	s10, s10
     120: e00e0e92     	mul	lr, r2, lr
     124: e5912004     	ldr	r2, [r1, #0x4]
     128: ee686a26     	vmul.f32	s13, s16, s13
     12c: ee687a27     	vmul.f32	s15, s16, s15
     130: e082210e     	add	r2, r2, lr, lsl #2
     134: ee287a07     	vmul.f32	s14, s16, s14
     138: ee286a06     	vmul.f32	s12, s16, s12
     13c: ee666aa5     	vmul.f32	s13, s13, s11
     140: ee677a85     	vmul.f32	s15, s15, s10
     144: e58d2004     	str	r2, [sp, #0x4]
     148: e5912000     	ldr	r2, [r1]
     14c: ee576a05     	vnmls.f32	s13, s14, s10
     150: ee567a25     	vnmls.f32	s15, s12, s11
     154: eeb58ac0     	vcmpe.f32	s16, #0
     158: e082208e     	add	r2, r2, lr, lsl #1
     15c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     160: e58d2008     	str	r2, [sp, #0x8]
     164: eebd7ae6     	vcvt.s32.f32	s14, s13
     168: eefd7ae7     	vcvt.s32.f32	s15, s15
     16c: ee178a10     	vmov	r8, s14
     170: e1a02108     	lsl	r2, r8, #2
     174: eef08a67     	vmov.f32	s17, s15
     178: e58d2030     	str	r2, [sp, #0x30]
     17c: da0000da     	ble	0x4ec <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x4ec> @ imm = #0x368
     180: e5991004     	ldr	r1, [r9, #0x4]
     184: e1a0c009     	mov	r12, r9
     188: e58d300c     	str	r3, [sp, #0xc]
     18c: e5946004     	ldr	r6, [r4, #0x4]
     190: e0411006     	sub	r1, r1, r6
     194: e0402006     	sub	r2, r0, r6
     198: e3510000     	cmp	r1, #0
     19c: da000014     	ble	0x1f4 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x1f4> @ imm = #0x50
     1a0: e59c0000     	ldr	r0, [r12]
     1a4: e1a0b005     	mov	r11, r5
     1a8: e58d3014     	str	r3, [sp, #0x14]
     1ac: e58d2010     	str	r2, [sp, #0x10]
     1b0: e040000a     	sub	r0, r0, r10
     1b4: e1a00800     	lsl	r0, r0, #16
     1b8: ebfffffe     	bl	0x1b8 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x1b8> @ imm = #-0x8
     1bc: e6ff2070     	uxth	r2, r0
     1c0: e1a03840     	asr	r3, r0, #16
     1c4: e58d202c     	str	r2, [sp, #0x2c]
     1c8: ee182a90     	vmov	r2, s17
     1cc: e58d303c     	str	r3, [sp, #0x3c]
     1d0: e0222893     	mla	r2, r3, r8, r2
     1d4: e2833001     	add	r3, r3, #1
     1d8: e58d3034     	str	r3, [sp, #0x34]
     1dc: e0883002     	add	r3, r8, r2
     1e0: e58d2040     	str	r2, [sp, #0x40]
     1e4: e58d3038     	str	r3, [sp, #0x38]
     1e8: e59d2010     	ldr	r2, [sp, #0x10]
     1ec: e59d3014     	ldr	r3, [sp, #0x14]
     1f0: ea000008     	b	0x218 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x218> @ imm = #0x20
     1f4: ee180a90     	vmov	r0, s17
     1f8: e3a01000     	mov	r1, #0
     1fc: e088c000     	add	r12, r8, r0
     200: e3a00001     	mov	r0, #1
     204: e58d102c     	str	r1, [sp, #0x2c]
     208: e58dc038     	str	r12, [sp, #0x38]
     20c: edcd8a10     	vstr	s17, [sp, #64]
     210: e58d0034     	str	r0, [sp, #0x34]
     214: e58d103c     	str	r1, [sp, #0x3c]
     218: e59d000c     	ldr	r0, [sp, #0xc]
     21c: e1a0c008     	mov	r12, r8
     220: e3a05000     	mov	r5, #0
     224: e1a0800a     	mov	r8, r10
     228: e58d9054     	str	r9, [sp, #0x54]
     22c: e1a0a00b     	mov	r10, r11
     230: e5901004     	ldr	r1, [r0, #0x4]
     234: e1a09005     	mov	r9, r5
     238: e58d3050     	str	r3, [sp, #0x50]
     23c: e1a0b00c     	mov	r11, r12
     240: e1a03000     	mov	r3, r0
     244: e0411006     	sub	r1, r1, r6
     248: e3510000     	cmp	r1, #0
     24c: e5944000     	ldr	r4, [r4]
     250: d3a03000     	movle	r3, #0
     254: d58d3024     	strle	r3, [sp, #0x24]
     258: da000006     	ble	0x278 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x278> @ imm = #0x18
     25c: e5933000     	ldr	r3, [r3]
     260: e58d200c     	str	r2, [sp, #0xc]
     264: e0433004     	sub	r3, r3, r4
     268: e1a00803     	lsl	r0, r3, #16
     26c: ebfffffe     	bl	0x26c <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x26c> @ imm = #-0x8
     270: e59d200c     	ldr	r2, [sp, #0xc]
     274: e58d0024     	str	r0, [sp, #0x24]
     278: e1a04804     	lsl	r4, r4, #16
     27c: e1a0c009     	mov	r12, r9
     280: e1a09007     	mov	r9, r7
     284: e58d5048     	str	r5, [sp, #0x48]
     288: e58d4044     	str	r4, [sp, #0x44]
     28c: e1a04002     	mov	r4, r2
     290: e3540000     	cmp	r4, #0
     294: da000060     	ble	0x41c <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x41c> @ imm = #0x180
     298: e59d7044     	ldr	r7, [sp, #0x44]
     29c: e1a06004     	mov	r6, r4
     2a0: e58d404c     	str	r4, [sp, #0x4c]
     2a4: e59d2008     	ldr	r2, [sp, #0x8]
     2a8: e068e847     	rsb	lr, r8, r7, asr #16
     2ac: e59d3004     	ldr	r3, [sp, #0x4]
     2b0: e2466001     	sub	r6, r6, #1
     2b4: e35e0002     	cmp	lr, #2
     2b8: e0821088     	add	r1, r2, r8, lsl #1
     2bc: e1a0200a     	mov	r2, r10
     2c0: e0833108     	add	r3, r3, r8, lsl #2
     2c4: da00002f     	ble	0x388 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x388> @ imm = #0xbc
     2c8: e24e4003     	sub	r4, lr, #3
     2cc: e58dc014     	str	r12, [sp, #0x14]
     2d0: e1a04124     	lsr	r4, r4, #2
     2d4: e2835010     	add	r5, r3, #16
     2d8: e2813008     	add	r3, r1, #8
     2dc: e58d8018     	str	r8, [sp, #0x18]
     2e0: e1a00184     	lsl	r0, r4, #3
     2e4: e2811010     	add	r1, r1, #16
     2e8: e1a02005     	mov	r2, r5
     2ec: e58d3010     	str	r3, [sp, #0x10]
     2f0: e0811000     	add	r1, r1, r0
     2f4: e58d000c     	str	r0, [sp, #0xc]
     2f8: e1a0000a     	mov	r0, r10
     2fc: e58da01c     	str	r10, [sp, #0x1c]
     300: e58d6020     	str	r6, [sp, #0x20]
     304: e153a0b8     	ldrh	r10, [r3, #-8]
     308: e080800b     	add	r8, r0, r11
     30c: e088600b     	add	r6, r8, r11
     310: e2833008     	add	r3, r3, #8
     314: e086c00b     	add	r12, r6, r11
     318: e2822010     	add	r2, r2, #16
     31c: e15a0720     	cmp	r10, r0, lsr #14
     320: e08c000b     	add	r0, r12, r11
     324: 95029020     	strls	r9, [r2, #-0x20]
     328: e153a0be     	ldrh	r10, [r3, #-14]
     32c: e15a0728     	cmp	r10, r8, lsr #14
     330: 9502901c     	strls	r9, [r2, #-0x1c]
     334: e15380bc     	ldrh	r8, [r3, #-12]
     338: e1580726     	cmp	r8, r6, lsr #14
     33c: 95029018     	strls	r9, [r2, #-0x18]
     340: e15360ba     	ldrh	r6, [r3, #-10]
     344: e156072c     	cmp	r6, r12, lsr #14
     348: 95029014     	strls	r9, [r2, #-0x14]
     34c: e1510003     	cmp	r1, r3
     350: 1affffeb     	bne	0x304 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x304> @ imm = #-0x54
     354: e59d300c     	ldr	r3, [sp, #0xc]
     358: e24ee004     	sub	lr, lr, #4
     35c: e59d2010     	ldr	r2, [sp, #0x10]
     360: e04ee104     	sub	lr, lr, r4, lsl #2
     364: e59d0030     	ldr	r0, [sp, #0x30]
     368: e59da01c     	ldr	r10, [sp, #0x1c]
     36c: e0831002     	add	r1, r3, r2
     370: e59dc014     	ldr	r12, [sp, #0x14]
     374: e59d8018     	ldr	r8, [sp, #0x18]
     378: e0853204     	add	r3, r5, r4, lsl #4
     37c: e08a2000     	add	r2, r10, r0
     380: e59d6020     	ldr	r6, [sp, #0x20]
     384: e0222094     	mla	r2, r4, r0, r2
     388: e35e0000     	cmp	lr, #0
     38c: a081e08e     	addge	lr, r1, lr, lsl #1
     390: a2411002     	subge	r1, r1, #2
     394: ba000006     	blt	0x3b4 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x3b4> @ imm = #0x18
     398: e1f100b2     	ldrh	r0, [r1, #2]!
     39c: e2833004     	add	r3, r3, #4
     3a0: e1500722     	cmp	r0, r2, lsr #14
     3a4: e082200b     	add	r2, r2, r11
     3a8: 95039004     	strls	r9, [r3, #-0x4]
     3ac: e15e0001     	cmp	lr, r1
     3b0: 1afffff8     	bne	0x398 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x398> @ imm = #-0x20
     3b4: e59d302c     	ldr	r3, [sp, #0x2c]
     3b8: e08cc003     	add	r12, r12, r3
     3bc: e35c0000     	cmp	r12, #0
     3c0: da00001b     	ble	0x434 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x434> @ imm = #0x6c
     3c4: e59d3034     	ldr	r3, [sp, #0x34]
     3c8: e24cc801     	sub	r12, r12, #65536
     3cc: e0888003     	add	r8, r8, r3
     3d0: e59d3038     	ldr	r3, [sp, #0x38]
     3d4: e08aa003     	add	r10, r10, r3
     3d8: e59d3024     	ldr	r3, [sp, #0x24]
     3dc: e3560000     	cmp	r6, #0
     3e0: e59d2004     	ldr	r2, [sp, #0x4]
     3e4: e0877003     	add	r7, r7, r3
     3e8: e59d3028     	ldr	r3, [sp, #0x28]
     3ec: e5933020     	ldr	r3, [r3, #0x20]
     3f0: e0822103     	add	r2, r2, r3, lsl #2
     3f4: e58d2004     	str	r2, [sp, #0x4]
     3f8: e59d2008     	ldr	r2, [sp, #0x8]
     3fc: e0823083     	add	r3, r2, r3, lsl #1
     400: e58d3008     	str	r3, [sp, #0x8]
     404: 1affffa6     	bne	0x2a4 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x2a4> @ imm = #-0x168
     408: e59d3044     	ldr	r3, [sp, #0x44]
     40c: e59d404c     	ldr	r4, [sp, #0x4c]
     410: e59d2024     	ldr	r2, [sp, #0x24]
     414: e0233294     	mla	r3, r4, r2, r3
     418: e58d3044     	str	r3, [sp, #0x44]
     41c: e59d3048     	ldr	r3, [sp, #0x48]
     420: e3530001     	cmp	r3, #1
     424: 1a000007     	bne	0x448 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x448> @ imm = #0x1c
     428: e28dd05c     	add	sp, sp, #92
     42c: ecbd8b02     	vpop	{d8}
     430: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     434: e59d303c     	ldr	r3, [sp, #0x3c]
     438: e0888003     	add	r8, r8, r3
     43c: e59d3040     	ldr	r3, [sp, #0x40]
     440: e08aa003     	add	r10, r10, r3
     444: eaffffe3     	b	0x3d8 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x3d8> @ imm = #-0x74
     448: e59d3054     	ldr	r3, [sp, #0x54]
     44c: e3a05001     	mov	r5, #1
     450: eeb58ac0     	vcmpe.f32	s16, #0
     454: eef1fa10     	vmrs	APSR_nzcv, fpscr
     458: e5931004     	ldr	r1, [r3, #0x4]
     45c: e59d3050     	ldr	r3, [sp, #0x50]
     460: e5933004     	ldr	r3, [r3, #0x4]
     464: e0411003     	sub	r1, r1, r3
     468: e0814005     	add	r4, r1, r5
     46c: da000005     	ble	0x488 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x488> @ imm = #0x14
     470: e1a02004     	mov	r2, r4
     474: e1a07009     	mov	r7, r9
     478: e59d4050     	ldr	r4, [sp, #0x50]
     47c: e1a0900c     	mov	r9, r12
     480: e59d3054     	ldr	r3, [sp, #0x54]
     484: eaffff6f     	b	0x248 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x248> @ imm = #-0x244
     488: e59d3050     	ldr	r3, [sp, #0x50]
     48c: e3510000     	cmp	r1, #0
     490: e5938000     	ldr	r8, [r3]
     494: da000018     	ble	0x4fc <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x4fc> @ imm = #0x60
     498: e59d3054     	ldr	r3, [sp, #0x54]
     49c: e5930000     	ldr	r0, [r3]
     4a0: e0400008     	sub	r0, r0, r8
     4a4: e1a00800     	lsl	r0, r0, #16
     4a8: ebfffffe     	bl	0x4a8 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x4a8> @ imm = #-0x8
     4ac: e6ff2070     	uxth	r2, r0
     4b0: e1a03840     	asr	r3, r0, #16
     4b4: e59dc048     	ldr	r12, [sp, #0x48]
     4b8: e58d202c     	str	r2, [sp, #0x2c]
     4bc: e59d2050     	ldr	r2, [sp, #0x50]
     4c0: e58d303c     	str	r3, [sp, #0x3c]
     4c4: e58d5048     	str	r5, [sp, #0x48]
     4c8: e592a008     	ldr	r10, [r2, #0x8]
     4cc: ee182a90     	vmov	r2, s17
     4d0: e0222b93     	mla	r2, r3, r11, r2
     4d4: e2833001     	add	r3, r3, #1
     4d8: e58d3034     	str	r3, [sp, #0x34]
     4dc: e08b3002     	add	r3, r11, r2
     4e0: e58d2040     	str	r2, [sp, #0x40]
     4e4: e58d3038     	str	r3, [sp, #0x38]
     4e8: eaffff68     	b	0x290 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x290> @ imm = #-0x260
     4ec: e1a01000     	mov	r1, r0
     4f0: e1a0c003     	mov	r12, r3
     4f4: e58d900c     	str	r9, [sp, #0xc]
     4f8: eaffff23     	b	0x18c <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x18c> @ imm = #-0x374
     4fc: e3540001     	cmp	r4, #1
     500: 1affffc8     	bne	0x428 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x428> @ imm = #-0xe0
     504: e59d2048     	ldr	r2, [sp, #0x48]
     508: e593a008     	ldr	r10, [r3, #0x8]
     50c: ee183a90     	vmov	r3, s17
     510: e08b1003     	add	r1, r11, r3
     514: e1a0c002     	mov	r12, r2
     518: e58d202c     	str	r2, [sp, #0x2c]
     51c: e58d203c     	str	r2, [sp, #0x3c]
     520: e58d4048     	str	r4, [sp, #0x48]
     524: e58d1038     	str	r1, [sp, #0x38]
     528: edcd8a10     	vstr	s17, [sp, #64]
     52c: e58d4034     	str	r4, [sp, #0x34]
     530: eaffff58     	b	0x298 <ZB_fillTriangleFlatNOBLEND_DT1_DW0+0x298> @ imm = #-0x2a0

Disassembly of section .text.ZB_fillTriangleFlatNOBLEND_DT1_DW1:

00000000 <ZB_fillTriangleFlatNOBLEND_DT1_DW1>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a04003     	mov	r4, r3
       8: ed2d8b02     	vpush	{d8}
       c: e24dd05c     	sub	sp, sp, #92
      10: e1a05002     	mov	r5, r2
      14: e5933018     	ldr	r3, [r3, #0x18]
      18: e58d0028     	str	r0, [sp, #0x28]
      1c: e5920004     	ldr	r0, [r2, #0x4]
      20: e5d4201e     	ldrb	r2, [r4, #0x1e]
      24: e1a03443     	asr	r3, r3, #8
      28: e591c004     	ldr	r12, [r1, #0x4]
      2c: e594e004     	ldr	lr, [r4, #0x4]
      30: e2033cff     	and	r3, r3, #65280
      34: e1833002     	orr	r3, r3, r2
      38: e5942014     	ldr	r2, [r4, #0x14]
      3c: e150000c     	cmp	r0, r12
      40: a1a0700c     	movge	r7, r12
      44: b1a07000     	movlt	r7, r0
      48: e20228ff     	and	r2, r2, #16711680
      4c: a1a0c000     	movge	r12, r0
      50: e1836002     	orr	r6, r3, r2
      54: b1a02005     	movlt	r2, r5
      58: a1a02001     	movge	r2, r1
      5c: b1a03001     	movlt	r3, r1
      60: a1a03005     	movge	r3, r5
      64: e15e0007     	cmp	lr, r7
      68: e1a00007     	mov	r0, r7
      6c: ba00000b     	blt	0xa0 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0xa0> @ imm = #0x2c
      70: e15e000c     	cmp	lr, r12
      74: e1a01004     	mov	r1, r4
      78: a1a0000c     	movge	r0, r12
      7c: a1a04002     	movge	r4, r2
      80: a1a0c00e     	movge	r12, lr
      84: a1a02003     	movge	r2, r3
      88: b1a0000e     	movlt	r0, lr
      8c: b1a04002     	movlt	r4, r2
      90: a1a0e007     	movge	lr, r7
      94: a1a03001     	movge	r3, r1
      98: b1a0e007     	movlt	lr, r7
      9c: b1a02001     	movlt	r2, r1
      a0: e04cc00e     	sub	r12, r12, lr
      a4: e5931000     	ldr	r1, [r3]
      a8: e5948008     	ldr	r8, [r4, #0x8]
      ac: ee07ca90     	vmov	s15, r12
      b0: e594c000     	ldr	r12, [r4]
      b4: eeb87ae7     	vcvt.f32.s32	s14, s15
      b8: e1a0a008     	mov	r10, r8
      bc: e041100c     	sub	r1, r1, r12
      c0: e58dc008     	str	r12, [sp, #0x8]
      c4: ee071a90     	vmov	s15, r1
      c8: e040100e     	sub	r1, r0, lr
      cc: ee061a90     	vmov	s13, r1
      d0: e5921000     	ldr	r1, [r2]
      d4: eef87ae7     	vcvt.f32.s32	s15, s15
      d8: eef86ae6     	vcvt.f32.s32	s13, s13
      dc: e041100c     	sub	r1, r1, r12
      e0: e59dc028     	ldr	r12, [sp, #0x28]
      e4: ee061a10     	vmov	s12, r1
      e8: ee268aa7     	vmul.f32	s16, s13, s15
      ec: eeb86ac6     	vcvt.f32.s32	s12, s12
      f0: e5931008     	ldr	r1, [r3, #0x8]
      f4: ee168a07     	vnmls.f32	s16, s12, s14
      f8: e0411008     	sub	r1, r1, r8
      fc: eeb58a40     	vcmp.f32	s16, #0
     100: eef1fa10     	vmrs	APSR_nzcv, fpscr
     104: 1ef75a00     	vmovne.f32	s11, #1.000000e+00
     108: 1e858a88     	vdivne.f32	s16, s11, s16
     10c: ee051a90     	vmov	s11, r1
     110: e5921008     	ldr	r1, [r2, #0x8]
     114: eef85ae5     	vcvt.f32.s32	s11, s11
     118: e0411008     	sub	r1, r1, r8
     11c: ee051a10     	vmov	s10, r1
     120: e59c1020     	ldr	r1, [r12, #0x20]
     124: eeb85ac5     	vcvt.f32.s32	s10, s10
     128: e00e0e91     	mul	lr, r1, lr
     12c: e59c1004     	ldr	r1, [r12, #0x4]
     130: ee686a26     	vmul.f32	s13, s16, s13
     134: ee687a27     	vmul.f32	s15, s16, s15
     138: e081110e     	add	r1, r1, lr, lsl #2
     13c: ee287a07     	vmul.f32	s14, s16, s14
     140: ee286a06     	vmul.f32	s12, s16, s12
     144: ee666aa5     	vmul.f32	s13, s13, s11
     148: ee677a85     	vmul.f32	s15, s15, s10
     14c: e58d1000     	str	r1, [sp]
     150: e59c1000     	ldr	r1, [r12]
     154: ee576a05     	vnmls.f32	s13, s14, s10
     158: ee567a25     	vnmls.f32	s15, s12, s11
     15c: eeb58ac0     	vcmpe.f32	s16, #0
     160: e081108e     	add	r1, r1, lr, lsl #1
     164: eef1fa10     	vmrs	APSR_nzcv, fpscr
     168: e58d1004     	str	r1, [sp, #0x4]
     16c: eebd7ae6     	vcvt.s32.f32	s14, s13
     170: eefd7ae7     	vcvt.s32.f32	s15, s15
     174: ee17ba10     	vmov	r11, s14
     178: e1a0110b     	lsl	r1, r11, #2
     17c: eef08a67     	vmov.f32	s17, s15
     180: e58d1030     	str	r1, [sp, #0x30]
     184: da0000ef     	ble	0x548 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x548> @ imm = #0x3bc
     188: e5931004     	ldr	r1, [r3, #0x4]
     18c: e1a0c003     	mov	r12, r3
     190: e1a07002     	mov	r7, r2
     194: e5945004     	ldr	r5, [r4, #0x4]
     198: e0411005     	sub	r1, r1, r5
     19c: e0400005     	sub	r0, r0, r5
     1a0: e3510000     	cmp	r1, #0
     1a4: e58d0044     	str	r0, [sp, #0x44]
     1a8: da000015     	ble	0x204 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x204> @ imm = #0x54
     1ac: e59c0000     	ldr	r0, [r12]
     1b0: e1a0a008     	mov	r10, r8
     1b4: e58d300c     	str	r3, [sp, #0xc]
     1b8: e59d3008     	ldr	r3, [sp, #0x8]
     1bc: e58d2010     	str	r2, [sp, #0x10]
     1c0: e0400003     	sub	r0, r0, r3
     1c4: e1a00800     	lsl	r0, r0, #16
     1c8: ebfffffe     	bl	0x1c8 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x1c8> @ imm = #-0x8
     1cc: e6ff2070     	uxth	r2, r0
     1d0: e1a03840     	asr	r3, r0, #16
     1d4: e58d202c     	str	r2, [sp, #0x2c]
     1d8: ee182a90     	vmov	r2, s17
     1dc: e58d303c     	str	r3, [sp, #0x3c]
     1e0: e0222b93     	mla	r2, r3, r11, r2
     1e4: e2833001     	add	r3, r3, #1
     1e8: e58d3034     	str	r3, [sp, #0x34]
     1ec: e08b3002     	add	r3, r11, r2
     1f0: e58d2040     	str	r2, [sp, #0x40]
     1f4: e58d3038     	str	r3, [sp, #0x38]
     1f8: e59d2010     	ldr	r2, [sp, #0x10]
     1fc: e59d300c     	ldr	r3, [sp, #0xc]
     200: ea000008     	b	0x228 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x228> @ imm = #0x20
     204: ee180a90     	vmov	r0, s17
     208: e3a01000     	mov	r1, #0
     20c: e08bc000     	add	r12, r11, r0
     210: e3a00001     	mov	r0, #1
     214: e58d102c     	str	r1, [sp, #0x2c]
     218: e58dc038     	str	r12, [sp, #0x38]
     21c: edcd8a10     	vstr	s17, [sp, #64]
     220: e58d0034     	str	r0, [sp, #0x34]
     224: e58d103c     	str	r1, [sp, #0x3c]
     228: e5971004     	ldr	r1, [r7, #0x4]
     22c: e1a00006     	mov	r0, r6
     230: e59d8008     	ldr	r8, [sp, #0x8]
     234: e3a09000     	mov	r9, #0
     238: e3a06000     	mov	r6, #0
     23c: e58d2054     	str	r2, [sp, #0x54]
     240: e0411005     	sub	r1, r1, r5
     244: e1a0500a     	mov	r5, r10
     248: e1a0a000     	mov	r10, r0
     24c: e3510000     	cmp	r1, #0
     250: e5944000     	ldr	r4, [r4]
     254: d3a02000     	movle	r2, #0
     258: d58d2024     	strle	r2, [sp, #0x24]
     25c: da000006     	ble	0x27c <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x27c> @ imm = #0x18
     260: e5970000     	ldr	r0, [r7]
     264: e58d3008     	str	r3, [sp, #0x8]
     268: e0400004     	sub	r0, r0, r4
     26c: e1a00800     	lsl	r0, r0, #16
     270: ebfffffe     	bl	0x270 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x270> @ imm = #-0x8
     274: e59d3008     	ldr	r3, [sp, #0x8]
     278: e58d0024     	str	r0, [sp, #0x24]
     27c: e1a04804     	lsl	r4, r4, #16
     280: e1a00005     	mov	r0, r5
     284: e1a01009     	mov	r1, r9
     288: e58d6048     	str	r6, [sp, #0x48]
     28c: e1a05008     	mov	r5, r8
     290: e1a07003     	mov	r7, r3
     294: e59d3044     	ldr	r3, [sp, #0x44]
     298: e3530000     	cmp	r3, #0
     29c: da000074     	ble	0x474 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x474> @ imm = #0x1d0
     2a0: e59d8044     	ldr	r8, [sp, #0x44]
     2a4: e1a09004     	mov	r9, r4
     2a8: e58d7050     	str	r7, [sp, #0x50]
     2ac: e58d404c     	str	r4, [sp, #0x4c]
     2b0: e1a07008     	mov	r7, r8
     2b4: e1a08000     	mov	r8, r0
     2b8: e1a00005     	mov	r0, r5
     2bc: e1a05001     	mov	r5, r1
     2c0: e59d3000     	ldr	r3, [sp]
     2c4: e0604849     	rsb	r4, r0, r9, asr #16
     2c8: e2477001     	sub	r7, r7, #1
     2cc: e3540002     	cmp	r4, #2
     2d0: e0832100     	add	r2, r3, r0, lsl #2
     2d4: e59d3004     	ldr	r3, [sp, #0x4]
     2d8: e083e080     	add	lr, r3, r0, lsl #1
     2dc: e1a03008     	mov	r3, r8
     2e0: da000039     	ble	0x3cc <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x3cc> @ imm = #0xe4
     2e4: e2446003     	sub	r6, r4, #3
     2e8: e2821010     	add	r1, r2, #16
     2ec: e1a06126     	lsr	r6, r6, #2
     2f0: e28e3008     	add	r3, lr, #8
     2f4: e28ee010     	add	lr, lr, #16
     2f8: e58d300c     	str	r3, [sp, #0xc]
     2fc: e1a02186     	lsl	r2, r6, #3
     300: e1a0c001     	mov	r12, r1
     304: e58d5010     	str	r5, [sp, #0x10]
     308: e08ee002     	add	lr, lr, r2
     30c: e58d2008     	str	r2, [sp, #0x8]
     310: e1a02008     	mov	r2, r8
     314: e58d0014     	str	r0, [sp, #0x14]
     318: e58d8018     	str	r8, [sp, #0x18]
     31c: e58d701c     	str	r7, [sp, #0x1c]
     320: e58d9020     	str	r9, [sp, #0x20]
     324: e15390b8     	ldrh	r9, [r3, #-8]
     328: e1a08722     	lsr	r8, r2, #14
     32c: e082500b     	add	r5, r2, r11
     330: e085000b     	add	r0, r5, r11
     334: e1a05725     	lsr	r5, r5, #14
     338: e1590008     	cmp	r9, r8
     33c: 950ca010     	strls	r10, [r12, #-0x10]
     340: e080200b     	add	r2, r0, r11
     344: 914380b8     	strhls	r8, [r3, #-8]
     348: e1a00720     	lsr	r0, r0, #14
     34c: e15380b6     	ldrh	r8, [r3, #-6]
     350: e1a07722     	lsr	r7, r2, #14
     354: e2833008     	add	r3, r3, #8
     358: e082200b     	add	r2, r2, r11
     35c: e28cc010     	add	r12, r12, #16
     360: e1580005     	cmp	r8, r5
     364: 950ca01c     	strls	r10, [r12, #-0x1c]
     368: 914350be     	strhls	r5, [r3, #-14]
     36c: e15350bc     	ldrh	r5, [r3, #-12]
     370: e1550000     	cmp	r5, r0
     374: 950ca018     	strls	r10, [r12, #-0x18]
     378: 914300bc     	strhls	r0, [r3, #-12]
     37c: e15300ba     	ldrh	r0, [r3, #-10]
     380: e1500007     	cmp	r0, r7
     384: 950ca014     	strls	r10, [r12, #-0x14]
     388: 914370ba     	strhls	r7, [r3, #-10]
     38c: e15e0003     	cmp	lr, r3
     390: 1affffe3     	bne	0x324 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x324> @ imm = #-0x74
     394: e59d3008     	ldr	r3, [sp, #0x8]
     398: e2444004     	sub	r4, r4, #4
     39c: e59d200c     	ldr	r2, [sp, #0xc]
     3a0: e0444106     	sub	r4, r4, r6, lsl #2
     3a4: e59d8018     	ldr	r8, [sp, #0x18]
     3a8: e59d5010     	ldr	r5, [sp, #0x10]
     3ac: e083e002     	add	lr, r3, r2
     3b0: e0812206     	add	r2, r1, r6, lsl #4
     3b4: e59d1030     	ldr	r1, [sp, #0x30]
     3b8: e59d0014     	ldr	r0, [sp, #0x14]
     3bc: e59d701c     	ldr	r7, [sp, #0x1c]
     3c0: e0813008     	add	r3, r1, r8
     3c4: e59d9020     	ldr	r9, [sp, #0x20]
     3c8: e0233196     	mla	r3, r6, r1, r3
     3cc: e3540000     	cmp	r4, #0
     3d0: a08e4084     	addge	r4, lr, r4, lsl #1
     3d4: a24ee002     	subge	lr, lr, #2
     3d8: ba000008     	blt	0x400 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x400> @ imm = #0x20
     3dc: e1fec0b2     	ldrh	r12, [lr, #2]!
     3e0: e1a01723     	lsr	r1, r3, #14
     3e4: e2822004     	add	r2, r2, #4
     3e8: e083300b     	add	r3, r3, r11
     3ec: e15c0001     	cmp	r12, r1
     3f0: 9502a004     	strls	r10, [r2, #-0x4]
     3f4: 91ce10b0     	strhls	r1, [lr]
     3f8: e154000e     	cmp	r4, lr
     3fc: 1afffff6     	bne	0x3dc <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x3dc> @ imm = #-0x28
     400: e59d302c     	ldr	r3, [sp, #0x2c]
     404: e0855003     	add	r5, r5, r3
     408: e3550000     	cmp	r5, #0
     40c: da00001e     	ble	0x48c <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x48c> @ imm = #0x78
     410: e59d3034     	ldr	r3, [sp, #0x34]
     414: e2455801     	sub	r5, r5, #65536
     418: e0800003     	add	r0, r0, r3
     41c: e59d3038     	ldr	r3, [sp, #0x38]
     420: e0888003     	add	r8, r8, r3
     424: e59d3024     	ldr	r3, [sp, #0x24]
     428: e3570000     	cmp	r7, #0
     42c: e59d2000     	ldr	r2, [sp]
     430: e0899003     	add	r9, r9, r3
     434: e59d3028     	ldr	r3, [sp, #0x28]
     438: e5933020     	ldr	r3, [r3, #0x20]
     43c: e0822103     	add	r2, r2, r3, lsl #2
     440: e58d2000     	str	r2, [sp]
     444: e59d2004     	ldr	r2, [sp, #0x4]
     448: e0823083     	add	r3, r2, r3, lsl #1
     44c: e58d3004     	str	r3, [sp, #0x4]
     450: 1affff9a     	bne	0x2c0 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x2c0> @ imm = #-0x198
     454: e59d404c     	ldr	r4, [sp, #0x4c]
     458: e1a01005     	mov	r1, r5
     45c: e59d3024     	ldr	r3, [sp, #0x24]
     460: e1a05000     	mov	r5, r0
     464: e59d2044     	ldr	r2, [sp, #0x44]
     468: e1a00008     	mov	r0, r8
     46c: e59d7050     	ldr	r7, [sp, #0x50]
     470: e0244392     	mla	r4, r2, r3, r4
     474: e59d3048     	ldr	r3, [sp, #0x48]
     478: e3530001     	cmp	r3, #1
     47c: 1a000007     	bne	0x4a0 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x4a0> @ imm = #0x1c
     480: e28dd05c     	add	sp, sp, #92
     484: ecbd8b02     	vpop	{d8}
     488: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     48c: e59d303c     	ldr	r3, [sp, #0x3c]
     490: e0800003     	add	r0, r0, r3
     494: e59d3040     	ldr	r3, [sp, #0x40]
     498: e0888003     	add	r8, r8, r3
     49c: eaffffe0     	b	0x424 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x424> @ imm = #-0x80
     4a0: e59d3054     	ldr	r3, [sp, #0x54]
     4a4: e3a06001     	mov	r6, #1
     4a8: e5972004     	ldr	r2, [r7, #0x4]
     4ac: eeb58ac0     	vcmpe.f32	s16, #0
     4b0: e5933004     	ldr	r3, [r3, #0x4]
     4b4: eef1fa10     	vmrs	APSR_nzcv, fpscr
     4b8: e0423003     	sub	r3, r2, r3
     4bc: e0832006     	add	r2, r3, r6
     4c0: e58d2044     	str	r2, [sp, #0x44]
     4c4: da000006     	ble	0x4e4 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x4e4> @ imm = #0x18
     4c8: e1a09001     	mov	r9, r1
     4cc: e1a08005     	mov	r8, r5
     4d0: e1a01003     	mov	r1, r3
     4d4: e59d4054     	ldr	r4, [sp, #0x54]
     4d8: e1a05000     	mov	r5, r0
     4dc: e1a03007     	mov	r3, r7
     4e0: eaffff59     	b	0x24c <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x24c> @ imm = #-0x29c
     4e4: e59d2054     	ldr	r2, [sp, #0x54]
     4e8: e3530000     	cmp	r3, #0
     4ec: e5925000     	ldr	r5, [r2]
     4f0: da000018     	ble	0x558 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x558> @ imm = #0x60
     4f4: e5970000     	ldr	r0, [r7]
     4f8: e1a01003     	mov	r1, r3
     4fc: e0400005     	sub	r0, r0, r5
     500: e1a00800     	lsl	r0, r0, #16
     504: ebfffffe     	bl	0x504 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x504> @ imm = #-0x8
     508: e6ff2070     	uxth	r2, r0
     50c: e1a03840     	asr	r3, r0, #16
     510: e59d1048     	ldr	r1, [sp, #0x48]
     514: e58d202c     	str	r2, [sp, #0x2c]
     518: e59d2054     	ldr	r2, [sp, #0x54]
     51c: e58d303c     	str	r3, [sp, #0x3c]
     520: e58d6048     	str	r6, [sp, #0x48]
     524: e5920008     	ldr	r0, [r2, #0x8]
     528: ee182a90     	vmov	r2, s17
     52c: e0222b93     	mla	r2, r3, r11, r2
     530: e2833001     	add	r3, r3, #1
     534: e58d3034     	str	r3, [sp, #0x34]
     538: e08b3002     	add	r3, r11, r2
     53c: e58d2040     	str	r2, [sp, #0x40]
     540: e58d3038     	str	r3, [sp, #0x38]
     544: eaffff52     	b	0x294 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x294> @ imm = #-0x2b8
     548: e1a01000     	mov	r1, r0
     54c: e1a0c002     	mov	r12, r2
     550: e1a07003     	mov	r7, r3
     554: eaffff0e     	b	0x194 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x194> @ imm = #-0x3c8
     558: e59d3044     	ldr	r3, [sp, #0x44]
     55c: e3530001     	cmp	r3, #1
     560: 1affffc6     	bne	0x480 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x480> @ imm = #-0xe8
     564: e59d1048     	ldr	r1, [sp, #0x48]
     568: e5920008     	ldr	r0, [r2, #0x8]
     56c: ee182a90     	vmov	r2, s17
     570: e08bc002     	add	r12, r11, r2
     574: e58d102c     	str	r1, [sp, #0x2c]
     578: e58d103c     	str	r1, [sp, #0x3c]
     57c: e58d3048     	str	r3, [sp, #0x48]
     580: e58dc038     	str	r12, [sp, #0x38]
     584: edcd8a10     	vstr	s17, [sp, #64]
     588: e58d3034     	str	r3, [sp, #0x34]
     58c: eaffff43     	b	0x2a0 <ZB_fillTriangleFlatNOBLEND_DT1_DW1+0x2a0> @ imm = #-0x2f4

Disassembly of section .text.ZB_fillTriangleSmooth_DT0_DW0:

00000000 <ZB_fillTriangleSmooth_DT0_DW0>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a0b003     	mov	r11, r3
       8: ed2d8b04     	vpush	{d8, d9}
       c: e24dd0bc     	sub	sp, sp, #188
      10: e592c004     	ldr	r12, [r2, #0x4]
      14: e5913004     	ldr	r3, [r1, #0x4]
      18: e58d005c     	str	r0, [sp, #0x5c]
      1c: e59b0004     	ldr	r0, [r11, #0x4]
      20: e15c0003     	cmp	r12, r3
      24: a1a04003     	movge	r4, r3
      28: b1a0400c     	movlt	r4, r12
      2c: a1a0300c     	movge	r3, r12
      30: b1a05002     	movlt	r5, r2
      34: a1a05001     	movge	r5, r1
      38: b1a0e001     	movlt	lr, r1
      3c: a1a0e002     	movge	lr, r2
      40: e1500004     	cmp	r0, r4
      44: e1a0c004     	mov	r12, r4
      48: e58d509c     	str	r5, [sp, #0x9c]
      4c: ba00000b     	blt	0x80 <ZB_fillTriangleSmooth_DT0_DW0+0x80> @ imm = #0x2c
      50: e1500003     	cmp	r0, r3
      54: e1a0200b     	mov	r2, r11
      58: a1a0c003     	movge	r12, r3
      5c: b1a0c000     	movlt	r12, r0
      60: a1a03000     	movge	r3, r0
      64: a58de09c     	strge	lr, [sp, #0x9c]
      68: a1a0b005     	movge	r11, r5
      6c: a1a00004     	movge	r0, r4
      70: a1a0e002     	movge	lr, r2
      74: b1a00004     	movlt	r0, r4
      78: b59db09c     	ldrlt	r11, [sp, #0x9c]
      7c: b58d209c     	strlt	r2, [sp, #0x9c]
      80: e0433000     	sub	r3, r3, r0
      84: e59b2000     	ldr	r2, [r11]
      88: e59b4014     	ldr	r4, [r11, #0x14]
      8c: ee073a10     	vmov	s14, r3
      90: e59e3000     	ldr	r3, [lr]
      94: eeb87ac7     	vcvt.f32.s32	s14, s14
      98: e59b1018     	ldr	r1, [r11, #0x18]
      9c: e0433002     	sub	r3, r3, r2
      a0: e59d509c     	ldr	r5, [sp, #0x9c]
      a4: ee073a90     	vmov	s15, r3
      a8: e04c3000     	sub	r3, r12, r0
      ac: ee063a90     	vmov	s13, r3
      b0: e59d309c     	ldr	r3, [sp, #0x9c]
      b4: eef87ae7     	vcvt.f32.s32	s15, s15
      b8: eef86ae6     	vcvt.f32.s32	s13, s13
      bc: e5933000     	ldr	r3, [r3]
      c0: e0433002     	sub	r3, r3, r2
      c4: ee268aa7     	vmul.f32	s16, s13, s15
      c8: e59b201c     	ldr	r2, [r11, #0x1c]
      cc: ee053a90     	vmov	s11, r3
      d0: e59e3014     	ldr	r3, [lr, #0x14]
      d4: eef85ae5     	vcvt.f32.s32	s11, s11
      d8: e0433004     	sub	r3, r3, r4
      dc: ee158a87     	vnmls.f32	s16, s11, s14
      e0: eeb58a40     	vcmp.f32	s16, #0
      e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
      e8: 1eb76a00     	vmovne.f32	s12, #1.000000e+00
      ec: 1e868a08     	vdivne.f32	s16, s12, s16
      f0: ee063a10     	vmov	s12, r3
      f4: e59e3018     	ldr	r3, [lr, #0x18]
      f8: eeb84ac6     	vcvt.f32.s32	s8, s12
      fc: e0433001     	sub	r3, r3, r1
     100: ee063a10     	vmov	s12, r3
     104: e59e301c     	ldr	r3, [lr, #0x1c]
     108: eef84ac6     	vcvt.f32.s32	s9, s12
     10c: e0433002     	sub	r3, r3, r2
     110: ee063a10     	vmov	s12, r3
     114: e5953014     	ldr	r3, [r5, #0x14]
     118: ee686a26     	vmul.f32	s13, s16, s13
     11c: eeb85ac6     	vcvt.f32.s32	s10, s12
     120: e0433004     	sub	r3, r3, r4
     124: ee287a07     	vmul.f32	s14, s16, s14
     128: ee687a27     	vmul.f32	s15, s16, s15
     12c: ee263a84     	vmul.f32	s6, s13, s8
     130: ee663aa4     	vmul.f32	s7, s13, s9
     134: ee266a85     	vmul.f32	s12, s13, s10
     138: ee063a90     	vmov	s13, r3
     13c: ee685a25     	vmul.f32	s11, s16, s11
     140: eef86ae6     	vcvt.f32.s32	s13, s13
     144: e5953018     	ldr	r3, [r5, #0x18]
     148: eeb58ac0     	vcmpe.f32	s16, #0
     14c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     150: ee173a26     	vnmls.f32	s6, s14, s13
     154: e0433001     	sub	r3, r3, r1
     158: ee676aa6     	vmul.f32	s13, s15, s13
     15c: e1a01005     	mov	r1, r5
     160: ee556a84     	vnmls.f32	s13, s11, s8
     164: eeb04a63     	vmov.f32	s8, s7
     168: eefd6ae6     	vcvt.s32.f32	s13, s13
     16c: eef08a66     	vmov.f32	s17, s13
     170: ee063a90     	vmov	s13, r3
     174: e595301c     	ldr	r3, [r5, #0x1c]
     178: eef86ae6     	vcvt.f32.s32	s13, s13
     17c: e0433002     	sub	r3, r3, r2
     180: e59d205c     	ldr	r2, [sp, #0x5c]
     184: ee174a26     	vnmls.f32	s8, s14, s13
     188: ee676aa6     	vmul.f32	s13, s15, s13
     18c: ee556aa4     	vnmls.f32	s13, s11, s9
     190: eefd6ae6     	vcvt.s32.f32	s13, s13
     194: eeb09a66     	vmov.f32	s18, s13
     198: ee063a90     	vmov	s13, r3
     19c: eef84ae6     	vcvt.f32.s32	s9, s13
     1a0: e5923020     	ldr	r3, [r2, #0x20]
     1a4: eef06a46     	vmov.f32	s13, s12
     1a8: ee677aa4     	vmul.f32	s15, s15, s9
     1ac: e0000093     	mul	r0, r3, r0
     1b0: ee576a24     	vnmls.f32	s13, s14, s9
     1b4: e5923004     	ldr	r3, [r2, #0x4]
     1b8: ee557a85     	vnmls.f32	s15, s11, s10
     1bc: e0833100     	add	r3, r3, r0, lsl #2
     1c0: e58d3040     	str	r3, [sp, #0x40]
     1c4: eefd7ae7     	vcvt.s32.f32	s15, s15
     1c8: eef09a67     	vmov.f32	s19, s15
     1cc: eefd7ac3     	vcvt.s32.f32	s15, s6
     1d0: edcd7a11     	vstr	s15, [sp, #68]
     1d4: eefd7ac4     	vcvt.s32.f32	s15, s8
     1d8: e59d2044     	ldr	r2, [sp, #0x44]
     1dc: edcd7a12     	vstr	s15, [sp, #72]
     1e0: e59d3048     	ldr	r3, [sp, #0x48]
     1e4: e1a05082     	lsl	r5, r2, #1
     1e8: eefd7ae6     	vcvt.s32.f32	s15, s13
     1ec: e58d5078     	str	r5, [sp, #0x78]
     1f0: e1a07103     	lsl	r7, r3, #2
     1f4: e1a04083     	lsl	r4, r3, #1
     1f8: ee178a90     	vmov	r8, s15
     1fc: e58d7010     	str	r7, [sp, #0x10]
     200: e1a07102     	lsl	r7, r2, #2
     204: e1a06088     	lsl	r6, r8, #1
     208: e0843003     	add	r3, r4, r3
     20c: e58d7014     	str	r7, [sp, #0x14]
     210: e1a07108     	lsl	r7, r8, #2
     214: e58d3068     	str	r3, [sp, #0x68]
     218: e0853002     	add	r3, r5, r2
     21c: e58d306c     	str	r3, [sp, #0x6c]
     220: e0863008     	add	r3, r6, r8
     224: e58d4074     	str	r4, [sp, #0x74]
     228: e58d6070     	str	r6, [sp, #0x70]
     22c: e58d700c     	str	r7, [sp, #0xc]
     230: e58d3064     	str	r3, [sp, #0x64]
     234: da00013d     	ble	0x730 <ZB_fillTriangleSmooth_DT0_DW0+0x730> @ imm = #0x4f4
     238: e59e2004     	ldr	r2, [lr, #0x4]
     23c: e1a0000e     	mov	r0, lr
     240: e58d10b0     	str	r1, [sp, #0xb0]
     244: e59b3004     	ldr	r3, [r11, #0x4]
     248: e3a09000     	mov	r9, #0
     24c: e1a0400b     	mov	r4, r11
     250: e1a0a009     	mov	r10, r9
     254: e1a06008     	mov	r6, r8
     258: e58d9058     	str	r9, [sp, #0x58]
     25c: e04c7003     	sub	r7, r12, r3
     260: e0421003     	sub	r1, r2, r3
     264: e3a03001     	mov	r3, #1
     268: e58de0ac     	str	lr, [sp, #0xac]
     26c: e58d3030     	str	r3, [sp, #0x30]
     270: e58db0b4     	str	r11, [sp, #0xb4]
     274: e5943000     	ldr	r3, [r4]
     278: e3510000     	cmp	r1, #0
     27c: e58d303c     	str	r3, [sp, #0x3c]
     280: da000114     	ble	0x6d8 <ZB_fillTriangleSmooth_DT0_DW0+0x6d8> @ imm = #0x450
     284: e5902000     	ldr	r2, [r0]
     288: e0422003     	sub	r2, r2, r3
     28c: e1a00802     	lsl	r0, r2, #16
     290: ebfffffe     	bl	0x290 <ZB_fillTriangleSmooth_DT0_DW0+0x290> @ imm = #-0x8
     294: e59d1044     	ldr	r1, [sp, #0x44]
     298: e6ff2070     	uxth	r2, r0
     29c: e1a03840     	asr	r3, r0, #16
     2a0: e58d2060     	str	r2, [sp, #0x60]
     2a4: ee182a90     	vmov	r2, s17
     2a8: e58d3098     	str	r3, [sp, #0x98]
     2ac: e0222193     	mla	r2, r3, r1, r2
     2b0: e59d1048     	ldr	r1, [sp, #0x48]
     2b4: e58d208c     	str	r2, [sp, #0x8c]
     2b8: ee192a10     	vmov	r2, s18
     2bc: e0222193     	mla	r2, r3, r1, r2
     2c0: e58d2090     	str	r2, [sp, #0x90]
     2c4: ee192a90     	vmov	r2, s19
     2c8: e0222693     	mla	r2, r3, r6, r2
     2cc: e2833001     	add	r3, r3, #1
     2d0: e58d3088     	str	r3, [sp, #0x88]
     2d4: e58d2094     	str	r2, [sp, #0x94]
     2d8: e59d3030     	ldr	r3, [sp, #0x30]
     2dc: e5945014     	ldr	r5, [r4, #0x14]
     2e0: e594b018     	ldr	r11, [r4, #0x18]
     2e4: e3530000     	cmp	r3, #0
     2e8: e594301c     	ldr	r3, [r4, #0x1c]
     2ec: e58d3004     	str	r3, [sp, #0x4]
     2f0: 1a000112     	bne	0x740 <ZB_fillTriangleSmooth_DT0_DW0+0x740> @ imm = #0x448
     2f4: e3570000     	cmp	r7, #0
     2f8: da0000c6     	ble	0x618 <ZB_fillTriangleSmooth_DT0_DW0+0x618> @ imm = #0x318
     2fc: e59d208c     	ldr	r2, [sp, #0x8c]
     300: e1a00005     	mov	r0, r5
     304: e59d3044     	ldr	r3, [sp, #0x44]
     308: e58d7034     	str	r7, [sp, #0x34]
     30c: e58da038     	str	r10, [sp, #0x38]
     310: e0833002     	add	r3, r3, r2
     314: e59d2090     	ldr	r2, [sp, #0x90]
     318: e58d307c     	str	r3, [sp, #0x7c]
     31c: e59d3048     	ldr	r3, [sp, #0x48]
     320: e58d90a0     	str	r9, [sp, #0xa0]
     324: e58d70a4     	str	r7, [sp, #0xa4]
     328: e0833002     	add	r3, r3, r2
     32c: e58da0a8     	str	r10, [sp, #0xa8]
     330: e58d3080     	str	r3, [sp, #0x80]
     334: e59d3094     	ldr	r3, [sp, #0x94]
     338: e0863003     	add	r3, r6, r3
     33c: e58d3084     	str	r3, [sp, #0x84]
     340: e59d3034     	ldr	r3, [sp, #0x34]
     344: e59d203c     	ldr	r2, [sp, #0x3c]
     348: e59d1040     	ldr	r1, [sp, #0x40]
     34c: e2433001     	sub	r3, r3, #1
     350: e58d3034     	str	r3, [sp, #0x34]
     354: e59d3038     	ldr	r3, [sp, #0x38]
     358: e0623843     	rsb	r3, r2, r3, asr #16
     35c: e0812102     	add	r2, r1, r2, lsl #2
     360: e3530002     	cmp	r3, #2
     364: da0000bd     	ble	0x660 <ZB_fillTriangleSmooth_DT0_DW0+0x660> @ imm = #0x2f4
     368: e2825010     	add	r5, r2, #16
     36c: e59d2074     	ldr	r2, [sp, #0x74]
     370: e59d1070     	ldr	r1, [sp, #0x70]
     374: e3a0c000     	mov	r12, #0
     378: e1a07003     	mov	r7, r3
     37c: e1a0400c     	mov	r4, r12
     380: e08b2002     	add	r2, r11, r2
     384: e1a0e00c     	mov	lr, r12
     388: e58d2028     	str	r2, [sp, #0x28]
     38c: e59d2078     	ldr	r2, [sp, #0x78]
     390: e58d504c     	str	r5, [sp, #0x4c]
     394: e58d6050     	str	r6, [sp, #0x50]
     398: e0802002     	add	r2, r0, r2
     39c: e58d3054     	str	r3, [sp, #0x54]
     3a0: e58d202c     	str	r2, [sp, #0x2c]
     3a4: e59d2004     	ldr	r2, [sp, #0x4]
     3a8: e0821001     	add	r1, r2, r1
     3ac: e086a002     	add	r10, r6, r2
     3b0: e58d1024     	str	r1, [sp, #0x24]
     3b4: e59d1068     	ldr	r1, [sp, #0x68]
     3b8: e081100b     	add	r1, r1, r11
     3bc: e58d101c     	str	r1, [sp, #0x1c]
     3c0: e59d106c     	ldr	r1, [sp, #0x6c]
     3c4: e0811000     	add	r1, r1, r0
     3c8: e58d1020     	str	r1, [sp, #0x20]
     3cc: e59d1064     	ldr	r1, [sp, #0x64]
     3d0: e0811002     	add	r1, r1, r2
     3d4: e59d2044     	ldr	r2, [sp, #0x44]
     3d8: e58d1018     	str	r1, [sp, #0x18]
     3dc: e59d1048     	ldr	r1, [sp, #0x48]
     3e0: e0822000     	add	r2, r2, r0
     3e4: e58d2008     	str	r2, [sp, #0x8]
     3e8: e081900b     	add	r9, r1, r11
     3ec: e59d601c     	ldr	r6, [sp, #0x1c]
     3f0: e08b200e     	add	r2, r11, lr
     3f4: e59d1028     	ldr	r1, [sp, #0x28]
     3f8: e089300e     	add	r3, r9, lr
     3fc: e1a02442     	asr	r2, r2, #8
     400: e1a03443     	asr	r3, r3, #8
     404: e086800e     	add	r8, r6, lr
     408: e59d6010     	ldr	r6, [sp, #0x10]
     40c: e081100e     	add	r1, r1, lr
     410: e2022cff     	and	r2, r2, #65280
     414: e2033cff     	and	r3, r3, #65280
     418: e1a01441     	asr	r1, r1, #8
     41c: e08ee006     	add	lr, lr, r6
     420: e59d6004     	ldr	r6, [sp, #0x4]
     424: e2011cff     	and	r1, r1, #65280
     428: e1a08448     	asr	r8, r8, #8
     42c: e2477004     	sub	r7, r7, #4
     430: e086600c     	add	r6, r6, r12
     434: e2088cff     	and	r8, r8, #65280
     438: e7e76856     	ubfx	r6, r6, #0x10, #0x8
     43c: e3570002     	cmp	r7, #2
     440: e1822006     	orr	r2, r2, r6
     444: e08a600c     	add	r6, r10, r12
     448: e7e76856     	ubfx	r6, r6, #0x10, #0x8
     44c: e2855010     	add	r5, r5, #16
     450: e1833006     	orr	r3, r3, r6
     454: e59d6024     	ldr	r6, [sp, #0x24]
     458: e086600c     	add	r6, r6, r12
     45c: e7e76856     	ubfx	r6, r6, #0x10, #0x8
     460: e1816006     	orr	r6, r1, r6
     464: e59d1018     	ldr	r1, [sp, #0x18]
     468: e081100c     	add	r1, r1, r12
     46c: e7e71851     	ubfx	r1, r1, #0x10, #0x8
     470: e1881001     	orr	r1, r8, r1
     474: e59d800c     	ldr	r8, [sp, #0xc]
     478: e08cc008     	add	r12, r12, r8
     47c: e0808004     	add	r8, r0, r4
     480: e20888ff     	and	r8, r8, #16711680
     484: e1822008     	orr	r2, r2, r8
     488: e5052020     	str	r2, [r5, #-0x20]
     48c: e59d2008     	ldr	r2, [sp, #0x8]
     490: e0822004     	add	r2, r2, r4
     494: e20228ff     	and	r2, r2, #16711680
     498: e1833002     	orr	r3, r3, r2
     49c: e505301c     	str	r3, [r5, #-0x1c]
     4a0: e59d302c     	ldr	r3, [sp, #0x2c]
     4a4: e0833004     	add	r3, r3, r4
     4a8: e20338ff     	and	r3, r3, #16711680
     4ac: e1866003     	orr	r6, r6, r3
     4b0: e59d3020     	ldr	r3, [sp, #0x20]
     4b4: e5056018     	str	r6, [r5, #-0x18]
     4b8: e0833004     	add	r3, r3, r4
     4bc: e20338ff     	and	r3, r3, #16711680
     4c0: e1811003     	orr	r1, r1, r3
     4c4: e59d3014     	ldr	r3, [sp, #0x14]
     4c8: e5051014     	str	r1, [r5, #-0x14]
     4cc: e0844003     	add	r4, r4, r3
     4d0: caffffc5     	bgt	0x3ec <ZB_fillTriangleSmooth_DT0_DW0+0x3ec> @ imm = #-0xec
     4d4: e59d2044     	ldr	r2, [sp, #0x44]
     4d8: e59d1008     	ldr	r1, [sp, #0x8]
     4dc: e59d3054     	ldr	r3, [sp, #0x54]
     4e0: e59d6050     	ldr	r6, [sp, #0x50]
     4e4: e0818082     	add	r8, r1, r2, lsl #1
     4e8: e59d1048     	ldr	r1, [sp, #0x48]
     4ec: e0888002     	add	r8, r8, r2
     4f0: e08aa086     	add	r10, r10, r6, lsl #1
     4f4: e0899081     	add	r9, r9, r1, lsl #1
     4f8: e08aa006     	add	r10, r10, r6
     4fc: e0899001     	add	r9, r9, r1
     500: e2431003     	sub	r1, r3, #3
     504: e1a02121     	lsr	r2, r1, #2
     508: e2433004     	sub	r3, r3, #4
     50c: e3c11003     	bic	r1, r1, #3
     510: e0433001     	sub	r3, r3, r1
     514: e59d100c     	ldr	r1, [sp, #0xc]
     518: e02aa192     	mla	r10, r2, r1, r10
     51c: e59d1014     	ldr	r1, [sp, #0x14]
     520: e0288192     	mla	r8, r2, r1, r8
     524: e59d1010     	ldr	r1, [sp, #0x10]
     528: e0299192     	mla	r9, r2, r1, r9
     52c: e59d104c     	ldr	r1, [sp, #0x4c]
     530: e0812202     	add	r2, r1, r2, lsl #4
     534: e3530000     	cmp	r3, #0
     538: ba00000f     	blt	0x57c <ZB_fillTriangleSmooth_DT0_DW0+0x57c> @ imm = #0x3c
     53c: e59dc044     	ldr	r12, [sp, #0x44]
     540: e0821103     	add	r1, r2, r3, lsl #2
     544: e59de048     	ldr	lr, [sp, #0x48]
     548: e2422004     	sub	r2, r2, #4
     54c: e1a03449     	asr	r3, r9, #8
     550: e7e7485a     	ubfx	r4, r10, #0x10, #0x8
     554: e089900e     	add	r9, r9, lr
     558: e2033cff     	and	r3, r3, #65280
     55c: e08aa006     	add	r10, r10, r6
     560: e1833004     	orr	r3, r3, r4
     564: e20848ff     	and	r4, r8, #16711680
     568: e1833004     	orr	r3, r3, r4
     56c: e088800c     	add	r8, r8, r12
     570: e5a23004     	str	r3, [r2, #0x4]!
     574: e1510002     	cmp	r1, r2
     578: 1afffff3     	bne	0x54c <ZB_fillTriangleSmooth_DT0_DW0+0x54c> @ imm = #-0x34
     57c: e59d3030     	ldr	r3, [sp, #0x30]
     580: e59d2060     	ldr	r2, [sp, #0x60]
     584: e0833002     	add	r3, r3, r2
     588: e3530000     	cmp	r3, #0
     58c: e58d3030     	str	r3, [sp, #0x30]
     590: da000025     	ble	0x62c <ZB_fillTriangleSmooth_DT0_DW0+0x62c> @ imm = #0x94
     594: e59d3030     	ldr	r3, [sp, #0x30]
     598: e59d2088     	ldr	r2, [sp, #0x88]
     59c: e2433801     	sub	r3, r3, #65536
     5a0: e58d3030     	str	r3, [sp, #0x30]
     5a4: e59d303c     	ldr	r3, [sp, #0x3c]
     5a8: e0833002     	add	r3, r3, r2
     5ac: e59d2084     	ldr	r2, [sp, #0x84]
     5b0: e58d303c     	str	r3, [sp, #0x3c]
     5b4: e59d307c     	ldr	r3, [sp, #0x7c]
     5b8: e0800003     	add	r0, r0, r3
     5bc: e59d3080     	ldr	r3, [sp, #0x80]
     5c0: e08bb003     	add	r11, r11, r3
     5c4: e59d3004     	ldr	r3, [sp, #0x4]
     5c8: e0833002     	add	r3, r3, r2
     5cc: e58d3004     	str	r3, [sp, #0x4]
     5d0: e59d3034     	ldr	r3, [sp, #0x34]
     5d4: e59d2058     	ldr	r2, [sp, #0x58]
     5d8: e3530000     	cmp	r3, #0
     5dc: e59d3038     	ldr	r3, [sp, #0x38]
     5e0: e0833002     	add	r3, r3, r2
     5e4: e59d2040     	ldr	r2, [sp, #0x40]
     5e8: e58d3038     	str	r3, [sp, #0x38]
     5ec: e59d305c     	ldr	r3, [sp, #0x5c]
     5f0: e5933020     	ldr	r3, [r3, #0x20]
     5f4: e0823103     	add	r3, r2, r3, lsl #2
     5f8: e58d3040     	str	r3, [sp, #0x40]
     5fc: 1affff4f     	bne	0x340 <ZB_fillTriangleSmooth_DT0_DW0+0x340> @ imm = #-0x2c4
     600: e59d70a4     	ldr	r7, [sp, #0xa4]
     604: e1a05000     	mov	r5, r0
     608: e59da0a8     	ldr	r10, [sp, #0xa8]
     60c: e59d3058     	ldr	r3, [sp, #0x58]
     610: e59d90a0     	ldr	r9, [sp, #0xa0]
     614: e02aa397     	mla	r10, r7, r3, r10
     618: e3590001     	cmp	r9, #1
     61c: 1a000013     	bne	0x670 <ZB_fillTriangleSmooth_DT0_DW0+0x670> @ imm = #0x4c
     620: e28dd0bc     	add	sp, sp, #188
     624: ecbd8b04     	vpop	{d8, d9}
     628: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     62c: e59d303c     	ldr	r3, [sp, #0x3c]
     630: e59d2098     	ldr	r2, [sp, #0x98]
     634: e0833002     	add	r3, r3, r2
     638: e59d2094     	ldr	r2, [sp, #0x94]
     63c: e58d303c     	str	r3, [sp, #0x3c]
     640: e59d308c     	ldr	r3, [sp, #0x8c]
     644: e0800003     	add	r0, r0, r3
     648: e59d3090     	ldr	r3, [sp, #0x90]
     64c: e08bb003     	add	r11, r11, r3
     650: e59d3004     	ldr	r3, [sp, #0x4]
     654: e0833002     	add	r3, r3, r2
     658: e58d3004     	str	r3, [sp, #0x4]
     65c: eaffffdb     	b	0x5d0 <ZB_fillTriangleSmooth_DT0_DW0+0x5d0> @ imm = #-0x94
     660: e59da004     	ldr	r10, [sp, #0x4]
     664: e1a0900b     	mov	r9, r11
     668: e1a08000     	mov	r8, r0
     66c: eaffffb0     	b	0x534 <ZB_fillTriangleSmooth_DT0_DW0+0x534> @ imm = #-0x140
     670: e59d00ac     	ldr	r0, [sp, #0xac]
     674: e59d309c     	ldr	r3, [sp, #0x9c]
     678: eeb58ac0     	vcmpe.f32	s16, #0
     67c: e5901004     	ldr	r1, [r0, #0x4]
     680: e5932004     	ldr	r2, [r3, #0x4]
     684: eef1fa10     	vmrs	APSR_nzcv, fpscr
     688: e0411002     	sub	r1, r1, r2
     68c: e2817001     	add	r7, r1, #1
     690: da000019     	ble	0x6fc <ZB_fillTriangleSmooth_DT0_DW0+0x6fc> @ imm = #0x64
     694: e3510000     	cmp	r1, #0
     698: e593a000     	ldr	r10, [r3]
     69c: e3a04001     	mov	r4, #1
     6a0: da00001a     	ble	0x710 <ZB_fillTriangleSmooth_DT0_DW0+0x710> @ imm = #0x68
     6a4: e59d309c     	ldr	r3, [sp, #0x9c]
     6a8: e58d30b4     	str	r3, [sp, #0xb4]
     6ac: e59d30ac     	ldr	r3, [sp, #0xac]
     6b0: e58d30b0     	str	r3, [sp, #0xb0]
     6b4: e59d30b0     	ldr	r3, [sp, #0xb0]
     6b8: e1a09004     	mov	r9, r4
     6bc: e5930000     	ldr	r0, [r3]
     6c0: e040000a     	sub	r0, r0, r10
     6c4: e1a00800     	lsl	r0, r0, #16
     6c8: ebfffffe     	bl	0x6c8 <ZB_fillTriangleSmooth_DT0_DW0+0x6c8> @ imm = #-0x8
     6cc: e58d0058     	str	r0, [sp, #0x58]
     6d0: e1a0a80a     	lsl	r10, r10, #16
     6d4: eaffff06     	b	0x2f4 <ZB_fillTriangleSmooth_DT0_DW0+0x2f4> @ imm = #-0x3e8
     6d8: e3a03000     	mov	r3, #0
     6dc: e3a02001     	mov	r2, #1
     6e0: e58d3060     	str	r3, [sp, #0x60]
     6e4: edcd9a25     	vstr	s19, [sp, #148]
     6e8: ed8d9a24     	vstr	s18, [sp, #144]
     6ec: edcd8a23     	vstr	s17, [sp, #140]
     6f0: e58d2088     	str	r2, [sp, #0x88]
     6f4: e58d3098     	str	r3, [sp, #0x98]
     6f8: eafffef6     	b	0x2d8 <ZB_fillTriangleSmooth_DT0_DW0+0x2d8> @ imm = #-0x428
     6fc: e58d9030     	str	r9, [sp, #0x30]
     700: e3a09001     	mov	r9, #1
     704: e59d00ac     	ldr	r0, [sp, #0xac]
     708: e59d409c     	ldr	r4, [sp, #0x9c]
     70c: eafffed8     	b	0x274 <ZB_fillTriangleSmooth_DT0_DW0+0x274> @ imm = #-0x4a0
     710: e3570000     	cmp	r7, #0
     714: daffffc1     	ble	0x620 <ZB_fillTriangleSmooth_DT0_DW0+0x620> @ imm = #-0xfc
     718: e58d9058     	str	r9, [sp, #0x58]
     71c: e1a0a80a     	lsl	r10, r10, #16
     720: e1a09004     	mov	r9, r4
     724: e58d30b4     	str	r3, [sp, #0xb4]
     728: e58d00b0     	str	r0, [sp, #0xb0]
     72c: eafffef2     	b	0x2fc <ZB_fillTriangleSmooth_DT0_DW0+0x2fc> @ imm = #-0x438
     730: e59d009c     	ldr	r0, [sp, #0x9c]
     734: e1a0200c     	mov	r2, r12
     738: e58de0b0     	str	lr, [sp, #0xb0]
     73c: eafffec0     	b	0x244 <ZB_fillTriangleSmooth_DT0_DW0+0x244> @ imm = #-0x500
     740: e59d30b0     	ldr	r3, [sp, #0xb0]
     744: e5931004     	ldr	r1, [r3, #0x4]
     748: e59d30b4     	ldr	r3, [sp, #0xb4]
     74c: e5932004     	ldr	r2, [r3, #0x4]
     750: e593a000     	ldr	r10, [r3]
     754: e0411002     	sub	r1, r1, r2
     758: e3510000     	cmp	r1, #0
     75c: c3a03000     	movgt	r3, #0
     760: c1a04009     	movgt	r4, r9
     764: c58d3030     	strgt	r3, [sp, #0x30]
     768: caffffd1     	bgt	0x6b4 <ZB_fillTriangleSmooth_DT0_DW0+0x6b4> @ imm = #-0xbc
     76c: e3a03000     	mov	r3, #0
     770: e58d3030     	str	r3, [sp, #0x30]
     774: e58d3058     	str	r3, [sp, #0x58]
     778: eaffffd4     	b	0x6d0 <ZB_fillTriangleSmooth_DT0_DW0+0x6d0> @ imm = #-0xb0

Disassembly of section .text.ZB_fillTriangleSmooth_DT0_DW1:

00000000 <ZB_fillTriangleSmooth_DT0_DW1>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a08003     	mov	r8, r3
       8: ed2d8b06     	vpush	{d8, d9, d10}
       c: e24dd0f4     	sub	sp, sp, #244
      10: e592c004     	ldr	r12, [r2, #0x4]
      14: e5913004     	ldr	r3, [r1, #0x4]
      18: e58d0088     	str	r0, [sp, #0x88]
      1c: e5980004     	ldr	r0, [r8, #0x4]
      20: e15c0003     	cmp	r12, r3
      24: a1a04003     	movge	r4, r3
      28: b1a0400c     	movlt	r4, r12
      2c: a1a0300c     	movge	r3, r12
      30: b1a07002     	movlt	r7, r2
      34: a1a07001     	movge	r7, r1
      38: b1a0e001     	movlt	lr, r1
      3c: a1a0e002     	movge	lr, r2
      40: e1500004     	cmp	r0, r4
      44: e1a0c004     	mov	r12, r4
      48: ba00000b     	blt	0x7c <ZB_fillTriangleSmooth_DT0_DW1+0x7c> @ imm = #0x2c
      4c: e1500003     	cmp	r0, r3
      50: e1a02008     	mov	r2, r8
      54: a1a0c003     	movge	r12, r3
      58: a1a08007     	movge	r8, r7
      5c: a1a03000     	movge	r3, r0
      60: a1a0700e     	movge	r7, lr
      64: b1a0c000     	movlt	r12, r0
      68: b1a08007     	movlt	r8, r7
      6c: a1a00004     	movge	r0, r4
      70: a1a0e002     	movge	lr, r2
      74: b1a00004     	movlt	r0, r4
      78: b1a07002     	movlt	r7, r2
      7c: e0433000     	sub	r3, r3, r0
      80: e5982000     	ldr	r2, [r8]
      84: e5985014     	ldr	r5, [r8, #0x14]
      88: ee073a90     	vmov	s15, r3
      8c: e59e3000     	ldr	r3, [lr]
      90: eef86ae7     	vcvt.f32.s32	s13, s15
      94: e5984018     	ldr	r4, [r8, #0x18]
      98: e0433002     	sub	r3, r3, r2
      9c: e598101c     	ldr	r1, [r8, #0x1c]
      a0: ee073a90     	vmov	s15, r3
      a4: e04c3000     	sub	r3, r12, r0
      a8: ee073a10     	vmov	s14, r3
      ac: e5973000     	ldr	r3, [r7]
      b0: eef87ae7     	vcvt.f32.s32	s15, s15
      b4: eeb87ac7     	vcvt.f32.s32	s14, s14
      b8: e0433002     	sub	r3, r3, r2
      bc: e5982008     	ldr	r2, [r8, #0x8]
      c0: ee053a90     	vmov	s11, r3
      c4: ee278a27     	vmul.f32	s16, s14, s15
      c8: eef85ae5     	vcvt.f32.s32	s11, s11
      cc: e59e3014     	ldr	r3, [lr, #0x14]
      d0: ee158aa6     	vnmls.f32	s16, s11, s13
      d4: e0433005     	sub	r3, r3, r5
      d8: eeb58a40     	vcmp.f32	s16, #0
      dc: eef1fa10     	vmrs	APSR_nzcv, fpscr
      e0: 1eb76a00     	vmovne.f32	s12, #1.000000e+00
      e4: 1e868a08     	vdivne.f32	s16, s12, s16
      e8: ee063a10     	vmov	s12, r3
      ec: e59e3018     	ldr	r3, [lr, #0x18]
      f0: eef83ac6     	vcvt.f32.s32	s7, s12
      f4: e0433004     	sub	r3, r3, r4
      f8: ee063a10     	vmov	s12, r3
      fc: e59e301c     	ldr	r3, [lr, #0x1c]
     100: eeb84ac6     	vcvt.f32.s32	s8, s12
     104: e0433001     	sub	r3, r3, r1
     108: ee063a10     	vmov	s12, r3
     10c: e59e3008     	ldr	r3, [lr, #0x8]
     110: eef84ac6     	vcvt.f32.s32	s9, s12
     114: ee287a07     	vmul.f32	s14, s16, s14
     118: e0433002     	sub	r3, r3, r2
     11c: ee686a26     	vmul.f32	s13, s16, s13
     120: ee687a27     	vmul.f32	s15, s16, s15
     124: ee063a10     	vmov	s12, r3
     128: e5973014     	ldr	r3, [r7, #0x14]
     12c: eeb85ac6     	vcvt.f32.s32	s10, s12
     130: ee672a23     	vmul.f32	s5, s14, s7
     134: e0433005     	sub	r3, r3, r5
     138: ee685a25     	vmul.f32	s11, s16, s11
     13c: ee272a04     	vmul.f32	s4, s14, s8
     140: ee063a10     	vmov	s12, r3
     144: e5973018     	ldr	r3, [r7, #0x18]
     148: eeb86ac6     	vcvt.f32.s32	s12, s12
     14c: ee273a24     	vmul.f32	s6, s14, s9
     150: e0433004     	sub	r3, r3, r4
     154: ee277a05     	vmul.f32	s14, s14, s10
     158: eeb58ac0     	vcmpe.f32	s16, #0
     15c: ee562a86     	vnmls.f32	s5, s13, s12
     160: ee276a86     	vmul.f32	s12, s15, s12
     164: eef1fa10     	vmrs	APSR_nzcv, fpscr
     168: ee156aa3     	vnmls.f32	s12, s11, s7
     16c: eef03a42     	vmov.f32	s7, s4
     170: eebd6ac6     	vcvt.s32.f32	s12, s12
     174: eeb09a46     	vmov.f32	s18, s12
     178: ee063a10     	vmov	s12, r3
     17c: e597301c     	ldr	r3, [r7, #0x1c]
     180: eeb86ac6     	vcvt.f32.s32	s12, s12
     184: e0433001     	sub	r3, r3, r1
     188: e59d1088     	ldr	r1, [sp, #0x88]
     18c: ee563a86     	vnmls.f32	s7, s13, s12
     190: ee276a86     	vmul.f32	s12, s15, s12
     194: ee156a84     	vnmls.f32	s12, s11, s8
     198: eeb04a43     	vmov.f32	s8, s6
     19c: eebd6ac6     	vcvt.s32.f32	s12, s12
     1a0: eef09a46     	vmov.f32	s19, s12
     1a4: ee063a10     	vmov	s12, r3
     1a8: e5973008     	ldr	r3, [r7, #0x8]
     1ac: eeb86ac6     	vcvt.f32.s32	s12, s12
     1b0: e0433002     	sub	r3, r3, r2
     1b4: e5912004     	ldr	r2, [r1, #0x4]
     1b8: ee164a86     	vnmls.f32	s8, s13, s12
     1bc: ee276a86     	vmul.f32	s12, s15, s12
     1c0: ee156aa4     	vnmls.f32	s12, s11, s9
     1c4: eebd6ac6     	vcvt.s32.f32	s12, s12
     1c8: eeb0aa46     	vmov.f32	s20, s12
     1cc: ee063a10     	vmov	s12, r3
     1d0: e5913020     	ldr	r3, [r1, #0x20]
     1d4: eeb86ac6     	vcvt.f32.s32	s12, s12
     1d8: e0000093     	mul	r0, r3, r0
     1dc: ee677a86     	vmul.f32	s15, s15, s12
     1e0: ee167a86     	vnmls.f32	s14, s13, s12
     1e4: e0822100     	add	r2, r2, r0, lsl #2
     1e8: ee557a85     	vnmls.f32	s15, s11, s10
     1ec: e58d205c     	str	r2, [sp, #0x5c]
     1f0: e5912000     	ldr	r2, [r1]
     1f4: e0822080     	add	r2, r2, r0, lsl #1
     1f8: eefd7ae7     	vcvt.s32.f32	s15, s15
     1fc: e58d2058     	str	r2, [sp, #0x58]
     200: eef08a67     	vmov.f32	s17, s15
     204: eefd7ae2     	vcvt.s32.f32	s15, s5
     208: edcd7a12     	vstr	s15, [sp, #72]
     20c: eefd7ae3     	vcvt.s32.f32	s15, s7
     210: e59d1048     	ldr	r1, [sp, #0x48]
     214: edcd7a18     	vstr	s15, [sp, #96]
     218: eefd7ac4     	vcvt.s32.f32	s15, s8
     21c: e1a05081     	lsl	r5, r1, #1
     220: e59d2060     	ldr	r2, [sp, #0x60]
     224: e58d50a4     	str	r5, [sp, #0xa4]
     228: edcd7a19     	vstr	s15, [sp, #100]
     22c: e59d0064     	ldr	r0, [sp, #0x64]
     230: e1a09102     	lsl	r9, r2, #2
     234: eefd7ac7     	vcvt.s32.f32	s15, s14
     238: e1a04082     	lsl	r4, r2, #1
     23c: e58d9020     	str	r9, [sp, #0x20]
     240: e1a09101     	lsl	r9, r1, #2
     244: e0842002     	add	r2, r4, r2
     248: e58d40a0     	str	r4, [sp, #0xa0]
     24c: e1a06080     	lsl	r6, r0, #1
     250: e58d9024     	str	r9, [sp, #0x24]
     254: e1a09100     	lsl	r9, r0, #2
     258: ee173a90     	vmov	r3, s15
     25c: e58d609c     	str	r6, [sp, #0x9c]
     260: e58d9028     	str	r9, [sp, #0x28]
     264: e1a09103     	lsl	r9, r3, #2
     268: e58d2094     	str	r2, [sp, #0x94]
     26c: e0852001     	add	r2, r5, r1
     270: e58d2098     	str	r2, [sp, #0x98]
     274: e0862000     	add	r2, r6, r0
     278: e58d90a8     	str	r9, [sp, #0xa8]
     27c: e58d2090     	str	r2, [sp, #0x90]
     280: da00018f     	ble	0x8c4 <ZB_fillTriangleSmooth_DT0_DW1+0x8c4> @ imm = #0x63c
     284: e59e1004     	ldr	r1, [lr, #0x4]
     288: e1a0000e     	mov	r0, lr
     28c: e58d70e8     	str	r7, [sp, #0xe8]
     290: e5982004     	ldr	r2, [r8, #0x4]
     294: e1a05083     	lsl	r5, r3, #1
     298: e3a09000     	mov	r9, #0
     29c: e58d80ec     	str	r8, [sp, #0xec]
     2a0: e1a04008     	mov	r4, r8
     2a4: e58d5004     	str	r5, [sp, #0x4]
     2a8: e04cb002     	sub	r11, r12, r2
     2ac: e1a0a009     	mov	r10, r9
     2b0: e1a0600b     	mov	r6, r11
     2b4: e1a08003     	mov	r8, r3
     2b8: e1a05009     	mov	r5, r9
     2bc: e0411002     	sub	r1, r1, r2
     2c0: e3a02001     	mov	r2, #1
     2c4: e58d9084     	str	r9, [sp, #0x84]
     2c8: e58d2014     	str	r2, [sp, #0x14]
     2cc: e58de0e4     	str	lr, [sp, #0xe4]
     2d0: e5943000     	ldr	r3, [r4]
     2d4: e3510000     	cmp	r1, #0
     2d8: e1a0b003     	mov	r11, r3
     2dc: da000161     	ble	0x868 <ZB_fillTriangleSmooth_DT0_DW1+0x868> @ imm = #0x584
     2e0: e5902000     	ldr	r2, [r0]
     2e4: e0422003     	sub	r2, r2, r3
     2e8: e1a00802     	lsl	r0, r2, #16
     2ec: ebfffffe     	bl	0x2ec <ZB_fillTriangleSmooth_DT0_DW1+0x2ec> @ imm = #-0x8
     2f0: e6ff2070     	uxth	r2, r0
     2f4: e1a03840     	asr	r3, r0, #16
     2f8: e59d1048     	ldr	r1, [sp, #0x48]
     2fc: e58d208c     	str	r2, [sp, #0x8c]
     300: ee182a90     	vmov	r2, s17
     304: e58d30d0     	str	r3, [sp, #0xd0]
     308: e0222893     	mla	r2, r3, r8, r2
     30c: e58d20c0     	str	r2, [sp, #0xc0]
     310: ee192a10     	vmov	r2, s18
     314: e0222193     	mla	r2, r3, r1, r2
     318: e59d1060     	ldr	r1, [sp, #0x60]
     31c: e58d20c4     	str	r2, [sp, #0xc4]
     320: ee192a90     	vmov	r2, s19
     324: e0222193     	mla	r2, r3, r1, r2
     328: e59d1064     	ldr	r1, [sp, #0x64]
     32c: e58d20c8     	str	r2, [sp, #0xc8]
     330: ee1a2a10     	vmov	r2, s20
     334: e0222193     	mla	r2, r3, r1, r2
     338: e2833001     	add	r3, r3, #1
     33c: e58d30bc     	str	r3, [sp, #0xbc]
     340: e58d20cc     	str	r2, [sp, #0xcc]
     344: e59d3014     	ldr	r3, [sp, #0x14]
     348: e3530000     	cmp	r3, #0
     34c: e5943008     	ldr	r3, [r4, #0x8]
     350: e58d304c     	str	r3, [sp, #0x4c]
     354: e5943014     	ldr	r3, [r4, #0x14]
     358: e58d300c     	str	r3, [sp, #0xc]
     35c: e5943018     	ldr	r3, [r4, #0x18]
     360: e58d3010     	str	r3, [sp, #0x10]
     364: e594301c     	ldr	r3, [r4, #0x1c]
     368: e58d3008     	str	r3, [sp, #0x8]
     36c: 1a000158     	bne	0x8d4 <ZB_fillTriangleSmooth_DT0_DW1+0x8d4> @ imm = #0x560
     370: e3560000     	cmp	r6, #0
     374: da000108     	ble	0x79c <ZB_fillTriangleSmooth_DT0_DW1+0x79c> @ imm = #0x420
     378: e59d30c0     	ldr	r3, [sp, #0xc0]
     37c: e59d20c4     	ldr	r2, [sp, #0xc4]
     380: e59d9014     	ldr	r9, [sp, #0x14]
     384: e0883003     	add	r3, r8, r3
     388: e58d6050     	str	r6, [sp, #0x50]
     38c: e58d30ac     	str	r3, [sp, #0xac]
     390: e59d3048     	ldr	r3, [sp, #0x48]
     394: e58da054     	str	r10, [sp, #0x54]
     398: e58d60d4     	str	r6, [sp, #0xd4]
     39c: e0833002     	add	r3, r3, r2
     3a0: e59d20c8     	ldr	r2, [sp, #0xc8]
     3a4: e58d30b0     	str	r3, [sp, #0xb0]
     3a8: e59d3060     	ldr	r3, [sp, #0x60]
     3ac: e58da0d8     	str	r10, [sp, #0xd8]
     3b0: e58d50dc     	str	r5, [sp, #0xdc]
     3b4: e0833002     	add	r3, r3, r2
     3b8: e59d20cc     	ldr	r2, [sp, #0xcc]
     3bc: e58d30b4     	str	r3, [sp, #0xb4]
     3c0: e59d3064     	ldr	r3, [sp, #0x64]
     3c4: e58d70e0     	str	r7, [sp, #0xe0]
     3c8: e0833002     	add	r3, r3, r2
     3cc: e58d30b8     	str	r3, [sp, #0xb8]
     3d0: e59d3050     	ldr	r3, [sp, #0x50]
     3d4: e59d2058     	ldr	r2, [sp, #0x58]
     3d8: e59dc04c     	ldr	r12, [sp, #0x4c]
     3dc: e2433001     	sub	r3, r3, #1
     3e0: e58d3050     	str	r3, [sp, #0x50]
     3e4: e59d3054     	ldr	r3, [sp, #0x54]
     3e8: e082008b     	add	r0, r2, r11, lsl #1
     3ec: e1a0400c     	mov	r4, r12
     3f0: e06b1843     	rsb	r1, r11, r3, asr #16
     3f4: e59d305c     	ldr	r3, [sp, #0x5c]
     3f8: e3510002     	cmp	r1, #2
     3fc: e083310b     	add	r3, r3, r11, lsl #2
     400: da0000fd     	ble	0x7fc <ZB_fillTriangleSmooth_DT0_DW1+0x7fc> @ imm = #0x3f4
     404: e2837010     	add	r7, r3, #16
     408: e59da00c     	ldr	r10, [sp, #0xc]
     40c: e2413003     	sub	r3, r1, #3
     410: e2802010     	add	r2, r0, #16
     414: e1a0e123     	lsr	lr, r3, #2
     418: e2803008     	add	r3, r0, #8
     41c: e59d509c     	ldr	r5, [sp, #0x9c]
     420: e3a04000     	mov	r4, #0
     424: e1a0018e     	lsl	r0, lr, #3
     428: e58de068     	str	lr, [sp, #0x68]
     42c: e59de0a0     	ldr	lr, [sp, #0xa0]
     430: e1a06004     	mov	r6, r4
     434: e0822000     	add	r2, r2, r0
     438: e58d7070     	str	r7, [sp, #0x70]
     43c: e58d202c     	str	r2, [sp, #0x2c]
     440: e08c2008     	add	r2, r12, r8
     444: e58d206c     	str	r2, [sp, #0x6c]
     448: e59d2010     	ldr	r2, [sp, #0x10]
     44c: e59dc060     	ldr	r12, [sp, #0x60]
     450: e58d9074     	str	r9, [sp, #0x74]
     454: e082e00e     	add	lr, r2, lr
     458: e58db078     	str	r11, [sp, #0x78]
     45c: e58de040     	str	lr, [sp, #0x40]
     460: e59de0a4     	ldr	lr, [sp, #0xa4]
     464: e58d107c     	str	r1, [sp, #0x7c]
     468: e58d0080     	str	r0, [sp, #0x80]
     46c: e08ae00e     	add	lr, r10, lr
     470: e58de044     	str	lr, [sp, #0x44]
     474: e59de008     	ldr	lr, [sp, #0x8]
     478: e08e5005     	add	r5, lr, r5
     47c: e58d503c     	str	r5, [sp, #0x3c]
     480: e59d5094     	ldr	r5, [sp, #0x94]
     484: e0855002     	add	r5, r5, r2
     488: e08c2002     	add	r2, r12, r2
     48c: e58d5034     	str	r5, [sp, #0x34]
     490: e59d5098     	ldr	r5, [sp, #0x98]
     494: e59dc064     	ldr	r12, [sp, #0x64]
     498: e58d2014     	str	r2, [sp, #0x14]
     49c: e59d2008     	ldr	r2, [sp, #0x8]
     4a0: e085500a     	add	r5, r5, r10
     4a4: e58d5038     	str	r5, [sp, #0x38]
     4a8: e59d5090     	ldr	r5, [sp, #0x90]
     4ac: e08c2002     	add	r2, r12, r2
     4b0: e59dc06c     	ldr	r12, [sp, #0x6c]
     4b4: e58d2018     	str	r2, [sp, #0x18]
     4b8: e59d2048     	ldr	r2, [sp, #0x48]
     4bc: e085500e     	add	r5, r5, lr
     4c0: e58d5030     	str	r5, [sp, #0x30]
     4c4: e1a0e003     	mov	lr, r3
     4c8: e1a05004     	mov	r5, r4
     4cc: e58d306c     	str	r3, [sp, #0x6c]
     4d0: e082200a     	add	r2, r2, r10
     4d4: e58d201c     	str	r2, [sp, #0x1c]
     4d8: e59d3010     	ldr	r3, [sp, #0x10]
     4dc: e04ca008     	sub	r10, r12, r8
     4e0: e59db03c     	ldr	r11, [sp, #0x3c]
     4e4: e1a0c72c     	lsr	r12, r12, #14
     4e8: e59d9020     	ldr	r9, [sp, #0x20]
     4ec: e28ee008     	add	lr, lr, #8
     4f0: e0830005     	add	r0, r3, r5
     4f4: e59d3014     	ldr	r3, [sp, #0x14]
     4f8: e08bb004     	add	r11, r11, r4
     4fc: e1a00440     	asr	r0, r0, #8
     500: e7e7b85b     	ubfx	r11, r11, #0x10, #0x8
     504: e0831005     	add	r1, r3, r5
     508: e59d3040     	ldr	r3, [sp, #0x40]
     50c: e2000cff     	and	r0, r0, #65280
     510: e1a01441     	asr	r1, r1, #8
     514: e2877010     	add	r7, r7, #16
     518: e0832005     	add	r2, r3, r5
     51c: e59d3034     	ldr	r3, [sp, #0x34]
     520: e1a02442     	asr	r2, r2, #8
     524: e2011cff     	and	r1, r1, #65280
     528: e2022cff     	and	r2, r2, #65280
     52c: e0833005     	add	r3, r3, r5
     530: e182200b     	orr	r2, r2, r11
     534: e0855009     	add	r5, r5, r9
     538: e59db030     	ldr	r11, [sp, #0x30]
     53c: e1a03443     	asr	r3, r3, #8
     540: e59d9008     	ldr	r9, [sp, #0x8]
     544: e2033cff     	and	r3, r3, #65280
     548: e08bb004     	add	r11, r11, r4
     54c: e0899004     	add	r9, r9, r4
     550: e7e7b85b     	ubfx	r11, r11, #0x10, #0x8
     554: e7e79859     	ubfx	r9, r9, #0x10, #0x8
     558: e183300b     	orr	r3, r3, r11
     55c: e1800009     	orr	r0, r0, r9
     560: e59db028     	ldr	r11, [sp, #0x28]
     564: e59d9018     	ldr	r9, [sp, #0x18]
     568: e0899004     	add	r9, r9, r4
     56c: e084400b     	add	r4, r4, r11
     570: e59db00c     	ldr	r11, [sp, #0xc]
     574: e7e79859     	ubfx	r9, r9, #0x10, #0x8
     578: e1811009     	orr	r1, r1, r9
     57c: e59d9004     	ldr	r9, [sp, #0x4]
     580: e08bb006     	add	r11, r11, r6
     584: e20bb8ff     	and	r11, r11, #16711680
     588: e08a9009     	add	r9, r10, r9
     58c: e180000b     	orr	r0, r0, r11
     590: e1a0a72a     	lsr	r10, r10, #14
     594: e5070020     	str	r0, [r7, #-0x20]
     598: e59d001c     	ldr	r0, [sp, #0x1c]
     59c: e14ea1b0     	strh	r10, [lr, #-16]
     5a0: e0800006     	add	r0, r0, r6
     5a4: e20008ff     	and	r0, r0, #16711680
     5a8: e1811000     	orr	r1, r1, r0
     5ac: e507101c     	str	r1, [r7, #-0x1c]
     5b0: e59d1044     	ldr	r1, [sp, #0x44]
     5b4: e14ec0be     	strh	r12, [lr, #-14]
     5b8: e089c008     	add	r12, r9, r8
     5bc: e1a09729     	lsr	r9, r9, #14
     5c0: e0811006     	add	r1, r1, r6
     5c4: e20118ff     	and	r1, r1, #16711680
     5c8: e1822001     	orr	r2, r2, r1
     5cc: e5072018     	str	r2, [r7, #-0x18]
     5d0: e59d2038     	ldr	r2, [sp, #0x38]
     5d4: e14e90bc     	strh	r9, [lr, #-12]
     5d8: e59d9004     	ldr	r9, [sp, #0x4]
     5dc: e0822006     	add	r2, r2, r6
     5e0: e20228ff     	and	r2, r2, #16711680
     5e4: e1833002     	orr	r3, r3, r2
     5e8: e5073014     	str	r3, [r7, #-0x14]
     5ec: e59d3024     	ldr	r3, [sp, #0x24]
     5f0: e0866003     	add	r6, r6, r3
     5f4: e1a0372c     	lsr	r3, r12, #14
     5f8: e08cc009     	add	r12, r12, r9
     5fc: e14e30ba     	strh	r3, [lr, #-10]
     600: e59d302c     	ldr	r3, [sp, #0x2c]
     604: e153000e     	cmp	r3, lr
     608: 1affffb2     	bne	0x4d8 <ZB_fillTriangleSmooth_DT0_DW1+0x4d8> @ imm = #-0x138
     60c: e59d306c     	ldr	r3, [sp, #0x6c]
     610: e59d0080     	ldr	r0, [sp, #0x80]
     614: e59d2064     	ldr	r2, [sp, #0x64]
     618: e59d4048     	ldr	r4, [sp, #0x48]
     61c: e0800003     	add	r0, r0, r3
     620: e59d3018     	ldr	r3, [sp, #0x18]
     624: e59d5060     	ldr	r5, [sp, #0x60]
     628: e59d107c     	ldr	r1, [sp, #0x7c]
     62c: e59d9074     	ldr	r9, [sp, #0x74]
     630: e083a082     	add	r10, r3, r2, lsl #1
     634: e59d301c     	ldr	r3, [sp, #0x1c]
     638: e59db078     	ldr	r11, [sp, #0x78]
     63c: e08aa002     	add	r10, r10, r2
     640: e59d2068     	ldr	r2, [sp, #0x68]
     644: e083c084     	add	r12, r3, r4, lsl #1
     648: e59d3014     	ldr	r3, [sp, #0x14]
     64c: e08cc004     	add	r12, r12, r4
     650: e59d404c     	ldr	r4, [sp, #0x4c]
     654: e083e085     	add	lr, r3, r5, lsl #1
     658: e2413004     	sub	r3, r1, #4
     65c: e59d1028     	ldr	r1, [sp, #0x28]
     660: e08ee005     	add	lr, lr, r5
     664: e59d50a8     	ldr	r5, [sp, #0xa8]
     668: e02aa291     	mla	r10, r1, r2, r10
     66c: e59d1024     	ldr	r1, [sp, #0x24]
     670: e0844005     	add	r4, r4, r5
     674: e0244295     	mla	r4, r5, r2, r4
     678: e02cc192     	mla	r12, r2, r1, r12
     67c: e59d1020     	ldr	r1, [sp, #0x20]
     680: e02ee192     	mla	lr, r2, r1, lr
     684: e0431102     	sub	r1, r3, r2, lsl #2
     688: e59d3070     	ldr	r3, [sp, #0x70]
     68c: e0833202     	add	r3, r3, r2, lsl #4
     690: e3510000     	cmp	r1, #0
     694: ba000013     	blt	0x6e8 <ZB_fillTriangleSmooth_DT0_DW1+0x6e8> @ imm = #0x4c
     698: e59d5060     	ldr	r5, [sp, #0x60]
     69c: e0831101     	add	r1, r3, r1, lsl #2
     6a0: e59d6064     	ldr	r6, [sp, #0x64]
     6a4: e2432004     	sub	r2, r3, #4
     6a8: e1a0744e     	asr	r7, lr, #8
     6ac: e7e7385a     	ubfx	r3, r10, #0x10, #0x8
     6b0: e08ee005     	add	lr, lr, r5
     6b4: e2077cff     	and	r7, r7, #65280
     6b8: e08aa006     	add	r10, r10, r6
     6bc: e1873003     	orr	r3, r7, r3
     6c0: e20c78ff     	and	r7, r12, #16711680
     6c4: e1833007     	orr	r3, r3, r7
     6c8: e5a23004     	str	r3, [r2, #0x4]!
     6cc: e1510002     	cmp	r1, r2
     6d0: e59d3048     	ldr	r3, [sp, #0x48]
     6d4: e08cc003     	add	r12, r12, r3
     6d8: e1a03724     	lsr	r3, r4, #14
     6dc: e0844008     	add	r4, r4, r8
     6e0: e0c030b2     	strh	r3, [r0], #2
     6e4: 1affffef     	bne	0x6a8 <ZB_fillTriangleSmooth_DT0_DW1+0x6a8> @ imm = #-0x44
     6e8: e59d308c     	ldr	r3, [sp, #0x8c]
     6ec: e0899003     	add	r9, r9, r3
     6f0: e3590000     	cmp	r9, #0
     6f4: da00002d     	ble	0x7b0 <ZB_fillTriangleSmooth_DT0_DW1+0x7b0> @ imm = #0xb4
     6f8: e59d30bc     	ldr	r3, [sp, #0xbc]
     6fc: e2499801     	sub	r9, r9, #65536
     700: e59d20ac     	ldr	r2, [sp, #0xac]
     704: e08bb003     	add	r11, r11, r3
     708: e59d304c     	ldr	r3, [sp, #0x4c]
     70c: e0833002     	add	r3, r3, r2
     710: e59d20b0     	ldr	r2, [sp, #0xb0]
     714: e58d304c     	str	r3, [sp, #0x4c]
     718: e59d300c     	ldr	r3, [sp, #0xc]
     71c: e0833002     	add	r3, r3, r2
     720: e59d20b4     	ldr	r2, [sp, #0xb4]
     724: e58d300c     	str	r3, [sp, #0xc]
     728: e59d3010     	ldr	r3, [sp, #0x10]
     72c: e0833002     	add	r3, r3, r2
     730: e59d20b8     	ldr	r2, [sp, #0xb8]
     734: e58d3010     	str	r3, [sp, #0x10]
     738: e59d3008     	ldr	r3, [sp, #0x8]
     73c: e0833002     	add	r3, r3, r2
     740: e58d3008     	str	r3, [sp, #0x8]
     744: e59d3050     	ldr	r3, [sp, #0x50]
     748: e59d2084     	ldr	r2, [sp, #0x84]
     74c: e3530000     	cmp	r3, #0
     750: e59d3054     	ldr	r3, [sp, #0x54]
     754: e0833002     	add	r3, r3, r2
     758: e59d205c     	ldr	r2, [sp, #0x5c]
     75c: e58d3054     	str	r3, [sp, #0x54]
     760: e59d3088     	ldr	r3, [sp, #0x88]
     764: e5933020     	ldr	r3, [r3, #0x20]
     768: e0822103     	add	r2, r2, r3, lsl #2
     76c: e58d205c     	str	r2, [sp, #0x5c]
     770: e59d2058     	ldr	r2, [sp, #0x58]
     774: e0823083     	add	r3, r2, r3, lsl #1
     778: e58d3058     	str	r3, [sp, #0x58]
     77c: 1affff13     	bne	0x3d0 <ZB_fillTriangleSmooth_DT0_DW1+0x3d0> @ imm = #-0x3b4
     780: e59d60d4     	ldr	r6, [sp, #0xd4]
     784: e59da0d8     	ldr	r10, [sp, #0xd8]
     788: e59d3084     	ldr	r3, [sp, #0x84]
     78c: e59d50dc     	ldr	r5, [sp, #0xdc]
     790: e59d70e0     	ldr	r7, [sp, #0xe0]
     794: e58d9014     	str	r9, [sp, #0x14]
     798: e02aa396     	mla	r10, r6, r3, r10
     79c: e3550001     	cmp	r5, #1
     7a0: 1a000018     	bne	0x808 <ZB_fillTriangleSmooth_DT0_DW1+0x808> @ imm = #0x60
     7a4: e28dd0f4     	add	sp, sp, #244
     7a8: ecbd8b06     	vpop	{d8, d9, d10}
     7ac: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     7b0: e59d30d0     	ldr	r3, [sp, #0xd0]
     7b4: e59d20c0     	ldr	r2, [sp, #0xc0]
     7b8: e08bb003     	add	r11, r11, r3
     7bc: e59d304c     	ldr	r3, [sp, #0x4c]
     7c0: e0833002     	add	r3, r3, r2
     7c4: e59d20c4     	ldr	r2, [sp, #0xc4]
     7c8: e58d304c     	str	r3, [sp, #0x4c]
     7cc: e59d300c     	ldr	r3, [sp, #0xc]
     7d0: e0833002     	add	r3, r3, r2
     7d4: e59d20c8     	ldr	r2, [sp, #0xc8]
     7d8: e58d300c     	str	r3, [sp, #0xc]
     7dc: e59d3010     	ldr	r3, [sp, #0x10]
     7e0: e0833002     	add	r3, r3, r2
     7e4: e59d20cc     	ldr	r2, [sp, #0xcc]
     7e8: e58d3010     	str	r3, [sp, #0x10]
     7ec: e59d3008     	ldr	r3, [sp, #0x8]
     7f0: e0833002     	add	r3, r3, r2
     7f4: e58d3008     	str	r3, [sp, #0x8]
     7f8: eaffffd1     	b	0x744 <ZB_fillTriangleSmooth_DT0_DW1+0x744> @ imm = #-0xbc
     7fc: e28da008     	add	r10, sp, #8
     800: e89a5400     	ldm	r10, {r10, r12, lr}
     804: eaffffa1     	b	0x690 <ZB_fillTriangleSmooth_DT0_DW1+0x690> @ imm = #-0x17c
     808: e59d30e4     	ldr	r3, [sp, #0xe4]
     80c: e5972004     	ldr	r2, [r7, #0x4]
     810: eeb58ac0     	vcmpe.f32	s16, #0
     814: e5931004     	ldr	r1, [r3, #0x4]
     818: eef1fa10     	vmrs	APSR_nzcv, fpscr
     81c: e0411002     	sub	r1, r1, r2
     820: e2816001     	add	r6, r1, #1
     824: da000019     	ble	0x890 <ZB_fillTriangleSmooth_DT0_DW1+0x890> @ imm = #0x64
     828: e3510000     	cmp	r1, #0
     82c: e597a000     	ldr	r10, [r7]
     830: e3a04001     	mov	r4, #1
     834: da00001a     	ble	0x8a4 <ZB_fillTriangleSmooth_DT0_DW1+0x8a4> @ imm = #0x68
     838: e59d30e4     	ldr	r3, [sp, #0xe4]
     83c: e58d70ec     	str	r7, [sp, #0xec]
     840: e58d30e8     	str	r3, [sp, #0xe8]
     844: e59d30e8     	ldr	r3, [sp, #0xe8]
     848: e1a05004     	mov	r5, r4
     84c: e5930000     	ldr	r0, [r3]
     850: e040000a     	sub	r0, r0, r10
     854: e1a00800     	lsl	r0, r0, #16
     858: ebfffffe     	bl	0x858 <ZB_fillTriangleSmooth_DT0_DW1+0x858> @ imm = #-0x8
     85c: e58d0084     	str	r0, [sp, #0x84]
     860: e1a0a80a     	lsl	r10, r10, #16
     864: eafffec1     	b	0x370 <ZB_fillTriangleSmooth_DT0_DW1+0x370> @ imm = #-0x4fc
     868: e3a03000     	mov	r3, #0
     86c: e3a02001     	mov	r2, #1
     870: e58d308c     	str	r3, [sp, #0x8c]
     874: ed8daa33     	vstr	s20, [sp, #204]
     878: edcd9a32     	vstr	s19, [sp, #200]
     87c: ed8d9a31     	vstr	s18, [sp, #196]
     880: edcd8a30     	vstr	s17, [sp, #192]
     884: e58d20bc     	str	r2, [sp, #0xbc]
     888: e58d30d0     	str	r3, [sp, #0xd0]
     88c: eafffeac     	b	0x344 <ZB_fillTriangleSmooth_DT0_DW1+0x344> @ imm = #-0x550
     890: e58d5014     	str	r5, [sp, #0x14]
     894: e1a04007     	mov	r4, r7
     898: e59d00e4     	ldr	r0, [sp, #0xe4]
     89c: e3a05001     	mov	r5, #1
     8a0: eafffe8a     	b	0x2d0 <ZB_fillTriangleSmooth_DT0_DW1+0x2d0> @ imm = #-0x5d8
     8a4: e3560000     	cmp	r6, #0
     8a8: daffffbd     	ble	0x7a4 <ZB_fillTriangleSmooth_DT0_DW1+0x7a4> @ imm = #-0x10c
     8ac: e58d5084     	str	r5, [sp, #0x84]
     8b0: e1a0a80a     	lsl	r10, r10, #16
     8b4: e1a05004     	mov	r5, r4
     8b8: e58d70ec     	str	r7, [sp, #0xec]
     8bc: e58d30e8     	str	r3, [sp, #0xe8]
     8c0: eafffeac     	b	0x378 <ZB_fillTriangleSmooth_DT0_DW1+0x378> @ imm = #-0x550
     8c4: e1a0100c     	mov	r1, r12
     8c8: e1a00007     	mov	r0, r7
     8cc: e58de0e8     	str	lr, [sp, #0xe8]
     8d0: eafffe6e     	b	0x290 <ZB_fillTriangleSmooth_DT0_DW1+0x290> @ imm = #-0x648
     8d4: e59d30e8     	ldr	r3, [sp, #0xe8]
     8d8: e5931004     	ldr	r1, [r3, #0x4]
     8dc: e59d30ec     	ldr	r3, [sp, #0xec]
     8e0: e5932004     	ldr	r2, [r3, #0x4]
     8e4: e593a000     	ldr	r10, [r3]
     8e8: e0411002     	sub	r1, r1, r2
     8ec: e3510000     	cmp	r1, #0
     8f0: c3a03000     	movgt	r3, #0
     8f4: c1a04005     	movgt	r4, r5
     8f8: c58d3014     	strgt	r3, [sp, #0x14]
     8fc: caffffd0     	bgt	0x844 <ZB_fillTriangleSmooth_DT0_DW1+0x844> @ imm = #-0xc0
     900: e3a03000     	mov	r3, #0
     904: e58d3014     	str	r3, [sp, #0x14]
     908: e58d3084     	str	r3, [sp, #0x84]
     90c: eaffffd3     	b	0x860 <ZB_fillTriangleSmooth_DT0_DW1+0x860> @ imm = #-0xb4

Disassembly of section .text.ZB_fillTriangleSmooth_DT1_DW0:

00000000 <ZB_fillTriangleSmooth_DT1_DW0>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a04003     	mov	r4, r3
       8: ed2d8b06     	vpush	{d8, d9, d10}
       c: e24dd0f4     	sub	sp, sp, #244
      10: e592c004     	ldr	r12, [r2, #0x4]
      14: e58d30d0     	str	r3, [sp, #0xd0]
      18: e5913004     	ldr	r3, [r1, #0x4]
      1c: e58d0080     	str	r0, [sp, #0x80]
      20: e5940004     	ldr	r0, [r4, #0x4]
      24: e15c0003     	cmp	r12, r3
      28: a1a0e003     	movge	lr, r3
      2c: b1a0e00c     	movlt	lr, r12
      30: a1a0300c     	movge	r3, r12
      34: b1a05002     	movlt	r5, r2
      38: a1a05001     	movge	r5, r1
      3c: b1a08001     	movlt	r8, r1
      40: a1a08002     	movge	r8, r2
      44: e150000e     	cmp	r0, lr
      48: e1a0c00e     	mov	r12, lr
      4c: e58d50cc     	str	r5, [sp, #0xcc]
      50: ba000007     	blt	0x74 <ZB_fillTriangleSmooth_DT1_DW0+0x74> @ imm = #0x1c
      54: e1500003     	cmp	r0, r3
      58: aa00021e     	bge	0x8d8 <ZB_fillTriangleSmooth_DT1_DW0+0x8d8> @ imm = #0x878
      5c: e59d20d0     	ldr	r2, [sp, #0xd0]
      60: e1a0c000     	mov	r12, r0
      64: e59d10cc     	ldr	r1, [sp, #0xcc]
      68: e1a0000e     	mov	r0, lr
      6c: e58d20cc     	str	r2, [sp, #0xcc]
      70: e58d10d0     	str	r1, [sp, #0xd0]
      74: e0433000     	sub	r3, r3, r0
      78: e59d50cc     	ldr	r5, [sp, #0xcc]
      7c: ee073a90     	vmov	s15, r3
      80: e59d30d0     	ldr	r3, [sp, #0xd0]
      84: eef86ae7     	vcvt.f32.s32	s13, s15
      88: e5932000     	ldr	r2, [r3]
      8c: e5983000     	ldr	r3, [r8]
      90: e0433002     	sub	r3, r3, r2
      94: ee073a90     	vmov	s15, r3
      98: e04c3000     	sub	r3, r12, r0
      9c: ee073a10     	vmov	s14, r3
      a0: e59d30cc     	ldr	r3, [sp, #0xcc]
      a4: eef87ae7     	vcvt.f32.s32	s15, s15
      a8: eeb87ac7     	vcvt.f32.s32	s14, s14
      ac: e5933000     	ldr	r3, [r3]
      b0: e0433002     	sub	r3, r3, r2
      b4: ee278a27     	vmul.f32	s16, s14, s15
      b8: e59d20d0     	ldr	r2, [sp, #0xd0]
      bc: ee053a90     	vmov	s11, r3
      c0: e5983014     	ldr	r3, [r8, #0x14]
      c4: eef85ae5     	vcvt.f32.s32	s11, s11
      c8: e5924014     	ldr	r4, [r2, #0x14]
      cc: e592e018     	ldr	lr, [r2, #0x18]
      d0: e592101c     	ldr	r1, [r2, #0x1c]
      d4: ee158aa6     	vnmls.f32	s16, s11, s13
      d8: e0433004     	sub	r3, r3, r4
      dc: e5922008     	ldr	r2, [r2, #0x8]
      e0: eeb58a40     	vcmp.f32	s16, #0
      e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
      e8: 1eb76a00     	vmovne.f32	s12, #1.000000e+00
      ec: 1e868a08     	vdivne.f32	s16, s12, s16
      f0: ee063a10     	vmov	s12, r3
      f4: e5983018     	ldr	r3, [r8, #0x18]
      f8: eef83ac6     	vcvt.f32.s32	s7, s12
      fc: e043300e     	sub	r3, r3, lr
     100: ee063a10     	vmov	s12, r3
     104: e598301c     	ldr	r3, [r8, #0x1c]
     108: eeb84ac6     	vcvt.f32.s32	s8, s12
     10c: e0433001     	sub	r3, r3, r1
     110: ee063a10     	vmov	s12, r3
     114: e5983008     	ldr	r3, [r8, #0x8]
     118: eef84ac6     	vcvt.f32.s32	s9, s12
     11c: ee287a07     	vmul.f32	s14, s16, s14
     120: e0433002     	sub	r3, r3, r2
     124: ee686a26     	vmul.f32	s13, s16, s13
     128: ee687a27     	vmul.f32	s15, s16, s15
     12c: ee063a10     	vmov	s12, r3
     130: e5953014     	ldr	r3, [r5, #0x14]
     134: eeb85ac6     	vcvt.f32.s32	s10, s12
     138: ee672a23     	vmul.f32	s5, s14, s7
     13c: e0433004     	sub	r3, r3, r4
     140: ee685a25     	vmul.f32	s11, s16, s11
     144: ee272a04     	vmul.f32	s4, s14, s8
     148: ee063a10     	vmov	s12, r3
     14c: e5953018     	ldr	r3, [r5, #0x18]
     150: eeb86ac6     	vcvt.f32.s32	s12, s12
     154: ee273a24     	vmul.f32	s6, s14, s9
     158: e043300e     	sub	r3, r3, lr
     15c: ee277a05     	vmul.f32	s14, s14, s10
     160: eeb58ac0     	vcmpe.f32	s16, #0
     164: ee562a86     	vnmls.f32	s5, s13, s12
     168: ee276a86     	vmul.f32	s12, s15, s12
     16c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     170: ee156aa3     	vnmls.f32	s12, s11, s7
     174: eef03a42     	vmov.f32	s7, s4
     178: eebd6ac6     	vcvt.s32.f32	s12, s12
     17c: eeb09a46     	vmov.f32	s18, s12
     180: ee063a10     	vmov	s12, r3
     184: e595301c     	ldr	r3, [r5, #0x1c]
     188: eeb86ac6     	vcvt.f32.s32	s12, s12
     18c: e0433001     	sub	r3, r3, r1
     190: e1a01005     	mov	r1, r5
     194: ee563a86     	vnmls.f32	s7, s13, s12
     198: ee276a86     	vmul.f32	s12, s15, s12
     19c: ee156a84     	vnmls.f32	s12, s11, s8
     1a0: eeb04a43     	vmov.f32	s8, s6
     1a4: eebd6ac6     	vcvt.s32.f32	s12, s12
     1a8: eef09a46     	vmov.f32	s19, s12
     1ac: ee063a10     	vmov	s12, r3
     1b0: e5953008     	ldr	r3, [r5, #0x8]
     1b4: eeb86ac6     	vcvt.f32.s32	s12, s12
     1b8: e0433002     	sub	r3, r3, r2
     1bc: e59d2080     	ldr	r2, [sp, #0x80]
     1c0: ee164a86     	vnmls.f32	s8, s13, s12
     1c4: ee276a86     	vmul.f32	s12, s15, s12
     1c8: ee156aa4     	vnmls.f32	s12, s11, s9
     1cc: eebd6ac6     	vcvt.s32.f32	s12, s12
     1d0: eeb0aa46     	vmov.f32	s20, s12
     1d4: ee063a10     	vmov	s12, r3
     1d8: e5923020     	ldr	r3, [r2, #0x20]
     1dc: eeb86ac6     	vcvt.f32.s32	s12, s12
     1e0: e0000093     	mul	r0, r3, r0
     1e4: e5923004     	ldr	r3, [r2, #0x4]
     1e8: ee677a86     	vmul.f32	s15, s15, s12
     1ec: ee167a86     	vnmls.f32	s14, s13, s12
     1f0: e0833100     	add	r3, r3, r0, lsl #2
     1f4: ee557a85     	vnmls.f32	s15, s11, s10
     1f8: e58d3058     	str	r3, [sp, #0x58]
     1fc: e5923000     	ldr	r3, [r2]
     200: e0833080     	add	r3, r3, r0, lsl #1
     204: eefd7ae7     	vcvt.s32.f32	s15, s15
     208: e58d3054     	str	r3, [sp, #0x54]
     20c: eef08a67     	vmov.f32	s17, s15
     210: eefd7ae2     	vcvt.s32.f32	s15, s5
     214: edcd7a0a     	vstr	s15, [sp, #40]
     218: eefd7ae3     	vcvt.s32.f32	s15, s7
     21c: e59d0028     	ldr	r0, [sp, #0x28]
     220: edcd7a17     	vstr	s15, [sp, #92]
     224: e59d305c     	ldr	r3, [sp, #0x5c]
     228: e1a06080     	lsl	r6, r0, #1
     22c: eefd7ac4     	vcvt.s32.f32	s15, s8
     230: e58d6090     	str	r6, [sp, #0x90]
     234: e1a04083     	lsl	r4, r3, #1
     238: e1a02103     	lsl	r2, r3, #2
     23c: edcd7a35     	vstr	s15, [sp, #212]
     240: e58d408c     	str	r4, [sp, #0x8c]
     244: e0843003     	add	r3, r4, r3
     248: e59de0d4     	ldr	lr, [sp, #0xd4]
     24c: eefd7ac7     	vcvt.s32.f32	s15, s14
     250: e58d2008     	str	r2, [sp, #0x8]
     254: e1a02100     	lsl	r2, r0, #2
     258: e58d3098     	str	r3, [sp, #0x98]
     25c: e0863000     	add	r3, r6, r0
     260: e1a0708e     	lsl	r7, lr, #1
     264: e58d200c     	str	r2, [sp, #0xc]
     268: ee175a90     	vmov	r5, s15
     26c: e1a0210e     	lsl	r2, lr, #2
     270: e1a09085     	lsl	r9, r5, #1
     274: e58d309c     	str	r3, [sp, #0x9c]
     278: e087300e     	add	r3, r7, lr
     27c: e58d2010     	str	r2, [sp, #0x10]
     280: e58d7088     	str	r7, [sp, #0x88]
     284: e1a02105     	lsl	r2, r5, #2
     288: e58d90a0     	str	r9, [sp, #0xa0]
     28c: e58d3094     	str	r3, [sp, #0x94]
     290: da000197     	ble	0x8f4 <ZB_fillTriangleSmooth_DT1_DW0+0x8f4> @ imm = #0x65c
     294: e5983004     	ldr	r3, [r8, #0x4]
     298: e1a00008     	mov	r0, r8
     29c: e58d10ec     	str	r1, [sp, #0xec]
     2a0: e59d40d0     	ldr	r4, [sp, #0xd0]
     2a4: e3a09000     	mov	r9, #0
     2a8: e58d20e8     	str	r2, [sp, #0xe8]
     2ac: e1a0a005     	mov	r10, r5
     2b0: e1a02009     	mov	r2, r9
     2b4: e58d907c     	str	r9, [sp, #0x7c]
     2b8: e5941004     	ldr	r1, [r4, #0x4]
     2bc: e04cb001     	sub	r11, r12, r1
     2c0: e0431001     	sub	r1, r3, r1
     2c4: e3a03001     	mov	r3, #1
     2c8: e58d3044     	str	r3, [sp, #0x44]
     2cc: e594c000     	ldr	r12, [r4]
     2d0: e3510000     	cmp	r1, #0
     2d4: e58dc050     	str	r12, [sp, #0x50]
     2d8: da000167     	ble	0x87c <ZB_fillTriangleSmooth_DT1_DW0+0x87c> @ imm = #0x59c
     2dc: e5903000     	ldr	r3, [r0]
     2e0: e58d2000     	str	r2, [sp]
     2e4: e043300c     	sub	r3, r3, r12
     2e8: e1a00803     	lsl	r0, r3, #16
     2ec: ebfffffe     	bl	0x2ec <ZB_fillTriangleSmooth_DT1_DW0+0x2ec> @ imm = #-0x8
     2f0: e6ff2070     	uxth	r2, r0
     2f4: e1a03840     	asr	r3, r0, #16
     2f8: e59d1028     	ldr	r1, [sp, #0x28]
     2fc: e58d2084     	str	r2, [sp, #0x84]
     300: ee182a90     	vmov	r2, s17
     304: e58d30c8     	str	r3, [sp, #0xc8]
     308: e0222a93     	mla	r2, r3, r10, r2
     30c: e58d20b8     	str	r2, [sp, #0xb8]
     310: ee192a10     	vmov	r2, s18
     314: e0222193     	mla	r2, r3, r1, r2
     318: e59d105c     	ldr	r1, [sp, #0x5c]
     31c: e58d20bc     	str	r2, [sp, #0xbc]
     320: ee192a90     	vmov	r2, s19
     324: e0222193     	mla	r2, r3, r1, r2
     328: e59d10d4     	ldr	r1, [sp, #0xd4]
     32c: e58d20c0     	str	r2, [sp, #0xc0]
     330: ee1a2a10     	vmov	r2, s20
     334: e0222193     	mla	r2, r3, r1, r2
     338: e2833001     	add	r3, r3, #1
     33c: e58d30b4     	str	r3, [sp, #0xb4]
     340: e58d20c4     	str	r2, [sp, #0xc4]
     344: e59d2000     	ldr	r2, [sp]
     348: e59d3044     	ldr	r3, [sp, #0x44]
     34c: e3530000     	cmp	r3, #0
     350: e5943008     	ldr	r3, [r4, #0x8]
     354: e58d3040     	str	r3, [sp, #0x40]
     358: e5943014     	ldr	r3, [r4, #0x14]
     35c: e58d3004     	str	r3, [sp, #0x4]
     360: e5943018     	ldr	r3, [r4, #0x18]
     364: e58d3000     	str	r3, [sp]
     368: e594301c     	ldr	r3, [r4, #0x1c]
     36c: e58d301c     	str	r3, [sp, #0x1c]
     370: 1a000163     	bne	0x904 <ZB_fillTriangleSmooth_DT1_DW0+0x904> @ imm = #0x58c
     374: e35b0000     	cmp	r11, #0
     378: da000109     	ble	0x7a4 <ZB_fillTriangleSmooth_DT1_DW0+0x7a4> @ imm = #0x424
     37c: e59d30b8     	ldr	r3, [sp, #0xb8]
     380: e59d10bc     	ldr	r1, [sp, #0xbc]
     384: e58d90e0     	str	r9, [sp, #0xe0]
     388: e08a3003     	add	r3, r10, r3
     38c: e59d90e8     	ldr	r9, [sp, #0xe8]
     390: e58d30a4     	str	r3, [sp, #0xa4]
     394: e59d3028     	ldr	r3, [sp, #0x28]
     398: e58db048     	str	r11, [sp, #0x48]
     39c: e58db0d8     	str	r11, [sp, #0xd8]
     3a0: e0833001     	add	r3, r3, r1
     3a4: e59d10c0     	ldr	r1, [sp, #0xc0]
     3a8: e58d30a8     	str	r3, [sp, #0xa8]
     3ac: e59d305c     	ldr	r3, [sp, #0x5c]
     3b0: e58d204c     	str	r2, [sp, #0x4c]
     3b4: e58d20dc     	str	r2, [sp, #0xdc]
     3b8: e0833001     	add	r3, r3, r1
     3bc: e59d10c4     	ldr	r1, [sp, #0xc4]
     3c0: e58d30ac     	str	r3, [sp, #0xac]
     3c4: e59d30d4     	ldr	r3, [sp, #0xd4]
     3c8: e58d80e4     	str	r8, [sp, #0xe4]
     3cc: e1a0b003     	mov	r11, r3
     3d0: e0831001     	add	r1, r3, r1
     3d4: e58d10b0     	str	r1, [sp, #0xb0]
     3d8: e59d3048     	ldr	r3, [sp, #0x48]
     3dc: e59d204c     	ldr	r2, [sp, #0x4c]
     3e0: e59d4040     	ldr	r4, [sp, #0x40]
     3e4: e2433001     	sub	r3, r3, #1
     3e8: e58d3048     	str	r3, [sp, #0x48]
     3ec: e59d3050     	ldr	r3, [sp, #0x50]
     3f0: e0638842     	rsb	r8, r3, r2, asr #16
     3f4: e59d2058     	ldr	r2, [sp, #0x58]
     3f8: e3580002     	cmp	r8, #2
     3fc: e082c103     	add	r12, r2, r3, lsl #2
     400: e59d2054     	ldr	r2, [sp, #0x54]
     404: e0823083     	add	r3, r2, r3, lsl #1
     408: e1a02004     	mov	r2, r4
     40c: da0000fe     	ble	0x80c <ZB_fillTriangleSmooth_DT1_DW0+0x80c> @ imm = #0x3f8
     410: e2482003     	sub	r2, r8, #3
     414: e2831008     	add	r1, r3, #8
     418: e1a02122     	lsr	r2, r2, #2
     41c: e2833010     	add	r3, r3, #16
     420: e59de090     	ldr	lr, [sp, #0x90]
     424: e28c5010     	add	r5, r12, #16
     428: e1a00182     	lsl	r0, r2, #3
     42c: e59d7088     	ldr	r7, [sp, #0x88]
     430: e3a0c000     	mov	r12, #0
     434: e58d5064     	str	r5, [sp, #0x64]
     438: e0833000     	add	r3, r3, r0
     43c: e58d0060     	str	r0, [sp, #0x60]
     440: e58d3018     	str	r3, [sp, #0x18]
     444: e59d30a0     	ldr	r3, [sp, #0xa0]
     448: e59d008c     	ldr	r0, [sp, #0x8c]
     44c: e58d1068     	str	r1, [sp, #0x68]
     450: e0846003     	add	r6, r4, r3
     454: e59d3000     	ldr	r3, [sp]
     458: e58dc06c     	str	r12, [sp, #0x6c]
     45c: e58db070     	str	r11, [sp, #0x70]
     460: e0830000     	add	r0, r3, r0
     464: e58d8074     	str	r8, [sp, #0x74]
     468: e58d0030     	str	r0, [sp, #0x30]
     46c: e59d0004     	ldr	r0, [sp, #0x4]
     470: e58d2078     	str	r2, [sp, #0x78]
     474: e080e00e     	add	lr, r0, lr
     478: e58de034     	str	lr, [sp, #0x34]
     47c: e59de01c     	ldr	lr, [sp, #0x1c]
     480: e08e7007     	add	r7, lr, r7
     484: e58d702c     	str	r7, [sp, #0x2c]
     488: e59d7098     	ldr	r7, [sp, #0x98]
     48c: e0837007     	add	r7, r3, r7
     490: e58d7014     	str	r7, [sp, #0x14]
     494: e59d709c     	ldr	r7, [sp, #0x9c]
     498: e0807007     	add	r7, r0, r7
     49c: e1a0000e     	mov	r0, lr
     4a0: e58d703c     	str	r7, [sp, #0x3c]
     4a4: e59d7094     	ldr	r7, [sp, #0x94]
     4a8: e08e7007     	add	r7, lr, r7
     4ac: e1a0e00c     	mov	lr, r12
     4b0: e58d7038     	str	r7, [sp, #0x38]
     4b4: e59d705c     	ldr	r7, [sp, #0x5c]
     4b8: e0837007     	add	r7, r3, r7
     4bc: e080300b     	add	r3, r0, r11
     4c0: e58d3024     	str	r3, [sp, #0x24]
     4c4: e59d0028     	ldr	r0, [sp, #0x28]
     4c8: e59d3004     	ldr	r3, [sp, #0x4]
     4cc: e0833000     	add	r3, r3, r0
     4d0: e1a0000c     	mov	r0, r12
     4d4: e58d3020     	str	r3, [sp, #0x20]
     4d8: e15130b8     	ldrh	r3, [r1, #-8]
     4dc: e084800a     	add	r8, r4, r10
     4e0: e1530724     	cmp	r3, r4, lsr #14
     4e4: e59d3000     	ldr	r3, [sp]
     4e8: e0832000     	add	r2, r3, r0
     4ec: e0873000     	add	r3, r7, r0
     4f0: e1a02442     	asr	r2, r2, #8
     4f4: e1a03443     	asr	r3, r3, #8
     4f8: e2022cff     	and	r2, r2, #65280
     4fc: 8a000008     	bhi	0x524 <ZB_fillTriangleSmooth_DT1_DW0+0x524> @ imm = #0x20
     500: e59db01c     	ldr	r11, [sp, #0x1c]
     504: e08bb00c     	add	r11, r11, r12
     508: e7e7b85b     	ubfx	r11, r11, #0x10, #0x8
     50c: e182200b     	orr	r2, r2, r11
     510: e59db004     	ldr	r11, [sp, #0x4]
     514: e08bb00e     	add	r11, r11, lr
     518: e20bb8ff     	and	r11, r11, #16711680
     51c: e182200b     	orr	r2, r2, r11
     520: e5052010     	str	r2, [r5, #-0x10]
     524: e15120b6     	ldrh	r2, [r1, #-6]
     528: e2033cff     	and	r3, r3, #65280
     52c: e0844009     	add	r4, r4, r9
     530: e1520728     	cmp	r2, r8, lsr #14
     534: 8a000008     	bhi	0x55c <ZB_fillTriangleSmooth_DT1_DW0+0x55c> @ imm = #0x20
     538: e59d2024     	ldr	r2, [sp, #0x24]
     53c: e082200c     	add	r2, r2, r12
     540: e7e72852     	ubfx	r2, r2, #0x10, #0x8
     544: e1833002     	orr	r3, r3, r2
     548: e59d2020     	ldr	r2, [sp, #0x20]
     54c: e082200e     	add	r2, r2, lr
     550: e20228ff     	and	r2, r2, #16711680
     554: e1833002     	orr	r3, r3, r2
     558: e505300c     	str	r3, [r5, #-0xc]
     55c: e15130b4     	ldrh	r3, [r1, #-4]
     560: e1530726     	cmp	r3, r6, lsr #14
     564: 8a00000c     	bhi	0x59c <ZB_fillTriangleSmooth_DT1_DW0+0x59c> @ imm = #0x30
     568: e59d3030     	ldr	r3, [sp, #0x30]
     56c: e59d202c     	ldr	r2, [sp, #0x2c]
     570: e0833000     	add	r3, r3, r0
     574: e1a03443     	asr	r3, r3, #8
     578: e082200c     	add	r2, r2, r12
     57c: e7e72852     	ubfx	r2, r2, #0x10, #0x8
     580: e2033cff     	and	r3, r3, #65280
     584: e1833002     	orr	r3, r3, r2
     588: e59d2034     	ldr	r2, [sp, #0x34]
     58c: e082200e     	add	r2, r2, lr
     590: e20228ff     	and	r2, r2, #16711680
     594: e1833002     	orr	r3, r3, r2
     598: e5053008     	str	r3, [r5, #-0x8]
     59c: e15120b2     	ldrh	r2, [r1, #-2]
     5a0: e2811008     	add	r1, r1, #8
     5a4: e59d8008     	ldr	r8, [sp, #0x8]
     5a8: e59d3014     	ldr	r3, [sp, #0x14]
     5ac: e0833000     	add	r3, r3, r0
     5b0: e0800008     	add	r0, r0, r8
     5b4: e086800a     	add	r8, r6, r10
     5b8: e1a03443     	asr	r3, r3, #8
     5bc: e1520728     	cmp	r2, r8, lsr #14
     5c0: e0866009     	add	r6, r6, r9
     5c4: e2033cff     	and	r3, r3, #65280
     5c8: 8a000008     	bhi	0x5f0 <ZB_fillTriangleSmooth_DT1_DW0+0x5f0> @ imm = #0x20
     5cc: e59d2038     	ldr	r2, [sp, #0x38]
     5d0: e082200c     	add	r2, r2, r12
     5d4: e7e72852     	ubfx	r2, r2, #0x10, #0x8
     5d8: e1833002     	orr	r3, r3, r2
     5dc: e59d203c     	ldr	r2, [sp, #0x3c]
     5e0: e082200e     	add	r2, r2, lr
     5e4: e20228ff     	and	r2, r2, #16711680
     5e8: e1833002     	orr	r3, r3, r2
     5ec: e5053004     	str	r3, [r5, #-0x4]
     5f0: e59d3018     	ldr	r3, [sp, #0x18]
     5f4: e2855010     	add	r5, r5, #16
     5f8: e1530001     	cmp	r3, r1
     5fc: e59d300c     	ldr	r3, [sp, #0xc]
     600: e08ee003     	add	lr, lr, r3
     604: e59d3010     	ldr	r3, [sp, #0x10]
     608: e08cc003     	add	r12, r12, r3
     60c: 1affffb1     	bne	0x4d8 <ZB_fillTriangleSmooth_DT1_DW0+0x4d8> @ imm = #-0x13c
     610: e59d1068     	ldr	r1, [sp, #0x68]
     614: e59d3060     	ldr	r3, [sp, #0x60]
     618: e59db070     	ldr	r11, [sp, #0x70]
     61c: e59de028     	ldr	lr, [sp, #0x28]
     620: e0833001     	add	r3, r3, r1
     624: e59d1024     	ldr	r1, [sp, #0x24]
     628: e59d2078     	ldr	r2, [sp, #0x78]
     62c: e59d405c     	ldr	r4, [sp, #0x5c]
     630: e59d8074     	ldr	r8, [sp, #0x74]
     634: e081008b     	add	r0, r1, r11, lsl #1
     638: e59d1020     	ldr	r1, [sp, #0x20]
     63c: e080000b     	add	r0, r0, r11
     640: e0877084     	add	r7, r7, r4, lsl #1
     644: e248c004     	sub	r12, r8, #4
     648: e081108e     	add	r1, r1, lr, lsl #1
     64c: e0877004     	add	r7, r7, r4
     650: e04c8102     	sub	r8, r12, r2, lsl #2
     654: e59dc064     	ldr	r12, [sp, #0x64]
     658: e081100e     	add	r1, r1, lr
     65c: e59de010     	ldr	lr, [sp, #0x10]
     660: e08cc202     	add	r12, r12, r2, lsl #4
     664: e020029e     	mla	r0, lr, r2, r0
     668: e59de00c     	ldr	lr, [sp, #0xc]
     66c: e0211e92     	mla	r1, r2, lr, r1
     670: e59de008     	ldr	lr, [sp, #0x8]
     674: e0277e92     	mla	r7, r2, lr, r7
     678: e59de040     	ldr	lr, [sp, #0x40]
     67c: e08ee009     	add	lr, lr, r9
     680: e022e299     	mla	r2, r9, r2, lr
     684: e3580000     	cmp	r8, #0
     688: ba000013     	blt	0x6dc <ZB_fillTriangleSmooth_DT1_DW0+0x6dc> @ imm = #0x4c
     68c: e59d605c     	ldr	r6, [sp, #0x5c]
     690: e0834088     	add	r4, r3, r8, lsl #1
     694: e243e002     	sub	lr, r3, #2
     698: e1fe50b2     	ldrh	r5, [lr, #2]!
     69c: e1a03447     	asr	r3, r7, #8
     6a0: e7e78850     	ubfx	r8, r0, #0x10, #0x8
     6a4: e2033cff     	and	r3, r3, #65280
     6a8: e0877006     	add	r7, r7, r6
     6ac: e1833008     	orr	r3, r3, r8
     6b0: e20188ff     	and	r8, r1, #16711680
     6b4: e1550722     	cmp	r5, r2, lsr #14
     6b8: e1833008     	orr	r3, r3, r8
     6bc: e59d5028     	ldr	r5, [sp, #0x28]
     6c0: e080000b     	add	r0, r0, r11
     6c4: 958c3000     	strls	r3, [r12]
     6c8: e154000e     	cmp	r4, lr
     6cc: e082200a     	add	r2, r2, r10
     6d0: e28cc004     	add	r12, r12, #4
     6d4: e0811005     	add	r1, r1, r5
     6d8: 1affffee     	bne	0x698 <ZB_fillTriangleSmooth_DT1_DW0+0x698> @ imm = #-0x48
     6dc: e59d3044     	ldr	r3, [sp, #0x44]
     6e0: e59d2084     	ldr	r2, [sp, #0x84]
     6e4: e0833002     	add	r3, r3, r2
     6e8: e3530000     	cmp	r3, #0
     6ec: e58d3044     	str	r3, [sp, #0x44]
     6f0: da000030     	ble	0x7b8 <ZB_fillTriangleSmooth_DT1_DW0+0x7b8> @ imm = #0xc0
     6f4: e59d3044     	ldr	r3, [sp, #0x44]
     6f8: e59d20b4     	ldr	r2, [sp, #0xb4]
     6fc: e2433801     	sub	r3, r3, #65536
     700: e58d3044     	str	r3, [sp, #0x44]
     704: e59d3050     	ldr	r3, [sp, #0x50]
     708: e0833002     	add	r3, r3, r2
     70c: e59d20a4     	ldr	r2, [sp, #0xa4]
     710: e58d3050     	str	r3, [sp, #0x50]
     714: e59d3040     	ldr	r3, [sp, #0x40]
     718: e0833002     	add	r3, r3, r2
     71c: e59d20a8     	ldr	r2, [sp, #0xa8]
     720: e58d3040     	str	r3, [sp, #0x40]
     724: e59d3004     	ldr	r3, [sp, #0x4]
     728: e0833002     	add	r3, r3, r2
     72c: e59d20ac     	ldr	r2, [sp, #0xac]
     730: e58d3004     	str	r3, [sp, #0x4]
     734: e59d3000     	ldr	r3, [sp]
     738: e0833002     	add	r3, r3, r2
     73c: e59d20b0     	ldr	r2, [sp, #0xb0]
     740: e58d3000     	str	r3, [sp]
     744: e59d301c     	ldr	r3, [sp, #0x1c]
     748: e0833002     	add	r3, r3, r2
     74c: e58d301c     	str	r3, [sp, #0x1c]
     750: e59d3048     	ldr	r3, [sp, #0x48]
     754: e59d207c     	ldr	r2, [sp, #0x7c]
     758: e3530000     	cmp	r3, #0
     75c: e59d304c     	ldr	r3, [sp, #0x4c]
     760: e0833002     	add	r3, r3, r2
     764: e59d2058     	ldr	r2, [sp, #0x58]
     768: e58d304c     	str	r3, [sp, #0x4c]
     76c: e59d3080     	ldr	r3, [sp, #0x80]
     770: e5933020     	ldr	r3, [r3, #0x20]
     774: e0822103     	add	r2, r2, r3, lsl #2
     778: e58d2058     	str	r2, [sp, #0x58]
     77c: e59d2054     	ldr	r2, [sp, #0x54]
     780: e0823083     	add	r3, r2, r3, lsl #1
     784: e58d3054     	str	r3, [sp, #0x54]
     788: 1affff12     	bne	0x3d8 <ZB_fillTriangleSmooth_DT1_DW0+0x3d8> @ imm = #-0x3b8
     78c: e59db0d8     	ldr	r11, [sp, #0xd8]
     790: e59d20dc     	ldr	r2, [sp, #0xdc]
     794: e59d307c     	ldr	r3, [sp, #0x7c]
     798: e59d90e0     	ldr	r9, [sp, #0xe0]
     79c: e59d80e4     	ldr	r8, [sp, #0xe4]
     7a0: e022239b     	mla	r2, r11, r3, r2
     7a4: e3590001     	cmp	r9, #1
     7a8: 1a00001b     	bne	0x81c <ZB_fillTriangleSmooth_DT1_DW0+0x81c> @ imm = #0x6c
     7ac: e28dd0f4     	add	sp, sp, #244
     7b0: ecbd8b06     	vpop	{d8, d9, d10}
     7b4: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     7b8: e59d3050     	ldr	r3, [sp, #0x50]
     7bc: e59d20c8     	ldr	r2, [sp, #0xc8]
     7c0: e0833002     	add	r3, r3, r2
     7c4: e59d20b8     	ldr	r2, [sp, #0xb8]
     7c8: e58d3050     	str	r3, [sp, #0x50]
     7cc: e59d3040     	ldr	r3, [sp, #0x40]
     7d0: e0833002     	add	r3, r3, r2
     7d4: e59d20bc     	ldr	r2, [sp, #0xbc]
     7d8: e58d3040     	str	r3, [sp, #0x40]
     7dc: e59d3004     	ldr	r3, [sp, #0x4]
     7e0: e0833002     	add	r3, r3, r2
     7e4: e59d20c0     	ldr	r2, [sp, #0xc0]
     7e8: e58d3004     	str	r3, [sp, #0x4]
     7ec: e59d3000     	ldr	r3, [sp]
     7f0: e0833002     	add	r3, r3, r2
     7f4: e59d20c4     	ldr	r2, [sp, #0xc4]
     7f8: e58d3000     	str	r3, [sp]
     7fc: e59d301c     	ldr	r3, [sp, #0x1c]
     800: e0833002     	add	r3, r3, r2
     804: e58d301c     	str	r3, [sp, #0x1c]
     808: eaffffd0     	b	0x750 <ZB_fillTriangleSmooth_DT1_DW0+0x750> @ imm = #-0xc0
     80c: e59d001c     	ldr	r0, [sp, #0x1c]
     810: e59d7000     	ldr	r7, [sp]
     814: e59d1004     	ldr	r1, [sp, #0x4]
     818: eaffff99     	b	0x684 <ZB_fillTriangleSmooth_DT1_DW0+0x684> @ imm = #-0x19c
     81c: e59d00cc     	ldr	r0, [sp, #0xcc]
     820: e5981004     	ldr	r1, [r8, #0x4]
     824: eeb58ac0     	vcmpe.f32	s16, #0
     828: e5903004     	ldr	r3, [r0, #0x4]
     82c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     830: e0411003     	sub	r1, r1, r3
     834: e281b001     	add	r11, r1, #1
     838: da000019     	ble	0x8a4 <ZB_fillTriangleSmooth_DT1_DW0+0x8a4> @ imm = #0x64
     83c: e3510000     	cmp	r1, #0
     840: e5904000     	ldr	r4, [r0]
     844: e3a05001     	mov	r5, #1
     848: da00001a     	ble	0x8b8 <ZB_fillTriangleSmooth_DT1_DW0+0x8b8> @ imm = #0x68
     84c: e59d30cc     	ldr	r3, [sp, #0xcc]
     850: e58d80ec     	str	r8, [sp, #0xec]
     854: e58d30d0     	str	r3, [sp, #0xd0]
     858: e59d30ec     	ldr	r3, [sp, #0xec]
     85c: e1a09005     	mov	r9, r5
     860: e5930000     	ldr	r0, [r3]
     864: e0400004     	sub	r0, r0, r4
     868: e1a00800     	lsl	r0, r0, #16
     86c: ebfffffe     	bl	0x86c <ZB_fillTriangleSmooth_DT1_DW0+0x86c> @ imm = #-0x8
     870: e58d007c     	str	r0, [sp, #0x7c]
     874: e1a02804     	lsl	r2, r4, #16
     878: eafffebd     	b	0x374 <ZB_fillTriangleSmooth_DT1_DW0+0x374> @ imm = #-0x50c
     87c: e3a03000     	mov	r3, #0
     880: e3a01001     	mov	r1, #1
     884: e58d3084     	str	r3, [sp, #0x84]
     888: ed8daa31     	vstr	s20, [sp, #196]
     88c: edcd9a30     	vstr	s19, [sp, #192]
     890: ed8d9a2f     	vstr	s18, [sp, #188]
     894: edcd8a2e     	vstr	s17, [sp, #184]
     898: e58d10b4     	str	r1, [sp, #0xb4]
     89c: e58d30c8     	str	r3, [sp, #0xc8]
     8a0: eafffea8     	b	0x348 <ZB_fillTriangleSmooth_DT1_DW0+0x348> @ imm = #-0x560
     8a4: e58d9044     	str	r9, [sp, #0x44]
     8a8: e1a00008     	mov	r0, r8
     8ac: e59d40cc     	ldr	r4, [sp, #0xcc]
     8b0: e3a09001     	mov	r9, #1
     8b4: eafffe84     	b	0x2cc <ZB_fillTriangleSmooth_DT1_DW0+0x2cc> @ imm = #-0x5f0
     8b8: e35b0000     	cmp	r11, #0
     8bc: daffffba     	ble	0x7ac <ZB_fillTriangleSmooth_DT1_DW0+0x7ac> @ imm = #-0x118
     8c0: e58d907c     	str	r9, [sp, #0x7c]
     8c4: e1a02804     	lsl	r2, r4, #16
     8c8: e1a09005     	mov	r9, r5
     8cc: e58d00d0     	str	r0, [sp, #0xd0]
     8d0: e58d80ec     	str	r8, [sp, #0xec]
     8d4: eafffea8     	b	0x37c <ZB_fillTriangleSmooth_DT1_DW0+0x37c> @ imm = #-0x560
     8d8: e1a0c003     	mov	r12, r3
     8dc: e58d80cc     	str	r8, [sp, #0xcc]
     8e0: e1a03000     	mov	r3, r0
     8e4: e1a08004     	mov	r8, r4
     8e8: e1a0000e     	mov	r0, lr
     8ec: e58d50d0     	str	r5, [sp, #0xd0]
     8f0: eafffddf     	b	0x74 <ZB_fillTriangleSmooth_DT1_DW0+0x74> @ imm = #-0x884
     8f4: e59d00cc     	ldr	r0, [sp, #0xcc]
     8f8: e1a0300c     	mov	r3, r12
     8fc: e58d80ec     	str	r8, [sp, #0xec]
     900: eafffe66     	b	0x2a0 <ZB_fillTriangleSmooth_DT1_DW0+0x2a0> @ imm = #-0x668
     904: e59d30ec     	ldr	r3, [sp, #0xec]
     908: e59d20d0     	ldr	r2, [sp, #0xd0]
     90c: e5931004     	ldr	r1, [r3, #0x4]
     910: e5923004     	ldr	r3, [r2, #0x4]
     914: e5924000     	ldr	r4, [r2]
     918: e0411003     	sub	r1, r1, r3
     91c: e3510000     	cmp	r1, #0
     920: c3a03000     	movgt	r3, #0
     924: c1a05009     	movgt	r5, r9
     928: c58d3044     	strgt	r3, [sp, #0x44]
     92c: caffffc9     	bgt	0x858 <ZB_fillTriangleSmooth_DT1_DW0+0x858> @ imm = #-0xdc
     930: e3a03000     	mov	r3, #0
     934: e58d3044     	str	r3, [sp, #0x44]
     938: e58d307c     	str	r3, [sp, #0x7c]
     93c: eaffffcc     	b	0x874 <ZB_fillTriangleSmooth_DT1_DW0+0x874> @ imm = #-0xd0

Disassembly of section .text.ZB_fillTriangleSmooth_DT1_DW1:

00000000 <ZB_fillTriangleSmooth_DT1_DW1>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a08003     	mov	r8, r3
       8: ed2d8b06     	vpush	{d8, d9, d10}
       c: e24dd0fc     	sub	sp, sp, #252
      10: e592c004     	ldr	r12, [r2, #0x4]
      14: e5913004     	ldr	r3, [r1, #0x4]
      18: e58d0088     	str	r0, [sp, #0x88]
      1c: e5980004     	ldr	r0, [r8, #0x4]
      20: e15c0003     	cmp	r12, r3
      24: a1a0e003     	movge	lr, r3
      28: b1a0e00c     	movlt	lr, r12
      2c: a1a0300c     	movge	r3, r12
      30: b1a0b002     	movlt	r11, r2
      34: a1a0b001     	movge	r11, r1
      38: b1a09001     	movlt	r9, r1
      3c: a1a09002     	movge	r9, r2
      40: e150000e     	cmp	r0, lr
      44: e1a0c00e     	mov	r12, lr
      48: ba00000b     	blt	0x7c <ZB_fillTriangleSmooth_DT1_DW1+0x7c> @ imm = #0x2c
      4c: e1500003     	cmp	r0, r3
      50: e1a02008     	mov	r2, r8
      54: a1a0c003     	movge	r12, r3
      58: a1a0800b     	movge	r8, r11
      5c: a1a03000     	movge	r3, r0
      60: a1a0b009     	movge	r11, r9
      64: b1a0c000     	movlt	r12, r0
      68: b1a0800b     	movlt	r8, r11
      6c: a1a0000e     	movge	r0, lr
      70: a1a09002     	movge	r9, r2
      74: b1a0000e     	movlt	r0, lr
      78: b1a0b002     	movlt	r11, r2
      7c: e0433000     	sub	r3, r3, r0
      80: e5982000     	ldr	r2, [r8]
      84: e5984014     	ldr	r4, [r8, #0x14]
      88: ee073a90     	vmov	s15, r3
      8c: e5993000     	ldr	r3, [r9]
      90: eef86ae7     	vcvt.f32.s32	s13, s15
      94: e598e018     	ldr	lr, [r8, #0x18]
      98: e0433002     	sub	r3, r3, r2
      9c: e598101c     	ldr	r1, [r8, #0x1c]
      a0: ee073a90     	vmov	s15, r3
      a4: e04c3000     	sub	r3, r12, r0
      a8: ee073a10     	vmov	s14, r3
      ac: e59b3000     	ldr	r3, [r11]
      b0: eef87ae7     	vcvt.f32.s32	s15, s15
      b4: eeb87ac7     	vcvt.f32.s32	s14, s14
      b8: e0433002     	sub	r3, r3, r2
      bc: e5982008     	ldr	r2, [r8, #0x8]
      c0: ee053a90     	vmov	s11, r3
      c4: ee278a27     	vmul.f32	s16, s14, s15
      c8: eef85ae5     	vcvt.f32.s32	s11, s11
      cc: e5993014     	ldr	r3, [r9, #0x14]
      d0: ee158aa6     	vnmls.f32	s16, s11, s13
      d4: e0433004     	sub	r3, r3, r4
      d8: eeb58a40     	vcmp.f32	s16, #0
      dc: eef1fa10     	vmrs	APSR_nzcv, fpscr
      e0: 1eb76a00     	vmovne.f32	s12, #1.000000e+00
      e4: 1e868a08     	vdivne.f32	s16, s12, s16
      e8: ee063a10     	vmov	s12, r3
      ec: e5993018     	ldr	r3, [r9, #0x18]
      f0: eef83ac6     	vcvt.f32.s32	s7, s12
      f4: e043300e     	sub	r3, r3, lr
      f8: ee063a10     	vmov	s12, r3
      fc: e599301c     	ldr	r3, [r9, #0x1c]
     100: eeb84ac6     	vcvt.f32.s32	s8, s12
     104: e0433001     	sub	r3, r3, r1
     108: ee063a10     	vmov	s12, r3
     10c: e5993008     	ldr	r3, [r9, #0x8]
     110: eef84ac6     	vcvt.f32.s32	s9, s12
     114: ee287a07     	vmul.f32	s14, s16, s14
     118: e0433002     	sub	r3, r3, r2
     11c: ee686a26     	vmul.f32	s13, s16, s13
     120: ee687a27     	vmul.f32	s15, s16, s15
     124: ee063a10     	vmov	s12, r3
     128: e59b3014     	ldr	r3, [r11, #0x14]
     12c: eeb85ac6     	vcvt.f32.s32	s10, s12
     130: ee672a23     	vmul.f32	s5, s14, s7
     134: e0433004     	sub	r3, r3, r4
     138: ee685a25     	vmul.f32	s11, s16, s11
     13c: ee272a04     	vmul.f32	s4, s14, s8
     140: ee063a10     	vmov	s12, r3
     144: e59b3018     	ldr	r3, [r11, #0x18]
     148: eeb86ac6     	vcvt.f32.s32	s12, s12
     14c: ee273a24     	vmul.f32	s6, s14, s9
     150: e043300e     	sub	r3, r3, lr
     154: ee277a05     	vmul.f32	s14, s14, s10
     158: eeb58ac0     	vcmpe.f32	s16, #0
     15c: ee562a86     	vnmls.f32	s5, s13, s12
     160: ee276a86     	vmul.f32	s12, s15, s12
     164: eef1fa10     	vmrs	APSR_nzcv, fpscr
     168: ee156aa3     	vnmls.f32	s12, s11, s7
     16c: eef03a42     	vmov.f32	s7, s4
     170: eebd6ac6     	vcvt.s32.f32	s12, s12
     174: eeb09a46     	vmov.f32	s18, s12
     178: ee063a10     	vmov	s12, r3
     17c: e59b301c     	ldr	r3, [r11, #0x1c]
     180: eeb86ac6     	vcvt.f32.s32	s12, s12
     184: e0433001     	sub	r3, r3, r1
     188: e59d1088     	ldr	r1, [sp, #0x88]
     18c: ee563a86     	vnmls.f32	s7, s13, s12
     190: ee276a86     	vmul.f32	s12, s15, s12
     194: ee156a84     	vnmls.f32	s12, s11, s8
     198: eeb04a43     	vmov.f32	s8, s6
     19c: eebd6ac6     	vcvt.s32.f32	s12, s12
     1a0: eef09a46     	vmov.f32	s19, s12
     1a4: ee063a10     	vmov	s12, r3
     1a8: e59b3008     	ldr	r3, [r11, #0x8]
     1ac: eeb86ac6     	vcvt.f32.s32	s12, s12
     1b0: e0433002     	sub	r3, r3, r2
     1b4: ee164a86     	vnmls.f32	s8, s13, s12
     1b8: ee276a86     	vmul.f32	s12, s15, s12
     1bc: ee156aa4     	vnmls.f32	s12, s11, s9
     1c0: eebd6ac6     	vcvt.s32.f32	s12, s12
     1c4: eeb0aa46     	vmov.f32	s20, s12
     1c8: ee063a10     	vmov	s12, r3
     1cc: e5913020     	ldr	r3, [r1, #0x20]
     1d0: eeb86ac6     	vcvt.f32.s32	s12, s12
     1d4: e0000093     	mul	r0, r3, r0
     1d8: e5913004     	ldr	r3, [r1, #0x4]
     1dc: ee677a86     	vmul.f32	s15, s15, s12
     1e0: ee167a86     	vnmls.f32	s14, s13, s12
     1e4: e0833100     	add	r3, r3, r0, lsl #2
     1e8: ee557a85     	vnmls.f32	s15, s11, s10
     1ec: e58d3058     	str	r3, [sp, #0x58]
     1f0: e5913000     	ldr	r3, [r1]
     1f4: e0833080     	add	r3, r3, r0, lsl #1
     1f8: eefd7ae7     	vcvt.s32.f32	s15, s15
     1fc: e58d305c     	str	r3, [sp, #0x5c]
     200: eef08a67     	vmov.f32	s17, s15
     204: eefd7ae2     	vcvt.s32.f32	s15, s5
     208: edcd7a0c     	vstr	s15, [sp, #48]
     20c: eefd7ae3     	vcvt.s32.f32	s15, s7
     210: e59d0030     	ldr	r0, [sp, #0x30]
     214: edcd7a35     	vstr	s15, [sp, #212]
     218: eefd7ac4     	vcvt.s32.f32	s15, s8
     21c: e1a05080     	lsl	r5, r0, #1
     220: e59d10d4     	ldr	r1, [sp, #0xd4]
     224: e58d5098     	str	r5, [sp, #0x98]
     228: edcd7a36     	vstr	s15, [sp, #216]
     22c: e59de0d8     	ldr	lr, [sp, #0xd8]
     230: e1a03101     	lsl	r3, r1, #2
     234: eefd7ac7     	vcvt.s32.f32	s15, s14
     238: e1a04081     	lsl	r4, r1, #1
     23c: e58d3008     	str	r3, [sp, #0x8]
     240: e1a03100     	lsl	r3, r0, #2
     244: e0841001     	add	r1, r4, r1
     248: e58d4094     	str	r4, [sp, #0x94]
     24c: e1a0608e     	lsl	r6, lr, #1
     250: e58d300c     	str	r3, [sp, #0xc]
     254: e1a0310e     	lsl	r3, lr, #2
     258: ee172a90     	vmov	r2, s15
     25c: e1a07082     	lsl	r7, r2, #1
     260: e58d6090     	str	r6, [sp, #0x90]
     264: e58d3010     	str	r3, [sp, #0x10]
     268: e1a03102     	lsl	r3, r2, #2
     26c: e58d10a0     	str	r1, [sp, #0xa0]
     270: e0851000     	add	r1, r5, r0
     274: e58d10a4     	str	r1, [sp, #0xa4]
     278: e086100e     	add	r1, r6, lr
     27c: e58d70a8     	str	r7, [sp, #0xa8]
     280: e58d109c     	str	r1, [sp, #0x9c]
     284: da0001a7     	ble	0x928 <ZB_fillTriangleSmooth_DT1_DW1+0x928> @ imm = #0x69c
     288: e599e004     	ldr	lr, [r9, #0x4]
     28c: e1a00009     	mov	r0, r9
     290: e58db0f0     	str	r11, [sp, #0xf0]
     294: e5981004     	ldr	r1, [r8, #0x4]
     298: e3a06000     	mov	r6, #0
     29c: e58d3064     	str	r3, [sp, #0x64]
     2a0: e1a0300b     	mov	r3, r11
     2a4: e1a04008     	mov	r4, r8
     2a8: e1a0b002     	mov	r11, r2
     2ac: e04ca001     	sub	r10, r12, r1
     2b0: e58d80f4     	str	r8, [sp, #0xf4]
     2b4: e1a0200a     	mov	r2, r10
     2b8: e04e1001     	sub	r1, lr, r1
     2bc: e1a07006     	mov	r7, r6
     2c0: e1a08009     	mov	r8, r9
     2c4: e1a0a003     	mov	r10, r3
     2c8: e3a0c001     	mov	r12, #1
     2cc: e58d6084     	str	r6, [sp, #0x84]
     2d0: e58dc048     	str	r12, [sp, #0x48]
     2d4: e594c000     	ldr	r12, [r4]
     2d8: e3510000     	cmp	r1, #0
     2dc: e58dc054     	str	r12, [sp, #0x54]
     2e0: da000179     	ble	0x8cc <ZB_fillTriangleSmooth_DT1_DW1+0x8cc> @ imm = #0x5e4
     2e4: e5903000     	ldr	r3, [r0]
     2e8: e58d2000     	str	r2, [sp]
     2ec: e043300c     	sub	r3, r3, r12
     2f0: e1a00803     	lsl	r0, r3, #16
     2f4: ebfffffe     	bl	0x2f4 <ZB_fillTriangleSmooth_DT1_DW1+0x2f4> @ imm = #-0x8
     2f8: e6ff2070     	uxth	r2, r0
     2fc: e1a03840     	asr	r3, r0, #16
     300: e59d1030     	ldr	r1, [sp, #0x30]
     304: e58d208c     	str	r2, [sp, #0x8c]
     308: ee182a90     	vmov	r2, s17
     30c: e58d30d0     	str	r3, [sp, #0xd0]
     310: e0222b93     	mla	r2, r3, r11, r2
     314: e58d20c0     	str	r2, [sp, #0xc0]
     318: ee192a10     	vmov	r2, s18
     31c: e0222193     	mla	r2, r3, r1, r2
     320: e59d10d4     	ldr	r1, [sp, #0xd4]
     324: e58d20c4     	str	r2, [sp, #0xc4]
     328: ee192a90     	vmov	r2, s19
     32c: e0222193     	mla	r2, r3, r1, r2
     330: e59d10d8     	ldr	r1, [sp, #0xd8]
     334: e58d20c8     	str	r2, [sp, #0xc8]
     338: ee1a2a10     	vmov	r2, s20
     33c: e0222193     	mla	r2, r3, r1, r2
     340: e2833001     	add	r3, r3, #1
     344: e58d30bc     	str	r3, [sp, #0xbc]
     348: e58d20cc     	str	r2, [sp, #0xcc]
     34c: e59d2000     	ldr	r2, [sp]
     350: e59d3048     	ldr	r3, [sp, #0x48]
     354: e5945018     	ldr	r5, [r4, #0x18]
     358: e3530000     	cmp	r3, #0
     35c: e5943008     	ldr	r3, [r4, #0x8]
     360: e58d3044     	str	r3, [sp, #0x44]
     364: e5943014     	ldr	r3, [r4, #0x14]
     368: e58d3020     	str	r3, [sp, #0x20]
     36c: e594301c     	ldr	r3, [r4, #0x1c]
     370: e58d3024     	str	r3, [sp, #0x24]
     374: 1a00016f     	bne	0x938 <ZB_fillTriangleSmooth_DT1_DW1+0x938> @ imm = #0x5bc
     378: e3520000     	cmp	r2, #0
     37c: da00011c     	ble	0x7f4 <ZB_fillTriangleSmooth_DT1_DW1+0x7f4> @ imm = #0x470
     380: e59d30c0     	ldr	r3, [sp, #0xc0]
     384: e59d10c4     	ldr	r1, [sp, #0xc4]
     388: e59d90d8     	ldr	r9, [sp, #0xd8]
     38c: e08b3003     	add	r3, r11, r3
     390: e58da0ec     	str	r10, [sp, #0xec]
     394: e58d30ac     	str	r3, [sp, #0xac]
     398: e59d3030     	ldr	r3, [sp, #0x30]
     39c: e58d204c     	str	r2, [sp, #0x4c]
     3a0: e58d7050     	str	r7, [sp, #0x50]
     3a4: e0833001     	add	r3, r3, r1
     3a8: e59d10c8     	ldr	r1, [sp, #0xc8]
     3ac: e58d30b0     	str	r3, [sp, #0xb0]
     3b0: e59d30d4     	ldr	r3, [sp, #0xd4]
     3b4: e58d60dc     	str	r6, [sp, #0xdc]
     3b8: e58d20e0     	str	r2, [sp, #0xe0]
     3bc: e0831001     	add	r1, r3, r1
     3c0: e1a0a003     	mov	r10, r3
     3c4: e58d10b4     	str	r1, [sp, #0xb4]
     3c8: e59d10cc     	ldr	r1, [sp, #0xcc]
     3cc: e58d70e4     	str	r7, [sp, #0xe4]
     3d0: e58d5000     	str	r5, [sp]
     3d4: e0891001     	add	r1, r9, r1
     3d8: e58d80e8     	str	r8, [sp, #0xe8]
     3dc: e58d10b8     	str	r1, [sp, #0xb8]
     3e0: e59d304c     	ldr	r3, [sp, #0x4c]
     3e4: e59d2050     	ldr	r2, [sp, #0x50]
     3e8: e59d5044     	ldr	r5, [sp, #0x44]
     3ec: e2433001     	sub	r3, r3, #1
     3f0: e58d304c     	str	r3, [sp, #0x4c]
     3f4: e59d3054     	ldr	r3, [sp, #0x54]
     3f8: e063c842     	rsb	r12, r3, r2, asr #16
     3fc: e59d2058     	ldr	r2, [sp, #0x58]
     400: e35c0002     	cmp	r12, #2
     404: e0824103     	add	r4, r2, r3, lsl #2
     408: e59d205c     	ldr	r2, [sp, #0x5c]
     40c: e0823083     	add	r3, r2, r3, lsl #1
     410: e1a02005     	mov	r2, r5
     414: da000110     	ble	0x85c <ZB_fillTriangleSmooth_DT1_DW1+0x85c> @ imm = #0x440
     418: e24c2003     	sub	r2, r12, #3
     41c: e2831008     	add	r1, r3, #8
     420: e1a02122     	lsr	r2, r2, #2
     424: e2833010     	add	r3, r3, #16
     428: e2846010     	add	r6, r4, #16
     42c: e59d8000     	ldr	r8, [sp]
     430: e1a00182     	lsl	r0, r2, #3
     434: e58d1060     	str	r1, [sp, #0x60]
     438: e59d7020     	ldr	r7, [sp, #0x20]
     43c: e3a0e000     	mov	lr, #0
     440: e0834000     	add	r4, r3, r0
     444: e59d3090     	ldr	r3, [sp, #0x90]
     448: e58d401c     	str	r4, [sp, #0x1c]
     44c: e59d40a8     	ldr	r4, [sp, #0xa8]
     450: e58d6068     	str	r6, [sp, #0x68]
     454: e58de06c     	str	lr, [sp, #0x6c]
     458: e0851004     	add	r1, r5, r4
     45c: e59d4094     	ldr	r4, [sp, #0x94]
     460: e58d1004     	str	r1, [sp, #0x4]
     464: e58da070     	str	r10, [sp, #0x70]
     468: e0884004     	add	r4, r8, r4
     46c: e58d9074     	str	r9, [sp, #0x74]
     470: e58d4014     	str	r4, [sp, #0x14]
     474: e59d4098     	ldr	r4, [sp, #0x98]
     478: e58dc078     	str	r12, [sp, #0x78]
     47c: e58d007c     	str	r0, [sp, #0x7c]
     480: e0874004     	add	r4, r7, r4
     484: e58d2080     	str	r2, [sp, #0x80]
     488: e58d4038     	str	r4, [sp, #0x38]
     48c: e59d4024     	ldr	r4, [sp, #0x24]
     490: e0843003     	add	r3, r4, r3
     494: e0841009     	add	r1, r4, r9
     498: e58d3034     	str	r3, [sp, #0x34]
     49c: e59d30a0     	ldr	r3, [sp, #0xa0]
     4a0: e58d102c     	str	r1, [sp, #0x2c]
     4a4: e1a01007     	mov	r1, r7
     4a8: e0883003     	add	r3, r8, r3
     4ac: e088800a     	add	r8, r8, r10
     4b0: e58d3018     	str	r3, [sp, #0x18]
     4b4: e59d30a4     	ldr	r3, [sp, #0xa4]
     4b8: e0873003     	add	r3, r7, r3
     4bc: e59d7030     	ldr	r7, [sp, #0x30]
     4c0: e58d3040     	str	r3, [sp, #0x40]
     4c4: e59d309c     	ldr	r3, [sp, #0x9c]
     4c8: e0811007     	add	r1, r1, r7
     4cc: e59d7004     	ldr	r7, [sp, #0x4]
     4d0: e58d1028     	str	r1, [sp, #0x28]
     4d4: e1a0100e     	mov	r1, lr
     4d8: e0844003     	add	r4, r4, r3
     4dc: e58d8004     	str	r8, [sp, #0x4]
     4e0: e58d403c     	str	r4, [sp, #0x3c]
     4e4: e1a0400e     	mov	r4, lr
     4e8: e59d3060     	ldr	r3, [sp, #0x60]
     4ec: e59d8064     	ldr	r8, [sp, #0x64]
     4f0: e59d2000     	ldr	r2, [sp]
     4f4: e1a00725     	lsr	r0, r5, #14
     4f8: e153c0b8     	ldrh	r12, [r3, #-8]
     4fc: e085900b     	add	r9, r5, r11
     500: e1a09729     	lsr	r9, r9, #14
     504: e0855008     	add	r5, r5, r8
     508: e0822001     	add	r2, r2, r1
     50c: e1a02442     	asr	r2, r2, #8
     510: e15c0000     	cmp	r12, r0
     514: e2022cff     	and	r2, r2, #65280
     518: 8a000009     	bhi	0x544 <ZB_fillTriangleSmooth_DT1_DW1+0x544> @ imm = #0x24
     51c: e59dc024     	ldr	r12, [sp, #0x24]
     520: e08cc00e     	add	r12, r12, lr
     524: e7e7c85c     	ubfx	r12, r12, #0x10, #0x8
     528: e182200c     	orr	r2, r2, r12
     52c: e59dc020     	ldr	r12, [sp, #0x20]
     530: e08cc004     	add	r12, r12, r4
     534: e20cc8ff     	and	r12, r12, #16711680
     538: e182200c     	orr	r2, r2, r12
     53c: e5062010     	str	r2, [r6, #-0x10]
     540: e14300b8     	strh	r0, [r3, #-8]
     544: e59d0014     	ldr	r0, [sp, #0x14]
     548: e59da008     	ldr	r10, [sp, #0x8]
     54c: e59d2004     	ldr	r2, [sp, #0x4]
     550: e080c001     	add	r12, r0, r1
     554: e59d0018     	ldr	r0, [sp, #0x18]
     558: e1a0c44c     	asr	r12, r12, #8
     55c: e0822001     	add	r2, r2, r1
     560: e0800001     	add	r0, r0, r1
     564: e081100a     	add	r1, r1, r10
     568: e153a0b6     	ldrh	r10, [r3, #-6]
     56c: e1a02442     	asr	r2, r2, #8
     570: e1a00440     	asr	r0, r0, #8
     574: e20cccff     	and	r12, r12, #65280
     578: e2022cff     	and	r2, r2, #65280
     57c: e15a0009     	cmp	r10, r9
     580: e2000cff     	and	r0, r0, #65280
     584: 8a000009     	bhi	0x5b0 <ZB_fillTriangleSmooth_DT1_DW1+0x5b0> @ imm = #0x24
     588: e59da02c     	ldr	r10, [sp, #0x2c]
     58c: e08aa00e     	add	r10, r10, lr
     590: e7e7a85a     	ubfx	r10, r10, #0x10, #0x8
     594: e182200a     	orr	r2, r2, r10
     598: e59da028     	ldr	r10, [sp, #0x28]
     59c: e08aa004     	add	r10, r10, r4
     5a0: e20aa8ff     	and	r10, r10, #16711680
     5a4: e182200a     	orr	r2, r2, r10
     5a8: e506200c     	str	r2, [r6, #-0xc]
     5ac: e14390b6     	strh	r9, [r3, #-6]
     5b0: e153a0b4     	ldrh	r10, [r3, #-4]
     5b4: e1a09727     	lsr	r9, r7, #14
     5b8: e087200b     	add	r2, r7, r11
     5bc: e1a02722     	lsr	r2, r2, #14
     5c0: e0877008     	add	r7, r7, r8
     5c4: e15a0009     	cmp	r10, r9
     5c8: 8a000009     	bhi	0x5f4 <ZB_fillTriangleSmooth_DT1_DW1+0x5f4> @ imm = #0x24
     5cc: e59da034     	ldr	r10, [sp, #0x34]
     5d0: e08aa00e     	add	r10, r10, lr
     5d4: e7e7a85a     	ubfx	r10, r10, #0x10, #0x8
     5d8: e18cc00a     	orr	r12, r12, r10
     5dc: e59da038     	ldr	r10, [sp, #0x38]
     5e0: e08aa004     	add	r10, r10, r4
     5e4: e20aa8ff     	and	r10, r10, #16711680
     5e8: e18cc00a     	orr	r12, r12, r10
     5ec: e506c008     	str	r12, [r6, #-0x8]
     5f0: e14390b4     	strh	r9, [r3, #-4]
     5f4: e153c0b2     	ldrh	r12, [r3, #-2]
     5f8: e15c0002     	cmp	r12, r2
     5fc: 8a000009     	bhi	0x628 <ZB_fillTriangleSmooth_DT1_DW1+0x628> @ imm = #0x24
     600: e59dc03c     	ldr	r12, [sp, #0x3c]
     604: e08cc00e     	add	r12, r12, lr
     608: e7e7c85c     	ubfx	r12, r12, #0x10, #0x8
     60c: e180000c     	orr	r0, r0, r12
     610: e59dc040     	ldr	r12, [sp, #0x40]
     614: e08cc004     	add	r12, r12, r4
     618: e20cc8ff     	and	r12, r12, #16711680
     61c: e180000c     	orr	r0, r0, r12
     620: e5060004     	str	r0, [r6, #-0x4]
     624: e14320b2     	strh	r2, [r3, #-2]
     628: e59d200c     	ldr	r2, [sp, #0xc]
     62c: e2833008     	add	r3, r3, #8
     630: e2866010     	add	r6, r6, #16
     634: e0844002     	add	r4, r4, r2
     638: e59d201c     	ldr	r2, [sp, #0x1c]
     63c: e1520003     	cmp	r2, r3
     640: e59d2010     	ldr	r2, [sp, #0x10]
     644: e08ee002     	add	lr, lr, r2
     648: 1affffa8     	bne	0x4f0 <ZB_fillTriangleSmooth_DT1_DW1+0x4f0> @ imm = #-0x160
     64c: e59d007c     	ldr	r0, [sp, #0x7c]
     650: e59d3060     	ldr	r3, [sp, #0x60]
     654: e59de030     	ldr	lr, [sp, #0x30]
     658: e59d9074     	ldr	r9, [sp, #0x74]
     65c: e0803003     	add	r3, r0, r3
     660: e59d0028     	ldr	r0, [sp, #0x28]
     664: e59d102c     	ldr	r1, [sp, #0x2c]
     668: e59d2080     	ldr	r2, [sp, #0x80]
     66c: e59da070     	ldr	r10, [sp, #0x70]
     670: e080008e     	add	r0, r0, lr, lsl #1
     674: e59d8004     	ldr	r8, [sp, #0x4]
     678: e0811089     	add	r1, r1, r9, lsl #1
     67c: e59d5064     	ldr	r5, [sp, #0x64]
     680: e080000e     	add	r0, r0, lr
     684: e59de010     	ldr	lr, [sp, #0x10]
     688: e0811009     	add	r1, r1, r9
     68c: e59dc078     	ldr	r12, [sp, #0x78]
     690: e088808a     	add	r8, r8, r10, lsl #1
     694: e021129e     	mla	r1, lr, r2, r1
     698: e59de00c     	ldr	lr, [sp, #0xc]
     69c: e088800a     	add	r8, r8, r10
     6a0: e24cc004     	sub	r12, r12, #4
     6a4: e0200e92     	mla	r0, r2, lr, r0
     6a8: e59de008     	ldr	lr, [sp, #0x8]
     6ac: e04cc102     	sub	r12, r12, r2, lsl #2
     6b0: e0288e92     	mla	r8, r2, lr, r8
     6b4: e59de068     	ldr	lr, [sp, #0x68]
     6b8: e08e4202     	add	r4, lr, r2, lsl #4
     6bc: e59de044     	ldr	lr, [sp, #0x44]
     6c0: e08ee005     	add	lr, lr, r5
     6c4: e022e295     	mla	r2, r5, r2, lr
     6c8: e35c0000     	cmp	r12, #0
     6cc: a083608c     	addge	r6, r3, r12, lsl #1
     6d0: a243e002     	subge	lr, r3, #2
     6d4: ba000012     	blt	0x724 <ZB_fillTriangleSmooth_DT1_DW1+0x724> @ imm = #0x48
     6d8: e1fe70b2     	ldrh	r7, [lr, #2]!
     6dc: e1a05722     	lsr	r5, r2, #14
     6e0: e1a0c448     	asr	r12, r8, #8
     6e4: e7e73851     	ubfx	r3, r1, #0x10, #0x8
     6e8: e082200b     	add	r2, r2, r11
     6ec: e1570005     	cmp	r7, r5
     6f0: e20cccff     	and	r12, r12, #65280
     6f4: e18c3003     	orr	r3, r12, r3
     6f8: 9200c8ff     	andls	r12, r0, #16711680
     6fc: 9183300c     	orrls	r3, r3, r12
     700: e088800a     	add	r8, r8, r10
     704: 95843000     	strls	r3, [r4]
     708: e0811009     	add	r1, r1, r9
     70c: e59d3030     	ldr	r3, [sp, #0x30]
     710: e2844004     	add	r4, r4, #4
     714: 91ce50b0     	strhls	r5, [lr]
     718: e156000e     	cmp	r6, lr
     71c: e0800003     	add	r0, r0, r3
     720: 1affffec     	bne	0x6d8 <ZB_fillTriangleSmooth_DT1_DW1+0x6d8> @ imm = #-0x50
     724: e59d3048     	ldr	r3, [sp, #0x48]
     728: e59d208c     	ldr	r2, [sp, #0x8c]
     72c: e0833002     	add	r3, r3, r2
     730: e3530000     	cmp	r3, #0
     734: e58d3048     	str	r3, [sp, #0x48]
     738: da000032     	ble	0x808 <ZB_fillTriangleSmooth_DT1_DW1+0x808> @ imm = #0xc8
     73c: e59d3048     	ldr	r3, [sp, #0x48]
     740: e59d20bc     	ldr	r2, [sp, #0xbc]
     744: e2433801     	sub	r3, r3, #65536
     748: e58d3048     	str	r3, [sp, #0x48]
     74c: e59d3054     	ldr	r3, [sp, #0x54]
     750: e0833002     	add	r3, r3, r2
     754: e59d20ac     	ldr	r2, [sp, #0xac]
     758: e58d3054     	str	r3, [sp, #0x54]
     75c: e59d3044     	ldr	r3, [sp, #0x44]
     760: e0833002     	add	r3, r3, r2
     764: e59d20b0     	ldr	r2, [sp, #0xb0]
     768: e58d3044     	str	r3, [sp, #0x44]
     76c: e59d3020     	ldr	r3, [sp, #0x20]
     770: e0833002     	add	r3, r3, r2
     774: e59d20b4     	ldr	r2, [sp, #0xb4]
     778: e58d3020     	str	r3, [sp, #0x20]
     77c: e59d3000     	ldr	r3, [sp]
     780: e0833002     	add	r3, r3, r2
     784: e59d20b8     	ldr	r2, [sp, #0xb8]
     788: e58d3000     	str	r3, [sp]
     78c: e59d3024     	ldr	r3, [sp, #0x24]
     790: e0833002     	add	r3, r3, r2
     794: e58d3024     	str	r3, [sp, #0x24]
     798: e59d304c     	ldr	r3, [sp, #0x4c]
     79c: e59d2084     	ldr	r2, [sp, #0x84]
     7a0: e3530000     	cmp	r3, #0
     7a4: e59d3050     	ldr	r3, [sp, #0x50]
     7a8: e0833002     	add	r3, r3, r2
     7ac: e59d2058     	ldr	r2, [sp, #0x58]
     7b0: e58d3050     	str	r3, [sp, #0x50]
     7b4: e59d3088     	ldr	r3, [sp, #0x88]
     7b8: e5933020     	ldr	r3, [r3, #0x20]
     7bc: e0822103     	add	r2, r2, r3, lsl #2
     7c0: e58d2058     	str	r2, [sp, #0x58]
     7c4: e59d205c     	ldr	r2, [sp, #0x5c]
     7c8: e0823083     	add	r3, r2, r3, lsl #1
     7cc: e58d305c     	str	r3, [sp, #0x5c]
     7d0: 1affff02     	bne	0x3e0 <ZB_fillTriangleSmooth_DT1_DW1+0x3e0> @ imm = #-0x3f8
     7d4: e59d20e0     	ldr	r2, [sp, #0xe0]
     7d8: e59d70e4     	ldr	r7, [sp, #0xe4]
     7dc: e59d3084     	ldr	r3, [sp, #0x84]
     7e0: e59d60dc     	ldr	r6, [sp, #0xdc]
     7e4: e59d5000     	ldr	r5, [sp]
     7e8: e59d80e8     	ldr	r8, [sp, #0xe8]
     7ec: e59da0ec     	ldr	r10, [sp, #0xec]
     7f0: e0277392     	mla	r7, r2, r3, r7
     7f4: e3560001     	cmp	r6, #1
     7f8: 1a00001b     	bne	0x86c <ZB_fillTriangleSmooth_DT1_DW1+0x86c> @ imm = #0x6c
     7fc: e28dd0fc     	add	sp, sp, #252
     800: ecbd8b06     	vpop	{d8, d9, d10}
     804: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     808: e59d3054     	ldr	r3, [sp, #0x54]
     80c: e59d20d0     	ldr	r2, [sp, #0xd0]
     810: e0833002     	add	r3, r3, r2
     814: e59d20c0     	ldr	r2, [sp, #0xc0]
     818: e58d3054     	str	r3, [sp, #0x54]
     81c: e59d3044     	ldr	r3, [sp, #0x44]
     820: e0833002     	add	r3, r3, r2
     824: e59d20c4     	ldr	r2, [sp, #0xc4]
     828: e58d3044     	str	r3, [sp, #0x44]
     82c: e59d3020     	ldr	r3, [sp, #0x20]
     830: e0833002     	add	r3, r3, r2
     834: e59d20c8     	ldr	r2, [sp, #0xc8]
     838: e58d3020     	str	r3, [sp, #0x20]
     83c: e59d3000     	ldr	r3, [sp]
     840: e0833002     	add	r3, r3, r2
     844: e59d20cc     	ldr	r2, [sp, #0xcc]
     848: e58d3000     	str	r3, [sp]
     84c: e59d3024     	ldr	r3, [sp, #0x24]
     850: e0833002     	add	r3, r3, r2
     854: e58d3024     	str	r3, [sp, #0x24]
     858: eaffffce     	b	0x798 <ZB_fillTriangleSmooth_DT1_DW1+0x798> @ imm = #-0xc8
     85c: e59d1024     	ldr	r1, [sp, #0x24]
     860: e59d8000     	ldr	r8, [sp]
     864: e59d0020     	ldr	r0, [sp, #0x20]
     868: eaffff96     	b	0x6c8 <ZB_fillTriangleSmooth_DT1_DW1+0x6c8> @ imm = #-0x1a8
     86c: e5981004     	ldr	r1, [r8, #0x4]
     870: e59a3004     	ldr	r3, [r10, #0x4]
     874: eeb58ac0     	vcmpe.f32	s16, #0
     878: eef1fa10     	vmrs	APSR_nzcv, fpscr
     87c: e0411003     	sub	r1, r1, r3
     880: e2812001     	add	r2, r1, #1
     884: da00001a     	ble	0x8f4 <ZB_fillTriangleSmooth_DT1_DW1+0x8f4> @ imm = #0x68
     888: e3510000     	cmp	r1, #0
     88c: e59a7000     	ldr	r7, [r10]
     890: e3a04001     	mov	r4, #1
     894: c58da0f4     	strgt	r10, [sp, #0xf4]
     898: c58d80f0     	strgt	r8, [sp, #0xf0]
     89c: da000019     	ble	0x908 <ZB_fillTriangleSmooth_DT1_DW1+0x908> @ imm = #0x64
     8a0: e59d30f0     	ldr	r3, [sp, #0xf0]
     8a4: e1a06004     	mov	r6, r4
     8a8: e58d2000     	str	r2, [sp]
     8ac: e5930000     	ldr	r0, [r3]
     8b0: e0400007     	sub	r0, r0, r7
     8b4: e1a00800     	lsl	r0, r0, #16
     8b8: ebfffffe     	bl	0x8b8 <ZB_fillTriangleSmooth_DT1_DW1+0x8b8> @ imm = #-0x8
     8bc: e59d2000     	ldr	r2, [sp]
     8c0: e58d0084     	str	r0, [sp, #0x84]
     8c4: e1a07807     	lsl	r7, r7, #16
     8c8: eafffeaa     	b	0x378 <ZB_fillTriangleSmooth_DT1_DW1+0x378> @ imm = #-0x558
     8cc: e3a03000     	mov	r3, #0
     8d0: e3a01001     	mov	r1, #1
     8d4: e58d308c     	str	r3, [sp, #0x8c]
     8d8: ed8daa33     	vstr	s20, [sp, #204]
     8dc: edcd9a32     	vstr	s19, [sp, #200]
     8e0: ed8d9a31     	vstr	s18, [sp, #196]
     8e4: edcd8a30     	vstr	s17, [sp, #192]
     8e8: e58d10bc     	str	r1, [sp, #0xbc]
     8ec: e58d30d0     	str	r3, [sp, #0xd0]
     8f0: eafffe96     	b	0x350 <ZB_fillTriangleSmooth_DT1_DW1+0x350> @ imm = #-0x5a8
     8f4: e58d6048     	str	r6, [sp, #0x48]
     8f8: e1a00008     	mov	r0, r8
     8fc: e3a06001     	mov	r6, #1
     900: e1a0400a     	mov	r4, r10
     904: eafffe72     	b	0x2d4 <ZB_fillTriangleSmooth_DT1_DW1+0x2d4> @ imm = #-0x638
     908: e3520000     	cmp	r2, #0
     90c: daffffba     	ble	0x7fc <ZB_fillTriangleSmooth_DT1_DW1+0x7fc> @ imm = #-0x118
     910: e58d6084     	str	r6, [sp, #0x84]
     914: e1a07807     	lsl	r7, r7, #16
     918: e1a06004     	mov	r6, r4
     91c: e58da0f4     	str	r10, [sp, #0xf4]
     920: e58d80f0     	str	r8, [sp, #0xf0]
     924: eafffe95     	b	0x380 <ZB_fillTriangleSmooth_DT1_DW1+0x380> @ imm = #-0x5ac
     928: e1a0e00c     	mov	lr, r12
     92c: e1a0000b     	mov	r0, r11
     930: e58d90f0     	str	r9, [sp, #0xf0]
     934: eafffe56     	b	0x294 <ZB_fillTriangleSmooth_DT1_DW1+0x294> @ imm = #-0x6a8
     938: e59d30f0     	ldr	r3, [sp, #0xf0]
     93c: e59d00f4     	ldr	r0, [sp, #0xf4]
     940: e5931004     	ldr	r1, [r3, #0x4]
     944: e5903004     	ldr	r3, [r0, #0x4]
     948: e5907000     	ldr	r7, [r0]
     94c: e0411003     	sub	r1, r1, r3
     950: e3510000     	cmp	r1, #0
     954: c3a03000     	movgt	r3, #0
     958: c1a04006     	movgt	r4, r6
     95c: c58d3048     	strgt	r3, [sp, #0x48]
     960: caffffce     	bgt	0x8a0 <ZB_fillTriangleSmooth_DT1_DW1+0x8a0> @ imm = #-0xc8
     964: e3a03000     	mov	r3, #0
     968: e58d3048     	str	r3, [sp, #0x48]
     96c: e58d3084     	str	r3, [sp, #0x84]
     970: eaffffd3     	b	0x8c4 <ZB_fillTriangleSmooth_DT1_DW1+0x8c4> @ imm = #-0xb4

Disassembly of section .text.ZB_fillTriangleMappingPerspective_DT0_DW0:

00000000 <ZB_fillTriangleMappingPerspective_DT0_DW0>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a0c003     	mov	r12, r3
       8: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
       c: e24ddf51     	sub	sp, sp, #324
      10: e5924004     	ldr	r4, [r2, #0x4]
      14: e5913004     	ldr	r3, [r1, #0x4]
      18: e58d00ac     	str	r0, [sp, #0xac]
      1c: e59c0004     	ldr	r0, [r12, #0x4]
      20: e1540003     	cmp	r4, r3
      24: a1a05003     	movge	r5, r3
      28: b1a05004     	movlt	r5, r4
      2c: a1a03004     	movge	r3, r4
      30: b1a0a002     	movlt	r10, r2
      34: a1a0a001     	movge	r10, r1
      38: b1a0e001     	movlt	lr, r1
      3c: a1a0e002     	movge	lr, r2
      40: e1500005     	cmp	r0, r5
      44: e1a04005     	mov	r4, r5
      48: ba00000b     	blt	0x7c <ZB_fillTriangleMappingPerspective_DT0_DW0+0x7c> @ imm = #0x2c
      4c: e1500003     	cmp	r0, r3
      50: e1a0200c     	mov	r2, r12
      54: a1a04003     	movge	r4, r3
      58: a1a0c00a     	movge	r12, r10
      5c: a1a03000     	movge	r3, r0
      60: a1a0a00e     	movge	r10, lr
      64: b1a04000     	movlt	r4, r0
      68: b1a0c00a     	movlt	r12, r10
      6c: a1a00005     	movge	r0, r5
      70: a1a0e002     	movge	lr, r2
      74: b1a00005     	movlt	r0, r5
      78: b1a0a002     	movlt	r10, r2
      7c: e0442000     	sub	r2, r4, r0
      80: e0433000     	sub	r3, r3, r0
      84: eddc5a03     	vldr	s11, [r12, #12]
      88: ee073a90     	vmov	s15, r3
      8c: ee072a10     	vmov	s14, r2
      90: e59e3000     	ldr	r3, [lr]
      94: e59c2000     	ldr	r2, [r12]
      98: eef86ae7     	vcvt.f32.s32	s13, s15
      9c: eeb87ac7     	vcvt.f32.s32	s14, s14
      a0: e0433002     	sub	r3, r3, r2
      a4: eef82ae5     	vcvt.f32.s32	s5, s11
      a8: eddc5a04     	vldr	s11, [r12, #16]
      ac: ee073a90     	vmov	s15, r3
      b0: e59a3000     	ldr	r3, [r10]
      b4: eef87ae7     	vcvt.f32.s32	s15, s15
      b8: eeb83ae5     	vcvt.f32.s32	s6, s11
      bc: e0433002     	sub	r3, r3, r2
      c0: e59c2008     	ldr	r2, [r12, #0x8]
      c4: eeb2aa00     	vmov.f32	s20, #8.000000e+00
      c8: ee053a10     	vmov	s10, r3
      cc: ee67ca27     	vmul.f32	s25, s14, s15
      d0: eeb85ac5     	vcvt.f32.s32	s10, s10
      d4: e59a3008     	ldr	r3, [r10, #0x8]
      d8: ee55ca26     	vnmls.f32	s25, s10, s13
      dc: e0433002     	sub	r3, r3, r2
      e0: eef5ca40     	vcmp.f32	s25, #0
      e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
      e8: 1eb76a00     	vmovne.f32	s12, #1.000000e+00
      ec: 1ec6ca2c     	vdivne.f32	s25, s12, s25
      f0: ee062a10     	vmov	s12, r2
      f4: eeb86ac6     	vcvt.f32.s32	s12, s12
      f8: ee622a86     	vmul.f32	s5, s5, s12
      fc: ee233a06     	vmul.f32	s6, s6, s12
     100: ee063a10     	vmov	s12, r3
     104: e59e3008     	ldr	r3, [lr, #0x8]
     108: eeb86ac6     	vcvt.f32.s32	s12, s12
     10c: e0433002     	sub	r3, r3, r2
     110: e59c2014     	ldr	r2, [r12, #0x14]
     114: ee6c7aa7     	vmul.f32	s15, s25, s15
     118: ee053a90     	vmov	s11, r3
     11c: ee2c5a85     	vmul.f32	s10, s25, s10
     120: eef85ae5     	vcvt.f32.s32	s11, s11
     124: ee674a86     	vmul.f32	s9, s15, s12
     128: ee2c7a87     	vmul.f32	s14, s25, s14
     12c: ee6c6aa6     	vmul.f32	s13, s25, s13
     130: e59a3014     	ldr	r3, [r10, #0x14]
     134: ee554a25     	vnmls.f32	s9, s10, s11
     138: ee673a25     	vmul.f32	s7, s14, s11
     13c: e0433002     	sub	r3, r3, r2
     140: eef5cac0     	vcmpe.f32	s25, #0
     144: eef1fa10     	vmrs	APSR_nzcv, fpscr
     148: ee563a86     	vnmls.f32	s7, s13, s12
     14c: eebd6ae4     	vcvt.s32.f32	s12, s9
     150: eeb0ea46     	vmov.f32	s28, s12
     154: ee063a10     	vmov	s12, r3
     158: e59e3014     	ldr	r3, [lr, #0x14]
     15c: eeb86ac6     	vcvt.f32.s32	s12, s12
     160: e0433002     	sub	r3, r3, r2
     164: e59c2018     	ldr	r2, [r12, #0x18]
     168: ee053a90     	vmov	s11, r3
     16c: ee674a86     	vmul.f32	s9, s15, s12
     170: eef85ae5     	vcvt.f32.s32	s11, s11
     174: e59a3018     	ldr	r3, [r10, #0x18]
     178: ee554a25     	vnmls.f32	s9, s10, s11
     17c: e0433002     	sub	r3, r3, r2
     180: ee274a25     	vmul.f32	s8, s14, s11
     184: ee164a86     	vnmls.f32	s8, s13, s12
     188: eebd6ae4     	vcvt.s32.f32	s12, s9
     18c: eef0ea46     	vmov.f32	s29, s12
     190: ee063a10     	vmov	s12, r3
     194: e59e3018     	ldr	r3, [lr, #0x18]
     198: eeb86ac6     	vcvt.f32.s32	s12, s12
     19c: e0433002     	sub	r3, r3, r2
     1a0: e59c201c     	ldr	r2, [r12, #0x1c]
     1a4: ee053a90     	vmov	s11, r3
     1a8: ee272a86     	vmul.f32	s4, s15, s12
     1ac: eef85ae5     	vcvt.f32.s32	s11, s11
     1b0: e59a301c     	ldr	r3, [r10, #0x1c]
     1b4: ee152a25     	vnmls.f32	s4, s10, s11
     1b8: e0433002     	sub	r3, r3, r2
     1bc: ee674a25     	vmul.f32	s9, s14, s11
     1c0: ee564a86     	vnmls.f32	s9, s13, s12
     1c4: eebd6ac2     	vcvt.s32.f32	s12, s4
     1c8: eeb0fa46     	vmov.f32	s30, s12
     1cc: ee063a10     	vmov	s12, r3
     1d0: e59e301c     	ldr	r3, [lr, #0x1c]
     1d4: eeb86ac6     	vcvt.f32.s32	s12, s12
     1d8: edcc2a08     	vstr	s5, [r12, #32]
     1dc: e0433002     	sub	r3, r3, r2
     1e0: ed8c3a09     	vstr	s6, [r12, #36]
     1e4: ee053a90     	vmov	s11, r3
     1e8: ee272a86     	vmul.f32	s4, s15, s12
     1ec: eef85ae5     	vcvt.f32.s32	s11, s11
     1f0: eef02a42     	vmov.f32	s5, s4
     1f4: ee273a25     	vmul.f32	s6, s14, s11
     1f8: ee552a25     	vnmls.f32	s5, s10, s11
     1fc: eef05a43     	vmov.f32	s11, s6
     200: ee565a86     	vnmls.f32	s11, s13, s12
     204: eebd6ae2     	vcvt.s32.f32	s12, s5
     208: eef0fa46     	vmov.f32	s31, s12
     20c: eebd6ae5     	vcvt.s32.f32	s12, s11
     210: ed8d6a0c     	vstr	s12, [sp, #48]
     214: ed9a6a02     	vldr	s12, [r10, #8]
     218: eef85ac6     	vcvt.f32.s32	s11, s12
     21c: ed9a6a03     	vldr	s12, [r10, #12]
     220: eeb86ac6     	vcvt.f32.s32	s12, s12
     224: ee266a25     	vmul.f32	s12, s12, s11
     228: ed8a6a08     	vstr	s12, [r10, #32]
     22c: ed9a6a04     	vldr	s12, [r10, #16]
     230: eeb86ac6     	vcvt.f32.s32	s12, s12
     234: ee266a25     	vmul.f32	s12, s12, s11
     238: ed8a6a09     	vstr	s12, [r10, #36]
     23c: ed9e6a02     	vldr	s12, [lr, #8]
     240: eeb83ac6     	vcvt.f32.s32	s6, s12
     244: ed9e6a03     	vldr	s12, [lr, #12]
     248: eef85ac6     	vcvt.f32.s32	s11, s12
     24c: ed9e6a04     	vldr	s12, [lr, #16]
     250: ee655a83     	vmul.f32	s11, s11, s6
     254: eeb86ac6     	vcvt.f32.s32	s12, s12
     258: edce5a08     	vstr	s11, [lr, #32]
     25c: ee266a03     	vmul.f32	s12, s12, s6
     260: ed8e6a09     	vstr	s12, [lr, #36]
     264: eddc1a08     	vldr	s3, [r12, #32]
     268: ed9c2a09     	vldr	s4, [r12, #36]
     26c: edda2a08     	vldr	s5, [r10, #32]
     270: ed9a3a09     	vldr	s6, [r10, #36]
     274: e59d50ac     	ldr	r5, [sp, #0xac]
     278: e59c0004     	ldr	r0, [r12, #0x4]
     27c: ee722ae1     	vsub.f32	s5, s5, s3
     280: ee333a42     	vsub.f32	s6, s6, s4
     284: e5953020     	ldr	r3, [r5, #0x20]
     288: e5952004     	ldr	r2, [r5, #0x4]
     28c: ee27daa2     	vmul.f32	s26, s15, s5
     290: ee27ca83     	vmul.f32	s24, s15, s6
     294: eefd7ae3     	vcvt.s32.f32	s15, s7
     298: e0030093     	mul	r3, r3, r0
     29c: e59d4030     	ldr	r4, [sp, #0x30]
     2a0: ee755ae1     	vsub.f32	s11, s11, s3
     2a4: ee366a42     	vsub.f32	s12, s12, s4
     2a8: eeb88ae7     	vcvt.f32.s32	s16, s15
     2ac: e0823103     	add	r3, r2, r3, lsl #2
     2b0: eef0da67     	vmov.f32	s27, s15
     2b4: e1a07084     	lsl	r7, r4, #1
     2b8: eefd7ac4     	vcvt.s32.f32	s15, s8
     2bc: e1a0b184     	lsl	r11, r4, #3
     2c0: e58d3098     	str	r3, [sp, #0x98]
     2c4: ee679a25     	vmul.f32	s19, s14, s11
     2c8: ee279a06     	vmul.f32	s18, s14, s12
     2cc: edcd7a0a     	vstr	s15, [sp, #40]
     2d0: e59d3028     	ldr	r3, [sp, #0x28]
     2d4: eefd7ae4     	vcvt.s32.f32	s15, s9
     2d8: e58d70e8     	str	r7, [sp, #0xe8]
     2dc: e0877004     	add	r7, r7, r4
     2e0: ee569aa2     	vnmls.f32	s19, s13, s5
     2e4: e1a02083     	lsl	r2, r3, #1
     2e8: e1a09183     	lsl	r9, r3, #3
     2ec: ee171a90     	vmov	r1, s15
     2f0: e58d20c8     	str	r2, [sp, #0xc8]
     2f4: e1a02103     	lsl	r2, r3, #2
     2f8: e1a06081     	lsl	r6, r1, #1
     2fc: e1a08181     	lsl	r8, r1, #3
     300: e58d20bc     	str	r2, [sp, #0xbc]
     304: e1a02101     	lsl	r2, r1, #2
     308: e58d60ec     	str	r6, [sp, #0xec]
     30c: e0866001     	add	r6, r6, r1
     310: e58d20b8     	str	r2, [sp, #0xb8]
     314: e1a02104     	lsl	r2, r4, #2
     318: e58d60c0     	str	r6, [sp, #0xc0]
     31c: e58d20b4     	str	r2, [sp, #0xb4]
     320: e59d20c8     	ldr	r2, [sp, #0xc8]
     324: ee169a83     	vnmls.f32	s18, s13, s6
     328: edcd7a0b     	vstr	s15, [sp, #44]
     32c: e0822003     	add	r2, r2, r3
     330: e58d8034     	str	r8, [sp, #0x34]
     334: e58d20c4     	str	r2, [sp, #0xc4]
     338: e59d60c4     	ldr	r6, [sp, #0xc4]
     33c: e5952008     	ldr	r2, [r5, #0x8]
     340: e59d50bc     	ldr	r5, [sp, #0xbc]
     344: e58d9038     	str	r9, [sp, #0x38]
     348: e1a06086     	lsl	r6, r6, #1
     34c: e58db03c     	str	r11, [sp, #0x3c]
     350: e58d70e4     	str	r7, [sp, #0xe4]
     354: e58d60f0     	str	r6, [sp, #0xf0]
     358: e59d60c0     	ldr	r6, [sp, #0xc0]
     35c: ee69aa8a     	vmul.f32	s21, s19, s20
     360: ee29ba0a     	vmul.f32	s22, s18, s20
     364: ee15da25     	vnmls.f32	s26, s10, s11
     368: e1a06086     	lsl	r6, r6, #1
     36c: ee15ca06     	vnmls.f32	s24, s10, s12
     370: ee28aa0a     	vmul.f32	s20, s16, s20
     374: e58d60f4     	str	r6, [sp, #0xf4]
     378: e1a06087     	lsl	r6, r7, #1
     37c: e58d60f8     	str	r6, [sp, #0xf8]
     380: e0856003     	add	r6, r5, r3
     384: e58d60e0     	str	r6, [sp, #0xe0]
     388: e0493003     	sub	r3, r9, r3
     38c: e59d50b8     	ldr	r5, [sp, #0xb8]
     390: e58d30d4     	str	r3, [sp, #0xd4]
     394: e0483001     	sub	r3, r8, r1
     398: e58d30d0     	str	r3, [sp, #0xd0]
     39c: e04b3004     	sub	r3, r11, r4
     3a0: e0856001     	add	r6, r5, r1
     3a4: e59d50b4     	ldr	r5, [sp, #0xb4]
     3a8: e58d60dc     	str	r6, [sp, #0xdc]
     3ac: e58d30cc     	str	r3, [sp, #0xcc]
     3b0: e0855004     	add	r5, r5, r4
     3b4: e58d50d8     	str	r5, [sp, #0xd8]
     3b8: da0002b1     	ble	0xe84 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xe84> @ imm = #0xac4
     3bc: e1a0500e     	mov	r5, lr
     3c0: e58da138     	str	r10, [sp, #0x138]
     3c4: e59a3004     	ldr	r3, [r10, #0x4]
     3c8: e3a06000     	mov	r6, #0
     3cc: e3a08b3e     	mov	r8, #63488
     3d0: e1a07006     	mov	r7, r6
     3d4: e3408007     	movt	r8, #0x7
     3d8: e1a09006     	mov	r9, r6
     3dc: e043b000     	sub	r11, r3, r0
     3e0: e5953004     	ldr	r3, [r5, #0x4]
     3e4: e58d60a8     	str	r6, [sp, #0xa8]
     3e8: e1a0400c     	mov	r4, r12
     3ec: e58d8000     	str	r8, [sp]
     3f0: e1a08007     	mov	r8, r7
     3f4: e0431000     	sub	r1, r3, r0
     3f8: e1a03006     	mov	r3, r6
     3fc: e34037f8     	movt	r3, #0x7f8
     400: e3a00001     	mov	r0, #1
     404: e58d0004     	str	r0, [sp, #0x4]
     408: e1a00003     	mov	r0, r3
     40c: eef7ba00     	vmov.f32	s23, #1.000000e+00
     410: e1a03005     	mov	r3, r5
     414: eef08a00     	vmov.f32	s17, #2.000000e+00
     418: e1a0600e     	mov	r6, lr
     41c: e1a07002     	mov	r7, r2
     420: e1a05000     	mov	r5, r0
     424: e58dc13c     	str	r12, [sp, #0x13c]
     428: e5942000     	ldr	r2, [r4]
     42c: e3510000     	cmp	r1, #0
     430: e58d2008     	str	r2, [sp, #0x8]
     434: da00027a     	ble	0xe24 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xe24> @ imm = #0x9e8
     438: e5933000     	ldr	r3, [r3]
     43c: e0433002     	sub	r3, r3, r2
     440: e1a00803     	lsl	r0, r3, #16
     444: ebfffffe     	bl	0x444 <ZB_fillTriangleMappingPerspective_DT0_DW0+0x444> @ imm = #-0x8
     448: e6ff2070     	uxth	r2, r0
     44c: e1a03840     	asr	r3, r0, #16
     450: ee1d1a90     	vmov	r1, s27
     454: e58d20b0     	str	r2, [sp, #0xb0]
     458: ee1e2a10     	vmov	r2, s28
     45c: ee073a90     	vmov	s15, r3
     460: e0222193     	mla	r2, r3, r1, r2
     464: e59d1028     	ldr	r1, [sp, #0x28]
     468: eeb86ae7     	vcvt.f32.s32	s12, s15
     46c: e58d3120     	str	r3, [sp, #0x120]
     470: e58d2110     	str	r2, [sp, #0x110]
     474: ee1e2a90     	vmov	r2, s29
     478: e0222193     	mla	r2, r3, r1, r2
     47c: e59d102c     	ldr	r1, [sp, #0x2c]
     480: e58d2114     	str	r2, [sp, #0x114]
     484: ee1f2a10     	vmov	r2, s30
     488: e0222193     	mla	r2, r3, r1, r2
     48c: e59d1030     	ldr	r1, [sp, #0x30]
     490: e58d2118     	str	r2, [sp, #0x118]
     494: ee1f2a90     	vmov	r2, s31
     498: e0222193     	mla	r2, r3, r1, r2
     49c: e2833001     	add	r3, r3, #1
     4a0: e58d310c     	str	r3, [sp, #0x10c]
     4a4: e58d211c     	str	r2, [sp, #0x11c]
     4a8: e59d3004     	ldr	r3, [sp, #0x4]
     4ac: ed947a08     	vldr	s14, [r4, #32]
     4b0: edd43a09     	vldr	s7, [r4, #36]
     4b4: e3530000     	cmp	r3, #0
     4b8: e5943008     	ldr	r3, [r4, #0x8]
     4bc: e58d3094     	str	r3, [sp, #0x94]
     4c0: e5943014     	ldr	r3, [r4, #0x14]
     4c4: e58d3010     	str	r3, [sp, #0x10]
     4c8: e5943018     	ldr	r3, [r4, #0x18]
     4cc: e58d3014     	str	r3, [sp, #0x14]
     4d0: e594301c     	ldr	r3, [r4, #0x1c]
     4d4: e58d3018     	str	r3, [sp, #0x18]
     4d8: 1a00026c     	bne	0xe90 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xe90> @ imm = #0x9b0
     4dc: e35b0000     	cmp	r11, #0
     4e0: da000210     	ble	0xd28 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xd28> @ imm = #0x840
     4e4: e59d2110     	ldr	r2, [sp, #0x110]
     4e8: eef01a4d     	vmov.f32	s3, s26
     4ec: eeb02a4c     	vmov.f32	s4, s24
     4f0: ee1d3a90     	vmov	r3, s27
     4f4: e0833002     	add	r3, r3, r2
     4f8: ee491a86     	vmla.f32	s3, s19, s12
     4fc: e59d2114     	ldr	r2, [sp, #0x114]
     500: ee092a06     	vmla.f32	s4, s18, s12
     504: e58d30fc     	str	r3, [sp, #0xfc]
     508: e59d3028     	ldr	r3, [sp, #0x28]
     50c: e58d8090     	str	r8, [sp, #0x90]
     510: e0833002     	add	r3, r3, r2
     514: e59d2118     	ldr	r2, [sp, #0x118]
     518: e58d3100     	str	r3, [sp, #0x100]
     51c: e59d302c     	ldr	r3, [sp, #0x2c]
     520: e58d8128     	str	r8, [sp, #0x128]
     524: e1a08007     	mov	r8, r7
     528: e58da134     	str	r10, [sp, #0x134]
     52c: e0833002     	add	r3, r3, r2
     530: ee792aa1     	vadd.f32	s5, s19, s3
     534: e59d211c     	ldr	r2, [sp, #0x11c]
     538: e58d3104     	str	r3, [sp, #0x104]
     53c: ee393a02     	vadd.f32	s6, s18, s4
     540: e59d3030     	ldr	r3, [sp, #0x30]
     544: e59d7008     	ldr	r7, [sp, #0x8]
     548: e59da004     	ldr	r10, [sp, #0x4]
     54c: e58d912c     	str	r9, [sp, #0x12c]
     550: e1a09005     	mov	r9, r5
     554: e0833002     	add	r3, r3, r2
     558: e58db08c     	str	r11, [sp, #0x8c]
     55c: e58d3108     	str	r3, [sp, #0x108]
     560: e58db124     	str	r11, [sp, #0x124]
     564: e58d6130     	str	r6, [sp, #0x130]
     568: e59d308c     	ldr	r3, [sp, #0x8c]
     56c: eddd7a25     	vldr	s15, [sp, #148]
     570: e2433001     	sub	r3, r3, #1
     574: e58d308c     	str	r3, [sp, #0x8c]
     578: e59d3090     	ldr	r3, [sp, #0x90]
     57c: eef80ae7     	vcvt.f32.s32	s1, s15
     580: e0670843     	rsb	r0, r7, r3, asr #16
     584: e59d3098     	ldr	r3, [sp, #0x98]
     588: eecb7aa0     	vdiv.f32	s15, s23, s1
     58c: e3500006     	cmp	r0, #6
     590: e58d0088     	str	r0, [sp, #0x88]
     594: e0831107     	add	r1, r3, r7, lsl #2
     598: da0001fc     	ble	0xd90 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xd90> @ imm = #0x7f0
     59c: e59d3010     	ldr	r3, [sp, #0x10]
     5a0: e2816020     	add	r6, r1, #32
     5a4: e59d20c8     	ldr	r2, [sp, #0xc8]
     5a8: e3a0c000     	mov	r12, #0
     5ac: e59d10ec     	ldr	r1, [sp, #0xec]
     5b0: e1a05006     	mov	r5, r6
     5b4: e59de0e8     	ldr	lr, [sp, #0xe8]
     5b8: e1a0400c     	mov	r4, r12
     5bc: e0832002     	add	r2, r3, r2
     5c0: eef04a63     	vmov.f32	s9, s7
     5c4: e58d2084     	str	r2, [sp, #0x84]
     5c8: e59d2014     	ldr	r2, [sp, #0x14]
     5cc: eeb04a47     	vmov.f32	s8, s14
     5d0: e0821001     	add	r1, r2, r1
     5d4: e58d1080     	str	r1, [sp, #0x80]
     5d8: e59d1018     	ldr	r1, [sp, #0x18]
     5dc: e081e00e     	add	lr, r1, lr
     5e0: e58de07c     	str	lr, [sp, #0x7c]
     5e4: e59de0c4     	ldr	lr, [sp, #0xc4]
     5e8: e08ee003     	add	lr, lr, r3
     5ec: e58de078     	str	lr, [sp, #0x78]
     5f0: e59de0c0     	ldr	lr, [sp, #0xc0]
     5f4: e08ee002     	add	lr, lr, r2
     5f8: e58de074     	str	lr, [sp, #0x74]
     5fc: e59de0e4     	ldr	lr, [sp, #0xe4]
     600: e08ee001     	add	lr, lr, r1
     604: e58de070     	str	lr, [sp, #0x70]
     608: e59de0bc     	ldr	lr, [sp, #0xbc]
     60c: e08ee003     	add	lr, lr, r3
     610: e58de06c     	str	lr, [sp, #0x6c]
     614: e59de0b8     	ldr	lr, [sp, #0xb8]
     618: e08ee002     	add	lr, lr, r2
     61c: e58de068     	str	lr, [sp, #0x68]
     620: e59de0b4     	ldr	lr, [sp, #0xb4]
     624: e08ee001     	add	lr, lr, r1
     628: e58de064     	str	lr, [sp, #0x64]
     62c: e59de0e0     	ldr	lr, [sp, #0xe0]
     630: e08ee003     	add	lr, lr, r3
     634: e58de060     	str	lr, [sp, #0x60]
     638: e59de0dc     	ldr	lr, [sp, #0xdc]
     63c: e08ee002     	add	lr, lr, r2
     640: e58de05c     	str	lr, [sp, #0x5c]
     644: e59de0d8     	ldr	lr, [sp, #0xd8]
     648: e08ee001     	add	lr, lr, r1
     64c: e58de058     	str	lr, [sp, #0x58]
     650: e59de0f0     	ldr	lr, [sp, #0xf0]
     654: e08ee003     	add	lr, lr, r3
     658: e58de054     	str	lr, [sp, #0x54]
     65c: e59de0f4     	ldr	lr, [sp, #0xf4]
     660: e08ee002     	add	lr, lr, r2
     664: e58de050     	str	lr, [sp, #0x50]
     668: e59de0f8     	ldr	lr, [sp, #0xf8]
     66c: e08ee001     	add	lr, lr, r1
     670: e58de04c     	str	lr, [sp, #0x4c]
     674: e59de0d4     	ldr	lr, [sp, #0xd4]
     678: e58d000c     	str	r0, [sp, #0xc]
     67c: e59d0028     	ldr	r0, [sp, #0x28]
     680: e08ee003     	add	lr, lr, r3
     684: e58da09c     	str	r10, [sp, #0x9c]
     688: e58de048     	str	lr, [sp, #0x48]
     68c: e59de0d0     	ldr	lr, [sp, #0xd0]
     690: e0803003     	add	r3, r0, r3
     694: e58d301c     	str	r3, [sp, #0x1c]
     698: e59d302c     	ldr	r3, [sp, #0x2c]
     69c: e08ee002     	add	lr, lr, r2
     6a0: e58d70a0     	str	r7, [sp, #0xa0]
     6a4: e58de044     	str	lr, [sp, #0x44]
     6a8: e59de0cc     	ldr	lr, [sp, #0xcc]
     6ac: e0833002     	add	r3, r3, r2
     6b0: e58d3020     	str	r3, [sp, #0x20]
     6b4: e59d3030     	ldr	r3, [sp, #0x30]
     6b8: e08ee001     	add	lr, lr, r1
     6bc: e58d8004     	str	r8, [sp, #0x4]
     6c0: e58de040     	str	lr, [sp, #0x40]
     6c4: e1a0e00c     	mov	lr, r12
     6c8: e0833001     	add	r3, r3, r1
     6cc: e58d60a4     	str	r6, [sp, #0xa4]
     6d0: e58d3024     	str	r3, [sp, #0x24]
     6d4: e58d9008     	str	r9, [sp, #0x8]
     6d8: ee646a27     	vmul.f32	s13, s8, s15
     6dc: e2855020     	add	r5, r5, #32
     6e0: eeb05a69     	vmov.f32	s10, s19
     6e4: ee240aa7     	vmul.f32	s0, s9, s15
     6e8: eef05a49     	vmov.f32	s11, s18
     6ec: ee085a66     	vmls.f32	s10, s16, s13
     6f0: eefd6ae6     	vcvt.s32.f32	s13, s13
     6f4: ee485a40     	vmls.f32	s11, s16, s0
     6f8: e59d300c     	ldr	r3, [sp, #0xc]
     6fc: ee162a90     	vmov	r2, s13
     700: e2433008     	sub	r3, r3, #8
     704: eefd6ac0     	vcvt.s32.f32	s13, s0
     708: e3530006     	cmp	r3, #6
     70c: ee255a27     	vmul.f32	s10, s10, s15
     710: e59d6018     	ldr	r6, [sp, #0x18]
     714: e59d8000     	ldr	r8, [sp]
     718: ee655aa7     	vmul.f32	s11, s11, s15
     71c: e086900c     	add	r9, r6, r12
     720: e58d300c     	str	r3, [sp, #0xc]
     724: e59d6008     	ldr	r6, [sp, #0x8]
     728: e0027008     	and	r7, r2, r8
     72c: ee163a90     	vmov	r3, s13
     730: eefd6ac5     	vcvt.s32.f32	s13, s10
     734: e59d1010     	ldr	r1, [sp, #0x10]
     738: e0036006     	and	r6, r3, r6
     73c: e1866007     	orr	r6, r6, r7
     740: e59d7004     	ldr	r7, [sp, #0x4]
     744: ee700a8a     	vadd.f32	s1, s1, s20
     748: ee160a90     	vmov	r0, s13
     74c: e081b004     	add	r11, r1, r4
     750: eefd6ae5     	vcvt.s32.f32	s13, s11
     754: e0802002     	add	r2, r0, r2
     758: e59d1014     	ldr	r1, [sp, #0x14]
     75c: e0028008     	and	r8, r2, r8
     760: e79774a6     	ldr	r7, [r7, r6, lsr #9]
     764: e0802002     	add	r2, r0, r2
     768: e59d6008     	ldr	r6, [sp, #0x8]
     76c: e081a00e     	add	r10, r1, lr
     770: ee161a90     	vmov	r1, s13
     774: e0813003     	add	r3, r1, r3
     778: eeb01a68     	vmov.f32	s2, s17
     77c: e0036006     	and	r6, r3, r6
     780: e0813003     	add	r3, r1, r3
     784: e1888006     	orr	r8, r8, r6
     788: e7e76857     	ubfx	r6, r7, #0x10, #0x8
     78c: e006069b     	mul	r6, r11, r6
     790: e7e7b457     	ubfx	r11, r7, #0x8, #0x8
     794: e6ef7077     	uxtb	r7, r7
     798: ee001ae7     	vmls.f32	s2, s1, s15
     79c: e00b0b9a     	mul	r11, r10, r11
     7a0: e59da01c     	ldr	r10, [sp, #0x1c]
     7a4: ee344a2a     	vadd.f32	s8, s8, s21
     7a8: e0070799     	mul	r7, r9, r7
     7ac: e1a06426     	lsr	r6, r6, #8
     7b0: e1a0b82b     	lsr	r11, r11, #16
     7b4: e20668ff     	and	r6, r6, #16711680
     7b8: e59d9024     	ldr	r9, [sp, #0x24]
     7bc: e20bbcff     	and	r11, r11, #65280
     7c0: ee744a8b     	vadd.f32	s9, s9, s22
     7c4: e186600b     	orr	r6, r6, r11
     7c8: e08ab004     	add	r11, r10, r4
     7cc: e1866c27     	orr	r6, r6, r7, lsr #24
     7d0: e59d7004     	ldr	r7, [sp, #0x4]
     7d4: e59da020     	ldr	r10, [sp, #0x20]
     7d8: e089900c     	add	r9, r9, r12
     7dc: e5056040     	str	r6, [r5, #-0x40]
     7e0: e79774a8     	ldr	r7, [r7, r8, lsr #9]
     7e4: e08aa00e     	add	r10, r10, lr
     7e8: e59d8000     	ldr	r8, [sp]
     7ec: ee617a27     	vmul.f32	s15, s2, s15
     7f0: e7e76857     	ubfx	r6, r7, #0x10, #0x8
     7f4: eeb01a68     	vmov.f32	s2, s17
     7f8: e006069b     	mul	r6, r11, r6
     7fc: e7e7b457     	ubfx	r11, r7, #0x8, #0x8
     800: e6ef7077     	uxtb	r7, r7
     804: e0028008     	and	r8, r2, r8
     808: e00b0b9a     	mul	r11, r10, r11
     80c: e59da084     	ldr	r10, [sp, #0x84]
     810: e0802002     	add	r2, r0, r2
     814: ee001ae7     	vmls.f32	s2, s1, s15
     818: e0070997     	mul	r7, r7, r9
     81c: e1a06426     	lsr	r6, r6, #8
     820: e59d907c     	ldr	r9, [sp, #0x7c]
     824: e1a0b82b     	lsr	r11, r11, #16
     828: e20668ff     	and	r6, r6, #16711680
     82c: e20bbcff     	and	r11, r11, #65280
     830: e089900c     	add	r9, r9, r12
     834: e186600b     	orr	r6, r6, r11
     838: e08ab004     	add	r11, r10, r4
     83c: e1866c27     	orr	r6, r6, r7, lsr #24
     840: e59d7004     	ldr	r7, [sp, #0x4]
     844: e59da080     	ldr	r10, [sp, #0x80]
     848: e505603c     	str	r6, [r5, #-0x3c]
     84c: e59d6008     	ldr	r6, [sp, #0x8]
     850: e08aa00e     	add	r10, r10, lr
     854: ee617a27     	vmul.f32	s15, s2, s15
     858: e0036006     	and	r6, r3, r6
     85c: e0813003     	add	r3, r1, r3
     860: e1886006     	orr	r6, r8, r6
     864: e59d8000     	ldr	r8, [sp]
     868: e79774a6     	ldr	r7, [r7, r6, lsr #9]
     86c: e0028008     	and	r8, r2, r8
     870: e0802002     	add	r2, r0, r2
     874: e7e76857     	ubfx	r6, r7, #0x10, #0x8
     878: e006069b     	mul	r6, r11, r6
     87c: e7e7b457     	ubfx	r11, r7, #0x8, #0x8
     880: e6ef7077     	uxtb	r7, r7
     884: e00b0b9a     	mul	r11, r10, r11
     888: e59da078     	ldr	r10, [sp, #0x78]
     88c: e0070799     	mul	r7, r9, r7
     890: e1a06426     	lsr	r6, r6, #8
     894: e59d9070     	ldr	r9, [sp, #0x70]
     898: e1a0b82b     	lsr	r11, r11, #16
     89c: e20668ff     	and	r6, r6, #16711680
     8a0: e20bbcff     	and	r11, r11, #65280
     8a4: e089900c     	add	r9, r9, r12
     8a8: e186600b     	orr	r6, r6, r11
     8ac: e08ab004     	add	r11, r10, r4
     8b0: e1866c27     	orr	r6, r6, r7, lsr #24
     8b4: e59d7004     	ldr	r7, [sp, #0x4]
     8b8: e59da074     	ldr	r10, [sp, #0x74]
     8bc: e5056038     	str	r6, [r5, #-0x38]
     8c0: e59d6008     	ldr	r6, [sp, #0x8]
     8c4: e08aa00e     	add	r10, r10, lr
     8c8: e0036006     	and	r6, r3, r6
     8cc: e0813003     	add	r3, r1, r3
     8d0: e1886006     	orr	r6, r8, r6
     8d4: e79774a6     	ldr	r7, [r7, r6, lsr #9]
     8d8: e59d8000     	ldr	r8, [sp]
     8dc: e7e76857     	ubfx	r6, r7, #0x10, #0x8
     8e0: e006069b     	mul	r6, r11, r6
     8e4: e7e7b457     	ubfx	r11, r7, #0x8, #0x8
     8e8: e6ef7077     	uxtb	r7, r7
     8ec: e0028008     	and	r8, r2, r8
     8f0: e00b0b9a     	mul	r11, r10, r11
     8f4: e59da06c     	ldr	r10, [sp, #0x6c]
     8f8: e0802002     	add	r2, r0, r2
     8fc: e0070799     	mul	r7, r9, r7
     900: e1a06426     	lsr	r6, r6, #8
     904: e59d9004     	ldr	r9, [sp, #0x4]
     908: e08aa004     	add	r10, r10, r4
     90c: e1a0b82b     	lsr	r11, r11, #16
     910: e20668ff     	and	r6, r6, #16711680
     914: e20bbcff     	and	r11, r11, #65280
     918: e186600b     	orr	r6, r6, r11
     91c: e59db000     	ldr	r11, [sp]
     920: e1866c27     	orr	r6, r6, r7, lsr #24
     924: e59d7068     	ldr	r7, [sp, #0x68]
     928: e5056034     	str	r6, [r5, #-0x34]
     92c: e59d6008     	ldr	r6, [sp, #0x8]
     930: e0036006     	and	r6, r3, r6
     934: e0813003     	add	r3, r1, r3
     938: e1886006     	orr	r6, r8, r6
     93c: e79964a6     	ldr	r6, [r9, r6, lsr #9]
     940: e087900e     	add	r9, r7, lr
     944: e59d7064     	ldr	r7, [sp, #0x64]
     948: e087800c     	add	r8, r7, r12
     94c: e002700b     	and	r7, r2, r11
     950: e0802002     	add	r2, r0, r2
     954: e0800002     	add	r0, r0, r2
     958: e002200b     	and	r2, r2, r11
     95c: e000000b     	and	r0, r0, r11
     960: e7e7b856     	ubfx	r11, r6, #0x10, #0x8
     964: e00a0b9a     	mul	r10, r10, r11
     968: e7e7b456     	ubfx	r11, r6, #0x8, #0x8
     96c: e6ef6076     	uxtb	r6, r6
     970: e00b0b99     	mul	r11, r9, r11
     974: e59d9004     	ldr	r9, [sp, #0x4]
     978: e0060698     	mul	r6, r8, r6
     97c: e1a0a42a     	lsr	r10, r10, #8
     980: e59d8008     	ldr	r8, [sp, #0x8]
     984: e1a0b82b     	lsr	r11, r11, #16
     988: e20aa8ff     	and	r10, r10, #16711680
     98c: e20bbcff     	and	r11, r11, #65280
     990: e18aa00b     	orr	r10, r10, r11
     994: e18aac26     	orr	r10, r10, r6, lsr #24
     998: e0036008     	and	r6, r3, r8
     99c: e0813003     	add	r3, r1, r3
     9a0: e0811003     	add	r1, r1, r3
     9a4: e1876006     	orr	r6, r7, r6
     9a8: e0033008     	and	r3, r3, r8
     9ac: e0011008     	and	r1, r1, r8
     9b0: e505a030     	str	r10, [r5, #-0x30]
     9b4: e1823003     	orr	r3, r2, r3
     9b8: e79964a6     	ldr	r6, [r9, r6, lsr #9]
     9bc: e1801001     	orr	r1, r0, r1
     9c0: e59d2060     	ldr	r2, [sp, #0x60]
     9c4: e59d005c     	ldr	r0, [sp, #0x5c]
     9c8: e0822004     	add	r2, r2, r4
     9cc: e080700e     	add	r7, r0, lr
     9d0: e7e70856     	ubfx	r0, r6, #0x10, #0x8
     9d4: e0000092     	mul	r0, r2, r0
     9d8: e7e72456     	ubfx	r2, r6, #0x8, #0x8
     9dc: e6ef6076     	uxtb	r6, r6
     9e0: e0020297     	mul	r2, r7, r2
     9e4: e1a00420     	lsr	r0, r0, #8
     9e8: e1a02822     	lsr	r2, r2, #16
     9ec: e20008ff     	and	r0, r0, #16711680
     9f0: e2022cff     	and	r2, r2, #65280
     9f4: e1802002     	orr	r2, r0, r2
     9f8: e59d0058     	ldr	r0, [sp, #0x58]
     9fc: e080000c     	add	r0, r0, r12
     a00: e0060690     	mul	r6, r0, r6
     a04: e1822c26     	orr	r2, r2, r6, lsr #24
     a08: e505202c     	str	r2, [r5, #-0x2c]
     a0c: e79924a3     	ldr	r2, [r9, r3, lsr #9]
     a10: e59d3054     	ldr	r3, [sp, #0x54]
     a14: e0838004     	add	r8, r3, r4
     a18: e59d3050     	ldr	r3, [sp, #0x50]
     a1c: e083700e     	add	r7, r3, lr
     a20: e59d304c     	ldr	r3, [sp, #0x4c]
     a24: e083600c     	add	r6, r3, r12
     a28: e59d3048     	ldr	r3, [sp, #0x48]
     a2c: e0830004     	add	r0, r3, r4
     a30: e59d3038     	ldr	r3, [sp, #0x38]
     a34: e0844003     	add	r4, r4, r3
     a38: e7e73852     	ubfx	r3, r2, #0x10, #0x8
     a3c: e0030398     	mul	r3, r8, r3
     a40: e7e78452     	ubfx	r8, r2, #0x8, #0x8
     a44: e6ef2072     	uxtb	r2, r2
     a48: e0080897     	mul	r8, r7, r8
     a4c: e0020296     	mul	r2, r6, r2
     a50: e1a03423     	lsr	r3, r3, #8
     a54: e1a08828     	lsr	r8, r8, #16
     a58: e20338ff     	and	r3, r3, #16711680
     a5c: e2088cff     	and	r8, r8, #65280
     a60: e1833008     	orr	r3, r3, r8
     a64: e1833c22     	orr	r3, r3, r2, lsr #24
     a68: e5053028     	str	r3, [r5, #-0x28]
     a6c: e59d3044     	ldr	r3, [sp, #0x44]
     a70: e79924a1     	ldr	r2, [r9, r1, lsr #9]
     a74: e083600e     	add	r6, r3, lr
     a78: e59d3034     	ldr	r3, [sp, #0x34]
     a7c: e08ee003     	add	lr, lr, r3
     a80: e59d3040     	ldr	r3, [sp, #0x40]
     a84: e083100c     	add	r1, r3, r12
     a88: e59d303c     	ldr	r3, [sp, #0x3c]
     a8c: e08cc003     	add	r12, r12, r3
     a90: e7e73852     	ubfx	r3, r2, #0x10, #0x8
     a94: e0030390     	mul	r3, r0, r3
     a98: e7e70452     	ubfx	r0, r2, #0x8, #0x8
     a9c: e6ef2072     	uxtb	r2, r2
     aa0: e0000096     	mul	r0, r6, r0
     aa4: e0020291     	mul	r2, r1, r2
     aa8: e1a03423     	lsr	r3, r3, #8
     aac: e1a00820     	lsr	r0, r0, #16
     ab0: e20338ff     	and	r3, r3, #16711680
     ab4: e2000cff     	and	r0, r0, #65280
     ab8: e1833000     	orr	r3, r3, r0
     abc: e1833c22     	orr	r3, r3, r2, lsr #24
     ac0: e5053024     	str	r3, [r5, #-0x24]
     ac4: caffff03     	bgt	0x6d8 <ZB_fillTriangleMappingPerspective_DT0_DW0+0x6d8> @ imm = #-0x3f4
     ac8: e59d1030     	ldr	r1, [sp, #0x30]
     acc: e1a08009     	mov	r8, r9
     ad0: e59d3024     	ldr	r3, [sp, #0x24]
     ad4: e59dc028     	ldr	r12, [sp, #0x28]
     ad8: e59de02c     	ldr	lr, [sp, #0x2c]
     adc: e59d60a4     	ldr	r6, [sp, #0xa4]
     ae0: e0830101     	add	r0, r3, r1, lsl #2
     ae4: e59d301c     	ldr	r3, [sp, #0x1c]
     ae8: e59da09c     	ldr	r10, [sp, #0x9c]
     aec: e0800081     	add	r0, r0, r1, lsl #1
     af0: e59d70a0     	ldr	r7, [sp, #0xa0]
     af4: e59d9008     	ldr	r9, [sp, #0x8]
     af8: e083210c     	add	r2, r3, r12, lsl #2
     afc: e59d3020     	ldr	r3, [sp, #0x20]
     b00: e0800001     	add	r0, r0, r1
     b04: e59d1088     	ldr	r1, [sp, #0x88]
     b08: e082208c     	add	r2, r2, r12, lsl #1
     b0c: e083310e     	add	r3, r3, lr, lsl #2
     b10: e082200c     	add	r2, r2, r12
     b14: e241c007     	sub	r12, r1, #7
     b18: e083308e     	add	r3, r3, lr, lsl #1
     b1c: e083300e     	add	r3, r3, lr
     b20: e241e008     	sub	lr, r1, #8
     b24: e1a011ac     	lsr	r1, r12, #3
     b28: e3ccc007     	bic	r12, r12, #7
     b2c: e04ec00c     	sub	r12, lr, r12
     b30: e58dc088     	str	r12, [sp, #0x88]
     b34: e59dc03c     	ldr	r12, [sp, #0x3c]
     b38: e0200c91     	mla	r0, r1, r12, r0
     b3c: e58d000c     	str	r0, [sp, #0xc]
     b40: e59d0038     	ldr	r0, [sp, #0x38]
     b44: e0222091     	mla	r2, r1, r0, r2
     b48: e58d2004     	str	r2, [sp, #0x4]
     b4c: e59d2034     	ldr	r2, [sp, #0x34]
     b50: e0233291     	mla	r3, r1, r2, r3
     b54: e0861281     	add	r1, r6, r1, lsl #5
     b58: e58d3008     	str	r3, [sp, #0x8]
     b5c: e59d3088     	ldr	r3, [sp, #0x88]
     b60: e3530000     	cmp	r3, #0
     b64: ba00003f     	blt	0xc68 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xc68> @ imm = #0xfc
     b68: ee244a27     	vmul.f32	s8, s8, s15
     b6c: e3a0e000     	mov	lr, #0
     b70: ee644aa7     	vmul.f32	s9, s9, s15
     b74: e1a0500e     	mov	r5, lr
     b78: eef05a69     	vmov.f32	s11, s19
     b7c: e1a0400e     	mov	r4, lr
     b80: eef06a49     	vmov.f32	s13, s18
     b84: ee485a44     	vmls.f32	s11, s16, s8
     b88: ee486a64     	vmls.f32	s13, s16, s9
     b8c: e59d3088     	ldr	r3, [sp, #0x88]
     b90: e59db000     	ldr	r11, [sp]
     b94: e2836001     	add	r6, r3, #1
     b98: e0813106     	add	r3, r1, r6, lsl #2
     b9c: e58da040     	str	r10, [sp, #0x40]
     ba0: e58d7044     	str	r7, [sp, #0x44]
     ba4: ee655aa7     	vmul.f32	s11, s11, s15
     ba8: ee667aa7     	vmul.f32	s15, s13, s15
     bac: eefd6ac4     	vcvt.s32.f32	s13, s8
     bb0: e58d301c     	str	r3, [sp, #0x1c]
     bb4: eefd7ae7     	vcvt.s32.f32	s15, s15
     bb8: ee16ca90     	vmov	r12, s13
     bbc: eefd6ae4     	vcvt.s32.f32	s13, s9
     bc0: edcd7a09     	vstr	s15, [sp, #36]
     bc4: ee160a90     	vmov	r0, s13
     bc8: eefd6ae5     	vcvt.s32.f32	s13, s11
     bcc: edcd6a08     	vstr	s13, [sp, #32]
     bd0: e59d2020     	ldr	r2, [sp, #0x20]
     bd4: e00c300b     	and	r3, r12, r11
     bd8: e08cc002     	add	r12, r12, r2
     bdc: e0002009     	and	r2, r0, r9
     be0: e1833002     	orr	r3, r3, r2
     be4: e59d2024     	ldr	r2, [sp, #0x24]
     be8: e0800002     	add	r0, r0, r2
     bec: e79824a3     	ldr	r2, [r8, r3, lsr #9]
     bf0: e59d3004     	ldr	r3, [sp, #0x4]
     bf4: e083a005     	add	r10, r3, r5
     bf8: e59d3028     	ldr	r3, [sp, #0x28]
     bfc: e0855003     	add	r5, r5, r3
     c00: e59d3008     	ldr	r3, [sp, #0x8]
     c04: e0837004     	add	r7, r3, r4
     c08: e59d302c     	ldr	r3, [sp, #0x2c]
     c0c: e0844003     	add	r4, r4, r3
     c10: e59d300c     	ldr	r3, [sp, #0xc]
     c14: e083600e     	add	r6, r3, lr
     c18: e59d3030     	ldr	r3, [sp, #0x30]
     c1c: e08ee003     	add	lr, lr, r3
     c20: e7e73852     	ubfx	r3, r2, #0x10, #0x8
     c24: e003039a     	mul	r3, r10, r3
     c28: e7e7a452     	ubfx	r10, r2, #0x8, #0x8
     c2c: e6ef2072     	uxtb	r2, r2
     c30: e00a0a97     	mul	r10, r7, r10
     c34: e0020296     	mul	r2, r6, r2
     c38: e1a03423     	lsr	r3, r3, #8
     c3c: e1a0a82a     	lsr	r10, r10, #16
     c40: e20338ff     	and	r3, r3, #16711680
     c44: e20aacff     	and	r10, r10, #65280
     c48: e183300a     	orr	r3, r3, r10
     c4c: e1833c22     	orr	r3, r3, r2, lsr #24
     c50: e4813004     	str	r3, [r1], #4
     c54: e59d301c     	ldr	r3, [sp, #0x1c]
     c58: e1510003     	cmp	r1, r3
     c5c: 1affffdb     	bne	0xbd0 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xbd0> @ imm = #-0x94
     c60: e59da040     	ldr	r10, [sp, #0x40]
     c64: e59d7044     	ldr	r7, [sp, #0x44]
     c68: e59d30b0     	ldr	r3, [sp, #0xb0]
     c6c: e08aa003     	add	r10, r10, r3
     c70: e35a0000     	cmp	r10, #0
     c74: da000030     	ble	0xd3c <ZB_fillTriangleMappingPerspective_DT0_DW0+0xd3c> @ imm = #0xc0
     c78: e59d310c     	ldr	r3, [sp, #0x10c]
     c7c: e24aa801     	sub	r10, r10, #65536
     c80: e59d20fc     	ldr	r2, [sp, #0xfc]
     c84: ee377a22     	vadd.f32	s14, s14, s5
     c88: e0877003     	add	r7, r7, r3
     c8c: e59d3094     	ldr	r3, [sp, #0x94]
     c90: ee733a83     	vadd.f32	s7, s7, s6
     c94: e0833002     	add	r3, r3, r2
     c98: e59d2100     	ldr	r2, [sp, #0x100]
     c9c: e58d3094     	str	r3, [sp, #0x94]
     ca0: e59d3010     	ldr	r3, [sp, #0x10]
     ca4: e0833002     	add	r3, r3, r2
     ca8: e59d2104     	ldr	r2, [sp, #0x104]
     cac: e58d3010     	str	r3, [sp, #0x10]
     cb0: e59d3014     	ldr	r3, [sp, #0x14]
     cb4: e0833002     	add	r3, r3, r2
     cb8: e59d2108     	ldr	r2, [sp, #0x108]
     cbc: e58d3014     	str	r3, [sp, #0x14]
     cc0: e59d3018     	ldr	r3, [sp, #0x18]
     cc4: e0833002     	add	r3, r3, r2
     cc8: e58d3018     	str	r3, [sp, #0x18]
     ccc: e59d308c     	ldr	r3, [sp, #0x8c]
     cd0: e59d20a8     	ldr	r2, [sp, #0xa8]
     cd4: e3530000     	cmp	r3, #0
     cd8: e59d3090     	ldr	r3, [sp, #0x90]
     cdc: e0833002     	add	r3, r3, r2
     ce0: e59d2098     	ldr	r2, [sp, #0x98]
     ce4: e58d3090     	str	r3, [sp, #0x90]
     ce8: e59d30ac     	ldr	r3, [sp, #0xac]
     cec: e5933020     	ldr	r3, [r3, #0x20]
     cf0: e0823103     	add	r3, r2, r3, lsl #2
     cf4: e58d3098     	str	r3, [sp, #0x98]
     cf8: 1afffe1a     	bne	0x568 <ZB_fillTriangleMappingPerspective_DT0_DW0+0x568> @ imm = #-0x798
     cfc: e59db124     	ldr	r11, [sp, #0x124]
     d00: e1a05009     	mov	r5, r9
     d04: e59d30a8     	ldr	r3, [sp, #0xa8]
     d08: e58d7008     	str	r7, [sp, #0x8]
     d0c: e1a07008     	mov	r7, r8
     d10: e59d8128     	ldr	r8, [sp, #0x128]
     d14: e58da004     	str	r10, [sp, #0x4]
     d18: e59d6130     	ldr	r6, [sp, #0x130]
     d1c: e59da134     	ldr	r10, [sp, #0x134]
     d20: e59d912c     	ldr	r9, [sp, #0x12c]
     d24: e028839b     	mla	r8, r11, r3, r8
     d28: e3590001     	cmp	r9, #1
     d2c: 1a000020     	bne	0xdb4 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xdb4> @ imm = #0x80
     d30: e28ddf51     	add	sp, sp, #324
     d34: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     d38: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     d3c: e59d3120     	ldr	r3, [sp, #0x120]
     d40: e59d2110     	ldr	r2, [sp, #0x110]
     d44: ee377a21     	vadd.f32	s14, s14, s3
     d48: e0877003     	add	r7, r7, r3
     d4c: e59d3094     	ldr	r3, [sp, #0x94]
     d50: ee733a82     	vadd.f32	s7, s7, s4
     d54: e0833002     	add	r3, r3, r2
     d58: e59d2114     	ldr	r2, [sp, #0x114]
     d5c: e58d3094     	str	r3, [sp, #0x94]
     d60: e59d3010     	ldr	r3, [sp, #0x10]
     d64: e0833002     	add	r3, r3, r2
     d68: e59d2118     	ldr	r2, [sp, #0x118]
     d6c: e58d3010     	str	r3, [sp, #0x10]
     d70: e59d3014     	ldr	r3, [sp, #0x14]
     d74: e0833002     	add	r3, r3, r2
     d78: e59d211c     	ldr	r2, [sp, #0x11c]
     d7c: e58d3014     	str	r3, [sp, #0x14]
     d80: e59d3018     	ldr	r3, [sp, #0x18]
     d84: e0833002     	add	r3, r3, r2
     d88: e58d3018     	str	r3, [sp, #0x18]
     d8c: eaffffce     	b	0xccc <ZB_fillTriangleMappingPerspective_DT0_DW0+0xccc> @ imm = #-0xc8
     d90: e59d3018     	ldr	r3, [sp, #0x18]
     d94: eef04a63     	vmov.f32	s9, s7
     d98: eeb04a47     	vmov.f32	s8, s14
     d9c: e58d300c     	str	r3, [sp, #0xc]
     da0: e59d3014     	ldr	r3, [sp, #0x14]
     da4: e58d3008     	str	r3, [sp, #0x8]
     da8: e59d3010     	ldr	r3, [sp, #0x10]
     dac: e58d3004     	str	r3, [sp, #0x4]
     db0: eaffff69     	b	0xb5c <ZB_fillTriangleMappingPerspective_DT0_DW0+0xb5c> @ imm = #-0x25c
     db4: e5961004     	ldr	r1, [r6, #0x4]
     db8: e59a3004     	ldr	r3, [r10, #0x4]
     dbc: eef5cac0     	vcmpe.f32	s25, #0
     dc0: eef1fa10     	vmrs	APSR_nzcv, fpscr
     dc4: e0411003     	sub	r1, r1, r3
     dc8: e281b001     	add	r11, r1, #1
     dcc: da00001f     	ble	0xe50 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xe50> @ imm = #0x7c
     dd0: e3510000     	cmp	r1, #0
     dd4: e59a8000     	ldr	r8, [r10]
     dd8: e3a04001     	mov	r4, #1
     ddc: c58da13c     	strgt	r10, [sp, #0x13c]
     de0: c58d6138     	strgt	r6, [sp, #0x138]
     de4: da00001e     	ble	0xe64 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xe64> @ imm = #0x78
     de8: e59d3138     	ldr	r3, [sp, #0x138]
     dec: e1a09004     	mov	r9, r4
     df0: ed8d6a08     	vstr	s12, [sp, #32]
     df4: edcd3a07     	vstr	s7, [sp, #28]
     df8: e5930000     	ldr	r0, [r3]
     dfc: ed8d7a03     	vstr	s14, [sp, #12]
     e00: e0400008     	sub	r0, r0, r8
     e04: e1a00800     	lsl	r0, r0, #16
     e08: ebfffffe     	bl	0xe08 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xe08> @ imm = #-0x8
     e0c: ed9d7a03     	vldr	s14, [sp, #12]
     e10: eddd3a07     	vldr	s7, [sp, #28]
     e14: ed9d6a08     	vldr	s12, [sp, #32]
     e18: e58d00a8     	str	r0, [sp, #0xa8]
     e1c: e1a08808     	lsl	r8, r8, #16
     e20: eafffdad     	b	0x4dc <ZB_fillTriangleMappingPerspective_DT0_DW0+0x4dc> @ imm = #-0x94c
     e24: e3a03000     	mov	r3, #0
     e28: e3a02001     	mov	r2, #1
     e2c: ed9f6a26     	vldr	s12, [pc, #152]         @ 0xecc <ZB_fillTriangleMappingPerspective_DT0_DW0+0xecc>
     e30: e58d30b0     	str	r3, [sp, #0xb0]
     e34: edcdfa47     	vstr	s31, [sp, #284]
     e38: ed8dfa46     	vstr	s30, [sp, #280]
     e3c: edcdea45     	vstr	s29, [sp, #276]
     e40: ed8dea44     	vstr	s28, [sp, #272]
     e44: e58d210c     	str	r2, [sp, #0x10c]
     e48: e58d3120     	str	r3, [sp, #0x120]
     e4c: eafffd95     	b	0x4a8 <ZB_fillTriangleMappingPerspective_DT0_DW0+0x4a8> @ imm = #-0x9ac
     e50: e58d9004     	str	r9, [sp, #0x4]
     e54: e1a03006     	mov	r3, r6
     e58: e3a09001     	mov	r9, #1
     e5c: e1a0400a     	mov	r4, r10
     e60: eafffd70     	b	0x428 <ZB_fillTriangleMappingPerspective_DT0_DW0+0x428> @ imm = #-0xa40
     e64: e35b0000     	cmp	r11, #0
     e68: daffffb0     	ble	0xd30 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xd30> @ imm = #-0x140
     e6c: e58d90a8     	str	r9, [sp, #0xa8]
     e70: e1a08808     	lsl	r8, r8, #16
     e74: e1a09004     	mov	r9, r4
     e78: e58da13c     	str	r10, [sp, #0x13c]
     e7c: e58d6138     	str	r6, [sp, #0x138]
     e80: eafffd97     	b	0x4e4 <ZB_fillTriangleMappingPerspective_DT0_DW0+0x4e4> @ imm = #-0x9a4
     e84: e1a0500a     	mov	r5, r10
     e88: e58de138     	str	lr, [sp, #0x138]
     e8c: eafffd4c     	b	0x3c4 <ZB_fillTriangleMappingPerspective_DT0_DW0+0x3c4> @ imm = #-0xad0
     e90: e59d3138     	ldr	r3, [sp, #0x138]
     e94: e59d213c     	ldr	r2, [sp, #0x13c]
     e98: e5931004     	ldr	r1, [r3, #0x4]
     e9c: e5923004     	ldr	r3, [r2, #0x4]
     ea0: e5928000     	ldr	r8, [r2]
     ea4: e0411003     	sub	r1, r1, r3
     ea8: e3510000     	cmp	r1, #0
     eac: c3a03000     	movgt	r3, #0
     eb0: c1a04009     	movgt	r4, r9
     eb4: c58d3004     	strgt	r3, [sp, #0x4]
     eb8: caffffca     	bgt	0xde8 <ZB_fillTriangleMappingPerspective_DT0_DW0+0xde8> @ imm = #-0xd8
     ebc: e3a03000     	mov	r3, #0
     ec0: e58d3004     	str	r3, [sp, #0x4]
     ec4: e58d30a8     	str	r3, [sp, #0xa8]
     ec8: eaffffd3     	b	0xe1c <ZB_fillTriangleMappingPerspective_DT0_DW0+0xe1c> @ imm = #-0xb4
     ecc: 00 00 00 00  	.word	0x00000000

Disassembly of section .text.ZB_fillTriangleMappingPerspective_DT0_DW1:

00000000 <ZB_fillTriangleMappingPerspective_DT0_DW1>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a07003     	mov	r7, r3
       8: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
       c: e24ddf61     	sub	sp, sp, #388
      10: e592c004     	ldr	r12, [r2, #0x4]
      14: e5913004     	ldr	r3, [r1, #0x4]
      18: e58d00ec     	str	r0, [sp, #0xec]
      1c: e5970004     	ldr	r0, [r7, #0x4]
      20: e15c0003     	cmp	r12, r3
      24: a1a0e003     	movge	lr, r3
      28: b1a0e00c     	movlt	lr, r12
      2c: a1a0300c     	movge	r3, r12
      30: b1a06002     	movlt	r6, r2
      34: a1a06001     	movge	r6, r1
      38: b1a08001     	movlt	r8, r1
      3c: a1a08002     	movge	r8, r2
      40: e150000e     	cmp	r0, lr
      44: e1a0c00e     	mov	r12, lr
      48: ba00000b     	blt	0x7c <ZB_fillTriangleMappingPerspective_DT0_DW1+0x7c> @ imm = #0x2c
      4c: e1500003     	cmp	r0, r3
      50: e1a02007     	mov	r2, r7
      54: a1a0c003     	movge	r12, r3
      58: a1a07006     	movge	r7, r6
      5c: a1a03000     	movge	r3, r0
      60: a1a06008     	movge	r6, r8
      64: b1a0c000     	movlt	r12, r0
      68: b1a07006     	movlt	r7, r6
      6c: a1a0000e     	movge	r0, lr
      70: a1a08002     	movge	r8, r2
      74: b1a0000e     	movlt	r0, lr
      78: b1a06002     	movlt	r6, r2
      7c: e04c2000     	sub	r2, r12, r0
      80: e0433000     	sub	r3, r3, r0
      84: edd75a03     	vldr	s11, [r7, #12]
      88: ee073a90     	vmov	s15, r3
      8c: ee072a10     	vmov	s14, r2
      90: e5983000     	ldr	r3, [r8]
      94: e5972000     	ldr	r2, [r7]
      98: eef86ae7     	vcvt.f32.s32	s13, s15
      9c: eeb87ac7     	vcvt.f32.s32	s14, s14
      a0: e0433002     	sub	r3, r3, r2
      a4: eef82ae5     	vcvt.f32.s32	s5, s11
      a8: edd75a04     	vldr	s11, [r7, #16]
      ac: ee073a90     	vmov	s15, r3
      b0: e5963000     	ldr	r3, [r6]
      b4: eef87ae7     	vcvt.f32.s32	s15, s15
      b8: eeb83ae5     	vcvt.f32.s32	s6, s11
      bc: e0433002     	sub	r3, r3, r2
      c0: e5972008     	ldr	r2, [r7, #0x8]
      c4: eeb2aa00     	vmov.f32	s20, #8.000000e+00
      c8: ee053a10     	vmov	s10, r3
      cc: ee67ca27     	vmul.f32	s25, s14, s15
      d0: eeb85ac5     	vcvt.f32.s32	s10, s10
      d4: e5963008     	ldr	r3, [r6, #0x8]
      d8: ee55ca26     	vnmls.f32	s25, s10, s13
      dc: e0433002     	sub	r3, r3, r2
      e0: eef5ca40     	vcmp.f32	s25, #0
      e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
      e8: 1eb76a00     	vmovne.f32	s12, #1.000000e+00
      ec: 1ec6ca2c     	vdivne.f32	s25, s12, s25
      f0: ee062a10     	vmov	s12, r2
      f4: eeb86ac6     	vcvt.f32.s32	s12, s12
      f8: ee622a86     	vmul.f32	s5, s5, s12
      fc: ee233a06     	vmul.f32	s6, s6, s12
     100: ee063a10     	vmov	s12, r3
     104: e5983008     	ldr	r3, [r8, #0x8]
     108: eeb86ac6     	vcvt.f32.s32	s12, s12
     10c: e0433002     	sub	r3, r3, r2
     110: e5972014     	ldr	r2, [r7, #0x14]
     114: ee6c7aa7     	vmul.f32	s15, s25, s15
     118: ee053a90     	vmov	s11, r3
     11c: ee2c5a85     	vmul.f32	s10, s25, s10
     120: eef85ae5     	vcvt.f32.s32	s11, s11
     124: ee674a86     	vmul.f32	s9, s15, s12
     128: ee2c7a87     	vmul.f32	s14, s25, s14
     12c: ee6c6aa6     	vmul.f32	s13, s25, s13
     130: e5963014     	ldr	r3, [r6, #0x14]
     134: ee554a25     	vnmls.f32	s9, s10, s11
     138: ee673a25     	vmul.f32	s7, s14, s11
     13c: e0433002     	sub	r3, r3, r2
     140: eef5cac0     	vcmpe.f32	s25, #0
     144: eef1fa10     	vmrs	APSR_nzcv, fpscr
     148: ee563a86     	vnmls.f32	s7, s13, s12
     14c: eebd6ae4     	vcvt.s32.f32	s12, s9
     150: eef0da46     	vmov.f32	s27, s12
     154: ee063a10     	vmov	s12, r3
     158: e5983014     	ldr	r3, [r8, #0x14]
     15c: eeb86ac6     	vcvt.f32.s32	s12, s12
     160: e0433002     	sub	r3, r3, r2
     164: e5972018     	ldr	r2, [r7, #0x18]
     168: ee053a90     	vmov	s11, r3
     16c: ee674a86     	vmul.f32	s9, s15, s12
     170: eef85ae5     	vcvt.f32.s32	s11, s11
     174: e5963018     	ldr	r3, [r6, #0x18]
     178: ee554a25     	vnmls.f32	s9, s10, s11
     17c: e0433002     	sub	r3, r3, r2
     180: ee274a25     	vmul.f32	s8, s14, s11
     184: ee164a86     	vnmls.f32	s8, s13, s12
     188: eebd6ae4     	vcvt.s32.f32	s12, s9
     18c: eeb0ea46     	vmov.f32	s28, s12
     190: ee063a10     	vmov	s12, r3
     194: e5983018     	ldr	r3, [r8, #0x18]
     198: eeb86ac6     	vcvt.f32.s32	s12, s12
     19c: e0433002     	sub	r3, r3, r2
     1a0: e597201c     	ldr	r2, [r7, #0x1c]
     1a4: ee053a90     	vmov	s11, r3
     1a8: ee272a86     	vmul.f32	s4, s15, s12
     1ac: eef85ae5     	vcvt.f32.s32	s11, s11
     1b0: e596301c     	ldr	r3, [r6, #0x1c]
     1b4: ee152a25     	vnmls.f32	s4, s10, s11
     1b8: e0433002     	sub	r3, r3, r2
     1bc: ee674a25     	vmul.f32	s9, s14, s11
     1c0: ee564a86     	vnmls.f32	s9, s13, s12
     1c4: eebd6ac2     	vcvt.s32.f32	s12, s4
     1c8: eef0ea46     	vmov.f32	s29, s12
     1cc: ee063a10     	vmov	s12, r3
     1d0: e598301c     	ldr	r3, [r8, #0x1c]
     1d4: eeb86ac6     	vcvt.f32.s32	s12, s12
     1d8: edc72a08     	vstr	s5, [r7, #32]
     1dc: e0433002     	sub	r3, r3, r2
     1e0: ed873a09     	vstr	s6, [r7, #36]
     1e4: ee053a90     	vmov	s11, r3
     1e8: ee272a86     	vmul.f32	s4, s15, s12
     1ec: eef85ae5     	vcvt.f32.s32	s11, s11
     1f0: eef02a42     	vmov.f32	s5, s4
     1f4: ee273a25     	vmul.f32	s6, s14, s11
     1f8: ee552a25     	vnmls.f32	s5, s10, s11
     1fc: eef05a43     	vmov.f32	s11, s6
     200: ee565a86     	vnmls.f32	s11, s13, s12
     204: eebd6ae2     	vcvt.s32.f32	s12, s5
     208: eeb0fa46     	vmov.f32	s30, s12
     20c: eebd6ae5     	vcvt.s32.f32	s12, s11
     210: ed8d6a16     	vstr	s12, [sp, #88]
     214: ed966a02     	vldr	s12, [r6, #8]
     218: eef85ac6     	vcvt.f32.s32	s11, s12
     21c: ed966a03     	vldr	s12, [r6, #12]
     220: eeb86ac6     	vcvt.f32.s32	s12, s12
     224: ee266a25     	vmul.f32	s12, s12, s11
     228: ed866a08     	vstr	s12, [r6, #32]
     22c: ed966a04     	vldr	s12, [r6, #16]
     230: eeb86ac6     	vcvt.f32.s32	s12, s12
     234: ee266a25     	vmul.f32	s12, s12, s11
     238: ed866a09     	vstr	s12, [r6, #36]
     23c: ed986a02     	vldr	s12, [r8, #8]
     240: eeb83ac6     	vcvt.f32.s32	s6, s12
     244: ed986a03     	vldr	s12, [r8, #12]
     248: eef85ac6     	vcvt.f32.s32	s11, s12
     24c: ed986a04     	vldr	s12, [r8, #16]
     250: ee655a83     	vmul.f32	s11, s11, s6
     254: eeb86ac6     	vcvt.f32.s32	s12, s12
     258: edc85a08     	vstr	s11, [r8, #32]
     25c: ee266a03     	vmul.f32	s12, s12, s6
     260: ed886a09     	vstr	s12, [r8, #36]
     264: edd71a08     	vldr	s3, [r7, #32]
     268: ed972a09     	vldr	s4, [r7, #36]
     26c: edd62a08     	vldr	s5, [r6, #32]
     270: e59dc0ec     	ldr	r12, [sp, #0xec]
     274: ed963a09     	vldr	s6, [r6, #36]
     278: e5971004     	ldr	r1, [r7, #0x4]
     27c: e59c3020     	ldr	r3, [r12, #0x20]
     280: ee722ae1     	vsub.f32	s5, s5, s3
     284: ee333a42     	vsub.f32	s6, s6, s4
     288: e59c2004     	ldr	r2, [r12, #0x4]
     28c: e0030193     	mul	r3, r3, r1
     290: e59d0058     	ldr	r0, [sp, #0x58]
     294: ee27daa2     	vmul.f32	s26, s15, s5
     298: ee27ca83     	vmul.f32	s24, s15, s6
     29c: eefd7ae3     	vcvt.s32.f32	s15, s7
     2a0: e0822103     	add	r2, r2, r3, lsl #2
     2a4: e1a0b180     	lsl	r11, r0, #3
     2a8: e1a09080     	lsl	r9, r0, #1
     2ac: ee755ae1     	vsub.f32	s11, s11, s3
     2b0: e58d20c8     	str	r2, [sp, #0xc8]
     2b4: e59c2000     	ldr	r2, [r12]
     2b8: edcd7a2c     	vstr	s15, [sp, #176]
     2bc: eeb88ae7     	vcvt.f32.s32	s16, s15
     2c0: eefd7ac4     	vcvt.s32.f32	s15, s8
     2c4: e0823083     	add	r3, r2, r3, lsl #1
     2c8: e58db048     	str	r11, [sp, #0x48]
     2cc: e58d9124     	str	r9, [sp, #0x124]
     2d0: e0899000     	add	r9, r9, r0
     2d4: e58d30c4     	str	r3, [sp, #0xc4]
     2d8: edcd7a14     	vstr	s15, [sp, #80]
     2dc: e59d3050     	ldr	r3, [sp, #0x50]
     2e0: eefd7ae4     	vcvt.s32.f32	s15, s9
     2e4: e58d911c     	str	r9, [sp, #0x11c]
     2e8: ee366a42     	vsub.f32	s12, s12, s4
     2ec: e1a0e183     	lsl	lr, r3, #3
     2f0: e1a04083     	lsl	r4, r3, #1
     2f4: ee172a90     	vmov	r2, s15
     2f8: e58de044     	str	lr, [sp, #0x44]
     2fc: e1a05082     	lsl	r5, r2, #1
     300: e59de0b0     	ldr	lr, [sp, #0xb0]
     304: e1a0a182     	lsl	r10, r2, #3
     308: e58d412c     	str	r4, [sp, #0x12c]
     30c: e0844003     	add	r4, r4, r3
     310: e58d5128     	str	r5, [sp, #0x128]
     314: e0855002     	add	r5, r5, r2
     318: e58d5100     	str	r5, [sp, #0x100]
     31c: e1a0b08e     	lsl	r11, lr, #1
     320: e58d4120     	str	r4, [sp, #0x120]
     324: edcd7a15     	vstr	s15, [sp, #84]
     328: e58db04c     	str	r11, [sp, #0x4c]
     32c: e1a0b103     	lsl	r11, r3, #2
     330: e58da05c     	str	r10, [sp, #0x5c]
     334: e58db0fc     	str	r11, [sp, #0xfc]
     338: e1a0b102     	lsl	r11, r2, #2
     33c: ee679a25     	vmul.f32	s19, s14, s11
     340: e58db0f8     	str	r11, [sp, #0xf8]
     344: e1a0b100     	lsl	r11, r0, #2
     348: ee279a06     	vmul.f32	s18, s14, s12
     34c: e58db0f4     	str	r11, [sp, #0xf4]
     350: e59cb008     	ldr	r11, [r12, #0x8]
     354: e1a0c18e     	lsl	r12, lr, #3
     358: ee569aa2     	vnmls.f32	s19, s13, s5
     35c: e58dc060     	str	r12, [sp, #0x60]
     360: e08ec10e     	add	r12, lr, lr, lsl #2
     364: ee169a83     	vnmls.f32	s18, s13, s6
     368: e58dc0b4     	str	r12, [sp, #0xb4]
     36c: e1a0c084     	lsl	r12, r4, #1
     370: e58dc130     	str	r12, [sp, #0x130]
     374: e59d5100     	ldr	r5, [sp, #0x100]
     378: e59d404c     	ldr	r4, [sp, #0x4c]
     37c: e59dc044     	ldr	r12, [sp, #0x44]
     380: ee69aa8a     	vmul.f32	s21, s19, s20
     384: e1a05085     	lsl	r5, r5, #1
     388: ee29ba0a     	vmul.f32	s22, s18, s20
     38c: e04cc003     	sub	r12, r12, r3
     390: ee15da25     	vnmls.f32	s26, s10, s11
     394: e58d5134     	str	r5, [sp, #0x134]
     398: e1a05089     	lsl	r5, r9, #1
     39c: ee15ca06     	vnmls.f32	s24, s10, s12
     3a0: e58d5138     	str	r5, [sp, #0x138]
     3a4: e084500e     	add	r5, r4, lr
     3a8: e59d40fc     	ldr	r4, [sp, #0xfc]
     3ac: e58d501c     	str	r5, [sp, #0x1c]
     3b0: e0845003     	add	r5, r4, r3
     3b4: e59d40f8     	ldr	r4, [sp, #0xf8]
     3b8: e59d3048     	ldr	r3, [sp, #0x48]
     3bc: e58d5118     	str	r5, [sp, #0x118]
     3c0: e0845002     	add	r5, r4, r2
     3c4: e59d40f4     	ldr	r4, [sp, #0xf4]
     3c8: e04a2002     	sub	r2, r10, r2
     3cc: e0433000     	sub	r3, r3, r0
     3d0: ee28aa0a     	vmul.f32	s20, s16, s20
     3d4: e0844000     	add	r4, r4, r0
     3d8: e58d5114     	str	r5, [sp, #0x114]
     3dc: e58d4110     	str	r4, [sp, #0x110]
     3e0: e58dc10c     	str	r12, [sp, #0x10c]
     3e4: e58d2108     	str	r2, [sp, #0x108]
     3e8: e58d3104     	str	r3, [sp, #0x104]
     3ec: da000307     	ble	0x1010 <ZB_fillTriangleMappingPerspective_DT0_DW1+0x1010> @ imm = #0xc1c
     3f0: e1a02008     	mov	r2, r8
     3f4: e58d6178     	str	r6, [sp, #0x178]
     3f8: e5963004     	ldr	r3, [r6, #0x4]
     3fc: e3a09000     	mov	r9, #0
     400: e1a04007     	mov	r4, r7
     404: e58d90e8     	str	r9, [sp, #0xe8]
     408: e58db008     	str	r11, [sp, #0x8]
     40c: e1a0b009     	mov	r11, r9
     410: e0430001     	sub	r0, r3, r1
     414: e5923004     	ldr	r3, [r2, #0x4]
     418: e58d9164     	str	r9, [sp, #0x164]
     41c: e3a0c001     	mov	r12, #1
     420: e58d717c     	str	r7, [sp, #0x17c]
     424: e1a07006     	mov	r7, r6
     428: e0431001     	sub	r1, r3, r1
     42c: e1a03009     	mov	r3, r9
     430: e34037f8     	movt	r3, #0x7f8
     434: e1a09008     	mov	r9, r8
     438: e58d3018     	str	r3, [sp, #0x18]
     43c: e1a08000     	mov	r8, r0
     440: e3a03b3e     	mov	r3, #63488
     444: eef7ba00     	vmov.f32	s23, #1.000000e+00
     448: eef08a00     	vmov.f32	s17, #2.000000e+00
     44c: e3403007     	movt	r3, #0x7
     450: e58dc0b8     	str	r12, [sp, #0xb8]
     454: e58d300c     	str	r3, [sp, #0xc]
     458: e5940000     	ldr	r0, [r4]
     45c: e3510000     	cmp	r1, #0
     460: e1a05000     	mov	r5, r0
     464: da0002d0     	ble	0xfac <ZB_fillTriangleMappingPerspective_DT0_DW1+0xfac> @ imm = #0xb40
     468: e5923000     	ldr	r3, [r2]
     46c: e0433000     	sub	r3, r3, r0
     470: e1a00803     	lsl	r0, r3, #16
     474: ebfffffe     	bl	0x474 <ZB_fillTriangleMappingPerspective_DT0_DW1+0x474> @ imm = #-0x8
     478: e59d10b0     	ldr	r1, [sp, #0xb0]
     47c: e6ff2070     	uxth	r2, r0
     480: e1a03840     	asr	r3, r0, #16
     484: e58d20f0     	str	r2, [sp, #0xf0]
     488: ee1d2a90     	vmov	r2, s27
     48c: ee073a90     	vmov	s15, r3
     490: e0222193     	mla	r2, r3, r1, r2
     494: e59d1050     	ldr	r1, [sp, #0x50]
     498: eef86ae7     	vcvt.f32.s32	s13, s15
     49c: e58d3160     	str	r3, [sp, #0x160]
     4a0: e58d2150     	str	r2, [sp, #0x150]
     4a4: ee1e2a10     	vmov	r2, s28
     4a8: e0222193     	mla	r2, r3, r1, r2
     4ac: e59d1054     	ldr	r1, [sp, #0x54]
     4b0: e58d2154     	str	r2, [sp, #0x154]
     4b4: ee1e2a90     	vmov	r2, s29
     4b8: e0222193     	mla	r2, r3, r1, r2
     4bc: e59d1058     	ldr	r1, [sp, #0x58]
     4c0: e58d2158     	str	r2, [sp, #0x158]
     4c4: ee1f2a10     	vmov	r2, s30
     4c8: e0222193     	mla	r2, r3, r1, r2
     4cc: e2833001     	add	r3, r3, #1
     4d0: e58d314c     	str	r3, [sp, #0x14c]
     4d4: e58d215c     	str	r2, [sp, #0x15c]
     4d8: e59d30b8     	ldr	r3, [sp, #0xb8]
     4dc: ed947a08     	vldr	s14, [r4, #32]
     4e0: ed944a09     	vldr	s8, [r4, #36]
     4e4: e3530000     	cmp	r3, #0
     4e8: e5943008     	ldr	r3, [r4, #0x8]
     4ec: e1a0a003     	mov	r10, r3
     4f0: e5943014     	ldr	r3, [r4, #0x14]
     4f4: e58d302c     	str	r3, [sp, #0x2c]
     4f8: e5943018     	ldr	r3, [r4, #0x18]
     4fc: e58d3030     	str	r3, [sp, #0x30]
     500: e594301c     	ldr	r3, [r4, #0x1c]
     504: e58d3034     	str	r3, [sp, #0x34]
     508: 1a0002c3     	bne	0x101c <ZB_fillTriangleMappingPerspective_DT0_DW1+0x101c> @ imm = #0xb0c
     50c: e3580000     	cmp	r8, #0
     510: da000267     	ble	0xeb4 <ZB_fillTriangleMappingPerspective_DT0_DW1+0xeb4> @ imm = #0x99c
     514: e59d2150     	ldr	r2, [sp, #0x150]
     518: e59d30b0     	ldr	r3, [sp, #0xb0]
     51c: eeb02a4d     	vmov.f32	s4, s26
     520: eef02a4c     	vmov.f32	s5, s24
     524: e0833002     	add	r3, r3, r2
     528: e59d2154     	ldr	r2, [sp, #0x154]
     52c: e58d313c     	str	r3, [sp, #0x13c]
     530: e59d3050     	ldr	r3, [sp, #0x50]
     534: ee092aa6     	vmla.f32	s4, s19, s13
     538: ee492a26     	vmla.f32	s5, s18, s13
     53c: e0833002     	add	r3, r3, r2
     540: e59d2158     	ldr	r2, [sp, #0x158]
     544: e58d3140     	str	r3, [sp, #0x140]
     548: e59d3054     	ldr	r3, [sp, #0x54]
     54c: e58d80bc     	str	r8, [sp, #0xbc]
     550: ee393a82     	vadd.f32	s6, s19, s4
     554: e0833002     	add	r3, r3, r2
     558: e59d215c     	ldr	r2, [sp, #0x15c]
     55c: e58d3144     	str	r3, [sp, #0x144]
     560: e59d3058     	ldr	r3, [sp, #0x58]
     564: ee793a22     	vadd.f32	s7, s18, s5
     568: e58d8168     	str	r8, [sp, #0x168]
     56c: e1a08005     	mov	r8, r5
     570: e0833002     	add	r3, r3, r2
     574: e58d9170     	str	r9, [sp, #0x170]
     578: e58d3148     	str	r3, [sp, #0x148]
     57c: e1a0900a     	mov	r9, r10
     580: e59d3164     	ldr	r3, [sp, #0x164]
     584: e58db16c     	str	r11, [sp, #0x16c]
     588: e58d7174     	str	r7, [sp, #0x174]
     58c: e58d30c0     	str	r3, [sp, #0xc0]
     590: e59d30bc     	ldr	r3, [sp, #0xbc]
     594: e1a00009     	mov	r0, r9
     598: ee079a90     	vmov	s15, r9
     59c: eef80ae7     	vcvt.f32.s32	s1, s15
     5a0: e2433001     	sub	r3, r3, #1
     5a4: e58d30bc     	str	r3, [sp, #0xbc]
     5a8: e59d30c0     	ldr	r3, [sp, #0xc0]
     5ac: eecb7aa0     	vdiv.f32	s15, s23, s1
     5b0: e0685843     	rsb	r5, r8, r3, asr #16
     5b4: e59d30c8     	ldr	r3, [sp, #0xc8]
     5b8: e3550006     	cmp	r5, #6
     5bc: e0837108     	add	r7, r3, r8, lsl #2
     5c0: e59d30c4     	ldr	r3, [sp, #0xc4]
     5c4: e0831088     	add	r1, r3, r8, lsl #1
     5c8: da000251     	ble	0xf14 <ZB_fillTriangleMappingPerspective_DT0_DW1+0xf14> @ imm = #0x944
     5cc: e2453007     	sub	r3, r5, #7
     5d0: e59d212c     	ldr	r2, [sp, #0x12c]
     5d4: e1a041a3     	lsr	r4, r3, #3
     5d8: e2816010     	add	r6, r1, #16
     5dc: e2811020     	add	r1, r1, #32
     5e0: e59dc124     	ldr	r12, [sp, #0x124]
     5e4: e1a00204     	lsl	r0, r4, #4
     5e8: eeb05a44     	vmov.f32	s10, s8
     5ec: eef04a47     	vmov.f32	s9, s14
     5f0: e1a0e006     	mov	lr, r6
     5f4: e0813000     	add	r3, r1, r0
     5f8: e59d1128     	ldr	r1, [sp, #0x128]
     5fc: e58d3064     	str	r3, [sp, #0x64]
     600: e2877020     	add	r7, r7, #32
     604: e59d30b0     	ldr	r3, [sp, #0xb0]
     608: e3a0a000     	mov	r10, #0
     60c: e089b003     	add	r11, r9, r3
     610: e59d304c     	ldr	r3, [sp, #0x4c]
     614: e0893003     	add	r3, r9, r3
     618: e58d3020     	str	r3, [sp, #0x20]
     61c: e59d302c     	ldr	r3, [sp, #0x2c]
     620: e0832002     	add	r2, r3, r2
     624: e58d20ac     	str	r2, [sp, #0xac]
     628: e59d2030     	ldr	r2, [sp, #0x30]
     62c: e0821001     	add	r1, r2, r1
     630: e58d10a8     	str	r1, [sp, #0xa8]
     634: e59d1034     	ldr	r1, [sp, #0x34]
     638: e081c00c     	add	r12, r1, r12
     63c: e58dc0a4     	str	r12, [sp, #0xa4]
     640: e59dc120     	ldr	r12, [sp, #0x120]
     644: e08cc003     	add	r12, r12, r3
     648: e58dc0a0     	str	r12, [sp, #0xa0]
     64c: e59dc100     	ldr	r12, [sp, #0x100]
     650: e08cc002     	add	r12, r12, r2
     654: e58dc09c     	str	r12, [sp, #0x9c]
     658: e59dc11c     	ldr	r12, [sp, #0x11c]
     65c: e08cc001     	add	r12, r12, r1
     660: e58dc098     	str	r12, [sp, #0x98]
     664: e59dc0fc     	ldr	r12, [sp, #0xfc]
     668: e08cc003     	add	r12, r12, r3
     66c: e58dc094     	str	r12, [sp, #0x94]
     670: e59dc0f8     	ldr	r12, [sp, #0xf8]
     674: e08cc002     	add	r12, r12, r2
     678: e58dc090     	str	r12, [sp, #0x90]
     67c: e59dc0f4     	ldr	r12, [sp, #0xf4]
     680: e08cc001     	add	r12, r12, r1
     684: e58dc08c     	str	r12, [sp, #0x8c]
     688: e59dc118     	ldr	r12, [sp, #0x118]
     68c: e08cc003     	add	r12, r12, r3
     690: e58dc088     	str	r12, [sp, #0x88]
     694: e59dc114     	ldr	r12, [sp, #0x114]
     698: e08cc002     	add	r12, r12, r2
     69c: e58dc084     	str	r12, [sp, #0x84]
     6a0: e59dc110     	ldr	r12, [sp, #0x110]
     6a4: e08cc001     	add	r12, r12, r1
     6a8: e58dc080     	str	r12, [sp, #0x80]
     6ac: e59dc130     	ldr	r12, [sp, #0x130]
     6b0: e08cc003     	add	r12, r12, r3
     6b4: e58dc07c     	str	r12, [sp, #0x7c]
     6b8: e59dc134     	ldr	r12, [sp, #0x134]
     6bc: e58d7004     	str	r7, [sp, #0x4]
     6c0: e58d9014     	str	r9, [sp, #0x14]
     6c4: e08cc002     	add	r12, r12, r2
     6c8: e58da000     	str	r10, [sp]
     6cc: e58dc078     	str	r12, [sp, #0x78]
     6d0: e59dc138     	ldr	r12, [sp, #0x138]
     6d4: e58da010     	str	r10, [sp, #0x10]
     6d8: e58d80cc     	str	r8, [sp, #0xcc]
     6dc: e08cc001     	add	r12, r12, r1
     6e0: e58d90d0     	str	r9, [sp, #0xd0]
     6e4: e58dc074     	str	r12, [sp, #0x74]
     6e8: e59dc10c     	ldr	r12, [sp, #0x10c]
     6ec: e58d50d4     	str	r5, [sp, #0xd4]
     6f0: e58d00d8     	str	r0, [sp, #0xd8]
     6f4: e08cc003     	add	r12, r12, r3
     6f8: e58d40dc     	str	r4, [sp, #0xdc]
     6fc: e58dc070     	str	r12, [sp, #0x70]
     700: e59dc108     	ldr	r12, [sp, #0x108]
     704: e58db024     	str	r11, [sp, #0x24]
     708: e58d70e0     	str	r7, [sp, #0xe0]
     70c: e08cc002     	add	r12, r12, r2
     710: e58d60e4     	str	r6, [sp, #0xe4]
     714: e58dc06c     	str	r12, [sp, #0x6c]
     718: e59dc104     	ldr	r12, [sp, #0x104]
     71c: e08cc001     	add	r12, r12, r1
     720: e58dc068     	str	r12, [sp, #0x68]
     724: e59dc050     	ldr	r12, [sp, #0x50]
     728: e08c3003     	add	r3, r12, r3
     72c: e1a0c009     	mov	r12, r9
     730: e58d3038     	str	r3, [sp, #0x38]
     734: e59d3054     	ldr	r3, [sp, #0x54]
     738: e0833002     	add	r3, r3, r2
     73c: e58d303c     	str	r3, [sp, #0x3c]
     740: e59d3058     	ldr	r3, [sp, #0x58]
     744: e0833001     	add	r3, r3, r1
     748: e58d3040     	str	r3, [sp, #0x40]
     74c: ee240aa7     	vmul.f32	s0, s9, s15
     750: e28ee010     	add	lr, lr, #16
     754: ee651a27     	vmul.f32	s3, s10, s15
     758: ee700a8a     	vadd.f32	s1, s1, s20
     75c: eef05a69     	vmov.f32	s11, s19
     760: eeb06a49     	vmov.f32	s12, s18
     764: eeb01a68     	vmov.f32	s2, s17
     768: ee086a61     	vmls.f32	s12, s16, s3
     76c: ee001ae7     	vmls.f32	s2, s1, s15
     770: ee485a40     	vmls.f32	s11, s16, s0
     774: e59d102c     	ldr	r1, [sp, #0x2c]
     778: e59db000     	ldr	r11, [sp]
     77c: ee266a27     	vmul.f32	s12, s12, s15
     780: e59d8010     	ldr	r8, [sp, #0x10]
     784: ee655aa7     	vmul.f32	s11, s11, s15
     788: e081b00b     	add	r11, r1, r11
     78c: ee617a27     	vmul.f32	s15, s2, s15
     790: eeb01a68     	vmov.f32	s2, s17
     794: e59d1030     	ldr	r1, [sp, #0x30]
     798: eebd6ac6     	vcvt.s32.f32	s12, s12
     79c: ee001ae7     	vmls.f32	s2, s1, s15
     7a0: eefd1ae1     	vcvt.s32.f32	s3, s3
     7a4: e0819008     	add	r9, r1, r8
     7a8: e59d1034     	ldr	r1, [sp, #0x34]
     7ac: e59d7018     	ldr	r7, [sp, #0x18]
     7b0: e59d401c     	ldr	r4, [sp, #0x1c]
     7b4: e081800a     	add	r8, r1, r10
     7b8: e59d600c     	ldr	r6, [sp, #0xc]
     7bc: eefd5ae5     	vcvt.s32.f32	s11, s11
     7c0: ee617a27     	vmul.f32	s15, s2, s15
     7c4: eebd1ac0     	vcvt.s32.f32	s2, s0
     7c8: ee113a90     	vmov	r3, s3
     7cc: ee161a10     	vmov	r1, s12
     7d0: e0035007     	and	r5, r3, r7
     7d4: e1a0772c     	lsr	r7, r12, #14
     7d8: e0813003     	add	r3, r1, r3
     7dc: ee112a10     	vmov	r2, s2
     7e0: e08cc004     	add	r12, r12, r4
     7e4: e59d1008     	ldr	r1, [sp, #0x8]
     7e8: e0026006     	and	r6, r2, r6
     7ec: e1855006     	orr	r5, r5, r6
     7f0: e58dc028     	str	r12, [sp, #0x28]
     7f4: e59d600c     	ldr	r6, [sp, #0xc]
     7f8: e59dc018     	ldr	r12, [sp, #0x18]
     7fc: ee150a90     	vmov	r0, s11
     800: e79154a5     	ldr	r5, [r1, r5, lsr #9]
     804: e0802002     	add	r2, r0, r2
     808: e003c00c     	and	r12, r3, r12
     80c: e0026006     	and	r6, r2, r6
     810: e186600c     	orr	r6, r6, r12
     814: ee16ca10     	vmov	r12, s12
     818: e08c3003     	add	r3, r12, r3
     81c: e7e7c855     	ubfx	r12, r5, #0x10, #0x8
     820: e00c0c9b     	mul	r12, r11, r12
     824: e7e7b455     	ubfx	r11, r5, #0x8, #0x8
     828: e6ef5075     	uxtb	r5, r5
     82c: e0802002     	add	r2, r0, r2
     830: e00b0b99     	mul	r11, r9, r11
     834: ed8d6a05     	vstr	s12, [sp, #20]
     838: ee744aaa     	vadd.f32	s9, s9, s21
     83c: e0050598     	mul	r5, r8, r5
     840: e1a0c42c     	lsr	r12, r12, #8
     844: e59d8010     	ldr	r8, [sp, #0x10]
     848: e1a0b82b     	lsr	r11, r11, #16
     84c: e20cc8ff     	and	r12, r12, #16711680
     850: ee355a0b     	vadd.f32	s10, s10, s22
     854: e20bbcff     	and	r11, r11, #65280
     858: e18cc00b     	orr	r12, r12, r11
     85c: e59db000     	ldr	r11, [sp]
     860: e18ccc25     	orr	r12, r12, r5, lsr #24
     864: e59d5004     	ldr	r5, [sp, #0x4]
     868: e505c020     	str	r12, [r5, #-0x20]
     86c: e14e72b0     	strh	r7, [lr, #-32]
     870: e79154a6     	ldr	r5, [r1, r6, lsr #9]
     874: e59d1038     	ldr	r1, [sp, #0x38]
     878: e7e7c855     	ubfx	r12, r5, #0x10, #0x8
     87c: e081900b     	add	r9, r1, r11
     880: e59d103c     	ldr	r1, [sp, #0x3c]
     884: e00c0c99     	mul	r12, r9, r12
     888: e7e79455     	ubfx	r9, r5, #0x8, #0x8
     88c: e6ef5075     	uxtb	r5, r5
     890: e0818008     	add	r8, r1, r8
     894: e59d1040     	ldr	r1, [sp, #0x40]
     898: e0090998     	mul	r9, r8, r9
     89c: e1a0c42c     	lsr	r12, r12, #8
     8a0: e081700a     	add	r7, r1, r10
     8a4: e59d1024     	ldr	r1, [sp, #0x24]
     8a8: e0050795     	mul	r5, r5, r7
     8ac: e20cc8ff     	and	r12, r12, #16711680
     8b0: e1a09829     	lsr	r9, r9, #16
     8b4: e2099cff     	and	r9, r9, #65280
     8b8: e1a06721     	lsr	r6, r1, #14
     8bc: e18cc009     	orr	r12, r12, r9
     8c0: e59d9008     	ldr	r9, [sp, #0x8]
     8c4: e18ccc25     	orr	r12, r12, r5, lsr #24
     8c8: e59d5004     	ldr	r5, [sp, #0x4]
     8cc: e0844001     	add	r4, r4, r1
     8d0: ee161a10     	vmov	r1, s12
     8d4: e505c01c     	str	r12, [r5, #-0x1c]
     8d8: e14e61be     	strh	r6, [lr, #-30]
     8dc: e59dc018     	ldr	r12, [sp, #0x18]
     8e0: e59d600c     	ldr	r6, [sp, #0xc]
     8e4: e003c00c     	and	r12, r3, r12
     8e8: e0813003     	add	r3, r1, r3
     8ec: e0025006     	and	r5, r2, r6
     8f0: e59d10ac     	ldr	r1, [sp, #0xac]
     8f4: e185c00c     	orr	r12, r5, r12
     8f8: e0802002     	add	r2, r0, r2
     8fc: e79954ac     	ldr	r5, [r9, r12, lsr #9]
     900: e0026006     	and	r6, r2, r6
     904: e081b00b     	add	r11, r1, r11
     908: e59d10a8     	ldr	r1, [sp, #0xa8]
     90c: e59d8010     	ldr	r8, [sp, #0x10]
     910: e0802002     	add	r2, r0, r2
     914: e7e7c855     	ubfx	r12, r5, #0x10, #0x8
     918: e59d7020     	ldr	r7, [sp, #0x20]
     91c: e00c0c9b     	mul	r12, r11, r12
     920: e7e7b455     	ubfx	r11, r5, #0x8, #0x8
     924: e0819008     	add	r9, r1, r8
     928: e59d10a4     	ldr	r1, [sp, #0xa4]
     92c: e00b0b99     	mul	r11, r9, r11
     930: e6ef5075     	uxtb	r5, r5
     934: e59d9008     	ldr	r9, [sp, #0x8]
     938: e1a07727     	lsr	r7, r7, #14
     93c: e1a0c42c     	lsr	r12, r12, #8
     940: e081800a     	add	r8, r1, r10
     944: ee161a10     	vmov	r1, s12
     948: e0050598     	mul	r5, r8, r5
     94c: e1a0b82b     	lsr	r11, r11, #16
     950: e20cc8ff     	and	r12, r12, #16711680
     954: e59d8010     	ldr	r8, [sp, #0x10]
     958: e20bbcff     	and	r11, r11, #65280
     95c: e18cc00b     	orr	r12, r12, r11
     960: e59db000     	ldr	r11, [sp]
     964: e18ccc25     	orr	r12, r12, r5, lsr #24
     968: e59d5004     	ldr	r5, [sp, #0x4]
     96c: e505c018     	str	r12, [r5, #-0x18]
     970: e59dc018     	ldr	r12, [sp, #0x18]
     974: e14e71bc     	strh	r7, [lr, #-28]
     978: e003c00c     	and	r12, r3, r12
     97c: e0813003     	add	r3, r1, r3
     980: e59d10a0     	ldr	r1, [sp, #0xa0]
     984: e186c00c     	orr	r12, r6, r12
     988: e79954ac     	ldr	r5, [r9, r12, lsr #9]
     98c: e59dc028     	ldr	r12, [sp, #0x28]
     990: e081900b     	add	r9, r1, r11
     994: e59d109c     	ldr	r1, [sp, #0x9c]
     998: e0818008     	add	r8, r1, r8
     99c: e59d1098     	ldr	r1, [sp, #0x98]
     9a0: e1a0672c     	lsr	r6, r12, #14
     9a4: e081700a     	add	r7, r1, r10
     9a8: e59d104c     	ldr	r1, [sp, #0x4c]
     9ac: e08cc001     	add	r12, r12, r1
     9b0: e58dc024     	str	r12, [sp, #0x24]
     9b4: e7e7c855     	ubfx	r12, r5, #0x10, #0x8
     9b8: e00c0c99     	mul	r12, r9, r12
     9bc: e7e79455     	ubfx	r9, r5, #0x8, #0x8
     9c0: e6ef5075     	uxtb	r5, r5
     9c4: e0090998     	mul	r9, r8, r9
     9c8: e59d8010     	ldr	r8, [sp, #0x10]
     9cc: e0050597     	mul	r5, r7, r5
     9d0: e1a0c42c     	lsr	r12, r12, #8
     9d4: e59d7018     	ldr	r7, [sp, #0x18]
     9d8: e1a09829     	lsr	r9, r9, #16
     9dc: e20cc8ff     	and	r12, r12, #16711680
     9e0: e2099cff     	and	r9, r9, #65280
     9e4: e18cc009     	orr	r12, r12, r9
     9e8: e59d9008     	ldr	r9, [sp, #0x8]
     9ec: e18ccc25     	orr	r12, r12, r5, lsr #24
     9f0: e59d5004     	ldr	r5, [sp, #0x4]
     9f4: e505c014     	str	r12, [r5, #-0x14]
     9f8: e003c007     	and	r12, r3, r7
     9fc: e59d500c     	ldr	r5, [sp, #0xc]
     a00: e1a07724     	lsr	r7, r4, #14
     a04: e14e61ba     	strh	r6, [lr, #-26]
     a08: ee166a10     	vmov	r6, s12
     a0c: e0025005     	and	r5, r2, r5
     a10: e0863003     	add	r3, r6, r3
     a14: e59d6094     	ldr	r6, [sp, #0x94]
     a18: e185c00c     	orr	r12, r5, r12
     a1c: e79954ac     	ldr	r5, [r9, r12, lsr #9]
     a20: e0802002     	add	r2, r0, r2
     a24: e086b00b     	add	r11, r6, r11
     a28: e59d6090     	ldr	r6, [sp, #0x90]
     a2c: e7e7c855     	ubfx	r12, r5, #0x10, #0x8
     a30: e00c0c9b     	mul	r12, r11, r12
     a34: e7e7b455     	ubfx	r11, r5, #0x8, #0x8
     a38: e0869008     	add	r9, r6, r8
     a3c: e59d608c     	ldr	r6, [sp, #0x8c]
     a40: e00b0b99     	mul	r11, r9, r11
     a44: e6ef5075     	uxtb	r5, r5
     a48: e086800a     	add	r8, r6, r10
     a4c: e1a0c42c     	lsr	r12, r12, #8
     a50: e0050598     	mul	r5, r8, r5
     a54: e0846001     	add	r6, r4, r1
     a58: e1a0b82b     	lsr	r11, r11, #16
     a5c: e20cc8ff     	and	r12, r12, #16711680
     a60: ee161a10     	vmov	r1, s12
     a64: e20bbcff     	and	r11, r11, #65280
     a68: e18cc00b     	orr	r12, r12, r11
     a6c: e18ccc25     	orr	r12, r12, r5, lsr #24
     a70: e59d5004     	ldr	r5, [sp, #0x4]
     a74: e505c010     	str	r12, [r5, #-0x10]
     a78: e14e71b8     	strh	r7, [lr, #-24]
     a7c: e59d500c     	ldr	r5, [sp, #0xc]
     a80: e59d7018     	ldr	r7, [sp, #0x18]
     a84: e59d9008     	ldr	r9, [sp, #0x8]
     a88: e002c005     	and	r12, r2, r5
     a8c: e0802002     	add	r2, r0, r2
     a90: e0800002     	add	r0, r0, r2
     a94: e59db000     	ldr	r11, [sp]
     a98: e59d4088     	ldr	r4, [sp, #0x88]
     a9c: e0022005     	and	r2, r2, r5
     aa0: e0000005     	and	r0, r0, r5
     aa4: e0035007     	and	r5, r3, r7
     aa8: e18cc005     	orr	r12, r12, r5
     aac: e0813003     	add	r3, r1, r3
     ab0: e799c4ac     	ldr	r12, [r9, r12, lsr #9]
     ab4: e0811003     	add	r1, r1, r3
     ab8: e0033007     	and	r3, r3, r7
     abc: e59d8010     	ldr	r8, [sp, #0x10]
     ac0: e1823003     	orr	r3, r2, r3
     ac4: e084200b     	add	r2, r4, r11
     ac8: e59d4084     	ldr	r4, [sp, #0x84]
     acc: e0011007     	and	r1, r1, r7
     ad0: e1801001     	orr	r1, r0, r1
     ad4: e7e7085c     	ubfx	r0, r12, #0x10, #0x8
     ad8: e0000092     	mul	r0, r2, r0
     adc: e7e7245c     	ubfx	r2, r12, #0x8, #0x8
     ae0: e0845008     	add	r5, r4, r8
     ae4: e59d4080     	ldr	r4, [sp, #0x80]
     ae8: e0020295     	mul	r2, r5, r2
     aec: e6efc07c     	uxtb	r12, r12
     af0: e59d5004     	ldr	r5, [sp, #0x4]
     af4: e1a00420     	lsr	r0, r0, #8
     af8: e1a02822     	lsr	r2, r2, #16
     afc: e20008ff     	and	r0, r0, #16711680
     b00: e2022cff     	and	r2, r2, #65280
     b04: e1802002     	orr	r2, r0, r2
     b08: e084000a     	add	r0, r4, r10
     b0c: e00c0c90     	mul	r12, r0, r12
     b10: e59d0024     	ldr	r0, [sp, #0x24]
     b14: e59d401c     	ldr	r4, [sp, #0x1c]
     b18: e1822c2c     	orr	r2, r2, r12, lsr #24
     b1c: e080c004     	add	r12, r0, r4
     b20: e505200c     	str	r2, [r5, #-0xc]
     b24: e1a02720     	lsr	r2, r0, #14
     b28: e59d007c     	ldr	r0, [sp, #0x7c]
     b2c: e14e21b6     	strh	r2, [lr, #-22]
     b30: e79924a3     	ldr	r2, [r9, r3, lsr #9]
     b34: e080700b     	add	r7, r0, r11
     b38: e59d0078     	ldr	r0, [sp, #0x78]
     b3c: e7e73852     	ubfx	r3, r2, #0x10, #0x8
     b40: e0805008     	add	r5, r0, r8
     b44: e0030397     	mul	r3, r7, r3
     b48: e7e77452     	ubfx	r7, r2, #0x8, #0x8
     b4c: e59d0074     	ldr	r0, [sp, #0x74]
     b50: e0070795     	mul	r7, r5, r7
     b54: e6ef2072     	uxtb	r2, r2
     b58: e59d5004     	ldr	r5, [sp, #0x4]
     b5c: e080000a     	add	r0, r0, r10
     b60: e1a03423     	lsr	r3, r3, #8
     b64: e0020290     	mul	r2, r0, r2
     b68: e1a07827     	lsr	r7, r7, #16
     b6c: e20338ff     	and	r3, r3, #16711680
     b70: e2077cff     	and	r7, r7, #65280
     b74: e1833007     	orr	r3, r3, r7
     b78: e1833c22     	orr	r3, r3, r2, lsr #24
     b7c: e5053008     	str	r3, [r5, #-0x8]
     b80: e1a03726     	lsr	r3, r6, #14
     b84: e14e31b4     	strh	r3, [lr, #-20]
     b88: e0863004     	add	r3, r6, r4
     b8c: e79924a1     	ldr	r2, [r9, r1, lsr #9]
     b90: e59d1044     	ldr	r1, [sp, #0x44]
     b94: e58d3024     	str	r3, [sp, #0x24]
     b98: e59d3070     	ldr	r3, [sp, #0x70]
     b9c: e08b1001     	add	r1, r11, r1
     ba0: e58d1000     	str	r1, [sp]
     ba4: e59d106c     	ldr	r1, [sp, #0x6c]
     ba8: e083300b     	add	r3, r3, r11
     bac: e0810008     	add	r0, r1, r8
     bb0: e59d105c     	ldr	r1, [sp, #0x5c]
     bb4: e0881001     	add	r1, r8, r1
     bb8: e58d1010     	str	r1, [sp, #0x10]
     bbc: e7e71852     	ubfx	r1, r2, #0x10, #0x8
     bc0: e0010193     	mul	r1, r3, r1
     bc4: e7e73452     	ubfx	r3, r2, #0x8, #0x8
     bc8: e6ef2072     	uxtb	r2, r2
     bcc: e0030390     	mul	r3, r0, r3
     bd0: e1a01421     	lsr	r1, r1, #8
     bd4: e1a03823     	lsr	r3, r3, #16
     bd8: e20118ff     	and	r1, r1, #16711680
     bdc: e2033cff     	and	r3, r3, #65280
     be0: e1813003     	orr	r3, r1, r3
     be4: e59d1068     	ldr	r1, [sp, #0x68]
     be8: e081100a     	add	r1, r1, r10
     bec: e0020291     	mul	r2, r1, r2
     bf0: e1833c22     	orr	r3, r3, r2, lsr #24
     bf4: e5053004     	str	r3, [r5, #-0x4]
     bf8: e59d3048     	ldr	r3, [sp, #0x48]
     bfc: e08aa003     	add	r10, r10, r3
     c00: e2853020     	add	r3, r5, #32
     c04: e58d3004     	str	r3, [sp, #0x4]
     c08: e59d2020     	ldr	r2, [sp, #0x20]
     c0c: e59d30b4     	ldr	r3, [sp, #0xb4]
     c10: e0833002     	add	r3, r3, r2
     c14: e1a03723     	lsr	r3, r3, #14
     c18: e14e31b2     	strh	r3, [lr, #-18]
     c1c: e59d3064     	ldr	r3, [sp, #0x64]
     c20: e153000e     	cmp	r3, lr
     c24: e59d3060     	ldr	r3, [sp, #0x60]
     c28: e0823003     	add	r3, r2, r3
     c2c: e58d3020     	str	r3, [sp, #0x20]
     c30: 1afffec5     	bne	0x74c <ZB_fillTriangleMappingPerspective_DT0_DW1+0x74c> @ imm = #-0x4ec
     c34: e28d00d8     	add	r0, sp, #216
     c38: e59de058     	ldr	lr, [sp, #0x58]
     c3c: e8900091     	ldm	r0, {r0, r4, r7}
     c40: e59d3040     	ldr	r3, [sp, #0x40]
     c44: e59d50d4     	ldr	r5, [sp, #0xd4]
     c48: e59d60e4     	ldr	r6, [sp, #0xe4]
     c4c: e59d2038     	ldr	r2, [sp, #0x38]
     c50: e0877284     	add	r7, r7, r4, lsl #5
     c54: e245c008     	sub	r12, r5, #8
     c58: e083310e     	add	r3, r3, lr, lsl #2
     c5c: e04c5184     	sub	r5, r12, r4, lsl #3
     c60: e59dc048     	ldr	r12, [sp, #0x48]
     c64: e083308e     	add	r3, r3, lr, lsl #1
     c68: e0801006     	add	r1, r0, r6
     c6c: e59d6050     	ldr	r6, [sp, #0x50]
     c70: e083300e     	add	r3, r3, lr
     c74: e59da054     	ldr	r10, [sp, #0x54]
     c78: e0233c94     	mla	r3, r4, r12, r3
     c7c: e59d90d0     	ldr	r9, [sp, #0xd0]
     c80: e59d80cc     	ldr	r8, [sp, #0xcc]
     c84: e0820106     	add	r0, r2, r6, lsl #2
     c88: e59d203c     	ldr	r2, [sp, #0x3c]
     c8c: e58d3010     	str	r3, [sp, #0x10]
     c90: e0800086     	add	r0, r0, r6, lsl #1
     c94: e59d3044     	ldr	r3, [sp, #0x44]
     c98: e0800006     	add	r0, r0, r6
     c9c: e082210a     	add	r2, r2, r10, lsl #2
     ca0: e082208a     	add	r2, r2, r10, lsl #1
     ca4: e0230394     	mla	r3, r4, r3, r0
     ca8: e082200a     	add	r2, r2, r10
     cac: e58d3000     	str	r3, [sp]
     cb0: e59d305c     	ldr	r3, [sp, #0x5c]
     cb4: e0232394     	mla	r3, r4, r3, r2
     cb8: e58d3004     	str	r3, [sp, #0x4]
     cbc: e59d3060     	ldr	r3, [sp, #0x60]
     cc0: e0890003     	add	r0, r9, r3
     cc4: e0200493     	mla	r0, r3, r4, r0
     cc8: e3550000     	cmp	r5, #0
     ccc: ba000044     	blt	0xde4 <ZB_fillTriangleMappingPerspective_DT0_DW1+0xde4> @ imm = #0x110
     cd0: ee644aa7     	vmul.f32	s9, s9, s15
     cd4: e2853001     	add	r3, r5, #1
     cd8: ee255a27     	vmul.f32	s10, s10, s15
     cdc: e3a04000     	mov	r4, #0
     ce0: eef05a69     	vmov.f32	s11, s19
     ce4: e0813083     	add	r3, r1, r3, lsl #1
     ce8: eeb06a49     	vmov.f32	s12, s18
     cec: e2477004     	sub	r7, r7, #4
     cf0: ee485a64     	vmls.f32	s11, s16, s9
     cf4: e1a06004     	mov	r6, r4
     cf8: ee086a45     	vmls.f32	s12, s16, s10
     cfc: e1a05004     	mov	r5, r4
     d00: e58d3014     	str	r3, [sp, #0x14]
     d04: e58d8024     	str	r8, [sp, #0x24]
     d08: e58d9028     	str	r9, [sp, #0x28]
     d0c: ee655aa7     	vmul.f32	s11, s11, s15
     d10: ee667a27     	vmul.f32	s15, s12, s15
     d14: eebd6ae4     	vcvt.s32.f32	s12, s9
     d18: eefd7ae7     	vcvt.s32.f32	s15, s15
     d1c: ee16ea10     	vmov	lr, s12
     d20: eebd6ac5     	vcvt.s32.f32	s12, s10
     d24: ee17aa90     	vmov	r10, s15
     d28: ee16ca10     	vmov	r12, s12
     d2c: eebd6ae5     	vcvt.s32.f32	s12, s11
     d30: ed8d6a08     	vstr	s12, [sp, #32]
     d34: e59d2020     	ldr	r2, [sp, #0x20]
     d38: e59d300c     	ldr	r3, [sp, #0xc]
     d3c: e00e3003     	and	r3, lr, r3
     d40: e08ee002     	add	lr, lr, r2
     d44: e59d2018     	ldr	r2, [sp, #0x18]
     d48: e00c2002     	and	r2, r12, r2
     d4c: e08cc00a     	add	r12, r12, r10
     d50: e1833002     	orr	r3, r3, r2
     d54: e59d2008     	ldr	r2, [sp, #0x8]
     d58: e79224a3     	ldr	r2, [r2, r3, lsr #9]
     d5c: e59d3000     	ldr	r3, [sp]
     d60: e083b006     	add	r11, r3, r6
     d64: e59d3050     	ldr	r3, [sp, #0x50]
     d68: e0866003     	add	r6, r6, r3
     d6c: e59d3004     	ldr	r3, [sp, #0x4]
     d70: e0839005     	add	r9, r3, r5
     d74: e59d3054     	ldr	r3, [sp, #0x54]
     d78: e0855003     	add	r5, r5, r3
     d7c: e59d3010     	ldr	r3, [sp, #0x10]
     d80: e0838004     	add	r8, r3, r4
     d84: e59d3058     	ldr	r3, [sp, #0x58]
     d88: e0844003     	add	r4, r4, r3
     d8c: e7e73852     	ubfx	r3, r2, #0x10, #0x8
     d90: e003039b     	mul	r3, r11, r3
     d94: e7e7b452     	ubfx	r11, r2, #0x8, #0x8
     d98: e6ef2072     	uxtb	r2, r2
     d9c: e00b0b99     	mul	r11, r9, r11
     da0: e0020298     	mul	r2, r8, r2
     da4: e1a03423     	lsr	r3, r3, #8
     da8: e1a0b82b     	lsr	r11, r11, #16
     dac: e20338ff     	and	r3, r3, #16711680
     db0: e20bbcff     	and	r11, r11, #65280
     db4: e183300b     	orr	r3, r3, r11
     db8: e1833c22     	orr	r3, r3, r2, lsr #24
     dbc: e5a73004     	str	r3, [r7, #0x4]!
     dc0: e1a03720     	lsr	r3, r0, #14
     dc4: e0c130b2     	strh	r3, [r1], #2
     dc8: e59d3014     	ldr	r3, [sp, #0x14]
     dcc: e1510003     	cmp	r1, r3
     dd0: e59d30b0     	ldr	r3, [sp, #0xb0]
     dd4: e0800003     	add	r0, r0, r3
     dd8: 1affffd5     	bne	0xd34 <ZB_fillTriangleMappingPerspective_DT0_DW1+0xd34> @ imm = #-0xac
     ddc: e59d8024     	ldr	r8, [sp, #0x24]
     de0: e59d9028     	ldr	r9, [sp, #0x28]
     de4: e59d30b8     	ldr	r3, [sp, #0xb8]
     de8: e59d20f0     	ldr	r2, [sp, #0xf0]
     dec: e0833002     	add	r3, r3, r2
     df0: e3530000     	cmp	r3, #0
     df4: e58d30b8     	str	r3, [sp, #0xb8]
     df8: da000032     	ble	0xec8 <ZB_fillTriangleMappingPerspective_DT0_DW1+0xec8> @ imm = #0xc8
     dfc: e59d30b8     	ldr	r3, [sp, #0xb8]
     e00: e59d2140     	ldr	r2, [sp, #0x140]
     e04: ee377a03     	vadd.f32	s14, s14, s6
     e08: e2433801     	sub	r3, r3, #65536
     e0c: ee344a23     	vadd.f32	s8, s8, s7
     e10: e58d30b8     	str	r3, [sp, #0xb8]
     e14: e59d314c     	ldr	r3, [sp, #0x14c]
     e18: e0888003     	add	r8, r8, r3
     e1c: e59d313c     	ldr	r3, [sp, #0x13c]
     e20: e0899003     	add	r9, r9, r3
     e24: e59d302c     	ldr	r3, [sp, #0x2c]
     e28: e0833002     	add	r3, r3, r2
     e2c: e59d2144     	ldr	r2, [sp, #0x144]
     e30: e58d302c     	str	r3, [sp, #0x2c]
     e34: e59d3030     	ldr	r3, [sp, #0x30]
     e38: e0833002     	add	r3, r3, r2
     e3c: e59d2148     	ldr	r2, [sp, #0x148]
     e40: e58d3030     	str	r3, [sp, #0x30]
     e44: e59d3034     	ldr	r3, [sp, #0x34]
     e48: e0833002     	add	r3, r3, r2
     e4c: e58d3034     	str	r3, [sp, #0x34]
     e50: e59d30bc     	ldr	r3, [sp, #0xbc]
     e54: e59d20e8     	ldr	r2, [sp, #0xe8]
     e58: e3530000     	cmp	r3, #0
     e5c: e59d30c0     	ldr	r3, [sp, #0xc0]
     e60: e0833002     	add	r3, r3, r2
     e64: e59d20c8     	ldr	r2, [sp, #0xc8]
     e68: e58d30c0     	str	r3, [sp, #0xc0]
     e6c: e59d30ec     	ldr	r3, [sp, #0xec]
     e70: e5933020     	ldr	r3, [r3, #0x20]
     e74: e0822103     	add	r2, r2, r3, lsl #2
     e78: e58d20c8     	str	r2, [sp, #0xc8]
     e7c: e59d20c4     	ldr	r2, [sp, #0xc4]
     e80: e0823083     	add	r3, r2, r3, lsl #1
     e84: e58d30c4     	str	r3, [sp, #0xc4]
     e88: 1afffdc0     	bne	0x590 <ZB_fillTriangleMappingPerspective_DT0_DW1+0x590> @ imm = #-0x900
     e8c: e59d3164     	ldr	r3, [sp, #0x164]
     e90: e1a05008     	mov	r5, r8
     e94: e59d20e8     	ldr	r2, [sp, #0xe8]
     e98: e1a0a009     	mov	r10, r9
     e9c: e59d8168     	ldr	r8, [sp, #0x168]
     ea0: e59db16c     	ldr	r11, [sp, #0x16c]
     ea4: e59d9170     	ldr	r9, [sp, #0x170]
     ea8: e59d7174     	ldr	r7, [sp, #0x174]
     eac: e0233298     	mla	r3, r8, r2, r3
     eb0: e58d3164     	str	r3, [sp, #0x164]
     eb4: e35b0001     	cmp	r11, #1
     eb8: 1a00001e     	bne	0xf38 <ZB_fillTriangleMappingPerspective_DT0_DW1+0xf38> @ imm = #0x78
     ebc: e28ddf61     	add	sp, sp, #388
     ec0: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     ec4: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     ec8: e59d3160     	ldr	r3, [sp, #0x160]
     ecc: e59d2154     	ldr	r2, [sp, #0x154]
     ed0: ee377a02     	vadd.f32	s14, s14, s4
     ed4: e0888003     	add	r8, r8, r3
     ed8: e59d3150     	ldr	r3, [sp, #0x150]
     edc: ee344a22     	vadd.f32	s8, s8, s5
     ee0: e0899003     	add	r9, r9, r3
     ee4: e59d302c     	ldr	r3, [sp, #0x2c]
     ee8: e0833002     	add	r3, r3, r2
     eec: e59d2158     	ldr	r2, [sp, #0x158]
     ef0: e58d302c     	str	r3, [sp, #0x2c]
     ef4: e59d3030     	ldr	r3, [sp, #0x30]
     ef8: e0833002     	add	r3, r3, r2
     efc: e59d215c     	ldr	r2, [sp, #0x15c]
     f00: e58d3030     	str	r3, [sp, #0x30]
     f04: e59d3034     	ldr	r3, [sp, #0x34]
     f08: e0833002     	add	r3, r3, r2
     f0c: e58d3034     	str	r3, [sp, #0x34]
     f10: eaffffce     	b	0xe50 <ZB_fillTriangleMappingPerspective_DT0_DW1+0xe50> @ imm = #-0xc8
     f14: e59d3034     	ldr	r3, [sp, #0x34]
     f18: eeb05a44     	vmov.f32	s10, s8
     f1c: eef04a47     	vmov.f32	s9, s14
     f20: e58d3010     	str	r3, [sp, #0x10]
     f24: e59d3030     	ldr	r3, [sp, #0x30]
     f28: e58d3004     	str	r3, [sp, #0x4]
     f2c: e59d302c     	ldr	r3, [sp, #0x2c]
     f30: e58d3000     	str	r3, [sp]
     f34: eaffff63     	b	0xcc8 <ZB_fillTriangleMappingPerspective_DT0_DW1+0xcc8> @ imm = #-0x274
     f38: e5991004     	ldr	r1, [r9, #0x4]
     f3c: e5973004     	ldr	r3, [r7, #0x4]
     f40: eef5cac0     	vcmpe.f32	s25, #0
     f44: eef1fa10     	vmrs	APSR_nzcv, fpscr
     f48: e0411003     	sub	r1, r1, r3
     f4c: e2818001     	add	r8, r1, #1
     f50: da000020     	ble	0xfd8 <ZB_fillTriangleMappingPerspective_DT0_DW1+0xfd8> @ imm = #0x80
     f54: e3510000     	cmp	r1, #0
     f58: e5974000     	ldr	r4, [r7]
     f5c: e3a06001     	mov	r6, #1
     f60: c58d717c     	strgt	r7, [sp, #0x17c]
     f64: c58d9178     	strgt	r9, [sp, #0x178]
     f68: da00001f     	ble	0xfec <ZB_fillTriangleMappingPerspective_DT0_DW1+0xfec> @ imm = #0x7c
     f6c: e59d3178     	ldr	r3, [sp, #0x178]
     f70: e1a0b006     	mov	r11, r6
     f74: edcd6a04     	vstr	s13, [sp, #16]
     f78: ed8d4a01     	vstr	s8, [sp, #4]
     f7c: e5930000     	ldr	r0, [r3]
     f80: ed8d7a00     	vstr	s14, [sp]
     f84: e0400004     	sub	r0, r0, r4
     f88: e1a00800     	lsl	r0, r0, #16
     f8c: ebfffffe     	bl	0xf8c <ZB_fillTriangleMappingPerspective_DT0_DW1+0xf8c> @ imm = #-0x8
     f90: ed9d7a00     	vldr	s14, [sp]
     f94: ed9d4a01     	vldr	s8, [sp, #4]
     f98: eddd6a04     	vldr	s13, [sp, #16]
     f9c: e58d00e8     	str	r0, [sp, #0xe8]
     fa0: e1a03804     	lsl	r3, r4, #16
     fa4: e58d3164     	str	r3, [sp, #0x164]
     fa8: eafffd57     	b	0x50c <ZB_fillTriangleMappingPerspective_DT0_DW1+0x50c> @ imm = #-0xaa4
     fac: e3a03000     	mov	r3, #0
     fb0: e3a02001     	mov	r2, #1
     fb4: eddf6a27     	vldr	s13, [pc, #156]         @ 0x1058 <ZB_fillTriangleMappingPerspective_DT0_DW1+0x1058>
     fb8: e58d30f0     	str	r3, [sp, #0xf0]
     fbc: ed8dfa57     	vstr	s30, [sp, #348]
     fc0: edcdea56     	vstr	s29, [sp, #344]
     fc4: ed8dea55     	vstr	s28, [sp, #340]
     fc8: edcdda54     	vstr	s27, [sp, #336]
     fcc: e58d214c     	str	r2, [sp, #0x14c]
     fd0: e58d3160     	str	r3, [sp, #0x160]
     fd4: eafffd3f     	b	0x4d8 <ZB_fillTriangleMappingPerspective_DT0_DW1+0x4d8> @ imm = #-0xb04
     fd8: e58db0b8     	str	r11, [sp, #0xb8]
     fdc: e1a02009     	mov	r2, r9
     fe0: e3a0b001     	mov	r11, #1
     fe4: e1a04007     	mov	r4, r7
     fe8: eafffd1a     	b	0x458 <ZB_fillTriangleMappingPerspective_DT0_DW1+0x458> @ imm = #-0xb98
     fec: e3580000     	cmp	r8, #0
     ff0: daffffb1     	ble	0xebc <ZB_fillTriangleMappingPerspective_DT0_DW1+0xebc> @ imm = #-0x13c
     ff4: e1a03804     	lsl	r3, r4, #16
     ff8: e58db0e8     	str	r11, [sp, #0xe8]
     ffc: e58d9178     	str	r9, [sp, #0x178]
    1000: e1a0b006     	mov	r11, r6
    1004: e58d3164     	str	r3, [sp, #0x164]
    1008: e58d717c     	str	r7, [sp, #0x17c]
    100c: eafffd40     	b	0x514 <ZB_fillTriangleMappingPerspective_DT0_DW1+0x514> @ imm = #-0xb00
    1010: e1a02006     	mov	r2, r6
    1014: e58d8178     	str	r8, [sp, #0x178]
    1018: eafffcf6     	b	0x3f8 <ZB_fillTriangleMappingPerspective_DT0_DW1+0x3f8> @ imm = #-0xc28
    101c: e59d3178     	ldr	r3, [sp, #0x178]
    1020: e59d217c     	ldr	r2, [sp, #0x17c]
    1024: e5931004     	ldr	r1, [r3, #0x4]
    1028: e5923004     	ldr	r3, [r2, #0x4]
    102c: e5924000     	ldr	r4, [r2]
    1030: e0411003     	sub	r1, r1, r3
    1034: e3510000     	cmp	r1, #0
    1038: c3a03000     	movgt	r3, #0
    103c: c1a0600b     	movgt	r6, r11
    1040: c58d30b8     	strgt	r3, [sp, #0xb8]
    1044: caffffc8     	bgt	0xf6c <ZB_fillTriangleMappingPerspective_DT0_DW1+0xf6c> @ imm = #-0xe0
    1048: e3a03000     	mov	r3, #0
    104c: e58d30b8     	str	r3, [sp, #0xb8]
    1050: e58d30e8     	str	r3, [sp, #0xe8]
    1054: eaffffd1     	b	0xfa0 <ZB_fillTriangleMappingPerspective_DT0_DW1+0xfa0> @ imm = #-0xbc
    1058: 00 00 00 00  	.word	0x00000000

Disassembly of section .text.ZB_fillTriangleMappingPerspective_DT1_DW0:

00000000 <ZB_fillTriangleMappingPerspective_DT1_DW0>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a0a003     	mov	r10, r3
       8: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
       c: e24ddf51     	sub	sp, sp, #324
      10: e592c004     	ldr	r12, [r2, #0x4]
      14: e5913004     	ldr	r3, [r1, #0x4]
      18: e58d00d4     	str	r0, [sp, #0xd4]
      1c: e59a0004     	ldr	r0, [r10, #0x4]
      20: e15c0003     	cmp	r12, r3
      24: a1a0e003     	movge	lr, r3
      28: b1a0e00c     	movlt	lr, r12
      2c: a1a04001     	movge	r4, r1
      30: a1a0300c     	movge	r3, r12
      34: b1a04002     	movlt	r4, r2
      38: a1a01002     	movge	r1, r2
      3c: e150000e     	cmp	r0, lr
      40: e1a0c00e     	mov	r12, lr
      44: e58d4124     	str	r4, [sp, #0x124]
      48: e58d1128     	str	r1, [sp, #0x128]
      4c: ba00000b     	blt	0x80 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x80> @ imm = #0x2c
      50: e1500003     	cmp	r0, r3
      54: e1a0200a     	mov	r2, r10
      58: a1a0c003     	movge	r12, r3
      5c: b1a0c000     	movlt	r12, r0
      60: a1a03000     	movge	r3, r0
      64: a1a0a004     	movge	r10, r4
      68: a1a0000e     	movge	r0, lr
      6c: b1a0000e     	movlt	r0, lr
      70: b59da124     	ldrlt	r10, [sp, #0x124]
      74: a58d1124     	strge	r1, [sp, #0x124]
      78: a58d2128     	strge	r2, [sp, #0x128]
      7c: b58d2124     	strlt	r2, [sp, #0x124]
      80: e0433000     	sub	r3, r3, r0
      84: e04c2000     	sub	r2, r12, r0
      88: edda5a03     	vldr	s11, [r10, #12]
      8c: ee073a90     	vmov	s15, r3
      90: e59d3128     	ldr	r3, [sp, #0x128]
      94: ee072a10     	vmov	s14, r2
      98: e59a2000     	ldr	r2, [r10]
      9c: e5933000     	ldr	r3, [r3]
      a0: eef86ae7     	vcvt.f32.s32	s13, s15
      a4: eeb87ac7     	vcvt.f32.s32	s14, s14
      a8: e0433002     	sub	r3, r3, r2
      ac: e59dc124     	ldr	r12, [sp, #0x124]
      b0: eef82ae5     	vcvt.f32.s32	s5, s11
      b4: ee073a90     	vmov	s15, r3
      b8: e59d3124     	ldr	r3, [sp, #0x124]
      bc: eef87ae7     	vcvt.f32.s32	s15, s15
      c0: edda5a04     	vldr	s11, [r10, #16]
      c4: e5933000     	ldr	r3, [r3]
      c8: e59d0128     	ldr	r0, [sp, #0x128]
      cc: ee67ca27     	vmul.f32	s25, s14, s15
      d0: e0433002     	sub	r3, r3, r2
      d4: e59a2008     	ldr	r2, [r10, #0x8]
      d8: eeb83ae5     	vcvt.f32.s32	s6, s11
      dc: ee053a10     	vmov	s10, r3
      e0: e59c3008     	ldr	r3, [r12, #0x8]
      e4: eeb85ac5     	vcvt.f32.s32	s10, s10
      e8: eeb2aa00     	vmov.f32	s20, #8.000000e+00
      ec: e0433002     	sub	r3, r3, r2
      f0: ee55ca26     	vnmls.f32	s25, s10, s13
      f4: eef5ca40     	vcmp.f32	s25, #0
      f8: eef1fa10     	vmrs	APSR_nzcv, fpscr
      fc: 1eb76a00     	vmovne.f32	s12, #1.000000e+00
     100: 1ec6ca2c     	vdivne.f32	s25, s12, s25
     104: ee062a10     	vmov	s12, r2
     108: eeb86ac6     	vcvt.f32.s32	s12, s12
     10c: ee622a86     	vmul.f32	s5, s5, s12
     110: ee233a06     	vmul.f32	s6, s6, s12
     114: ee063a10     	vmov	s12, r3
     118: e5903008     	ldr	r3, [r0, #0x8]
     11c: eeb86ac6     	vcvt.f32.s32	s12, s12
     120: e0433002     	sub	r3, r3, r2
     124: e59a2014     	ldr	r2, [r10, #0x14]
     128: ee6c7aa7     	vmul.f32	s15, s25, s15
     12c: ee053a90     	vmov	s11, r3
     130: ee2c5a85     	vmul.f32	s10, s25, s10
     134: eef85ae5     	vcvt.f32.s32	s11, s11
     138: ee674a86     	vmul.f32	s9, s15, s12
     13c: ee2c7a87     	vmul.f32	s14, s25, s14
     140: ee6c6aa6     	vmul.f32	s13, s25, s13
     144: e59c3014     	ldr	r3, [r12, #0x14]
     148: ee554a25     	vnmls.f32	s9, s10, s11
     14c: ee673a25     	vmul.f32	s7, s14, s11
     150: e0433002     	sub	r3, r3, r2
     154: eef5cac0     	vcmpe.f32	s25, #0
     158: eef1fa10     	vmrs	APSR_nzcv, fpscr
     15c: ee563a86     	vnmls.f32	s7, s13, s12
     160: eebd6ae4     	vcvt.s32.f32	s12, s9
     164: eef0da46     	vmov.f32	s27, s12
     168: ee063a10     	vmov	s12, r3
     16c: e5903014     	ldr	r3, [r0, #0x14]
     170: eeb86ac6     	vcvt.f32.s32	s12, s12
     174: e0433002     	sub	r3, r3, r2
     178: e59a2018     	ldr	r2, [r10, #0x18]
     17c: ee053a90     	vmov	s11, r3
     180: ee674a86     	vmul.f32	s9, s15, s12
     184: eef85ae5     	vcvt.f32.s32	s11, s11
     188: e59c3018     	ldr	r3, [r12, #0x18]
     18c: ee554a25     	vnmls.f32	s9, s10, s11
     190: e0433002     	sub	r3, r3, r2
     194: ee274a25     	vmul.f32	s8, s14, s11
     198: ee164a86     	vnmls.f32	s8, s13, s12
     19c: eebd6ae4     	vcvt.s32.f32	s12, s9
     1a0: eeb0ea46     	vmov.f32	s28, s12
     1a4: ee063a10     	vmov	s12, r3
     1a8: e5903018     	ldr	r3, [r0, #0x18]
     1ac: eeb86ac6     	vcvt.f32.s32	s12, s12
     1b0: e0433002     	sub	r3, r3, r2
     1b4: e59a201c     	ldr	r2, [r10, #0x1c]
     1b8: ee053a90     	vmov	s11, r3
     1bc: ee272a86     	vmul.f32	s4, s15, s12
     1c0: eef85ae5     	vcvt.f32.s32	s11, s11
     1c4: e59c301c     	ldr	r3, [r12, #0x1c]
     1c8: ee152a25     	vnmls.f32	s4, s10, s11
     1cc: e0433002     	sub	r3, r3, r2
     1d0: ee674a25     	vmul.f32	s9, s14, s11
     1d4: ee564a86     	vnmls.f32	s9, s13, s12
     1d8: eebd6ac2     	vcvt.s32.f32	s12, s4
     1dc: eef0ea46     	vmov.f32	s29, s12
     1e0: ee063a10     	vmov	s12, r3
     1e4: e590301c     	ldr	r3, [r0, #0x1c]
     1e8: eeb86ac6     	vcvt.f32.s32	s12, s12
     1ec: edca2a08     	vstr	s5, [r10, #32]
     1f0: e0433002     	sub	r3, r3, r2
     1f4: ed8a3a09     	vstr	s6, [r10, #36]
     1f8: ee053a90     	vmov	s11, r3
     1fc: ee272a86     	vmul.f32	s4, s15, s12
     200: eef85ae5     	vcvt.f32.s32	s11, s11
     204: e59c3008     	ldr	r3, [r12, #0x8]
     208: eef02a42     	vmov.f32	s5, s4
     20c: ee273a25     	vmul.f32	s6, s14, s11
     210: ee552a25     	vnmls.f32	s5, s10, s11
     214: eef05a43     	vmov.f32	s11, s6
     218: ee565a86     	vnmls.f32	s11, s13, s12
     21c: eebd6ae2     	vcvt.s32.f32	s12, s5
     220: eeb0fa46     	vmov.f32	s30, s12
     224: eebd6ae5     	vcvt.s32.f32	s12, s11
     228: ed8d6a0f     	vstr	s12, [sp, #60]
     22c: ee063a10     	vmov	s12, r3
     230: e59c300c     	ldr	r3, [r12, #0xc]
     234: eef85ac6     	vcvt.f32.s32	s11, s12
     238: ee063a10     	vmov	s12, r3
     23c: e59c3010     	ldr	r3, [r12, #0x10]
     240: eeb86ac6     	vcvt.f32.s32	s12, s12
     244: ee266a25     	vmul.f32	s12, s12, s11
     248: ed8c6a08     	vstr	s12, [r12, #32]
     24c: ee063a10     	vmov	s12, r3
     250: eeb86ac6     	vcvt.f32.s32	s12, s12
     254: ee266a25     	vmul.f32	s12, s12, s11
     258: ed8c6a09     	vstr	s12, [r12, #36]
     25c: e5903008     	ldr	r3, [r0, #0x8]
     260: ee063a10     	vmov	s12, r3
     264: e590300c     	ldr	r3, [r0, #0xc]
     268: eeb83ac6     	vcvt.f32.s32	s6, s12
     26c: ee063a10     	vmov	s12, r3
     270: eef85ac6     	vcvt.f32.s32	s11, s12
     274: ee655a83     	vmul.f32	s11, s11, s6
     278: edc05a08     	vstr	s11, [r0, #32]
     27c: e5903010     	ldr	r3, [r0, #0x10]
     280: e59d40d4     	ldr	r4, [sp, #0xd4]
     284: e59de03c     	ldr	lr, [sp, #0x3c]
     288: e58d3004     	str	r3, [sp, #0x4]
     28c: ee063a10     	vmov	s12, r3
     290: eeb86ac6     	vcvt.f32.s32	s12, s12
     294: e1a0708e     	lsl	r7, lr, #1
     298: e58d70e8     	str	r7, [sp, #0xe8]
     29c: ee266a03     	vmul.f32	s12, s12, s6
     2a0: ed806a09     	vstr	s12, [r0, #36]
     2a4: edda1a08     	vldr	s3, [r10, #32]
     2a8: ed9a2a09     	vldr	s4, [r10, #36]
     2ac: eddc2a08     	vldr	s5, [r12, #32]
     2b0: ed9c3a09     	vldr	s6, [r12, #36]
     2b4: e5943020     	ldr	r3, [r4, #0x20]
     2b8: e59a1004     	ldr	r1, [r10, #0x4]
     2bc: ee722ae1     	vsub.f32	s5, s5, s3
     2c0: ee333a42     	vsub.f32	s6, s6, s4
     2c4: e5942004     	ldr	r2, [r4, #0x4]
     2c8: e0030193     	mul	r3, r3, r1
     2cc: ee755ae1     	vsub.f32	s11, s11, s3
     2d0: ee27daa2     	vmul.f32	s26, s15, s5
     2d4: ee27ca83     	vmul.f32	s24, s15, s6
     2d8: eefd7ae3     	vcvt.s32.f32	s15, s7
     2dc: e0822103     	add	r2, r2, r3, lsl #2
     2e0: ee366a42     	vsub.f32	s12, s12, s4
     2e4: ee679a25     	vmul.f32	s19, s14, s11
     2e8: e58d20b4     	str	r2, [sp, #0xb4]
     2ec: e5942000     	ldr	r2, [r4]
     2f0: e1a0418e     	lsl	r4, lr, #3
     2f4: ee178a90     	vmov	r8, s15
     2f8: eeb88ae7     	vcvt.f32.s32	s16, s15
     2fc: eefd7ac4     	vcvt.s32.f32	s15, s8
     300: e0823083     	add	r3, r2, r3, lsl #1
     304: e58d4030     	str	r4, [sp, #0x30]
     308: ee279a06     	vmul.f32	s18, s14, s12
     30c: e58d30b8     	str	r3, [sp, #0xb8]
     310: edcd7a0d     	vstr	s15, [sp, #52]
     314: e59d3034     	ldr	r3, [sp, #0x34]
     318: eefd7ae4     	vcvt.s32.f32	s15, s9
     31c: ee569aa2     	vnmls.f32	s19, s13, s5
     320: ee169a83     	vnmls.f32	s18, s13, s6
     324: e1a04103     	lsl	r4, r3, #2
     328: e1a05083     	lsl	r5, r3, #1
     32c: ee172a90     	vmov	r2, s15
     330: e1a0b183     	lsl	r11, r3, #3
     334: e1a06082     	lsl	r6, r2, #1
     338: e1a09182     	lsl	r9, r2, #3
     33c: e58d407c     	str	r4, [sp, #0x7c]
     340: e1a04102     	lsl	r4, r2, #2
     344: e58d60ec     	str	r6, [sp, #0xec]
     348: e0866002     	add	r6, r6, r2
     34c: e58d4078     	str	r4, [sp, #0x78]
     350: e1a0410e     	lsl	r4, lr, #2
     354: e58d60e4     	str	r6, [sp, #0xe4]
     358: e087600e     	add	r6, r7, lr
     35c: e58d4074     	str	r4, [sp, #0x74]
     360: e1a07188     	lsl	r7, r8, #3
     364: e59d40d4     	ldr	r4, [sp, #0xd4]
     368: e58d50f0     	str	r5, [sp, #0xf0]
     36c: e0855003     	add	r5, r5, r3
     370: e58d50f4     	str	r5, [sp, #0xf4]
     374: e58d60e0     	str	r6, [sp, #0xe0]
     378: edcd7a0e     	vstr	s15, [sp, #56]
     37c: e5946008     	ldr	r6, [r4, #0x8]
     380: e58d9040     	str	r9, [sp, #0x40]
     384: e58db044     	str	r11, [sp, #0x44]
     388: e58d7048     	str	r7, [sp, #0x48]
     38c: e0887108     	add	r7, r8, r8, lsl #2
     390: ee69aa8a     	vmul.f32	s21, s19, s20
     394: e58d70f8     	str	r7, [sp, #0xf8]
     398: e1a07085     	lsl	r7, r5, #1
     39c: e59d507c     	ldr	r5, [sp, #0x7c]
     3a0: e58d7098     	str	r7, [sp, #0x98]
     3a4: e59d70e4     	ldr	r7, [sp, #0xe4]
     3a8: ee29ba0a     	vmul.f32	s22, s18, s20
     3ac: ee15da25     	vnmls.f32	s26, s10, s11
     3b0: ee15ca06     	vnmls.f32	s24, s10, s12
     3b4: e1a07087     	lsl	r7, r7, #1
     3b8: ee28aa0a     	vmul.f32	s20, s16, s20
     3bc: e58d709c     	str	r7, [sp, #0x9c]
     3c0: e59d70e0     	ldr	r7, [sp, #0xe0]
     3c4: e1a07087     	lsl	r7, r7, #1
     3c8: e58d70a0     	str	r7, [sp, #0xa0]
     3cc: e0857003     	add	r7, r5, r3
     3d0: e59d5078     	ldr	r5, [sp, #0x78]
     3d4: e58d7088     	str	r7, [sp, #0x88]
     3d8: e0857002     	add	r7, r5, r2
     3dc: e59d5074     	ldr	r5, [sp, #0x74]
     3e0: e0492002     	sub	r2, r9, r2
     3e4: e58d7084     	str	r7, [sp, #0x84]
     3e8: e58d2090     	str	r2, [sp, #0x90]
     3ec: e085500e     	add	r5, r5, lr
     3f0: e58d5080     	str	r5, [sp, #0x80]
     3f4: e04b5003     	sub	r5, r11, r3
     3f8: e59d3030     	ldr	r3, [sp, #0x30]
     3fc: e58d5094     	str	r5, [sp, #0x94]
     400: e043300e     	sub	r3, r3, lr
     404: e58d308c     	str	r3, [sp, #0x8c]
     408: da0002f7     	ble	0xfec <ZB_fillTriangleMappingPerspective_DT1_DW0+0xfec> @ imm = #0xbdc
     40c: e58dc138     	str	r12, [sp, #0x138]
     410: e59d3124     	ldr	r3, [sp, #0x124]
     414: e3a09000     	mov	r9, #0
     418: e1a02009     	mov	r2, r9
     41c: e58d600c     	str	r6, [sp, #0xc]
     420: eef7ba00     	vmov.f32	s23, #1.000000e+00
     424: e1a0400a     	mov	r4, r10
     428: e5933004     	ldr	r3, [r3, #0x4]
     42c: e1a0b009     	mov	r11, r9
     430: eef08a00     	vmov.f32	s17, #2.000000e+00
     434: e34027f8     	movt	r2, #0x7f8
     438: e3a0e001     	mov	lr, #1
     43c: e58d90d8     	str	r9, [sp, #0xd8]
     440: e043c001     	sub	r12, r3, r1
     444: e5903004     	ldr	r3, [r0, #0x4]
     448: e1a0600c     	mov	r6, r12
     44c: e58de0a4     	str	lr, [sp, #0xa4]
     450: e58d8008     	str	r8, [sp, #0x8]
     454: e0431001     	sub	r1, r3, r1
     458: e3a03b3e     	mov	r3, #63488
     45c: e3403007     	movt	r3, #0x7
     460: e58da13c     	str	r10, [sp, #0x13c]
     464: e58d2010     	str	r2, [sp, #0x10]
     468: e58d3014     	str	r3, [sp, #0x14]
     46c: e5942000     	ldr	r2, [r4]
     470: e3510000     	cmp	r1, #0
     474: e58d20b0     	str	r2, [sp, #0xb0]
     478: da0002c3     	ble	0xf8c <ZB_fillTriangleMappingPerspective_DT1_DW0+0xf8c> @ imm = #0xb0c
     47c: e5903000     	ldr	r3, [r0]
     480: e0433002     	sub	r3, r3, r2
     484: e1a00803     	lsl	r0, r3, #16
     488: ebfffffe     	bl	0x488 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x488> @ imm = #-0x8
     48c: e59d1008     	ldr	r1, [sp, #0x8]
     490: e6ff2070     	uxth	r2, r0
     494: e1a03840     	asr	r3, r0, #16
     498: e58d20dc     	str	r2, [sp, #0xdc]
     49c: ee1d2a90     	vmov	r2, s27
     4a0: ee073a90     	vmov	s15, r3
     4a4: e0222193     	mla	r2, r3, r1, r2
     4a8: e59d1034     	ldr	r1, [sp, #0x34]
     4ac: eeb87ae7     	vcvt.f32.s32	s14, s15
     4b0: e58d3120     	str	r3, [sp, #0x120]
     4b4: e58d2110     	str	r2, [sp, #0x110]
     4b8: ee1e2a10     	vmov	r2, s28
     4bc: e0222193     	mla	r2, r3, r1, r2
     4c0: e59d1038     	ldr	r1, [sp, #0x38]
     4c4: e58d2114     	str	r2, [sp, #0x114]
     4c8: ee1e2a90     	vmov	r2, s29
     4cc: e0222193     	mla	r2, r3, r1, r2
     4d0: e59d103c     	ldr	r1, [sp, #0x3c]
     4d4: e58d2118     	str	r2, [sp, #0x118]
     4d8: ee1f2a10     	vmov	r2, s30
     4dc: e0222193     	mla	r2, r3, r1, r2
     4e0: e2833001     	add	r3, r3, #1
     4e4: e58d310c     	str	r3, [sp, #0x10c]
     4e8: e58d211c     	str	r2, [sp, #0x11c]
     4ec: e59d30a4     	ldr	r3, [sp, #0xa4]
     4f0: edd46a08     	vldr	s13, [r4, #32]
     4f4: edd43a09     	vldr	s7, [r4, #36]
     4f8: e3530000     	cmp	r3, #0
     4fc: e5943008     	ldr	r3, [r4, #0x8]
     500: e1a05003     	mov	r5, r3
     504: e5943014     	ldr	r3, [r4, #0x14]
     508: e58d3020     	str	r3, [sp, #0x20]
     50c: e5943018     	ldr	r3, [r4, #0x18]
     510: e58d3024     	str	r3, [sp, #0x24]
     514: e594301c     	ldr	r3, [r4, #0x1c]
     518: e58d3028     	str	r3, [sp, #0x28]
     51c: 1a0002b6     	bne	0xffc <ZB_fillTriangleMappingPerspective_DT1_DW0+0xffc> @ imm = #0xad8
     520: e3560000     	cmp	r6, #0
     524: da000255     	ble	0xe80 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xe80> @ imm = #0x954
     528: e59d3008     	ldr	r3, [sp, #0x8]
     52c: eef01a4d     	vmov.f32	s3, s26
     530: eeb02a4c     	vmov.f32	s4, s24
     534: ee491a87     	vmla.f32	s3, s19, s14
     538: e1a02083     	lsl	r2, r3, #1
     53c: ee092a07     	vmla.f32	s4, s18, s14
     540: e58db0ac     	str	r11, [sp, #0xac]
     544: e58d202c     	str	r2, [sp, #0x2c]
     548: e59d2110     	ldr	r2, [sp, #0x110]
     54c: e58db134     	str	r11, [sp, #0x134]
     550: e1a0b005     	mov	r11, r5
     554: ee792aa1     	vadd.f32	s5, s19, s3
     558: e0833002     	add	r3, r3, r2
     55c: e59d2114     	ldr	r2, [sp, #0x114]
     560: e58d30fc     	str	r3, [sp, #0xfc]
     564: e59d3034     	ldr	r3, [sp, #0x34]
     568: ee393a02     	vadd.f32	s6, s18, s4
     56c: e58d60a8     	str	r6, [sp, #0xa8]
     570: e0833002     	add	r3, r3, r2
     574: e59d2118     	ldr	r2, [sp, #0x118]
     578: e58d3100     	str	r3, [sp, #0x100]
     57c: e59d3038     	ldr	r3, [sp, #0x38]
     580: e58d912c     	str	r9, [sp, #0x12c]
     584: e58d6130     	str	r6, [sp, #0x130]
     588: e0833002     	add	r3, r3, r2
     58c: e59d211c     	ldr	r2, [sp, #0x11c]
     590: e58d3104     	str	r3, [sp, #0x104]
     594: e59d303c     	ldr	r3, [sp, #0x3c]
     598: e0833002     	add	r3, r3, r2
     59c: e58d3108     	str	r3, [sp, #0x108]
     5a0: e59d30a8     	ldr	r3, [sp, #0xa8]
     5a4: e1a0000b     	mov	r0, r11
     5a8: e59d20ac     	ldr	r2, [sp, #0xac]
     5ac: ee07ba90     	vmov	s15, r11
     5b0: e2433001     	sub	r3, r3, #1
     5b4: eeb86ae7     	vcvt.f32.s32	s12, s15
     5b8: e58d30a8     	str	r3, [sp, #0xa8]
     5bc: e59d30b0     	ldr	r3, [sp, #0xb0]
     5c0: eecb7a86     	vdiv.f32	s15, s23, s12
     5c4: e063c842     	rsb	r12, r3, r2, asr #16
     5c8: e59d20b4     	ldr	r2, [sp, #0xb4]
     5cc: e35c0006     	cmp	r12, #6
     5d0: e0826103     	add	r6, r2, r3, lsl #2
     5d4: e59d20b8     	ldr	r2, [sp, #0xb8]
     5d8: e0821083     	add	r1, r2, r3, lsl #1
     5dc: da000241     	ble	0xee8 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xee8> @ imm = #0x904
     5e0: e24c3007     	sub	r3, r12, #7
     5e4: e59da038     	ldr	r10, [sp, #0x38]
     5e8: e1a091a3     	lsr	r9, r3, #3
     5ec: e2813010     	add	r3, r1, #16
     5f0: e2811020     	add	r1, r1, #32
     5f4: e59d003c     	ldr	r0, [sp, #0x3c]
     5f8: e1a08209     	lsl	r8, r9, #4
     5fc: e2867020     	add	r7, r6, #32
     600: e3a0e000     	mov	lr, #0
     604: eef04a63     	vmov.f32	s9, s7
     608: e0812008     	add	r2, r1, r8
     60c: e59d1034     	ldr	r1, [sp, #0x34]
     610: e58d204c     	str	r2, [sp, #0x4c]
     614: e1a05007     	mov	r5, r7
     618: e59d20f8     	ldr	r2, [sp, #0xf8]
     61c: e1a0600e     	mov	r6, lr
     620: eeb04a66     	vmov.f32	s8, s13
     624: e58db0bc     	str	r11, [sp, #0xbc]
     628: e08b2002     	add	r2, r11, r2
     62c: e58dc0c0     	str	r12, [sp, #0xc0]
     630: e58d201c     	str	r2, [sp, #0x1c]
     634: e59d2008     	ldr	r2, [sp, #0x8]
     638: e58d80c4     	str	r8, [sp, #0xc4]
     63c: e58d90c8     	str	r9, [sp, #0xc8]
     640: e08b2002     	add	r2, r11, r2
     644: e58d70cc     	str	r7, [sp, #0xcc]
     648: e58d2018     	str	r2, [sp, #0x18]
     64c: e59d2020     	ldr	r2, [sp, #0x20]
     650: e58d30d0     	str	r3, [sp, #0xd0]
     654: e0821001     	add	r1, r2, r1
     658: e58d1050     	str	r1, [sp, #0x50]
     65c: e59d1024     	ldr	r1, [sp, #0x24]
     660: e081a00a     	add	r10, r1, r10
     664: e58da054     	str	r10, [sp, #0x54]
     668: e59da028     	ldr	r10, [sp, #0x28]
     66c: e08a0000     	add	r0, r10, r0
     670: e58d0058     	str	r0, [sp, #0x58]
     674: e59d00f0     	ldr	r0, [sp, #0xf0]
     678: e0820000     	add	r0, r2, r0
     67c: e58d005c     	str	r0, [sp, #0x5c]
     680: e59d00ec     	ldr	r0, [sp, #0xec]
     684: e0810000     	add	r0, r1, r0
     688: e58d0060     	str	r0, [sp, #0x60]
     68c: e59d00e8     	ldr	r0, [sp, #0xe8]
     690: e08a0000     	add	r0, r10, r0
     694: e58d0064     	str	r0, [sp, #0x64]
     698: e59d00f4     	ldr	r0, [sp, #0xf4]
     69c: e0822000     	add	r2, r2, r0
     6a0: e1a00003     	mov	r0, r3
     6a4: e58d2068     	str	r2, [sp, #0x68]
     6a8: e59d20e4     	ldr	r2, [sp, #0xe4]
     6ac: e0812002     	add	r2, r1, r2
     6b0: e58d206c     	str	r2, [sp, #0x6c]
     6b4: e59d20e0     	ldr	r2, [sp, #0xe0]
     6b8: e08a2002     	add	r2, r10, r2
     6bc: e58d2070     	str	r2, [sp, #0x70]
     6c0: e58de004     	str	lr, [sp, #0x4]
     6c4: ee366a0a     	vadd.f32	s12, s12, s20
     6c8: ee640a27     	vmul.f32	s1, s8, s15
     6cc: ee240aa7     	vmul.f32	s0, s9, s15
     6d0: eeb01a68     	vmov.f32	s2, s17
     6d4: eeb05a69     	vmov.f32	s10, s19
     6d8: ee061a67     	vmls.f32	s2, s12, s15
     6dc: eef05a49     	vmov.f32	s11, s18
     6e0: ee085a60     	vmls.f32	s10, s16, s1
     6e4: ee485a40     	vmls.f32	s11, s16, s0
     6e8: e15031b0     	ldrh	r3, [r0, #-16]
     6ec: ee211a27     	vmul.f32	s2, s2, s15
     6f0: e59d1008     	ldr	r1, [sp, #0x8]
     6f4: ee255a27     	vmul.f32	s10, s10, s15
     6f8: ee655aa7     	vmul.f32	s11, s11, s15
     6fc: eef07a68     	vmov.f32	s15, s17
     700: e59d2018     	ldr	r2, [sp, #0x18]
     704: ee467a41     	vmls.f32	s15, s12, s2
     708: eebd5ac5     	vcvt.s32.f32	s10, s10
     70c: e0422001     	sub	r2, r2, r1
     710: eefd5ae5     	vcvt.s32.f32	s11, s11
     714: e1530722     	cmp	r3, r2, lsr #14
     718: ee151a10     	vmov	r1, s10
     71c: ee15ca90     	vmov	r12, s11
     720: ee677a81     	vmul.f32	s15, s15, s2
     724: eebd1ae0     	vcvt.s32.f32	s2, s1
     728: ee114a10     	vmov	r4, s2
     72c: eebd1ac0     	vcvt.s32.f32	s2, s0
     730: ee113a10     	vmov	r3, s2
     734: 8a00001a     	bhi	0x7a4 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x7a4> @ imm = #0x68
     738: e59d7010     	ldr	r7, [sp, #0x10]
     73c: e59d8014     	ldr	r8, [sp, #0x14]
     740: e0037007     	and	r7, r3, r7
     744: e0048008     	and	r8, r4, r8
     748: e1877008     	orr	r7, r7, r8
     74c: e59d800c     	ldr	r8, [sp, #0xc]
     750: e798a4a7     	ldr	r10, [r8, r7, lsr #9]
     754: e59d7020     	ldr	r7, [sp, #0x20]
     758: e59d8004     	ldr	r8, [sp, #0x4]
     75c: e087b008     	add	r11, r7, r8
     760: e59d7024     	ldr	r7, [sp, #0x24]
     764: e0878006     	add	r8, r7, r6
     768: e59d7028     	ldr	r7, [sp, #0x28]
     76c: e087900e     	add	r9, r7, lr
     770: e7e7785a     	ubfx	r7, r10, #0x10, #0x8
     774: e007079b     	mul	r7, r11, r7
     778: e7e7b45a     	ubfx	r11, r10, #0x8, #0x8
     77c: e6efa07a     	uxtb	r10, r10
     780: e0080b98     	mul	r8, r8, r11
     784: e0090a99     	mul	r9, r9, r10
     788: e1a07427     	lsr	r7, r7, #8
     78c: e1a08828     	lsr	r8, r8, #16
     790: e20778ff     	and	r7, r7, #16711680
     794: e2088cff     	and	r8, r8, #65280
     798: e1877008     	orr	r7, r7, r8
     79c: e1877c29     	orr	r7, r7, r9, lsr #24
     7a0: e5057020     	str	r7, [r5, #-0x20]
     7a4: e15070be     	ldrh	r7, [r0, #-14]
     7a8: e0814004     	add	r4, r1, r4
     7ac: e59d8018     	ldr	r8, [sp, #0x18]
     7b0: e08c3003     	add	r3, r12, r3
     7b4: e1570728     	cmp	r7, r8, lsr #14
     7b8: 8a00001a     	bhi	0x828 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x828> @ imm = #0x68
     7bc: e59d7014     	ldr	r7, [sp, #0x14]
     7c0: e59d8010     	ldr	r8, [sp, #0x10]
     7c4: e0047007     	and	r7, r4, r7
     7c8: e0038008     	and	r8, r3, r8
     7cc: e1877008     	orr	r7, r7, r8
     7d0: e59d800c     	ldr	r8, [sp, #0xc]
     7d4: e798a4a7     	ldr	r10, [r8, r7, lsr #9]
     7d8: e59d7050     	ldr	r7, [sp, #0x50]
     7dc: e59d8004     	ldr	r8, [sp, #0x4]
     7e0: e087b008     	add	r11, r7, r8
     7e4: e59d7054     	ldr	r7, [sp, #0x54]
     7e8: e0878006     	add	r8, r7, r6
     7ec: e59d7058     	ldr	r7, [sp, #0x58]
     7f0: e087900e     	add	r9, r7, lr
     7f4: e7e7785a     	ubfx	r7, r10, #0x10, #0x8
     7f8: e007079b     	mul	r7, r11, r7
     7fc: e7e7b45a     	ubfx	r11, r10, #0x8, #0x8
     800: e6efa07a     	uxtb	r10, r10
     804: e0080b98     	mul	r8, r8, r11
     808: e009099a     	mul	r9, r10, r9
     80c: e1a07427     	lsr	r7, r7, #8
     810: e1a08828     	lsr	r8, r8, #16
     814: e20778ff     	and	r7, r7, #16711680
     818: e2088cff     	and	r8, r8, #65280
     81c: e1877008     	orr	r7, r7, r8
     820: e1877c29     	orr	r7, r7, r9, lsr #24
     824: e505701c     	str	r7, [r5, #-0x1c]
     828: e59d702c     	ldr	r7, [sp, #0x2c]
     82c: e0814004     	add	r4, r1, r4
     830: e08c3003     	add	r3, r12, r3
     834: e0822007     	add	r2, r2, r7
     838: e15070bc     	ldrh	r7, [r0, #-12]
     83c: e1570722     	cmp	r7, r2, lsr #14
     840: 8a00001a     	bhi	0x8b0 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x8b0> @ imm = #0x68
     844: e59d7014     	ldr	r7, [sp, #0x14]
     848: e59d8010     	ldr	r8, [sp, #0x10]
     84c: e0047007     	and	r7, r4, r7
     850: e0038008     	and	r8, r3, r8
     854: e1877008     	orr	r7, r7, r8
     858: e59d800c     	ldr	r8, [sp, #0xc]
     85c: e798a4a7     	ldr	r10, [r8, r7, lsr #9]
     860: e59d705c     	ldr	r7, [sp, #0x5c]
     864: e59d8004     	ldr	r8, [sp, #0x4]
     868: e087b008     	add	r11, r7, r8
     86c: e59d7060     	ldr	r7, [sp, #0x60]
     870: e0878006     	add	r8, r7, r6
     874: e59d7064     	ldr	r7, [sp, #0x64]
     878: e087900e     	add	r9, r7, lr
     87c: e7e7785a     	ubfx	r7, r10, #0x10, #0x8
     880: e007079b     	mul	r7, r11, r7
     884: e7e7b45a     	ubfx	r11, r10, #0x8, #0x8
     888: e6efa07a     	uxtb	r10, r10
     88c: e0080b98     	mul	r8, r8, r11
     890: e009099a     	mul	r9, r10, r9
     894: e1a07427     	lsr	r7, r7, #8
     898: e1a08828     	lsr	r8, r8, #16
     89c: e20778ff     	and	r7, r7, #16711680
     8a0: e2088cff     	and	r8, r8, #65280
     8a4: e1877008     	orr	r7, r7, r8
     8a8: e1877c29     	orr	r7, r7, r9, lsr #24
     8ac: e5057018     	str	r7, [r5, #-0x18]
     8b0: e59d7008     	ldr	r7, [sp, #0x8]
     8b4: e0814004     	add	r4, r1, r4
     8b8: e08c3003     	add	r3, r12, r3
     8bc: e0822007     	add	r2, r2, r7
     8c0: e15070ba     	ldrh	r7, [r0, #-10]
     8c4: e1570722     	cmp	r7, r2, lsr #14
     8c8: 8a00001a     	bhi	0x938 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x938> @ imm = #0x68
     8cc: e59d2014     	ldr	r2, [sp, #0x14]
     8d0: e59d7010     	ldr	r7, [sp, #0x10]
     8d4: e0042002     	and	r2, r4, r2
     8d8: e0037007     	and	r7, r3, r7
     8dc: e1822007     	orr	r2, r2, r7
     8e0: e59d700c     	ldr	r7, [sp, #0xc]
     8e4: e79794a2     	ldr	r9, [r7, r2, lsr #9]
     8e8: e59d2068     	ldr	r2, [sp, #0x68]
     8ec: e59d7004     	ldr	r7, [sp, #0x4]
     8f0: e082a007     	add	r10, r2, r7
     8f4: e59d206c     	ldr	r2, [sp, #0x6c]
     8f8: e0827006     	add	r7, r2, r6
     8fc: e59d2070     	ldr	r2, [sp, #0x70]
     900: e082800e     	add	r8, r2, lr
     904: e7e72859     	ubfx	r2, r9, #0x10, #0x8
     908: e002029a     	mul	r2, r10, r2
     90c: e7e7a459     	ubfx	r10, r9, #0x8, #0x8
     910: e6ef9079     	uxtb	r9, r9
     914: e0070a97     	mul	r7, r7, r10
     918: e0080899     	mul	r8, r9, r8
     91c: e1a02422     	lsr	r2, r2, #8
     920: e1a07827     	lsr	r7, r7, #16
     924: e20228ff     	and	r2, r2, #16711680
     928: e2077cff     	and	r7, r7, #65280
     92c: e1822007     	orr	r2, r2, r7
     930: e1822c28     	orr	r2, r2, r8, lsr #24
     934: e5052014     	str	r2, [r5, #-0x14]
     938: e15020b8     	ldrh	r2, [r0, #-8]
     93c: e0814004     	add	r4, r1, r4
     940: e59d701c     	ldr	r7, [sp, #0x1c]
     944: e08c3003     	add	r3, r12, r3
     948: e59d8008     	ldr	r8, [sp, #0x8]
     94c: e0477008     	sub	r7, r7, r8
     950: e1520727     	cmp	r2, r7, lsr #14
     954: 8a000020     	bhi	0x9dc <ZB_fillTriangleMappingPerspective_DT1_DW0+0x9dc> @ imm = #0x80
     958: e59d2014     	ldr	r2, [sp, #0x14]
     95c: e59d8010     	ldr	r8, [sp, #0x10]
     960: e59d907c     	ldr	r9, [sp, #0x7c]
     964: e0042002     	and	r2, r4, r2
     968: e0038008     	and	r8, r3, r8
     96c: e1822008     	orr	r2, r2, r8
     970: e59d800c     	ldr	r8, [sp, #0xc]
     974: e79884a2     	ldr	r8, [r8, r2, lsr #9]
     978: e59d2020     	ldr	r2, [sp, #0x20]
     97c: e082b009     	add	r11, r2, r9
     980: e59d2004     	ldr	r2, [sp, #0x4]
     984: e59d9078     	ldr	r9, [sp, #0x78]
     988: e08bb002     	add	r11, r11, r2
     98c: e59d2024     	ldr	r2, [sp, #0x24]
     990: e082a009     	add	r10, r2, r9
     994: e59d2028     	ldr	r2, [sp, #0x28]
     998: e59d9074     	ldr	r9, [sp, #0x74]
     99c: e08aa006     	add	r10, r10, r6
     9a0: e0829009     	add	r9, r2, r9
     9a4: e7e72858     	ubfx	r2, r8, #0x10, #0x8
     9a8: e002029b     	mul	r2, r11, r2
     9ac: e7e7b458     	ubfx	r11, r8, #0x8, #0x8
     9b0: e6ef8078     	uxtb	r8, r8
     9b4: e089900e     	add	r9, r9, lr
     9b8: e00b0b9a     	mul	r11, r10, r11
     9bc: e0080998     	mul	r8, r8, r9
     9c0: e1a02422     	lsr	r2, r2, #8
     9c4: e1a0b82b     	lsr	r11, r11, #16
     9c8: e20228ff     	and	r2, r2, #16711680
     9cc: e20bbcff     	and	r11, r11, #65280
     9d0: e182200b     	orr	r2, r2, r11
     9d4: e1822c28     	orr	r2, r2, r8, lsr #24
     9d8: e5052010     	str	r2, [r5, #-0x10]
     9dc: e15020b6     	ldrh	r2, [r0, #-6]
     9e0: e0814004     	add	r4, r1, r4
     9e4: e59d801c     	ldr	r8, [sp, #0x1c]
     9e8: e08c3003     	add	r3, r12, r3
     9ec: e1520728     	cmp	r2, r8, lsr #14
     9f0: 8a000020     	bhi	0xa78 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xa78> @ imm = #0x80
     9f4: e59d2014     	ldr	r2, [sp, #0x14]
     9f8: e59d8010     	ldr	r8, [sp, #0x10]
     9fc: e59d9088     	ldr	r9, [sp, #0x88]
     a00: e0042002     	and	r2, r4, r2
     a04: e0038008     	and	r8, r3, r8
     a08: e1822008     	orr	r2, r2, r8
     a0c: e59d800c     	ldr	r8, [sp, #0xc]
     a10: e79884a2     	ldr	r8, [r8, r2, lsr #9]
     a14: e59d2020     	ldr	r2, [sp, #0x20]
     a18: e082b009     	add	r11, r2, r9
     a1c: e59d2004     	ldr	r2, [sp, #0x4]
     a20: e59d9084     	ldr	r9, [sp, #0x84]
     a24: e08bb002     	add	r11, r11, r2
     a28: e59d2024     	ldr	r2, [sp, #0x24]
     a2c: e082a009     	add	r10, r2, r9
     a30: e59d2028     	ldr	r2, [sp, #0x28]
     a34: e59d9080     	ldr	r9, [sp, #0x80]
     a38: e08aa006     	add	r10, r10, r6
     a3c: e0829009     	add	r9, r2, r9
     a40: e7e72858     	ubfx	r2, r8, #0x10, #0x8
     a44: e002029b     	mul	r2, r11, r2
     a48: e7e7b458     	ubfx	r11, r8, #0x8, #0x8
     a4c: e6ef8078     	uxtb	r8, r8
     a50: e089900e     	add	r9, r9, lr
     a54: e00b0b9a     	mul	r11, r10, r11
     a58: e0080998     	mul	r8, r8, r9
     a5c: e1a02422     	lsr	r2, r2, #8
     a60: e1a0b82b     	lsr	r11, r11, #16
     a64: e20228ff     	and	r2, r2, #16711680
     a68: e20bbcff     	and	r11, r11, #65280
     a6c: e182200b     	orr	r2, r2, r11
     a70: e1822c28     	orr	r2, r2, r8, lsr #24
     a74: e505200c     	str	r2, [r5, #-0xc]
     a78: e59d202c     	ldr	r2, [sp, #0x2c]
     a7c: e0814004     	add	r4, r1, r4
     a80: e08c3003     	add	r3, r12, r3
     a84: e0877002     	add	r7, r7, r2
     a88: e15020b4     	ldrh	r2, [r0, #-4]
     a8c: e1520727     	cmp	r2, r7, lsr #14
     a90: 8a000020     	bhi	0xb18 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xb18> @ imm = #0x80
     a94: e59d2014     	ldr	r2, [sp, #0x14]
     a98: e59d8010     	ldr	r8, [sp, #0x10]
     a9c: e59d9098     	ldr	r9, [sp, #0x98]
     aa0: e0042002     	and	r2, r4, r2
     aa4: e0038008     	and	r8, r3, r8
     aa8: e1822008     	orr	r2, r2, r8
     aac: e59d800c     	ldr	r8, [sp, #0xc]
     ab0: e79884a2     	ldr	r8, [r8, r2, lsr #9]
     ab4: e59d2020     	ldr	r2, [sp, #0x20]
     ab8: e082b009     	add	r11, r2, r9
     abc: e59d2004     	ldr	r2, [sp, #0x4]
     ac0: e59d909c     	ldr	r9, [sp, #0x9c]
     ac4: e08bb002     	add	r11, r11, r2
     ac8: e59d2024     	ldr	r2, [sp, #0x24]
     acc: e082a009     	add	r10, r2, r9
     ad0: e59d2028     	ldr	r2, [sp, #0x28]
     ad4: e59d90a0     	ldr	r9, [sp, #0xa0]
     ad8: e08aa006     	add	r10, r10, r6
     adc: e0829009     	add	r9, r2, r9
     ae0: e7e72858     	ubfx	r2, r8, #0x10, #0x8
     ae4: e002029b     	mul	r2, r11, r2
     ae8: e7e7b458     	ubfx	r11, r8, #0x8, #0x8
     aec: e6ef8078     	uxtb	r8, r8
     af0: e089900e     	add	r9, r9, lr
     af4: e00b0b9a     	mul	r11, r10, r11
     af8: e0080998     	mul	r8, r8, r9
     afc: e1a02422     	lsr	r2, r2, #8
     b00: e1a0b82b     	lsr	r11, r11, #16
     b04: e20228ff     	and	r2, r2, #16711680
     b08: e20bbcff     	and	r11, r11, #65280
     b0c: e182200b     	orr	r2, r2, r11
     b10: e1822c28     	orr	r2, r2, r8, lsr #24
     b14: e5052008     	str	r2, [r5, #-0x8]
     b18: e59d2008     	ldr	r2, [sp, #0x8]
     b1c: e0877002     	add	r7, r7, r2
     b20: e15020b2     	ldrh	r2, [r0, #-2]
     b24: e1520727     	cmp	r2, r7, lsr #14
     b28: 8a000022     	bhi	0xbb8 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xbb8> @ imm = #0x88
     b2c: e59d2014     	ldr	r2, [sp, #0x14]
     b30: e0811004     	add	r1, r1, r4
     b34: e08c3003     	add	r3, r12, r3
     b38: e0011002     	and	r1, r1, r2
     b3c: e59d2010     	ldr	r2, [sp, #0x10]
     b40: e0033002     	and	r3, r3, r2
     b44: e1811003     	orr	r1, r1, r3
     b48: e59d300c     	ldr	r3, [sp, #0xc]
     b4c: e79324a1     	ldr	r2, [r3, r1, lsr #9]
     b50: e59d3020     	ldr	r3, [sp, #0x20]
     b54: e59d1094     	ldr	r1, [sp, #0x94]
     b58: e0834001     	add	r4, r3, r1
     b5c: e59d3004     	ldr	r3, [sp, #0x4]
     b60: e59d1090     	ldr	r1, [sp, #0x90]
     b64: e0844003     	add	r4, r4, r3
     b68: e59d3024     	ldr	r3, [sp, #0x24]
     b6c: e083c001     	add	r12, r3, r1
     b70: e59d3028     	ldr	r3, [sp, #0x28]
     b74: e59d108c     	ldr	r1, [sp, #0x8c]
     b78: e08cc006     	add	r12, r12, r6
     b7c: e0831001     	add	r1, r3, r1
     b80: e7e73852     	ubfx	r3, r2, #0x10, #0x8
     b84: e0030394     	mul	r3, r4, r3
     b88: e7e74452     	ubfx	r4, r2, #0x8, #0x8
     b8c: e6ef2072     	uxtb	r2, r2
     b90: e081100e     	add	r1, r1, lr
     b94: e004049c     	mul	r4, r12, r4
     b98: e0020192     	mul	r2, r2, r1
     b9c: e1a03423     	lsr	r3, r3, #8
     ba0: e1a04824     	lsr	r4, r4, #16
     ba4: e20338ff     	and	r3, r3, #16711680
     ba8: e2044cff     	and	r4, r4, #65280
     bac: e1833004     	orr	r3, r3, r4
     bb0: e1833c22     	orr	r3, r3, r2, lsr #24
     bb4: e5053004     	str	r3, [r5, #-0x4]
     bb8: e59d304c     	ldr	r3, [sp, #0x4c]
     bbc: e2800010     	add	r0, r0, #16
     bc0: e59d2044     	ldr	r2, [sp, #0x44]
     bc4: e2855020     	add	r5, r5, #32
     bc8: ee344a2a     	vadd.f32	s8, s8, s21
     bcc: e1530000     	cmp	r3, r0
     bd0: e59d3040     	ldr	r3, [sp, #0x40]
     bd4: ee744a8b     	vadd.f32	s9, s9, s22
     bd8: e0866003     	add	r6, r6, r3
     bdc: e59d3004     	ldr	r3, [sp, #0x4]
     be0: e0833002     	add	r3, r3, r2
     be4: e59d201c     	ldr	r2, [sp, #0x1c]
     be8: e58d3004     	str	r3, [sp, #0x4]
     bec: e59d3030     	ldr	r3, [sp, #0x30]
     bf0: e08ee003     	add	lr, lr, r3
     bf4: e59d3048     	ldr	r3, [sp, #0x48]
     bf8: e0822003     	add	r2, r2, r3
     bfc: e58d201c     	str	r2, [sp, #0x1c]
     c00: e59d2018     	ldr	r2, [sp, #0x18]
     c04: e0823003     	add	r3, r2, r3
     c08: e58d3018     	str	r3, [sp, #0x18]
     c0c: 1afffeac     	bne	0x6c4 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x6c4> @ imm = #-0x550
     c10: e59d30d0     	ldr	r3, [sp, #0xd0]
     c14: e59d80c4     	ldr	r8, [sp, #0xc4]
     c18: e59de03c     	ldr	lr, [sp, #0x3c]
     c1c: e59d90c8     	ldr	r9, [sp, #0xc8]
     c20: e0881003     	add	r1, r8, r3
     c24: e59d3058     	ldr	r3, [sp, #0x58]
     c28: e59d4034     	ldr	r4, [sp, #0x34]
     c2c: e59d5038     	ldr	r5, [sp, #0x38]
     c30: e59d2054     	ldr	r2, [sp, #0x54]
     c34: e083010e     	add	r0, r3, lr, lsl #2
     c38: e59d3050     	ldr	r3, [sp, #0x50]
     c3c: e59db0bc     	ldr	r11, [sp, #0xbc]
     c40: e080008e     	add	r0, r0, lr, lsl #1
     c44: e59dc0c0     	ldr	r12, [sp, #0xc0]
     c48: e0822105     	add	r2, r2, r5, lsl #2
     c4c: e59d70cc     	ldr	r7, [sp, #0xcc]
     c50: e080000e     	add	r0, r0, lr
     c54: e59de030     	ldr	lr, [sp, #0x30]
     c58: e0833104     	add	r3, r3, r4, lsl #2
     c5c: e0822085     	add	r2, r2, r5, lsl #1
     c60: e0833084     	add	r3, r3, r4, lsl #1
     c64: e0822005     	add	r2, r2, r5
     c68: e24cc008     	sub	r12, r12, #8
     c6c: e0876289     	add	r6, r7, r9, lsl #5
     c70: e0833004     	add	r3, r3, r4
     c74: e04cc189     	sub	r12, r12, r9, lsl #3
     c78: e0200e99     	mla	r0, r9, lr, r0
     c7c: e58d0050     	str	r0, [sp, #0x50]
     c80: e59d0044     	ldr	r0, [sp, #0x44]
     c84: e0233099     	mla	r3, r9, r0, r3
     c88: e58d301c     	str	r3, [sp, #0x1c]
     c8c: e59d3040     	ldr	r3, [sp, #0x40]
     c90: e0232399     	mla	r3, r9, r3, r2
     c94: e58d304c     	str	r3, [sp, #0x4c]
     c98: e59d3048     	ldr	r3, [sp, #0x48]
     c9c: e08b0003     	add	r0, r11, r3
     ca0: e0200993     	mla	r0, r3, r9, r0
     ca4: e35c0000     	cmp	r12, #0
     ca8: ba000042     	blt	0xdb8 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xdb8> @ imm = #0x108
     cac: ee244a27     	vmul.f32	s8, s8, s15
     cb0: e3a07000     	mov	r7, #0
     cb4: ee644aa7     	vmul.f32	s9, s9, s15
     cb8: e081a08c     	add	r10, r1, r12, lsl #1
     cbc: eef05a69     	vmov.f32	s11, s19
     cc0: e2411002     	sub	r1, r1, #2
     cc4: eeb06a49     	vmov.f32	s12, s18
     cc8: e1a09007     	mov	r9, r7
     ccc: ee485a44     	vmls.f32	s11, s16, s8
     cd0: e1a08007     	mov	r8, r7
     cd4: ee086a64     	vmls.f32	s12, s16, s9
     cd8: e58db054     	str	r11, [sp, #0x54]
     cdc: ee655aa7     	vmul.f32	s11, s11, s15
     ce0: ee667a27     	vmul.f32	s15, s12, s15
     ce4: eebd6ac4     	vcvt.s32.f32	s12, s8
     ce8: eefd7ae7     	vcvt.s32.f32	s15, s15
     cec: ee165a10     	vmov	r5, s12
     cf0: eebd6ae4     	vcvt.s32.f32	s12, s9
     cf4: edcd7a06     	vstr	s15, [sp, #24]
     cf8: ee164a10     	vmov	r4, s12
     cfc: eebd6ae5     	vcvt.s32.f32	s12, s11
     d00: ed8d6a01     	vstr	s12, [sp, #4]
     d04: e1f130b2     	ldrh	r3, [r1, #2]!
     d08: e1530720     	cmp	r3, r0, lsr #14
     d0c: e59d3008     	ldr	r3, [sp, #0x8]
     d10: e0800003     	add	r0, r0, r3
     d14: 8a000019     	bhi	0xd80 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xd80> @ imm = #0x64
     d18: e59d3014     	ldr	r3, [sp, #0x14]
     d1c: e59d2010     	ldr	r2, [sp, #0x10]
     d20: e0053003     	and	r3, r5, r3
     d24: e0042002     	and	r2, r4, r2
     d28: e1833002     	orr	r3, r3, r2
     d2c: e59d200c     	ldr	r2, [sp, #0xc]
     d30: e792e4a3     	ldr	lr, [r2, r3, lsr #9]
     d34: e59d301c     	ldr	r3, [sp, #0x1c]
     d38: e083b009     	add	r11, r3, r9
     d3c: e59d304c     	ldr	r3, [sp, #0x4c]
     d40: e0832008     	add	r2, r3, r8
     d44: e59d3050     	ldr	r3, [sp, #0x50]
     d48: e083c007     	add	r12, r3, r7
     d4c: e7e7385e     	ubfx	r3, lr, #0x10, #0x8
     d50: e003039b     	mul	r3, r11, r3
     d54: e7e7b45e     	ubfx	r11, lr, #0x8, #0x8
     d58: e6efe07e     	uxtb	lr, lr
     d5c: e0020b92     	mul	r2, r2, r11
     d60: e00c0e9c     	mul	r12, r12, lr
     d64: e1a03423     	lsr	r3, r3, #8
     d68: e1a02822     	lsr	r2, r2, #16
     d6c: e20338ff     	and	r3, r3, #16711680
     d70: e2022cff     	and	r2, r2, #65280
     d74: e1833002     	orr	r3, r3, r2
     d78: e1833c2c     	orr	r3, r3, r12, lsr #24
     d7c: e5863000     	str	r3, [r6]
     d80: e59d3004     	ldr	r3, [sp, #0x4]
     d84: e15a0001     	cmp	r10, r1
     d88: e2866004     	add	r6, r6, #4
     d8c: e0855003     	add	r5, r5, r3
     d90: e59d3018     	ldr	r3, [sp, #0x18]
     d94: e0844003     	add	r4, r4, r3
     d98: e59d3038     	ldr	r3, [sp, #0x38]
     d9c: e0888003     	add	r8, r8, r3
     da0: e59d3034     	ldr	r3, [sp, #0x34]
     da4: e0899003     	add	r9, r9, r3
     da8: e59d303c     	ldr	r3, [sp, #0x3c]
     dac: e0877003     	add	r7, r7, r3
     db0: 1affffd3     	bne	0xd04 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xd04> @ imm = #-0xb4
     db4: e59db054     	ldr	r11, [sp, #0x54]
     db8: e59d30a4     	ldr	r3, [sp, #0xa4]
     dbc: e59d20dc     	ldr	r2, [sp, #0xdc]
     dc0: e0833002     	add	r3, r3, r2
     dc4: e3530000     	cmp	r3, #0
     dc8: e58d30a4     	str	r3, [sp, #0xa4]
     dcc: da000030     	ble	0xe94 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xe94> @ imm = #0xc0
     dd0: e59d30a4     	ldr	r3, [sp, #0xa4]
     dd4: e59d210c     	ldr	r2, [sp, #0x10c]
     dd8: ee766aa2     	vadd.f32	s13, s13, s5
     ddc: e2433801     	sub	r3, r3, #65536
     de0: ee733a83     	vadd.f32	s7, s7, s6
     de4: e58d30a4     	str	r3, [sp, #0xa4]
     de8: e59d30b0     	ldr	r3, [sp, #0xb0]
     dec: e0833002     	add	r3, r3, r2
     df0: e59d2100     	ldr	r2, [sp, #0x100]
     df4: e58d30b0     	str	r3, [sp, #0xb0]
     df8: e59d30fc     	ldr	r3, [sp, #0xfc]
     dfc: e08bb003     	add	r11, r11, r3
     e00: e59d3020     	ldr	r3, [sp, #0x20]
     e04: e0833002     	add	r3, r3, r2
     e08: e59d2104     	ldr	r2, [sp, #0x104]
     e0c: e58d3020     	str	r3, [sp, #0x20]
     e10: e59d3024     	ldr	r3, [sp, #0x24]
     e14: e0833002     	add	r3, r3, r2
     e18: e59d2108     	ldr	r2, [sp, #0x108]
     e1c: e58d3024     	str	r3, [sp, #0x24]
     e20: e59d3028     	ldr	r3, [sp, #0x28]
     e24: e0833002     	add	r3, r3, r2
     e28: e58d3028     	str	r3, [sp, #0x28]
     e2c: e59d30a8     	ldr	r3, [sp, #0xa8]
     e30: e59d20d8     	ldr	r2, [sp, #0xd8]
     e34: e3530000     	cmp	r3, #0
     e38: e59d30ac     	ldr	r3, [sp, #0xac]
     e3c: e0833002     	add	r3, r3, r2
     e40: e59d20b4     	ldr	r2, [sp, #0xb4]
     e44: e58d30ac     	str	r3, [sp, #0xac]
     e48: e59d30d4     	ldr	r3, [sp, #0xd4]
     e4c: e5933020     	ldr	r3, [r3, #0x20]
     e50: e0822103     	add	r2, r2, r3, lsl #2
     e54: e58d20b4     	str	r2, [sp, #0xb4]
     e58: e59d20b8     	ldr	r2, [sp, #0xb8]
     e5c: e0823083     	add	r3, r2, r3, lsl #1
     e60: e58d30b8     	str	r3, [sp, #0xb8]
     e64: 1afffdcd     	bne	0x5a0 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x5a0> @ imm = #-0x8cc
     e68: e59d6130     	ldr	r6, [sp, #0x130]
     e6c: e1a0500b     	mov	r5, r11
     e70: e59d30d8     	ldr	r3, [sp, #0xd8]
     e74: e59db134     	ldr	r11, [sp, #0x134]
     e78: e59d912c     	ldr	r9, [sp, #0x12c]
     e7c: e02bb396     	mla	r11, r6, r3, r11
     e80: e3590001     	cmp	r9, #1
     e84: 1a000020     	bne	0xf0c <ZB_fillTriangleMappingPerspective_DT1_DW0+0xf0c> @ imm = #0x80
     e88: e28ddf51     	add	sp, sp, #324
     e8c: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     e90: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     e94: e59d30b0     	ldr	r3, [sp, #0xb0]
     e98: e59d2120     	ldr	r2, [sp, #0x120]
     e9c: ee766aa1     	vadd.f32	s13, s13, s3
     ea0: ee733a82     	vadd.f32	s7, s7, s4
     ea4: e0833002     	add	r3, r3, r2
     ea8: e59d2114     	ldr	r2, [sp, #0x114]
     eac: e58d30b0     	str	r3, [sp, #0xb0]
     eb0: e59d3110     	ldr	r3, [sp, #0x110]
     eb4: e08bb003     	add	r11, r11, r3
     eb8: e59d3020     	ldr	r3, [sp, #0x20]
     ebc: e0833002     	add	r3, r3, r2
     ec0: e59d2118     	ldr	r2, [sp, #0x118]
     ec4: e58d3020     	str	r3, [sp, #0x20]
     ec8: e59d3024     	ldr	r3, [sp, #0x24]
     ecc: e0833002     	add	r3, r3, r2
     ed0: e59d211c     	ldr	r2, [sp, #0x11c]
     ed4: e58d3024     	str	r3, [sp, #0x24]
     ed8: e59d3028     	ldr	r3, [sp, #0x28]
     edc: e0833002     	add	r3, r3, r2
     ee0: e58d3028     	str	r3, [sp, #0x28]
     ee4: eaffffd0     	b	0xe2c <ZB_fillTriangleMappingPerspective_DT1_DW0+0xe2c> @ imm = #-0xc0
     ee8: e59d3028     	ldr	r3, [sp, #0x28]
     eec: eef04a63     	vmov.f32	s9, s7
     ef0: eeb04a66     	vmov.f32	s8, s13
     ef4: e58d3050     	str	r3, [sp, #0x50]
     ef8: e59d3024     	ldr	r3, [sp, #0x24]
     efc: e58d304c     	str	r3, [sp, #0x4c]
     f00: e59d3020     	ldr	r3, [sp, #0x20]
     f04: e58d301c     	str	r3, [sp, #0x1c]
     f08: eaffff65     	b	0xca4 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xca4> @ imm = #-0x26c
     f0c: e59d0128     	ldr	r0, [sp, #0x128]
     f10: e59d2124     	ldr	r2, [sp, #0x124]
     f14: eef5cac0     	vcmpe.f32	s25, #0
     f18: e5901004     	ldr	r1, [r0, #0x4]
     f1c: e5923004     	ldr	r3, [r2, #0x4]
     f20: eef1fa10     	vmrs	APSR_nzcv, fpscr
     f24: e0411003     	sub	r1, r1, r3
     f28: e2816001     	add	r6, r1, #1
     f2c: da000021     	ble	0xfb8 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xfb8> @ imm = #0x84
     f30: e3510000     	cmp	r1, #0
     f34: e592b000     	ldr	r11, [r2]
     f38: e3a04001     	mov	r4, #1
     f3c: da000022     	ble	0xfcc <ZB_fillTriangleMappingPerspective_DT1_DW0+0xfcc> @ imm = #0x88
     f40: e59d3124     	ldr	r3, [sp, #0x124]
     f44: e58d313c     	str	r3, [sp, #0x13c]
     f48: e59d3128     	ldr	r3, [sp, #0x128]
     f4c: e58d3138     	str	r3, [sp, #0x138]
     f50: e59d3138     	ldr	r3, [sp, #0x138]
     f54: e1a09004     	mov	r9, r4
     f58: ed8d7a07     	vstr	s14, [sp, #28]
     f5c: edcd3a06     	vstr	s7, [sp, #24]
     f60: e5930000     	ldr	r0, [r3]
     f64: edcd6a01     	vstr	s13, [sp, #4]
     f68: e040000b     	sub	r0, r0, r11
     f6c: e1a00800     	lsl	r0, r0, #16
     f70: ebfffffe     	bl	0xf70 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xf70> @ imm = #-0x8
     f74: eddd6a01     	vldr	s13, [sp, #4]
     f78: eddd3a06     	vldr	s7, [sp, #24]
     f7c: ed9d7a07     	vldr	s14, [sp, #28]
     f80: e58d00d8     	str	r0, [sp, #0xd8]
     f84: e1a0b80b     	lsl	r11, r11, #16
     f88: eafffd64     	b	0x520 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x520> @ imm = #-0xa70
     f8c: e3a03000     	mov	r3, #0
     f90: e3a02001     	mov	r2, #1
     f94: ed9f7a27     	vldr	s14, [pc, #156]         @ 0x1038 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x1038>
     f98: e58d30dc     	str	r3, [sp, #0xdc]
     f9c: ed8dfa47     	vstr	s30, [sp, #284]
     fa0: edcdea46     	vstr	s29, [sp, #280]
     fa4: ed8dea45     	vstr	s28, [sp, #276]
     fa8: edcdda44     	vstr	s27, [sp, #272]
     fac: e58d210c     	str	r2, [sp, #0x10c]
     fb0: e58d3120     	str	r3, [sp, #0x120]
     fb4: eafffd4c     	b	0x4ec <ZB_fillTriangleMappingPerspective_DT1_DW0+0x4ec> @ imm = #-0xad0
     fb8: e58d90a4     	str	r9, [sp, #0xa4]
     fbc: e3a09001     	mov	r9, #1
     fc0: e59d0128     	ldr	r0, [sp, #0x128]
     fc4: e59d4124     	ldr	r4, [sp, #0x124]
     fc8: eafffd27     	b	0x46c <ZB_fillTriangleMappingPerspective_DT1_DW0+0x46c> @ imm = #-0xb64
     fcc: e3560000     	cmp	r6, #0
     fd0: daffffac     	ble	0xe88 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xe88> @ imm = #-0x150
     fd4: e58d90d8     	str	r9, [sp, #0xd8]
     fd8: e1a0b80b     	lsl	r11, r11, #16
     fdc: e1a09004     	mov	r9, r4
     fe0: e58d0138     	str	r0, [sp, #0x138]
     fe4: e58d213c     	str	r2, [sp, #0x13c]
     fe8: eafffd4e     	b	0x528 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x528> @ imm = #-0xac8
     fec: e59d3128     	ldr	r3, [sp, #0x128]
     ff0: e59d0124     	ldr	r0, [sp, #0x124]
     ff4: e58d3138     	str	r3, [sp, #0x138]
     ff8: eafffd04     	b	0x410 <ZB_fillTriangleMappingPerspective_DT1_DW0+0x410> @ imm = #-0xbf0
     ffc: e59d3138     	ldr	r3, [sp, #0x138]
    1000: e59d213c     	ldr	r2, [sp, #0x13c]
    1004: e5931004     	ldr	r1, [r3, #0x4]
    1008: e5923004     	ldr	r3, [r2, #0x4]
    100c: e592b000     	ldr	r11, [r2]
    1010: e0411003     	sub	r1, r1, r3
    1014: e3510000     	cmp	r1, #0
    1018: c3a03000     	movgt	r3, #0
    101c: c1a04009     	movgt	r4, r9
    1020: c58d30a4     	strgt	r3, [sp, #0xa4]
    1024: caffffc9     	bgt	0xf50 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xf50> @ imm = #-0xdc
    1028: e3a03000     	mov	r3, #0
    102c: e58d30a4     	str	r3, [sp, #0xa4]
    1030: e58d30d8     	str	r3, [sp, #0xd8]
    1034: eaffffd2     	b	0xf84 <ZB_fillTriangleMappingPerspective_DT1_DW0+0xf84> @ imm = #-0xb8
    1038: 00 00 00 00  	.word	0x00000000

Disassembly of section .text.ZB_fillTriangleMappingPerspective_DT1_DW1:

00000000 <ZB_fillTriangleMappingPerspective_DT1_DW1>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a04003     	mov	r4, r3
       8: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
       c: e24ddf51     	sub	sp, sp, #324
      10: e592c004     	ldr	r12, [r2, #0x4]
      14: e58d3128     	str	r3, [sp, #0x128]
      18: e5913004     	ldr	r3, [r1, #0x4]
      1c: e58d00d8     	str	r0, [sp, #0xd8]
      20: e5940004     	ldr	r0, [r4, #0x4]
      24: e15c0003     	cmp	r12, r3
      28: a1a0e003     	movge	lr, r3
      2c: b1a0e00c     	movlt	lr, r12
      30: a1a0300c     	movge	r3, r12
      34: b1a05002     	movlt	r5, r2
      38: a1a05001     	movge	r5, r1
      3c: b1a0b001     	movlt	r11, r1
      40: a1a0b002     	movge	r11, r2
      44: e150000e     	cmp	r0, lr
      48: e1a0c00e     	mov	r12, lr
      4c: e58d5124     	str	r5, [sp, #0x124]
      50: ba000007     	blt	0x74 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x74> @ imm = #0x1c
      54: e1500003     	cmp	r0, r3
      58: aa0003fb     	bge	0x104c <ZB_fillTriangleMappingPerspective_DT1_DW1+0x104c> @ imm = #0xfec
      5c: e59d2128     	ldr	r2, [sp, #0x128]
      60: e1a0c000     	mov	r12, r0
      64: e59d1124     	ldr	r1, [sp, #0x124]
      68: e1a0000e     	mov	r0, lr
      6c: e58d2124     	str	r2, [sp, #0x124]
      70: e58d1128     	str	r1, [sp, #0x128]
      74: e0433000     	sub	r3, r3, r0
      78: e04c2000     	sub	r2, r12, r0
      7c: e59d0128     	ldr	r0, [sp, #0x128]
      80: ee073a90     	vmov	s15, r3
      84: e59d3128     	ldr	r3, [sp, #0x128]
      88: ee072a10     	vmov	s14, r2
      8c: eef86ae7     	vcvt.f32.s32	s13, s15
      90: e5932000     	ldr	r2, [r3]
      94: e59b3000     	ldr	r3, [r11]
      98: eeb87ac7     	vcvt.f32.s32	s14, s14
      9c: e59dc124     	ldr	r12, [sp, #0x124]
      a0: e0433002     	sub	r3, r3, r2
      a4: eeb2aa00     	vmov.f32	s20, #8.000000e+00
      a8: ee073a90     	vmov	s15, r3
      ac: e59d3124     	ldr	r3, [sp, #0x124]
      b0: eef87ae7     	vcvt.f32.s32	s15, s15
      b4: e5933000     	ldr	r3, [r3]
      b8: ee67ca27     	vmul.f32	s25, s14, s15
      bc: e0433002     	sub	r3, r3, r2
      c0: e5902008     	ldr	r2, [r0, #0x8]
      c4: ee053a10     	vmov	s10, r3
      c8: e590300c     	ldr	r3, [r0, #0xc]
      cc: eeb85ac5     	vcvt.f32.s32	s10, s10
      d0: ee053a90     	vmov	s11, r3
      d4: ee55ca26     	vnmls.f32	s25, s10, s13
      d8: e5903010     	ldr	r3, [r0, #0x10]
      dc: eef82ae5     	vcvt.f32.s32	s5, s11
      e0: ee053a90     	vmov	s11, r3
      e4: e59c3008     	ldr	r3, [r12, #0x8]
      e8: eeb83ae5     	vcvt.f32.s32	s6, s11
      ec: eef5ca40     	vcmp.f32	s25, #0
      f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
      f4: e0433002     	sub	r3, r3, r2
      f8: 1eb76a00     	vmovne.f32	s12, #1.000000e+00
      fc: 1ec6ca2c     	vdivne.f32	s25, s12, s25
     100: ee062a10     	vmov	s12, r2
     104: eeb86ac6     	vcvt.f32.s32	s12, s12
     108: ee622a86     	vmul.f32	s5, s5, s12
     10c: ee233a06     	vmul.f32	s6, s6, s12
     110: ee063a10     	vmov	s12, r3
     114: e59b3008     	ldr	r3, [r11, #0x8]
     118: eeb86ac6     	vcvt.f32.s32	s12, s12
     11c: e0433002     	sub	r3, r3, r2
     120: e5902014     	ldr	r2, [r0, #0x14]
     124: ee6c7aa7     	vmul.f32	s15, s25, s15
     128: ee053a90     	vmov	s11, r3
     12c: ee2c5a85     	vmul.f32	s10, s25, s10
     130: eef85ae5     	vcvt.f32.s32	s11, s11
     134: ee674a86     	vmul.f32	s9, s15, s12
     138: ee2c7a87     	vmul.f32	s14, s25, s14
     13c: ee6c6aa6     	vmul.f32	s13, s25, s13
     140: e59c3014     	ldr	r3, [r12, #0x14]
     144: ee554a25     	vnmls.f32	s9, s10, s11
     148: ee673a25     	vmul.f32	s7, s14, s11
     14c: e0433002     	sub	r3, r3, r2
     150: eef5cac0     	vcmpe.f32	s25, #0
     154: eef1fa10     	vmrs	APSR_nzcv, fpscr
     158: ee563a86     	vnmls.f32	s7, s13, s12
     15c: eebd6ae4     	vcvt.s32.f32	s12, s9
     160: eef0da46     	vmov.f32	s27, s12
     164: ee063a10     	vmov	s12, r3
     168: e59b3014     	ldr	r3, [r11, #0x14]
     16c: eeb86ac6     	vcvt.f32.s32	s12, s12
     170: e0433002     	sub	r3, r3, r2
     174: e5902018     	ldr	r2, [r0, #0x18]
     178: ee053a90     	vmov	s11, r3
     17c: ee674a86     	vmul.f32	s9, s15, s12
     180: eef85ae5     	vcvt.f32.s32	s11, s11
     184: e59c3018     	ldr	r3, [r12, #0x18]
     188: ee554a25     	vnmls.f32	s9, s10, s11
     18c: e0433002     	sub	r3, r3, r2
     190: ee274a25     	vmul.f32	s8, s14, s11
     194: ee164a86     	vnmls.f32	s8, s13, s12
     198: eebd6ae4     	vcvt.s32.f32	s12, s9
     19c: eeb0ea46     	vmov.f32	s28, s12
     1a0: ee063a10     	vmov	s12, r3
     1a4: e59b3018     	ldr	r3, [r11, #0x18]
     1a8: eeb86ac6     	vcvt.f32.s32	s12, s12
     1ac: e0433002     	sub	r3, r3, r2
     1b0: e590201c     	ldr	r2, [r0, #0x1c]
     1b4: ee053a90     	vmov	s11, r3
     1b8: ee272a86     	vmul.f32	s4, s15, s12
     1bc: eef85ae5     	vcvt.f32.s32	s11, s11
     1c0: e59c301c     	ldr	r3, [r12, #0x1c]
     1c4: ee152a25     	vnmls.f32	s4, s10, s11
     1c8: e0433002     	sub	r3, r3, r2
     1cc: ee674a25     	vmul.f32	s9, s14, s11
     1d0: ee564a86     	vnmls.f32	s9, s13, s12
     1d4: eebd6ac2     	vcvt.s32.f32	s12, s4
     1d8: eef0ea46     	vmov.f32	s29, s12
     1dc: ee063a10     	vmov	s12, r3
     1e0: e59b301c     	ldr	r3, [r11, #0x1c]
     1e4: eeb86ac6     	vcvt.f32.s32	s12, s12
     1e8: edc02a08     	vstr	s5, [r0, #32]
     1ec: e0433002     	sub	r3, r3, r2
     1f0: ed803a09     	vstr	s6, [r0, #36]
     1f4: ee053a90     	vmov	s11, r3
     1f8: ee272a86     	vmul.f32	s4, s15, s12
     1fc: eef85ae5     	vcvt.f32.s32	s11, s11
     200: e59c3008     	ldr	r3, [r12, #0x8]
     204: eef02a42     	vmov.f32	s5, s4
     208: ee273a25     	vmul.f32	s6, s14, s11
     20c: ee552a25     	vnmls.f32	s5, s10, s11
     210: eef05a43     	vmov.f32	s11, s6
     214: ee565a86     	vnmls.f32	s11, s13, s12
     218: eebd6ae2     	vcvt.s32.f32	s12, s5
     21c: eeb0fa46     	vmov.f32	s30, s12
     220: eebd6ae5     	vcvt.s32.f32	s12, s11
     224: ed8d6a0e     	vstr	s12, [sp, #56]
     228: ee063a10     	vmov	s12, r3
     22c: e59c300c     	ldr	r3, [r12, #0xc]
     230: eef85ac6     	vcvt.f32.s32	s11, s12
     234: ee063a10     	vmov	s12, r3
     238: e59c3010     	ldr	r3, [r12, #0x10]
     23c: eeb86ac6     	vcvt.f32.s32	s12, s12
     240: e58d3000     	str	r3, [sp]
     244: ee266a25     	vmul.f32	s12, s12, s11
     248: ed8c6a08     	vstr	s12, [r12, #32]
     24c: ee063a10     	vmov	s12, r3
     250: eeb86ac6     	vcvt.f32.s32	s12, s12
     254: ee266a25     	vmul.f32	s12, s12, s11
     258: ed8c6a09     	vstr	s12, [r12, #36]
     25c: ed9b6a02     	vldr	s12, [r11, #8]
     260: eeb83ac6     	vcvt.f32.s32	s6, s12
     264: ed9b6a03     	vldr	s12, [r11, #12]
     268: eef85ac6     	vcvt.f32.s32	s11, s12
     26c: ee655a83     	vmul.f32	s11, s11, s6
     270: edcb5a08     	vstr	s11, [r11, #32]
     274: ed9b6a04     	vldr	s12, [r11, #16]
     278: e59de0d8     	ldr	lr, [sp, #0xd8]
     27c: eeb86ac6     	vcvt.f32.s32	s12, s12
     280: ee266a03     	vmul.f32	s12, s12, s6
     284: ed8b6a09     	vstr	s12, [r11, #36]
     288: edd01a08     	vldr	s3, [r0, #32]
     28c: ed902a09     	vldr	s4, [r0, #36]
     290: eddc2a08     	vldr	s5, [r12, #32]
     294: ed9c3a09     	vldr	s6, [r12, #36]
     298: e5901004     	ldr	r1, [r0, #0x4]
     29c: e59e3020     	ldr	r3, [lr, #0x20]
     2a0: ee722ae1     	vsub.f32	s5, s5, s3
     2a4: ee333a42     	vsub.f32	s6, s6, s4
     2a8: e59e2004     	ldr	r2, [lr, #0x4]
     2ac: e0030193     	mul	r3, r3, r1
     2b0: e59d0038     	ldr	r0, [sp, #0x38]
     2b4: ee27daa2     	vmul.f32	s26, s15, s5
     2b8: ee27ca83     	vmul.f32	s24, s15, s6
     2bc: eefd7ae3     	vcvt.s32.f32	s15, s7
     2c0: e0822103     	add	r2, r2, r3, lsl #2
     2c4: e1a06080     	lsl	r6, r0, #1
     2c8: e1a0a180     	lsl	r10, r0, #3
     2cc: ee755ae1     	vsub.f32	s11, s11, s3
     2d0: e58d20b0     	str	r2, [sp, #0xb0]
     2d4: e59e2000     	ldr	r2, [lr]
     2d8: ee178a90     	vmov	r8, s15
     2dc: eeb88ae7     	vcvt.f32.s32	s16, s15
     2e0: eefd7ac4     	vcvt.s32.f32	s15, s8
     2e4: e0823083     	add	r3, r2, r3, lsl #1
     2e8: e58d60ec     	str	r6, [sp, #0xec]
     2ec: e0866000     	add	r6, r6, r0
     2f0: e58da044     	str	r10, [sp, #0x44]
     2f4: e58d30b4     	str	r3, [sp, #0xb4]
     2f8: edcd7a0c     	vstr	s15, [sp, #48]
     2fc: e59d3030     	ldr	r3, [sp, #0x30]
     300: eefd7ae4     	vcvt.s32.f32	s15, s9
     304: e58d60e4     	str	r6, [sp, #0xe4]
     308: e59e6008     	ldr	r6, [lr, #0x8]
     30c: e1a0e188     	lsl	lr, r8, #3
     310: e1a04083     	lsl	r4, r3, #1
     314: e1a09183     	lsl	r9, r3, #3
     318: ee172a90     	vmov	r2, s15
     31c: e58d40e0     	str	r4, [sp, #0xe0]
     320: e1a04103     	lsl	r4, r3, #2
     324: e1a05082     	lsl	r5, r2, #1
     328: e1a07182     	lsl	r7, r2, #3
     32c: e58d407c     	str	r4, [sp, #0x7c]
     330: e1a04102     	lsl	r4, r2, #2
     334: e58de048     	str	lr, [sp, #0x48]
     338: e088e108     	add	lr, r8, r8, lsl #2
     33c: e58d4078     	str	r4, [sp, #0x78]
     340: e1a04100     	lsl	r4, r0, #2
     344: e58d50f0     	str	r5, [sp, #0xf0]
     348: e0855002     	add	r5, r5, r2
     34c: e58d4074     	str	r4, [sp, #0x74]
     350: e59d40e0     	ldr	r4, [sp, #0xe0]
     354: e58d50e8     	str	r5, [sp, #0xe8]
     358: edcd7a0d     	vstr	s15, [sp, #52]
     35c: e0844003     	add	r4, r4, r3
     360: e58d703c     	str	r7, [sp, #0x3c]
     364: e58d9040     	str	r9, [sp, #0x40]
     368: e58d40f4     	str	r4, [sp, #0xf4]
     36c: e58de0f8     	str	lr, [sp, #0xf8]
     370: e1a0e084     	lsl	lr, r4, #1
     374: e59d50e8     	ldr	r5, [sp, #0xe8]
     378: ee366a42     	vsub.f32	s12, s12, s4
     37c: ee679a25     	vmul.f32	s19, s14, s11
     380: e58de098     	str	lr, [sp, #0x98]
     384: e1a05085     	lsl	r5, r5, #1
     388: e59de07c     	ldr	lr, [sp, #0x7c]
     38c: ee279a06     	vmul.f32	s18, s14, s12
     390: e58d509c     	str	r5, [sp, #0x9c]
     394: e59d50e4     	ldr	r5, [sp, #0xe4]
     398: ee569aa2     	vnmls.f32	s19, s13, s5
     39c: ee169a83     	vnmls.f32	s18, s13, s6
     3a0: ee15da25     	vnmls.f32	s26, s10, s11
     3a4: e1a05085     	lsl	r5, r5, #1
     3a8: ee15ca06     	vnmls.f32	s24, s10, s12
     3ac: e58d50a0     	str	r5, [sp, #0xa0]
     3b0: e08e5003     	add	r5, lr, r3
     3b4: e59de078     	ldr	lr, [sp, #0x78]
     3b8: e0493003     	sub	r3, r9, r3
     3bc: e58d5088     	str	r5, [sp, #0x88]
     3c0: e58d3094     	str	r3, [sp, #0x94]
     3c4: e0473002     	sub	r3, r7, r2
     3c8: e08e5002     	add	r5, lr, r2
     3cc: e59de074     	ldr	lr, [sp, #0x74]
     3d0: e58d3090     	str	r3, [sp, #0x90]
     3d4: e04a3000     	sub	r3, r10, r0
     3d8: ee69aa8a     	vmul.f32	s21, s19, s20
     3dc: e08ee000     	add	lr, lr, r0
     3e0: ee29ba0a     	vmul.f32	s22, s18, s20
     3e4: e58d5084     	str	r5, [sp, #0x84]
     3e8: ee28aa0a     	vmul.f32	s20, s16, s20
     3ec: e58de080     	str	lr, [sp, #0x80]
     3f0: e58d308c     	str	r3, [sp, #0x8c]
     3f4: da00031b     	ble	0x1068 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x1068> @ imm = #0xc6c
     3f8: e1a0000b     	mov	r0, r11
     3fc: e58dc13c     	str	r12, [sp, #0x13c]
     400: e59d3124     	ldr	r3, [sp, #0x124]
     404: e3a09000     	mov	r9, #0
     408: e59d4128     	ldr	r4, [sp, #0x128]
     40c: e1a02009     	mov	r2, r9
     410: e58d600c     	str	r6, [sp, #0xc]
     414: e1a07009     	mov	r7, r9
     418: e5933004     	ldr	r3, [r3, #0x4]
     41c: e1a0a008     	mov	r10, r8
     420: eef7ba00     	vmov.f32	s23, #1.000000e+00
     424: e34027f8     	movt	r2, #0x7f8
     428: eef08a00     	vmov.f32	s17, #2.000000e+00
     42c: e3a0e001     	mov	lr, #1
     430: e043c001     	sub	r12, r3, r1
     434: e5903004     	ldr	r3, [r0, #0x4]
     438: e1a0600c     	mov	r6, r12
     43c: e58d90d4     	str	r9, [sp, #0xd4]
     440: e58de0a4     	str	lr, [sp, #0xa4]
     444: e0431001     	sub	r1, r3, r1
     448: e3a03b3e     	mov	r3, #63488
     44c: e3403007     	movt	r3, #0x7
     450: e58d2010     	str	r2, [sp, #0x10]
     454: e58d3014     	str	r3, [sp, #0x14]
     458: e5942000     	ldr	r2, [r4]
     45c: e3510000     	cmp	r1, #0
     460: e1a08002     	mov	r8, r2
     464: da0002e0     	ble	0xfec <ZB_fillTriangleMappingPerspective_DT1_DW1+0xfec> @ imm = #0xb80
     468: e5903000     	ldr	r3, [r0]
     46c: e0433002     	sub	r3, r3, r2
     470: e1a00803     	lsl	r0, r3, #16
     474: ebfffffe     	bl	0x474 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x474> @ imm = #-0x8
     478: e6ff2070     	uxth	r2, r0
     47c: e1a03840     	asr	r3, r0, #16
     480: e59d1030     	ldr	r1, [sp, #0x30]
     484: e58d20dc     	str	r2, [sp, #0xdc]
     488: ee1d2a90     	vmov	r2, s27
     48c: ee073a90     	vmov	s15, r3
     490: e0222a93     	mla	r2, r3, r10, r2
     494: eeb87ae7     	vcvt.f32.s32	s14, s15
     498: e58d3120     	str	r3, [sp, #0x120]
     49c: e58d2110     	str	r2, [sp, #0x110]
     4a0: ee1e2a10     	vmov	r2, s28
     4a4: e0222193     	mla	r2, r3, r1, r2
     4a8: e59d1034     	ldr	r1, [sp, #0x34]
     4ac: e58d2114     	str	r2, [sp, #0x114]
     4b0: ee1e2a90     	vmov	r2, s29
     4b4: e0222193     	mla	r2, r3, r1, r2
     4b8: e59d1038     	ldr	r1, [sp, #0x38]
     4bc: e58d2118     	str	r2, [sp, #0x118]
     4c0: ee1f2a10     	vmov	r2, s30
     4c4: e0222193     	mla	r2, r3, r1, r2
     4c8: e2833001     	add	r3, r3, #1
     4cc: e58d310c     	str	r3, [sp, #0x10c]
     4d0: e58d211c     	str	r2, [sp, #0x11c]
     4d4: e59d30a4     	ldr	r3, [sp, #0xa4]
     4d8: edd46a08     	vldr	s13, [r4, #32]
     4dc: edd43a09     	vldr	s7, [r4, #36]
     4e0: e3530000     	cmp	r3, #0
     4e4: e5943008     	ldr	r3, [r4, #0x8]
     4e8: e58d3000     	str	r3, [sp]
     4ec: e5943014     	ldr	r3, [r4, #0x14]
     4f0: e58d3020     	str	r3, [sp, #0x20]
     4f4: e5943018     	ldr	r3, [r4, #0x18]
     4f8: e58d3024     	str	r3, [sp, #0x24]
     4fc: e594301c     	ldr	r3, [r4, #0x1c]
     500: e58d3028     	str	r3, [sp, #0x28]
     504: 1a0002da     	bne	0x1074 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x1074> @ imm = #0xb68
     508: e3560000     	cmp	r6, #0
     50c: da000277     	ble	0xef0 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xef0> @ imm = #0x9dc
     510: e1a0308a     	lsl	r3, r10, #1
     514: eef01a4d     	vmov.f32	s3, s26
     518: eeb02a4c     	vmov.f32	s4, s24
     51c: e58d302c     	str	r3, [sp, #0x2c]
     520: e59d3110     	ldr	r3, [sp, #0x110]
     524: e59d2114     	ldr	r2, [sp, #0x114]
     528: ee491a87     	vmla.f32	s3, s19, s14
     52c: e08a3003     	add	r3, r10, r3
     530: ee092a07     	vmla.f32	s4, s18, s14
     534: e58d30fc     	str	r3, [sp, #0xfc]
     538: e59d3030     	ldr	r3, [sp, #0x30]
     53c: e0833002     	add	r3, r3, r2
     540: e59d2118     	ldr	r2, [sp, #0x118]
     544: e58d3100     	str	r3, [sp, #0x100]
     548: e59d3034     	ldr	r3, [sp, #0x34]
     54c: e58db138     	str	r11, [sp, #0x138]
     550: ee792aa1     	vadd.f32	s5, s19, s3
     554: e0833002     	add	r3, r3, r2
     558: ee393a02     	vadd.f32	s6, s18, s4
     55c: e59d211c     	ldr	r2, [sp, #0x11c]
     560: e58d3104     	str	r3, [sp, #0x104]
     564: e59db000     	ldr	r11, [sp]
     568: e59d3038     	ldr	r3, [sp, #0x38]
     56c: e58da008     	str	r10, [sp, #0x8]
     570: e1a0a008     	mov	r10, r8
     574: e58d60a8     	str	r6, [sp, #0xa8]
     578: e0833002     	add	r3, r3, r2
     57c: e58d70ac     	str	r7, [sp, #0xac]
     580: e58d3108     	str	r3, [sp, #0x108]
     584: e58d912c     	str	r9, [sp, #0x12c]
     588: e58d6130     	str	r6, [sp, #0x130]
     58c: e58d7134     	str	r7, [sp, #0x134]
     590: e59d30a8     	ldr	r3, [sp, #0xa8]
     594: e1a0000b     	mov	r0, r11
     598: ee07ba90     	vmov	s15, r11
     59c: eeb86ae7     	vcvt.f32.s32	s12, s15
     5a0: e2433001     	sub	r3, r3, #1
     5a4: e58d30a8     	str	r3, [sp, #0xa8]
     5a8: e59d30ac     	ldr	r3, [sp, #0xac]
     5ac: eecb7a86     	vdiv.f32	s15, s23, s12
     5b0: e06ac843     	rsb	r12, r10, r3, asr #16
     5b4: e59d30b0     	ldr	r3, [sp, #0xb0]
     5b8: e35c0006     	cmp	r12, #6
     5bc: e083610a     	add	r6, r3, r10, lsl #2
     5c0: e59d30b4     	ldr	r3, [sp, #0xb4]
     5c4: e083108a     	add	r1, r3, r10, lsl #1
     5c8: da000260     	ble	0xf50 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xf50> @ imm = #0x980
     5cc: e24c3007     	sub	r3, r12, #7
     5d0: e59d8034     	ldr	r8, [sp, #0x34]
     5d4: e1a051a3     	lsr	r5, r3, #3
     5d8: e2810010     	add	r0, r1, #16
     5dc: e2811020     	add	r1, r1, #32
     5e0: e59d9038     	ldr	r9, [sp, #0x38]
     5e4: e1a02205     	lsl	r2, r5, #4
     5e8: e2867020     	add	r7, r6, #32
     5ec: e59d6030     	ldr	r6, [sp, #0x30]
     5f0: e1a04007     	mov	r4, r7
     5f4: e0813002     	add	r3, r1, r2
     5f8: eef04a63     	vmov.f32	s9, s7
     5fc: e58d304c     	str	r3, [sp, #0x4c]
     600: e1a01000     	mov	r1, r0
     604: e59d30f8     	ldr	r3, [sp, #0xf8]
     608: e3a0e000     	mov	lr, #0
     60c: eeb04a66     	vmov.f32	s8, s13
     610: e58de000     	str	lr, [sp]
     614: e08b3003     	add	r3, r11, r3
     618: e58de004     	str	lr, [sp, #0x4]
     61c: e58d301c     	str	r3, [sp, #0x1c]
     620: e59d3008     	ldr	r3, [sp, #0x8]
     624: e58da0b8     	str	r10, [sp, #0xb8]
     628: e58db0bc     	str	r11, [sp, #0xbc]
     62c: e08b3003     	add	r3, r11, r3
     630: e58dc0c0     	str	r12, [sp, #0xc0]
     634: e58d3018     	str	r3, [sp, #0x18]
     638: e59d3020     	ldr	r3, [sp, #0x20]
     63c: e58d20c4     	str	r2, [sp, #0xc4]
     640: e0836006     	add	r6, r3, r6
     644: e58d6050     	str	r6, [sp, #0x50]
     648: e59d6024     	ldr	r6, [sp, #0x24]
     64c: e0868008     	add	r8, r6, r8
     650: e58d8054     	str	r8, [sp, #0x54]
     654: e59d8028     	ldr	r8, [sp, #0x28]
     658: e0889009     	add	r9, r8, r9
     65c: e58d9058     	str	r9, [sp, #0x58]
     660: e59d90e0     	ldr	r9, [sp, #0xe0]
     664: e0839009     	add	r9, r3, r9
     668: e58d905c     	str	r9, [sp, #0x5c]
     66c: e59d90f0     	ldr	r9, [sp, #0xf0]
     670: e0869009     	add	r9, r6, r9
     674: e58d9060     	str	r9, [sp, #0x60]
     678: e59d90ec     	ldr	r9, [sp, #0xec]
     67c: e0889009     	add	r9, r8, r9
     680: e58d9064     	str	r9, [sp, #0x64]
     684: e59d90f4     	ldr	r9, [sp, #0xf4]
     688: e0833009     	add	r3, r3, r9
     68c: e58d3068     	str	r3, [sp, #0x68]
     690: e59d30e8     	ldr	r3, [sp, #0xe8]
     694: e0863003     	add	r3, r6, r3
     698: e58d306c     	str	r3, [sp, #0x6c]
     69c: e59d30e4     	ldr	r3, [sp, #0xe4]
     6a0: e0883003     	add	r3, r8, r3
     6a4: e58d3070     	str	r3, [sp, #0x70]
     6a8: e58d50c8     	str	r5, [sp, #0xc8]
     6ac: e58d70cc     	str	r7, [sp, #0xcc]
     6b0: e58d00d0     	str	r0, [sp, #0xd0]
     6b4: ee366a0a     	vadd.f32	s12, s12, s20
     6b8: ee640a27     	vmul.f32	s1, s8, s15
     6bc: ee240aa7     	vmul.f32	s0, s9, s15
     6c0: eeb01a68     	vmov.f32	s2, s17
     6c4: eeb05a69     	vmov.f32	s10, s19
     6c8: ee061a67     	vmls.f32	s2, s12, s15
     6cc: eef05a49     	vmov.f32	s11, s18
     6d0: ee085a60     	vmls.f32	s10, s16, s1
     6d4: ee485a40     	vmls.f32	s11, s16, s0
     6d8: e59d0008     	ldr	r0, [sp, #0x8]
     6dc: ee211a27     	vmul.f32	s2, s2, s15
     6e0: e59d2018     	ldr	r2, [sp, #0x18]
     6e4: ee255a27     	vmul.f32	s10, s10, s15
     6e8: ee655aa7     	vmul.f32	s11, s11, s15
     6ec: eef07a68     	vmov.f32	s15, s17
     6f0: e0422000     	sub	r2, r2, r0
     6f4: e15131b0     	ldrh	r3, [r1, #-16]
     6f8: e1a0a722     	lsr	r10, r2, #14
     6fc: ee467a41     	vmls.f32	s15, s12, s2
     700: eebd5ac5     	vcvt.s32.f32	s10, s10
     704: eefd5ae5     	vcvt.s32.f32	s11, s11
     708: e153000a     	cmp	r3, r10
     70c: ee150a10     	vmov	r0, s10
     710: ee15ca90     	vmov	r12, s11
     714: ee677a81     	vmul.f32	s15, s15, s2
     718: eebd1ae0     	vcvt.s32.f32	s2, s1
     71c: ee116a10     	vmov	r6, s2
     720: eebd1ac0     	vcvt.s32.f32	s2, s0
     724: ee113a10     	vmov	r3, s2
     728: 8a00001c     	bhi	0x7a0 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x7a0> @ imm = #0x70
     72c: e59d5010     	ldr	r5, [sp, #0x10]
     730: e59d7014     	ldr	r7, [sp, #0x14]
     734: e0035005     	and	r5, r3, r5
     738: e0067007     	and	r7, r6, r7
     73c: e1855007     	orr	r5, r5, r7
     740: e59d700c     	ldr	r7, [sp, #0xc]
     744: e79794a5     	ldr	r9, [r7, r5, lsr #9]
     748: e59d5020     	ldr	r5, [sp, #0x20]
     74c: e59d7000     	ldr	r7, [sp]
     750: e085b007     	add	r11, r5, r7
     754: e59d5024     	ldr	r5, [sp, #0x24]
     758: e59d7004     	ldr	r7, [sp, #0x4]
     75c: e0857007     	add	r7, r5, r7
     760: e59d5028     	ldr	r5, [sp, #0x28]
     764: e085800e     	add	r8, r5, lr
     768: e7e75859     	ubfx	r5, r9, #0x10, #0x8
     76c: e005059b     	mul	r5, r11, r5
     770: e7e7b459     	ubfx	r11, r9, #0x8, #0x8
     774: e6ef9079     	uxtb	r9, r9
     778: e0070b97     	mul	r7, r7, r11
     77c: e0080998     	mul	r8, r8, r9
     780: e1a05425     	lsr	r5, r5, #8
     784: e1a07827     	lsr	r7, r7, #16
     788: e20558ff     	and	r5, r5, #16711680
     78c: e2077cff     	and	r7, r7, #65280
     790: e1855007     	orr	r5, r5, r7
     794: e1855c28     	orr	r5, r5, r8, lsr #24
     798: e5045020     	str	r5, [r4, #-0x20]
     79c: e141a1b0     	strh	r10, [r1, #-16]
     7a0: e59d7018     	ldr	r7, [sp, #0x18]
     7a4: e0806006     	add	r6, r0, r6
     7a8: e15150be     	ldrh	r5, [r1, #-14]
     7ac: e08c3003     	add	r3, r12, r3
     7b0: e1a0a727     	lsr	r10, r7, #14
     7b4: e155000a     	cmp	r5, r10
     7b8: 8a00001c     	bhi	0x830 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x830> @ imm = #0x70
     7bc: e59d5014     	ldr	r5, [sp, #0x14]
     7c0: e59d7010     	ldr	r7, [sp, #0x10]
     7c4: e0065005     	and	r5, r6, r5
     7c8: e0037007     	and	r7, r3, r7
     7cc: e1855007     	orr	r5, r5, r7
     7d0: e59d700c     	ldr	r7, [sp, #0xc]
     7d4: e79794a5     	ldr	r9, [r7, r5, lsr #9]
     7d8: e59d5050     	ldr	r5, [sp, #0x50]
     7dc: e59d7000     	ldr	r7, [sp]
     7e0: e085b007     	add	r11, r5, r7
     7e4: e59d5054     	ldr	r5, [sp, #0x54]
     7e8: e59d7004     	ldr	r7, [sp, #0x4]
     7ec: e0857007     	add	r7, r5, r7
     7f0: e59d5058     	ldr	r5, [sp, #0x58]
     7f4: e085800e     	add	r8, r5, lr
     7f8: e7e75859     	ubfx	r5, r9, #0x10, #0x8
     7fc: e005059b     	mul	r5, r11, r5
     800: e7e7b459     	ubfx	r11, r9, #0x8, #0x8
     804: e6ef9079     	uxtb	r9, r9
     808: e0070b97     	mul	r7, r7, r11
     80c: e0080899     	mul	r8, r9, r8
     810: e1a05425     	lsr	r5, r5, #8
     814: e1a07827     	lsr	r7, r7, #16
     818: e20558ff     	and	r5, r5, #16711680
     81c: e2077cff     	and	r7, r7, #65280
     820: e1855007     	orr	r5, r5, r7
     824: e1855c28     	orr	r5, r5, r8, lsr #24
     828: e504501c     	str	r5, [r4, #-0x1c]
     82c: e141a0be     	strh	r10, [r1, #-14]
     830: e59d502c     	ldr	r5, [sp, #0x2c]
     834: e0806006     	add	r6, r0, r6
     838: e08c3003     	add	r3, r12, r3
     83c: e0822005     	add	r2, r2, r5
     840: e15150bc     	ldrh	r5, [r1, #-12]
     844: e1a0a722     	lsr	r10, r2, #14
     848: e155000a     	cmp	r5, r10
     84c: 8a00001c     	bhi	0x8c4 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x8c4> @ imm = #0x70
     850: e59d5014     	ldr	r5, [sp, #0x14]
     854: e59d7010     	ldr	r7, [sp, #0x10]
     858: e0065005     	and	r5, r6, r5
     85c: e0037007     	and	r7, r3, r7
     860: e1855007     	orr	r5, r5, r7
     864: e59d700c     	ldr	r7, [sp, #0xc]
     868: e79794a5     	ldr	r9, [r7, r5, lsr #9]
     86c: e59d505c     	ldr	r5, [sp, #0x5c]
     870: e59d7000     	ldr	r7, [sp]
     874: e085b007     	add	r11, r5, r7
     878: e59d5060     	ldr	r5, [sp, #0x60]
     87c: e59d7004     	ldr	r7, [sp, #0x4]
     880: e0857007     	add	r7, r5, r7
     884: e59d5064     	ldr	r5, [sp, #0x64]
     888: e085800e     	add	r8, r5, lr
     88c: e7e75859     	ubfx	r5, r9, #0x10, #0x8
     890: e005059b     	mul	r5, r11, r5
     894: e7e7b459     	ubfx	r11, r9, #0x8, #0x8
     898: e6ef9079     	uxtb	r9, r9
     89c: e0070b97     	mul	r7, r7, r11
     8a0: e0080899     	mul	r8, r9, r8
     8a4: e1a05425     	lsr	r5, r5, #8
     8a8: e1a07827     	lsr	r7, r7, #16
     8ac: e20558ff     	and	r5, r5, #16711680
     8b0: e2077cff     	and	r7, r7, #65280
     8b4: e1855007     	orr	r5, r5, r7
     8b8: e1855c28     	orr	r5, r5, r8, lsr #24
     8bc: e5045018     	str	r5, [r4, #-0x18]
     8c0: e141a0bc     	strh	r10, [r1, #-12]
     8c4: e59d5008     	ldr	r5, [sp, #0x8]
     8c8: e0806006     	add	r6, r0, r6
     8cc: e08c3003     	add	r3, r12, r3
     8d0: e0822005     	add	r2, r2, r5
     8d4: e15150ba     	ldrh	r5, [r1, #-10]
     8d8: e1a02722     	lsr	r2, r2, #14
     8dc: e1550002     	cmp	r5, r2
     8e0: 8a00001c     	bhi	0x958 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x958> @ imm = #0x70
     8e4: e59d5014     	ldr	r5, [sp, #0x14]
     8e8: e59d7010     	ldr	r7, [sp, #0x10]
     8ec: e0065005     	and	r5, r6, r5
     8f0: e0037007     	and	r7, r3, r7
     8f4: e1855007     	orr	r5, r5, r7
     8f8: e59d700c     	ldr	r7, [sp, #0xc]
     8fc: e79794a5     	ldr	r9, [r7, r5, lsr #9]
     900: e59d5068     	ldr	r5, [sp, #0x68]
     904: e59d7000     	ldr	r7, [sp]
     908: e085a007     	add	r10, r5, r7
     90c: e59d506c     	ldr	r5, [sp, #0x6c]
     910: e59d7004     	ldr	r7, [sp, #0x4]
     914: e0857007     	add	r7, r5, r7
     918: e59d5070     	ldr	r5, [sp, #0x70]
     91c: e085800e     	add	r8, r5, lr
     920: e7e75859     	ubfx	r5, r9, #0x10, #0x8
     924: e005059a     	mul	r5, r10, r5
     928: e7e7a459     	ubfx	r10, r9, #0x8, #0x8
     92c: e6ef9079     	uxtb	r9, r9
     930: e0070a97     	mul	r7, r7, r10
     934: e0080899     	mul	r8, r9, r8
     938: e1a05425     	lsr	r5, r5, #8
     93c: e1a07827     	lsr	r7, r7, #16
     940: e20558ff     	and	r5, r5, #16711680
     944: e2077cff     	and	r7, r7, #65280
     948: e1855007     	orr	r5, r5, r7
     94c: e1855c28     	orr	r5, r5, r8, lsr #24
     950: e5045014     	str	r5, [r4, #-0x14]
     954: e14120ba     	strh	r2, [r1, #-10]
     958: e59d501c     	ldr	r5, [sp, #0x1c]
     95c: e0806006     	add	r6, r0, r6
     960: e59d7008     	ldr	r7, [sp, #0x8]
     964: e08c3003     	add	r3, r12, r3
     968: e15120b8     	ldrh	r2, [r1, #-8]
     96c: e0455007     	sub	r5, r5, r7
     970: e1a08725     	lsr	r8, r5, #14
     974: e1520008     	cmp	r2, r8
     978: 8a000022     	bhi	0xa08 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xa08> @ imm = #0x88
     97c: e59d2014     	ldr	r2, [sp, #0x14]
     980: e59d7010     	ldr	r7, [sp, #0x10]
     984: e59d907c     	ldr	r9, [sp, #0x7c]
     988: e0062002     	and	r2, r6, r2
     98c: e0037007     	and	r7, r3, r7
     990: e1822007     	orr	r2, r2, r7
     994: e59d700c     	ldr	r7, [sp, #0xc]
     998: e79774a2     	ldr	r7, [r7, r2, lsr #9]
     99c: e59d2020     	ldr	r2, [sp, #0x20]
     9a0: e082b009     	add	r11, r2, r9
     9a4: e59d2000     	ldr	r2, [sp]
     9a8: e59d9078     	ldr	r9, [sp, #0x78]
     9ac: e08bb002     	add	r11, r11, r2
     9b0: e59d2024     	ldr	r2, [sp, #0x24]
     9b4: e082a009     	add	r10, r2, r9
     9b8: e59d2004     	ldr	r2, [sp, #0x4]
     9bc: e59d9074     	ldr	r9, [sp, #0x74]
     9c0: e08aa002     	add	r10, r10, r2
     9c4: e59d2028     	ldr	r2, [sp, #0x28]
     9c8: e0829009     	add	r9, r2, r9
     9cc: e7e72857     	ubfx	r2, r7, #0x10, #0x8
     9d0: e002029b     	mul	r2, r11, r2
     9d4: e7e7b457     	ubfx	r11, r7, #0x8, #0x8
     9d8: e6ef7077     	uxtb	r7, r7
     9dc: e089900e     	add	r9, r9, lr
     9e0: e00b0b9a     	mul	r11, r10, r11
     9e4: e0070997     	mul	r7, r7, r9
     9e8: e1a02422     	lsr	r2, r2, #8
     9ec: e1a0b82b     	lsr	r11, r11, #16
     9f0: e20228ff     	and	r2, r2, #16711680
     9f4: e20bbcff     	and	r11, r11, #65280
     9f8: e182200b     	orr	r2, r2, r11
     9fc: e1822c27     	orr	r2, r2, r7, lsr #24
     a00: e5042010     	str	r2, [r4, #-0x10]
     a04: e14180b8     	strh	r8, [r1, #-8]
     a08: e59d701c     	ldr	r7, [sp, #0x1c]
     a0c: e0806006     	add	r6, r0, r6
     a10: e15120b6     	ldrh	r2, [r1, #-6]
     a14: e08c3003     	add	r3, r12, r3
     a18: e1a08727     	lsr	r8, r7, #14
     a1c: e1520008     	cmp	r2, r8
     a20: 8a000022     	bhi	0xab0 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xab0> @ imm = #0x88
     a24: e59d2014     	ldr	r2, [sp, #0x14]
     a28: e59d7010     	ldr	r7, [sp, #0x10]
     a2c: e59d9088     	ldr	r9, [sp, #0x88]
     a30: e0062002     	and	r2, r6, r2
     a34: e0037007     	and	r7, r3, r7
     a38: e1822007     	orr	r2, r2, r7
     a3c: e59d700c     	ldr	r7, [sp, #0xc]
     a40: e79774a2     	ldr	r7, [r7, r2, lsr #9]
     a44: e59d2020     	ldr	r2, [sp, #0x20]
     a48: e082b009     	add	r11, r2, r9
     a4c: e59d2000     	ldr	r2, [sp]
     a50: e59d9084     	ldr	r9, [sp, #0x84]
     a54: e08bb002     	add	r11, r11, r2
     a58: e59d2024     	ldr	r2, [sp, #0x24]
     a5c: e082a009     	add	r10, r2, r9
     a60: e59d2004     	ldr	r2, [sp, #0x4]
     a64: e59d9080     	ldr	r9, [sp, #0x80]
     a68: e08aa002     	add	r10, r10, r2
     a6c: e59d2028     	ldr	r2, [sp, #0x28]
     a70: e0829009     	add	r9, r2, r9
     a74: e7e72857     	ubfx	r2, r7, #0x10, #0x8
     a78: e002029b     	mul	r2, r11, r2
     a7c: e7e7b457     	ubfx	r11, r7, #0x8, #0x8
     a80: e6ef7077     	uxtb	r7, r7
     a84: e089900e     	add	r9, r9, lr
     a88: e00b0b9a     	mul	r11, r10, r11
     a8c: e0070997     	mul	r7, r7, r9
     a90: e1a02422     	lsr	r2, r2, #8
     a94: e1a0b82b     	lsr	r11, r11, #16
     a98: e20228ff     	and	r2, r2, #16711680
     a9c: e20bbcff     	and	r11, r11, #65280
     aa0: e182200b     	orr	r2, r2, r11
     aa4: e1822c27     	orr	r2, r2, r7, lsr #24
     aa8: e504200c     	str	r2, [r4, #-0xc]
     aac: e14180b6     	strh	r8, [r1, #-6]
     ab0: e59d202c     	ldr	r2, [sp, #0x2c]
     ab4: e0806006     	add	r6, r0, r6
     ab8: e08c3003     	add	r3, r12, r3
     abc: e0855002     	add	r5, r5, r2
     ac0: e15120b4     	ldrh	r2, [r1, #-4]
     ac4: e1a08725     	lsr	r8, r5, #14
     ac8: e1520008     	cmp	r2, r8
     acc: 8a000022     	bhi	0xb5c <ZB_fillTriangleMappingPerspective_DT1_DW1+0xb5c> @ imm = #0x88
     ad0: e59d2014     	ldr	r2, [sp, #0x14]
     ad4: e59d7010     	ldr	r7, [sp, #0x10]
     ad8: e59d9098     	ldr	r9, [sp, #0x98]
     adc: e0062002     	and	r2, r6, r2
     ae0: e0037007     	and	r7, r3, r7
     ae4: e1822007     	orr	r2, r2, r7
     ae8: e59d700c     	ldr	r7, [sp, #0xc]
     aec: e79774a2     	ldr	r7, [r7, r2, lsr #9]
     af0: e59d2020     	ldr	r2, [sp, #0x20]
     af4: e082b009     	add	r11, r2, r9
     af8: e59d2000     	ldr	r2, [sp]
     afc: e59d909c     	ldr	r9, [sp, #0x9c]
     b00: e08bb002     	add	r11, r11, r2
     b04: e59d2024     	ldr	r2, [sp, #0x24]
     b08: e082a009     	add	r10, r2, r9
     b0c: e59d2004     	ldr	r2, [sp, #0x4]
     b10: e59d90a0     	ldr	r9, [sp, #0xa0]
     b14: e08aa002     	add	r10, r10, r2
     b18: e59d2028     	ldr	r2, [sp, #0x28]
     b1c: e0829009     	add	r9, r2, r9
     b20: e7e72857     	ubfx	r2, r7, #0x10, #0x8
     b24: e002029b     	mul	r2, r11, r2
     b28: e7e7b457     	ubfx	r11, r7, #0x8, #0x8
     b2c: e6ef7077     	uxtb	r7, r7
     b30: e089900e     	add	r9, r9, lr
     b34: e00b0b9a     	mul	r11, r10, r11
     b38: e0070997     	mul	r7, r7, r9
     b3c: e1a02422     	lsr	r2, r2, #8
     b40: e1a0b82b     	lsr	r11, r11, #16
     b44: e20228ff     	and	r2, r2, #16711680
     b48: e20bbcff     	and	r11, r11, #65280
     b4c: e182200b     	orr	r2, r2, r11
     b50: e1822c27     	orr	r2, r2, r7, lsr #24
     b54: e5042008     	str	r2, [r4, #-0x8]
     b58: e14180b4     	strh	r8, [r1, #-4]
     b5c: e59d2008     	ldr	r2, [sp, #0x8]
     b60: e0855002     	add	r5, r5, r2
     b64: e15120b2     	ldrh	r2, [r1, #-2]
     b68: e1a05725     	lsr	r5, r5, #14
     b6c: e1520005     	cmp	r2, r5
     b70: 8a000024     	bhi	0xc08 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xc08> @ imm = #0x90
     b74: e59d2014     	ldr	r2, [sp, #0x14]
     b78: e0800006     	add	r0, r0, r6
     b7c: e08c3003     	add	r3, r12, r3
     b80: e0000002     	and	r0, r0, r2
     b84: e59d2010     	ldr	r2, [sp, #0x10]
     b88: e0033002     	and	r3, r3, r2
     b8c: e1800003     	orr	r0, r0, r3
     b90: e59d300c     	ldr	r3, [sp, #0xc]
     b94: e79324a0     	ldr	r2, [r3, r0, lsr #9]
     b98: e59d3020     	ldr	r3, [sp, #0x20]
     b9c: e59d0094     	ldr	r0, [sp, #0x94]
     ba0: e0836000     	add	r6, r3, r0
     ba4: e59d3000     	ldr	r3, [sp]
     ba8: e59d0090     	ldr	r0, [sp, #0x90]
     bac: e0866003     	add	r6, r6, r3
     bb0: e59d3024     	ldr	r3, [sp, #0x24]
     bb4: e083c000     	add	r12, r3, r0
     bb8: e59d3004     	ldr	r3, [sp, #0x4]
     bbc: e59d008c     	ldr	r0, [sp, #0x8c]
     bc0: e08cc003     	add	r12, r12, r3
     bc4: e59d3028     	ldr	r3, [sp, #0x28]
     bc8: e0830000     	add	r0, r3, r0
     bcc: e7e73852     	ubfx	r3, r2, #0x10, #0x8
     bd0: e0030396     	mul	r3, r6, r3
     bd4: e7e76452     	ubfx	r6, r2, #0x8, #0x8
     bd8: e6ef2072     	uxtb	r2, r2
     bdc: e080000e     	add	r0, r0, lr
     be0: e006069c     	mul	r6, r12, r6
     be4: e0020092     	mul	r2, r2, r0
     be8: e1a03423     	lsr	r3, r3, #8
     bec: e1a06826     	lsr	r6, r6, #16
     bf0: e20338ff     	and	r3, r3, #16711680
     bf4: e2066cff     	and	r6, r6, #65280
     bf8: e1833006     	orr	r3, r3, r6
     bfc: e1833c22     	orr	r3, r3, r2, lsr #24
     c00: e5043004     	str	r3, [r4, #-0x4]
     c04: e14150b2     	strh	r5, [r1, #-2]
     c08: e59d304c     	ldr	r3, [sp, #0x4c]
     c0c: e2811010     	add	r1, r1, #16
     c10: e59d203c     	ldr	r2, [sp, #0x3c]
     c14: e2844020     	add	r4, r4, #32
     c18: ee344a2a     	vadd.f32	s8, s8, s21
     c1c: e1530001     	cmp	r3, r1
     c20: e59d3004     	ldr	r3, [sp, #0x4]
     c24: ee744a8b     	vadd.f32	s9, s9, s22
     c28: e0833002     	add	r3, r3, r2
     c2c: e59d2040     	ldr	r2, [sp, #0x40]
     c30: e58d3004     	str	r3, [sp, #0x4]
     c34: e59d3000     	ldr	r3, [sp]
     c38: e0833002     	add	r3, r3, r2
     c3c: e59d201c     	ldr	r2, [sp, #0x1c]
     c40: e58d3000     	str	r3, [sp]
     c44: e59d3044     	ldr	r3, [sp, #0x44]
     c48: e08ee003     	add	lr, lr, r3
     c4c: e59d3048     	ldr	r3, [sp, #0x48]
     c50: e0822003     	add	r2, r2, r3
     c54: e58d201c     	str	r2, [sp, #0x1c]
     c58: e59d2018     	ldr	r2, [sp, #0x18]
     c5c: e0823003     	add	r3, r2, r3
     c60: e58d3018     	str	r3, [sp, #0x18]
     c64: 1afffe92     	bne	0x6b4 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x6b4> @ imm = #-0x5b8
     c68: e28d20c4     	add	r2, sp, #196
     c6c: e59de038     	ldr	lr, [sp, #0x38]
     c70: e89200a4     	ldm	r2, {r2, r5, r7}
     c74: e28da0b8     	add	r10, sp, #184
     c78: e59d3058     	ldr	r3, [sp, #0x58]
     c7c: e59d00d0     	ldr	r0, [sp, #0xd0]
     c80: e59d4030     	ldr	r4, [sp, #0x30]
     c84: e59d6034     	ldr	r6, [sp, #0x34]
     c88: e0821000     	add	r1, r2, r0
     c8c: e083010e     	add	r0, r3, lr, lsl #2
     c90: e59d3050     	ldr	r3, [sp, #0x50]
     c94: e080008e     	add	r0, r0, lr, lsl #1
     c98: e59d2054     	ldr	r2, [sp, #0x54]
     c9c: e89a1c00     	ldm	r10, {r10, r11, r12}
     ca0: e080000e     	add	r0, r0, lr
     ca4: e59de044     	ldr	lr, [sp, #0x44]
     ca8: e0833104     	add	r3, r3, r4, lsl #2
     cac: e0822106     	add	r2, r2, r6, lsl #2
     cb0: e0833084     	add	r3, r3, r4, lsl #1
     cb4: e24cc008     	sub	r12, r12, #8
     cb8: e0822086     	add	r2, r2, r6, lsl #1
     cbc: e04cc185     	sub	r12, r12, r5, lsl #3
     cc0: e0200e95     	mla	r0, r5, lr, r0
     cc4: e0833004     	add	r3, r3, r4
     cc8: e0822006     	add	r2, r2, r6
     ccc: e0876285     	add	r6, r7, r5, lsl #5
     cd0: e58d0050     	str	r0, [sp, #0x50]
     cd4: e59d0040     	ldr	r0, [sp, #0x40]
     cd8: e0233095     	mla	r3, r5, r0, r3
     cdc: e58d301c     	str	r3, [sp, #0x1c]
     ce0: e59d303c     	ldr	r3, [sp, #0x3c]
     ce4: e0232395     	mla	r3, r5, r3, r2
     ce8: e58d304c     	str	r3, [sp, #0x4c]
     cec: e59d3048     	ldr	r3, [sp, #0x48]
     cf0: e08b0003     	add	r0, r11, r3
     cf4: e0200593     	mla	r0, r3, r5, r0
     cf8: e35c0000     	cmp	r12, #0
     cfc: ba000048     	blt	0xe24 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xe24> @ imm = #0x120
     d00: ee244a27     	vmul.f32	s8, s8, s15
     d04: e3a07000     	mov	r7, #0
     d08: ee644aa7     	vmul.f32	s9, s9, s15
     d0c: e081308c     	add	r3, r1, r12, lsl #1
     d10: eef05a69     	vmov.f32	s11, s19
     d14: e2411002     	sub	r1, r1, #2
     d18: eeb06a49     	vmov.f32	s12, s18
     d1c: e1a09007     	mov	r9, r7
     d20: ee485a44     	vmls.f32	s11, s16, s8
     d24: e1a08007     	mov	r8, r7
     d28: ee086a64     	vmls.f32	s12, s16, s9
     d2c: e58d3000     	str	r3, [sp]
     d30: e58da054     	str	r10, [sp, #0x54]
     d34: e58db058     	str	r11, [sp, #0x58]
     d38: ee655aa7     	vmul.f32	s11, s11, s15
     d3c: ee667a27     	vmul.f32	s15, s12, s15
     d40: eebd6ac4     	vcvt.s32.f32	s12, s8
     d44: eefd7ae7     	vcvt.s32.f32	s15, s15
     d48: ee165a10     	vmov	r5, s12
     d4c: eebd6ae4     	vcvt.s32.f32	s12, s9
     d50: edcd7a06     	vstr	s15, [sp, #24]
     d54: ee164a10     	vmov	r4, s12
     d58: eebd6ae5     	vcvt.s32.f32	s12, s11
     d5c: ed8d6a01     	vstr	s12, [sp, #4]
     d60: e1f130b2     	ldrh	r3, [r1, #2]!
     d64: e1a0a720     	lsr	r10, r0, #14
     d68: e153000a     	cmp	r3, r10
     d6c: e59d3008     	ldr	r3, [sp, #0x8]
     d70: e0800003     	add	r0, r0, r3
     d74: 8a00001a     	bhi	0xde4 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xde4> @ imm = #0x68
     d78: e59d3014     	ldr	r3, [sp, #0x14]
     d7c: e59d2010     	ldr	r2, [sp, #0x10]
     d80: e0053003     	and	r3, r5, r3
     d84: e0042002     	and	r2, r4, r2
     d88: e1833002     	orr	r3, r3, r2
     d8c: e59d200c     	ldr	r2, [sp, #0xc]
     d90: e792e4a3     	ldr	lr, [r2, r3, lsr #9]
     d94: e59d301c     	ldr	r3, [sp, #0x1c]
     d98: e083b009     	add	r11, r3, r9
     d9c: e59d304c     	ldr	r3, [sp, #0x4c]
     da0: e0832008     	add	r2, r3, r8
     da4: e59d3050     	ldr	r3, [sp, #0x50]
     da8: e083c007     	add	r12, r3, r7
     dac: e7e7385e     	ubfx	r3, lr, #0x10, #0x8
     db0: e003039b     	mul	r3, r11, r3
     db4: e7e7b45e     	ubfx	r11, lr, #0x8, #0x8
     db8: e6efe07e     	uxtb	lr, lr
     dbc: e0020b92     	mul	r2, r2, r11
     dc0: e00c0e9c     	mul	r12, r12, lr
     dc4: e1a03423     	lsr	r3, r3, #8
     dc8: e1a02822     	lsr	r2, r2, #16
     dcc: e20338ff     	and	r3, r3, #16711680
     dd0: e2022cff     	and	r2, r2, #65280
     dd4: e1833002     	orr	r3, r3, r2
     dd8: e1833c2c     	orr	r3, r3, r12, lsr #24
     ddc: e5863000     	str	r3, [r6]
     de0: e1c1a0b0     	strh	r10, [r1]
     de4: e59d3000     	ldr	r3, [sp]
     de8: e2866004     	add	r6, r6, #4
     dec: e1530001     	cmp	r3, r1
     df0: e59d3004     	ldr	r3, [sp, #0x4]
     df4: e0855003     	add	r5, r5, r3
     df8: e59d3018     	ldr	r3, [sp, #0x18]
     dfc: e0844003     	add	r4, r4, r3
     e00: e59d3034     	ldr	r3, [sp, #0x34]
     e04: e0888003     	add	r8, r8, r3
     e08: e59d3030     	ldr	r3, [sp, #0x30]
     e0c: e0899003     	add	r9, r9, r3
     e10: e59d3038     	ldr	r3, [sp, #0x38]
     e14: e0877003     	add	r7, r7, r3
     e18: 1affffd0     	bne	0xd60 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xd60> @ imm = #-0xc0
     e1c: e59da054     	ldr	r10, [sp, #0x54]
     e20: e59db058     	ldr	r11, [sp, #0x58]
     e24: e59d30a4     	ldr	r3, [sp, #0xa4]
     e28: e59d20dc     	ldr	r2, [sp, #0xdc]
     e2c: e0833002     	add	r3, r3, r2
     e30: e3530000     	cmp	r3, #0
     e34: e58d30a4     	str	r3, [sp, #0xa4]
     e38: da000031     	ble	0xf04 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xf04> @ imm = #0xc4
     e3c: e59d30a4     	ldr	r3, [sp, #0xa4]
     e40: e59d2100     	ldr	r2, [sp, #0x100]
     e44: ee766aa2     	vadd.f32	s13, s13, s5
     e48: e2433801     	sub	r3, r3, #65536
     e4c: ee733a83     	vadd.f32	s7, s7, s6
     e50: e58d30a4     	str	r3, [sp, #0xa4]
     e54: e59d310c     	ldr	r3, [sp, #0x10c]
     e58: e08aa003     	add	r10, r10, r3
     e5c: e59d30fc     	ldr	r3, [sp, #0xfc]
     e60: e08bb003     	add	r11, r11, r3
     e64: e59d3020     	ldr	r3, [sp, #0x20]
     e68: e0833002     	add	r3, r3, r2
     e6c: e59d2104     	ldr	r2, [sp, #0x104]
     e70: e58d3020     	str	r3, [sp, #0x20]
     e74: e59d3024     	ldr	r3, [sp, #0x24]
     e78: e0833002     	add	r3, r3, r2
     e7c: e59d2108     	ldr	r2, [sp, #0x108]
     e80: e58d3024     	str	r3, [sp, #0x24]
     e84: e59d3028     	ldr	r3, [sp, #0x28]
     e88: e0833002     	add	r3, r3, r2
     e8c: e58d3028     	str	r3, [sp, #0x28]
     e90: e59d30a8     	ldr	r3, [sp, #0xa8]
     e94: e59d20d4     	ldr	r2, [sp, #0xd4]
     e98: e3530000     	cmp	r3, #0
     e9c: e59d30ac     	ldr	r3, [sp, #0xac]
     ea0: e0833002     	add	r3, r3, r2
     ea4: e59d20b0     	ldr	r2, [sp, #0xb0]
     ea8: e58d30ac     	str	r3, [sp, #0xac]
     eac: e59d30d8     	ldr	r3, [sp, #0xd8]
     eb0: e5933020     	ldr	r3, [r3, #0x20]
     eb4: e0822103     	add	r2, r2, r3, lsl #2
     eb8: e58d20b0     	str	r2, [sp, #0xb0]
     ebc: e59d20b4     	ldr	r2, [sp, #0xb4]
     ec0: e0823083     	add	r3, r2, r3, lsl #1
     ec4: e58d30b4     	str	r3, [sp, #0xb4]
     ec8: 1afffdb0     	bne	0x590 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x590> @ imm = #-0x940
     ecc: e59d6130     	ldr	r6, [sp, #0x130]
     ed0: e1a0800a     	mov	r8, r10
     ed4: e59d7134     	ldr	r7, [sp, #0x134]
     ed8: e59d30d4     	ldr	r3, [sp, #0xd4]
     edc: e58db000     	str	r11, [sp]
     ee0: e59d912c     	ldr	r9, [sp, #0x12c]
     ee4: e59da008     	ldr	r10, [sp, #0x8]
     ee8: e59db138     	ldr	r11, [sp, #0x138]
     eec: e0277396     	mla	r7, r6, r3, r7
     ef0: e3590001     	cmp	r9, #1
     ef4: 1a00001e     	bne	0xf74 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xf74> @ imm = #0x78
     ef8: e28ddf51     	add	sp, sp, #324
     efc: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     f00: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     f04: e59d3120     	ldr	r3, [sp, #0x120]
     f08: e59d2114     	ldr	r2, [sp, #0x114]
     f0c: ee766aa1     	vadd.f32	s13, s13, s3
     f10: e08aa003     	add	r10, r10, r3
     f14: e59d3110     	ldr	r3, [sp, #0x110]
     f18: ee733a82     	vadd.f32	s7, s7, s4
     f1c: e08bb003     	add	r11, r11, r3
     f20: e59d3020     	ldr	r3, [sp, #0x20]
     f24: e0833002     	add	r3, r3, r2
     f28: e59d2118     	ldr	r2, [sp, #0x118]
     f2c: e58d3020     	str	r3, [sp, #0x20]
     f30: e59d3024     	ldr	r3, [sp, #0x24]
     f34: e0833002     	add	r3, r3, r2
     f38: e59d211c     	ldr	r2, [sp, #0x11c]
     f3c: e58d3024     	str	r3, [sp, #0x24]
     f40: e59d3028     	ldr	r3, [sp, #0x28]
     f44: e0833002     	add	r3, r3, r2
     f48: e58d3028     	str	r3, [sp, #0x28]
     f4c: eaffffcf     	b	0xe90 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xe90> @ imm = #-0xc4
     f50: e59d3028     	ldr	r3, [sp, #0x28]
     f54: eef04a63     	vmov.f32	s9, s7
     f58: eeb04a66     	vmov.f32	s8, s13
     f5c: e58d3050     	str	r3, [sp, #0x50]
     f60: e59d3024     	ldr	r3, [sp, #0x24]
     f64: e58d304c     	str	r3, [sp, #0x4c]
     f68: e59d3020     	ldr	r3, [sp, #0x20]
     f6c: e58d301c     	str	r3, [sp, #0x1c]
     f70: eaffff60     	b	0xcf8 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xcf8> @ imm = #-0x280
     f74: e59d2124     	ldr	r2, [sp, #0x124]
     f78: e59b1004     	ldr	r1, [r11, #0x4]
     f7c: eef5cac0     	vcmpe.f32	s25, #0
     f80: e5923004     	ldr	r3, [r2, #0x4]
     f84: eef1fa10     	vmrs	APSR_nzcv, fpscr
     f88: e0411003     	sub	r1, r1, r3
     f8c: e2816001     	add	r6, r1, #1
     f90: da000020     	ble	0x1018 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x1018> @ imm = #0x80
     f94: e3510000     	cmp	r1, #0
     f98: e5924000     	ldr	r4, [r2]
     f9c: e3a05001     	mov	r5, #1
     fa0: da000021     	ble	0x102c <ZB_fillTriangleMappingPerspective_DT1_DW1+0x102c> @ imm = #0x84
     fa4: e59d3124     	ldr	r3, [sp, #0x124]
     fa8: e58db13c     	str	r11, [sp, #0x13c]
     fac: e58d3128     	str	r3, [sp, #0x128]
     fb0: e59d313c     	ldr	r3, [sp, #0x13c]
     fb4: e1a09005     	mov	r9, r5
     fb8: ed8d7a06     	vstr	s14, [sp, #24]
     fbc: edcd3a02     	vstr	s7, [sp, #8]
     fc0: e5930000     	ldr	r0, [r3]
     fc4: edcd6a01     	vstr	s13, [sp, #4]
     fc8: e0400004     	sub	r0, r0, r4
     fcc: e1a00800     	lsl	r0, r0, #16
     fd0: ebfffffe     	bl	0xfd0 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xfd0> @ imm = #-0x8
     fd4: eddd6a01     	vldr	s13, [sp, #4]
     fd8: eddd3a02     	vldr	s7, [sp, #8]
     fdc: ed9d7a06     	vldr	s14, [sp, #24]
     fe0: e58d00d4     	str	r0, [sp, #0xd4]
     fe4: e1a07804     	lsl	r7, r4, #16
     fe8: eafffd46     	b	0x508 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x508> @ imm = #-0xae8
     fec: e3a03000     	mov	r3, #0
     ff0: e3a02001     	mov	r2, #1
     ff4: ed9f7a2d     	vldr	s14, [pc, #180]         @ 0x10b0 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x10b0>
     ff8: e58d30dc     	str	r3, [sp, #0xdc]
     ffc: ed8dfa47     	vstr	s30, [sp, #284]
    1000: edcdea46     	vstr	s29, [sp, #280]
    1004: ed8dea45     	vstr	s28, [sp, #276]
    1008: edcdda44     	vstr	s27, [sp, #272]
    100c: e58d210c     	str	r2, [sp, #0x10c]
    1010: e58d3120     	str	r3, [sp, #0x120]
    1014: eafffd2e     	b	0x4d4 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x4d4> @ imm = #-0xb48
    1018: e58d90a4     	str	r9, [sp, #0xa4]
    101c: e1a0000b     	mov	r0, r11
    1020: e59d4124     	ldr	r4, [sp, #0x124]
    1024: e3a09001     	mov	r9, #1
    1028: eafffd0a     	b	0x458 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x458> @ imm = #-0xbd8
    102c: e3560000     	cmp	r6, #0
    1030: daffffb0     	ble	0xef8 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xef8> @ imm = #-0x140
    1034: e58d90d4     	str	r9, [sp, #0xd4]
    1038: e1a07804     	lsl	r7, r4, #16
    103c: e1a09005     	mov	r9, r5
    1040: e58db13c     	str	r11, [sp, #0x13c]
    1044: e58d2128     	str	r2, [sp, #0x128]
    1048: eafffd30     	b	0x510 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x510> @ imm = #-0xb40
    104c: e1a0c003     	mov	r12, r3
    1050: e58db124     	str	r11, [sp, #0x124]
    1054: e1a03000     	mov	r3, r0
    1058: e1a0b004     	mov	r11, r4
    105c: e1a0000e     	mov	r0, lr
    1060: e58d5128     	str	r5, [sp, #0x128]
    1064: eafffc02     	b	0x74 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x74> @ imm = #-0xff8
    1068: e59d0124     	ldr	r0, [sp, #0x124]
    106c: e58db13c     	str	r11, [sp, #0x13c]
    1070: eafffce2     	b	0x400 <ZB_fillTriangleMappingPerspective_DT1_DW1+0x400> @ imm = #-0xc78
    1074: e59d313c     	ldr	r3, [sp, #0x13c]
    1078: e59d2128     	ldr	r2, [sp, #0x128]
    107c: e5931004     	ldr	r1, [r3, #0x4]
    1080: e5923004     	ldr	r3, [r2, #0x4]
    1084: e5924000     	ldr	r4, [r2]
    1088: e0411003     	sub	r1, r1, r3
    108c: e3510000     	cmp	r1, #0
    1090: c3a03000     	movgt	r3, #0
    1094: c1a05009     	movgt	r5, r9
    1098: c58d30a4     	strgt	r3, [sp, #0xa4]
    109c: caffffc3     	bgt	0xfb0 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xfb0> @ imm = #-0xf4
    10a0: e3a03000     	mov	r3, #0
    10a4: e58d30a4     	str	r3, [sp, #0xa4]
    10a8: e58d30d4     	str	r3, [sp, #0xd4]
    10ac: eaffffcc     	b	0xfe4 <ZB_fillTriangleMappingPerspective_DT1_DW1+0xfe4> @ imm = #-0xd0
    10b0: 00 00 00 00  	.word	0x00000000

Disassembly of section .text.ZB_fillTriangleMappingPerspectiveNOBLEND_DT0_DW0:

00000000 <ZB_fillTriangleMappingPerspectiveNOBLEND_DT0_DW0>:
       0: eafffffe     	b	0x0 <ZB_fillTriangleMappingPerspectiveNOBLEND_DT0_DW0> @ imm = #-0x8

Disassembly of section .text.ZB_fillTriangleMappingPerspectiveNOBLEND_DT0_DW1:

00000000 <ZB_fillTriangleMappingPerspectiveNOBLEND_DT0_DW1>:
       0: eafffffe     	b	0x0 <ZB_fillTriangleMappingPerspectiveNOBLEND_DT0_DW1> @ imm = #-0x8

Disassembly of section .text.ZB_fillTriangleMappingPerspectiveNOBLEND_DT1_DW0:

00000000 <ZB_fillTriangleMappingPerspectiveNOBLEND_DT1_DW0>:
       0: eafffffe     	b	0x0 <ZB_fillTriangleMappingPerspectiveNOBLEND_DT1_DW0> @ imm = #-0x8

Disassembly of section .text.ZB_fillTriangleMappingPerspectiveNOBLEND_DT1_DW1:

00000000 <ZB_fillTriangleMappingPerspectiveNOBLEND_DT1_DW1>:
       0: eafffffe     	b	0x0 <ZB_fillTriangleMappingPerspectiveNOBLEND_DT1_DW1> @ imm = #-0x8

Disassembly of section .text.ZB_fillTriangleSmoothNOBLEND_DT0_DW0:

00000000 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a07003     	mov	r7, r3
       8: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
       c: e24ddf47     	sub	sp, sp, #284
      10: e5923004     	ldr	r3, [r2, #0x4]
      14: e58d0098     	str	r0, [sp, #0x98]
      18: e5910004     	ldr	r0, [r1, #0x4]
      1c: e597c004     	ldr	r12, [r7, #0x4]
      20: e59fe90c     	ldr	lr, [pc, #0x90c]        @ 0x934 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x934>
      24: e1530000     	cmp	r3, r0
      28: a1a04000     	movge	r4, r0
      2c: b1a04003     	movlt	r4, r3
      30: a1a00003     	movge	r0, r3
      34: b1a06002     	movlt	r6, r2
      38: a1a06001     	movge	r6, r1
      3c: b1a08001     	movlt	r8, r1
      40: a1a08002     	movge	r8, r2
      44: e15c0004     	cmp	r12, r4
      48: e08fe00e     	add	lr, pc, lr
      4c: e1a03004     	mov	r3, r4
      50: ba00000b     	blt	0x84 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x84> @ imm = #0x2c
      54: e15c0000     	cmp	r12, r0
      58: e1a02007     	mov	r2, r7
      5c: a1a03000     	movge	r3, r0
      60: a1a07006     	movge	r7, r6
      64: a1a0000c     	movge	r0, r12
      68: a1a06008     	movge	r6, r8
      6c: b1a0300c     	movlt	r3, r12
      70: b1a07006     	movlt	r7, r6
      74: a1a0c004     	movge	r12, r4
      78: a1a08002     	movge	r8, r2
      7c: b1a0c004     	movlt	r12, r4
      80: b1a06002     	movlt	r6, r2
      84: e040200c     	sub	r2, r0, r12
      88: e5971000     	ldr	r1, [r7]
      8c: e5974014     	ldr	r4, [r7, #0x14]
      90: ee062a90     	vmov	s13, r2
      94: e5982000     	ldr	r2, [r8]
      98: eef86ae6     	vcvt.f32.s32	s13, s13
      9c: e5970018     	ldr	r0, [r7, #0x18]
      a0: e0422001     	sub	r2, r2, r1
      a4: ee072a90     	vmov	s15, r2
      a8: e043200c     	sub	r2, r3, r12
      ac: ee072a10     	vmov	s14, r2
      b0: e5962000     	ldr	r2, [r6]
      b4: eef87ae7     	vcvt.f32.s32	s15, s15
      b8: eeb87ac7     	vcvt.f32.s32	s14, s14
      bc: e0422001     	sub	r2, r2, r1
      c0: e597101c     	ldr	r1, [r7, #0x1c]
      c4: ee062a10     	vmov	s12, r2
      c8: ee278a27     	vmul.f32	s16, s14, s15
      cc: eeb86ac6     	vcvt.f32.s32	s12, s12
      d0: e5982014     	ldr	r2, [r8, #0x14]
      d4: ee168a26     	vnmls.f32	s16, s12, s13
      d8: e0422004     	sub	r2, r2, r4
      dc: eeb58a40     	vcmp.f32	s16, #0
      e0: eef1fa10     	vmrs	APSR_nzcv, fpscr
      e4: 1ef75a00     	vmovne.f32	s11, #1.000000e+00
      e8: 1e858a88     	vdivne.f32	s16, s11, s16
      ec: ee284a06     	vmul.f32	s8, s16, s12
      f0: ee062a10     	vmov	s12, r2
      f4: e5982018     	ldr	r2, [r8, #0x18]
      f8: eef84ac6     	vcvt.f32.s32	s9, s12
      fc: ee287a07     	vmul.f32	s14, s16, s14
     100: e0422000     	sub	r2, r2, r0
     104: ee686a26     	vmul.f32	s13, s16, s13
     108: ee687a27     	vmul.f32	s15, s16, s15
     10c: ee062a10     	vmov	s12, r2
     110: e598201c     	ldr	r2, [r8, #0x1c]
     114: eeb85ac6     	vcvt.f32.s32	s10, s12
     118: ee273a24     	vmul.f32	s6, s14, s9
     11c: e0422001     	sub	r2, r2, r1
     120: eeb58ac0     	vcmpe.f32	s16, #0
     124: eef1fa10     	vmrs	APSR_nzcv, fpscr
     128: ee062a10     	vmov	s12, r2
     12c: e5962014     	ldr	r2, [r6, #0x14]
     130: eef85ac6     	vcvt.f32.s32	s11, s12
     134: ee673a05     	vmul.f32	s7, s14, s10
     138: e0422004     	sub	r2, r2, r4
     13c: ee062a10     	vmov	s12, r2
     140: e5962018     	ldr	r2, [r6, #0x18]
     144: eeb86ac6     	vcvt.f32.s32	s12, s12
     148: ee277a25     	vmul.f32	s14, s14, s11
     14c: e0422000     	sub	r2, r2, r0
     150: ee163a86     	vnmls.f32	s6, s13, s12
     154: ee276a86     	vmul.f32	s12, s15, s12
     158: ee146a24     	vnmls.f32	s12, s8, s9
     15c: eef04a63     	vmov.f32	s9, s7
     160: eebd6ac6     	vcvt.s32.f32	s12, s12
     164: eef08a46     	vmov.f32	s17, s12
     168: ee062a10     	vmov	s12, r2
     16c: e596201c     	ldr	r2, [r6, #0x1c]
     170: eeb86ac6     	vcvt.f32.s32	s12, s12
     174: e0422001     	sub	r2, r2, r1
     178: e59d1098     	ldr	r1, [sp, #0x98]
     17c: ee564a86     	vnmls.f32	s9, s13, s12
     180: ee276a86     	vmul.f32	s12, s15, s12
     184: ee146a05     	vnmls.f32	s12, s8, s10
     188: eebd6ac6     	vcvt.s32.f32	s12, s12
     18c: ed8d6a3e     	vstr	s12, [sp, #248]
     190: ee062a10     	vmov	s12, r2
     194: e5912020     	ldr	r2, [r1, #0x20]
     198: eeb86ac6     	vcvt.f32.s32	s12, s12
     19c: e5911004     	ldr	r1, [r1, #0x4]
     1a0: e0020c92     	mul	r2, r2, r12
     1a4: ee677a86     	vmul.f32	s15, s15, s12
     1a8: ee167a86     	vnmls.f32	s14, s13, s12
     1ac: e0812102     	add	r2, r1, r2, lsl #2
     1b0: ee547a25     	vnmls.f32	s15, s8, s11
     1b4: e58d2078     	str	r2, [sp, #0x78]
     1b8: eefd7ae7     	vcvt.s32.f32	s15, s15
     1bc: edcd7a3f     	vstr	s15, [sp, #252]
     1c0: eefd7ac3     	vcvt.s32.f32	s15, s6
     1c4: edcd7a1f     	vstr	s15, [sp, #124]
     1c8: e59d207c     	ldr	r2, [sp, #0x7c]
     1cc: eefd7ae4     	vcvt.s32.f32	s15, s9
     1d0: e1a01102     	lsl	r1, r2, #2
     1d4: e1a04082     	lsl	r4, r2, #1
     1d8: edcd7a20     	vstr	s15, [sp, #128]
     1dc: e58d1020     	str	r1, [sp, #0x20]
     1e0: e0842002     	add	r2, r4, r2
     1e4: eeac1b10     	vdup.32	q6, r1
     1e8: e59d1080     	ldr	r1, [sp, #0x80]
     1ec: eefd7ac7     	vcvt.s32.f32	s15, s14
     1f0: e58d20a8     	str	r2, [sp, #0xa8]
     1f4: e58d40b4     	str	r4, [sp, #0xb4]
     1f8: e1a00101     	lsl	r0, r1, #2
     1fc: edcd7a21     	vstr	s15, [sp, #132]
     200: e58d0024     	str	r0, [sp, #0x24]
     204: eeae0b10     	vdup.32	q7, r0
     208: ee170a90     	vmov	r0, s15
     20c: e1a0c100     	lsl	r12, r0, #2
     210: e1a05080     	lsl	r5, r0, #1
     214: e58dc048     	str	r12, [sp, #0x48]
     218: e0852000     	add	r2, r5, r0
     21c: eea8cb90     	vdup.32	q12, r12
     220: e1a0c081     	lsl	r12, r1, #1
     224: e58d50ac     	str	r5, [sp, #0xac]
     228: e08c1001     	add	r1, r12, r1
     22c: e58dc0b0     	str	r12, [sp, #0xb0]
     230: e58d10a4     	str	r1, [sp, #0xa4]
     234: e58d20a0     	str	r2, [sp, #0xa0]
     238: da0001aa     	ble	0x8e8 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x8e8> @ imm = #0x6a8
     23c: e5981004     	ldr	r1, [r8, #0x4]
     240: e1a02008     	mov	r2, r8
     244: e1a05006     	mov	r5, r6
     248: e5970004     	ldr	r0, [r7, #0x4]
     24c: e3a0b000     	mov	r11, #0
     250: f3c7e25f     	vmov.i32	q15, #0xff00
     254: e1a04007     	mov	r4, r7
     258: f3c7c05f     	vmov.i32	q14, #0xff
     25c: e1a0a00b     	mov	r10, r11
     260: e0439000     	sub	r9, r3, r0
     264: e59f36cc     	ldr	r3, [pc, #0x6cc]        @ 0x938 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x938>
     268: f3c7a45f     	vmov.i32	q13, #0xff0000
     26c: e0411000     	sub	r1, r1, r0
     270: e58db090     	str	r11, [sp, #0x90]
     274: e08f3003     	add	r3, pc, r3
     278: e58d5100     	str	r5, [sp, #0x100]
     27c: e58d30ec     	str	r3, [sp, #0xec]
     280: e3a03001     	mov	r3, #1
     284: e58d305c     	str	r3, [sp, #0x5c]
     288: e28d3f42     	add	r3, sp, #264
     28c: e58d30f0     	str	r3, [sp, #0xf0]
     290: e58d7104     	str	r7, [sp, #0x104]
     294: e58de0f4     	str	lr, [sp, #0xf4]
     298: e5940000     	ldr	r0, [r4]
     29c: e3510000     	cmp	r1, #0
     2a0: e58d0074     	str	r0, [sp, #0x74]
     2a4: da000177     	ble	0x888 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x888> @ imm = #0x5dc
     2a8: e28d3038     	add	r3, sp, #56
     2ac: f44d8adf     	vst1.64	{d24, d25}, [sp:64]
     2b0: f443aadf     	vst1.64	{d26, d27}, [r3:64]
     2b4: e28d3028     	add	r3, sp, #40
     2b8: f443cadf     	vst1.64	{d28, d29}, [r3:64]
     2bc: e28d3010     	add	r3, sp, #16
     2c0: f443eadf     	vst1.64	{d30, d31}, [r3:64]
     2c4: e5923000     	ldr	r3, [r2]
     2c8: e0433000     	sub	r3, r3, r0
     2cc: e1a00803     	lsl	r0, r3, #16
     2d0: ebfffffe     	bl	0x2d0 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x2d0> @ imm = #-0x8
     2d4: e59d107c     	ldr	r1, [sp, #0x7c]
     2d8: e6ff2070     	uxth	r2, r0
     2dc: e1a03840     	asr	r3, r0, #16
     2e0: f46d8adf     	vld1.64	{d24, d25}, [sp:64]
     2e4: e58d209c     	str	r2, [sp, #0x9c]
     2e8: ee182a90     	vmov	r2, s17
     2ec: e58d30d4     	str	r3, [sp, #0xd4]
     2f0: e0222193     	mla	r2, r3, r1, r2
     2f4: e59d1080     	ldr	r1, [sp, #0x80]
     2f8: e58d20c8     	str	r2, [sp, #0xc8]
     2fc: e59d20f8     	ldr	r2, [sp, #0xf8]
     300: e0222193     	mla	r2, r3, r1, r2
     304: e59d1084     	ldr	r1, [sp, #0x84]
     308: e58d20cc     	str	r2, [sp, #0xcc]
     30c: e59d20fc     	ldr	r2, [sp, #0xfc]
     310: e0222193     	mla	r2, r3, r1, r2
     314: e2833001     	add	r3, r3, #1
     318: e58d30c4     	str	r3, [sp, #0xc4]
     31c: e28d3010     	add	r3, sp, #16
     320: e58d20d0     	str	r2, [sp, #0xd0]
     324: f463eadf     	vld1.64	{d30, d31}, [r3:64]
     328: e28d3028     	add	r3, sp, #40
     32c: f463cadf     	vld1.64	{d28, d29}, [r3:64]
     330: e28d3038     	add	r3, sp, #56
     334: f463aadf     	vld1.64	{d26, d27}, [r3:64]
     338: e59d305c     	ldr	r3, [sp, #0x5c]
     33c: e3530000     	cmp	r3, #0
     340: e5943014     	ldr	r3, [r4, #0x14]
     344: e58d3060     	str	r3, [sp, #0x60]
     348: e5943018     	ldr	r3, [r4, #0x18]
     34c: e58d3064     	str	r3, [sp, #0x64]
     350: e594301c     	ldr	r3, [r4, #0x1c]
     354: e58d3068     	str	r3, [sp, #0x68]
     358: 1a000166     	bne	0x8f8 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x8f8> @ imm = #0x598
     35c: e3590000     	cmp	r9, #0
     360: da0000dc     	ble	0x6d8 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x6d8> @ imm = #0x370
     364: e59d20f4     	ldr	r2, [sp, #0xf4]
     368: e59f35cc     	ldr	r3, [pc, #0x5cc]        @ 0x93c <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x93c>
     36c: e7923003     	ldr	r3, [r2, r3]
     370: e59d20c8     	ldr	r2, [sp, #0xc8]
     374: e58d906c     	str	r9, [sp, #0x6c]
     378: e58d3094     	str	r3, [sp, #0x94]
     37c: e59d307c     	ldr	r3, [sp, #0x7c]
     380: e58da070     	str	r10, [sp, #0x70]
     384: e58da0d8     	str	r10, [sp, #0xd8]
     388: e0833002     	add	r3, r3, r2
     38c: e59d20cc     	ldr	r2, [sp, #0xcc]
     390: e58d30b8     	str	r3, [sp, #0xb8]
     394: e59d3080     	ldr	r3, [sp, #0x80]
     398: e58d90dc     	str	r9, [sp, #0xdc]
     39c: e58db0e0     	str	r11, [sp, #0xe0]
     3a0: e0833002     	add	r3, r3, r2
     3a4: e59d20d0     	ldr	r2, [sp, #0xd0]
     3a8: e58d30bc     	str	r3, [sp, #0xbc]
     3ac: e59d3084     	ldr	r3, [sp, #0x84]
     3b0: e58d80e4     	str	r8, [sp, #0xe4]
     3b4: e58d60e8     	str	r6, [sp, #0xe8]
     3b8: e0833002     	add	r3, r3, r2
     3bc: e58d30c0     	str	r3, [sp, #0xc0]
     3c0: e59d306c     	ldr	r3, [sp, #0x6c]
     3c4: e59d2074     	ldr	r2, [sp, #0x74]
     3c8: e59d1070     	ldr	r1, [sp, #0x70]
     3cc: e2433001     	sub	r3, r3, #1
     3d0: e58d306c     	str	r3, [sp, #0x6c]
     3d4: e59d3094     	ldr	r3, [sp, #0x94]
     3d8: e0626841     	rsb	r6, r2, r1, asr #16
     3dc: e5933000     	ldr	r3, [r3]
     3e0: e3530000     	cmp	r3, #0
     3e4: 1356000e     	cmpne	r6, #14
     3e8: e59d3078     	ldr	r3, [sp, #0x78]
     3ec: e083c102     	add	r12, r3, r2, lsl #2
     3f0: ca0000ce     	bgt	0x730 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x730> @ imm = #0x338
     3f4: e59d8068     	ldr	r8, [sp, #0x68]
     3f8: e59da064     	ldr	r10, [sp, #0x64]
     3fc: e59d9060     	ldr	r9, [sp, #0x60]
     400: e3560002     	cmp	r6, #2
     404: da000074     	ble	0x5dc <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x5dc> @ imm = #0x1d0
     408: e59d20b0     	ldr	r2, [sp, #0xb0]
     40c: e28c3010     	add	r3, r12, #16
     410: e3a0c000     	mov	r12, #0
     414: e1a05003     	mov	r5, r3
     418: e1a07006     	mov	r7, r6
     41c: e1a0400c     	mov	r4, r12
     420: e08a2002     	add	r2, r10, r2
     424: e1a0e00c     	mov	lr, r12
     428: e58d2054     	str	r2, [sp, #0x54]
     42c: e59d20b4     	ldr	r2, [sp, #0xb4]
     430: e58d6088     	str	r6, [sp, #0x88]
     434: e58d308c     	str	r3, [sp, #0x8c]
     438: e0892002     	add	r2, r9, r2
     43c: e58d2058     	str	r2, [sp, #0x58]
     440: e59d20ac     	ldr	r2, [sp, #0xac]
     444: e0882002     	add	r2, r8, r2
     448: e58d2050     	str	r2, [sp, #0x50]
     44c: e59d20a4     	ldr	r2, [sp, #0xa4]
     450: e082200a     	add	r2, r2, r10
     454: e58d2038     	str	r2, [sp, #0x38]
     458: e59d20a8     	ldr	r2, [sp, #0xa8]
     45c: e0822009     	add	r2, r2, r9
     460: e58d204c     	str	r2, [sp, #0x4c]
     464: e59d20a0     	ldr	r2, [sp, #0xa0]
     468: e0822008     	add	r2, r2, r8
     46c: e58d2028     	str	r2, [sp, #0x28]
     470: e59d2080     	ldr	r2, [sp, #0x80]
     474: e082200a     	add	r2, r2, r10
     478: e58d2010     	str	r2, [sp, #0x10]
     47c: e59d2084     	ldr	r2, [sp, #0x84]
     480: e082b008     	add	r11, r2, r8
     484: e59d207c     	ldr	r2, [sp, #0x7c]
     488: e0822009     	add	r2, r2, r9
     48c: e58d2000     	str	r2, [sp]
     490: e59d0038     	ldr	r0, [sp, #0x38]
     494: e08a200e     	add	r2, r10, lr
     498: e59d3010     	ldr	r3, [sp, #0x10]
     49c: e1a02442     	asr	r2, r2, #8
     4a0: e59d1054     	ldr	r1, [sp, #0x54]
     4a4: e2477004     	sub	r7, r7, #4
     4a8: e080600e     	add	r6, r0, lr
     4ac: e59d0024     	ldr	r0, [sp, #0x24]
     4b0: e083300e     	add	r3, r3, lr
     4b4: e2022cff     	and	r2, r2, #65280
     4b8: e081100e     	add	r1, r1, lr
     4bc: e1a03443     	asr	r3, r3, #8
     4c0: e08ee000     	add	lr, lr, r0
     4c4: e1a01441     	asr	r1, r1, #8
     4c8: e088000c     	add	r0, r8, r12
     4cc: e1a06446     	asr	r6, r6, #8
     4d0: e7e70850     	ubfx	r0, r0, #0x10, #0x8
     4d4: e1822000     	orr	r2, r2, r0
     4d8: e08b000c     	add	r0, r11, r12
     4dc: e7e70850     	ubfx	r0, r0, #0x10, #0x8
     4e0: e2033cff     	and	r3, r3, #65280
     4e4: e1833000     	orr	r3, r3, r0
     4e8: e59d0050     	ldr	r0, [sp, #0x50]
     4ec: e2011cff     	and	r1, r1, #65280
     4f0: e2066cff     	and	r6, r6, #65280
     4f4: e3570002     	cmp	r7, #2
     4f8: e2855010     	add	r5, r5, #16
     4fc: e080000c     	add	r0, r0, r12
     500: e7e70850     	ubfx	r0, r0, #0x10, #0x8
     504: e1810000     	orr	r0, r1, r0
     508: e59d1028     	ldr	r1, [sp, #0x28]
     50c: e081100c     	add	r1, r1, r12
     510: e7e71851     	ubfx	r1, r1, #0x10, #0x8
     514: e1861001     	orr	r1, r6, r1
     518: e59d6048     	ldr	r6, [sp, #0x48]
     51c: e08cc006     	add	r12, r12, r6
     520: e0896004     	add	r6, r9, r4
     524: e20668ff     	and	r6, r6, #16711680
     528: e1822006     	orr	r2, r2, r6
     52c: e5052020     	str	r2, [r5, #-0x20]
     530: e59d2000     	ldr	r2, [sp]
     534: e0822004     	add	r2, r2, r4
     538: e20228ff     	and	r2, r2, #16711680
     53c: e1833002     	orr	r3, r3, r2
     540: e505301c     	str	r3, [r5, #-0x1c]
     544: e59d3058     	ldr	r3, [sp, #0x58]
     548: e0833004     	add	r3, r3, r4
     54c: e20338ff     	and	r3, r3, #16711680
     550: e1800003     	orr	r0, r0, r3
     554: e59d304c     	ldr	r3, [sp, #0x4c]
     558: e5050018     	str	r0, [r5, #-0x18]
     55c: e0833004     	add	r3, r3, r4
     560: e20338ff     	and	r3, r3, #16711680
     564: e1811003     	orr	r1, r1, r3
     568: e59d3020     	ldr	r3, [sp, #0x20]
     56c: e5051014     	str	r1, [r5, #-0x14]
     570: e0844003     	add	r4, r4, r3
     574: caffffc5     	bgt	0x490 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x490> @ imm = #-0xec
     578: e59dc07c     	ldr	r12, [sp, #0x7c]
     57c: e59d2000     	ldr	r2, [sp]
     580: e59d0084     	ldr	r0, [sp, #0x84]
     584: e59de080     	ldr	lr, [sp, #0x80]
     588: e59d6088     	ldr	r6, [sp, #0x88]
     58c: e082108c     	add	r1, r2, r12, lsl #1
     590: e59d2010     	ldr	r2, [sp, #0x10]
     594: e08bb080     	add	r11, r11, r0, lsl #1
     598: e59d308c     	ldr	r3, [sp, #0x8c]
     59c: e081100c     	add	r1, r1, r12
     5a0: e08bb000     	add	r11, r11, r0
     5a4: e2460003     	sub	r0, r6, #3
     5a8: e082208e     	add	r2, r2, lr, lsl #1
     5ac: e1a0c120     	lsr	r12, r0, #2
     5b0: e082200e     	add	r2, r2, lr
     5b4: e3c00003     	bic	r0, r0, #3
     5b8: e246e004     	sub	lr, r6, #4
     5bc: e04e6000     	sub	r6, lr, r0
     5c0: e59d0048     	ldr	r0, [sp, #0x48]
     5c4: e028b09c     	mla	r8, r12, r0, r11
     5c8: e59d0020     	ldr	r0, [sp, #0x20]
     5cc: e029109c     	mla	r9, r12, r0, r1
     5d0: e59d1024     	ldr	r1, [sp, #0x24]
     5d4: e02a219c     	mla	r10, r12, r1, r2
     5d8: e083c20c     	add	r12, r3, r12, lsl #4
     5dc: e3560000     	cmp	r6, #0
     5e0: ba000010     	blt	0x628 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x628> @ imm = #0x40
     5e4: e59d007c     	ldr	r0, [sp, #0x7c]
     5e8: e08c2106     	add	r2, r12, r6, lsl #2
     5ec: e59de080     	ldr	lr, [sp, #0x80]
     5f0: e24cc004     	sub	r12, r12, #4
     5f4: e59d4084     	ldr	r4, [sp, #0x84]
     5f8: e1a0344a     	asr	r3, r10, #8
     5fc: e7e71858     	ubfx	r1, r8, #0x10, #0x8
     600: e08aa00e     	add	r10, r10, lr
     604: e2033cff     	and	r3, r3, #65280
     608: e0888004     	add	r8, r8, r4
     60c: e1833001     	orr	r3, r3, r1
     610: e20918ff     	and	r1, r9, #16711680
     614: e1833001     	orr	r3, r3, r1
     618: e0899000     	add	r9, r9, r0
     61c: e5ac3004     	str	r3, [r12, #0x4]!
     620: e152000c     	cmp	r2, r12
     624: 1afffff3     	bne	0x5f8 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x5f8> @ imm = #-0x34
     628: e59d305c     	ldr	r3, [sp, #0x5c]
     62c: e59d209c     	ldr	r2, [sp, #0x9c]
     630: e0833002     	add	r3, r3, r2
     634: e3530000     	cmp	r3, #0
     638: e58d305c     	str	r3, [sp, #0x5c]
     63c: da00002a     	ble	0x6ec <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x6ec> @ imm = #0xa8
     640: e59d305c     	ldr	r3, [sp, #0x5c]
     644: e59d20c4     	ldr	r2, [sp, #0xc4]
     648: e2433801     	sub	r3, r3, #65536
     64c: e58d305c     	str	r3, [sp, #0x5c]
     650: e59d3074     	ldr	r3, [sp, #0x74]
     654: e0833002     	add	r3, r3, r2
     658: e59d20b8     	ldr	r2, [sp, #0xb8]
     65c: e58d3074     	str	r3, [sp, #0x74]
     660: e59d3060     	ldr	r3, [sp, #0x60]
     664: e0833002     	add	r3, r3, r2
     668: e59d20bc     	ldr	r2, [sp, #0xbc]
     66c: e58d3060     	str	r3, [sp, #0x60]
     670: e59d3064     	ldr	r3, [sp, #0x64]
     674: e0833002     	add	r3, r3, r2
     678: e59d20c0     	ldr	r2, [sp, #0xc0]
     67c: e58d3064     	str	r3, [sp, #0x64]
     680: e59d3068     	ldr	r3, [sp, #0x68]
     684: e0833002     	add	r3, r3, r2
     688: e58d3068     	str	r3, [sp, #0x68]
     68c: e59d306c     	ldr	r3, [sp, #0x6c]
     690: e59d2090     	ldr	r2, [sp, #0x90]
     694: e3530000     	cmp	r3, #0
     698: e59d3070     	ldr	r3, [sp, #0x70]
     69c: e0833002     	add	r3, r3, r2
     6a0: e59d2078     	ldr	r2, [sp, #0x78]
     6a4: e58d3070     	str	r3, [sp, #0x70]
     6a8: e59d3098     	ldr	r3, [sp, #0x98]
     6ac: e5933020     	ldr	r3, [r3, #0x20]
     6b0: e0823103     	add	r3, r2, r3, lsl #2
     6b4: e58d3078     	str	r3, [sp, #0x78]
     6b8: 1affff40     	bne	0x3c0 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x3c0> @ imm = #-0x300
     6bc: e59da0d8     	ldr	r10, [sp, #0xd8]
     6c0: e59d90dc     	ldr	r9, [sp, #0xdc]
     6c4: e59d3090     	ldr	r3, [sp, #0x90]
     6c8: e59db0e0     	ldr	r11, [sp, #0xe0]
     6cc: e59d80e4     	ldr	r8, [sp, #0xe4]
     6d0: e59d60e8     	ldr	r6, [sp, #0xe8]
     6d4: e02aa399     	mla	r10, r9, r3, r10
     6d8: e35b0001     	cmp	r11, #1
     6dc: 1a000045     	bne	0x7f8 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x7f8> @ imm = #0x114
     6e0: e28ddf47     	add	sp, sp, #284
     6e4: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     6e8: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     6ec: e59d3074     	ldr	r3, [sp, #0x74]
     6f0: e59d20d4     	ldr	r2, [sp, #0xd4]
     6f4: e0833002     	add	r3, r3, r2
     6f8: e59d20c8     	ldr	r2, [sp, #0xc8]
     6fc: e58d3074     	str	r3, [sp, #0x74]
     700: e59d3060     	ldr	r3, [sp, #0x60]
     704: e0833002     	add	r3, r3, r2
     708: e59d20cc     	ldr	r2, [sp, #0xcc]
     70c: e58d3060     	str	r3, [sp, #0x60]
     710: e59d3064     	ldr	r3, [sp, #0x64]
     714: e0833002     	add	r3, r3, r2
     718: e59d20d0     	ldr	r2, [sp, #0xd0]
     71c: e58d3064     	str	r3, [sp, #0x64]
     720: e59d3068     	ldr	r3, [sp, #0x68]
     724: e0833002     	add	r3, r3, r2
     728: e58d3068     	str	r3, [sp, #0x68]
     72c: eaffffd6     	b	0x68c <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x68c> @ imm = #-0xa8
     730: e59d3060     	ldr	r3, [sp, #0x60]
     734: e2865001     	add	r5, r6, #1
     738: e59d70f0     	ldr	r7, [sp, #0xf0]
     73c: e1a0400c     	mov	r4, r12
     740: e3a0e004     	mov	lr, #4
     744: eea63b90     	vdup.32	q11, r3
     748: e28d307c     	add	r3, sp, #124
     74c: f4a3980f     	vld1.32	{d9[0]}, [r3]
     750: e59d3064     	ldr	r3, [sp, #0x64]
     754: eea43b90     	vdup.32	q10, r3
     758: e28d3080     	add	r3, sp, #128
     75c: f4a3a80f     	vld1.32	{d10[0]}, [r3]
     760: e59d3068     	ldr	r3, [sp, #0x68]
     764: eea23b90     	vdup.32	q9, r3
     768: e28d3084     	add	r3, sp, #132
     76c: f4a3b80f     	vld1.32	{d11[0]}, [r3]
     770: e59d30ec     	ldr	r3, [sp, #0xec]
     774: e893000f     	ldm	r3, {r0, r1, r2, r3}
     778: e887000f     	stm	r7, {r0, r1, r2, r3}
     77c: f4670a9f     	vld1.32	{d16, d17}, [r7:64]
     780: f3e060c9     	vmla.i32	q11, q8, d9[0]
     784: f3e040ca     	vmla.i32	q10, q8, d10[0]
     788: f3e020cb     	vmla.i32	q9, q8, d11[0]
     78c: e1a0800e     	mov	r8, lr
     790: e28ee004     	add	lr, lr, #4
     794: e155000e     	cmp	r5, lr
     798: f3f80074     	vshr.u32	q8, q10, #0x8
     79c: f3b04072     	vshr.u32	q2, q9, #0x10
     7a0: f24001fe     	vand	q8, q8, q15
     7a4: f204417c     	vand	q2, q2, q14
     7a8: f20661fa     	vand	q3, q11, q13
     7ac: f26001d4     	vorr	q8, q8, q2
     7b0: f26448ce     	vadd.i32	q10, q10, q7
     7b4: f26001d6     	vorr	q8, q8, q3
     7b8: f26668cc     	vadd.i32	q11, q11, q6
     7bc: f26228e8     	vadd.i32	q9, q9, q12
     7c0: f4440a8d     	vst1.32	{d16, d17}, [r4]!
     7c4: 2afffff0     	bhs	0x78c <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x78c> @ imm = #-0x40
     7c8: e59d3060     	ldr	r3, [sp, #0x60]
     7cc: e08cc108     	add	r12, r12, r8, lsl #2
     7d0: e59d207c     	ldr	r2, [sp, #0x7c]
     7d4: e0466008     	sub	r6, r6, r8
     7d8: e0293298     	mla	r9, r8, r2, r3
     7dc: e59d3064     	ldr	r3, [sp, #0x64]
     7e0: e59d2080     	ldr	r2, [sp, #0x80]
     7e4: e02a3298     	mla	r10, r8, r2, r3
     7e8: e59d3068     	ldr	r3, [sp, #0x68]
     7ec: e59d2084     	ldr	r2, [sp, #0x84]
     7f0: e0283298     	mla	r8, r8, r2, r3
     7f4: eaffff01     	b	0x400 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x400> @ imm = #-0x3fc
     7f8: e5981004     	ldr	r1, [r8, #0x4]
     7fc: e5963004     	ldr	r3, [r6, #0x4]
     800: eeb58ac0     	vcmpe.f32	s16, #0
     804: eef1fa10     	vmrs	APSR_nzcv, fpscr
     808: e0411003     	sub	r1, r1, r3
     80c: e2819001     	add	r9, r1, #1
     810: da000027     	ble	0x8b4 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x8b4> @ imm = #0x9c
     814: e3510000     	cmp	r1, #0
     818: e596a000     	ldr	r10, [r6]
     81c: e3a04001     	mov	r4, #1
     820: c58d8100     	strgt	r8, [sp, #0x100]
     824: c58d6104     	strgt	r6, [sp, #0x104]
     828: da000026     	ble	0x8c8 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x8c8> @ imm = #0x98
     82c: e28d3038     	add	r3, sp, #56
     830: f44d8adf     	vst1.64	{d24, d25}, [sp:64]
     834: e1a0b004     	mov	r11, r4
     838: f443aadf     	vst1.64	{d26, d27}, [r3:64]
     83c: e28d3028     	add	r3, sp, #40
     840: f443cadf     	vst1.64	{d28, d29}, [r3:64]
     844: e28d3010     	add	r3, sp, #16
     848: f443eadf     	vst1.64	{d30, d31}, [r3:64]
     84c: e59d3100     	ldr	r3, [sp, #0x100]
     850: e5930000     	ldr	r0, [r3]
     854: e040000a     	sub	r0, r0, r10
     858: e1a00800     	lsl	r0, r0, #16
     85c: ebfffffe     	bl	0x85c <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x85c> @ imm = #-0x8
     860: e28d3010     	add	r3, sp, #16
     864: f46d8adf     	vld1.64	{d24, d25}, [sp:64]
     868: e58d0090     	str	r0, [sp, #0x90]
     86c: f463eadf     	vld1.64	{d30, d31}, [r3:64]
     870: e28d3028     	add	r3, sp, #40
     874: f463cadf     	vld1.64	{d28, d29}, [r3:64]
     878: e28d3038     	add	r3, sp, #56
     87c: f463aadf     	vld1.64	{d26, d27}, [r3:64]
     880: e1a0a80a     	lsl	r10, r10, #16
     884: eafffeb4     	b	0x35c <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x35c> @ imm = #-0x530
     888: e59d20fc     	ldr	r2, [sp, #0xfc]
     88c: e3a03000     	mov	r3, #0
     890: e58d309c     	str	r3, [sp, #0x9c]
     894: edcd8a32     	vstr	s17, [sp, #200]
     898: e58d20d0     	str	r2, [sp, #0xd0]
     89c: e59d20f8     	ldr	r2, [sp, #0xf8]
     8a0: e58d30d4     	str	r3, [sp, #0xd4]
     8a4: e58d20cc     	str	r2, [sp, #0xcc]
     8a8: e3a02001     	mov	r2, #1
     8ac: e58d20c4     	str	r2, [sp, #0xc4]
     8b0: eafffea0     	b	0x338 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x338> @ imm = #-0x580
     8b4: e58db05c     	str	r11, [sp, #0x5c]
     8b8: e1a02008     	mov	r2, r8
     8bc: e3a0b001     	mov	r11, #1
     8c0: e1a04006     	mov	r4, r6
     8c4: eafffe73     	b	0x298 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x298> @ imm = #-0x634
     8c8: e3590000     	cmp	r9, #0
     8cc: daffff83     	ble	0x6e0 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x6e0> @ imm = #-0x1f4
     8d0: e58db090     	str	r11, [sp, #0x90]
     8d4: e1a0a80a     	lsl	r10, r10, #16
     8d8: e1a0b004     	mov	r11, r4
     8dc: e58d8100     	str	r8, [sp, #0x100]
     8e0: e58d6104     	str	r6, [sp, #0x104]
     8e4: eafffe9e     	b	0x364 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x364> @ imm = #-0x588
     8e8: e1a01003     	mov	r1, r3
     8ec: e1a02006     	mov	r2, r6
     8f0: e1a05008     	mov	r5, r8
     8f4: eafffe53     	b	0x248 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x248> @ imm = #-0x6b4
     8f8: e59d3100     	ldr	r3, [sp, #0x100]
     8fc: e59d2104     	ldr	r2, [sp, #0x104]
     900: e5931004     	ldr	r1, [r3, #0x4]
     904: e5923004     	ldr	r3, [r2, #0x4]
     908: e592a000     	ldr	r10, [r2]
     90c: e0411003     	sub	r1, r1, r3
     910: e3510000     	cmp	r1, #0
     914: c3a03000     	movgt	r3, #0
     918: c1a0400b     	movgt	r4, r11
     91c: c58d305c     	strgt	r3, [sp, #0x5c]
     920: caffffc1     	bgt	0x82c <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x82c> @ imm = #-0xfc
     924: e3a03000     	mov	r3, #0
     928: e58d305c     	str	r3, [sp, #0x5c]
     92c: e58d3090     	str	r3, [sp, #0x90]
     930: eaffffd2     	b	0x880 <ZB_fillTriangleSmoothNOBLEND_DT0_DW0+0x880> @ imm = #-0xb8
     934: e4 08 00 00  	.word	0x000008e4
     938: bc 06 00 00  	.word	0x000006bc
     93c: 00 00 00 00  	.word	0x00000000

Disassembly of section .text.ZB_fillTriangleSmoothNOBLEND_DT1_DW0:

00000000 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a07003     	mov	r7, r3
       8: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
       c: e24ddf5d     	sub	sp, sp, #372
      10: e5923004     	ldr	r3, [r2, #0x4]
      14: e58d00dc     	str	r0, [sp, #0xdc]
      18: e5910004     	ldr	r0, [r1, #0x4]
      1c: e597e004     	ldr	lr, [r7, #0x4]
      20: e59fcaf0     	ldr	r12, [pc, #0xaf0]       @ 0xb18 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0xb18>
      24: e1530000     	cmp	r3, r0
      28: a1a04000     	movge	r4, r0
      2c: b1a04003     	movlt	r4, r3
      30: a1a00003     	movge	r0, r3
      34: b1a06002     	movlt	r6, r2
      38: a1a06001     	movge	r6, r1
      3c: b1a08001     	movlt	r8, r1
      40: a1a08002     	movge	r8, r2
      44: e15e0004     	cmp	lr, r4
      48: e08fc00c     	add	r12, pc, r12
      4c: e1a03004     	mov	r3, r4
      50: ba00000b     	blt	0x84 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x84> @ imm = #0x2c
      54: e15e0000     	cmp	lr, r0
      58: e1a02007     	mov	r2, r7
      5c: a1a03000     	movge	r3, r0
      60: a1a07006     	movge	r7, r6
      64: a1a0000e     	movge	r0, lr
      68: a1a06008     	movge	r6, r8
      6c: b1a0300e     	movlt	r3, lr
      70: b1a07006     	movlt	r7, r6
      74: a1a0e004     	movge	lr, r4
      78: a1a08002     	movge	r8, r2
      7c: b1a0e004     	movlt	lr, r4
      80: b1a06002     	movlt	r6, r2
      84: e040200e     	sub	r2, r0, lr
      88: e5971000     	ldr	r1, [r7]
      8c: e5975008     	ldr	r5, [r7, #0x8]
      90: ee072a10     	vmov	s14, r2
      94: e5982000     	ldr	r2, [r8]
      98: eeb87ac7     	vcvt.f32.s32	s14, s14
      9c: e5974014     	ldr	r4, [r7, #0x14]
      a0: e0422001     	sub	r2, r2, r1
      a4: e5970018     	ldr	r0, [r7, #0x18]
      a8: ee072a90     	vmov	s15, r2
      ac: e043200e     	sub	r2, r3, lr
      b0: ee062a90     	vmov	s13, r2
      b4: e5962000     	ldr	r2, [r6]
      b8: eef87ae7     	vcvt.f32.s32	s15, s15
      bc: eef86ae6     	vcvt.f32.s32	s13, s13
      c0: e0422001     	sub	r2, r2, r1
      c4: e597101c     	ldr	r1, [r7, #0x1c]
      c8: ee052a90     	vmov	s11, r2
      cc: ee268aa7     	vmul.f32	s16, s13, s15
      d0: eef85ae5     	vcvt.f32.s32	s11, s11
      d4: e5982008     	ldr	r2, [r8, #0x8]
      d8: ee158a87     	vnmls.f32	s16, s11, s14
      dc: e0422005     	sub	r2, r2, r5
      e0: eeb58a40     	vcmp.f32	s16, #0
      e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
      e8: 1eb76a00     	vmovne.f32	s12, #1.000000e+00
      ec: 1e868a08     	vdivne.f32	s16, s12, s16
      f0: ee062a10     	vmov	s12, r2
      f4: e5982014     	ldr	r2, [r8, #0x14]
      f8: eeb82ac6     	vcvt.f32.s32	s4, s12
      fc: e0422004     	sub	r2, r2, r4
     100: ee062a10     	vmov	s12, r2
     104: e5982018     	ldr	r2, [r8, #0x18]
     108: eeb83ac6     	vcvt.f32.s32	s6, s12
     10c: e0422000     	sub	r2, r2, r0
     110: ee062a10     	vmov	s12, r2
     114: e598201c     	ldr	r2, [r8, #0x1c]
     118: eeb84ac6     	vcvt.f32.s32	s8, s12
     11c: ee686a26     	vmul.f32	s13, s16, s13
     120: e0422001     	sub	r2, r2, r1
     124: ee287a07     	vmul.f32	s14, s16, s14
     128: ee687a27     	vmul.f32	s15, s16, s15
     12c: ee062a10     	vmov	s12, r2
     130: e5962008     	ldr	r2, [r6, #0x8]
     134: eeb85ac6     	vcvt.f32.s32	s10, s12
     138: ee662a84     	vmul.f32	s5, s13, s8
     13c: e0422005     	sub	r2, r2, r5
     140: ee664a82     	vmul.f32	s9, s13, s4
     144: ee663a83     	vmul.f32	s7, s13, s6
     148: ee266a85     	vmul.f32	s12, s13, s10
     14c: ee062a90     	vmov	s13, r2
     150: ee685a25     	vmul.f32	s11, s16, s11
     154: eef86ae6     	vcvt.f32.s32	s13, s13
     158: e5962014     	ldr	r2, [r6, #0x14]
     15c: eeb58ac0     	vcmpe.f32	s16, #0
     160: eef1fa10     	vmrs	APSR_nzcv, fpscr
     164: ee574a26     	vnmls.f32	s9, s14, s13
     168: e0422004     	sub	r2, r2, r4
     16c: ee676aa6     	vmul.f32	s13, s15, s13
     170: ee556a82     	vnmls.f32	s13, s11, s4
     174: eefd6ae6     	vcvt.s32.f32	s13, s13
     178: eef08a66     	vmov.f32	s17, s13
     17c: ee062a90     	vmov	s13, r2
     180: e5962018     	ldr	r2, [r6, #0x18]
     184: eef86ae6     	vcvt.f32.s32	s13, s13
     188: e0422000     	sub	r2, r2, r0
     18c: ee573a26     	vnmls.f32	s7, s14, s13
     190: ee676aa6     	vmul.f32	s13, s15, s13
     194: ee556a83     	vnmls.f32	s13, s11, s6
     198: eefd6ae6     	vcvt.s32.f32	s13, s13
     19c: edcd6a54     	vstr	s13, [sp, #336]
     1a0: ee062a90     	vmov	s13, r2
     1a4: e596201c     	ldr	r2, [r6, #0x1c]
     1a8: eef86ae6     	vcvt.f32.s32	s13, s13
     1ac: e0422001     	sub	r2, r2, r1
     1b0: e59d10dc     	ldr	r1, [sp, #0xdc]
     1b4: ee572a26     	vnmls.f32	s5, s14, s13
     1b8: ee676aa6     	vmul.f32	s13, s15, s13
     1bc: ee556a84     	vnmls.f32	s13, s11, s8
     1c0: eefd6ae6     	vcvt.s32.f32	s13, s13
     1c4: edcd6a55     	vstr	s13, [sp, #340]
     1c8: ee062a90     	vmov	s13, r2
     1cc: eeb84ae6     	vcvt.f32.s32	s8, s13
     1d0: e5912020     	ldr	r2, [r1, #0x20]
     1d4: eef06a46     	vmov.f32	s13, s12
     1d8: ee677a84     	vmul.f32	s15, s15, s8
     1dc: ee576a04     	vnmls.f32	s13, s14, s8
     1e0: e00e0e92     	mul	lr, r2, lr
     1e4: e5912004     	ldr	r2, [r1, #0x4]
     1e8: ee557a85     	vnmls.f32	s15, s11, s10
     1ec: e082210e     	add	r2, r2, lr, lsl #2
     1f0: e58d209c     	str	r2, [sp, #0x9c]
     1f4: e5912000     	ldr	r2, [r1]
     1f8: eefd7ae7     	vcvt.s32.f32	s15, s15
     1fc: e082208e     	add	r2, r2, lr, lsl #1
     200: edcd7a56     	vstr	s15, [sp, #344]
     204: eefd7ae3     	vcvt.s32.f32	s15, s7
     208: e58d20a0     	str	r2, [sp, #0xa0]
     20c: edcd7a29     	vstr	s15, [sp, #164]
     210: eefd7ae2     	vcvt.s32.f32	s15, s5
     214: edcd7a2a     	vstr	s15, [sp, #168]
     218: eefd7ae6     	vcvt.s32.f32	s15, s13
     21c: edcd7a2b     	vstr	s15, [sp, #172]
     220: eefd7ae4     	vcvt.s32.f32	s15, s9
     224: ee175a90     	vmov	r5, s15
     228: e1a02105     	lsl	r2, r5, #2
     22c: e1a0a085     	lsl	r10, r5, #1
     230: e58d20b0     	str	r2, [sp, #0xb0]
     234: eeac2b10     	vdup.32	q6, r2
     238: e59d20a4     	ldr	r2, [sp, #0xa4]
     23c: e1a01102     	lsl	r1, r2, #2
     240: e1a04082     	lsl	r4, r2, #1
     244: e58d1014     	str	r1, [sp, #0x14]
     248: e0842002     	add	r2, r4, r2
     24c: eeae1b10     	vdup.32	q7, r1
     250: e59d10a8     	ldr	r1, [sp, #0xa8]
     254: e1a00101     	lsl	r0, r1, #2
     258: e58d0018     	str	r0, [sp, #0x18]
     25c: eeac0b90     	vdup.32	q14, r0
     260: e59d00ac     	ldr	r0, [sp, #0xac]
     264: e58d20f8     	str	r2, [sp, #0xf8]
     268: e58d40ec     	str	r4, [sp, #0xec]
     26c: e58da0b4     	str	r10, [sp, #0xb4]
     270: e1a0e100     	lsl	lr, r0, #2
     274: e1a09080     	lsl	r9, r0, #1
     278: e58de01c     	str	lr, [sp, #0x1c]
     27c: e0892000     	add	r2, r9, r0
     280: eeaeeb90     	vdup.32	q15, lr
     284: e1a0e081     	lsl	lr, r1, #1
     288: e58d90e4     	str	r9, [sp, #0xe4]
     28c: e08e1001     	add	r1, lr, r1
     290: e58de0e8     	str	lr, [sp, #0xe8]
     294: e58d10f4     	str	r1, [sp, #0xf4]
     298: e58d20f0     	str	r2, [sp, #0xf0]
     29c: da00020a     	ble	0xacc <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0xacc> @ imm = #0x828
     2a0: e5981004     	ldr	r1, [r8, #0x4]
     2a4: e1a02008     	mov	r2, r8
     2a8: e58d614c     	str	r6, [sp, #0x14c]
     2ac: e5970004     	ldr	r0, [r7, #0x4]
     2b0: e3a0b000     	mov	r11, #0
     2b4: f387225f     	vmov.i32	q1, #0xff00
     2b8: e1a04007     	mov	r4, r7
     2bc: f387405f     	vmov.i32	q2, #0xff
     2c0: e1a0a00b     	mov	r10, r11
     2c4: e0439000     	sub	r9, r3, r0
     2c8: e59f384c     	ldr	r3, [pc, #0x84c]        @ 0xb1c <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0xb1c>
     2cc: f387645f     	vmov.i32	q3, #0xff0000
     2d0: e0411000     	sub	r1, r1, r0
     2d4: e58db0d4     	str	r11, [sp, #0xd4]
     2d8: e08f3003     	add	r3, pc, r3
     2dc: e58d715c     	str	r7, [sp, #0x15c]
     2e0: e58d3140     	str	r3, [sp, #0x140]
     2e4: e3a03001     	mov	r3, #1
     2e8: e58d307c     	str	r3, [sp, #0x7c]
     2ec: e28d3e16     	add	r3, sp, #352
     2f0: e58d3144     	str	r3, [sp, #0x144]
     2f4: e58dc148     	str	r12, [sp, #0x148]
     2f8: e5940000     	ldr	r0, [r4]
     2fc: e3510000     	cmp	r1, #0
     300: e58d0098     	str	r0, [sp, #0x98]
     304: da0001d6     	ble	0xa64 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0xa64> @ imm = #0x758
     308: e28d3050     	add	r3, sp, #80
     30c: f40d2adf     	vst1.64	{d2, d3}, [sp:64]
     310: f443eadf     	vst1.64	{d30, d31}, [r3:64]
     314: e28d3040     	add	r3, sp, #64
     318: f443cadf     	vst1.64	{d28, d29}, [r3:64]
     31c: e28d3030     	add	r3, sp, #48
     320: f4036adf     	vst1.64	{d6, d7}, [r3:64]
     324: e28d3020     	add	r3, sp, #32
     328: f4034adf     	vst1.64	{d4, d5}, [r3:64]
     32c: e5923000     	ldr	r3, [r2]
     330: e0433000     	sub	r3, r3, r0
     334: e1a00803     	lsl	r0, r3, #16
     338: ebfffffe     	bl	0x338 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x338> @ imm = #-0x8
     33c: e6ff2070     	uxth	r2, r0
     340: e1a03840     	asr	r3, r0, #16
     344: e59d10a4     	ldr	r1, [sp, #0xa4]
     348: e58d20e0     	str	r2, [sp, #0xe0]
     34c: ee182a90     	vmov	r2, s17
     350: e58d3128     	str	r3, [sp, #0x128]
     354: e0222593     	mla	r2, r3, r5, r2
     358: f42d2adf     	vld1.64	{d2, d3}, [sp:64]
     35c: e58d2110     	str	r2, [sp, #0x110]
     360: e59d2150     	ldr	r2, [sp, #0x150]
     364: e0222193     	mla	r2, r3, r1, r2
     368: e59d10a8     	ldr	r1, [sp, #0xa8]
     36c: e58d2114     	str	r2, [sp, #0x114]
     370: e59d2154     	ldr	r2, [sp, #0x154]
     374: e0222193     	mla	r2, r3, r1, r2
     378: e59d10ac     	ldr	r1, [sp, #0xac]
     37c: e58d2118     	str	r2, [sp, #0x118]
     380: e59d2158     	ldr	r2, [sp, #0x158]
     384: e0222193     	mla	r2, r3, r1, r2
     388: e2833001     	add	r3, r3, #1
     38c: e58d310c     	str	r3, [sp, #0x10c]
     390: e28d3020     	add	r3, sp, #32
     394: e58d211c     	str	r2, [sp, #0x11c]
     398: f4234adf     	vld1.64	{d4, d5}, [r3:64]
     39c: e28d3030     	add	r3, sp, #48
     3a0: f4236adf     	vld1.64	{d6, d7}, [r3:64]
     3a4: e28d3040     	add	r3, sp, #64
     3a8: f463cadf     	vld1.64	{d28, d29}, [r3:64]
     3ac: e28d3050     	add	r3, sp, #80
     3b0: f463eadf     	vld1.64	{d30, d31}, [r3:64]
     3b4: e59d307c     	ldr	r3, [sp, #0x7c]
     3b8: e3530000     	cmp	r3, #0
     3bc: e5943008     	ldr	r3, [r4, #0x8]
     3c0: e58d3080     	str	r3, [sp, #0x80]
     3c4: e5943014     	ldr	r3, [r4, #0x14]
     3c8: e58d3084     	str	r3, [sp, #0x84]
     3cc: e5943018     	ldr	r3, [r4, #0x18]
     3d0: e58d3088     	str	r3, [sp, #0x88]
     3d4: e594301c     	ldr	r3, [r4, #0x1c]
     3d8: e58d308c     	str	r3, [sp, #0x8c]
     3dc: 1a0001be     	bne	0xadc <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0xadc> @ imm = #0x6f8
     3e0: e3590000     	cmp	r9, #0
     3e4: da00011a     	ble	0x854 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x854> @ imm = #0x468
     3e8: e59d2148     	ldr	r2, [sp, #0x148]
     3ec: e59f372c     	ldr	r3, [pc, #0x72c]        @ 0xb20 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0xb20>
     3f0: e7923003     	ldr	r3, [r2, r3]
     3f4: e59d2114     	ldr	r2, [sp, #0x114]
     3f8: e58db12c     	str	r11, [sp, #0x12c]
     3fc: e1a0b005     	mov	r11, r5
     400: e58d30d8     	str	r3, [sp, #0xd8]
     404: e59d3110     	ldr	r3, [sp, #0x110]
     408: e58d9090     	str	r9, [sp, #0x90]
     40c: e58da094     	str	r10, [sp, #0x94]
     410: e0853003     	add	r3, r5, r3
     414: e58d9130     	str	r9, [sp, #0x130]
     418: e58d30fc     	str	r3, [sp, #0xfc]
     41c: e59d30a4     	ldr	r3, [sp, #0xa4]
     420: e58da134     	str	r10, [sp, #0x134]
     424: e58d8138     	str	r8, [sp, #0x138]
     428: e0833002     	add	r3, r3, r2
     42c: e59d2118     	ldr	r2, [sp, #0x118]
     430: e58d3100     	str	r3, [sp, #0x100]
     434: e59d30a8     	ldr	r3, [sp, #0xa8]
     438: e58d613c     	str	r6, [sp, #0x13c]
     43c: e0833002     	add	r3, r3, r2
     440: e59d211c     	ldr	r2, [sp, #0x11c]
     444: e58d3104     	str	r3, [sp, #0x104]
     448: e59d30ac     	ldr	r3, [sp, #0xac]
     44c: e0833002     	add	r3, r3, r2
     450: e58d3108     	str	r3, [sp, #0x108]
     454: e59d3090     	ldr	r3, [sp, #0x90]
     458: e59d2098     	ldr	r2, [sp, #0x98]
     45c: e59d1094     	ldr	r1, [sp, #0x94]
     460: e2433001     	sub	r3, r3, #1
     464: e59d0080     	ldr	r0, [sp, #0x80]
     468: e58d3090     	str	r3, [sp, #0x90]
     46c: e59d30d8     	ldr	r3, [sp, #0xd8]
     470: e062a841     	rsb	r10, r2, r1, asr #16
     474: e5933000     	ldr	r3, [r3]
     478: e3530000     	cmp	r3, #0
     47c: 135a000e     	cmpne	r10, #14
     480: e59d309c     	ldr	r3, [sp, #0x9c]
     484: e083e102     	add	lr, r3, r2, lsl #2
     488: e59d30a0     	ldr	r3, [sp, #0xa0]
     48c: e083c082     	add	r12, r3, r2, lsl #1
     490: ca000109     	bgt	0x8bc <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x8bc> @ imm = #0x424
     494: e59d908c     	ldr	r9, [sp, #0x8c]
     498: e59d7088     	ldr	r7, [sp, #0x88]
     49c: e59d8084     	ldr	r8, [sp, #0x84]
     4a0: e35a0002     	cmp	r10, #2
     4a4: da0000a0     	ble	0x72c <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x72c> @ imm = #0x280
     4a8: e24a3003     	sub	r3, r10, #3
     4ac: e28c1010     	add	r1, r12, #16
     4b0: e1a02123     	lsr	r2, r3, #2
     4b4: e28c3008     	add	r3, r12, #8
     4b8: e58d8074     	str	r8, [sp, #0x74]
     4bc: e28e5010     	add	r5, lr, #16
     4c0: e1a0c182     	lsl	r12, r2, #3
     4c4: e3a0e000     	mov	lr, #0
     4c8: e1a0400e     	mov	r4, lr
     4cc: e58d50c0     	str	r5, [sp, #0xc0]
     4d0: e081100c     	add	r1, r1, r12
     4d4: e58dc0b8     	str	r12, [sp, #0xb8]
     4d8: e58d1030     	str	r1, [sp, #0x30]
     4dc: e1a0c00e     	mov	r12, lr
     4e0: e59d10b4     	ldr	r1, [sp, #0xb4]
     4e4: e58d30c4     	str	r3, [sp, #0xc4]
     4e8: e58da0c8     	str	r10, [sp, #0xc8]
     4ec: e0806001     	add	r6, r0, r1
     4f0: e59d10e8     	ldr	r1, [sp, #0xe8]
     4f4: e58d60bc     	str	r6, [sp, #0xbc]
     4f8: e58d9078     	str	r9, [sp, #0x78]
     4fc: e0871001     	add	r1, r7, r1
     500: e58d20cc     	str	r2, [sp, #0xcc]
     504: e58d1064     	str	r1, [sp, #0x64]
     508: e59d10ec     	ldr	r1, [sp, #0xec]
     50c: e58d30d0     	str	r3, [sp, #0xd0]
     510: e0881001     	add	r1, r8, r1
     514: e58d1068     	str	r1, [sp, #0x68]
     518: e59d10e4     	ldr	r1, [sp, #0xe4]
     51c: e0891001     	add	r1, r9, r1
     520: e58d1060     	str	r1, [sp, #0x60]
     524: e59d10f4     	ldr	r1, [sp, #0xf4]
     528: e0871001     	add	r1, r7, r1
     52c: e58d1020     	str	r1, [sp, #0x20]
     530: e59d10f8     	ldr	r1, [sp, #0xf8]
     534: e0881001     	add	r1, r8, r1
     538: e58d1070     	str	r1, [sp, #0x70]
     53c: e59d10f0     	ldr	r1, [sp, #0xf0]
     540: e0891001     	add	r1, r9, r1
     544: e58d106c     	str	r1, [sp, #0x6c]
     548: e59d10a8     	ldr	r1, [sp, #0xa8]
     54c: e0811007     	add	r1, r1, r7
     550: e58d1000     	str	r1, [sp]
     554: e59d10ac     	ldr	r1, [sp, #0xac]
     558: e0811009     	add	r1, r1, r9
     55c: e58d1050     	str	r1, [sp, #0x50]
     560: e59d10a4     	ldr	r1, [sp, #0xa4]
     564: e0811008     	add	r1, r1, r8
     568: e59d80b0     	ldr	r8, [sp, #0xb0]
     56c: e58d1040     	str	r1, [sp, #0x40]
     570: e1a01003     	mov	r1, r3
     574: e15130b8     	ldrh	r3, [r1, #-8]
     578: e087200c     	add	r2, r7, r12
     57c: e1a02442     	asr	r2, r2, #8
     580: e08b9000     	add	r9, r11, r0
     584: e2022cff     	and	r2, r2, #65280
     588: e1530720     	cmp	r3, r0, lsr #14
     58c: e59d3000     	ldr	r3, [sp]
     590: e083300c     	add	r3, r3, r12
     594: e1a03443     	asr	r3, r3, #8
     598: 8a000008     	bhi	0x5c0 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x5c0> @ imm = #0x20
     59c: e59da078     	ldr	r10, [sp, #0x78]
     5a0: e08aa00e     	add	r10, r10, lr
     5a4: e7e7a85a     	ubfx	r10, r10, #0x10, #0x8
     5a8: e182200a     	orr	r2, r2, r10
     5ac: e59da074     	ldr	r10, [sp, #0x74]
     5b0: e08aa004     	add	r10, r10, r4
     5b4: e20aa8ff     	and	r10, r10, #16711680
     5b8: e182200a     	orr	r2, r2, r10
     5bc: e5052010     	str	r2, [r5, #-0x10]
     5c0: e15120b6     	ldrh	r2, [r1, #-6]
     5c4: e2033cff     	and	r3, r3, #65280
     5c8: e0800008     	add	r0, r0, r8
     5cc: e1520729     	cmp	r2, r9, lsr #14
     5d0: 8a000008     	bhi	0x5f8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x5f8> @ imm = #0x20
     5d4: e59d2050     	ldr	r2, [sp, #0x50]
     5d8: e082200e     	add	r2, r2, lr
     5dc: e7e72852     	ubfx	r2, r2, #0x10, #0x8
     5e0: e1833002     	orr	r3, r3, r2
     5e4: e59d2040     	ldr	r2, [sp, #0x40]
     5e8: e0822004     	add	r2, r2, r4
     5ec: e20228ff     	and	r2, r2, #16711680
     5f0: e1833002     	orr	r3, r3, r2
     5f4: e505300c     	str	r3, [r5, #-0xc]
     5f8: e15130b4     	ldrh	r3, [r1, #-4]
     5fc: e1530726     	cmp	r3, r6, lsr #14
     600: 8a00000c     	bhi	0x638 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x638> @ imm = #0x30
     604: e59d3064     	ldr	r3, [sp, #0x64]
     608: e59d2060     	ldr	r2, [sp, #0x60]
     60c: e083300c     	add	r3, r3, r12
     610: e1a03443     	asr	r3, r3, #8
     614: e082200e     	add	r2, r2, lr
     618: e7e72852     	ubfx	r2, r2, #0x10, #0x8
     61c: e2033cff     	and	r3, r3, #65280
     620: e1833002     	orr	r3, r3, r2
     624: e59d2068     	ldr	r2, [sp, #0x68]
     628: e0822004     	add	r2, r2, r4
     62c: e20228ff     	and	r2, r2, #16711680
     630: e1833002     	orr	r3, r3, r2
     634: e5053008     	str	r3, [r5, #-0x8]
     638: e15120b2     	ldrh	r2, [r1, #-2]
     63c: e2811008     	add	r1, r1, #8
     640: e59d9018     	ldr	r9, [sp, #0x18]
     644: e59d3020     	ldr	r3, [sp, #0x20]
     648: e083300c     	add	r3, r3, r12
     64c: e08cc009     	add	r12, r12, r9
     650: e08b9006     	add	r9, r11, r6
     654: e1a03443     	asr	r3, r3, #8
     658: e1520729     	cmp	r2, r9, lsr #14
     65c: e0866008     	add	r6, r6, r8
     660: e2033cff     	and	r3, r3, #65280
     664: 8a000008     	bhi	0x68c <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x68c> @ imm = #0x20
     668: e59d206c     	ldr	r2, [sp, #0x6c]
     66c: e082200e     	add	r2, r2, lr
     670: e7e72852     	ubfx	r2, r2, #0x10, #0x8
     674: e1833002     	orr	r3, r3, r2
     678: e59d2070     	ldr	r2, [sp, #0x70]
     67c: e0822004     	add	r2, r2, r4
     680: e20228ff     	and	r2, r2, #16711680
     684: e1833002     	orr	r3, r3, r2
     688: e5053004     	str	r3, [r5, #-0x4]
     68c: e59d3030     	ldr	r3, [sp, #0x30]
     690: e2855010     	add	r5, r5, #16
     694: e1530001     	cmp	r3, r1
     698: e59d3014     	ldr	r3, [sp, #0x14]
     69c: e0844003     	add	r4, r4, r3
     6a0: e59d301c     	ldr	r3, [sp, #0x1c]
     6a4: e08ee003     	add	lr, lr, r3
     6a8: 1affffb1     	bne	0x574 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x574> @ imm = #-0x13c
     6ac: e59de0a4     	ldr	lr, [sp, #0xa4]
     6b0: e59d30d0     	ldr	r3, [sp, #0xd0]
     6b4: e59d10b8     	ldr	r1, [sp, #0xb8]
     6b8: e59d0040     	ldr	r0, [sp, #0x40]
     6bc: e59d20cc     	ldr	r2, [sp, #0xcc]
     6c0: e081c003     	add	r12, r1, r3
     6c4: e59d10ac     	ldr	r1, [sp, #0xac]
     6c8: e59d3050     	ldr	r3, [sp, #0x50]
     6cc: e080808e     	add	r8, r0, lr, lsl #1
     6d0: e59d40a8     	ldr	r4, [sp, #0xa8]
     6d4: e59d0000     	ldr	r0, [sp]
     6d8: e088800e     	add	r8, r8, lr
     6dc: e59de01c     	ldr	lr, [sp, #0x1c]
     6e0: e0833081     	add	r3, r3, r1, lsl #1
     6e4: e59da0c8     	ldr	r10, [sp, #0xc8]
     6e8: e0833001     	add	r3, r3, r1
     6ec: e0807084     	add	r7, r0, r4, lsl #1
     6f0: e59d10b4     	ldr	r1, [sp, #0xb4]
     6f4: e0293e92     	mla	r9, r2, lr, r3
     6f8: e59d3014     	ldr	r3, [sp, #0x14]
     6fc: e59d00bc     	ldr	r0, [sp, #0xbc]
     700: e0877004     	add	r7, r7, r4
     704: e0288392     	mla	r8, r2, r3, r8
     708: e59d3018     	ldr	r3, [sp, #0x18]
     70c: e0810000     	add	r0, r1, r0
     710: e24a1004     	sub	r1, r10, #4
     714: e0277392     	mla	r7, r2, r3, r7
     718: e59d30b0     	ldr	r3, [sp, #0xb0]
     71c: e041a102     	sub	r10, r1, r2, lsl #2
     720: e0200392     	mla	r0, r2, r3, r0
     724: e59d30c0     	ldr	r3, [sp, #0xc0]
     728: e083e202     	add	lr, r3, r2, lsl #4
     72c: e35a0000     	cmp	r10, #0
     730: ba000013     	blt	0x784 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x784> @ imm = #0x4c
     734: e28d40a4     	add	r4, sp, #164
     738: e08c108a     	add	r1, r12, r10, lsl #1
     73c: e8940070     	ldm	r4, {r4, r5, r6}
     740: e24c2002     	sub	r2, r12, #2
     744: e1a03447     	asr	r3, r7, #8
     748: e7e7a859     	ubfx	r10, r9, #0x10, #0x8
     74c: e1f2c0b2     	ldrh	r12, [r2, #2]!
     750: e0877005     	add	r7, r7, r5
     754: e2033cff     	and	r3, r3, #65280
     758: e0899006     	add	r9, r9, r6
     75c: e183300a     	orr	r3, r3, r10
     760: e208a8ff     	and	r10, r8, #16711680
     764: e183300a     	orr	r3, r3, r10
     768: e0888004     	add	r8, r8, r4
     76c: e15c0720     	cmp	r12, r0, lsr #14
     770: e28ee004     	add	lr, lr, #4
     774: e080000b     	add	r0, r0, r11
     778: 950e3004     	strls	r3, [lr, #-0x4]
     77c: e1510002     	cmp	r1, r2
     780: 1affffef     	bne	0x744 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x744> @ imm = #-0x44
     784: e59d307c     	ldr	r3, [sp, #0x7c]
     788: e59d20e0     	ldr	r2, [sp, #0xe0]
     78c: e0833002     	add	r3, r3, r2
     790: e3530000     	cmp	r3, #0
     794: e58d307c     	str	r3, [sp, #0x7c]
     798: da000032     	ble	0x868 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x868> @ imm = #0xc8
     79c: e59d307c     	ldr	r3, [sp, #0x7c]
     7a0: e59d210c     	ldr	r2, [sp, #0x10c]
     7a4: e2433801     	sub	r3, r3, #65536
     7a8: e58d307c     	str	r3, [sp, #0x7c]
     7ac: e59d3098     	ldr	r3, [sp, #0x98]
     7b0: e0833002     	add	r3, r3, r2
     7b4: e59d20fc     	ldr	r2, [sp, #0xfc]
     7b8: e58d3098     	str	r3, [sp, #0x98]
     7bc: e59d3080     	ldr	r3, [sp, #0x80]
     7c0: e0833002     	add	r3, r3, r2
     7c4: e59d2100     	ldr	r2, [sp, #0x100]
     7c8: e58d3080     	str	r3, [sp, #0x80]
     7cc: e59d3084     	ldr	r3, [sp, #0x84]
     7d0: e0833002     	add	r3, r3, r2
     7d4: e59d2104     	ldr	r2, [sp, #0x104]
     7d8: e58d3084     	str	r3, [sp, #0x84]
     7dc: e59d3088     	ldr	r3, [sp, #0x88]
     7e0: e0833002     	add	r3, r3, r2
     7e4: e59d2108     	ldr	r2, [sp, #0x108]
     7e8: e58d3088     	str	r3, [sp, #0x88]
     7ec: e59d308c     	ldr	r3, [sp, #0x8c]
     7f0: e0833002     	add	r3, r3, r2
     7f4: e58d308c     	str	r3, [sp, #0x8c]
     7f8: e59d3090     	ldr	r3, [sp, #0x90]
     7fc: e59d20d4     	ldr	r2, [sp, #0xd4]
     800: e3530000     	cmp	r3, #0
     804: e59d3094     	ldr	r3, [sp, #0x94]
     808: e0833002     	add	r3, r3, r2
     80c: e59d209c     	ldr	r2, [sp, #0x9c]
     810: e58d3094     	str	r3, [sp, #0x94]
     814: e59d30dc     	ldr	r3, [sp, #0xdc]
     818: e5933020     	ldr	r3, [r3, #0x20]
     81c: e0822103     	add	r2, r2, r3, lsl #2
     820: e58d209c     	str	r2, [sp, #0x9c]
     824: e59d20a0     	ldr	r2, [sp, #0xa0]
     828: e0823083     	add	r3, r2, r3, lsl #1
     82c: e58d30a0     	str	r3, [sp, #0xa0]
     830: 1affff07     	bne	0x454 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x454> @ imm = #-0x3e4
     834: e59d9130     	ldr	r9, [sp, #0x130]
     838: e1a0500b     	mov	r5, r11
     83c: e59da134     	ldr	r10, [sp, #0x134]
     840: e59d30d4     	ldr	r3, [sp, #0xd4]
     844: e59db12c     	ldr	r11, [sp, #0x12c]
     848: e59d8138     	ldr	r8, [sp, #0x138]
     84c: e59d613c     	ldr	r6, [sp, #0x13c]
     850: e02aa399     	mla	r10, r9, r3, r10
     854: e35b0001     	cmp	r11, #1
     858: 1a000059     	bne	0x9c4 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x9c4> @ imm = #0x164
     85c: e28ddf5d     	add	sp, sp, #372
     860: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     864: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     868: e59d3098     	ldr	r3, [sp, #0x98]
     86c: e59d2128     	ldr	r2, [sp, #0x128]
     870: e0833002     	add	r3, r3, r2
     874: e59d2110     	ldr	r2, [sp, #0x110]
     878: e58d3098     	str	r3, [sp, #0x98]
     87c: e59d3080     	ldr	r3, [sp, #0x80]
     880: e0833002     	add	r3, r3, r2
     884: e59d2114     	ldr	r2, [sp, #0x114]
     888: e58d3080     	str	r3, [sp, #0x80]
     88c: e59d3084     	ldr	r3, [sp, #0x84]
     890: e0833002     	add	r3, r3, r2
     894: e59d2118     	ldr	r2, [sp, #0x118]
     898: e58d3084     	str	r3, [sp, #0x84]
     89c: e59d3088     	ldr	r3, [sp, #0x88]
     8a0: e0833002     	add	r3, r3, r2
     8a4: e59d211c     	ldr	r2, [sp, #0x11c]
     8a8: e58d3088     	str	r3, [sp, #0x88]
     8ac: e59d308c     	ldr	r3, [sp, #0x8c]
     8b0: e0833002     	add	r3, r3, r2
     8b4: e58d308c     	str	r3, [sp, #0x8c]
     8b8: eaffffce     	b	0x7f8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x7f8> @ imm = #-0xc8
     8bc: e59d3084     	ldr	r3, [sp, #0x84]
     8c0: e28a7001     	add	r7, r10, #1
     8c4: e59d8144     	ldr	r8, [sp, #0x144]
     8c8: e1a0600c     	mov	r6, r12
     8cc: eeaa0b90     	vdup.32	q13, r0
     8d0: e1a0500e     	mov	r5, lr
     8d4: e3a04004     	mov	r4, #4
     8d8: eea83b90     	vdup.32	q12, r3
     8dc: e28d30a4     	add	r3, sp, #164
     8e0: f4a3980f     	vld1.32	{d9[0]}, [r3]
     8e4: e59d3088     	ldr	r3, [sp, #0x88]
     8e8: ed9d1b48     	vldr	d1, [sp, #288]
     8ec: ee01bb10     	vmov.32	d1[0], r11
     8f0: eea63b90     	vdup.32	q11, r3
     8f4: e28d30a8     	add	r3, sp, #168
     8f8: f4a3a80f     	vld1.32	{d10[0]}, [r3]
     8fc: e59d308c     	ldr	r3, [sp, #0x8c]
     900: ed8d1b48     	vstr	d1, [sp, #288]
     904: eea43b90     	vdup.32	q10, r3
     908: e28d30ac     	add	r3, sp, #172
     90c: f4a3b80f     	vld1.32	{d11[0]}, [r3]
     910: e59d3140     	ldr	r3, [sp, #0x140]
     914: e893000f     	ldm	r3, {r0, r1, r2, r3}
     918: e888000f     	stm	r8, {r0, r1, r2, r3}
     91c: f4680a9f     	vld1.32	{d16, d17}, [r8:64]
     920: f3e0a0c1     	vmla.i32	q13, q8, d1[0]
     924: f3e080c9     	vmla.i32	q12, q8, d9[0]
     928: f3e060ca     	vmla.i32	q11, q8, d10[0]
     92c: f3e040cb     	vmla.i32	q10, q8, d11[0]
     930: e1a09004     	mov	r9, r4
     934: e2844004     	add	r4, r4, #4
     938: e1570004     	cmp	r7, r4
     93c: f426074d     	vld1.16	{d0}, [r6]!
     940: f3f2207a     	vshr.u32	q9, q13, #0xe
     944: f3900a10     	vmovl.u16	q0, d0
     948: f3f80076     	vshr.u32	q8, q11, #0x8
     94c: f26aa8cc     	vadd.i32	q13, q13, q6
     950: f36223d0     	vcge.u32	q9, q9, q0
     954: f3b00074     	vshr.u32	q0, q10, #0x10
     958: f24001d2     	vand	q8, q8, q1
     95c: f2000154     	vand	q0, q0, q2
     960: f26668ec     	vadd.i32	q11, q11, q14
     964: f26001d0     	vorr	q8, q8, q0
     968: f20801d6     	vand	q0, q12, q3
     96c: f26448ee     	vadd.i32	q10, q10, q15
     970: f26001d0     	vorr	q8, q8, q0
     974: f4250a8f     	vld1.32	{d0, d1}, [r5]
     978: f3700172     	vbif	q8, q0, q9
     97c: f26888ce     	vadd.i32	q12, q12, q7
     980: f4450a8d     	vst1.32	{d16, d17}, [r5]!
     984: 2affffe9     	bhs	0x930 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x930> @ imm = #-0x5c
     988: e59d3080     	ldr	r3, [sp, #0x80]
     98c: e08ee109     	add	lr, lr, r9, lsl #2
     990: e59d20a4     	ldr	r2, [sp, #0xa4]
     994: e08cc089     	add	r12, r12, r9, lsl #1
     998: e04aa009     	sub	r10, r10, r9
     99c: e0203b99     	mla	r0, r9, r11, r3
     9a0: e59d3084     	ldr	r3, [sp, #0x84]
     9a4: e0283299     	mla	r8, r9, r2, r3
     9a8: e59d3088     	ldr	r3, [sp, #0x88]
     9ac: e59d20a8     	ldr	r2, [sp, #0xa8]
     9b0: e0273299     	mla	r7, r9, r2, r3
     9b4: e59d308c     	ldr	r3, [sp, #0x8c]
     9b8: e59d20ac     	ldr	r2, [sp, #0xac]
     9bc: e0293299     	mla	r9, r9, r2, r3
     9c0: eafffeb6     	b	0x4a0 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x4a0> @ imm = #-0x528
     9c4: e5981004     	ldr	r1, [r8, #0x4]
     9c8: e5963004     	ldr	r3, [r6, #0x4]
     9cc: eeb58ac0     	vcmpe.f32	s16, #0
     9d0: eef1fa10     	vmrs	APSR_nzcv, fpscr
     9d4: e0411003     	sub	r1, r1, r3
     9d8: e2819001     	add	r9, r1, #1
     9dc: da00002d     	ble	0xa98 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0xa98> @ imm = #0xb4
     9e0: e3510000     	cmp	r1, #0
     9e4: e596a000     	ldr	r10, [r6]
     9e8: e3a04001     	mov	r4, #1
     9ec: c58d814c     	strgt	r8, [sp, #0x14c]
     9f0: c58d615c     	strgt	r6, [sp, #0x15c]
     9f4: da00002c     	ble	0xaac <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0xaac> @ imm = #0xb0
     9f8: e28d3050     	add	r3, sp, #80
     9fc: f40d2adf     	vst1.64	{d2, d3}, [sp:64]
     a00: e1a0b004     	mov	r11, r4
     a04: f443eadf     	vst1.64	{d30, d31}, [r3:64]
     a08: e28d3040     	add	r3, sp, #64
     a0c: f443cadf     	vst1.64	{d28, d29}, [r3:64]
     a10: e28d3030     	add	r3, sp, #48
     a14: f4036adf     	vst1.64	{d6, d7}, [r3:64]
     a18: e28d3020     	add	r3, sp, #32
     a1c: f4034adf     	vst1.64	{d4, d5}, [r3:64]
     a20: e59d314c     	ldr	r3, [sp, #0x14c]
     a24: e5930000     	ldr	r0, [r3]
     a28: e040000a     	sub	r0, r0, r10
     a2c: e1a00800     	lsl	r0, r0, #16
     a30: ebfffffe     	bl	0xa30 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0xa30> @ imm = #-0x8
     a34: e28d3020     	add	r3, sp, #32
     a38: f42d2adf     	vld1.64	{d2, d3}, [sp:64]
     a3c: e58d00d4     	str	r0, [sp, #0xd4]
     a40: f4234adf     	vld1.64	{d4, d5}, [r3:64]
     a44: e28d3030     	add	r3, sp, #48
     a48: f4236adf     	vld1.64	{d6, d7}, [r3:64]
     a4c: e28d3040     	add	r3, sp, #64
     a50: f463cadf     	vld1.64	{d28, d29}, [r3:64]
     a54: e28d3050     	add	r3, sp, #80
     a58: f463eadf     	vld1.64	{d30, d31}, [r3:64]
     a5c: e1a0a80a     	lsl	r10, r10, #16
     a60: eafffe5e     	b	0x3e0 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x3e0> @ imm = #-0x688
     a64: e59d2158     	ldr	r2, [sp, #0x158]
     a68: e3a03000     	mov	r3, #0
     a6c: e58d30e0     	str	r3, [sp, #0xe0]
     a70: edcd8a44     	vstr	s17, [sp, #272]
     a74: e58d211c     	str	r2, [sp, #0x11c]
     a78: e59d2154     	ldr	r2, [sp, #0x154]
     a7c: e58d3128     	str	r3, [sp, #0x128]
     a80: e58d2118     	str	r2, [sp, #0x118]
     a84: e59d2150     	ldr	r2, [sp, #0x150]
     a88: e58d2114     	str	r2, [sp, #0x114]
     a8c: e3a02001     	mov	r2, #1
     a90: e58d210c     	str	r2, [sp, #0x10c]
     a94: eafffe46     	b	0x3b4 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x3b4> @ imm = #-0x6e8
     a98: e58db07c     	str	r11, [sp, #0x7c]
     a9c: e1a02008     	mov	r2, r8
     aa0: e3a0b001     	mov	r11, #1
     aa4: e1a04006     	mov	r4, r6
     aa8: eafffe12     	b	0x2f8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x2f8> @ imm = #-0x7b8
     aac: e3590000     	cmp	r9, #0
     ab0: daffff69     	ble	0x85c <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x85c> @ imm = #-0x25c
     ab4: e58db0d4     	str	r11, [sp, #0xd4]
     ab8: e1a0a80a     	lsl	r10, r10, #16
     abc: e1a0b004     	mov	r11, r4
     ac0: e58d814c     	str	r8, [sp, #0x14c]
     ac4: e58d615c     	str	r6, [sp, #0x15c]
     ac8: eafffe46     	b	0x3e8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x3e8> @ imm = #-0x6e8
     acc: e1a01003     	mov	r1, r3
     ad0: e1a02006     	mov	r2, r6
     ad4: e58d814c     	str	r8, [sp, #0x14c]
     ad8: eafffdf3     	b	0x2ac <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x2ac> @ imm = #-0x834
     adc: e59d314c     	ldr	r3, [sp, #0x14c]
     ae0: e59d215c     	ldr	r2, [sp, #0x15c]
     ae4: e5931004     	ldr	r1, [r3, #0x4]
     ae8: e5923004     	ldr	r3, [r2, #0x4]
     aec: e592a000     	ldr	r10, [r2]
     af0: e0411003     	sub	r1, r1, r3
     af4: e3510000     	cmp	r1, #0
     af8: c3a03000     	movgt	r3, #0
     afc: c1a0400b     	movgt	r4, r11
     b00: c58d307c     	strgt	r3, [sp, #0x7c]
     b04: caffffbb     	bgt	0x9f8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0x9f8> @ imm = #-0x114
     b08: e3a03000     	mov	r3, #0
     b0c: e58d307c     	str	r3, [sp, #0x7c]
     b10: e58d30d4     	str	r3, [sp, #0xd4]
     b14: eaffffd0     	b	0xa5c <ZB_fillTriangleSmoothNOBLEND_DT1_DW0+0xa5c> @ imm = #-0xc0
     b18: c8 0a 00 00  	.word	0x00000ac8
     b1c: 3c 08 00 00  	.word	0x0000083c
     b20: 00 00 00 00  	.word	0x00000000

Disassembly of section .text.ZB_fillTriangleSmoothNOBLEND_DT0_DW1:

00000000 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a08003     	mov	r8, r3
       8: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
       c: e24ddf5d     	sub	sp, sp, #372
      10: e5923004     	ldr	r3, [r2, #0x4]
      14: e58d00d4     	str	r0, [sp, #0xd4]
      18: e5910004     	ldr	r0, [r1, #0x4]
      1c: e598e004     	ldr	lr, [r8, #0x4]
      20: e59fcae8     	ldr	r12, [pc, #0xae8]       @ 0xb10 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0xb10>
      24: e1530000     	cmp	r3, r0
      28: a1a04000     	movge	r4, r0
      2c: b1a04003     	movlt	r4, r3
      30: a1a00003     	movge	r0, r3
      34: b1a07002     	movlt	r7, r2
      38: a1a07001     	movge	r7, r1
      3c: b1a09001     	movlt	r9, r1
      40: a1a09002     	movge	r9, r2
      44: e15e0004     	cmp	lr, r4
      48: e08fc00c     	add	r12, pc, r12
      4c: e1a03004     	mov	r3, r4
      50: ba00000b     	blt	0x84 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x84> @ imm = #0x2c
      54: e15e0000     	cmp	lr, r0
      58: e1a02008     	mov	r2, r8
      5c: a1a03000     	movge	r3, r0
      60: a1a08007     	movge	r8, r7
      64: a1a0000e     	movge	r0, lr
      68: a1a07009     	movge	r7, r9
      6c: b1a0300e     	movlt	r3, lr
      70: b1a08007     	movlt	r8, r7
      74: a1a0e004     	movge	lr, r4
      78: a1a09002     	movge	r9, r2
      7c: b1a0e004     	movlt	lr, r4
      80: b1a07002     	movlt	r7, r2
      84: e040200e     	sub	r2, r0, lr
      88: e5981000     	ldr	r1, [r8]
      8c: e5985008     	ldr	r5, [r8, #0x8]
      90: ee072a10     	vmov	s14, r2
      94: e5992000     	ldr	r2, [r9]
      98: eeb87ac7     	vcvt.f32.s32	s14, s14
      9c: e5984014     	ldr	r4, [r8, #0x14]
      a0: e0422001     	sub	r2, r2, r1
      a4: e5980018     	ldr	r0, [r8, #0x18]
      a8: ee072a90     	vmov	s15, r2
      ac: e043200e     	sub	r2, r3, lr
      b0: ee062a90     	vmov	s13, r2
      b4: e5972000     	ldr	r2, [r7]
      b8: eef87ae7     	vcvt.f32.s32	s15, s15
      bc: eef86ae6     	vcvt.f32.s32	s13, s13
      c0: e0422001     	sub	r2, r2, r1
      c4: e598101c     	ldr	r1, [r8, #0x1c]
      c8: ee052a90     	vmov	s11, r2
      cc: ee268aa7     	vmul.f32	s16, s13, s15
      d0: eef85ae5     	vcvt.f32.s32	s11, s11
      d4: e5992008     	ldr	r2, [r9, #0x8]
      d8: ee158a87     	vnmls.f32	s16, s11, s14
      dc: e0422005     	sub	r2, r2, r5
      e0: eeb58a40     	vcmp.f32	s16, #0
      e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
      e8: 1eb76a00     	vmovne.f32	s12, #1.000000e+00
      ec: 1e868a08     	vdivne.f32	s16, s12, s16
      f0: ee062a10     	vmov	s12, r2
      f4: e5992014     	ldr	r2, [r9, #0x14]
      f8: eeb82ac6     	vcvt.f32.s32	s4, s12
      fc: e0422004     	sub	r2, r2, r4
     100: ee062a10     	vmov	s12, r2
     104: e5992018     	ldr	r2, [r9, #0x18]
     108: eeb83ac6     	vcvt.f32.s32	s6, s12
     10c: e0422000     	sub	r2, r2, r0
     110: ee062a10     	vmov	s12, r2
     114: e599201c     	ldr	r2, [r9, #0x1c]
     118: eeb84ac6     	vcvt.f32.s32	s8, s12
     11c: ee686a26     	vmul.f32	s13, s16, s13
     120: e0422001     	sub	r2, r2, r1
     124: ee287a07     	vmul.f32	s14, s16, s14
     128: ee687a27     	vmul.f32	s15, s16, s15
     12c: ee062a10     	vmov	s12, r2
     130: e5972008     	ldr	r2, [r7, #0x8]
     134: eeb85ac6     	vcvt.f32.s32	s10, s12
     138: ee662a84     	vmul.f32	s5, s13, s8
     13c: e0422005     	sub	r2, r2, r5
     140: ee664a82     	vmul.f32	s9, s13, s4
     144: ee663a83     	vmul.f32	s7, s13, s6
     148: ee266a85     	vmul.f32	s12, s13, s10
     14c: ee062a90     	vmov	s13, r2
     150: ee685a25     	vmul.f32	s11, s16, s11
     154: eef86ae6     	vcvt.f32.s32	s13, s13
     158: e5972014     	ldr	r2, [r7, #0x14]
     15c: eeb58ac0     	vcmpe.f32	s16, #0
     160: eef1fa10     	vmrs	APSR_nzcv, fpscr
     164: ee574a26     	vnmls.f32	s9, s14, s13
     168: e0422004     	sub	r2, r2, r4
     16c: ee676aa6     	vmul.f32	s13, s15, s13
     170: ee556a82     	vnmls.f32	s13, s11, s4
     174: eefd6ae6     	vcvt.s32.f32	s13, s13
     178: eef08a66     	vmov.f32	s17, s13
     17c: ee062a90     	vmov	s13, r2
     180: e5972018     	ldr	r2, [r7, #0x18]
     184: eef86ae6     	vcvt.f32.s32	s13, s13
     188: e0422000     	sub	r2, r2, r0
     18c: ee573a26     	vnmls.f32	s7, s14, s13
     190: ee676aa6     	vmul.f32	s13, s15, s13
     194: ee556a83     	vnmls.f32	s13, s11, s6
     198: eefd6ae6     	vcvt.s32.f32	s13, s13
     19c: edcd6a54     	vstr	s13, [sp, #336]
     1a0: ee062a90     	vmov	s13, r2
     1a4: e597201c     	ldr	r2, [r7, #0x1c]
     1a8: eef86ae6     	vcvt.f32.s32	s13, s13
     1ac: e0422001     	sub	r2, r2, r1
     1b0: e59d10d4     	ldr	r1, [sp, #0xd4]
     1b4: ee572a26     	vnmls.f32	s5, s14, s13
     1b8: ee676aa6     	vmul.f32	s13, s15, s13
     1bc: ee556a84     	vnmls.f32	s13, s11, s8
     1c0: eefd6ae6     	vcvt.s32.f32	s13, s13
     1c4: edcd6a55     	vstr	s13, [sp, #340]
     1c8: ee062a90     	vmov	s13, r2
     1cc: eeb84ae6     	vcvt.f32.s32	s8, s13
     1d0: e5912020     	ldr	r2, [r1, #0x20]
     1d4: eef06a46     	vmov.f32	s13, s12
     1d8: ee677a84     	vmul.f32	s15, s15, s8
     1dc: ee576a04     	vnmls.f32	s13, s14, s8
     1e0: e00e0e92     	mul	lr, r2, lr
     1e4: e5912004     	ldr	r2, [r1, #0x4]
     1e8: ee557a85     	vnmls.f32	s15, s11, s10
     1ec: e082210e     	add	r2, r2, lr, lsl #2
     1f0: e58d20a4     	str	r2, [sp, #0xa4]
     1f4: e5912000     	ldr	r2, [r1]
     1f8: eefd7ae7     	vcvt.s32.f32	s15, s15
     1fc: e082208e     	add	r2, r2, lr, lsl #1
     200: edcd7a56     	vstr	s15, [sp, #344]
     204: eefd7ae3     	vcvt.s32.f32	s15, s7
     208: e58d20a0     	str	r2, [sp, #0xa0]
     20c: edcd7a2a     	vstr	s15, [sp, #168]
     210: eefd7ae2     	vcvt.s32.f32	s15, s5
     214: edcd7a2b     	vstr	s15, [sp, #172]
     218: eefd7ae6     	vcvt.s32.f32	s15, s13
     21c: edcd7a2c     	vstr	s15, [sp, #176]
     220: eefd7ae4     	vcvt.s32.f32	s15, s9
     224: ee175a90     	vmov	r5, s15
     228: e1a02105     	lsl	r2, r5, #2
     22c: e58d20dc     	str	r2, [sp, #0xdc]
     230: eeae2b10     	vdup.32	q7, r2
     234: e59d20a8     	ldr	r2, [sp, #0xa8]
     238: e1a01102     	lsl	r1, r2, #2
     23c: e1a04082     	lsl	r4, r2, #1
     240: e58d1034     	str	r1, [sp, #0x34]
     244: e0842002     	add	r2, r4, r2
     248: eeae1b90     	vdup.32	q15, r1
     24c: e59d10ac     	ldr	r1, [sp, #0xac]
     250: e1a00101     	lsl	r0, r1, #2
     254: e58d0038     	str	r0, [sp, #0x38]
     258: eea60b10     	vdup.32	q3, r0
     25c: e59d00b0     	ldr	r0, [sp, #0xb0]
     260: e58d20e8     	str	r2, [sp, #0xe8]
     264: e58d40f4     	str	r4, [sp, #0xf4]
     268: e1a0e100     	lsl	lr, r0, #2
     26c: e1a06080     	lsl	r6, r0, #1
     270: e58de03c     	str	lr, [sp, #0x3c]
     274: e0862000     	add	r2, r6, r0
     278: eea4eb10     	vdup.32	q2, lr
     27c: e1a0e081     	lsl	lr, r1, #1
     280: e58d60ec     	str	r6, [sp, #0xec]
     284: e08e1001     	add	r1, lr, r1
     288: e58de0f0     	str	lr, [sp, #0xf0]
     28c: e58d10e4     	str	r1, [sp, #0xe4]
     290: e58d20e0     	str	r2, [sp, #0xe0]
     294: da00020a     	ble	0xac4 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0xac4> @ imm = #0x828
     298: e5991004     	ldr	r1, [r9, #0x4]
     29c: e1a02009     	mov	r2, r9
     2a0: e58d714c     	str	r7, [sp, #0x14c]
     2a4: e5980004     	ldr	r0, [r8, #0x4]
     2a8: e3a06000     	mov	r6, #0
     2ac: f387025f     	vmov.i32	q0, #0xff00
     2b0: e1a04008     	mov	r4, r8
     2b4: f387205f     	vmov.i32	q1, #0xff
     2b8: e1a0b006     	mov	r11, r6
     2bc: e043a000     	sub	r10, r3, r0
     2c0: e59f384c     	ldr	r3, [pc, #0x84c]        @ 0xb14 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0xb14>
     2c4: e58d5030     	str	r5, [sp, #0x30]
     2c8: e0411000     	sub	r1, r1, r0
     2cc: e58d60cc     	str	r6, [sp, #0xcc]
     2d0: e08f3003     	add	r3, pc, r3
     2d4: e58d815c     	str	r8, [sp, #0x15c]
     2d8: e58d3140     	str	r3, [sp, #0x140]
     2dc: e1a03085     	lsl	r3, r5, #1
     2e0: e1a05009     	mov	r5, r9
     2e4: e58dc148     	str	r12, [sp, #0x148]
     2e8: e58d30f8     	str	r3, [sp, #0xf8]
     2ec: e3a03001     	mov	r3, #1
     2f0: e58d3080     	str	r3, [sp, #0x80]
     2f4: e28d3e16     	add	r3, sp, #352
     2f8: e58d3144     	str	r3, [sp, #0x144]
     2fc: e5940000     	ldr	r0, [r4]
     300: e3510000     	cmp	r1, #0
     304: e58d009c     	str	r0, [sp, #0x9c]
     308: da0001d3     	ble	0xa5c <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0xa5c> @ imm = #0x74c
     30c: e28d3050     	add	r3, sp, #80
     310: f40d0adf     	vst1.64	{d0, d1}, [sp:64]
     314: f4034adf     	vst1.64	{d4, d5}, [r3:64]
     318: e28d3040     	add	r3, sp, #64
     31c: f4036adf     	vst1.64	{d6, d7}, [r3:64]
     320: e28d3020     	add	r3, sp, #32
     324: f443eadf     	vst1.64	{d30, d31}, [r3:64]
     328: e28d3010     	add	r3, sp, #16
     32c: f4032adf     	vst1.64	{d2, d3}, [r3:64]
     330: e5923000     	ldr	r3, [r2]
     334: e0433000     	sub	r3, r3, r0
     338: e1a00803     	lsl	r0, r3, #16
     33c: ebfffffe     	bl	0x33c <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x33c> @ imm = #-0x8
     340: e59d1030     	ldr	r1, [sp, #0x30]
     344: e6ff2070     	uxth	r2, r0
     348: e1a03840     	asr	r3, r0, #16
     34c: f42d0adf     	vld1.64	{d0, d1}, [sp:64]
     350: e58d20d8     	str	r2, [sp, #0xd8]
     354: ee182a90     	vmov	r2, s17
     358: e58d3128     	str	r3, [sp, #0x128]
     35c: e0222193     	mla	r2, r3, r1, r2
     360: e59d10a8     	ldr	r1, [sp, #0xa8]
     364: e58d2110     	str	r2, [sp, #0x110]
     368: e59d2150     	ldr	r2, [sp, #0x150]
     36c: e0222193     	mla	r2, r3, r1, r2
     370: e59d10ac     	ldr	r1, [sp, #0xac]
     374: e58d2114     	str	r2, [sp, #0x114]
     378: e59d2154     	ldr	r2, [sp, #0x154]
     37c: e0222193     	mla	r2, r3, r1, r2
     380: e59d10b0     	ldr	r1, [sp, #0xb0]
     384: e58d2118     	str	r2, [sp, #0x118]
     388: e59d2158     	ldr	r2, [sp, #0x158]
     38c: e0222193     	mla	r2, r3, r1, r2
     390: e2833001     	add	r3, r3, #1
     394: e58d310c     	str	r3, [sp, #0x10c]
     398: e28d3010     	add	r3, sp, #16
     39c: e58d211c     	str	r2, [sp, #0x11c]
     3a0: f4232adf     	vld1.64	{d2, d3}, [r3:64]
     3a4: e28d3020     	add	r3, sp, #32
     3a8: f463eadf     	vld1.64	{d30, d31}, [r3:64]
     3ac: e28d3040     	add	r3, sp, #64
     3b0: f4236adf     	vld1.64	{d6, d7}, [r3:64]
     3b4: e28d3050     	add	r3, sp, #80
     3b8: f4234adf     	vld1.64	{d4, d5}, [r3:64]
     3bc: e59d3080     	ldr	r3, [sp, #0x80]
     3c0: e3530000     	cmp	r3, #0
     3c4: e5943008     	ldr	r3, [r4, #0x8]
     3c8: e58d3084     	str	r3, [sp, #0x84]
     3cc: e5943014     	ldr	r3, [r4, #0x14]
     3d0: e58d3088     	str	r3, [sp, #0x88]
     3d4: e5943018     	ldr	r3, [r4, #0x18]
     3d8: e58d308c     	str	r3, [sp, #0x8c]
     3dc: e594301c     	ldr	r3, [r4, #0x1c]
     3e0: e58d3090     	str	r3, [sp, #0x90]
     3e4: 1a0001ba     	bne	0xad4 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0xad4> @ imm = #0x6e8
     3e8: e35a0000     	cmp	r10, #0
     3ec: da000117     	ble	0x850 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x850> @ imm = #0x45c
     3f0: e59d2148     	ldr	r2, [sp, #0x148]
     3f4: e59f371c     	ldr	r3, [pc, #0x71c]        @ 0xb18 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0xb18>
     3f8: f3c7c45f     	vmov.i32	q14, #0xff0000
     3fc: e7923003     	ldr	r3, [r2, r3]
     400: e59d2110     	ldr	r2, [sp, #0x110]
     404: e58da094     	str	r10, [sp, #0x94]
     408: e58d30d0     	str	r3, [sp, #0xd0]
     40c: e59d3030     	ldr	r3, [sp, #0x30]
     410: e58db098     	str	r11, [sp, #0x98]
     414: e58da12c     	str	r10, [sp, #0x12c]
     418: e0833002     	add	r3, r3, r2
     41c: e59d2114     	ldr	r2, [sp, #0x114]
     420: e58d30fc     	str	r3, [sp, #0xfc]
     424: e59d30a8     	ldr	r3, [sp, #0xa8]
     428: e58db130     	str	r11, [sp, #0x130]
     42c: e58d6134     	str	r6, [sp, #0x134]
     430: e0833002     	add	r3, r3, r2
     434: e59d2118     	ldr	r2, [sp, #0x118]
     438: e58d3100     	str	r3, [sp, #0x100]
     43c: e59d30ac     	ldr	r3, [sp, #0xac]
     440: e58d5138     	str	r5, [sp, #0x138]
     444: e58d713c     	str	r7, [sp, #0x13c]
     448: e0833002     	add	r3, r3, r2
     44c: e59d211c     	ldr	r2, [sp, #0x11c]
     450: e58d3104     	str	r3, [sp, #0x104]
     454: e59d30b0     	ldr	r3, [sp, #0xb0]
     458: e0833002     	add	r3, r3, r2
     45c: e58d3108     	str	r3, [sp, #0x108]
     460: e59d3094     	ldr	r3, [sp, #0x94]
     464: e59d209c     	ldr	r2, [sp, #0x9c]
     468: e59d1098     	ldr	r1, [sp, #0x98]
     46c: e2433001     	sub	r3, r3, #1
     470: e58d3094     	str	r3, [sp, #0x94]
     474: e59d30d0     	ldr	r3, [sp, #0xd0]
     478: e0628841     	rsb	r8, r2, r1, asr #16
     47c: e5933000     	ldr	r3, [r3]
     480: e3530000     	cmp	r3, #0
     484: 1358000e     	cmpne	r8, #14
     488: e59d30a4     	ldr	r3, [sp, #0xa4]
     48c: e083e102     	add	lr, r3, r2, lsl #2
     490: e59d30a0     	ldr	r3, [sp, #0xa0]
     494: e083c082     	add	r12, r3, r2, lsl #1
     498: e59d2084     	ldr	r2, [sp, #0x84]
     49c: ca000105     	bgt	0x8b8 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x8b8> @ imm = #0x414
     4a0: e59d9090     	ldr	r9, [sp, #0x90]
     4a4: e59db08c     	ldr	r11, [sp, #0x8c]
     4a8: e59da088     	ldr	r10, [sp, #0x88]
     4ac: e3580002     	cmp	r8, #2
     4b0: da00009d     	ble	0x72c <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x72c> @ imm = #0x274
     4b4: e2483003     	sub	r3, r8, #3
     4b8: e28e7010     	add	r7, lr, #16
     4bc: e1a01123     	lsr	r1, r3, #2
     4c0: e28c3010     	add	r3, r12, #16
     4c4: e28ce008     	add	lr, r12, #8
     4c8: e58da074     	str	r10, [sp, #0x74]
     4cc: e1a00181     	lsl	r0, r1, #3
     4d0: e3a04000     	mov	r4, #0
     4d4: e1a06004     	mov	r6, r4
     4d8: e58d70b4     	str	r7, [sp, #0xb4]
     4dc: e0833000     	add	r3, r3, r0
     4e0: e1a05004     	mov	r5, r4
     4e4: e58d3040     	str	r3, [sp, #0x40]
     4e8: e59d3030     	ldr	r3, [sp, #0x30]
     4ec: e58de0b8     	str	lr, [sp, #0xb8]
     4f0: e58d80bc     	str	r8, [sp, #0xbc]
     4f4: e083c002     	add	r12, r3, r2
     4f8: e59d30f0     	ldr	r3, [sp, #0xf0]
     4fc: e58d20c0     	str	r2, [sp, #0xc0]
     500: e58db078     	str	r11, [sp, #0x78]
     504: e08b3003     	add	r3, r11, r3
     508: e58d907c     	str	r9, [sp, #0x7c]
     50c: e58d306c     	str	r3, [sp, #0x6c]
     510: e59d30f4     	ldr	r3, [sp, #0xf4]
     514: e58d00c4     	str	r0, [sp, #0xc4]
     518: e58d10c8     	str	r1, [sp, #0xc8]
     51c: e08a3003     	add	r3, r10, r3
     520: e58d3070     	str	r3, [sp, #0x70]
     524: e59d30ec     	ldr	r3, [sp, #0xec]
     528: e0893003     	add	r3, r9, r3
     52c: e58d3068     	str	r3, [sp, #0x68]
     530: e59d30e4     	ldr	r3, [sp, #0xe4]
     534: e083300b     	add	r3, r3, r11
     538: e58d3060     	str	r3, [sp, #0x60]
     53c: e59d30e8     	ldr	r3, [sp, #0xe8]
     540: e083300a     	add	r3, r3, r10
     544: e58d3064     	str	r3, [sp, #0x64]
     548: e59d30e0     	ldr	r3, [sp, #0xe0]
     54c: e0833009     	add	r3, r3, r9
     550: e58d3050     	str	r3, [sp, #0x50]
     554: e59d30ac     	ldr	r3, [sp, #0xac]
     558: e083300b     	add	r3, r3, r11
     55c: e58d3010     	str	r3, [sp, #0x10]
     560: e59d30b0     	ldr	r3, [sp, #0xb0]
     564: e0833009     	add	r3, r3, r9
     568: e58d3020     	str	r3, [sp, #0x20]
     56c: e59d30a8     	ldr	r3, [sp, #0xa8]
     570: e083300a     	add	r3, r3, r10
     574: e59da0f8     	ldr	r10, [sp, #0xf8]
     578: e58d3000     	str	r3, [sp]
     57c: e59d3078     	ldr	r3, [sp, #0x78]
     580: e28ee008     	add	lr, lr, #8
     584: e59d8038     	ldr	r8, [sp, #0x38]
     588: e2877010     	add	r7, r7, #16
     58c: e59db07c     	ldr	r11, [sp, #0x7c]
     590: e0830005     	add	r0, r3, r5
     594: e59d3010     	ldr	r3, [sp, #0x10]
     598: e1a00440     	asr	r0, r0, #8
     59c: e0831005     	add	r1, r3, r5
     5a0: e59d306c     	ldr	r3, [sp, #0x6c]
     5a4: e2000cff     	and	r0, r0, #65280
     5a8: e1a01441     	asr	r1, r1, #8
     5ac: e0832005     	add	r2, r3, r5
     5b0: e59d3060     	ldr	r3, [sp, #0x60]
     5b4: e1a02442     	asr	r2, r2, #8
     5b8: e2011cff     	and	r1, r1, #65280
     5bc: e0833005     	add	r3, r3, r5
     5c0: e0855008     	add	r5, r5, r8
     5c4: e59d8030     	ldr	r8, [sp, #0x30]
     5c8: e2022cff     	and	r2, r2, #65280
     5cc: e1a03443     	asr	r3, r3, #8
     5d0: e04c9008     	sub	r9, r12, r8
     5d4: e08b8004     	add	r8, r11, r4
     5d8: e59db020     	ldr	r11, [sp, #0x20]
     5dc: e7e78858     	ubfx	r8, r8, #0x10, #0x8
     5e0: e1800008     	orr	r0, r0, r8
     5e4: e2033cff     	and	r3, r3, #65280
     5e8: e1a0c72c     	lsr	r12, r12, #14
     5ec: e08b8004     	add	r8, r11, r4
     5f0: e59db068     	ldr	r11, [sp, #0x68]
     5f4: e7e78858     	ubfx	r8, r8, #0x10, #0x8
     5f8: e1811008     	orr	r1, r1, r8
     5fc: e089800a     	add	r8, r9, r10
     600: e08bb004     	add	r11, r11, r4
     604: e1a09729     	lsr	r9, r9, #14
     608: e7e7b85b     	ubfx	r11, r11, #0x10, #0x8
     60c: e182200b     	orr	r2, r2, r11
     610: e59db050     	ldr	r11, [sp, #0x50]
     614: e08bb004     	add	r11, r11, r4
     618: e7e7b85b     	ubfx	r11, r11, #0x10, #0x8
     61c: e183300b     	orr	r3, r3, r11
     620: e59db03c     	ldr	r11, [sp, #0x3c]
     624: e084400b     	add	r4, r4, r11
     628: e59db074     	ldr	r11, [sp, #0x74]
     62c: e08bb006     	add	r11, r11, r6
     630: e20bb8ff     	and	r11, r11, #16711680
     634: e180000b     	orr	r0, r0, r11
     638: e5070020     	str	r0, [r7, #-0x20]
     63c: e59d0000     	ldr	r0, [sp]
     640: e14e91b0     	strh	r9, [lr, #-16]
     644: e0800006     	add	r0, r0, r6
     648: e20008ff     	and	r0, r0, #16711680
     64c: e1811000     	orr	r1, r1, r0
     650: e507101c     	str	r1, [r7, #-0x1c]
     654: e59d1070     	ldr	r1, [sp, #0x70]
     658: e14ec0be     	strh	r12, [lr, #-14]
     65c: e0811006     	add	r1, r1, r6
     660: e20118ff     	and	r1, r1, #16711680
     664: e1822001     	orr	r2, r2, r1
     668: e5072018     	str	r2, [r7, #-0x18]
     66c: e59d2030     	ldr	r2, [sp, #0x30]
     670: e088c002     	add	r12, r8, r2
     674: e59d2064     	ldr	r2, [sp, #0x64]
     678: e1a08728     	lsr	r8, r8, #14
     67c: e0822006     	add	r2, r2, r6
     680: e14e80bc     	strh	r8, [lr, #-12]
     684: e20228ff     	and	r2, r2, #16711680
     688: e1833002     	orr	r3, r3, r2
     68c: e5073014     	str	r3, [r7, #-0x14]
     690: e59d3034     	ldr	r3, [sp, #0x34]
     694: e0866003     	add	r6, r6, r3
     698: e1a0372c     	lsr	r3, r12, #14
     69c: e08cc00a     	add	r12, r12, r10
     6a0: e14e30ba     	strh	r3, [lr, #-10]
     6a4: e59d3040     	ldr	r3, [sp, #0x40]
     6a8: e153000e     	cmp	r3, lr
     6ac: 1affffb2     	bne	0x57c <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x57c> @ imm = #-0x138
     6b0: e59d40a8     	ldr	r4, [sp, #0xa8]
     6b4: e59d00c4     	ldr	r0, [sp, #0xc4]
     6b8: e59d30b8     	ldr	r3, [sp, #0xb8]
     6bc: e59de000     	ldr	lr, [sp]
     6c0: e59d10c8     	ldr	r1, [sp, #0xc8]
     6c4: e080c003     	add	r12, r0, r3
     6c8: e59d00b0     	ldr	r0, [sp, #0xb0]
     6cc: e59d3020     	ldr	r3, [sp, #0x20]
     6d0: e08ea084     	add	r10, lr, r4, lsl #1
     6d4: e59d50ac     	ldr	r5, [sp, #0xac]
     6d8: e59de010     	ldr	lr, [sp, #0x10]
     6dc: e08aa004     	add	r10, r10, r4
     6e0: e59d403c     	ldr	r4, [sp, #0x3c]
     6e4: e0833080     	add	r3, r3, r0, lsl #1
     6e8: e59d60dc     	ldr	r6, [sp, #0xdc]
     6ec: e59d20c0     	ldr	r2, [sp, #0xc0]
     6f0: e0833000     	add	r3, r3, r0
     6f4: e08ee085     	add	lr, lr, r5, lsl #1
     6f8: e59d80bc     	ldr	r8, [sp, #0xbc]
     6fc: e0293491     	mla	r9, r1, r4, r3
     700: e59d3034     	ldr	r3, [sp, #0x34]
     704: e08ee005     	add	lr, lr, r5
     708: e0862002     	add	r2, r6, r2
     70c: e2480004     	sub	r0, r8, #4
     710: e02aa391     	mla	r10, r1, r3, r10
     714: e59d3038     	ldr	r3, [sp, #0x38]
     718: e0408101     	sub	r8, r0, r1, lsl #2
     71c: e02be391     	mla	r11, r1, r3, lr
     720: e59d30b4     	ldr	r3, [sp, #0xb4]
     724: e0222691     	mla	r2, r1, r6, r2
     728: e083e201     	add	lr, r3, r1, lsl #4
     72c: e3580000     	cmp	r8, #0
     730: ba000013     	blt	0x784 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x784> @ imm = #0x4c
     734: e28d40a8     	add	r4, sp, #168
     738: e59d0030     	ldr	r0, [sp, #0x30]
     73c: e8940070     	ldm	r4, {r4, r5, r6}
     740: e08e1108     	add	r1, lr, r8, lsl #2
     744: e24ee004     	sub	lr, lr, #4
     748: e1a0344b     	asr	r3, r11, #8
     74c: e7e77859     	ubfx	r7, r9, #0x10, #0x8
     750: e08bb005     	add	r11, r11, r5
     754: e2033cff     	and	r3, r3, #65280
     758: e0899006     	add	r9, r9, r6
     75c: e1833007     	orr	r3, r3, r7
     760: e20a78ff     	and	r7, r10, #16711680
     764: e1833007     	orr	r3, r3, r7
     768: e08aa004     	add	r10, r10, r4
     76c: e5ae3004     	str	r3, [lr, #0x4]!
     770: e1a03722     	lsr	r3, r2, #14
     774: e151000e     	cmp	r1, lr
     778: e0822000     	add	r2, r2, r0
     77c: e0cc30b2     	strh	r3, [r12], #2
     780: 1afffff0     	bne	0x748 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x748> @ imm = #-0x40
     784: e59d3080     	ldr	r3, [sp, #0x80]
     788: e59d20d8     	ldr	r2, [sp, #0xd8]
     78c: e0833002     	add	r3, r3, r2
     790: e3530000     	cmp	r3, #0
     794: e58d3080     	str	r3, [sp, #0x80]
     798: da000031     	ble	0x864 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x864> @ imm = #0xc4
     79c: e59d3080     	ldr	r3, [sp, #0x80]
     7a0: e59d210c     	ldr	r2, [sp, #0x10c]
     7a4: e2433801     	sub	r3, r3, #65536
     7a8: e58d3080     	str	r3, [sp, #0x80]
     7ac: e59d309c     	ldr	r3, [sp, #0x9c]
     7b0: e0833002     	add	r3, r3, r2
     7b4: e59d20fc     	ldr	r2, [sp, #0xfc]
     7b8: e58d309c     	str	r3, [sp, #0x9c]
     7bc: e59d3084     	ldr	r3, [sp, #0x84]
     7c0: e0833002     	add	r3, r3, r2
     7c4: e59d2100     	ldr	r2, [sp, #0x100]
     7c8: e58d3084     	str	r3, [sp, #0x84]
     7cc: e59d3088     	ldr	r3, [sp, #0x88]
     7d0: e0833002     	add	r3, r3, r2
     7d4: e59d2104     	ldr	r2, [sp, #0x104]
     7d8: e58d3088     	str	r3, [sp, #0x88]
     7dc: e59d308c     	ldr	r3, [sp, #0x8c]
     7e0: e0833002     	add	r3, r3, r2
     7e4: e59d2108     	ldr	r2, [sp, #0x108]
     7e8: e58d308c     	str	r3, [sp, #0x8c]
     7ec: e59d3090     	ldr	r3, [sp, #0x90]
     7f0: e0833002     	add	r3, r3, r2
     7f4: e58d3090     	str	r3, [sp, #0x90]
     7f8: e59d3094     	ldr	r3, [sp, #0x94]
     7fc: e59d20cc     	ldr	r2, [sp, #0xcc]
     800: e3530000     	cmp	r3, #0
     804: e59d3098     	ldr	r3, [sp, #0x98]
     808: e0833002     	add	r3, r3, r2
     80c: e59d20a4     	ldr	r2, [sp, #0xa4]
     810: e58d3098     	str	r3, [sp, #0x98]
     814: e59d30d4     	ldr	r3, [sp, #0xd4]
     818: e5933020     	ldr	r3, [r3, #0x20]
     81c: e0822103     	add	r2, r2, r3, lsl #2
     820: e58d20a4     	str	r2, [sp, #0xa4]
     824: e59d20a0     	ldr	r2, [sp, #0xa0]
     828: e0823083     	add	r3, r2, r3, lsl #1
     82c: e58d30a0     	str	r3, [sp, #0xa0]
     830: 1affff0a     	bne	0x460 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x460> @ imm = #-0x3d8
     834: e59da12c     	ldr	r10, [sp, #0x12c]
     838: e59db130     	ldr	r11, [sp, #0x130]
     83c: e59d30cc     	ldr	r3, [sp, #0xcc]
     840: e59d6134     	ldr	r6, [sp, #0x134]
     844: e59d5138     	ldr	r5, [sp, #0x138]
     848: e59d713c     	ldr	r7, [sp, #0x13c]
     84c: e02bb39a     	mla	r11, r10, r3, r11
     850: e3560001     	cmp	r6, #1
     854: 1a000058     	bne	0x9bc <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x9bc> @ imm = #0x160
     858: e28ddf5d     	add	sp, sp, #372
     85c: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     860: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     864: e59d309c     	ldr	r3, [sp, #0x9c]
     868: e59d2128     	ldr	r2, [sp, #0x128]
     86c: e0833002     	add	r3, r3, r2
     870: e59d2110     	ldr	r2, [sp, #0x110]
     874: e58d309c     	str	r3, [sp, #0x9c]
     878: e59d3084     	ldr	r3, [sp, #0x84]
     87c: e0833002     	add	r3, r3, r2
     880: e59d2114     	ldr	r2, [sp, #0x114]
     884: e58d3084     	str	r3, [sp, #0x84]
     888: e59d3088     	ldr	r3, [sp, #0x88]
     88c: e0833002     	add	r3, r3, r2
     890: e59d2118     	ldr	r2, [sp, #0x118]
     894: e58d3088     	str	r3, [sp, #0x88]
     898: e59d308c     	ldr	r3, [sp, #0x8c]
     89c: e0833002     	add	r3, r3, r2
     8a0: e59d211c     	ldr	r2, [sp, #0x11c]
     8a4: e58d308c     	str	r3, [sp, #0x8c]
     8a8: e59d3090     	ldr	r3, [sp, #0x90]
     8ac: e0833002     	add	r3, r3, r2
     8b0: e58d3090     	str	r3, [sp, #0x90]
     8b4: eaffffcf     	b	0x7f8 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x7f8> @ imm = #-0xc4
     8b8: e28d3030     	add	r3, sp, #48
     8bc: ed9dcb48     	vldr	d12, [sp, #288]
     8c0: e59d9144     	ldr	r9, [sp, #0x144]
     8c4: e2887001     	add	r7, r8, #1
     8c8: f4a3980f     	vld1.32	{d9[0]}, [r3]
     8cc: e1a0500e     	mov	r5, lr
     8d0: e59d3088     	ldr	r3, [sp, #0x88]
     8d4: e1a0600c     	mov	r6, r12
     8d8: eeaa2b90     	vdup.32	q13, r2
     8dc: e3a04004     	mov	r4, #4
     8e0: eea83b90     	vdup.32	q12, r3
     8e4: e28d30a8     	add	r3, sp, #168
     8e8: f4a3a80f     	vld1.32	{d10[0]}, [r3]
     8ec: e59d308c     	ldr	r3, [sp, #0x8c]
     8f0: eea63b90     	vdup.32	q11, r3
     8f4: e28d30ac     	add	r3, sp, #172
     8f8: f4a3b80f     	vld1.32	{d11[0]}, [r3]
     8fc: e59d3090     	ldr	r3, [sp, #0x90]
     900: eea43b90     	vdup.32	q10, r3
     904: e28d30b0     	add	r3, sp, #176
     908: f4a3c80f     	vld1.32	{d12[0]}, [r3]
     90c: e59d3140     	ldr	r3, [sp, #0x140]
     910: ed8dcb48     	vstr	d12, [sp, #288]
     914: e893000f     	ldm	r3, {r0, r1, r2, r3}
     918: e889000f     	stm	r9, {r0, r1, r2, r3}
     91c: f4690a9f     	vld1.32	{d16, d17}, [r9:64]
     920: f3e0a0c9     	vmla.i32	q13, q8, d9[0]
     924: f3e080ca     	vmla.i32	q12, q8, d10[0]
     928: f3e060cb     	vmla.i32	q11, q8, d11[0]
     92c: f3e040cc     	vmla.i32	q10, q8, d12[0]
     930: e1a03004     	mov	r3, r4
     934: e2844004     	add	r4, r4, #4
     938: e1570004     	cmp	r7, r4
     93c: f3f80076     	vshr.u32	q8, q11, #0x8
     940: f3b0c074     	vshr.u32	q6, q10, #0x10
     944: f24001d0     	vand	q8, q8, q0
     948: f20cc152     	vand	q6, q6, q1
     94c: f3f2207a     	vshr.u32	q9, q13, #0xe
     950: f26001dc     	vorr	q8, q8, q6
     954: f208c1fc     	vand	q6, q12, q14
     958: f3f62222     	vmovn.i32	d18, q9
     95c: f26001dc     	vorr	q8, q8, q6
     960: f26aa8ce     	vadd.i32	q13, q13, q7
     964: f26668c6     	vadd.i32	q11, q11, q3
     968: f4450a8d     	vst1.32	{d16, d17}, [r5]!
     96c: f26448c4     	vadd.i32	q10, q10, q2
     970: f26888ee     	vadd.i32	q12, q12, q15
     974: f446274d     	vst1.16	{d18}, [r6]!
     978: 2affffec     	bhs	0x930 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x930> @ imm = #-0x50
     97c: e59d1030     	ldr	r1, [sp, #0x30]
     980: e08ee103     	add	lr, lr, r3, lsl #2
     984: e59d2084     	ldr	r2, [sp, #0x84]
     988: e08cc083     	add	r12, r12, r3, lsl #1
     98c: e59d00a8     	ldr	r0, [sp, #0xa8]
     990: e0488003     	sub	r8, r8, r3
     994: e0222193     	mla	r2, r3, r1, r2
     998: e59d1088     	ldr	r1, [sp, #0x88]
     99c: e02a1093     	mla	r10, r3, r0, r1
     9a0: e59d108c     	ldr	r1, [sp, #0x8c]
     9a4: e59d00ac     	ldr	r0, [sp, #0xac]
     9a8: e02b1093     	mla	r11, r3, r0, r1
     9ac: e59d1090     	ldr	r1, [sp, #0x90]
     9b0: e59d00b0     	ldr	r0, [sp, #0xb0]
     9b4: e0291093     	mla	r9, r3, r0, r1
     9b8: eafffebb     	b	0x4ac <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x4ac> @ imm = #-0x514
     9bc: e5951004     	ldr	r1, [r5, #0x4]
     9c0: e5973004     	ldr	r3, [r7, #0x4]
     9c4: eeb58ac0     	vcmpe.f32	s16, #0
     9c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
     9cc: e0411003     	sub	r1, r1, r3
     9d0: e281a001     	add	r10, r1, #1
     9d4: da00002d     	ble	0xa90 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0xa90> @ imm = #0xb4
     9d8: e3510000     	cmp	r1, #0
     9dc: e597b000     	ldr	r11, [r7]
     9e0: e3a04001     	mov	r4, #1
     9e4: c58d514c     	strgt	r5, [sp, #0x14c]
     9e8: c58d715c     	strgt	r7, [sp, #0x15c]
     9ec: da00002c     	ble	0xaa4 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0xaa4> @ imm = #0xb0
     9f0: e28d3050     	add	r3, sp, #80
     9f4: f40d0adf     	vst1.64	{d0, d1}, [sp:64]
     9f8: e1a06004     	mov	r6, r4
     9fc: f4034adf     	vst1.64	{d4, d5}, [r3:64]
     a00: e28d3040     	add	r3, sp, #64
     a04: f4036adf     	vst1.64	{d6, d7}, [r3:64]
     a08: e28d3020     	add	r3, sp, #32
     a0c: f443eadf     	vst1.64	{d30, d31}, [r3:64]
     a10: e28d3010     	add	r3, sp, #16
     a14: f4032adf     	vst1.64	{d2, d3}, [r3:64]
     a18: e59d314c     	ldr	r3, [sp, #0x14c]
     a1c: e5930000     	ldr	r0, [r3]
     a20: e040000b     	sub	r0, r0, r11
     a24: e1a00800     	lsl	r0, r0, #16
     a28: ebfffffe     	bl	0xa28 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0xa28> @ imm = #-0x8
     a2c: e28d3010     	add	r3, sp, #16
     a30: f42d0adf     	vld1.64	{d0, d1}, [sp:64]
     a34: e58d00cc     	str	r0, [sp, #0xcc]
     a38: f4232adf     	vld1.64	{d2, d3}, [r3:64]
     a3c: e28d3020     	add	r3, sp, #32
     a40: f463eadf     	vld1.64	{d30, d31}, [r3:64]
     a44: e28d3040     	add	r3, sp, #64
     a48: f4236adf     	vld1.64	{d6, d7}, [r3:64]
     a4c: e28d3050     	add	r3, sp, #80
     a50: f4234adf     	vld1.64	{d4, d5}, [r3:64]
     a54: e1a0b80b     	lsl	r11, r11, #16
     a58: eafffe62     	b	0x3e8 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x3e8> @ imm = #-0x678
     a5c: e59d2158     	ldr	r2, [sp, #0x158]
     a60: e3a03000     	mov	r3, #0
     a64: e58d30d8     	str	r3, [sp, #0xd8]
     a68: edcd8a44     	vstr	s17, [sp, #272]
     a6c: e58d211c     	str	r2, [sp, #0x11c]
     a70: e59d2154     	ldr	r2, [sp, #0x154]
     a74: e58d3128     	str	r3, [sp, #0x128]
     a78: e58d2118     	str	r2, [sp, #0x118]
     a7c: e59d2150     	ldr	r2, [sp, #0x150]
     a80: e58d2114     	str	r2, [sp, #0x114]
     a84: e3a02001     	mov	r2, #1
     a88: e58d210c     	str	r2, [sp, #0x10c]
     a8c: eafffe4a     	b	0x3bc <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x3bc> @ imm = #-0x6d8
     a90: e58d6080     	str	r6, [sp, #0x80]
     a94: e1a02005     	mov	r2, r5
     a98: e3a06001     	mov	r6, #1
     a9c: e1a04007     	mov	r4, r7
     aa0: eafffe15     	b	0x2fc <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x2fc> @ imm = #-0x7ac
     aa4: e35a0000     	cmp	r10, #0
     aa8: daffff6a     	ble	0x858 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x858> @ imm = #-0x258
     aac: e58d60cc     	str	r6, [sp, #0xcc]
     ab0: e1a0b80b     	lsl	r11, r11, #16
     ab4: e1a06004     	mov	r6, r4
     ab8: e58d514c     	str	r5, [sp, #0x14c]
     abc: e58d715c     	str	r7, [sp, #0x15c]
     ac0: eafffe4a     	b	0x3f0 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x3f0> @ imm = #-0x6d8
     ac4: e1a01003     	mov	r1, r3
     ac8: e1a02007     	mov	r2, r7
     acc: e58d914c     	str	r9, [sp, #0x14c]
     ad0: eafffdf3     	b	0x2a4 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x2a4> @ imm = #-0x834
     ad4: e59d314c     	ldr	r3, [sp, #0x14c]
     ad8: e59d215c     	ldr	r2, [sp, #0x15c]
     adc: e5931004     	ldr	r1, [r3, #0x4]
     ae0: e5923004     	ldr	r3, [r2, #0x4]
     ae4: e592b000     	ldr	r11, [r2]
     ae8: e0411003     	sub	r1, r1, r3
     aec: e3510000     	cmp	r1, #0
     af0: c3a03000     	movgt	r3, #0
     af4: c1a04006     	movgt	r4, r6
     af8: c58d3080     	strgt	r3, [sp, #0x80]
     afc: caffffbb     	bgt	0x9f0 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0x9f0> @ imm = #-0x114
     b00: e3a03000     	mov	r3, #0
     b04: e58d3080     	str	r3, [sp, #0x80]
     b08: e58d30cc     	str	r3, [sp, #0xcc]
     b0c: eaffffd0     	b	0xa54 <ZB_fillTriangleSmoothNOBLEND_DT0_DW1+0xa54> @ imm = #-0xc0
     b10: c0 0a 00 00  	.word	0x00000ac0
     b14: 3c 08 00 00  	.word	0x0000083c
     b18: 00 00 00 00  	.word	0x00000000

Disassembly of section .text.ZB_fillTriangleSmoothNOBLEND_DT1_DW1:

00000000 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a07003     	mov	r7, r3
       8: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
       c: e24ddf63     	sub	sp, sp, #396
      10: e5923004     	ldr	r3, [r2, #0x4]
      14: e58d00d4     	str	r0, [sp, #0xd4]
      18: e5910004     	ldr	r0, [r1, #0x4]
      1c: e597e004     	ldr	lr, [r7, #0x4]
      20: e59fcb88     	ldr	r12, [pc, #0xb88]       @ 0xbb0 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xbb0>
      24: e1530000     	cmp	r3, r0
      28: a1a04000     	movge	r4, r0
      2c: b1a04003     	movlt	r4, r3
      30: a1a00003     	movge	r0, r3
      34: b1a05002     	movlt	r5, r2
      38: a1a05001     	movge	r5, r1
      3c: b1a08001     	movlt	r8, r1
      40: a1a08002     	movge	r8, r2
      44: e15e0004     	cmp	lr, r4
      48: e08fc00c     	add	r12, pc, r12
      4c: e1a03004     	mov	r3, r4
      50: e58d511c     	str	r5, [sp, #0x11c]
      54: ba00000b     	blt	0x88 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x88> @ imm = #0x2c
      58: e15e0000     	cmp	lr, r0
      5c: e1a02007     	mov	r2, r7
      60: a1a03000     	movge	r3, r0
      64: b1a0300e     	movlt	r3, lr
      68: a1a0000e     	movge	r0, lr
      6c: a58d811c     	strge	r8, [sp, #0x11c]
      70: a1a07005     	movge	r7, r5
      74: a1a0e004     	movge	lr, r4
      78: a1a08002     	movge	r8, r2
      7c: b1a0e004     	movlt	lr, r4
      80: b59d711c     	ldrlt	r7, [sp, #0x11c]
      84: b58d211c     	strlt	r2, [sp, #0x11c]
      88: e040200e     	sub	r2, r0, lr
      8c: e5971000     	ldr	r1, [r7]
      90: e5975008     	ldr	r5, [r7, #0x8]
      94: ee072a10     	vmov	s14, r2
      98: e5982000     	ldr	r2, [r8]
      9c: eeb87ac7     	vcvt.f32.s32	s14, s14
      a0: e5974014     	ldr	r4, [r7, #0x14]
      a4: e0422001     	sub	r2, r2, r1
      a8: e5970018     	ldr	r0, [r7, #0x18]
      ac: e59d611c     	ldr	r6, [sp, #0x11c]
      b0: ee072a90     	vmov	s15, r2
      b4: e043200e     	sub	r2, r3, lr
      b8: ee062a90     	vmov	s13, r2
      bc: e59d211c     	ldr	r2, [sp, #0x11c]
      c0: eef87ae7     	vcvt.f32.s32	s15, s15
      c4: eef86ae6     	vcvt.f32.s32	s13, s13
      c8: e5922000     	ldr	r2, [r2]
      cc: e0422001     	sub	r2, r2, r1
      d0: ee268aa7     	vmul.f32	s16, s13, s15
      d4: e597101c     	ldr	r1, [r7, #0x1c]
      d8: ee052a90     	vmov	s11, r2
      dc: e5982008     	ldr	r2, [r8, #0x8]
      e0: eef85ae5     	vcvt.f32.s32	s11, s11
      e4: e0422005     	sub	r2, r2, r5
      e8: ee158a87     	vnmls.f32	s16, s11, s14
      ec: eeb58a40     	vcmp.f32	s16, #0
      f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
      f4: 1eb76a00     	vmovne.f32	s12, #1.000000e+00
      f8: 1e868a08     	vdivne.f32	s16, s12, s16
      fc: ee062a10     	vmov	s12, r2
     100: e5982014     	ldr	r2, [r8, #0x14]
     104: eeb82ac6     	vcvt.f32.s32	s4, s12
     108: e0422004     	sub	r2, r2, r4
     10c: ee062a10     	vmov	s12, r2
     110: e5982018     	ldr	r2, [r8, #0x18]
     114: eeb83ac6     	vcvt.f32.s32	s6, s12
     118: e0422000     	sub	r2, r2, r0
     11c: ee062a10     	vmov	s12, r2
     120: e598201c     	ldr	r2, [r8, #0x1c]
     124: eeb84ac6     	vcvt.f32.s32	s8, s12
     128: ee686a26     	vmul.f32	s13, s16, s13
     12c: e0422001     	sub	r2, r2, r1
     130: ee287a07     	vmul.f32	s14, s16, s14
     134: ee687a27     	vmul.f32	s15, s16, s15
     138: ee062a10     	vmov	s12, r2
     13c: e5962008     	ldr	r2, [r6, #0x8]
     140: eeb85ac6     	vcvt.f32.s32	s10, s12
     144: ee662a84     	vmul.f32	s5, s13, s8
     148: e0422005     	sub	r2, r2, r5
     14c: ee664a82     	vmul.f32	s9, s13, s4
     150: ee663a83     	vmul.f32	s7, s13, s6
     154: ee266a85     	vmul.f32	s12, s13, s10
     158: ee062a90     	vmov	s13, r2
     15c: ee685a25     	vmul.f32	s11, s16, s11
     160: eef86ae6     	vcvt.f32.s32	s13, s13
     164: e5962014     	ldr	r2, [r6, #0x14]
     168: eeb58ac0     	vcmpe.f32	s16, #0
     16c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     170: ee574a26     	vnmls.f32	s9, s14, s13
     174: e0422004     	sub	r2, r2, r4
     178: ee676aa6     	vmul.f32	s13, s15, s13
     17c: ee556a82     	vnmls.f32	s13, s11, s4
     180: eefd6ae6     	vcvt.s32.f32	s13, s13
     184: edcd6a58     	vstr	s13, [sp, #352]
     188: ee062a90     	vmov	s13, r2
     18c: e5962018     	ldr	r2, [r6, #0x18]
     190: eef86ae6     	vcvt.f32.s32	s13, s13
     194: e0422000     	sub	r2, r2, r0
     198: e1a00006     	mov	r0, r6
     19c: ee573a26     	vnmls.f32	s7, s14, s13
     1a0: ee676aa6     	vmul.f32	s13, s15, s13
     1a4: ee556a83     	vnmls.f32	s13, s11, s6
     1a8: eefd6ae6     	vcvt.s32.f32	s13, s13
     1ac: edcd6a59     	vstr	s13, [sp, #356]
     1b0: ee062a90     	vmov	s13, r2
     1b4: e596201c     	ldr	r2, [r6, #0x1c]
     1b8: eef86ae6     	vcvt.f32.s32	s13, s13
     1bc: e0422001     	sub	r2, r2, r1
     1c0: e59d10d4     	ldr	r1, [sp, #0xd4]
     1c4: ee572a26     	vnmls.f32	s5, s14, s13
     1c8: ee676aa6     	vmul.f32	s13, s15, s13
     1cc: ee556a84     	vnmls.f32	s13, s11, s8
     1d0: eefd6ae6     	vcvt.s32.f32	s13, s13
     1d4: edcd6a5a     	vstr	s13, [sp, #360]
     1d8: ee062a90     	vmov	s13, r2
     1dc: eeb84ae6     	vcvt.f32.s32	s8, s13
     1e0: e5912020     	ldr	r2, [r1, #0x20]
     1e4: eef06a46     	vmov.f32	s13, s12
     1e8: ee677a84     	vmul.f32	s15, s15, s8
     1ec: e00e0e92     	mul	lr, r2, lr
     1f0: e5912004     	ldr	r2, [r1, #0x4]
     1f4: ee576a04     	vnmls.f32	s13, s14, s8
     1f8: ee557a85     	vnmls.f32	s15, s11, s10
     1fc: e082210e     	add	r2, r2, lr, lsl #2
     200: e58d209c     	str	r2, [sp, #0x9c]
     204: e5912000     	ldr	r2, [r1]
     208: eefd7ae7     	vcvt.s32.f32	s15, s15
     20c: e082208e     	add	r2, r2, lr, lsl #1
     210: e58d2098     	str	r2, [sp, #0x98]
     214: edcd7a5b     	vstr	s15, [sp, #364]
     218: eefd7ae3     	vcvt.s32.f32	s15, s7
     21c: edcd7a18     	vstr	s15, [sp, #96]
     220: eefd7ae2     	vcvt.s32.f32	s15, s5
     224: e59d2060     	ldr	r2, [sp, #0x60]
     228: edcd7a19     	vstr	s15, [sp, #100]
     22c: eefd7ae6     	vcvt.s32.f32	s15, s13
     230: e1a01102     	lsl	r1, r2, #2
     234: e1a06082     	lsl	r6, r2, #1
     238: e58d1014     	str	r1, [sp, #0x14]
     23c: eeac1b10     	vdup.32	q6, r1
     240: e0862002     	add	r2, r6, r2
     244: edcd7a28     	vstr	s15, [sp, #160]
     248: e59d1064     	ldr	r1, [sp, #0x64]
     24c: eefd7ae4     	vcvt.s32.f32	s15, s9
     250: e58d20f0     	str	r2, [sp, #0xf0]
     254: e58d60e4     	str	r6, [sp, #0xe4]
     258: e1a0e101     	lsl	lr, r1, #2
     25c: ee175a90     	vmov	r5, s15
     260: e58de018     	str	lr, [sp, #0x18]
     264: e1a0b105     	lsl	r11, r5, #2
     268: eeaeeb10     	vdup.32	q7, lr
     26c: e1a0a085     	lsl	r10, r5, #1
     270: e59de0a0     	ldr	lr, [sp, #0xa0]
     274: eeaabb10     	vdup.32	q5, r11
     278: e58da0a4     	str	r10, [sp, #0xa4]
     27c: e1a0410e     	lsl	r4, lr, #2
     280: e1a0908e     	lsl	r9, lr, #1
     284: e58d401c     	str	r4, [sp, #0x1c]
     288: e089200e     	add	r2, r9, lr
     28c: eea64b10     	vdup.32	q3, r4
     290: e1a04081     	lsl	r4, r1, #1
     294: e58d90dc     	str	r9, [sp, #0xdc]
     298: e0841001     	add	r1, r4, r1
     29c: e58d40e0     	str	r4, [sp, #0xe0]
     2a0: e58d10ec     	str	r1, [sp, #0xec]
     2a4: e58d20e8     	str	r2, [sp, #0xe8]
     2a8: da00022d     	ble	0xb64 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xb64> @ imm = #0x8b4
     2ac: e5981004     	ldr	r1, [r8, #0x4]
     2b0: e1a02008     	mov	r2, r8
     2b4: e58d015c     	str	r0, [sp, #0x15c]
     2b8: e5970004     	ldr	r0, [r7, #0x4]
     2bc: e3a06000     	mov	r6, #0
     2c0: f387025f     	vmov.i32	q0, #0xff00
     2c4: e1a04007     	mov	r4, r7
     2c8: f387205f     	vmov.i32	q1, #0xff
     2cc: e1a0a006     	mov	r10, r6
     2d0: e0439000     	sub	r9, r3, r0
     2d4: e59f38d8     	ldr	r3, [pc, #0x8d8]        @ 0xbb4 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xbb4>
     2d8: f387445f     	vmov.i32	q2, #0xff0000
     2dc: e0411000     	sub	r1, r1, r0
     2e0: e58db0a8     	str	r11, [sp, #0xa8]
     2e4: e1a0b005     	mov	r11, r5
     2e8: e08f3003     	add	r3, pc, r3
     2ec: e58d60cc     	str	r6, [sp, #0xcc]
     2f0: e58d3150     	str	r3, [sp, #0x150]
     2f4: e3a03001     	mov	r3, #1
     2f8: e58d3010     	str	r3, [sp, #0x10]
     2fc: e28d3f5e     	add	r3, sp, #376
     300: e58d3154     	str	r3, [sp, #0x154]
     304: ed8d8a5c     	vstr	s16, [sp, #368]
     308: e58d7174     	str	r7, [sp, #0x174]
     30c: e58dc158     	str	r12, [sp, #0x158]
     310: e5940000     	ldr	r0, [r4]
     314: e3510000     	cmp	r1, #0
     318: e58d0094     	str	r0, [sp, #0x94]
     31c: da0001f5     	ble	0xaf8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xaf8> @ imm = #0x7d4
     320: e28d3040     	add	r3, sp, #64
     324: f40d0adf     	vst1.64	{d0, d1}, [sp:64]
     328: f4036adf     	vst1.64	{d6, d7}, [r3:64]
     32c: e28d3030     	add	r3, sp, #48
     330: f4034adf     	vst1.64	{d4, d5}, [r3:64]
     334: e28d3020     	add	r3, sp, #32
     338: f4032adf     	vst1.64	{d2, d3}, [r3:64]
     33c: e5923000     	ldr	r3, [r2]
     340: e0433000     	sub	r3, r3, r0
     344: e1a00803     	lsl	r0, r3, #16
     348: ebfffffe     	bl	0x348 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x348> @ imm = #-0x8
     34c: e6ff2070     	uxth	r2, r0
     350: e1a03840     	asr	r3, r0, #16
     354: e59d1060     	ldr	r1, [sp, #0x60]
     358: e58d20d8     	str	r2, [sp, #0xd8]
     35c: e59d2160     	ldr	r2, [sp, #0x160]
     360: e58d3118     	str	r3, [sp, #0x118]
     364: f42d0adf     	vld1.64	{d0, d1}, [sp:64]
     368: e0222b93     	mla	r2, r3, r11, r2
     36c: e58d2108     	str	r2, [sp, #0x108]
     370: e59d2164     	ldr	r2, [sp, #0x164]
     374: e0222193     	mla	r2, r3, r1, r2
     378: e59d1064     	ldr	r1, [sp, #0x64]
     37c: e58d210c     	str	r2, [sp, #0x10c]
     380: e59d2168     	ldr	r2, [sp, #0x168]
     384: e0222193     	mla	r2, r3, r1, r2
     388: e59d10a0     	ldr	r1, [sp, #0xa0]
     38c: e58d2110     	str	r2, [sp, #0x110]
     390: e59d216c     	ldr	r2, [sp, #0x16c]
     394: e0222193     	mla	r2, r3, r1, r2
     398: e2833001     	add	r3, r3, #1
     39c: e58d3104     	str	r3, [sp, #0x104]
     3a0: e28d3020     	add	r3, sp, #32
     3a4: e58d2114     	str	r2, [sp, #0x114]
     3a8: f4232adf     	vld1.64	{d2, d3}, [r3:64]
     3ac: e28d3030     	add	r3, sp, #48
     3b0: f4234adf     	vld1.64	{d4, d5}, [r3:64]
     3b4: e28d3040     	add	r3, sp, #64
     3b8: f4236adf     	vld1.64	{d6, d7}, [r3:64]
     3bc: e59d3010     	ldr	r3, [sp, #0x10]
     3c0: e3530000     	cmp	r3, #0
     3c4: e5943008     	ldr	r3, [r4, #0x8]
     3c8: e58d307c     	str	r3, [sp, #0x7c]
     3cc: e5943014     	ldr	r3, [r4, #0x14]
     3d0: e58d3080     	str	r3, [sp, #0x80]
     3d4: e5943018     	ldr	r3, [r4, #0x18]
     3d8: e58d3084     	str	r3, [sp, #0x84]
     3dc: e594301c     	ldr	r3, [r4, #0x1c]
     3e0: e58d3088     	str	r3, [sp, #0x88]
     3e4: 1a0001e2     	bne	0xb74 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xb74> @ imm = #0x788
     3e8: e3590000     	cmp	r9, #0
     3ec: da00012d     	ble	0x8a8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x8a8> @ imm = #0x4b4
     3f0: e59d2158     	ldr	r2, [sp, #0x158]
     3f4: e59f37bc     	ldr	r3, [pc, #0x7bc]        @ 0xbb8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xbb8>
     3f8: e7923003     	ldr	r3, [r2, r3]
     3fc: e59d210c     	ldr	r2, [sp, #0x10c]
     400: e58da090     	str	r10, [sp, #0x90]
     404: e58d30d0     	str	r3, [sp, #0xd0]
     408: e59d3108     	ldr	r3, [sp, #0x108]
     40c: e58da148     	str	r10, [sp, #0x148]
     410: e1a0a00b     	mov	r10, r11
     414: e58d908c     	str	r9, [sp, #0x8c]
     418: e08b3003     	add	r3, r11, r3
     41c: e59db010     	ldr	r11, [sp, #0x10]
     420: e58d30f4     	str	r3, [sp, #0xf4]
     424: e59d3060     	ldr	r3, [sp, #0x60]
     428: e58d9140     	str	r9, [sp, #0x140]
     42c: e58d6144     	str	r6, [sp, #0x144]
     430: e0833002     	add	r3, r3, r2
     434: e59d2110     	ldr	r2, [sp, #0x110]
     438: e58d30f8     	str	r3, [sp, #0xf8]
     43c: e59d3064     	ldr	r3, [sp, #0x64]
     440: e58d814c     	str	r8, [sp, #0x14c]
     444: e0833002     	add	r3, r3, r2
     448: e59d2114     	ldr	r2, [sp, #0x114]
     44c: e58d30fc     	str	r3, [sp, #0xfc]
     450: e59d30a0     	ldr	r3, [sp, #0xa0]
     454: e0833002     	add	r3, r3, r2
     458: e58d3100     	str	r3, [sp, #0x100]
     45c: e59d308c     	ldr	r3, [sp, #0x8c]
     460: e59d2094     	ldr	r2, [sp, #0x94]
     464: e59d1090     	ldr	r1, [sp, #0x90]
     468: e2433001     	sub	r3, r3, #1
     46c: e59d007c     	ldr	r0, [sp, #0x7c]
     470: e58d308c     	str	r3, [sp, #0x8c]
     474: e59d30d0     	ldr	r3, [sp, #0xd0]
     478: e0621841     	rsb	r1, r2, r1, asr #16
     47c: e5933000     	ldr	r3, [r3]
     480: e58d1054     	str	r1, [sp, #0x54]
     484: e3530000     	cmp	r3, #0
     488: 1351000e     	cmpne	r1, #14
     48c: e59d309c     	ldr	r3, [sp, #0x9c]
     490: e0834102     	add	r4, r3, r2, lsl #2
     494: e59d3098     	ldr	r3, [sp, #0x98]
     498: e083c082     	add	r12, r3, r2, lsl #1
     49c: ca00011b     	bgt	0x910 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x910> @ imm = #0x46c
     4a0: e28d3080     	add	r3, sp, #128
     4a4: e8930308     	ldm	r3, {r3, r8, r9}
     4a8: e58d3010     	str	r3, [sp, #0x10]
     4ac: e59d3054     	ldr	r3, [sp, #0x54]
     4b0: e3530002     	cmp	r3, #2
     4b4: da0000af     	ble	0x778 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x778> @ imm = #0x2bc
     4b8: e2433003     	sub	r3, r3, #3
     4bc: e28ce008     	add	lr, r12, #8
     4c0: e1a02123     	lsr	r2, r3, #2
     4c4: e28c3010     	add	r3, r12, #16
     4c8: e59d10e4     	ldr	r1, [sp, #0xe4]
     4cc: e2846010     	add	r6, r4, #16
     4d0: e1a0c182     	lsl	r12, r2, #3
     4d4: e3a04000     	mov	r4, #0
     4d8: e1a05004     	mov	r5, r4
     4dc: e58d8050     	str	r8, [sp, #0x50]
     4e0: e083300c     	add	r3, r3, r12
     4e4: e58d60b0     	str	r6, [sp, #0xb0]
     4e8: e58d3040     	str	r3, [sp, #0x40]
     4ec: e59d30a4     	ldr	r3, [sp, #0xa4]
     4f0: e58de0b4     	str	lr, [sp, #0xb4]
     4f4: e58d40b8     	str	r4, [sp, #0xb8]
     4f8: e0807003     	add	r7, r0, r3
     4fc: e59d30e0     	ldr	r3, [sp, #0xe0]
     500: e58d70ac     	str	r7, [sp, #0xac]
     504: e58db0bc     	str	r11, [sp, #0xbc]
     508: e0883003     	add	r3, r8, r3
     50c: e58d9078     	str	r9, [sp, #0x78]
     510: e58d3020     	str	r3, [sp, #0x20]
     514: e59d3010     	ldr	r3, [sp, #0x10]
     518: e58dc0c0     	str	r12, [sp, #0xc0]
     51c: e58d20c4     	str	r2, [sp, #0xc4]
     520: e0831001     	add	r1, r3, r1
     524: e58d106c     	str	r1, [sp, #0x6c]
     528: e59d10dc     	ldr	r1, [sp, #0xdc]
     52c: e0891001     	add	r1, r9, r1
     530: e58d1068     	str	r1, [sp, #0x68]
     534: e59d10ec     	ldr	r1, [sp, #0xec]
     538: e0881001     	add	r1, r8, r1
     53c: e58d1030     	str	r1, [sp, #0x30]
     540: e59d10f0     	ldr	r1, [sp, #0xf0]
     544: e0831001     	add	r1, r3, r1
     548: e59d3064     	ldr	r3, [sp, #0x64]
     54c: e58d1074     	str	r1, [sp, #0x74]
     550: e59d10e8     	ldr	r1, [sp, #0xe8]
     554: e0833008     	add	r3, r3, r8
     558: e59d80a8     	ldr	r8, [sp, #0xa8]
     55c: e58d3000     	str	r3, [sp]
     560: e59d30a0     	ldr	r3, [sp, #0xa0]
     564: e0891001     	add	r1, r9, r1
     568: e58d1070     	str	r1, [sp, #0x70]
     56c: e59d1060     	ldr	r1, [sp, #0x60]
     570: e0833009     	add	r3, r3, r9
     574: e58d305c     	str	r3, [sp, #0x5c]
     578: e59d3010     	ldr	r3, [sp, #0x10]
     57c: e0813003     	add	r3, r1, r3
     580: e1a01004     	mov	r1, r4
     584: e58d3058     	str	r3, [sp, #0x58]
     588: e1a0300e     	mov	r3, lr
     58c: e58de0c8     	str	lr, [sp, #0xc8]
     590: e59d2050     	ldr	r2, [sp, #0x50]
     594: e1a0c720     	lsr	r12, r0, #14
     598: e153e0b8     	ldrh	lr, [r3, #-8]
     59c: e08a9000     	add	r9, r10, r0
     5a0: e1a09729     	lsr	r9, r9, #14
     5a4: e0800008     	add	r0, r0, r8
     5a8: e0822001     	add	r2, r2, r1
     5ac: e1a02442     	asr	r2, r2, #8
     5b0: e15e000c     	cmp	lr, r12
     5b4: e2022cff     	and	r2, r2, #65280
     5b8: 8a000009     	bhi	0x5e4 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x5e4> @ imm = #0x24
     5bc: e59de078     	ldr	lr, [sp, #0x78]
     5c0: e08ee004     	add	lr, lr, r4
     5c4: e7e7e85e     	ubfx	lr, lr, #0x10, #0x8
     5c8: e182200e     	orr	r2, r2, lr
     5cc: e59de010     	ldr	lr, [sp, #0x10]
     5d0: e08ee005     	add	lr, lr, r5
     5d4: e20ee8ff     	and	lr, lr, #16711680
     5d8: e182200e     	orr	r2, r2, lr
     5dc: e5062010     	str	r2, [r6, #-0x10]
     5e0: e143c0b8     	strh	r12, [r3, #-8]
     5e4: e59dc020     	ldr	r12, [sp, #0x20]
     5e8: e59db018     	ldr	r11, [sp, #0x18]
     5ec: e59d2000     	ldr	r2, [sp]
     5f0: e08ce001     	add	lr, r12, r1
     5f4: e59dc030     	ldr	r12, [sp, #0x30]
     5f8: e1a0e44e     	asr	lr, lr, #8
     5fc: e0822001     	add	r2, r2, r1
     600: e08cc001     	add	r12, r12, r1
     604: e081100b     	add	r1, r1, r11
     608: e153b0b6     	ldrh	r11, [r3, #-6]
     60c: e1a02442     	asr	r2, r2, #8
     610: e1a0c44c     	asr	r12, r12, #8
     614: e20eecff     	and	lr, lr, #65280
     618: e2022cff     	and	r2, r2, #65280
     61c: e15b0009     	cmp	r11, r9
     620: e20cccff     	and	r12, r12, #65280
     624: 8a000009     	bhi	0x650 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x650> @ imm = #0x24
     628: e59db05c     	ldr	r11, [sp, #0x5c]
     62c: e08bb004     	add	r11, r11, r4
     630: e7e7b85b     	ubfx	r11, r11, #0x10, #0x8
     634: e182200b     	orr	r2, r2, r11
     638: e59db058     	ldr	r11, [sp, #0x58]
     63c: e08bb005     	add	r11, r11, r5
     640: e20bb8ff     	and	r11, r11, #16711680
     644: e182200b     	orr	r2, r2, r11
     648: e506200c     	str	r2, [r6, #-0xc]
     64c: e14390b6     	strh	r9, [r3, #-6]
     650: e153b0b4     	ldrh	r11, [r3, #-4]
     654: e1a09727     	lsr	r9, r7, #14
     658: e08a2007     	add	r2, r10, r7
     65c: e1a02722     	lsr	r2, r2, #14
     660: e0877008     	add	r7, r7, r8
     664: e15b0009     	cmp	r11, r9
     668: 8a000009     	bhi	0x694 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x694> @ imm = #0x24
     66c: e59db068     	ldr	r11, [sp, #0x68]
     670: e08bb004     	add	r11, r11, r4
     674: e7e7b85b     	ubfx	r11, r11, #0x10, #0x8
     678: e18ee00b     	orr	lr, lr, r11
     67c: e59db06c     	ldr	r11, [sp, #0x6c]
     680: e08bb005     	add	r11, r11, r5
     684: e20bb8ff     	and	r11, r11, #16711680
     688: e18ee00b     	orr	lr, lr, r11
     68c: e506e008     	str	lr, [r6, #-0x8]
     690: e14390b4     	strh	r9, [r3, #-4]
     694: e153e0b2     	ldrh	lr, [r3, #-2]
     698: e15e0002     	cmp	lr, r2
     69c: 8a000009     	bhi	0x6c8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x6c8> @ imm = #0x24
     6a0: e59de070     	ldr	lr, [sp, #0x70]
     6a4: e08ee004     	add	lr, lr, r4
     6a8: e7e7e85e     	ubfx	lr, lr, #0x10, #0x8
     6ac: e18cc00e     	orr	r12, r12, lr
     6b0: e59de074     	ldr	lr, [sp, #0x74]
     6b4: e08ee005     	add	lr, lr, r5
     6b8: e20ee8ff     	and	lr, lr, #16711680
     6bc: e18cc00e     	orr	r12, r12, lr
     6c0: e506c004     	str	r12, [r6, #-0x4]
     6c4: e14320b2     	strh	r2, [r3, #-2]
     6c8: e59d2014     	ldr	r2, [sp, #0x14]
     6cc: e2833008     	add	r3, r3, #8
     6d0: e2866010     	add	r6, r6, #16
     6d4: e0855002     	add	r5, r5, r2
     6d8: e59d2040     	ldr	r2, [sp, #0x40]
     6dc: e1520003     	cmp	r2, r3
     6e0: e59d201c     	ldr	r2, [sp, #0x1c]
     6e4: e0844002     	add	r4, r4, r2
     6e8: 1affffa8     	bne	0x590 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x590> @ imm = #-0x160
     6ec: e59de0c8     	ldr	lr, [sp, #0xc8]
     6f0: e59dc0c0     	ldr	r12, [sp, #0xc0]
     6f4: e59d0058     	ldr	r0, [sp, #0x58]
     6f8: e59d10a0     	ldr	r1, [sp, #0xa0]
     6fc: e08cc00e     	add	r12, r12, lr
     700: e59de060     	ldr	lr, [sp, #0x60]
     704: e59d305c     	ldr	r3, [sp, #0x5c]
     708: e59d20c4     	ldr	r2, [sp, #0xc4]
     70c: e59d5064     	ldr	r5, [sp, #0x64]
     710: e080408e     	add	r4, r0, lr, lsl #1
     714: e59d0000     	ldr	r0, [sp]
     718: e0833081     	add	r3, r3, r1, lsl #1
     71c: e59db0bc     	ldr	r11, [sp, #0xbc]
     720: e084400e     	add	r4, r4, lr
     724: e59de01c     	ldr	lr, [sp, #0x1c]
     728: e0833001     	add	r3, r3, r1
     72c: e59d10a4     	ldr	r1, [sp, #0xa4]
     730: e0808085     	add	r8, r0, r5, lsl #1
     734: e59d00ac     	ldr	r0, [sp, #0xac]
     738: e0293e92     	mla	r9, r2, lr, r3
     73c: e59d3014     	ldr	r3, [sp, #0x14]
     740: e0888005     	add	r8, r8, r5
     744: e0810000     	add	r0, r1, r0
     748: e59d1054     	ldr	r1, [sp, #0x54]
     74c: e0234392     	mla	r3, r2, r3, r4
     750: e2411004     	sub	r1, r1, #4
     754: e58d3010     	str	r3, [sp, #0x10]
     758: e59d3018     	ldr	r3, [sp, #0x18]
     75c: e0288392     	mla	r8, r2, r3, r8
     760: e59d30a8     	ldr	r3, [sp, #0xa8]
     764: e0200392     	mla	r0, r2, r3, r0
     768: e0413102     	sub	r3, r1, r2, lsl #2
     76c: e58d3054     	str	r3, [sp, #0x54]
     770: e59d30b0     	ldr	r3, [sp, #0xb0]
     774: e0834202     	add	r4, r3, r2, lsl #4
     778: e59d3054     	ldr	r3, [sp, #0x54]
     77c: e3530000     	cmp	r3, #0
     780: ba000018     	blt	0x7e8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x7e8> @ imm = #0x60
     784: e59d3054     	ldr	r3, [sp, #0x54]
     788: e24c1002     	sub	r1, r12, #2
     78c: e59d70a0     	ldr	r7, [sp, #0xa0]
     790: e08ce083     	add	lr, r12, r3, lsl #1
     794: e59dc010     	ldr	r12, [sp, #0x10]
     798: e1a02448     	asr	r2, r8, #8
     79c: e1f160b2     	ldrh	r6, [r1, #2]!
     7a0: e7e73859     	ubfx	r3, r9, #0x10, #0x8
     7a4: e1a05720     	lsr	r5, r0, #14
     7a8: e2022cff     	and	r2, r2, #65280
     7ac: e1823003     	orr	r3, r2, r3
     7b0: e59d2064     	ldr	r2, [sp, #0x64]
     7b4: e1560005     	cmp	r6, r5
     7b8: e080000a     	add	r0, r0, r10
     7bc: e0899007     	add	r9, r9, r7
     7c0: e2844004     	add	r4, r4, #4
     7c4: e0888002     	add	r8, r8, r2
     7c8: 920c28ff     	andls	r2, r12, #16711680
     7cc: 91833002     	orrls	r3, r3, r2
     7d0: 95043004     	strls	r3, [r4, #-0x4]
     7d4: e59d3060     	ldr	r3, [sp, #0x60]
     7d8: 91c150b0     	strhls	r5, [r1]
     7dc: e15e0001     	cmp	lr, r1
     7e0: e08cc003     	add	r12, r12, r3
     7e4: 1affffeb     	bne	0x798 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x798> @ imm = #-0x54
     7e8: e59d30d8     	ldr	r3, [sp, #0xd8]
     7ec: e08bb003     	add	r11, r11, r3
     7f0: e35b0000     	cmp	r11, #0
     7f4: da000030     	ble	0x8bc <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x8bc> @ imm = #0xc0
     7f8: e59d3094     	ldr	r3, [sp, #0x94]
     7fc: e24bb801     	sub	r11, r11, #65536
     800: e59d2104     	ldr	r2, [sp, #0x104]
     804: e0833002     	add	r3, r3, r2
     808: e59d20f4     	ldr	r2, [sp, #0xf4]
     80c: e58d3094     	str	r3, [sp, #0x94]
     810: e59d307c     	ldr	r3, [sp, #0x7c]
     814: e0833002     	add	r3, r3, r2
     818: e59d20f8     	ldr	r2, [sp, #0xf8]
     81c: e58d307c     	str	r3, [sp, #0x7c]
     820: e59d3080     	ldr	r3, [sp, #0x80]
     824: e0833002     	add	r3, r3, r2
     828: e59d20fc     	ldr	r2, [sp, #0xfc]
     82c: e58d3080     	str	r3, [sp, #0x80]
     830: e59d3084     	ldr	r3, [sp, #0x84]
     834: e0833002     	add	r3, r3, r2
     838: e59d2100     	ldr	r2, [sp, #0x100]
     83c: e58d3084     	str	r3, [sp, #0x84]
     840: e59d3088     	ldr	r3, [sp, #0x88]
     844: e0833002     	add	r3, r3, r2
     848: e58d3088     	str	r3, [sp, #0x88]
     84c: e59d308c     	ldr	r3, [sp, #0x8c]
     850: e59d20cc     	ldr	r2, [sp, #0xcc]
     854: e3530000     	cmp	r3, #0
     858: e59d3090     	ldr	r3, [sp, #0x90]
     85c: e0833002     	add	r3, r3, r2
     860: e59d209c     	ldr	r2, [sp, #0x9c]
     864: e58d3090     	str	r3, [sp, #0x90]
     868: e59d30d4     	ldr	r3, [sp, #0xd4]
     86c: e5933020     	ldr	r3, [r3, #0x20]
     870: e0822103     	add	r2, r2, r3, lsl #2
     874: e58d209c     	str	r2, [sp, #0x9c]
     878: e59d2098     	ldr	r2, [sp, #0x98]
     87c: e0823083     	add	r3, r2, r3, lsl #1
     880: e58d3098     	str	r3, [sp, #0x98]
     884: 1afffef4     	bne	0x45c <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x45c> @ imm = #-0x430
     888: e59d9140     	ldr	r9, [sp, #0x140]
     88c: e59d30cc     	ldr	r3, [sp, #0xcc]
     890: e58db010     	str	r11, [sp, #0x10]
     894: e1a0b00a     	mov	r11, r10
     898: e59da148     	ldr	r10, [sp, #0x148]
     89c: e59d6144     	ldr	r6, [sp, #0x144]
     8a0: e59d814c     	ldr	r8, [sp, #0x14c]
     8a4: e02aa399     	mla	r10, r9, r3, r10
     8a8: e3560001     	cmp	r6, #1
     8ac: 1a00006a     	bne	0xa5c <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xa5c> @ imm = #0x1a8
     8b0: e28ddf63     	add	sp, sp, #396
     8b4: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     8b8: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     8bc: e59d3094     	ldr	r3, [sp, #0x94]
     8c0: e59d2118     	ldr	r2, [sp, #0x118]
     8c4: e0833002     	add	r3, r3, r2
     8c8: e59d2108     	ldr	r2, [sp, #0x108]
     8cc: e58d3094     	str	r3, [sp, #0x94]
     8d0: e59d307c     	ldr	r3, [sp, #0x7c]
     8d4: e0833002     	add	r3, r3, r2
     8d8: e59d210c     	ldr	r2, [sp, #0x10c]
     8dc: e58d307c     	str	r3, [sp, #0x7c]
     8e0: e59d3080     	ldr	r3, [sp, #0x80]
     8e4: e0833002     	add	r3, r3, r2
     8e8: e59d2110     	ldr	r2, [sp, #0x110]
     8ec: e58d3080     	str	r3, [sp, #0x80]
     8f0: e59d3084     	ldr	r3, [sp, #0x84]
     8f4: e0833002     	add	r3, r3, r2
     8f8: e59d2114     	ldr	r2, [sp, #0x114]
     8fc: e58d3084     	str	r3, [sp, #0x84]
     900: e59d3088     	ldr	r3, [sp, #0x88]
     904: e0833002     	add	r3, r3, r2
     908: e58d3088     	str	r3, [sp, #0x88]
     90c: eaffffce     	b	0x84c <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x84c> @ imm = #-0xc8
     910: e59d3080     	ldr	r3, [sp, #0x80]
     914: e2818001     	add	r8, r1, #1
     918: eddd3b4a     	vldr	d19, [sp, #296]
     91c: e1a0700c     	mov	r7, r12
     920: e59de154     	ldr	lr, [sp, #0x154]
     924: e1a06004     	mov	r6, r4
     928: eddd4b4c     	vldr	d20, [sp, #304]
     92c: e3a05004     	mov	r5, #4
     930: eeac3b90     	vdup.32	q14, r3
     934: e28d3060     	add	r3, sp, #96
     938: f4e3380f     	vld1.32	{d19[0]}, [r3]
     93c: e59d3084     	ldr	r3, [sp, #0x84]
     940: eddd5b4e     	vldr	d21, [sp, #312]
     944: eddd2b48     	vldr	d18, [sp, #288]
     948: eeaa3b90     	vdup.32	q13, r3
     94c: e28d3064     	add	r3, sp, #100
     950: f4e3480f     	vld1.32	{d20[0]}, [r3]
     954: e59d3088     	ldr	r3, [sp, #0x88]
     958: eeae0b90     	vdup.32	q15, r0
     95c: ee02ab90     	vmov.32	d18[0], r10
     960: eea83b90     	vdup.32	q12, r3
     964: e28d30a0     	add	r3, sp, #160
     968: f22281b2     	vorr	d8, d18, d18
     96c: f4e3580f     	vld1.32	{d21[0]}, [r3]
     970: e59d3150     	ldr	r3, [sp, #0x150]
     974: edcd2b48     	vstr	d18, [sp, #288]
     978: edcd3b4a     	vstr	d19, [sp, #296]
     97c: e893000f     	ldm	r3, {r0, r1, r2, r3}
     980: edcd4b4c     	vstr	d20, [sp, #304]
     984: e88e000f     	stm	lr, {r0, r1, r2, r3}
     988: f46e0a9f     	vld1.32	{d16, d17}, [lr:64]
     98c: f3e0e0c8     	vmla.i32	q15, q8, d8[0]
     990: edcd5b4e     	vstr	d21, [sp, #312]
     994: f22381b3     	vorr	d8, d19, d19
     998: f3e0c0c8     	vmla.i32	q14, q8, d8[0]
     99c: f22481b4     	vorr	d8, d20, d20
     9a0: f3e0a0c8     	vmla.i32	q13, q8, d8[0]
     9a4: f22581b5     	vorr	d8, d21, d21
     9a8: f3e080c8     	vmla.i32	q12, q8, d8[0]
     9ac: e1a09005     	mov	r9, r5
     9b0: e2855004     	add	r5, r5, #4
     9b4: e1580005     	cmp	r8, r5
     9b8: f467174f     	vld1.16	{d17}, [r7]
     9bc: f3f8207a     	vshr.u32	q9, q13, #0x8
     9c0: f3f06078     	vshr.u32	q11, q12, #0x10
     9c4: f24221d0     	vand	q9, q9, q0
     9c8: f24661d2     	vand	q11, q11, q1
     9cc: f3b2807e     	vshr.u32	q4, q15, #0xe
     9d0: f26221f6     	vorr	q9, q9, q11
     9d4: f3d06a31     	vmovl.u16	q11, d17
     9d8: f3f65208     	vmovn.i32	d21, q4
     9dc: f26ee8ca     	vadd.i32	q15, q15, q5
     9e0: f3686376     	vcge.u32	q11, q4, q11
     9e4: f26aa8ce     	vadd.i32	q13, q13, q7
     9e8: f26888c6     	vadd.i32	q12, q12, q3
     9ec: f3f64226     	vmovn.i32	d20, q11
     9f0: f35541b1     	vbsl	d20, d21, d17
     9f4: f24c01d4     	vand	q8, q14, q2
     9f8: f26cc8cc     	vadd.i32	q14, q14, q6
     9fc: f26221f0     	vorr	q9, q9, q8
     a00: f4660a8f     	vld1.32	{d16, d17}, [r6]
     a04: f36201f6     	vbit	q8, q9, q11
     a08: f4460a8d     	vst1.32	{d16, d17}, [r6]!
     a0c: f447474d     	vst1.16	{d20}, [r7]!
     a10: 2affffe5     	bhs	0x9ac <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x9ac> @ imm = #-0x6c
     a14: e59d3054     	ldr	r3, [sp, #0x54]
     a18: e0844109     	add	r4, r4, r9, lsl #2
     a1c: e59d2060     	ldr	r2, [sp, #0x60]
     a20: e08cc089     	add	r12, r12, r9, lsl #1
     a24: e0433009     	sub	r3, r3, r9
     a28: e58d3054     	str	r3, [sp, #0x54]
     a2c: e59d307c     	ldr	r3, [sp, #0x7c]
     a30: e0203a99     	mla	r0, r9, r10, r3
     a34: e59d3080     	ldr	r3, [sp, #0x80]
     a38: e0233299     	mla	r3, r9, r2, r3
     a3c: e59d2064     	ldr	r2, [sp, #0x64]
     a40: e58d3010     	str	r3, [sp, #0x10]
     a44: e59d3084     	ldr	r3, [sp, #0x84]
     a48: e0283299     	mla	r8, r9, r2, r3
     a4c: e59d3088     	ldr	r3, [sp, #0x88]
     a50: e59d20a0     	ldr	r2, [sp, #0xa0]
     a54: e0293299     	mla	r9, r9, r2, r3
     a58: eafffe93     	b	0x4ac <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x4ac> @ imm = #-0x5b4
     a5c: ed9d8a5c     	vldr	s16, [sp, #368]
     a60: e59d211c     	ldr	r2, [sp, #0x11c]
     a64: e5981004     	ldr	r1, [r8, #0x4]
     a68: e5923004     	ldr	r3, [r2, #0x4]
     a6c: eeb58ac0     	vcmpe.f32	s16, #0
     a70: eef1fa10     	vmrs	APSR_nzcv, fpscr
     a74: e0411003     	sub	r1, r1, r3
     a78: e2819001     	add	r9, r1, #1
     a7c: da00002b     	ble	0xb30 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xb30> @ imm = #0xac
     a80: e3510000     	cmp	r1, #0
     a84: e592a000     	ldr	r10, [r2]
     a88: e3a04001     	mov	r4, #1
     a8c: da00002c     	ble	0xb44 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xb44> @ imm = #0xb0
     a90: e59d311c     	ldr	r3, [sp, #0x11c]
     a94: e58d815c     	str	r8, [sp, #0x15c]
     a98: e58d3174     	str	r3, [sp, #0x174]
     a9c: e28d3040     	add	r3, sp, #64
     aa0: f40d0adf     	vst1.64	{d0, d1}, [sp:64]
     aa4: e1a06004     	mov	r6, r4
     aa8: f4036adf     	vst1.64	{d6, d7}, [r3:64]
     aac: e28d3030     	add	r3, sp, #48
     ab0: f4034adf     	vst1.64	{d4, d5}, [r3:64]
     ab4: e28d3020     	add	r3, sp, #32
     ab8: f4032adf     	vst1.64	{d2, d3}, [r3:64]
     abc: e59d315c     	ldr	r3, [sp, #0x15c]
     ac0: e5930000     	ldr	r0, [r3]
     ac4: e040000a     	sub	r0, r0, r10
     ac8: e1a00800     	lsl	r0, r0, #16
     acc: ebfffffe     	bl	0xacc <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xacc> @ imm = #-0x8
     ad0: e28d3020     	add	r3, sp, #32
     ad4: f42d0adf     	vld1.64	{d0, d1}, [sp:64]
     ad8: e58d00cc     	str	r0, [sp, #0xcc]
     adc: f4232adf     	vld1.64	{d2, d3}, [r3:64]
     ae0: e28d3030     	add	r3, sp, #48
     ae4: f4234adf     	vld1.64	{d4, d5}, [r3:64]
     ae8: e28d3040     	add	r3, sp, #64
     aec: f4236adf     	vld1.64	{d6, d7}, [r3:64]
     af0: e1a0a80a     	lsl	r10, r10, #16
     af4: eafffe3b     	b	0x3e8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x3e8> @ imm = #-0x714
     af8: e59d216c     	ldr	r2, [sp, #0x16c]
     afc: e3a03000     	mov	r3, #0
     b00: e58d30d8     	str	r3, [sp, #0xd8]
     b04: e58d3118     	str	r3, [sp, #0x118]
     b08: e58d2114     	str	r2, [sp, #0x114]
     b0c: e59d2168     	ldr	r2, [sp, #0x168]
     b10: e58d2110     	str	r2, [sp, #0x110]
     b14: e59d2164     	ldr	r2, [sp, #0x164]
     b18: e58d210c     	str	r2, [sp, #0x10c]
     b1c: e59d2160     	ldr	r2, [sp, #0x160]
     b20: e58d2108     	str	r2, [sp, #0x108]
     b24: e3a02001     	mov	r2, #1
     b28: e58d2104     	str	r2, [sp, #0x104]
     b2c: eafffe22     	b	0x3bc <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x3bc> @ imm = #-0x778
     b30: e58d6010     	str	r6, [sp, #0x10]
     b34: e1a02008     	mov	r2, r8
     b38: e59d411c     	ldr	r4, [sp, #0x11c]
     b3c: e3a06001     	mov	r6, #1
     b40: eafffdf2     	b	0x310 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x310> @ imm = #-0x838
     b44: e3590000     	cmp	r9, #0
     b48: daffff58     	ble	0x8b0 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x8b0> @ imm = #-0x2a0
     b4c: e58d60cc     	str	r6, [sp, #0xcc]
     b50: e1a0a80a     	lsl	r10, r10, #16
     b54: e1a06004     	mov	r6, r4
     b58: e58d2174     	str	r2, [sp, #0x174]
     b5c: e58d815c     	str	r8, [sp, #0x15c]
     b60: eafffe22     	b	0x3f0 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x3f0> @ imm = #-0x778
     b64: e59d211c     	ldr	r2, [sp, #0x11c]
     b68: e1a01003     	mov	r1, r3
     b6c: e58d815c     	str	r8, [sp, #0x15c]
     b70: eafffdd0     	b	0x2b8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x2b8> @ imm = #-0x8c0
     b74: e59d315c     	ldr	r3, [sp, #0x15c]
     b78: e59d2174     	ldr	r2, [sp, #0x174]
     b7c: e5931004     	ldr	r1, [r3, #0x4]
     b80: e5923004     	ldr	r3, [r2, #0x4]
     b84: e592a000     	ldr	r10, [r2]
     b88: e0411003     	sub	r1, r1, r3
     b8c: e3510000     	cmp	r1, #0
     b90: c3a03000     	movgt	r3, #0
     b94: c1a04006     	movgt	r4, r6
     b98: c58d3010     	strgt	r3, [sp, #0x10]
     b9c: caffffbe     	bgt	0xa9c <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xa9c> @ imm = #-0x108
     ba0: e3a03000     	mov	r3, #0
     ba4: e58d3010     	str	r3, [sp, #0x10]
     ba8: e58d30cc     	str	r3, [sp, #0xcc]
     bac: eaffffcf     	b	0xaf0 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xaf0> @ imm = #-0xc4
     bb0: 60 0b 00 00  	.word	0x00000b60
     bb4: c4 08 00 00  	.word	0x000008c4
     bb8: 00 00 00 00  	.word	0x00000000

Disassembly of section .text.ZB_setTexture:

00000000 <ZB_setTexture>:
       0: e5801008     	str	r1, [r0, #0x8]
       4: e12fff1e     	bx	lr

Disassembly of section .text.ZB_fillTriangleFlat:

00000000 <ZB_fillTriangleFlat>:
       0: e52de004     	str	lr, [sp, #-0x4]!
       4: e590e02c     	ldr	lr, [r0, #0x2c]
       8: e590c030     	ldr	r12, [r0, #0x30]
       c: e25ee000     	subs	lr, lr, #0
      10: 13a0e001     	movne	lr, #1
      14: e25cc000     	subs	r12, r12, #0
      18: 13a0c001     	movne	r12, #1
      1c: e18cc08e     	orr	r12, r12, lr, lsl #1
      20: e35c0002     	cmp	r12, #2
      24: 0a000007     	beq	0x48 <ZB_fillTriangleFlat+0x48> @ imm = #0x1c
      28: e35c0003     	cmp	r12, #3
      2c: 0a000003     	beq	0x40 <ZB_fillTriangleFlat+0x40> @ imm = #0xc
      30: e35c0001     	cmp	r12, #1
      34: 0a000005     	beq	0x50 <ZB_fillTriangleFlat+0x50> @ imm = #0x14
      38: e49de004     	ldr	lr, [sp], #4
      3c: eafffffe     	b	0x3c <ZB_fillTriangleFlat+0x3c> @ imm = #-0x8
      40: e49de004     	ldr	lr, [sp], #4
      44: eafffffe     	b	0x44 <ZB_fillTriangleFlat+0x44> @ imm = #-0x8
      48: e49de004     	ldr	lr, [sp], #4
      4c: eafffffe     	b	0x4c <ZB_fillTriangleFlat+0x4c> @ imm = #-0x8
      50: e49de004     	ldr	lr, [sp], #4
      54: eafffffe     	b	0x54 <ZB_fillTriangleFlat+0x54> @ imm = #-0x8

Disassembly of section .text.ZB_fillTriangleFlatNOBLEND:

00000000 <ZB_fillTriangleFlatNOBLEND>:
       0: e52de004     	str	lr, [sp, #-0x4]!
       4: e590e02c     	ldr	lr, [r0, #0x2c]
       8: e590c030     	ldr	r12, [r0, #0x30]
       c: e25ee000     	subs	lr, lr, #0
      10: 13a0e001     	movne	lr, #1
      14: e25cc000     	subs	r12, r12, #0
      18: 13a0c001     	movne	r12, #1
      1c: e18cc08e     	orr	r12, r12, lr, lsl #1
      20: e35c0002     	cmp	r12, #2
      24: 0a000007     	beq	0x48 <ZB_fillTriangleFlatNOBLEND+0x48> @ imm = #0x1c
      28: e35c0003     	cmp	r12, #3
      2c: 0a000003     	beq	0x40 <ZB_fillTriangleFlatNOBLEND+0x40> @ imm = #0xc
      30: e35c0001     	cmp	r12, #1
      34: 0a000005     	beq	0x50 <ZB_fillTriangleFlatNOBLEND+0x50> @ imm = #0x14
      38: e49de004     	ldr	lr, [sp], #4
      3c: eafffffe     	b	0x3c <ZB_fillTriangleFlatNOBLEND+0x3c> @ imm = #-0x8
      40: e49de004     	ldr	lr, [sp], #4
      44: eafffffe     	b	0x44 <ZB_fillTriangleFlatNOBLEND+0x44> @ imm = #-0x8
      48: e49de004     	ldr	lr, [sp], #4
      4c: eafffffe     	b	0x4c <ZB_fillTriangleFlatNOBLEND+0x4c> @ imm = #-0x8
      50: e49de004     	ldr	lr, [sp], #4
      54: eafffffe     	b	0x54 <ZB_fillTriangleFlatNOBLEND+0x54> @ imm = #-0x8

Disassembly of section .text.ZB_fillTriangleSmooth:

00000000 <ZB_fillTriangleSmooth>:
       0: e52de004     	str	lr, [sp, #-0x4]!
       4: e590e02c     	ldr	lr, [r0, #0x2c]
       8: e590c030     	ldr	r12, [r0, #0x30]
       c: e25ee000     	subs	lr, lr, #0
      10: 13a0e001     	movne	lr, #1
      14: e25cc000     	subs	r12, r12, #0
      18: 13a0c001     	movne	r12, #1
      1c: e18cc08e     	orr	r12, r12, lr, lsl #1
      20: e35c0002     	cmp	r12, #2
      24: 0a000007     	beq	0x48 <ZB_fillTriangleSmooth+0x48> @ imm = #0x1c
      28: e35c0003     	cmp	r12, #3
      2c: 0a000003     	beq	0x40 <ZB_fillTriangleSmooth+0x40> @ imm = #0xc
      30: e35c0001     	cmp	r12, #1
      34: 0a000005     	beq	0x50 <ZB_fillTriangleSmooth+0x50> @ imm = #0x14
      38: e49de004     	ldr	lr, [sp], #4
      3c: eafffffe     	b	0x3c <ZB_fillTriangleSmooth+0x3c> @ imm = #-0x8
      40: e49de004     	ldr	lr, [sp], #4
      44: eafffffe     	b	0x44 <ZB_fillTriangleSmooth+0x44> @ imm = #-0x8
      48: e49de004     	ldr	lr, [sp], #4
      4c: eafffffe     	b	0x4c <ZB_fillTriangleSmooth+0x4c> @ imm = #-0x8
      50: e49de004     	ldr	lr, [sp], #4
      54: eafffffe     	b	0x54 <ZB_fillTriangleSmooth+0x54> @ imm = #-0x8

Disassembly of section .text.ZB_fillTriangleSmoothNOBLEND:

00000000 <ZB_fillTriangleSmoothNOBLEND>:
       0: e52de004     	str	lr, [sp, #-0x4]!
       4: e590e02c     	ldr	lr, [r0, #0x2c]
       8: e590c030     	ldr	r12, [r0, #0x30]
       c: e25ee000     	subs	lr, lr, #0
      10: 13a0e001     	movne	lr, #1
      14: e25cc000     	subs	r12, r12, #0
      18: 13a0c001     	movne	r12, #1
      1c: e18cc08e     	orr	r12, r12, lr, lsl #1
      20: e35c0002     	cmp	r12, #2
      24: 0a000007     	beq	0x48 <ZB_fillTriangleSmoothNOBLEND+0x48> @ imm = #0x1c
      28: e35c0003     	cmp	r12, #3
      2c: 0a000003     	beq	0x40 <ZB_fillTriangleSmoothNOBLEND+0x40> @ imm = #0xc
      30: e35c0001     	cmp	r12, #1
      34: 0a000005     	beq	0x50 <ZB_fillTriangleSmoothNOBLEND+0x50> @ imm = #0x14
      38: e49de004     	ldr	lr, [sp], #4
      3c: eafffffe     	b	0x3c <ZB_fillTriangleSmoothNOBLEND+0x3c> @ imm = #-0x8
      40: e49de004     	ldr	lr, [sp], #4
      44: eafffffe     	b	0x44 <ZB_fillTriangleSmoothNOBLEND+0x44> @ imm = #-0x8
      48: e49de004     	ldr	lr, [sp], #4
      4c: eafffffe     	b	0x4c <ZB_fillTriangleSmoothNOBLEND+0x4c> @ imm = #-0x8
      50: e49de004     	ldr	lr, [sp], #4
      54: eafffffe     	b	0x54 <ZB_fillTriangleSmoothNOBLEND+0x54> @ imm = #-0x8

Disassembly of section .text.ZB_fillTriangleMappingPerspective:

00000000 <ZB_fillTriangleMappingPerspective>:
       0: e52de004     	str	lr, [sp, #-0x4]!
       4: e590e02c     	ldr	lr, [r0, #0x2c]
       8: e590c030     	ldr	r12, [r0, #0x30]
       c: e25ee000     	subs	lr, lr, #0
      10: 13a0e001     	movne	lr, #1
      14: e25cc000     	subs	r12, r12, #0
      18: 13a0c001     	movne	r12, #1
      1c: e18cc08e     	orr	r12, r12, lr, lsl #1
      20: e35c0002     	cmp	r12, #2
      24: 0a000007     	beq	0x48 <ZB_fillTriangleMappingPerspective+0x48> @ imm = #0x1c
      28: e35c0003     	cmp	r12, #3
      2c: 0a000003     	beq	0x40 <ZB_fillTriangleMappingPerspective+0x40> @ imm = #0xc
      30: e35c0001     	cmp	r12, #1
      34: 0a000005     	beq	0x50 <ZB_fillTriangleMappingPerspective+0x50> @ imm = #0x14
      38: e49de004     	ldr	lr, [sp], #4
      3c: eafffffe     	b	0x3c <ZB_fillTriangleMappingPerspective+0x3c> @ imm = #-0x8
      40: e49de004     	ldr	lr, [sp], #4
      44: eafffffe     	b	0x44 <ZB_fillTriangleMappingPerspective+0x44> @ imm = #-0x8
      48: e49de004     	ldr	lr, [sp], #4
      4c: eafffffe     	b	0x4c <ZB_fillTriangleMappingPerspective+0x4c> @ imm = #-0x8
      50: e49de004     	ldr	lr, [sp], #4
      54: eafffffe     	b	0x54 <ZB_fillTriangleMappingPerspective+0x54> @ imm = #-0x8

Disassembly of section .text.ZB_fillTriangleMappingPerspectiveNOBLEND:

00000000 <ZB_fillTriangleMappingPerspectiveNOBLEND>:
       0: e52de004     	str	lr, [sp, #-0x4]!
       4: e590e02c     	ldr	lr, [r0, #0x2c]
       8: e590c030     	ldr	r12, [r0, #0x30]
       c: e25ee000     	subs	lr, lr, #0
      10: 13a0e001     	movne	lr, #1
      14: e25cc000     	subs	r12, r12, #0
      18: 13a0c001     	movne	r12, #1
      1c: e18cc08e     	orr	r12, r12, lr, lsl #1
      20: e35c0002     	cmp	r12, #2
      24: 0a000007     	beq	0x48 <ZB_fillTriangleMappingPerspectiveNOBLEND+0x48> @ imm = #0x1c
      28: e35c0003     	cmp	r12, #3
      2c: 0a000003     	beq	0x40 <ZB_fillTriangleMappingPerspectiveNOBLEND+0x40> @ imm = #0xc
      30: e35c0001     	cmp	r12, #1
      34: 0a000005     	beq	0x50 <ZB_fillTriangleMappingPerspectiveNOBLEND+0x50> @ imm = #0x14
      38: e49de004     	ldr	lr, [sp], #4
      3c: eafffffe     	b	0x3c <ZB_fillTriangleMappingPerspectiveNOBLEND+0x3c> @ imm = #-0x8
      40: e49de004     	ldr	lr, [sp], #4
      44: eafffffe     	b	0x44 <ZB_fillTriangleMappingPerspectiveNOBLEND+0x44> @ imm = #-0x8
      48: e49de004     	ldr	lr, [sp], #4
      4c: eafffffe     	b	0x4c <ZB_fillTriangleMappingPerspectiveNOBLEND+0x4c> @ imm = #-0x8
      50: e49de004     	ldr	lr, [sp], #4
      54: eafffffe     	b	0x54 <ZB_fillTriangleMappingPerspectiveNOBLEND+0x54> @ imm = #-0x8


.context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-pic-vfp-matrix-bench-tinygl-internal-raster/90-clear.arm.o:	file format elf32-littlearm

Disassembly of section .text.glopClearColor:

00000000 <glopClearColor>:
       0: e92d4010     	push	{r4, lr}
       4: e1a04000     	mov	r4, r0
       8: ebfffffe     	bl	0x8 <glopClearColor+0x8> @ imm = #-0x8
       c: e2802b03     	add	r2, r0, #3072
      10: e5941004     	ldr	r1, [r4, #0x4]
      14: e1a03002     	mov	r3, r2
      18: e5821000     	str	r1, [r2]
      1c: e5942008     	ldr	r2, [r4, #0x8]
      20: e5832004     	str	r2, [r3, #0x4]
      24: e2803b03     	add	r3, r0, #3072
      28: e1a00003     	mov	r0, r3
      2c: e594200c     	ldr	r2, [r4, #0xc]
      30: e5832008     	str	r2, [r3, #0x8]
      34: e5943010     	ldr	r3, [r4, #0x10]
      38: e580300c     	str	r3, [r0, #0xc]
      3c: e8bd8010     	pop	{r4, pc}

Disassembly of section .text.glopClearDepth:

00000000 <glopClearDepth>:
       0: e92d4010     	push	{r4, lr}
       4: e1a04000     	mov	r4, r0
       8: ebfffffe     	bl	0x8 <glopClearDepth+0x8> @ imm = #-0x8
       c: e2800ed1     	add	r0, r0, #3344
      10: e5943004     	ldr	r3, [r4, #0x4]
      14: e5803000     	str	r3, [r0]
      18: e8bd8010     	pop	{r4, pc}

Disassembly of section .text.glopClear:

00000000 <glopClear>:
       0: e92d4010     	push	{r4, lr}
       4: e24dd010     	sub	sp, sp, #16
       8: e1a04000     	mov	r4, r0
       c: ebfffffe     	bl	0xc <glopClear+0xc>     @ imm = #-0x8
      10: e280eb03     	add	lr, r0, #3072
      14: ed9f7a13     	vldr	s14, [pc, #76]          @ 0x68 <glopClear+0x68>
      18: edde7a02     	vldr	s15, [lr, #8]
      1c: e3a02000     	mov	r2, #0
      20: e5941004     	ldr	r1, [r4, #0x4]
      24: e5900c5c     	ldr	r0, [r0, #0xc5c]
      28: ee677a87     	vmul.f32	s15, s15, s14
      2c: e2013901     	and	r3, r1, #16384
      30: e2011c01     	and	r1, r1, #256
      34: eefd7ae7     	vcvt.s32.f32	s15, s15
      38: edcd7a02     	vstr	s15, [sp, #8]
      3c: edde7a01     	vldr	s15, [lr, #4]
      40: ee677a87     	vmul.f32	s15, s15, s14
      44: eefd7ae7     	vcvt.s32.f32	s15, s15
      48: edcd7a01     	vstr	s15, [sp, #4]
      4c: edde7a00     	vldr	s15, [lr]
      50: ee677a87     	vmul.f32	s15, s15, s14
      54: eefd7ae7     	vcvt.s32.f32	s15, s15
      58: edcd7a00     	vstr	s15, [sp]
      5c: ebfffffe     	bl	0x5c <glopClear+0x5c>   @ imm = #-0x8
      60: e28dd010     	add	sp, sp, #16
      64: e8bd8010     	pop	{r4, pc}
      68: 00 00 7f 4b  	.word	0x4b7f0000


.context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-pic-vfp-matrix-bench-tinygl-internal-raster/115-clear.arm.o:	file format elf32-littlearm

Disassembly of section .text.zz_fill32:

00000000 <zz_fill32>:
       0: e352000f     	cmp	r2, #15
       4: e92d4070     	push	{r4, r5, r6, lr}
       8: eea01b90     	vdup.32	q8, r1
       c: e1a04002     	mov	r4, r2
      10: e1a05001     	mov	r5, r1
      14: e1a06000     	mov	r6, r0
      18: 9a00001e     	bls	0x98 <zz_fill32+0x98>   @ imm = #0x78
      1c: e1a0c000     	mov	r12, r0
      20: e3a00010     	mov	r0, #16
      24: e1a03000     	mov	r3, r0
      28: e2800010     	add	r0, r0, #16
      2c: e1a0100c     	mov	r1, r12
      30: e28c2020     	add	r2, r12, #32
      34: e28ce030     	add	lr, r12, #48
      38: e1540000     	cmp	r4, r0
      3c: f4410a8d     	vst1.32	{d16, d17}, [r1]!
      40: e28cc040     	add	r12, r12, #64
      44: f4410a8f     	vst1.32	{d16, d17}, [r1]
      48: f4420a8f     	vst1.32	{d16, d17}, [r2]
      4c: f44e0a8f     	vst1.32	{d16, d17}, [lr]
      50: 2afffff3     	bhs	0x24 <zz_fill32+0x24>   @ imm = #-0x34
      54: e283c004     	add	r12, r3, #4
      58: e154000c     	cmp	r4, r12
      5c: 3a000005     	blo	0x78 <zz_fill32+0x78>   @ imm = #0x14
      60: e0862103     	add	r2, r6, r3, lsl #2
      64: e1a0300c     	mov	r3, r12
      68: e28cc004     	add	r12, r12, #4
      6c: e154000c     	cmp	r4, r12
      70: f4420a8f     	vst1.32	{d16, d17}, [r2]
      74: 2afffff9     	bhs	0x60 <zz_fill32+0x60>   @ imm = #-0x1c
      78: e1540003     	cmp	r4, r3
      7c: 98bd8070     	popls	{r4, r5, r6, pc}
      80: e0863103     	add	r3, r6, r3, lsl #2
      84: e0866104     	add	r6, r6, r4, lsl #2
      88: e4835004     	str	r5, [r3], #4
      8c: e1530006     	cmp	r3, r6
      90: 1afffffc     	bne	0x88 <zz_fill32+0x88>   @ imm = #-0x10
      94: e8bd8070     	pop	{r4, r5, r6, pc}
      98: e3a0c004     	mov	r12, #4
      9c: e3a03000     	mov	r3, #0
      a0: eaffffec     	b	0x58 <zz_fill32+0x58>   @ imm = #-0x50

Disassembly of section .text.zz_fill16:

00000000 <zz_fill16>:
       0: e352001f     	cmp	r2, #31
       4: e92d4070     	push	{r4, r5, r6, lr}
       8: eea01bb0     	vdup.16	q8, r1
       c: e1a05002     	mov	r5, r2
      10: e1a06001     	mov	r6, r1
      14: e1a04000     	mov	r4, r0
      18: 9a00001f     	bls	0x9c <zz_fill16+0x9c>   @ imm = #0x7c
      1c: e1a0c000     	mov	r12, r0
      20: e3a00020     	mov	r0, #32
      24: e1a03000     	mov	r3, r0
      28: e2800020     	add	r0, r0, #32
      2c: e1a0100c     	mov	r1, r12
      30: e28c2020     	add	r2, r12, #32
      34: e28ce030     	add	lr, r12, #48
      38: e1550000     	cmp	r5, r0
      3c: f4410a4d     	vst1.16	{d16, d17}, [r1]!
      40: e28cc040     	add	r12, r12, #64
      44: f4410a4f     	vst1.16	{d16, d17}, [r1]
      48: f4420a4f     	vst1.16	{d16, d17}, [r2]
      4c: f44e0a4f     	vst1.16	{d16, d17}, [lr]
      50: 2afffff3     	bhs	0x24 <zz_fill16+0x24>   @ imm = #-0x34
      54: e283c008     	add	r12, r3, #8
      58: e155000c     	cmp	r5, r12
      5c: 3a000005     	blo	0x78 <zz_fill16+0x78>   @ imm = #0x14
      60: e0842083     	add	r2, r4, r3, lsl #1
      64: e1a0300c     	mov	r3, r12
      68: e28cc008     	add	r12, r12, #8
      6c: e155000c     	cmp	r5, r12
      70: f4420a4f     	vst1.16	{d16, d17}, [r2]
      74: 2afffff9     	bhs	0x60 <zz_fill16+0x60>   @ imm = #-0x1c
      78: e1550003     	cmp	r5, r3
      7c: 98bd8070     	popls	{r4, r5, r6, pc}
      80: e2444002     	sub	r4, r4, #2
      84: e0843083     	add	r3, r4, r3, lsl #1
      88: e0844085     	add	r4, r4, r5, lsl #1
      8c: e1e360b2     	strh	r6, [r3, #2]!
      90: e1530004     	cmp	r3, r4
      94: 1afffffc     	bne	0x8c <zz_fill16+0x8c>   @ imm = #-0x10
      98: e8bd8070     	pop	{r4, r5, r6, pc}
      9c: e3a0c008     	mov	r12, #8
      a0: e3a03000     	mov	r3, #0
      a4: eaffffeb     	b	0x58 <zz_fill16+0x58>   @ imm = #-0x54

Disassembly of section .text.zz_clear_buffers:

00000000 <zz_clear_buffers>:
       0: e3510000     	cmp	r1, #0
       4: e92d4070     	push	{r4, r5, r6, lr}
       8: e1a04000     	mov	r4, r0
       c: e1a05003     	mov	r5, r3
      10: 1a00001a     	bne	0x80 <zz_clear_buffers+0x80> @ imm = #0x68
      14: e3550000     	cmp	r5, #0
      18: 0a000016     	beq	0x78 <zz_clear_buffers+0x78> @ imm = #0x58
      1c: e5943024     	ldr	r3, [r4, #0x24]
      20: e3530000     	cmp	r3, #0
      24: da000013     	ble	0x78 <zz_clear_buffers+0x78> @ imm = #0x4c
      28: e59d3014     	ldr	r3, [sp, #0x14]
      2c: e3a05000     	mov	r5, #0
      30: e1a06443     	asr	r6, r3, #8
      34: e59d3010     	ldr	r3, [sp, #0x10]
      38: e2066cff     	and	r6, r6, #65280
      3c: e20328ff     	and	r2, r3, #16711680
      40: e59d3018     	ldr	r3, [sp, #0x18]
      44: e1866002     	orr	r6, r6, r2
      48: e7e73853     	ubfx	r3, r3, #0x10, #0x8
      4c: e1866003     	orr	r6, r6, r3
      50: e5943004     	ldr	r3, [r4, #0x4]
      54: e1a01006     	mov	r1, r6
      58: e5940028     	ldr	r0, [r4, #0x28]
      5c: e5942020     	ldr	r2, [r4, #0x20]
      60: e0203590     	mla	r0, r0, r5, r3
      64: e2855001     	add	r5, r5, #1
      68: ebfffffe     	bl	0x68 <zz_clear_buffers+0x68> @ imm = #-0x8
      6c: e5943024     	ldr	r3, [r4, #0x24]
      70: e1530005     	cmp	r3, r5
      74: cafffff5     	bgt	0x50 <zz_clear_buffers+0x50> @ imm = #-0x2c
      78: e3a00001     	mov	r0, #1
      7c: e8bd8070     	pop	{r4, r5, r6, pc}
      80: e5903020     	ldr	r3, [r0, #0x20]
      84: e6ff1072     	uxth	r1, r2
      88: e5900024     	ldr	r0, [r0, #0x24]
      8c: e0020390     	mul	r2, r0, r3
      90: e5940000     	ldr	r0, [r4]
      94: ebfffffe     	bl	0x94 <zz_clear_buffers+0x94> @ imm = #-0x8
      98: eaffffdd     	b	0x14 <zz_clear_buffers+0x14> @ imm = #-0x8c

Disassembly of section .text.zz_raster_selftest:

00000000 <zz_raster_selftest>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e3a03000     	mov	r3, #0
       8: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
       c: e305b678     	movw	r11, #0x5678
      10: e24ddfb9     	sub	sp, sp, #740
      14: e1a09003     	mov	r9, r3
      18: e349b534     	movt	r11, #0x9534
      1c: e58d3008     	str	r3, [sp, #0x8]
      20: e28d3c01     	add	r3, sp, #256
      24: e3037456     	movw	r7, #0x3456
      28: e1a0800b     	mov	r8, r11
      2c: e58d3010     	str	r3, [sp, #0x10]
      30: ed9fabe8     	vldr	d10, [pc, #928]         @ 0x3d8 <zz_raster_selftest+0x3d8>
      34: ed9fbbe9     	vldr	d11, [pc, #932]         @ 0x3e0 <zz_raster_selftest+0x3e0>
      38: e28d3e22     	add	r3, sp, #544
      3c: e28d4e16     	add	r4, sp, #352
      40: e1a0b009     	mov	r11, r9
      44: e28d2090     	add	r2, sp, #144
      48: e34a7b12     	movt	r7, #0xab12
      4c: e58d3064     	str	r3, [sp, #0x64]
      50: e28d30a0     	add	r3, sp, #160
      54: e58d2080     	str	r2, [sp, #0x80]
      58: e58d300c     	str	r3, [sp, #0xc]
      5c: e58d7088     	str	r7, [sp, #0x88]
      60: e59d3008     	ldr	r3, [sp, #0x8]
      64: e1a0a008     	mov	r10, r8
      68: e28d6f87     	add	r6, sp, #540
      6c: e3a08000     	mov	r8, #0
      70: e28d2f57     	add	r2, sp, #348
      74: e58d4058     	str	r4, [sp, #0x58]
      78: e58d3060     	str	r3, [sp, #0x60]
      7c: e59d300c     	ldr	r3, [sp, #0xc]
      80: e58d2038     	str	r2, [sp, #0x38]
      84: e58d408c     	str	r4, [sp, #0x8c]
      88: e58d305c     	str	r3, [sp, #0x5c]
      8c: e2433002     	sub	r3, r3, #2
      90: e58d3084     	str	r3, [sp, #0x84]
      94: e3041567     	movw	r1, #0x4567
      98: e1a02088     	lsl	r2, r8, #1
      9c: e3401123     	movt	r1, #0x123
      a0: e1a03108     	lsl	r3, r8, #2
      a4: e58d1014     	str	r1, [sp, #0x14]
      a8: e1a0700a     	mov	r7, r10
      ac: e59d100c     	ldr	r1, [sp, #0xc]
      b0: e1a0a006     	mov	r10, r6
      b4: ed9f8bcb     	vldr	d8, [pc, #812]          @ 0x3e8 <zz_raster_selftest+0x3e8>
      b8: ed9f9bcc     	vldr	d9, [pc, #816]          @ 0x3f0 <zz_raster_selftest+0x3f0>
      bc: e1a06008     	mov	r6, r8
      c0: e0811002     	add	r1, r1, r2
      c4: e58d1078     	str	r1, [sp, #0x78]
      c8: e59d1010     	ldr	r1, [sp, #0x10]
      cc: e0812002     	add	r2, r1, r2
      d0: e58d2070     	str	r2, [sp, #0x70]
      d4: e59d208c     	ldr	r2, [sp, #0x8c]
      d8: e0822003     	add	r2, r2, r3
      dc: e58d207c     	str	r2, [sp, #0x7c]
      e0: e59d2064     	ldr	r2, [sp, #0x64]
      e4: e0823003     	add	r3, r2, r3
      e8: e58d3074     	str	r3, [sp, #0x74]
      ec: e3a03000     	mov	r3, #0
      f0: e58d3004     	str	r3, [sp, #0x4]
      f4: e2423004     	sub	r3, r2, #4
      f8: e58d3068     	str	r3, [sp, #0x68]
      fc: e2413002     	sub	r3, r1, #2
     100: e58d306c     	str	r3, [sp, #0x6c]
     104: e59d1068     	ldr	r1, [sp, #0x68]
     108: e3a03000     	mov	r3, #0
     10c: e59d0038     	ldr	r0, [sp, #0x38]
     110: e3e08000     	mvn	r8, #0
     114: e59d506c     	ldr	r5, [sp, #0x6c]
     118: e59de084     	ldr	lr, [sp, #0x84]
     11c: e1a0c001     	mov	r12, r1
     120: e1a04000     	mov	r4, r0
     124: e2139003     	ands	r9, r3, #3
     128: e0832007     	add	r2, r3, r7
     12c: e5a12004     	str	r2, [r1, #0x4]!
     130: e5a02004     	str	r2, [r0, #0x4]!
     134: 02833001     	addeq	r3, r3, #1
     138: 01e580b2     	strheq	r8, [r5, #2]!
     13c: 01ee80b2     	strheq	r8, [lr, #2]!
     140: 0afffff7     	beq	0x124 <zz_raster_selftest+0x124> @ imm = #-0x24
     144: e3590001     	cmp	r9, #1
     148: e2833001     	add	r3, r3, #1
     14c: e3032039     	movw	r2, #0x3039
     150: 03a02000     	moveq	r2, #0
     154: e3530030     	cmp	r3, #48
     158: e1e520b2     	strh	r2, [r5, #2]!
     15c: e1ee20b2     	strh	r2, [lr, #2]!
     160: 1affffef     	bne	0x124 <zz_raster_selftest+0x124> @ imm = #-0x44
     164: e3022bcf     	movw	r2, #0x2bcf
     168: edddbb08     	vldr	d27, [sp, #32]
     16c: e34f2fff     	movt	r2, #0xffff
     170: e28d3014     	add	r3, sp, #20
     174: edddab06     	vldr	d26, [sp, #24]
     178: e59d5080     	ldr	r5, [sp, #0x80]
     17c: ee0b2b90     	vmov.32	d27[0], r2
     180: e3022aff     	movw	r2, #0x2aff
     184: e3402001     	movt	r2, #0x1
     188: edddcb0a     	vldr	d28, [sp, #40]
     18c: f2c0807f     	vmvn.i32	q12, #0xf
     190: f4e3a80f     	vld1.32	{d26[0]}, [r3]
     194: e59f327c     	ldr	r3, [pc, #0x27c]        @ 0x418 <zz_raster_selftest+0x418>
     198: ee0c2b90     	vmov.32	d28[0], r2
     19c: e30729d1     	movw	r2, #0x79d1
     1a0: e34f2ffe     	movt	r2, #0xfffe
     1a4: eddddb0c     	vldr	d29, [sp, #48]
     1a8: e08f3003     	add	r3, pc, r3
     1ac: f22a71ba     	vorr	d7, d26, d26
     1b0: ee0d2b90     	vmov.32	d29[0], r2
     1b4: e893000f     	ldm	r3, {r0, r1, r2, r3}
     1b8: f3c76c7f     	vmvn.i32	q11, #0xffff
     1bc: f3c04670     	vmvn.i32	q10, #0x80000000
     1c0: e885000f     	stm	r5, {r0, r1, r2, r3}
     1c4: f4650a9f     	vld1.32	{d16, d17}, [r5:64]
     1c8: f3e080c7     	vmla.i32	q12, q8, d7[0]
     1cc: e59de008     	ldr	lr, [sp, #0x8]
     1d0: edcdab06     	vstr	d26, [sp, #24]
     1d4: f22b71bb     	vorr	d7, d27, d27
     1d8: e35e0003     	cmp	lr, #3
     1dc: f2c12d52     	vmov.i32	q9, #0x12ffff
     1e0: edcdbb08     	vstr	d27, [sp, #32]
     1e4: f3e060c7     	vmla.i32	q11, q8, d7[0]
     1e8: edcdcb0a     	vstr	d28, [sp, #40]
     1ec: f22c71bc     	vorr	d7, d28, d28
     1f0: edcddb0c     	vstr	d29, [sp, #48]
     1f4: f3e040c7     	vmla.i32	q10, q8, d7[0]
     1f8: f22d71bd     	vorr	d7, d29, d29
     1fc: f3e020c7     	vmla.i32	q9, q8, d7[0]
     200: 9a00006f     	bls	0x3c4 <zz_raster_selftest+0x3c4> @ imm = #0x1bc
     204: e59d3004     	ldr	r3, [sp, #0x4]
     208: e3a00004     	mov	r0, #4
     20c: e59d2078     	ldr	r2, [sp, #0x78]
     210: ed9f6b78     	vldr	d6, [pc, #480]          @ 0x3f8 <zz_raster_selftest+0x3f8>
     214: ed9f7b79     	vldr	d7, [pc, #484]          @ 0x400 <zz_raster_selftest+0x400>
     218: e2038001     	and	r8, r3, #1
     21c: f387025f     	vmov.i32	q0, #0xff00
     220: e1a050a3     	lsr	r5, r3, #1
     224: f387205f     	vmov.i32	q1, #0xff
     228: e59d307c     	ldr	r3, [sp, #0x7c]
     22c: f387445f     	vmov.i32	q2, #0xff0000
     230: eddfeb74     	vldr	d30, [pc, #464]         @ 0x408 <zz_raster_selftest+0x408>
     234: eddffb75     	vldr	d31, [pc, #468]         @ 0x410 <zz_raster_selftest+0x410>
     238: e58d5000     	str	r5, [sp]
     23c: ea000015     	b	0x298 <zz_raster_selftest+0x298> @ imm = #0x54
     240: f462a74f     	vld1.16	{d26}, [r2]
     244: e3550000     	cmp	r5, #0
     248: f3f6b20c     	vmovn.i32	d27, q6
     24c: f3d0ca3a     	vmovl.u16	q14, d26
     250: f423ea8f     	vld1.32	{d14, d15}, [r3]
     254: f36cc37c     	vcge.u32	q14, q6, q14
     258: f320e1fc     	vbit	q7, q8, q14
     25c: f3f6c22c     	vmovn.i32	d28, q14
     260: f403ea8f     	vst1.32	{d14, d15}, [r3]
     264: f35bc1ba     	vbsl	d28, d27, d26
     268: 0a000000     	beq	0x270 <zz_raster_selftest+0x270> @ imm = #0x0
     26c: f442c74f     	vst1.16	{d28}, [r2]
     270: e2801004     	add	r1, r0, #4
     274: f26888ca     	vadd.i32	q12, q12, q5
     278: e15e0001     	cmp	lr, r1
     27c: f26668c8     	vadd.i32	q11, q11, q4
     280: f26448c6     	vadd.i32	q10, q10, q3
     284: e2822008     	add	r2, r2, #8
     288: f26228ee     	vadd.i32	q9, q9, q15
     28c: e2833010     	add	r3, r3, #16
     290: 3a000010     	blo	0x2d8 <zz_raster_selftest+0x2d8> @ imm = #0x40
     294: e1a00001     	mov	r0, r1
     298: f3f80074     	vshr.u32	q8, q10, #0x8
     29c: e3580000     	cmp	r8, #0
     2a0: f3f0c072     	vshr.u32	q14, q9, #0x10
     2a4: f24001d0     	vand	q8, q8, q0
     2a8: f24cc1d2     	vand	q14, q14, q1
     2ac: f246a1d4     	vand	q13, q11, q2
     2b0: f26001fc     	vorr	q8, q8, q14
     2b4: f3b2c078     	vshr.u32	q6, q12, #0xe
     2b8: f26001fa     	vorr	q8, q8, q13
     2bc: 1affffdf     	bne	0x240 <zz_raster_selftest+0x240> @ imm = #-0x84
     2c0: e3550000     	cmp	r5, #0
     2c4: f3b6c20c     	vmovn.i32	d12, q6
     2c8: f4430a8f     	vst1.32	{d16, d17}, [r3]
     2cc: 0affffe7     	beq	0x270 <zz_raster_selftest+0x270> @ imm = #-0x64
     2d0: f402c74f     	vst1.16	{d12}, [r2]
     2d4: eaffffe5     	b	0x270 <zz_raster_selftest+0x270> @ imm = #-0x6c
     2d8: e59d3008     	ldr	r3, [sp, #0x8]
     2dc: e30f1fff     	movw	r1, #0xffff
     2e0: e3401012     	movt	r1, #0x12
     2e4: e58d6048     	str	r6, [sp, #0x48]
     2e8: e58da054     	str	r10, [sp, #0x54]
     2ec: e3e0e102     	mvn	lr, #-2147483648
     2f0: e3c33003     	bic	r3, r3, #3
     2f4: e59d9074     	ldr	r9, [sp, #0x74]
     2f8: e1530000     	cmp	r3, r0
     2fc: e30739d1     	movw	r3, #0x79d1
     300: e34f3ffe     	movt	r3, #0xfffe
     304: e59d5070     	ldr	r5, [sp, #0x70]
     308: e0221093     	mla	r2, r3, r0, r1
     30c: e59d6014     	ldr	r6, [sp, #0x14]
     310: e58d7050     	str	r7, [sp, #0x50]
     314: e3020aff     	movw	r0, #0x2aff
     318: e58d4044     	str	r4, [sp, #0x44]
     31c: e3400001     	movt	r0, #0x1
     320: e1a07003     	mov	r7, r3
     324: e58d0040     	str	r0, [sp, #0x40]
     328: e3e0000f     	mvn	r0, #15
     32c: e59da040     	ldr	r10, [sp, #0x40]
     330: 128bb001     	addne	r11, r11, #1
     334: e58d203c     	str	r2, [sp, #0x3c]
     338: e3a02000     	mov	r2, #0
     33c: e34f2fff     	movt	r2, #0xffff
     340: e59d403c     	ldr	r4, [sp, #0x3c]
     344: e58dc04c     	str	r12, [sp, #0x4c]
     348: e58db03c     	str	r11, [sp, #0x3c]
     34c: e1a0342e     	lsr	r3, lr, #8
     350: e3580000     	cmp	r8, #0
     354: e7e7b851     	ubfx	r11, r1, #0x10, #0x8
     358: e1a0c720     	lsr	r12, r0, #14
     35c: e2033cff     	and	r3, r3, #65280
     360: e0811007     	add	r1, r1, r7
     364: e183300b     	orr	r3, r3, r11
     368: 0a000002     	beq	0x378 <zz_raster_selftest+0x378> @ imm = #0x8
     36c: e1d5b0b0     	ldrh	r11, [r5]
     370: e15b000c     	cmp	r11, r12
     374: 8a000005     	bhi	0x390 <zz_raster_selftest+0x390> @ imm = #0x14
     378: e59db000     	ldr	r11, [sp]
     37c: e35b0000     	cmp	r11, #0
     380: e202b8ff     	and	r11, r2, #16711680
     384: e183300b     	orr	r3, r3, r11
     388: 11c5c0b0     	strhne	r12, [r5]
     38c: e5893000     	str	r3, [r9]
     390: e1510004     	cmp	r1, r4
     394: e2422b35     	sub	r2, r2, #54272
     398: e0800006     	add	r0, r0, r6
     39c: e2422031     	sub	r2, r2, #49
     3a0: e08ee00a     	add	lr, lr, r10
     3a4: e2855002     	add	r5, r5, #2
     3a8: e2899004     	add	r9, r9, #4
     3ac: 1affffe6     	bne	0x34c <zz_raster_selftest+0x34c> @ imm = #-0x68
     3b0: e28d4044     	add	r4, sp, #68
     3b4: e59db03c     	ldr	r11, [sp, #0x3c]
     3b8: e8941050     	ldm	r4, {r4, r6, r12}
     3bc: e59d7050     	ldr	r7, [sp, #0x50]
     3c0: e59da054     	ldr	r10, [sp, #0x54]
     3c4: e59d1010     	ldr	r1, [sp, #0x10]
     3c8: e59d200c     	ldr	r2, [sp, #0xc]
     3cc: e59d3038     	ldr	r3, [sp, #0x38]
     3d0: ea000016     	b	0x430 <zz_raster_selftest+0x430> @ imm = #0x58
     3d4: e320f000     	nop
     3d8: 9c 15 8d 04  	.word	0x048d159c
     3dc: 9c 15 8d 04  	.word	0x048d159c
     3e0: 9c 15 8d 04  	.word	0x048d159c
     3e4: 9c 15 8d 04  	.word	0x048d159c
     3e8: 3c af fc ff  	.word	0xfffcaf3c
     3ec: 3c af fc ff  	.word	0xfffcaf3c
     3f0: 3c af fc ff  	.word	0xfffcaf3c
     3f4: 3c af fc ff  	.word	0xfffcaf3c
     3f8: fc ab 04 00  	.word	0x0004abfc
     3fc: fc ab 04 00  	.word	0x0004abfc
     400: fc ab 04 00  	.word	0x0004abfc
     404: fc ab 04 00  	.word	0x0004abfc
     408: 44 e7 f9 ff  	.word	0xfff9e744
     40c: 44 e7 f9 ff  	.word	0xfff9e744
     410: 44 e7 f9 ff  	.word	0xfff9e744
     414: 44 e7 f9 ff  	.word	0xfff9e744
     418: 68 02 00 00  	.word	0x00000268
     41c: e153000a     	cmp	r3, r10
     420: e28bb001     	add	r11, r11, #1
     424: e2822002     	add	r2, r2, #2
     428: e2811002     	add	r1, r1, #2
     42c: 0a00000b     	beq	0x460 <zz_raster_selftest+0x460> @ imm = #0x2c
     430: e5b3e004     	ldr	lr, [r3, #0x4]!
     434: e5bc0004     	ldr	r0, [r12, #0x4]!
     438: e15e0000     	cmp	lr, r0
     43c: 1afffff6     	bne	0x41c <zz_raster_selftest+0x41c> @ imm = #-0x28
     440: e1d2e0b0     	ldrh	lr, [r2]
     444: e1d100b0     	ldrh	r0, [r1]
     448: e15e0000     	cmp	lr, r0
     44c: 1afffff2     	bne	0x41c <zz_raster_selftest+0x41c> @ imm = #-0x38
     450: e153000a     	cmp	r3, r10
     454: e2822002     	add	r2, r2, #2
     458: e2811002     	add	r1, r1, #2
     45c: 1afffff3     	bne	0x430 <zz_raster_selftest+0x430> @ imm = #-0x34
     460: e59d5008     	ldr	r5, [sp, #0x8]
     464: e59d8088     	ldr	r8, [sp, #0x88]
     468: e59d0058     	ldr	r0, [sp, #0x58]
     46c: e1a02005     	mov	r2, r5
     470: e1a01008     	mov	r1, r8
     474: ebfffffe     	bl	0x474 <zz_raster_selftest+0x474> @ imm = #-0x8
     478: e1a02005     	mov	r2, r5
     47c: e59d005c     	ldr	r0, [sp, #0x5c]
     480: e3051678     	movw	r1, #0x5678
     484: e3055678     	movw	r5, #0x5678
     488: ebfffffe     	bl	0x488 <zz_raster_selftest+0x488> @ imm = #-0x8
     48c: e59d000c     	ldr	r0, [sp, #0xc]
     490: e3a03000     	mov	r3, #0
     494: e59d1010     	ldr	r1, [sp, #0x10]
     498: e59d2064     	ldr	r2, [sp, #0x64]
     49c: e59d9060     	ldr	r9, [sp, #0x60]
     4a0: ea000004     	b	0x4b8 <zz_raster_selftest+0x4b8> @ imm = #0x10
     4a4: e3530030     	cmp	r3, #48
     4a8: e28bb001     	add	r11, r11, #1
     4ac: e2811002     	add	r1, r1, #2
     4b0: e2800002     	add	r0, r0, #2
     4b4: 0a000014     	beq	0x50c <zz_raster_selftest+0x50c> @ imm = #0x50
     4b8: e1560003     	cmp	r6, r3
     4bc: 8a000004     	bhi	0x4d4 <zz_raster_selftest+0x4d4> @ imm = #0x10
     4c0: e1530009     	cmp	r3, r9
     4c4: e1a0c008     	mov	r12, r8
     4c8: 31c150b0     	strhlo	r5, [r1]
     4cc: 35828000     	strlo	r8, [r2]
     4d0: 3a000000     	blo	0x4d8 <zz_raster_selftest+0x4d8> @ imm = #0x0
     4d4: e592c000     	ldr	r12, [r2]
     4d8: e5b4e004     	ldr	lr, [r4, #0x4]!
     4dc: e2833001     	add	r3, r3, #1
     4e0: e2822004     	add	r2, r2, #4
     4e4: e15e000c     	cmp	lr, r12
     4e8: 1affffed     	bne	0x4a4 <zz_raster_selftest+0x4a4> @ imm = #-0x4c
     4ec: e1d0e0b0     	ldrh	lr, [r0]
     4f0: e1d1c0b0     	ldrh	r12, [r1]
     4f4: e15e000c     	cmp	lr, r12
     4f8: 1affffe9     	bne	0x4a4 <zz_raster_selftest+0x4a4> @ imm = #-0x5c
     4fc: e3530030     	cmp	r3, #48
     500: e2811002     	add	r1, r1, #2
     504: e2800002     	add	r0, r0, #2
     508: 1affffea     	bne	0x4b8 <zz_raster_selftest+0x4b8> @ imm = #-0x58
     50c: e59d3004     	ldr	r3, [sp, #0x4]
     510: e2833001     	add	r3, r3, #1
     514: e3530004     	cmp	r3, #4
     518: e58d3004     	str	r3, [sp, #0x4]
     51c: 1afffef8     	bne	0x104 <zz_raster_selftest+0x104> @ imm = #-0x420
     520: e59d3058     	ldr	r3, [sp, #0x58]
     524: e2868001     	add	r8, r6, #1
     528: e3580004     	cmp	r8, #4
     52c: e1a0600a     	mov	r6, r10
     530: e1a0a007     	mov	r10, r7
     534: e2833004     	add	r3, r3, #4
     538: e58d3058     	str	r3, [sp, #0x58]
     53c: e59d305c     	ldr	r3, [sp, #0x5c]
     540: e2833002     	add	r3, r3, #2
     544: e58d305c     	str	r3, [sp, #0x5c]
     548: e59d3060     	ldr	r3, [sp, #0x60]
     54c: e2833001     	add	r3, r3, #1
     550: e58d3060     	str	r3, [sp, #0x60]
     554: 1afffece     	bne	0x94 <zz_raster_selftest+0x94> @ imm = #-0x4c8
     558: e59d3008     	ldr	r3, [sp, #0x8]
     55c: e1a08007     	mov	r8, r7
     560: e59d408c     	ldr	r4, [sp, #0x8c]
     564: e2833001     	add	r3, r3, #1
     568: e3530029     	cmp	r3, #41
     56c: e58d3008     	str	r3, [sp, #0x8]
     570: 1afffeba     	bne	0x60 <zz_raster_selftest+0x60> @ imm = #-0x518
     574: e1a0000b     	mov	r0, r11
     578: e28ddfb9     	add	sp, sp, #740
     57c: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     580: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
