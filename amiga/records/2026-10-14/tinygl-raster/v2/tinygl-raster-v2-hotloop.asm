<ZB_fillTriangleSmoothNOBLEND_DT1_DW1>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e1a07003     	mov	r7, r3
       8: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
       c: e24ddf59     	sub	sp, sp, #356
      10: e5923004     	ldr	r3, [r2, #0x4]
      14: e58d00bc     	str	r0, [sp, #0xbc]
      18: e5910004     	ldr	r0, [r1, #0x4]
      1c: e597e004     	ldr	lr, [r7, #0x4]
      20: e59fcc20     	ldr	r12, [pc, #0xc20]       @ 0xc48 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xc48>
      24: e1530000     	cmp	r3, r0
      28: a1a04000     	movge	r4, r0
      2c: b1a04003     	movlt	r4, r3
      30: a1a00003     	movge	r0, r3
      34: b1a06002     	movlt	r6, r2
      38: a1a06001     	movge	r6, r1
      3c: b1a09001     	movlt	r9, r1
      40: a1a09002     	movge	r9, r2
      44: e15e0004     	cmp	lr, r4
      48: e08fc00c     	add	r12, pc, r12
      4c: e1a03004     	mov	r3, r4
      50: ba00000b     	blt	0x84 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x84> @ imm = #0x2c
      54: e15e0000     	cmp	lr, r0
      58: e1a02007     	mov	r2, r7
      5c: a1a03000     	movge	r3, r0
      60: a1a07006     	movge	r7, r6
      64: a1a0000e     	movge	r0, lr
      68: a1a06009     	movge	r6, r9
      6c: b1a0300e     	movlt	r3, lr
      70: b1a07006     	movlt	r7, r6
      74: a1a0e004     	movge	lr, r4
      78: a1a09002     	movge	r9, r2
      7c: b1a0e004     	movlt	lr, r4
      80: b1a06002     	movlt	r6, r2
      84: e040200e     	sub	r2, r0, lr
      88: e5971000     	ldr	r1, [r7]
      8c: e5975008     	ldr	r5, [r7, #0x8]
      90: ee072a10     	vmov	s14, r2
      94: e5992000     	ldr	r2, [r9]
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
     178: edcd6a51     	vstr	s13, [sp, #324]
     17c: ee062a90     	vmov	s13, r2
     180: e5962018     	ldr	r2, [r6, #0x18]
     184: eef86ae6     	vcvt.f32.s32	s13, s13
     188: e0422000     	sub	r2, r2, r0
     18c: ee573a26     	vnmls.f32	s7, s14, s13
     190: ee676aa6     	vmul.f32	s13, s15, s13
     194: ee556a83     	vnmls.f32	s13, s11, s6
     198: eefd9ae6     	vcvt.s32.f32	s19, s13
     19c: ee062a90     	vmov	s13, r2
     1a0: e596201c     	ldr	r2, [r6, #0x1c]
     1a4: eef86ae6     	vcvt.f32.s32	s13, s13
     1a8: e0422001     	sub	r2, r2, r1
     1ac: e59d10bc     	ldr	r1, [sp, #0xbc]
     1b0: ee572a26     	vnmls.f32	s5, s14, s13
     1b4: ee676aa6     	vmul.f32	s13, s15, s13
     1b8: ee556a84     	vnmls.f32	s13, s11, s8
     1bc: eebd9ae6     	vcvt.s32.f32	s18, s13
     1c0: ee062a90     	vmov	s13, r2
     1c4: eeb84ae6     	vcvt.f32.s32	s8, s13
     1c8: e5912020     	ldr	r2, [r1, #0x20]
     1cc: eef06a46     	vmov.f32	s13, s12
     1d0: ee677a84     	vmul.f32	s15, s15, s8
     1d4: ee576a04     	vnmls.f32	s13, s14, s8
     1d8: e00e0e92     	mul	lr, r2, lr
     1dc: e5912004     	ldr	r2, [r1, #0x4]
     1e0: ee557a85     	vnmls.f32	s15, s11, s10
     1e4: e082210e     	add	r2, r2, lr, lsl #2
     1e8: e58d208c     	str	r2, [sp, #0x8c]
     1ec: e5912000     	ldr	r2, [r1]
     1f0: eefd8ae7     	vcvt.s32.f32	s17, s15
     1f4: eefd7ae3     	vcvt.s32.f32	s15, s7
     1f8: e082208e     	add	r2, r2, lr, lsl #1
     1fc: e58d2088     	str	r2, [sp, #0x88]
     200: edcd7a0f     	vstr	s15, [sp, #60]
     204: eefd7ae2     	vcvt.s32.f32	s15, s5
     208: edcd7a19     	vstr	s15, [sp, #100]
     20c: eefd7ae6     	vcvt.s32.f32	s15, s13
     210: edcd7a1a     	vstr	s15, [sp, #104]
     214: eefd7ae4     	vcvt.s32.f32	s15, s9
     218: ee175a90     	vmov	r5, s15
     21c: e1a02105     	lsl	r2, r5, #2
     220: e1a0a085     	lsl	r10, r5, #1
     224: e58d2090     	str	r2, [sp, #0x90]
     228: eeaa2b10     	vdup.32	q5, r2
     22c: e59d203c     	ldr	r2, [sp, #0x3c]
     230: e1a01102     	lsl	r1, r2, #2
     234: e1a04082     	lsl	r4, r2, #1
     238: e58d1010     	str	r1, [sp, #0x10]
     23c: e0842002     	add	r2, r4, r2
     240: eeac1b10     	vdup.32	q6, r1
     244: e59d1064     	ldr	r1, [sp, #0x64]
     248: e1a00101     	lsl	r0, r1, #2
     24c: e58d0014     	str	r0, [sp, #0x14]
     250: eeae0b10     	vdup.32	q7, r0
     254: e59d0068     	ldr	r0, [sp, #0x68]
     258: e1a0e100     	lsl	lr, r0, #2
     25c: e1a08080     	lsl	r8, r0, #1
     260: e58de028     	str	lr, [sp, #0x28]
     264: eea4eb10     	vdup.32	q2, lr
     268: e1a0e081     	lsl	lr, r1, #1
     26c: e58de0c8     	str	lr, [sp, #0xc8]
     270: e08e1001     	add	r1, lr, r1
     274: e58d20d8     	str	r2, [sp, #0xd8]
     278: e0882000     	add	r2, r8, r0
     27c: e58d40cc     	str	r4, [sp, #0xcc]
     280: e58d80c4     	str	r8, [sp, #0xc4]
     284: e58da094     	str	r10, [sp, #0x94]
     288: e58d10d4     	str	r1, [sp, #0xd4]
     28c: e58d20d0     	str	r2, [sp, #0xd0]
     290: da000250     	ble	0xbd8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xbd8> @ imm = #0x940
     294: e5991004     	ldr	r1, [r9, #0x4]
     298: e1a02009     	mov	r2, r9
     29c: e58d6148     	str	r6, [sp, #0x148]
     2a0: e5970004     	ldr	r0, [r7, #0x4]
     2a4: e3a0b000     	mov	r11, #0
     2a8: e1a0800b     	mov	r8, r11
     2ac: f387225f     	vmov.i32	q1, #0xff00
     2b0: e1a04007     	mov	r4, r7
     2b4: e58db0b4     	str	r11, [sp, #0xb4]
     2b8: e043a000     	sub	r10, r3, r0
     2bc: e0411000     	sub	r1, r1, r0
     2c0: e59f0984     	ldr	r0, [pc, #0x984]        @ 0xc4c <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xc4c>
     2c4: e3a03001     	mov	r3, #1
     2c8: e1a0b005     	mov	r11, r5
     2cc: e58d714c     	str	r7, [sp, #0x14c]
     2d0: e1a05008     	mov	r5, r8
     2d4: e1a07006     	mov	r7, r6
     2d8: e1a06008     	mov	r6, r8
     2dc: e08f0000     	add	r0, pc, r0
     2e0: e58d3060     	str	r3, [sp, #0x60]
     2e4: e28d3e15     	add	r3, sp, #336
     2e8: e58d0138     	str	r0, [sp, #0x138]
     2ec: e58d313c     	str	r3, [sp, #0x13c]
     2f0: e58dc140     	str	r12, [sp, #0x140]
     2f4: e5940000     	ldr	r0, [r4]
     2f8: e3510000     	cmp	r1, #0
     2fc: e58d0074     	str	r0, [sp, #0x74]
     300: da00021c     	ble	0xb78 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xb78> @ imm = #0x870
     304: e28d3018     	add	r3, sp, #24
     308: f40d2adf     	vst1.64	{d2, d3}, [sp:64]
     30c: f4034adf     	vst1.64	{d4, d5}, [r3:64]
     310: e5923000     	ldr	r3, [r2]
     314: e0433000     	sub	r3, r3, r0
     318: e1a00803     	lsl	r0, r3, #16
     31c: ebfffffe     	bl	0x31c <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x31c> @ imm = #-0x8
     320: e6ff2070     	uxth	r2, r0
     324: e1a03840     	asr	r3, r0, #16
     328: e59d103c     	ldr	r1, [sp, #0x3c]
     32c: e58d20c0     	str	r2, [sp, #0xc0]
     330: e59d2144     	ldr	r2, [sp, #0x144]
     334: e58d3120     	str	r3, [sp, #0x120]
     338: f42d2adf     	vld1.64	{d2, d3}, [sp:64]
     33c: e0222b93     	mla	r2, r3, r11, r2
     340: e58d20f0     	str	r2, [sp, #0xf0]
     344: ee192a90     	vmov	r2, s19
     348: e0222193     	mla	r2, r3, r1, r2
     34c: e59d1064     	ldr	r1, [sp, #0x64]
     350: e58d20f4     	str	r2, [sp, #0xf4]
     354: ee192a10     	vmov	r2, s18
     358: e0222193     	mla	r2, r3, r1, r2
     35c: e59d1068     	ldr	r1, [sp, #0x68]
     360: e58d20f8     	str	r2, [sp, #0xf8]
     364: ee182a90     	vmov	r2, s17
     368: e0222193     	mla	r2, r3, r1, r2
     36c: e2833001     	add	r3, r3, #1
     370: e58d30ec     	str	r3, [sp, #0xec]
     374: e28d3018     	add	r3, sp, #24
     378: e58d20fc     	str	r2, [sp, #0xfc]
     37c: f4234adf     	vld1.64	{d4, d5}, [r3:64]
     380: e59d3060     	ldr	r3, [sp, #0x60]
     384: e3530000     	cmp	r3, #0
     388: e5943008     	ldr	r3, [r4, #0x8]
     38c: e58d3078     	str	r3, [sp, #0x78]
     390: e5943014     	ldr	r3, [r4, #0x14]
     394: e58d307c     	str	r3, [sp, #0x7c]
     398: e5943018     	ldr	r3, [r4, #0x18]
     39c: e58d3080     	str	r3, [sp, #0x80]
     3a0: e594301c     	ldr	r3, [r4, #0x1c]
     3a4: e58d3084     	str	r3, [sp, #0x84]
     3a8: 1a00020e     	bne	0xbe8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xbe8> @ imm = #0x838
     3ac: e35a0000     	cmp	r10, #0
     3b0: da00012e     	ble	0x870 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x870> @ imm = #0x4b8
     3b4: e59d2140     	ldr	r2, [sp, #0x140]
     3b8: e59f3890     	ldr	r3, [pc, #0x890]        @ 0xc50 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xc50>
     3bc: e7923003     	ldr	r3, [r2, r3]
     3c0: e59d20f4     	ldr	r2, [sp, #0xf4]
     3c4: e58da06c     	str	r10, [sp, #0x6c]
     3c8: e58d30b8     	str	r3, [sp, #0xb8]
     3cc: e59d30f0     	ldr	r3, [sp, #0xf0]
     3d0: e58da124     	str	r10, [sp, #0x124]
     3d4: e1a0a00b     	mov	r10, r11
     3d8: e58d6070     	str	r6, [sp, #0x70]
     3dc: e08b3003     	add	r3, r11, r3
     3e0: e58d5128     	str	r5, [sp, #0x128]
     3e4: e58d30dc     	str	r3, [sp, #0xdc]
     3e8: e59d303c     	ldr	r3, [sp, #0x3c]
     3ec: e58d612c     	str	r6, [sp, #0x12c]
     3f0: e58d9130     	str	r9, [sp, #0x130]
     3f4: e0833002     	add	r3, r3, r2
     3f8: e59d20f8     	ldr	r2, [sp, #0xf8]
     3fc: e58d30e0     	str	r3, [sp, #0xe0]
     400: e59d3064     	ldr	r3, [sp, #0x64]
     404: e58d7134     	str	r7, [sp, #0x134]
     408: e0833002     	add	r3, r3, r2
     40c: e59d20fc     	ldr	r2, [sp, #0xfc]
     410: e58d30e4     	str	r3, [sp, #0xe4]
     414: e59d3068     	ldr	r3, [sp, #0x68]
     418: e0833002     	add	r3, r3, r2
     41c: e58d30e8     	str	r3, [sp, #0xe8]
     420: e59d306c     	ldr	r3, [sp, #0x6c]
     424: e59d2074     	ldr	r2, [sp, #0x74]
     428: e59d1070     	ldr	r1, [sp, #0x70]
     42c: e2433001     	sub	r3, r3, #1
     430: e59dc078     	ldr	r12, [sp, #0x78]
     434: e58d306c     	str	r3, [sp, #0x6c]
     438: e59d30b8     	ldr	r3, [sp, #0xb8]
     43c: e0621841     	rsb	r1, r2, r1, asr #16
     440: e5933000     	ldr	r3, [r3]
     444: e58d1048     	str	r1, [sp, #0x48]
     448: e3530000     	cmp	r3, #0
     44c: 1351001e     	cmpne	r1, #30
     450: e59d308c     	ldr	r3, [sp, #0x8c]
     454: e0834102     	add	r4, r3, r2, lsl #2
     458: e59d3088     	ldr	r3, [sp, #0x88]
     45c: e0835082     	add	r5, r3, r2, lsl #1
     460: ca00011c     	bgt	0x8d8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x8d8> @ imm = #0x470
     464: e28d307c     	add	r3, sp, #124
     468: e8930908     	ldm	r3, {r3, r8, r11}
     46c: e58d3038     	str	r3, [sp, #0x38]
     470: e59d3048     	ldr	r3, [sp, #0x48]
     474: e3530002     	cmp	r3, #2
     478: da0000ad     	ble	0x734 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x734> @ imm = #0x2b4
     47c: e2433003     	sub	r3, r3, #3
     480: e2859008     	add	r9, r5, #8
     484: e1a02123     	lsr	r2, r3, #2
     488: e59d10cc     	ldr	r1, [sp, #0xcc]
     48c: e2853010     	add	r3, r5, #16
     490: e59d5038     	ldr	r5, [sp, #0x38]
     494: e1a00182     	lsl	r0, r2, #3
     498: e58d8034     	str	r8, [sp, #0x34]
     49c: e2846010     	add	r6, r4, #16
     4a0: e58d909c     	str	r9, [sp, #0x9c]
     4a4: e0851001     	add	r1, r5, r1
     4a8: e0833000     	add	r3, r3, r0
     4ac: e58d3030     	str	r3, [sp, #0x30]
     4b0: e3a04000     	mov	r4, #0
     4b4: e59d3094     	ldr	r3, [sp, #0x94]
     4b8: e58d1050     	str	r1, [sp, #0x50]
     4bc: e59d10c4     	ldr	r1, [sp, #0xc4]
     4c0: e08ce003     	add	lr, r12, r3
     4c4: e59d30c8     	ldr	r3, [sp, #0xc8]
     4c8: e1a0700e     	mov	r7, lr
     4cc: e58d6098     	str	r6, [sp, #0x98]
     4d0: e08b1001     	add	r1, r11, r1
     4d4: e58d40a0     	str	r4, [sp, #0xa0]
     4d8: e58d104c     	str	r1, [sp, #0x4c]
     4dc: e0883003     	add	r3, r8, r3
     4e0: e59d10d4     	ldr	r1, [sp, #0xd4]
     4e4: e58d3018     	str	r3, [sp, #0x18]
     4e8: e59d3064     	ldr	r3, [sp, #0x64]
     4ec: e0881001     	add	r1, r8, r1
     4f0: e58db05c     	str	r11, [sp, #0x5c]
     4f4: e58d102c     	str	r1, [sp, #0x2c]
     4f8: e59d10d8     	ldr	r1, [sp, #0xd8]
     4fc: e0883003     	add	r3, r8, r3
     500: e58d3000     	str	r3, [sp]
     504: e1a03005     	mov	r3, r5
     508: e59d8090     	ldr	r8, [sp, #0x90]
     50c: e0851001     	add	r1, r5, r1
     510: e59d503c     	ldr	r5, [sp, #0x3c]
     514: e58d1058     	str	r1, [sp, #0x58]
     518: e59d10d0     	ldr	r1, [sp, #0xd0]
     51c: e0833005     	add	r3, r3, r5
     520: e1a05004     	mov	r5, r4
     524: e58d3040     	str	r3, [sp, #0x40]
     528: e59d3068     	ldr	r3, [sp, #0x68]
     52c: e08b1001     	add	r1, r11, r1
     530: e58d1054     	str	r1, [sp, #0x54]
     534: e1a01004     	mov	r1, r4
     538: e58d00a4     	str	r0, [sp, #0xa4]
     53c: e08b3003     	add	r3, r11, r3
     540: e58d20a8     	str	r2, [sp, #0xa8]
     544: e58d3044     	str	r3, [sp, #0x44]
     548: e1a03009     	mov	r3, r9
     54c: e58de0ac     	str	lr, [sp, #0xac]
     550: e58d90b0     	str	r9, [sp, #0xb0]
     554: e59d2034     	ldr	r2, [sp, #0x34]
     558: e1a0072c     	lsr	r0, r12, #14
     55c: e153e0b8     	ldrh	lr, [r3, #-8]
     560: e08a900c     	add	r9, r10, r12
     564: e1a09729     	lsr	r9, r9, #14
     568: e08cc008     	add	r12, r12, r8
     56c: e0822001     	add	r2, r2, r1
     570: e1a02442     	asr	r2, r2, #8
     574: e15e0000     	cmp	lr, r0
     578: e2022cff     	and	r2, r2, #65280
     57c: 8a000009     	bhi	0x5a8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x5a8> @ imm = #0x24
     580: e59de05c     	ldr	lr, [sp, #0x5c]
     584: e08ee004     	add	lr, lr, r4
     588: e7e7e85e     	ubfx	lr, lr, #0x10, #0x8
     58c: e182200e     	orr	r2, r2, lr
     590: e59de038     	ldr	lr, [sp, #0x38]
     594: e08ee005     	add	lr, lr, r5
     598: e20ee8ff     	and	lr, lr, #16711680
     59c: e182200e     	orr	r2, r2, lr
     5a0: e5062010     	str	r2, [r6, #-0x10]
     5a4: e14300b8     	strh	r0, [r3, #-8]
     5a8: e59d0018     	ldr	r0, [sp, #0x18]
     5ac: e59db014     	ldr	r11, [sp, #0x14]
     5b0: e59d2000     	ldr	r2, [sp]
     5b4: e080e001     	add	lr, r0, r1
     5b8: e59d002c     	ldr	r0, [sp, #0x2c]
     5bc: e1a0e44e     	asr	lr, lr, #8
     5c0: e0822001     	add	r2, r2, r1
     5c4: e0800001     	add	r0, r0, r1
     5c8: e081100b     	add	r1, r1, r11
     5cc: e153b0b6     	ldrh	r11, [r3, #-6]
     5d0: e1a02442     	asr	r2, r2, #8
     5d4: e1a00440     	asr	r0, r0, #8
     5d8: e20eecff     	and	lr, lr, #65280
     5dc: e2022cff     	and	r2, r2, #65280
     5e0: e15b0009     	cmp	r11, r9
     5e4: e2000cff     	and	r0, r0, #65280
     5e8: 8a000009     	bhi	0x614 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x614> @ imm = #0x24
     5ec: e59db044     	ldr	r11, [sp, #0x44]
     5f0: e08bb004     	add	r11, r11, r4
     5f4: e7e7b85b     	ubfx	r11, r11, #0x10, #0x8
     5f8: e182200b     	orr	r2, r2, r11
     5fc: e59db040     	ldr	r11, [sp, #0x40]
     600: e08bb005     	add	r11, r11, r5
     604: e20bb8ff     	and	r11, r11, #16711680
     608: e182200b     	orr	r2, r2, r11
     60c: e506200c     	str	r2, [r6, #-0xc]
     610: e14390b6     	strh	r9, [r3, #-6]
     614: e153b0b4     	ldrh	r11, [r3, #-4]
     618: e1a09727     	lsr	r9, r7, #14
     61c: e08a2007     	add	r2, r10, r7
     620: e1a02722     	lsr	r2, r2, #14
     624: e0877008     	add	r7, r7, r8
     628: e15b0009     	cmp	r11, r9
     62c: 8a000009     	bhi	0x658 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x658> @ imm = #0x24
     630: e59db04c     	ldr	r11, [sp, #0x4c]
     634: e08bb004     	add	r11, r11, r4
     638: e7e7b85b     	ubfx	r11, r11, #0x10, #0x8
     63c: e18ee00b     	orr	lr, lr, r11
     640: e59db050     	ldr	r11, [sp, #0x50]
     644: e08bb005     	add	r11, r11, r5
     648: e20bb8ff     	and	r11, r11, #16711680
     64c: e18ee00b     	orr	lr, lr, r11
     650: e506e008     	str	lr, [r6, #-0x8]
     654: e14390b4     	strh	r9, [r3, #-4]
     658: e153e0b2     	ldrh	lr, [r3, #-2]
     65c: e15e0002     	cmp	lr, r2
     660: 8a000009     	bhi	0x68c <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x68c> @ imm = #0x24
     664: e59de054     	ldr	lr, [sp, #0x54]
     668: e08ee004     	add	lr, lr, r4
     66c: e7e7e85e     	ubfx	lr, lr, #0x10, #0x8
     670: e180000e     	orr	r0, r0, lr
     674: e59de058     	ldr	lr, [sp, #0x58]
     678: e08ee005     	add	lr, lr, r5
     67c: e20ee8ff     	and	lr, lr, #16711680
     680: e180000e     	orr	r0, r0, lr
     684: e5060004     	str	r0, [r6, #-0x4]
     688: e14320b2     	strh	r2, [r3, #-2]
     68c: e59d2010     	ldr	r2, [sp, #0x10]
     690: e2833008     	add	r3, r3, #8
     694: e2866010     	add	r6, r6, #16
     698: e0855002     	add	r5, r5, r2
     69c: e59d2030     	ldr	r2, [sp, #0x30]
     6a0: e1520003     	cmp	r2, r3
     6a4: e59d2028     	ldr	r2, [sp, #0x28]
     6a8: e0844002     	add	r4, r4, r2
     6ac: 1affffa8     	bne	0x554 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x554> @ imm = #-0x160
     6b0: e28d00a4     	add	r0, sp, #164
     6b4: e59d90b0     	ldr	r9, [sp, #0xb0]
     6b8: e8904005     	ldm	r0, {r0, r2, lr}
     6bc: e59d3044     	ldr	r3, [sp, #0x44]
     6c0: e59dc000     	ldr	r12, [sp]
     6c4: e0805009     	add	r5, r0, r9
     6c8: e59d0068     	ldr	r0, [sp, #0x68]
     6cc: e59d6064     	ldr	r6, [sp, #0x64]
     6d0: e59d403c     	ldr	r4, [sp, #0x3c]
     6d4: e59d1040     	ldr	r1, [sp, #0x40]
     6d8: e0833080     	add	r3, r3, r0, lsl #1
     6dc: e08c8086     	add	r8, r12, r6, lsl #1
     6e0: e0833000     	add	r3, r3, r0
     6e4: e59d0094     	ldr	r0, [sp, #0x94]
     6e8: e0811084     	add	r1, r1, r4, lsl #1
     6ec: e0888006     	add	r8, r8, r6
     6f0: e080c00e     	add	r12, r0, lr
     6f4: e59de028     	ldr	lr, [sp, #0x28]
     6f8: e0810004     	add	r0, r1, r4
     6fc: e59d1048     	ldr	r1, [sp, #0x48]
     700: e02b3e92     	mla	r11, r2, lr, r3
     704: e59d3010     	ldr	r3, [sp, #0x10]
     708: e2411004     	sub	r1, r1, #4
     70c: e0230392     	mla	r3, r2, r3, r0
     710: e58d3038     	str	r3, [sp, #0x38]
     714: e59d3014     	ldr	r3, [sp, #0x14]
     718: e0288392     	mla	r8, r2, r3, r8
     71c: e59d3090     	ldr	r3, [sp, #0x90]
     720: e02cc392     	mla	r12, r2, r3, r12
     724: e0413102     	sub	r3, r1, r2, lsl #2
     728: e58d3048     	str	r3, [sp, #0x48]
     72c: e59d3098     	ldr	r3, [sp, #0x98]
     730: e0834202     	add	r4, r3, r2, lsl #4
     734: e59d3048     	ldr	r3, [sp, #0x48]
     738: e3530000     	cmp	r3, #0
     73c: ba000018     	blt	0x7a4 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x7a4> @ imm = #0x60
     740: e59d3048     	ldr	r3, [sp, #0x48]
     744: e2451002     	sub	r1, r5, #2
     748: e59d7064     	ldr	r7, [sp, #0x64]
     74c: e59d9068     	ldr	r9, [sp, #0x68]
     750: e59d0038     	ldr	r0, [sp, #0x38]
     754: e085e083     	add	lr, r5, r3, lsl #1
     758: e1f160b2     	ldrh	r6, [r1, #2]!
     75c: e1a0572c     	lsr	r5, r12, #14
     760: e1a02448     	asr	r2, r8, #8
     764: e7e7385b     	ubfx	r3, r11, #0x10, #0x8
     768: e08cc00a     	add	r12, r12, r10
     76c: e1560005     	cmp	r6, r5
     770: e2022cff     	and	r2, r2, #65280
     774: e1823003     	orr	r3, r2, r3
     778: 920028ff     	andls	r2, r0, #16711680
     77c: 91833002     	orrls	r3, r3, r2
     780: e0888007     	add	r8, r8, r7
     784: 95843000     	strls	r3, [r4]
     788: e08bb009     	add	r11, r11, r9
     78c: e59d303c     	ldr	r3, [sp, #0x3c]
     790: e2844004     	add	r4, r4, #4
     794: 91c150b0     	strhls	r5, [r1]
     798: e15e0001     	cmp	lr, r1
     79c: e0800003     	add	r0, r0, r3
     7a0: 1affffec     	bne	0x758 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x758> @ imm = #-0x50
     7a4: e59d3060     	ldr	r3, [sp, #0x60]
     7a8: e59d20c0     	ldr	r2, [sp, #0xc0]
     7ac: e0833002     	add	r3, r3, r2
     7b0: e3530000     	cmp	r3, #0
     7b4: e58d3060     	str	r3, [sp, #0x60]
     7b8: da000031     	ble	0x884 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x884> @ imm = #0xc4
     7bc: e59d3060     	ldr	r3, [sp, #0x60]
     7c0: e59d20ec     	ldr	r2, [sp, #0xec]
     7c4: e2433801     	sub	r3, r3, #65536
     7c8: e58d3060     	str	r3, [sp, #0x60]
     7cc: e59d3074     	ldr	r3, [sp, #0x74]
     7d0: e0833002     	add	r3, r3, r2
     7d4: e59d20dc     	ldr	r2, [sp, #0xdc]
     7d8: e58d3074     	str	r3, [sp, #0x74]
     7dc: e59d3078     	ldr	r3, [sp, #0x78]
     7e0: e0833002     	add	r3, r3, r2
     7e4: e59d20e0     	ldr	r2, [sp, #0xe0]
     7e8: e58d3078     	str	r3, [sp, #0x78]
     7ec: e59d307c     	ldr	r3, [sp, #0x7c]
     7f0: e0833002     	add	r3, r3, r2
     7f4: e59d20e4     	ldr	r2, [sp, #0xe4]
     7f8: e58d307c     	str	r3, [sp, #0x7c]
     7fc: e59d3080     	ldr	r3, [sp, #0x80]
     800: e0833002     	add	r3, r3, r2
     804: e59d20e8     	ldr	r2, [sp, #0xe8]
     808: e58d3080     	str	r3, [sp, #0x80]
     80c: e59d3084     	ldr	r3, [sp, #0x84]
     810: e0833002     	add	r3, r3, r2
     814: e58d3084     	str	r3, [sp, #0x84]
     818: e59d306c     	ldr	r3, [sp, #0x6c]
     81c: e59d20b4     	ldr	r2, [sp, #0xb4]
     820: e3530000     	cmp	r3, #0
     824: e59d3070     	ldr	r3, [sp, #0x70]
     828: e0833002     	add	r3, r3, r2
     82c: e59d208c     	ldr	r2, [sp, #0x8c]
     830: e58d3070     	str	r3, [sp, #0x70]
     834: e59d30bc     	ldr	r3, [sp, #0xbc]
     838: e5933020     	ldr	r3, [r3, #0x20]
     83c: e0822103     	add	r2, r2, r3, lsl #2
     840: e58d208c     	str	r2, [sp, #0x8c]
     844: e59d2088     	ldr	r2, [sp, #0x88]
     848: e0823083     	add	r3, r2, r3, lsl #1
     84c: e58d3088     	str	r3, [sp, #0x88]
     850: 1afffef2     	bne	0x420 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x420> @ imm = #-0x438
     854: e28d5f4a     	add	r5, sp, #296
     858: e59d30b4     	ldr	r3, [sp, #0xb4]
     85c: e8950260     	ldm	r5, {r5, r6, r9}
     860: e1a0b00a     	mov	r11, r10
     864: e59da124     	ldr	r10, [sp, #0x124]
     868: e59d7134     	ldr	r7, [sp, #0x134]
     86c: e026639a     	mla	r6, r10, r3, r6
     870: e3550001     	cmp	r5, #1
     874: 1a0000a0     	bne	0xafc <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xafc> @ imm = #0x280
     878: e28ddf59     	add	sp, sp, #356
     87c: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     880: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     884: e59d3074     	ldr	r3, [sp, #0x74]
     888: e59d2120     	ldr	r2, [sp, #0x120]
     88c: e0833002     	add	r3, r3, r2
     890: e59d20f0     	ldr	r2, [sp, #0xf0]
     894: e58d3074     	str	r3, [sp, #0x74]
     898: e59d3078     	ldr	r3, [sp, #0x78]
     89c: e0833002     	add	r3, r3, r2
     8a0: e59d20f4     	ldr	r2, [sp, #0xf4]
     8a4: e58d3078     	str	r3, [sp, #0x78]
     8a8: e59d307c     	ldr	r3, [sp, #0x7c]
     8ac: e0833002     	add	r3, r3, r2
     8b0: e59d20f8     	ldr	r2, [sp, #0xf8]
     8b4: e58d307c     	str	r3, [sp, #0x7c]
     8b8: e59d3080     	ldr	r3, [sp, #0x80]
     8bc: e0833002     	add	r3, r3, r2
     8c0: e59d20fc     	ldr	r2, [sp, #0xfc]
     8c4: e58d3080     	str	r3, [sp, #0x80]
     8c8: e59d3084     	ldr	r3, [sp, #0x84]
     8cc: e0833002     	add	r3, r3, r2
     8d0: e58d3084     	str	r3, [sp, #0x84]
     8d4: eaffffcf     	b	0x818 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x818> @ imm = #-0xc4
     8d8: e314000f     	tst	r4, #15
     8dc: 0a0000d3     	beq	0xc30 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xc30> @ imm = #0x34c
     8e0: e28d007c     	add	r0, sp, #124
     8e4: e59d9064     	ldr	r9, [sp, #0x64]
     8e8: e8900901     	ldm	r0, {r0, r8, r11}
     8ec: e1a01005     	mov	r1, r5
     8f0: e59d6068     	ldr	r6, [sp, #0x68]
     8f4: e59d7048     	ldr	r7, [sp, #0x48]
     8f8: e0d150b2     	ldrh	r5, [r1], #2
     8fc: e1a0e72c     	lsr	lr, r12, #14
     900: e1a02448     	asr	r2, r8, #8
     904: e7e7385b     	ubfx	r3, r11, #0x10, #0x8
     908: e2844004     	add	r4, r4, #4
     90c: e155000e     	cmp	r5, lr
     910: e2022cff     	and	r2, r2, #65280
     914: e1823003     	orr	r3, r2, r3
     918: 920058ff     	andls	r5, r0, #16711680
     91c: 91833005     	orrls	r3, r3, r5
     920: e2472001     	sub	r2, r7, #1
     924: 95043004     	strls	r3, [r4, #-0x4]
     928: e08cc00a     	add	r12, r12, r10
     92c: e59d303c     	ldr	r3, [sp, #0x3c]
     930: e0888009     	add	r8, r8, r9
     934: 9141e0b2     	strhls	lr, [r1, #-2]
     938: e314000f     	tst	r4, #15
     93c: e08bb006     	add	r11, r11, r6
     940: e1a05001     	mov	r5, r1
     944: e0800003     	add	r0, r0, r3
     948: 0a000087     	beq	0xb6c <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xb6c> @ imm = #0x21c
     94c: e3720001     	cmn	r2, #1
     950: e1a07002     	mov	r7, r2
     954: 1affffe7     	bne	0x8f8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x8f8> @ imm = #-0x64
     958: e1a06002     	mov	r6, r2
     95c: e3a07000     	mov	r7, #0
     960: e58d0038     	str	r0, [sp, #0x38]
     964: e59d3038     	ldr	r3, [sp, #0x38]
     968: e3570003     	cmp	r7, #3
     96c: eddd3b42     	vldr	d19, [sp, #264]
     970: e59de13c     	ldr	lr, [sp, #0x13c]
     974: eddd6b44     	vldr	d22, [sp, #272]
     978: eeaa3b90     	vdup.32	q13, r3
     97c: e28d303c     	add	r3, sp, #60
     980: f4e3380f     	vld1.32	{d19[0]}, [r3]
     984: e28d3064     	add	r3, sp, #100
     988: eddd7b46     	vldr	d23, [sp, #280]
     98c: f4e3680f     	vld1.32	{d22[0]}, [r3]
     990: e28d3068     	add	r3, sp, #104
     994: f4e3780f     	vld1.32	{d23[0]}, [r3]
     998: e59d3138     	ldr	r3, [sp, #0x138]
     99c: eddd2b40     	vldr	d18, [sp, #256]
     9a0: ee02ab90     	vmov.32	d18[0], r10
     9a4: e893000f     	ldm	r3, {r0, r1, r2, r3}
     9a8: f22271b2     	vorr	d7, d18, d18
     9ac: eeaccb90     	vdup.32	q14, r12
     9b0: e88e000f     	stm	lr, {r0, r1, r2, r3}
     9b4: f46e0a9f     	vld1.32	{d16, d17}, [lr:64]
     9b8: f3e0c0c7     	vmla.i32	q14, q8, d7[0]
     9bc: eea88b90     	vdup.32	q12, r8
     9c0: f22371b3     	vorr	d7, d19, d19
     9c4: eea4bb90     	vdup.32	q10, r11
     9c8: f3e0a0c7     	vmla.i32	q13, q8, d7[0]
     9cc: edcd2b40     	vstr	d18, [sp, #256]
     9d0: f22671b6     	vorr	d7, d22, d22
     9d4: edcd3b42     	vstr	d19, [sp, #264]
     9d8: f3e080c7     	vmla.i32	q12, q8, d7[0]
     9dc: edcd6b44     	vstr	d22, [sp, #272]
     9e0: f22771b7     	vorr	d7, d23, d23
     9e4: edcd7b46     	vstr	d23, [sp, #280]
     9e8: f3e040c7     	vmla.i32	q10, q8, d7[0]
     9ec: 9a000089     	bls	0xc18 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xc18> @ imm = #0x224
     9f0: e1a03005     	mov	r3, r5
     9f4: e1a02004     	mov	r2, r4
     9f8: e3a01004     	mov	r1, #4
     9fc: ed8d8a00     	vstr	s16, [sp]
     a00: edcd9a06     	vstr	s19, [sp, #24]
     a04: ed8d9a0b     	vstr	s18, [sp, #44]
     a08: edcd8a0c     	vstr	s17, [sp, #48]
     a0c: ea00000d     	b	0xa48 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xa48> @ imm = #0x34
     a10: f4622a8f     	vld1.32	{d18, d19}, [r2]
     a14: f3586172     	vbsl	q11, q4, q9
     a18: f4426a8f     	vst1.32	{d22, d23}, [r2]
     a1c: f443174f     	vst1.16	{d17}, [r3]
     a20: e2810004     	add	r0, r1, #4
     a24: f26cc8ca     	vadd.i32	q14, q14, q5
     a28: e1500007     	cmp	r0, r7
     a2c: f26aa8cc     	vadd.i32	q13, q13, q6
     a30: f26888ce     	vadd.i32	q12, q12, q7
     a34: e2833008     	add	r3, r3, #8
     a38: f26448c4     	vadd.i32	q10, q10, q2
     a3c: e2822010     	add	r2, r2, #16
     a40: 8a00001b     	bhi	0xab4 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xab4> @ imm = #0x6c
     a44: e1a01000     	mov	r1, r0
     a48: f463274f     	vld1.16	{d18}, [r3]
     a4c: f3c7e45f     	vmov.i32	q15, #0xff0000
     a50: f3b2007c     	vshr.u32	q0, q14, #0xe
     a54: f3d06a32     	vmovl.u16	q11, d18
     a58: f20a61fe     	vand	q3, q13, q15
     a5c: f3b88078     	vshr.u32	q4, q12, #0x8
     a60: f3f00074     	vshr.u32	q8, q10, #0x10
     a64: f3c7e05f     	vmov.i32	q15, #0xff
     a68: f2088152     	vand	q4, q4, q1
     a6c: f3606376     	vcge.u32	q11, q0, q11
     a70: f24001fe     	vand	q8, q8, q15
     a74: f2680170     	vorr	q8, q4, q8
     a78: f22081d6     	vorr	q4, q8, q3
     a7c: f3b67226     	vmovn.i32	d7, q11
     a80: ee17ea10     	vmov	lr, s14
     a84: ee170a90     	vmov	r0, s15
     a88: e190900e     	orrs	r9, r0, lr
     a8c: 0affffe3     	beq	0xa20 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xa20> @ imm = #-0x74
     a90: f3f61200     	vmovn.i32	d17, q0
     a94: e3700001     	cmn	r0, #1
     a98: 037e0001     	cmneq	lr, #1
     a9c: f3721197     	vbif	d17, d18, d7
     aa0: 1affffda     	bne	0xa10 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xa10> @ imm = #-0x98
     aa4: f3b60200     	vmovn.i32	d0, q0
     aa8: f4028a8f     	vst1.32	{d8, d9}, [r2]
     aac: f403074f     	vst1.16	{d0}, [r3]
     ab0: eaffffda     	b	0xa20 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xa20> @ imm = #-0x98
     ab4: e59d3038     	ldr	r3, [sp, #0x38]
     ab8: e02cca91     	mla	r12, r1, r10, r12
     abc: e59d203c     	ldr	r2, [sp, #0x3c]
     ac0: ed9d8a00     	vldr	s16, [sp]
     ac4: e0844101     	add	r4, r4, r1, lsl #2
     ac8: eddd9a06     	vldr	s19, [sp, #24]
     acc: e0855081     	add	r5, r5, r1, lsl #1
     ad0: ed9d9a0b     	vldr	s18, [sp, #44]
     ad4: e0233291     	mla	r3, r1, r2, r3
     ad8: eddd8a0c     	vldr	s17, [sp, #48]
     adc: e58d3038     	str	r3, [sp, #0x38]
     ae0: e59d3064     	ldr	r3, [sp, #0x64]
     ae4: e0288391     	mla	r8, r1, r3, r8
     ae8: e59d3068     	ldr	r3, [sp, #0x68]
     aec: e02bb391     	mla	r11, r1, r3, r11
     af0: e0463001     	sub	r3, r6, r1
     af4: e58d3048     	str	r3, [sp, #0x48]
     af8: eafffe5c     	b	0x470 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x470> @ imm = #-0x690
     afc: e5991004     	ldr	r1, [r9, #0x4]
     b00: e5973004     	ldr	r3, [r7, #0x4]
     b04: eeb58ac0     	vcmpe.f32	s16, #0
     b08: eef1fa10     	vmrs	APSR_nzcv, fpscr
     b0c: e0411003     	sub	r1, r1, r3
     b10: e281a001     	add	r10, r1, #1
     b14: da000022     	ble	0xba4 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xba4> @ imm = #0x88
     b18: e3510000     	cmp	r1, #0
     b1c: e5976000     	ldr	r6, [r7]
     b20: e3a04001     	mov	r4, #1
     b24: c58d714c     	strgt	r7, [sp, #0x14c]
     b28: c58d9148     	strgt	r9, [sp, #0x148]
     b2c: da000021     	ble	0xbb8 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xbb8> @ imm = #0x84
     b30: e28d3018     	add	r3, sp, #24
     b34: f40d2adf     	vst1.64	{d2, d3}, [sp:64]
     b38: e1a05004     	mov	r5, r4
     b3c: f4034adf     	vst1.64	{d4, d5}, [r3:64]
     b40: e59d3148     	ldr	r3, [sp, #0x148]
     b44: e5930000     	ldr	r0, [r3]
     b48: e0400006     	sub	r0, r0, r6
     b4c: e1a00800     	lsl	r0, r0, #16
     b50: ebfffffe     	bl	0xb50 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xb50> @ imm = #-0x8
     b54: e28d3018     	add	r3, sp, #24
     b58: f42d2adf     	vld1.64	{d2, d3}, [sp:64]
     b5c: e58d00b4     	str	r0, [sp, #0xb4]
     b60: f4234adf     	vld1.64	{d4, d5}, [r3:64]
     b64: e1a06806     	lsl	r6, r6, #16
     b68: eafffe0f     	b	0x3ac <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x3ac> @ imm = #-0x7c4
     b6c: e1a06002     	mov	r6, r2
     b70: e58d0038     	str	r0, [sp, #0x38]
     b74: eaffff7a     	b	0x964 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x964> @ imm = #-0x218
     b78: e59d2144     	ldr	r2, [sp, #0x144]
     b7c: e3a03000     	mov	r3, #0
     b80: e58d30c0     	str	r3, [sp, #0xc0]
     b84: edcd8a3f     	vstr	s17, [sp, #252]
     b88: e58d20f0     	str	r2, [sp, #0xf0]
     b8c: e3a02001     	mov	r2, #1
     b90: ed8d9a3e     	vstr	s18, [sp, #248]
     b94: edcd9a3d     	vstr	s19, [sp, #244]
     b98: e58d20ec     	str	r2, [sp, #0xec]
     b9c: e58d3120     	str	r3, [sp, #0x120]
     ba0: eafffdf6     	b	0x380 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x380> @ imm = #-0x828
     ba4: e58d5060     	str	r5, [sp, #0x60]
     ba8: e1a02009     	mov	r2, r9
     bac: e3a05001     	mov	r5, #1
     bb0: e1a04007     	mov	r4, r7
     bb4: eafffdce     	b	0x2f4 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x2f4> @ imm = #-0x8c8
     bb8: e35a0000     	cmp	r10, #0
     bbc: daffff2d     	ble	0x878 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x878> @ imm = #-0x34c
     bc0: e58d50b4     	str	r5, [sp, #0xb4]
     bc4: e1a06806     	lsl	r6, r6, #16
     bc8: e1a05004     	mov	r5, r4
     bcc: e58d9148     	str	r9, [sp, #0x148]
     bd0: e58d714c     	str	r7, [sp, #0x14c]
     bd4: eafffdf6     	b	0x3b4 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x3b4> @ imm = #-0x828
     bd8: e1a01003     	mov	r1, r3
     bdc: e1a02006     	mov	r2, r6
     be0: e58d9148     	str	r9, [sp, #0x148]
     be4: eafffdad     	b	0x2a0 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x2a0> @ imm = #-0x94c
     be8: e59d3148     	ldr	r3, [sp, #0x148]
     bec: e59d214c     	ldr	r2, [sp, #0x14c]
     bf0: e5931004     	ldr	r1, [r3, #0x4]
     bf4: e5923004     	ldr	r3, [r2, #0x4]
     bf8: e5926000     	ldr	r6, [r2]
     bfc: e0411003     	sub	r1, r1, r3
     c00: e3510000     	cmp	r1, #0
     c04: da000005     	ble	0xc20 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xc20> @ imm = #0x14
     c08: e3a03000     	mov	r3, #0
     c0c: e1a04005     	mov	r4, r5
     c10: e58d3060     	str	r3, [sp, #0x60]
     c14: eaffffc5     	b	0xb30 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xb30> @ imm = #-0xec
     c18: e58d6048     	str	r6, [sp, #0x48]
     c1c: eafffe13     	b	0x470 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x470> @ imm = #-0x7b4
     c20: e3a03000     	mov	r3, #0
     c24: e58d3060     	str	r3, [sp, #0x60]
     c28: e58d30b4     	str	r3, [sp, #0xb4]
     c2c: eaffffcc     	b	0xb64 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0xb64> @ imm = #-0xd0
     c30: e28d307c     	add	r3, sp, #124
     c34: e59d6048     	ldr	r6, [sp, #0x48]
     c38: e8930908     	ldm	r3, {r3, r8, r11}
     c3c: e2867001     	add	r7, r6, #1
     c40: e58d3038     	str	r3, [sp, #0x38]
     c44: eaffff46     	b	0x964 <ZB_fillTriangleSmoothNOBLEND_DT1_DW1+0x964> @ imm = #-0x2e8
     c48: f8 0b 00 00  	.word	0x00000bf8
     c4c: 68 09 00 00  	.word	0x00000968
     c50: 00 00 00 00  	.word	0x00000000

Disassembly of section .text.ZB_setTexture:
