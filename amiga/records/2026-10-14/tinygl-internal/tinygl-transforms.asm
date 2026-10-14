
.context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-pic-vfp-matrix-bench-tinygl-internal/102-vertex.arm.o:	file format elf32-littlearm

Disassembly of section .text.glopNormal:

00000000 <glopNormal>:
       0: e92d4010     	push	{r4, lr}
       4: e1a04000     	mov	r4, r0
       8: ebfffffe     	bl	0x8 <glopNormal+0x8>    @ imm = #-0x8
       c: e2803ec2     	add	r3, r0, #3104
      10: e5942004     	ldr	r2, [r4, #0x4]
      14: e594c008     	ldr	r12, [r4, #0x8]
      18: e594100c     	ldr	r1, [r4, #0xc]
      1c: e5832000     	str	r2, [r3]
      20: e3a02000     	mov	r2, #0
      24: e583c004     	str	r12, [r3, #0x4]
      28: e2803ec2     	add	r3, r0, #3104
      2c: e5831008     	str	r1, [r3, #0x8]
      30: e583200c     	str	r2, [r3, #0xc]
      34: e8bd8010     	pop	{r4, pc}

Disassembly of section .text.glopTexCoord:

00000000 <glopTexCoord>:
       0: e92d4070     	push	{r4, r5, r6, lr}
       4: e1a04000     	mov	r4, r0
       8: ebfffffe     	bl	0x8 <glopTexCoord+0x8>  @ imm = #-0x8
       c: e2803ec3     	add	r3, r0, #3120
      10: e5945004     	ldr	r5, [r4, #0x4]
      14: e1a0e003     	mov	lr, r3
      18: e594c008     	ldr	r12, [r4, #0x8]
      1c: e594100c     	ldr	r1, [r4, #0xc]
      20: e5942010     	ldr	r2, [r4, #0x10]
      24: e5835000     	str	r5, [r3]
      28: e583c004     	str	r12, [r3, #0x4]
      2c: e5831008     	str	r1, [r3, #0x8]
      30: e583200c     	str	r2, [r3, #0xc]
      34: e8bd8070     	pop	{r4, r5, r6, pc}

Disassembly of section .text.glopEdgeFlag:

00000000 <glopEdgeFlag>:
       0: e92d4010     	push	{r4, lr}
       4: e1a04000     	mov	r4, r0
       8: ebfffffe     	bl	0x8 <glopEdgeFlag+0x8>  @ imm = #-0x8
       c: e5943004     	ldr	r3, [r4, #0x4]
      10: e5803d14     	str	r3, [r0, #0xd14]
      14: e8bd8010     	pop	{r4, pc}

Disassembly of section .text.glopColor:

00000000 <glopColor>:
       0: e92d4010     	push	{r4, lr}
       4: e24dd020     	sub	sp, sp, #32
       8: e1a04000     	mov	r4, r0
       c: ebfffffe     	bl	0xc <glopColor+0xc>     @ imm = #-0x8
      10: e594e008     	ldr	lr, [r4, #0x8]
      14: e1a03000     	mov	r3, r0
      18: e5940004     	ldr	r0, [r4, #0x4]
      1c: e2832ec1     	add	r2, r3, #3088
      20: e594c00c     	ldr	r12, [r4, #0xc]
      24: e582e004     	str	lr, [r2, #0x4]
      28: e5820000     	str	r0, [r2]
      2c: e5932cac     	ldr	r2, [r3, #0xcac]
      30: e5941010     	ldr	r1, [r4, #0x10]
      34: e3520000     	cmp	r2, #0
      38: e2832ec1     	add	r2, r3, #3088
      3c: e582c008     	str	r12, [r2, #0x8]
      40: e582100c     	str	r1, [r2, #0xc]
      44: 1a000001     	bne	0x50 <glopColor+0x50>   @ imm = #0x4
      48: e28dd020     	add	sp, sp, #32
      4c: e8bd8010     	pop	{r4, pc}
      50: e5932cb0     	ldr	r2, [r3, #0xcb0]
      54: e5933cb4     	ldr	r3, [r3, #0xcb4]
      58: e58d0010     	str	r0, [sp, #0x10]
      5c: e28d0004     	add	r0, sp, #4
      60: e58de014     	str	lr, [sp, #0x14]
      64: e58d300c     	str	r3, [sp, #0xc]
      68: e3a03013     	mov	r3, #19
      6c: e58dc018     	str	r12, [sp, #0x18]
      70: e58d101c     	str	r1, [sp, #0x1c]
      74: e58d2008     	str	r2, [sp, #0x8]
      78: e58d3004     	str	r3, [sp, #0x4]
      7c: ebfffffe     	bl	0x7c <glopColor+0x7c>   @ imm = #-0x8
      80: e28dd020     	add	sp, sp, #32
      84: e8bd8010     	pop	{r4, pc}

Disassembly of section .text.glopBegin:

00000000 <glopBegin>:
       0: e92d4070     	push	{r4, r5, r6, lr}
       4: e24dd048     	sub	sp, sp, #72
       8: e1a06000     	mov	r6, r0
       c: e59f51d0     	ldr	r5, [pc, #0x1d0]        @ 0x1e4 <glopBegin+0x1e4>
      10: ebfffffe     	bl	0x10 <glopBegin+0x10>   @ imm = #-0x8
      14: e5902ce0     	ldr	r2, [r0, #0xce0]
      18: e1a04000     	mov	r4, r0
      1c: e5963004     	ldr	r3, [r6, #0x4]
      20: e3a00000     	mov	r0, #0
      24: e3a01001     	mov	r1, #1
      28: e08f5005     	add	r5, pc, r5
      2c: e1520000     	cmp	r2, r0
      30: e5841d18     	str	r1, [r4, #0xd18]
      34: e5840d20     	str	r0, [r4, #0xd20]
      38: e5843d1c     	str	r3, [r4, #0xd1c]
      3c: e5840d24     	str	r0, [r4, #0xd24]
      40: 0a00001a     	beq	0xb0 <glopBegin+0xb0>   @ imm = #0x68
      44: e59f319c     	ldr	r3, [pc, #0x19c]        @ 0x1e8 <glopBegin+0x1e8>
      48: e7953003     	ldr	r3, [r5, r3]
      4c: e1a06003     	mov	r6, r3
      50: e5933000     	ldr	r3, [r3]
      54: e1530000     	cmp	r3, r0
      58: 1a00005b     	bne	0x1cc <glopBegin+0x1cc> @ imm = #0x16c
      5c: e5943ca4     	ldr	r3, [r4, #0xca4]
      60: e5942c78     	ldr	r2, [r4, #0xc78]
      64: e3530000     	cmp	r3, #0
      68: 0a00002f     	beq	0x12c <glopBegin+0x12c> @ imm = #0xbc
      6c: e28d0008     	add	r0, sp, #8
      70: e1a01002     	mov	r1, r2
      74: e58d0000     	str	r0, [sp]
      78: ebfffffe     	bl	0x78 <glopBegin+0x78>   @ imm = #-0x8
      7c: e59d1000     	ldr	r1, [sp]
      80: e2840eb7     	add	r0, r4, #2928
      84: ebfffffe     	bl	0x84 <glopBegin+0x84>   @ imm = #-0x8
      88: e5940c80     	ldr	r0, [r4, #0xc80]
      8c: ebfffffe     	bl	0x8c <glopBegin+0x8c>   @ imm = #-0x8
      90: e5963000     	ldr	r3, [r6]
      94: e3a02000     	mov	r2, #0
      98: e16f0f10     	clz	r0, r0
      9c: e5842ce0     	str	r2, [r4, #0xce0]
      a0: e1a002a0     	lsr	r0, r0, #5
      a4: e1530002     	cmp	r3, r2
      a8: e5840ce8     	str	r0, [r4, #0xce8]
      ac: 1a000044     	bne	0x1c4 <glopBegin+0x1c4> @ imm = #0x110
      b0: e5943cf0     	ldr	r3, [r4, #0xcf0]
      b4: e3530c1b     	cmp	r3, #6912
      b8: 0a000039     	beq	0x1a4 <glopBegin+0x1a4> @ imm = #0xe4
      bc: e3012b01     	movw	r2, #0x1b01
      c0: e1530002     	cmp	r3, r2
      c4: 1a00000d     	bne	0x100 <glopBegin+0x100> @ imm = #0x34
      c8: e59f311c     	ldr	r3, [pc, #0x11c]        @ 0x1ec <glopBegin+0x1ec>
      cc: e7952003     	ldr	r2, [r5, r3]
      d0: e5943cec     	ldr	r3, [r4, #0xcec]
      d4: e5842c84     	str	r2, [r4, #0xc84]
      d8: e3530c1b     	cmp	r3, #6912
      dc: 0a00000d     	beq	0x118 <glopBegin+0x118> @ imm = #0x34
      e0: e3012b01     	movw	r2, #0x1b01
      e4: e1530002     	cmp	r3, r2
      e8: 0a000030     	beq	0x1b0 <glopBegin+0x1b0> @ imm = #0xc0
      ec: e59f30fc     	ldr	r3, [pc, #0xfc]         @ 0x1f0 <glopBegin+0x1f0>
      f0: e7953003     	ldr	r3, [r5, r3]
      f4: e5843c88     	str	r3, [r4, #0xc88]
      f8: e28dd048     	add	sp, sp, #72
      fc: e8bd8070     	pop	{r4, r5, r6, pc}
     100: e59f30e8     	ldr	r3, [pc, #0xe8]         @ 0x1f0 <glopBegin+0x1f0>
     104: e7952003     	ldr	r2, [r5, r3]
     108: e5943cec     	ldr	r3, [r4, #0xcec]
     10c: e5842c84     	str	r2, [r4, #0xc84]
     110: e3530c1b     	cmp	r3, #6912
     114: 1afffff1     	bne	0xe0 <glopBegin+0xe0>   @ imm = #-0x3c
     118: e59f30d4     	ldr	r3, [pc, #0xd4]         @ 0x1f4 <glopBegin+0x1f4>
     11c: e7953003     	ldr	r3, [r5, r3]
     120: e5843c88     	str	r3, [r4, #0xc88]
     124: e28dd048     	add	sp, sp, #72
     128: e8bd8070     	pop	{r4, r5, r6, pc}
     12c: e2840ebb     	add	r0, r4, #2992
     130: e5941c7c     	ldr	r1, [r4, #0xc7c]
     134: e58d3004     	str	r3, [sp, #0x4]
     138: e58d0000     	str	r0, [sp]
     13c: ebfffffe     	bl	0x13c <glopBegin+0x13c> @ imm = #-0x8
     140: e59d0000     	ldr	r0, [sp]
     144: e2842ebd     	add	r2, r4, #3024
     148: e59d3004     	ldr	r3, [sp, #0x4]
     14c: f460018f     	vld4.32	{d16, d18, d20, d22}, [r0]
     150: f462118f     	vld4.32	{d17, d19, d21, d23}, [r2]
     154: e5843ce4     	str	r3, [r4, #0xce4]
     158: e2843ebe     	add	r3, r4, #3040
     15c: edd37a00     	vldr	s15, [r3]
     160: e59f3090     	ldr	r3, [pc, #0x90]         @ 0x1f8 <glopBegin+0x1f8>
     164: eef57a40     	vcmp.f32	s15, #0
     168: e08f3003     	add	r3, pc, r3
     16c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     170: ecc30b10     	vstmia	r3, {d16, d17, d18, d19, d20, d21, d22, d23}
     174: 1affffc3     	bne	0x88 <glopBegin+0x88>   @ imm = #-0xf4
     178: e2843ebe     	add	r3, r4, #3040
     17c: edd37a01     	vldr	s15, [r3, #4]
     180: eef57a40     	vcmp.f32	s15, #0
     184: eef1fa10     	vmrs	APSR_nzcv, fpscr
     188: 1affffbe     	bne	0x88 <glopBegin+0x88>   @ imm = #-0x108
     18c: edd37a02     	vldr	s15, [r3, #8]
     190: eef57a40     	vcmp.f32	s15, #0
     194: eef1fa10     	vmrs	APSR_nzcv, fpscr
     198: 03a03001     	moveq	r3, #1
     19c: 05843ce4     	streq	r3, [r4, #0xce4]
     1a0: eaffffb8     	b	0x88 <glopBegin+0x88>   @ imm = #-0x120
     1a4: e59f3048     	ldr	r3, [pc, #0x48]         @ 0x1f4 <glopBegin+0x1f4>
     1a8: e7952003     	ldr	r2, [r5, r3]
     1ac: eaffffc7     	b	0xd0 <glopBegin+0xd0>   @ imm = #-0xe4
     1b0: e59f3034     	ldr	r3, [pc, #0x34]         @ 0x1ec <glopBegin+0x1ec>
     1b4: e7953003     	ldr	r3, [r5, r3]
     1b8: e5843c88     	str	r3, [r4, #0xc88]
     1bc: e28dd048     	add	sp, sp, #72
     1c0: e8bd8070     	pop	{r4, r5, r6, pc}
     1c4: ebfffffe     	bl	0x1c4 <glopBegin+0x1c4> @ imm = #-0x8
     1c8: eaffffb8     	b	0xb0 <glopBegin+0xb0>   @ imm = #-0x120
     1cc: ebfffffe     	bl	0x1cc <glopBegin+0x1cc> @ imm = #-0x8
     1d0: e5943ca4     	ldr	r3, [r4, #0xca4]
     1d4: e5942c78     	ldr	r2, [r4, #0xc78]
     1d8: e3530000     	cmp	r3, #0
     1dc: 0affffd2     	beq	0x12c <glopBegin+0x12c> @ imm = #-0xb8
     1e0: eaffffa1     	b	0x6c <glopBegin+0x6c>   @ imm = #-0x17c
     1e4: b4 01 00 00  	.word	0x000001b4
		...
     1f8: 88 00 00 00  	.word	0x00000088

Disassembly of section .text.gl_V3_Norm_Fast:

00000000 <gl_V3_Norm_Fast>:
       0: edd06a01     	vldr	s13, [r0, #4]
       4: e1a03000     	mov	r3, r0
       8: ed906a00     	vldr	s12, [r0]
       c: ed907a02     	vldr	s14, [r0, #8]
      10: ee667aa6     	vmul.f32	s15, s13, s13
      14: ee467a06     	vmla.f32	s15, s12, s12
      18: ee477a07     	vmla.f32	s15, s14, s14
      1c: eef70ae7     	vcvt.f64.f32	d16, s15
      20: eef10be0     	vsqrt.f64	d16, d16
      24: eef77be0     	vcvt.f32.f64	s15, d16
      28: eef57a40     	vcmp.f32	s15, #0
      2c: eef1fa10     	vmrs	APSR_nzcv, fpscr
      30: 1eb75a00     	vmovne.f32	s10, #1.000000e+00
      34: 13a00000     	movne	r0, #0
      38: 03a00001     	moveq	r0, #1
      3c: 1ec55a27     	vdivne.f32	s11, s10, s15
      40: 1e266a25     	vmulne.f32	s12, s12, s11
      44: 1e666aa5     	vmulne.f32	s13, s13, s11
      48: 1e277a25     	vmulne.f32	s14, s14, s11
      4c: 1d836a00     	vstrne	s12, [r3]
      50: 1dc36a01     	vstrne	s13, [r3, #4]
      54: 1d837a02     	vstrne	s14, [r3, #8]
      58: e12fff1e     	bx	lr

Disassembly of section .text.glopVertex:

00000000 <glopVertex>:
       0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       4: e24dd00c     	sub	sp, sp, #12
       8: e1a07000     	mov	r7, r0
       c: e3a0608c     	mov	r6, #140
      10: ebfffffe     	bl	0x10 <glopVertex+0x10>  @ imm = #-0x8
      14: e5905d20     	ldr	r5, [r0, #0xd20]
      18: e1a04000     	mov	r4, r0
      1c: e5908d24     	ldr	r8, [r0, #0xd24]
      20: e5971004     	ldr	r1, [r7, #0x4]
      24: e597c008     	ldr	r12, [r7, #0x8]
      28: e2859001     	add	r9, r5, #1
      2c: e0060596     	mul	r6, r6, r5
      30: e2888001     	add	r8, r8, #1
      34: e5808d24     	str	r8, [r0, #0xd24]
      38: e597e010     	ldr	lr, [r7, #0x10]
      3c: e597000c     	ldr	r0, [r7, #0xc]
      40: e084a006     	add	r10, r4, r6
      44: e59fb6d4     	ldr	r11, [pc, #0x6d4]       @ 0x720 <glopVertex+0x720>
      48: e59f36d4     	ldr	r3, [pc, #0x6d4]        @ 0x724 <glopVertex+0x724>
      4c: e28a2d25     	add	r2, r10, #2368
      50: e582100c     	str	r1, [r2, #0xc]
      54: e28a1e95     	add	r1, r10, #2384
      58: e08fb00b     	add	r11, pc, r11
      5c: e581c000     	str	r12, [r1]
      60: e5810004     	str	r0, [r1, #0x4]
      64: e581e008     	str	lr, [r1, #0x8]
      68: e79b7003     	ldr	r7, [r11, r3]
      6c: e5973000     	ldr	r3, [r7]
      70: e3530000     	cmp	r3, #0
      74: 1a00019d     	bne	0x6f0 <glopVertex+0x6f0> @ imm = #0x674
      78: e3a0208c     	mov	r2, #140
      7c: e5943ca4     	ldr	r3, [r4, #0xca4]
      80: e0224592     	mla	r2, r2, r5, r4
      84: e3530000     	cmp	r3, #0
      88: e2823d25     	add	r3, r2, #2368
      8c: ed936a03     	vldr	s12, [r3, #12]
      90: 0a000135     	beq	0x56c <glopVertex+0x56c> @ imm = #0x4d4
      94: e5943c78     	ldr	r3, [r4, #0xc78]
      98: e2821e95     	add	r1, r2, #2384
      9c: ed917a00     	vldr	s14, [r1]
      a0: e2840d2e     	add	r0, r4, #2944
      a4: e284cec2     	add	r12, r4, #3104
      a8: e284aeb7     	add	r10, r4, #2928
      ac: edd35a01     	vldr	s11, [r3, #4]
      b0: ed935a00     	vldr	s10, [r3]
      b4: edd36a02     	vldr	s13, [r3, #8]
      b8: edd37a03     	vldr	s15, [r3, #12]
      bc: ee675a25     	vmul.f32	s11, s14, s11
      c0: ee465a05     	vmla.f32	s11, s12, s10
      c4: ed915a01     	vldr	s10, [r1, #4]
      c8: e2821e97     	add	r1, r2, #2416
      cc: ee455a26     	vmla.f32	s11, s10, s13
      d0: ee755aa7     	vadd.f32	s11, s11, s15
      d4: edc15a03     	vstr	s11, [r1, #12]
      d8: e2821d26     	add	r1, r2, #2432
      dc: edd37a05     	vldr	s15, [r3, #20]
      e0: ed934a04     	vldr	s8, [r3, #16]
      e4: edd34a06     	vldr	s9, [r3, #24]
      e8: edd36a07     	vldr	s13, [r3, #28]
      ec: ee677a27     	vmul.f32	s15, s14, s15
      f0: ee467a04     	vmla.f32	s15, s12, s8
      f4: ee457a24     	vmla.f32	s15, s10, s9
      f8: ee777aa6     	vadd.f32	s15, s15, s13
      fc: edc17a00     	vstr	s15, [r1]
     100: edd36a09     	vldr	s13, [r3, #36]
     104: edd33a08     	vldr	s7, [r3, #32]
     108: ed934a0a     	vldr	s8, [r3, #40]
     10c: edd34a0b     	vldr	s9, [r3, #44]
     110: ee676a26     	vmul.f32	s13, s14, s13
     114: ee466a23     	vmla.f32	s13, s12, s7
     118: ee456a04     	vmla.f32	s13, s10, s8
     11c: ee766aa4     	vadd.f32	s13, s13, s9
     120: edc16a01     	vstr	s13, [r1, #4]
     124: ed933a0d     	vldr	s6, [r3, #52]
     128: edd33a0c     	vldr	s7, [r3, #48]
     12c: ed934a0e     	vldr	s8, [r3, #56]
     130: edd34a0f     	vldr	s9, [r3, #60]
     134: ee277a03     	vmul.f32	s14, s14, s6
     138: e5943c7c     	ldr	r3, [r4, #0xc7c]
     13c: ee067a23     	vmla.f32	s14, s12, s7
     140: ee057a04     	vmla.f32	s14, s10, s8
     144: ee377a24     	vadd.f32	s14, s14, s9
     148: ed817a02     	vstr	s14, [r1, #8]
     14c: ed936a01     	vldr	s12, [r3, #4]
     150: ed934a00     	vldr	s8, [r3]
     154: edd34a02     	vldr	s9, [r3, #8]
     158: ed935a03     	vldr	s10, [r3, #12]
     15c: ee276a86     	vmul.f32	s12, s15, s12
     160: ee056a84     	vmla.f32	s12, s11, s8
     164: ee066aa4     	vmla.f32	s12, s13, s9
     168: ee076a05     	vmla.f32	s12, s14, s10
     16c: ed816a03     	vstr	s12, [r1, #12]
     170: e2821e99     	add	r1, r2, #2448
     174: ed936a05     	vldr	s12, [r3, #20]
     178: e1a0b001     	mov	r11, r1
     17c: ed934a04     	vldr	s8, [r3, #16]
     180: edd34a06     	vldr	s9, [r3, #24]
     184: ed935a07     	vldr	s10, [r3, #28]
     188: ee276a86     	vmul.f32	s12, s15, s12
     18c: ee056a84     	vmla.f32	s12, s11, s8
     190: ee066aa4     	vmla.f32	s12, s13, s9
     194: ee076a05     	vmla.f32	s12, s14, s10
     198: ed816a00     	vstr	s12, [r1]
     19c: ed936a09     	vldr	s12, [r3, #36]
     1a0: ed934a08     	vldr	s8, [r3, #32]
     1a4: edd34a0a     	vldr	s9, [r3, #40]
     1a8: ed935a0b     	vldr	s10, [r3, #44]
     1ac: ee276a86     	vmul.f32	s12, s15, s12
     1b0: ee056a84     	vmla.f32	s12, s11, s8
     1b4: ee066aa4     	vmla.f32	s12, s13, s9
     1b8: ee076a05     	vmla.f32	s12, s14, s10
     1bc: ed816a01     	vstr	s12, [r1, #4]
     1c0: e2801008     	add	r1, r0, #8
     1c4: ed934a0d     	vldr	s8, [r3, #52]
     1c8: edd34a0c     	vldr	s9, [r3, #48]
     1cc: ed935a0e     	vldr	s10, [r3, #56]
     1d0: ed936a0f     	vldr	s12, [r3, #60]
     1d4: e2823d25     	add	r3, r2, #2368
     1d8: ee677a84     	vmul.f32	s15, s15, s8
     1dc: e2832008     	add	r2, r3, #8
     1e0: e58d1004     	str	r1, [sp, #0x4]
     1e4: e2831004     	add	r1, r3, #4
     1e8: e58d3000     	str	r3, [sp]
     1ec: ee457aa4     	vmla.f32	s15, s11, s9
     1f0: ee467a85     	vmla.f32	s15, s13, s10
     1f4: ee477a06     	vmla.f32	s15, s14, s12
     1f8: edcb7a02     	vstr	s15, [r11, #8]
     1fc: ee173a90     	vmov	r3, s15
     200: edda6a01     	vldr	s13, [r10, #4]
     204: eddc7a01     	vldr	s15, [r12, #4]
     208: eddc5a00     	vldr	s11, [r12]
     20c: ed9a5a00     	vldr	s10, [r10]
     210: ed9c6a02     	vldr	s12, [r12, #8]
     214: ee676aa6     	vmul.f32	s13, s15, s13
     218: ed9a7a02     	vldr	s14, [r10, #8]
     21c: e59dc000     	ldr	r12, [sp]
     220: ee456a85     	vmla.f32	s13, s11, s10
     224: ee466a07     	vmla.f32	s13, s12, s14
     228: edcc6a00     	vstr	s13, [r12]
     22c: ed907a01     	vldr	s14, [r0, #4]
     230: e2840d2e     	add	r0, r4, #2944
     234: edd04a00     	vldr	s9, [r0]
     238: e59d0004     	ldr	r0, [sp, #0x4]
     23c: ee277a87     	vmul.f32	s14, s15, s14
     240: ed905a00     	vldr	s10, [r0]
     244: e2840eb9     	add	r0, r4, #2960
     248: ee057aa4     	vmla.f32	s14, s11, s9
     24c: ee067a05     	vmla.f32	s14, s12, s10
     250: ed817a00     	vstr	s14, [r1]
     254: ed905a01     	vldr	s10, [r0, #4]
     258: ee677a85     	vmul.f32	s15, s15, s10
     25c: ed905a00     	vldr	s10, [r0]
     260: ee457a85     	vmla.f32	s15, s11, s10
     264: edd05a02     	vldr	s11, [r0, #8]
     268: e5940d04     	ldr	r0, [r4, #0xd04]
     26c: e3500000     	cmp	r0, #0
     270: ee467a25     	vmla.f32	s15, s12, s11
     274: edc27a00     	vstr	s15, [r2]
     278: 1a000144     	bne	0x790 <glopVertex+0x790> @ imm = #0x510
     27c: e3a0a08c     	mov	r10, #140
     280: e02a459a     	mla	r10, r10, r5, r4
     284: e28a1e99     	add	r1, r10, #2448
     288: e5912004     	ldr	r2, [r1, #0x4]
     28c: e28a1d26     	add	r1, r10, #2432
     290: e591000c     	ldr	r0, [r1, #0xc]
     294: e28a1e99     	add	r1, r10, #2448
     298: e5911000     	ldr	r1, [r1]
     29c: ebfffffe     	bl	0x29c <glopVertex+0x29c> @ imm = #-0x8
     2a0: e5973000     	ldr	r3, [r7]
     2a4: e58a09c4     	str	r0, [r10, #0x9c4]
     2a8: e3530000     	cmp	r3, #0
     2ac: 1a000133     	bne	0x780 <glopVertex+0x780> @ imm = #0x4cc
     2b0: e5943ca4     	ldr	r3, [r4, #0xca4]
     2b4: e3530000     	cmp	r3, #0
     2b8: 1a00011d     	bne	0x734 <glopVertex+0x734> @ imm = #0x474
     2bc: e3a0c08c     	mov	r12, #140
     2c0: e2843ec1     	add	r3, r4, #3088
     2c4: e02c459c     	mla	r12, r12, r5, r4
     2c8: e893000f     	ldm	r3, {r0, r1, r2, r3}
     2cc: e28cce96     	add	r12, r12, #2400
     2d0: e28cc00c     	add	r12, r12, #12
     2d4: e88c000f     	stm	r12, {r0, r1, r2, r3}
     2d8: e5943ce8     	ldr	r3, [r4, #0xce8]
     2dc: e3530000     	cmp	r3, #0
     2e0: 1a000119     	bne	0x74c <glopVertex+0x74c> @ imm = #0x464
     2e4: e3a0c08c     	mov	r12, #140
     2e8: e2843ec3     	add	r3, r4, #3120
     2ec: e02c459c     	mla	r12, r12, r5, r4
     2f0: e893000f     	ldm	r3, {r0, r1, r2, r3}
     2f4: e28cce95     	add	r12, r12, #2384
     2f8: e28cc00c     	add	r12, r12, #12
     2fc: e88c000f     	stm	r12, {r0, r1, r2, r3}
     300: e5971000     	ldr	r1, [r7]
     304: e3510000     	cmp	r1, #0
     308: 1a000118     	bne	0x770 <glopVertex+0x770> @ imm = #0x460
     30c: e3a0308c     	mov	r3, #140
     310: e2842d22     	add	r2, r4, #2176
     314: e0234593     	mla	r3, r3, r5, r4
     318: edd27a03     	vldr	s15, [r2, #12]
     31c: edd25a00     	vldr	s11, [r2]
     320: eef74a00     	vmov.f32	s9, #1.000000e+00
     324: ed9f6af4     	vldr	s12, [pc, #976]         @ 0x6fc <glopVertex+0x6fc>
     328: e2832e99     	add	r2, r3, #2448
     32c: e2833d26     	add	r3, r3, #2432
     330: ed925a02     	vldr	s10, [r2, #8]
     334: edd36a03     	vldr	s13, [r3, #12]
     338: ee847a85     	vdiv.f32	s14, s9, s10
     33c: ee676a26     	vmul.f32	s13, s14, s13
     340: ee467aa5     	vmla.f32	s15, s13, s11
     344: eef06ae7     	vabs.f32	s13, s15
     348: eef46a46     	vcmp.f32	s13, s12
     34c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     350: 83a03000     	movhi	r3, #0
     354: 8a00000a     	bhi	0x384 <glopVertex+0x384> @ imm = #0x28
     358: eddf6ae8     	vldr	s13, [pc, #928]         @ 0x700 <glopVertex+0x700>
     35c: eef47ae6     	vcmpe.f32	s15, s13
     360: eef1fa10     	vmrs	APSR_nzcv, fpscr
     364: a3e03102     	mvnge	r3, #-2147483648
     368: aa000005     	bge	0x384 <glopVertex+0x384> @ imm = #0x14
     36c: eddf6ae4     	vldr	s13, [pc, #912]         @ 0x704 <glopVertex+0x704>
     370: eef47ae6     	vcmpe.f32	s15, s13
     374: eef1fa10     	vmrs	APSR_nzcv, fpscr
     378: 9a00018d     	bls	0x9b4 <glopVertex+0x9b4> @ imm = #0x634
     37c: eefd7ae7     	vcvt.s32.f32	s15, s15
     380: ee173a90     	vmov	r3, s15
     384: e3a0208c     	mov	r2, #140
     388: ed9f6adb     	vldr	s12, [pc, #876]         @ 0x6fc <glopVertex+0x6fc>
     38c: e0224592     	mla	r2, r2, r5, r4
     390: e582399c     	str	r3, [r2, #0x99c]
     394: e2822e99     	add	r2, r2, #2448
     398: edd26a00     	vldr	s13, [r2]
     39c: e2843d22     	add	r3, r4, #2176
     3a0: edd35a01     	vldr	s11, [r3, #4]
     3a4: e2843e89     	add	r3, r4, #2192
     3a8: edd37a00     	vldr	s15, [r3]
     3ac: ee676a26     	vmul.f32	s13, s14, s13
     3b0: ee467aa5     	vmla.f32	s15, s13, s11
     3b4: eef06ae7     	vabs.f32	s13, s15
     3b8: eef46a46     	vcmp.f32	s13, s12
     3bc: eef1fa10     	vmrs	APSR_nzcv, fpscr
     3c0: 83a02000     	movhi	r2, #0
     3c4: 8a00000a     	bhi	0x3f4 <glopVertex+0x3f4> @ imm = #0x28
     3c8: eddf6acc     	vldr	s13, [pc, #816]         @ 0x700 <glopVertex+0x700>
     3cc: eef47ae6     	vcmpe.f32	s15, s13
     3d0: eef1fa10     	vmrs	APSR_nzcv, fpscr
     3d4: a3e02102     	mvnge	r2, #-2147483648
     3d8: aa000005     	bge	0x3f4 <glopVertex+0x3f4> @ imm = #0x14
     3dc: eddf6ac8     	vldr	s13, [pc, #800]         @ 0x704 <glopVertex+0x704>
     3e0: eef47ae6     	vcmpe.f32	s15, s13
     3e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
     3e8: 9a00016f     	bls	0x9ac <glopVertex+0x9ac> @ imm = #0x5bc
     3ec: eefd7ae7     	vcvt.s32.f32	s15, s15
     3f0: ee172a90     	vmov	r2, s15
     3f4: e3a0308c     	mov	r3, #140
     3f8: eddf6abf     	vldr	s13, [pc, #764]         @ 0x6fc <glopVertex+0x6fc>
     3fc: e0234593     	mla	r3, r3, r5, r4
     400: e58329a0     	str	r2, [r3, #0x9a0]
     404: e2833e99     	add	r3, r3, #2448
     408: edd37a01     	vldr	s15, [r3, #4]
     40c: e2843d22     	add	r3, r4, #2176
     410: ed936a02     	vldr	s12, [r3, #8]
     414: e2843e89     	add	r3, r4, #2192
     418: ee277a27     	vmul.f32	s14, s14, s15
     41c: edd37a01     	vldr	s15, [r3, #4]
     420: ee477a06     	vmla.f32	s15, s14, s12
     424: eeb07ae7     	vabs.f32	s14, s15
     428: eeb47a66     	vcmp.f32	s14, s13
     42c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     430: 83a02000     	movhi	r2, #0
     434: 8a00000a     	bhi	0x464 <glopVertex+0x464> @ imm = #0x28
     438: ed9f7ab0     	vldr	s14, [pc, #704]         @ 0x700 <glopVertex+0x700>
     43c: eef47ac7     	vcmpe.f32	s15, s14
     440: eef1fa10     	vmrs	APSR_nzcv, fpscr
     444: a3e02102     	mvnge	r2, #-2147483648
     448: aa000005     	bge	0x464 <glopVertex+0x464> @ imm = #0x14
     44c: ed9f7aac     	vldr	s14, [pc, #688]         @ 0x704 <glopVertex+0x704>
     450: eef47ac7     	vcmpe.f32	s15, s14
     454: eef1fa10     	vmrs	APSR_nzcv, fpscr
     458: 9a000151     	bls	0x9a4 <glopVertex+0x9a4> @ imm = #0x544
     45c: eefd7ae7     	vcvt.s32.f32	s15, s15
     460: ee172a90     	vmov	r2, s15
     464: e3a0308c     	mov	r3, #140
     468: eddf7aa6     	vldr	s15, [pc, #664]         @ 0x708 <glopVertex+0x708>
     46c: e0234593     	mla	r3, r3, r5, r4
     470: ed9f6aa5     	vldr	s12, [pc, #660]         @ 0x70c <glopVertex+0x70c>
     474: eef06a67     	vmov.f32	s13, s15
     478: e58329a4     	str	r2, [r3, #0x9a4]
     47c: e2832e96     	add	r2, r3, #2400
     480: ed927a03     	vldr	s14, [r2, #12]
     484: e2832e97     	add	r2, r3, #2416
     488: edd25a01     	vldr	s11, [r2, #4]
     48c: ee476a06     	vmla.f32	s13, s14, s12
     490: eeb07a67     	vmov.f32	s14, s15
     494: ee057a86     	vmla.f32	s14, s11, s12
     498: edd25a00     	vldr	s11, [r2]
     49c: eefd6ae6     	vcvt.s32.f32	s13, s13
     4a0: ee457a86     	vmla.f32	s15, s11, s12
     4a4: eebd7ac7     	vcvt.s32.f32	s14, s14
     4a8: ee162a90     	vmov	r2, s13
     4ac: e3c224ff     	bic	r2, r2, #-16777216
     4b0: e58329b0     	str	r2, [r3, #0x9b0]
     4b4: ee172a10     	vmov	r2, s14
     4b8: e3c224ff     	bic	r2, r2, #-16777216
     4bc: eefd7ae7     	vcvt.s32.f32	s15, s15
     4c0: e58329b8     	str	r2, [r3, #0x9b8]
     4c4: ee172a90     	vmov	r2, s15
     4c8: e3c224ff     	bic	r2, r2, #-16777216
     4cc: e58329b4     	str	r2, [r3, #0x9b4]
     4d0: e5942cb8     	ldr	r2, [r4, #0xcb8]
     4d4: e3520000     	cmp	r2, #0
     4d8: 0a00000f     	beq	0x51c <glopVertex+0x51c> @ imm = #0x3c
     4dc: e2832e95     	add	r2, r3, #2384
     4e0: ed9f6a8a     	vldr	s12, [pc, #552]         @ 0x710 <glopVertex+0x710>
     4e4: edd25a03     	vldr	s11, [r2, #12]
     4e8: ed9f7a89     	vldr	s14, [pc, #548]         @ 0x714 <glopVertex+0x714>
     4ec: eddf6a89     	vldr	s13, [pc, #548]         @ 0x718 <glopVertex+0x718>
     4f0: eddf7a89     	vldr	s15, [pc, #548]         @ 0x71c <glopVertex+0x71c>
     4f4: ee057a86     	vmla.f32	s14, s11, s12
     4f8: eebd7ac7     	vcvt.s32.f32	s14, s14
     4fc: ee172a10     	vmov	r2, s14
     500: e58329a8     	str	r2, [r3, #0x9a8]
     504: e2832e96     	add	r2, r3, #2400
     508: ed927a00     	vldr	s14, [r2]
     50c: ee477a26     	vmla.f32	s15, s14, s13
     510: eefd7ae7     	vcvt.s32.f32	s15, s15
     514: ee172a90     	vmov	r2, s15
     518: e58329ac     	str	r2, [r3, #0x9ac]
     51c: e3510000     	cmp	r1, #0
     520: 1a000098     	bne	0x788 <glopVertex+0x788> @ imm = #0x260
     524: e3a0308c     	mov	r3, #140
     528: e0254593     	mla	r5, r3, r5, r4
     52c: e5943d14     	ldr	r3, [r4, #0xd14]
     530: e58539c8     	str	r3, [r5, #0x9c8]
     534: e59f31ec     	ldr	r3, [pc, #0x1ec]        @ 0x728 <glopVertex+0x728>
     538: e5942d1c     	ldr	r2, [r4, #0xd1c]
     53c: e08f3003     	add	r3, pc, r3
     540: e3520008     	cmp	r2, #8
     544: 8a000005     	bhi	0x560 <glopVertex+0x560> @ imm = #0x14
     548: e7d32002     	ldrb	r2, [r3, r2]
     54c: e08ff102     	add	pc, pc, r2, lsl #2
     550: e320f000     	nop
     554: e2840d25     	add	r0, r4, #2368
     558: ebfffffe     	bl	0x558 <glopVertex+0x558> @ imm = #-0x8
     55c: e3a09000     	mov	r9, #0
     560: e5849d20     	str	r9, [r4, #0xd20]
     564: e28dd00c     	add	sp, sp, #12
     568: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     56c: e59f31b8     	ldr	r3, [pc, #0x1b8]        @ 0x72c <glopVertex+0x72c>
     570: e79b3003     	ldr	r3, [r11, r3]
     574: e5933000     	ldr	r3, [r3]
     578: e3530000     	cmp	r3, #0
     57c: 0a000020     	beq	0x604 <glopVertex+0x604> @ imm = #0x80
     580: f2804010     	vmov.i32	d4, #0x0
     584: e28a3d26     	add	r3, r10, #2432
     588: ee161a10     	vmov	r1, s12
     58c: e283300c     	add	r3, r3, #12
     590: f2245114     	vorr	d5, d4, d4
     594: f2247114     	vorr	d7, d4, d4
     598: ee041b10     	vmov.32	d4[0], r1
     59c: e59f118c     	ldr	r1, [pc, #0x18c]        @ 0x730 <glopVertex+0x730>
     5a0: e08f1001     	add	r1, pc, r1
     5a4: f4610adf     	vld1.64	{d16, d17}, [r1:64]
     5a8: f3e009c4     	vmul.f32	q8, q8, d4[0]
     5ac: edd16b04     	vldr	d22, [r1, #16]
     5b0: edd17b06     	vldr	d23, [r1, #24]
     5b4: edd14b08     	vldr	d20, [r1, #32]
     5b8: edd15b0a     	vldr	d21, [r1, #40]
     5bc: edd12b0c     	vldr	d18, [r1, #48]
     5c0: edd13b0e     	vldr	d19, [r1, #56]
     5c4: e2821e95     	add	r1, r2, #2384
     5c8: e2822e99     	add	r2, r2, #2448
     5cc: f4a1580f     	vld1.32	{d5[0]}, [r1]
     5d0: e2811004     	add	r1, r1, #4
     5d4: f4a1780f     	vld1.32	{d7[0]}, [r1]
     5d8: f3e601c5     	vmla.f32	q8, q11, d5[0]
     5dc: f3e401c7     	vmla.f32	q8, q10, d7[0]
     5e0: f2400de2     	vadd.f32	q8, q8, q9
     5e4: f4430a8f     	vst1.32	{d16, d17}, [r3]
     5e8: e5943ce4     	ldr	r3, [r4, #0xce4]
     5ec: e3530000     	cmp	r3, #0
     5f0: 12841ebe     	addne	r1, r4, #3040
     5f4: 05923008     	ldreq	r3, [r2, #0x8]
     5f8: 1591300c     	ldrne	r3, [r1, #0xc]
     5fc: 15823008     	strne	r3, [r2, #0x8]
     600: eaffff1d     	b	0x27c <glopVertex+0x27c> @ imm = #-0x38c
     604: e2843ebb     	add	r3, r4, #2992
     608: e2821e99     	add	r1, r2, #2448
     60c: edd37a01     	vldr	s15, [r3, #4]
     610: e2823e95     	add	r3, r2, #2384
     614: ed937a00     	vldr	s14, [r3]
     618: e2843ebb     	add	r3, r4, #2992
     61c: edd36a00     	vldr	s13, [r3]
     620: e2823e95     	add	r3, r2, #2384
     624: ee677a27     	vmul.f32	s15, s14, s15
     628: ee467a26     	vmla.f32	s15, s12, s13
     62c: edd36a01     	vldr	s13, [r3, #4]
     630: e2843ebb     	add	r3, r4, #2992
     634: edd35a02     	vldr	s11, [r3, #8]
     638: ee467aa5     	vmla.f32	s15, s13, s11
     63c: edd35a03     	vldr	s11, [r3, #12]
     640: e2823d26     	add	r3, r2, #2432
     644: ee777aa5     	vadd.f32	s15, s15, s11
     648: edc37a03     	vstr	s15, [r3, #12]
     64c: e2843d2f     	add	r3, r4, #3008
     650: edd37a01     	vldr	s15, [r3, #4]
     654: edd35a00     	vldr	s11, [r3]
     658: ee677a27     	vmul.f32	s15, s14, s15
     65c: ee467a25     	vmla.f32	s15, s12, s11
     660: edd35a02     	vldr	s11, [r3, #8]
     664: ee467aa5     	vmla.f32	s15, s13, s11
     668: edd35a03     	vldr	s11, [r3, #12]
     66c: e2843ebd     	add	r3, r4, #3024
     670: ee777aa5     	vadd.f32	s15, s15, s11
     674: edc17a00     	vstr	s15, [r1]
     678: edd37a01     	vldr	s15, [r3, #4]
     67c: e2843ebd     	add	r3, r4, #3024
     680: edd35a00     	vldr	s11, [r3]
     684: ee677a27     	vmul.f32	s15, s14, s15
     688: ee467a25     	vmla.f32	s15, s12, s11
     68c: edd35a02     	vldr	s11, [r3, #8]
     690: ee467aa5     	vmla.f32	s15, s13, s11
     694: edd35a03     	vldr	s11, [r3, #12]
     698: e5943ce4     	ldr	r3, [r4, #0xce4]
     69c: e3530000     	cmp	r3, #0
     6a0: e2843ebe     	add	r3, r4, #3040
     6a4: e283300c     	add	r3, r3, #12
     6a8: ee777aa5     	vadd.f32	s15, s15, s11
     6ac: edc17a01     	vstr	s15, [r1, #4]
     6b0: e5933000     	ldr	r3, [r3]
     6b4: 15813008     	strne	r3, [r1, #0x8]
     6b8: 1afffeef     	bne	0x27c <glopVertex+0x27c> @ imm = #-0x444
     6bc: e2841ebe     	add	r1, r4, #3040
     6c0: e2822e99     	add	r2, r2, #2448
     6c4: edd17a01     	vldr	s15, [r1, #4]
     6c8: ee277a27     	vmul.f32	s14, s14, s15
     6cc: edd17a00     	vldr	s15, [r1]
     6d0: ee067a27     	vmla.f32	s14, s12, s15
     6d4: edd17a02     	vldr	s15, [r1, #8]
     6d8: ee067aa7     	vmla.f32	s14, s13, s15
     6dc: ee073a90     	vmov	s15, r3
     6e0: ee777a87     	vadd.f32	s15, s15, s14
     6e4: ee173a90     	vmov	r3, s15
     6e8: edc27a02     	vstr	s15, [r2, #8]
     6ec: eafffee2     	b	0x27c <glopVertex+0x27c> @ imm = #-0x478
     6f0: e3a00000     	mov	r0, #0
     6f4: ebfffffe     	bl	0x6f4 <glopVertex+0x6f4> @ imm = #-0x8
     6f8: eafffe5e     	b	0x78 <glopVertex+0x78>  @ imm = #-0x688
     6fc: ff ff 7f 7f  	.word	0x7f7fffff
     700: 00 00 00 4f  	.word	0x4f000000
     704: 00 00 00 cf  	.word	0xcf000000
     708: 00 ff 7f 47  	.word	0x477fff00
     70c: 00 00 7e 4b  	.word	0x4b7e0000
     710: 00 00 ff 48  	.word	0x48ff0000
     714: 00 00 80 44  	.word	0x44800000
     718: 00 00 ff 4c  	.word	0x4cff0000
     71c: 00 00 80 48  	.word	0x48800000
     720: c0 06 00 00  	.word	0x000006c0
     724: 00 00 00 00  	.word	0x00000000
     728: e4 01 00 00  	.word	0x000001e4
     72c: 00 00 00 00  	.word	0x00000000
     730: 88 01 00 00  	.word	0x00000188
     734: e2860d25     	add	r0, r6, #2368
     738: e0840000     	add	r0, r4, r0
     73c: ebfffffe     	bl	0x73c <glopVertex+0x73c> @ imm = #-0x8
     740: e5943ce8     	ldr	r3, [r4, #0xce8]
     744: e3530000     	cmp	r3, #0
     748: 0afffee5     	beq	0x2e4 <glopVertex+0x2e4> @ imm = #-0x46c
     74c: e2860e95     	add	r0, r6, #2384
     750: e5941c80     	ldr	r1, [r4, #0xc80]
     754: e280000c     	add	r0, r0, #12
     758: e2842ec3     	add	r2, r4, #3120
     75c: e0840000     	add	r0, r4, r0
     760: ebfffffe     	bl	0x760 <glopVertex+0x760> @ imm = #-0x8
     764: e5971000     	ldr	r1, [r7]
     768: e3510000     	cmp	r1, #0
     76c: 0afffee6     	beq	0x30c <glopVertex+0x30c> @ imm = #-0x468
     770: e3a00000     	mov	r0, #0
     774: ebfffffe     	bl	0x774 <glopVertex+0x774> @ imm = #-0x8
     778: e5971000     	ldr	r1, [r7]
     77c: eafffee2     	b	0x30c <glopVertex+0x30c> @ imm = #-0x478
     780: ebfffffe     	bl	0x780 <glopVertex+0x780> @ imm = #-0x8
     784: eafffec9     	b	0x2b0 <glopVertex+0x2b0> @ imm = #-0x4dc
     788: ebfffffe     	bl	0x788 <glopVertex+0x788> @ imm = #-0x8
     78c: eaffff64     	b	0x524 <glopVertex+0x524> @ imm = #-0x270
     790: ee276a07     	vmul.f32	s12, s14, s14
     794: ee066aa6     	vmla.f32	s12, s13, s13
     798: ee076aa7     	vmla.f32	s12, s15, s15
     79c: eef70ac6     	vcvt.f64.f32	d16, s12
     7a0: eef10be0     	vsqrt.f64	d16, d16
     7a4: eeb76be0     	vcvt.f32.f64	s12, d16
     7a8: eeb56a40     	vcmp.f32	s12, #0
     7ac: eef1fa10     	vmrs	APSR_nzcv, fpscr
     7b0: 0afffeb1     	beq	0x27c <glopVertex+0x27c> @ imm = #-0x53c
     7b4: eeb75a00     	vmov.f32	s10, #1.000000e+00
     7b8: eec55a06     	vdiv.f32	s11, s10, s12
     7bc: ee666aa5     	vmul.f32	s13, s13, s11
     7c0: ee277a25     	vmul.f32	s14, s14, s11
     7c4: ee677aa5     	vmul.f32	s15, s15, s11
     7c8: edcc6a00     	vstr	s13, [r12]
     7cc: ed817a00     	vstr	s14, [r1]
     7d0: edc27a00     	vstr	s15, [r2]
     7d4: eafffea8     	b	0x27c <glopVertex+0x27c> @ imm = #-0x560
     7d8: e3590002     	cmp	r9, #2
     7dc: 1affff5f     	bne	0x560 <glopVertex+0x560> @ imm = #-0x284
     7e0: e2841d27     	add	r1, r4, #2496
     7e4: e2840d25     	add	r0, r4, #2368
     7e8: e281100c     	add	r1, r1, #12
     7ec: ebfffffe     	bl	0x7ec <glopVertex+0x7ec> @ imm = #-0x8
     7f0: eaffff59     	b	0x55c <glopVertex+0x55c> @ imm = #-0x29c
     7f4: e3590001     	cmp	r9, #1
     7f8: 0a000063     	beq	0x98c <glopVertex+0x98c> @ imm = #0x18c
     7fc: e3590002     	cmp	r9, #2
     800: 1affff56     	bne	0x560 <glopVertex+0x560> @ imm = #-0x2a8
     804: e2841d27     	add	r1, r4, #2496
     808: e2840d25     	add	r0, r4, #2368
     80c: e281100c     	add	r1, r1, #12
     810: ebfffffe     	bl	0x810 <glopVertex+0x810> @ imm = #-0x8
     814: e2841d27     	add	r1, r4, #2496
     818: e3a0208c     	mov	r2, #140
     81c: e281100c     	add	r1, r1, #12
     820: e2840d25     	add	r0, r4, #2368
     824: ebfffffe     	bl	0x824 <glopVertex+0x824> @ imm = #-0x8
     828: e3a09001     	mov	r9, #1
     82c: eaffff4b     	b	0x560 <glopVertex+0x560> @ imm = #-0x2d4
     830: e3590003     	cmp	r9, #3
     834: 1affff49     	bne	0x560 <glopVertex+0x560> @ imm = #-0x2dc
     838: e2842ea5     	add	r2, r4, #2640
     83c: e2841d27     	add	r1, r4, #2496
     840: e2822008     	add	r2, r2, #8
     844: e281100c     	add	r1, r1, #12
     848: e2840d25     	add	r0, r4, #2368
     84c: ebfffffe     	bl	0x84c <glopVertex+0x84c> @ imm = #-0x8
     850: eaffff41     	b	0x55c <glopVertex+0x55c> @ imm = #-0x2fc
     854: e3580002     	cmp	r8, #2
     858: daffff40     	ble	0x560 <glopVertex+0x560> @ imm = #-0x300
     85c: e3590003     	cmp	r9, #3
     860: 03a09000     	moveq	r9, #0
     864: e3180001     	tst	r8, #1
     868: 02841d27     	addeq	r1, r4, #2496
     86c: 02840ea5     	addeq	r0, r4, #2640
     870: 12842ea5     	addne	r2, r4, #2640
     874: 12841d27     	addne	r1, r4, #2496
     878: 02842d25     	addeq	r2, r4, #2368
     87c: 0281100c     	addeq	r1, r1, #12
     880: 02800008     	addeq	r0, r0, #8
     884: 12822008     	addne	r2, r2, #8
     888: 1281100c     	addne	r1, r1, #12
     88c: 12840d25     	addne	r0, r4, #2368
     890: ebfffffe     	bl	0x890 <glopVertex+0x890> @ imm = #-0x8
     894: eaffff31     	b	0x560 <glopVertex+0x560> @ imm = #-0x33c
     898: e3590003     	cmp	r9, #3
     89c: 1affff2f     	bne	0x560 <glopVertex+0x560> @ imm = #-0x344
     8a0: e2842ea5     	add	r2, r4, #2640
     8a4: e2841d27     	add	r1, r4, #2496
     8a8: e2840d25     	add	r0, r4, #2368
     8ac: e2822008     	add	r2, r2, #8
     8b0: e281100c     	add	r1, r1, #12
     8b4: ebfffffe     	bl	0x8b4 <glopVertex+0x8b4> @ imm = #-0x8
     8b8: e2841ea5     	add	r1, r4, #2640
     8bc: e2840d27     	add	r0, r4, #2496
     8c0: e3a0208c     	mov	r2, #140
     8c4: e2811008     	add	r1, r1, #8
     8c8: e280000c     	add	r0, r0, #12
     8cc: ebfffffe     	bl	0x8cc <glopVertex+0x8cc> @ imm = #-0x8
     8d0: ea00002b     	b	0x984 <glopVertex+0x984> @ imm = #0xac
     8d4: e3590004     	cmp	r9, #4
     8d8: 1affff20     	bne	0x560 <glopVertex+0x560> @ imm = #-0x380
     8dc: e3a07000     	mov	r7, #0
     8e0: e2842ea5     	add	r2, r4, #2640
     8e4: e2841d27     	add	r1, r4, #2496
     8e8: e2840d25     	add	r0, r4, #2368
     8ec: e2822008     	add	r2, r2, #8
     8f0: e281100c     	add	r1, r1, #12
     8f4: e5847ae0     	str	r7, [r4, #0xae0]
     8f8: ebfffffe     	bl	0x8f8 <glopVertex+0x8f8> @ imm = #-0x8
     8fc: e3a03001     	mov	r3, #1
     900: e2841ea5     	add	r1, r4, #2640
     904: e2842eae     	add	r2, r4, #2784
     908: e2811008     	add	r1, r1, #8
     90c: e2840d25     	add	r0, r4, #2368
     910: e2822004     	add	r2, r2, #4
     914: e5843ae0     	str	r3, [r4, #0xae0]
     918: e58479c8     	str	r7, [r4, #0x9c8]
     91c: ebfffffe     	bl	0x91c <glopVertex+0x91c> @ imm = #-0x8
     920: eaffff0d     	b	0x55c <glopVertex+0x55c> @ imm = #-0x3cc
     924: e3590004     	cmp	r9, #4
     928: 1affff0c     	bne	0x560 <glopVertex+0x560> @ imm = #-0x3d0
     92c: e2845d27     	add	r5, r4, #2496
     930: e2846ea5     	add	r6, r4, #2640
     934: e285500c     	add	r5, r5, #12
     938: e2866008     	add	r6, r6, #8
     93c: e1a02006     	mov	r2, r6
     940: e1a01005     	mov	r1, r5
     944: e2840d25     	add	r0, r4, #2368
     948: ebfffffe     	bl	0x948 <glopVertex+0x948> @ imm = #-0x8
     94c: e2841eae     	add	r1, r4, #2784
     950: e1a02006     	mov	r2, r6
     954: e2811004     	add	r1, r1, #4
     958: e1a00005     	mov	r0, r5
     95c: ebfffffe     	bl	0x95c <glopVertex+0x95c> @ imm = #-0x8
     960: e1a01006     	mov	r1, r6
     964: e3a0208c     	mov	r2, #140
     968: e2840d25     	add	r0, r4, #2368
     96c: ebfffffe     	bl	0x96c <glopVertex+0x96c> @ imm = #-0x8
     970: e2841eae     	add	r1, r4, #2784
     974: e3a0208c     	mov	r2, #140
     978: e2811004     	add	r1, r1, #4
     97c: e1a00005     	mov	r0, r5
     980: ebfffffe     	bl	0x980 <glopVertex+0x980> @ imm = #-0x8
     984: e3a09002     	mov	r9, #2
     988: eafffef4     	b	0x560 <glopVertex+0x560> @ imm = #-0x430
     98c: e2840ea5     	add	r0, r4, #2640
     990: e3a0208c     	mov	r2, #140
     994: e2841d25     	add	r1, r4, #2368
     998: e2800008     	add	r0, r0, #8
     99c: ebfffffe     	bl	0x99c <glopVertex+0x99c> @ imm = #-0x8
     9a0: eaffffa0     	b	0x828 <glopVertex+0x828> @ imm = #-0x180
     9a4: e3a02102     	mov	r2, #-2147483648
     9a8: eafffead     	b	0x464 <glopVertex+0x464> @ imm = #-0x54c
     9ac: e3a02102     	mov	r2, #-2147483648
     9b0: eafffe8f     	b	0x3f4 <glopVertex+0x3f4> @ imm = #-0x5c4
     9b4: e3a03102     	mov	r3, #-2147483648
     9b8: eafffe71     	b	0x384 <glopVertex+0x384> @ imm = #-0x63c

Disassembly of section .text.glopEnd:

00000000 <glopEnd>:
       0: e92d4010     	push	{r4, lr}
       4: ebfffffe     	bl	0x4 <glopEnd+0x4>       @ imm = #-0x8
       8: e3a03000     	mov	r3, #0
       c: e5803d18     	str	r3, [r0, #0xd18]
      10: e8bd8010     	pop	{r4, pc}


.context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-pic-vfp-matrix-bench-tinygl-internal/85-pixels_neon.arm.o:	file format elf32-littlearm

Disassembly of section .text.pc_neon_black:

00000000 <pc_neon_black>:
       0: e3510003     	cmp	r1, #3
       4: e52de004     	str	lr, [sp, #-0x4]!
       8: 9a00000f     	bls	0x4c <pc_neon_black+0x4c> @ imm = #0x3c
       c: f2c00050     	vmov.i32	q8, #0x0
      10: e1a0c000     	mov	r12, r0
      14: e3a03004     	mov	r3, #4
      18: e1a0e003     	mov	lr, r3
      1c: e2833004     	add	r3, r3, #4
      20: e1510003     	cmp	r1, r3
      24: f44c0a8d     	vst1.32	{d16, d17}, [r12]!
      28: 2afffffa     	bhs	0x18 <pc_neon_black+0x18> @ imm = #-0x18
      2c: e151000e     	cmp	r1, lr
      30: 949df004     	ldrls	pc, [sp], #4
      34: e041200e     	sub	r2, r1, lr
      38: e080010e     	add	r0, r0, lr, lsl #2
      3c: e1a02102     	lsl	r2, r2, #2
      40: e49de004     	ldr	lr, [sp], #4
      44: e3a01000     	mov	r1, #0
      48: eafffffe     	b	0x48 <pc_neon_black+0x48> @ imm = #-0x8
      4c: e3a0e000     	mov	lr, #0
      50: e151000e     	cmp	r1, lr
      54: 8afffff6     	bhi	0x34 <pc_neon_black+0x34> @ imm = #-0x28
      58: e49df004     	ldr	pc, [sp], #4

Disassembly of section .text.pc_neon_capture:

00000000 <pc_neon_capture>:
       0: e3530010     	cmp	r3, #16
       4: e92d40f0     	push	{r4, r5, r6, r7, lr}
       8: 0a00001e     	beq	0x88 <pc_neon_capture+0x88> @ imm = #0x78
       c: e3520003     	cmp	r2, #3
      10: 93a04000     	movls	r4, #0
      14: 9a000008     	bls	0x3c <pc_neon_capture+0x3c> @ imm = #0x20
      18: e1a0e001     	mov	lr, r1
      1c: e1a0c000     	mov	r12, r0
      20: e3a03004     	mov	r3, #4
      24: e1a04003     	mov	r4, r3
      28: e2833004     	add	r3, r3, #4
      2c: f46e0a0d     	vld1.8	{d16, d17}, [lr]!
      30: e1520003     	cmp	r2, r3
      34: f44c0a8d     	vst1.32	{d16, d17}, [r12]!
      38: 2afffff9     	bhs	0x24 <pc_neon_capture+0x24> @ imm = #-0x1c
      3c: e1520004     	cmp	r2, r4
      40: 98bd80f0     	popls	{r4, r5, r6, r7, pc}
      44: e1a04104     	lsl	r4, r4, #2
      48: e0812102     	add	r2, r1, r2, lsl #2
      4c: e0800004     	add	r0, r0, r4
      50: e0811004     	add	r1, r1, r4
      54: e5d13002     	ldrb	r3, [r1, #0x2]
      58: e2811004     	add	r1, r1, #4
      5c: e5514003     	ldrb	r4, [r1, #-0x3]
      60: e551e004     	ldrb	lr, [r1, #-0x4]
      64: e551c001     	ldrb	r12, [r1, #-0x1]
      68: e1520001     	cmp	r2, r1
      6c: e1a03803     	lsl	r3, r3, #16
      70: e1833404     	orr	r3, r3, r4, lsl #8
      74: e183300e     	orr	r3, r3, lr
      78: e1833c0c     	orr	r3, r3, r12, lsl #24
      7c: e4803004     	str	r3, [r0], #4
      80: 1afffff3     	bne	0x54 <pc_neon_capture+0x54> @ imm = #-0x34
      84: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
      88: e3520007     	cmp	r2, #7
      8c: 9a000040     	bls	0x194 <pc_neon_capture+0x194> @ imm = #0x100
      90: f2c3685f     	vmov.i16	q11, #0x3f
      94: e1a05001     	mov	r5, r1
      98: f2c1485f     	vmov.i16	q10, #0x1f
      9c: e1a04000     	mov	r4, r0
      a0: e3a03008     	mov	r3, #8
      a4: f4650a0d     	vld1.8	{d16, d17}, [r5]!
      a8: e1a0c003     	mov	r12, r3
      ac: f240a1f4     	vand	q13, q8, q10
      b0: e2833008     	add	r3, r3, #8
      b4: f3db2070     	vshr.u16	q9, q8, #0x5
      b8: e1a0e004     	mov	lr, r4
      bc: f3d50070     	vshr.u16	q8, q8, #0xb
      c0: e1520003     	cmp	r2, r3
      c4: f24221f6     	vand	q9, q9, q11
      c8: e2844020     	add	r4, r4, #32
      cc: f2d3c57a     	vshl.i16	q14, q13, #0x3
      d0: f2d3e570     	vshl.i16	q15, q8, #0x3
      d4: f2d28572     	vshl.i16	q12, q9, #0x2
      d8: f3de0070     	vshr.u16	q8, q8, #0x2
      dc: f3dc2072     	vshr.u16	q9, q9, #0x4
      e0: f26001fe     	vorr	q8, q8, q15
      e4: f26221f8     	vorr	q9, q9, q12
      e8: f3dea07a     	vshr.u16	q13, q13, #0x2
      ec: f3d0ea30     	vmovl.u16	q15, d16
      f0: f26aa1fc     	vorr	q13, q13, q14
      f4: f3d08a32     	vmovl.u16	q12, d18
      f8: f3d0ca31     	vmovl.u16	q14, d17
      fc: f3d00a33     	vmovl.u16	q8, d19
     100: f2f0e57e     	vshl.i32	q15, q15, #0x10
     104: f2f0c57c     	vshl.i32	q14, q14, #0x10
     108: f2e82578     	vshl.i32	q9, q12, #0x8
     10c: f2e80570     	vshl.i32	q8, q8, #0x8
     110: f3904a3a     	vmovl.u16	q2, d26
     114: f3906a3b     	vmovl.u16	q3, d27
     118: f26221fe     	vorr	q9, q9, q15
     11c: f26001fc     	vorr	q8, q8, q14
     120: f26221d4     	vorr	q9, q9, q2
     124: f26001d6     	vorr	q8, q8, q3
     128: f44e2a8d     	vst1.32	{d18, d19}, [lr]!
     12c: f44e0a8f     	vst1.32	{d16, d17}, [lr]
     130: 2affffdb     	bhs	0xa4 <pc_neon_capture+0xa4> @ imm = #-0x94
     134: e152000c     	cmp	r2, r12
     138: 98bd80f0     	popls	{r4, r5, r6, r7, pc}
     13c: e240e004     	sub	lr, r0, #4
     140: e2815001     	add	r5, r1, #1
     144: e08ee10c     	add	lr, lr, r12, lsl #2
     148: e7d5008c     	ldrb	r0, [r5, r12, lsl #1]
     14c: e7d1308c     	ldrb	r3, [r1, r12, lsl #1]
     150: e28cc001     	add	r12, r12, #1
     154: e152000c     	cmp	r2, r12
     158: e1833400     	orr	r3, r3, r0, lsl #8
     15c: e203401f     	and	r4, r3, #31
     160: e1a075a3     	lsr	r7, r3, #11
     164: e1a066a3     	lsr	r6, r3, #13
     168: e1a00124     	lsr	r0, r4, #2
     16c: e7e532d3     	ubfx	r3, r3, #0x5, #0x6
     170: e1800184     	orr	r0, r0, r4, lsl #3
     174: e1866187     	orr	r6, r6, r7, lsl #3
     178: e1a04223     	lsr	r4, r3, #4
     17c: e1800806     	orr	r0, r0, r6, lsl #16
     180: e1843103     	orr	r3, r4, r3, lsl #2
     184: e1803403     	orr	r3, r0, r3, lsl #8
     188: e5ae3004     	str	r3, [lr, #0x4]!
     18c: 1affffed     	bne	0x148 <pc_neon_capture+0x148> @ imm = #-0x4c
     190: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
     194: e3a0c000     	mov	r12, #0
     198: eaffffe5     	b	0x134 <pc_neon_capture+0x134> @ imm = #-0x6c

Disassembly of section .text.pc_neon_scale:

00000000 <pc_neon_scale>:
       0: e3520003     	cmp	r2, #3
       4: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
       8: e59d5018     	ldr	r5, [sp, #0x18]
       c: e1a07000     	mov	r7, r0
      10: e1a04002     	mov	r4, r2
      14: e1a00082     	lsl	r0, r2, #1
      18: e1a08001     	mov	r8, r1
      1c: e1a06003     	mov	r6, r3
      20: 9a000019     	bls	0x8c <pc_neon_scale+0x8c> @ imm = #0x64
      24: e1a0e001     	mov	lr, r1
      28: e1a02007     	mov	r2, r7
      2c: e1a01007     	mov	r1, r7
      30: e3a0c004     	mov	r12, #4
      34: ea000007     	b	0x58 <pc_neon_scale+0x58> @ imm = #0x1c
      38: f4420a8f     	vst1.32	{d16, d17}, [r2]
      3c: e28c3004     	add	r3, r12, #4
      40: e28ee010     	add	lr, lr, #16
      44: e1540003     	cmp	r4, r3
      48: e2822010     	add	r2, r2, #16
      4c: e2811020     	add	r1, r1, #32
      50: 3a00000e     	blo	0x90 <pc_neon_scale+0x90> @ imm = #0x38
      54: e1a0c003     	mov	r12, r3
      58: f46e2a8f     	vld1.32	{d18, d19}, [lr]
      5c: e3560003     	cmp	r6, #3
      60: f3f000e2     	vrev32.8	q8, q9
      64: 1a000000     	bne	0x6c <pc_neon_scale+0x6c> @ imm = #0x0
      68: f26201f2     	vorr	q8, q9, q9
      6c: e1550000     	cmp	r5, r0
      70: 1afffff0     	bne	0x38 <pc_neon_scale+0x38> @ imm = #-0x40
      74: f26041f0     	vorr	q10, q8, q8
      78: e1a03001     	mov	r3, r1
      7c: f3fa41e0     	vzip.32	q10, q8
      80: f4434a8d     	vst1.32	{d20, d21}, [r3]!
      84: f4430a8f     	vst1.32	{d16, d17}, [r3]
      88: eaffffeb     	b	0x3c <pc_neon_scale+0x3c> @ imm = #-0x54
      8c: e3a0c000     	mov	r12, #0
      90: e154000c     	cmp	r4, r12
      94: 98bd81f0     	popls	{r4, r5, r6, r7, r8, pc}
      98: e3560003     	cmp	r6, #3
      9c: e798310c     	ldr	r3, [r8, r12, lsl #2]
      a0: e2872004     	add	r2, r7, #4
      a4: 1a000008     	bne	0xcc <pc_neon_scale+0xcc> @ imm = #0x20
      a8: e1550000     	cmp	r5, r0
      ac: 1787310c     	strne	r3, [r7, r12, lsl #2]
      b0: 0a000010     	beq	0xf8 <pc_neon_scale+0xf8> @ imm = #0x40
      b4: e28cc001     	add	r12, r12, #1
      b8: e154000c     	cmp	r4, r12
      bc: 08bd81f0     	popeq	{r4, r5, r6, r7, r8, pc}
      c0: e3560003     	cmp	r6, #3
      c4: e798310c     	ldr	r3, [r8, r12, lsl #2]
      c8: 0afffff6     	beq	0xa8 <pc_neon_scale+0xa8> @ imm = #-0x28
      cc: e1550000     	cmp	r5, r0
      d0: e6bf3f33     	rev	r3, r3
      d4: 0a000007     	beq	0xf8 <pc_neon_scale+0xf8> @ imm = #0x1c
      d8: e787310c     	str	r3, [r7, r12, lsl #2]
      dc: e28cc001     	add	r12, r12, #1
      e0: e154000c     	cmp	r4, r12
      e4: 08bd81f0     	popeq	{r4, r5, r6, r7, r8, pc}
      e8: e798310c     	ldr	r3, [r8, r12, lsl #2]
      ec: e1550000     	cmp	r5, r0
      f0: e6bf3f33     	rev	r3, r3
      f4: 1afffff7     	bne	0xd8 <pc_neon_scale+0xd8> @ imm = #-0x24
      f8: e787318c     	str	r3, [r7, r12, lsl #3]
      fc: e782318c     	str	r3, [r2, r12, lsl #3]
     100: eaffffeb     	b	0xb4 <pc_neon_scale+0xb4> @ imm = #-0x54

Disassembly of section .text.pc_neon_scale_dim:

00000000 <pc_neon_scale_dim>:
       0: e3520003     	cmp	r2, #3
       4: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
       8: e1a04002     	mov	r4, r2
       c: e1a07000     	mov	r7, r0
      10: e1a08001     	mov	r8, r1
      14: e1a00003     	mov	r0, r3
      18: e59d5024     	ldr	r5, [sp, #0x24]
      1c: e1a01084     	lsl	r1, r4, #1
      20: e59d2028     	ldr	r2, [sp, #0x28]
      24: 9a000021     	bls	0xb0 <pc_neon_scale_dim+0xb0> @ imm = #0x84
      28: eec62b90     	vdup.8	d22, r2
      2c: e1a0e008     	mov	lr, r8
      30: e1a06007     	mov	r6, r7
      34: e1a09007     	mov	r9, r7
      38: e3a0c004     	mov	r12, #4
      3c: ea000007     	b	0x60 <pc_neon_scale_dim+0x60> @ imm = #0x1c
      40: f4460a8f     	vst1.32	{d16, d17}, [r6]
      44: e28c3004     	add	r3, r12, #4
      48: e28ee010     	add	lr, lr, #16
      4c: e1540003     	cmp	r4, r3
      50: e2866010     	add	r6, r6, #16
      54: e2899020     	add	r9, r9, #32
      58: 3a000015     	blo	0xb4 <pc_neon_scale_dim+0xb4> @ imm = #0x54
      5c: e1a0c003     	mov	r12, r3
      60: e3520008     	cmp	r2, #8
      64: f46e0a8f     	vld1.32	{d16, d17}, [lr]
      68: f3c04ca6     	vmull.u8	q10, d16, d22
      6c: f3c12ca6     	vmull.u8	q9, d17, d22
      70: 0a000003     	beq	0x84 <pc_neon_scale_dim+0x84> @ imm = #0xc
      74: f2cd4834     	vshrn.i16	d20, q10, #0x3
      78: f2cd2832     	vshrn.i16	d18, q9, #0x3
      7c: f26401b4     	vorr	d16, d20, d20
      80: f26211b2     	vorr	d17, d18, d18
      84: e3500003     	cmp	r0, #3
      88: 0a000000     	beq	0x90 <pc_neon_scale_dim+0x90> @ imm = #0x0
      8c: f3f000e0     	vrev32.8	q8, q8
      90: e1550001     	cmp	r5, r1
      94: 1affffe9     	bne	0x40 <pc_neon_scale_dim+0x40> @ imm = #-0x5c
      98: f26041f0     	vorr	q10, q8, q8
      9c: e1a03009     	mov	r3, r9
      a0: f3fa41e0     	vzip.32	q10, q8
      a4: f4434a8d     	vst1.32	{d20, d21}, [r3]!
      a8: f4430a8f     	vst1.32	{d16, d17}, [r3]
      ac: eaffffe4     	b	0x44 <pc_neon_scale_dim+0x44> @ imm = #-0x70
      b0: e3a0c000     	mov	r12, #0
      b4: e154000c     	cmp	r4, r12
      b8: 98bd8ff0     	popls	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
      bc: e3a060ff     	mov	r6, #255
      c0: e3a09cff     	mov	r9, #65280
      c4: e34060ff     	movt	r6, #0xff
      c8: e34f9f00     	movt	r9, #0xff00
      cc: e287b004     	add	r11, r7, #4
      d0: e798310c     	ldr	r3, [r8, r12, lsl #2]
      d4: e3520008     	cmp	r2, #8
      d8: e003a006     	and	r10, r3, r6
      dc: e006e423     	and	lr, r6, r3, lsr #8
      e0: e00a0a92     	mul	r10, r2, r10
      e4: e00e0e92     	mul	lr, r2, lr
      e8: e006a1aa     	and	r10, r6, r10, lsr #3
      ec: e009e28e     	and	lr, r9, lr, lsl #5
      f0: 118e300a     	orrne	r3, lr, r10
      f4: e3500003     	cmp	r0, #3
      f8: 16bf3f33     	revne	r3, r3
      fc: e1550001     	cmp	r5, r1
     100: 0787318c     	streq	r3, [r7, r12, lsl #3]
     104: 1787310c     	strne	r3, [r7, r12, lsl #2]
     108: 078b318c     	streq	r3, [r11, r12, lsl #3]
     10c: e28cc001     	add	r12, r12, #1
     110: e154000c     	cmp	r4, r12
     114: 1affffed     	bne	0xd0 <pc_neon_scale_dim+0xd0> @ imm = #-0x4c
     118: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}


.context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-pic-vfp-matrix-bench-tinygl-internal/111-neon.arm.o:	file format elf32-littlearm

Disassembly of section .text.zz_mat_neon:

00000000 <zz_mat_neon>:
       0: f2803010     	vmov.i32	d3, #0x0
       4: e280c010     	add	r12, r0, #16
       8: e1a03001     	mov	r3, r1
       c: f2235113     	vorr	d5, d3, d3
      10: f2237113     	vorr	d7, d3, d3
      14: f2230113     	vorr	d0, d3, d3
      18: f4ac580f     	vld1.32	{d5[0]}, [r12]
      1c: e280c020     	add	r12, r0, #32
      20: f2231113     	vorr	d1, d3, d3
      24: f4ac780f     	vld1.32	{d7[0]}, [r12]
      28: e280c030     	add	r12, r0, #48
      2c: f4ac080f     	vld1.32	{d0[0]}, [r12]
      30: e280c004     	add	r12, r0, #4
      34: f2212111     	vorr	d2, d1, d1
      38: f4a0380f     	vld1.32	{d3[0]}, [r0]
      3c: f2214111     	vorr	d4, d1, d1
      40: f2216111     	vorr	d6, d1, d1
      44: f4ac180f     	vld1.32	{d1[0]}, [r12]
      48: e280c014     	add	r12, r0, #20
      4c: f4630a8d     	vld1.32	{d16, d17}, [r3]!
      50: f4ac280f     	vld1.32	{d2[0]}, [r12]
      54: e280c024     	add	r12, r0, #36
      58: f3e069c3     	vmul.f32	q11, q8, d3[0]
      5c: f4ac480f     	vld1.32	{d4[0]}, [r12]
      60: e280c034     	add	r12, r0, #52
      64: f3e049c5     	vmul.f32	q10, q8, d5[0]
      68: f4ac680f     	vld1.32	{d6[0]}, [r12]
      6c: e280c008     	add	r12, r0, #8
      70: f3e029c7     	vmul.f32	q9, q8, d7[0]
      74: f4638a8f     	vld1.32	{d24, d25}, [r3]
      78: e2813020     	add	r3, r1, #32
      7c: f3e009c0     	vmul.f32	q8, q8, d0[0]
      80: e2811030     	add	r1, r1, #48
      84: f2803010     	vmov.i32	d3, #0x0
      88: f3e861c1     	vmla.f32	q11, q12, d1[0]
      8c: f3e821c4     	vmla.f32	q9, q12, d4[0]
      90: f3e801c6     	vmla.f32	q8, q12, d6[0]
      94: f3e841c2     	vmla.f32	q10, q12, d2[0]
      98: f4638a8f     	vld1.32	{d24, d25}, [r3]
      9c: e2803038     	add	r3, r0, #56
      a0: f2235113     	vorr	d5, d3, d3
      a4: f2237113     	vorr	d7, d3, d3
      a8: f2236113     	vorr	d6, d3, d3
      ac: f4ac380f     	vld1.32	{d3[0]}, [r12]
      b0: e280c018     	add	r12, r0, #24
      b4: f2804010     	vmov.i32	d4, #0x0
      b8: f4ac580f     	vld1.32	{d5[0]}, [r12]
      bc: e280c028     	add	r12, r0, #40
      c0: f3e861c3     	vmla.f32	q11, q12, d3[0]
      c4: f4a3680f     	vld1.32	{d6[0]}, [r3]
      c8: e280300c     	add	r3, r0, #12
      cc: f3e841c5     	vmla.f32	q10, q12, d5[0]
      d0: f4ac780f     	vld1.32	{d7[0]}, [r12]
      d4: f3e801c6     	vmla.f32	q8, q12, d6[0]
      d8: f3e821c7     	vmla.f32	q9, q12, d7[0]
      dc: f4618a8f     	vld1.32	{d24, d25}, [r1]
      e0: e2821020     	add	r1, r2, #32
      e4: f2245114     	vorr	d5, d4, d4
      e8: f2246114     	vorr	d6, d4, d4
      ec: f2247114     	vorr	d7, d4, d4
      f0: f4a3480f     	vld1.32	{d4[0]}, [r3]
      f4: e280301c     	add	r3, r0, #28
      f8: f4a3580f     	vld1.32	{d5[0]}, [r3]
      fc: e280302c     	add	r3, r0, #44
     100: f3e861c4     	vmla.f32	q11, q12, d4[0]
     104: f4a3680f     	vld1.32	{d6[0]}, [r3]
     108: e280303c     	add	r3, r0, #60
     10c: f3e841c5     	vmla.f32	q10, q12, d5[0]
     110: f4a3780f     	vld1.32	{d7[0]}, [r3]
     114: e2823030     	add	r3, r2, #48
     118: f3e821c6     	vmla.f32	q9, q12, d6[0]
     11c: f3e801c7     	vmla.f32	q8, q12, d7[0]
     120: f4426a8d     	vst1.32	{d22, d23}, [r2]!
     124: f4424a8f     	vst1.32	{d20, d21}, [r2]
     128: f4412a8f     	vst1.32	{d18, d19}, [r1]
     12c: f4430a8f     	vst1.32	{d16, d17}, [r3]
     130: e12fff1e     	bx	lr

Disassembly of section .text.zz_vec_neon:

00000000 <zz_vec_neon>:
       0: e3530003     	cmp	r3, #3
       4: e92d40f0     	push	{r4, r5, r6, r7, lr}
       8: e1a04003     	mov	r4, r3
       c: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
      10: e1a0e001     	mov	lr, r1
      14: e24dd044     	sub	sp, sp, #68
      18: e1a06002     	mov	r6, r2
      1c: 9a00005e     	bls	0x19c <zz_vec_neon+0x19c> @ imm = #0x178
      20: e3a0c004     	mov	r12, #4
      24: e1a07001     	mov	r7, r1
      28: e1a03001     	mov	r3, r1
      2c: f4a0780f     	vld1.32	{d7[0]}, [r0]
      30: e280e004     	add	lr, r0, #4
      34: e1a0500c     	mov	r5, r12
      38: f463818d     	vld4.32	{d24, d26, d28, d30}, [r3]!
      3c: e28cc004     	add	r12, r12, #4
      40: e154000c     	cmp	r4, r12
      44: e2811040     	add	r1, r1, #64
      48: f4ae380f     	vld1.32	{d3[0]}, [lr]
      4c: e280e010     	add	lr, r0, #16
      50: f463918f     	vld4.32	{d25, d27, d29, d31}, [r3]
      54: f4ae680f     	vld1.32	{d6[0]}, [lr]
      58: e280e014     	add	lr, r0, #20
      5c: f3a889c7     	vmul.f32	q4, q12, d7[0]
      60: f4ae280f     	vld1.32	{d2[0]}, [lr]
      64: e280e008     	add	lr, r0, #8
      68: f4ee080f     	vld1.32	{d16[0]}, [lr]
      6c: e280e020     	add	lr, r0, #32
      70: f4ae580f     	vld1.32	{d5[0]}, [lr]
      74: e280e024     	add	lr, r0, #36
      78: f3aa81c3     	vmla.f32	q4, q13, d3[0]
      7c: f4ae180f     	vld1.32	{d1[0]}, [lr]
      80: e280e018     	add	lr, r0, #24
      84: f4ee180f     	vld1.32	{d17[0]}, [lr]
      88: e280e00c     	add	lr, r0, #12
      8c: f4ee480f     	vld1.32	{d20[0]}, [lr]
      90: e280e030     	add	lr, r0, #48
      94: f4ae480f     	vld1.32	{d4[0]}, [lr]
      98: e280e034     	add	lr, r0, #52
      9c: f4ae080f     	vld1.32	{d0[0]}, [lr]
      a0: e280e028     	add	lr, r0, #40
      a4: f228a158     	vorr	q5, q4, q4
      a8: f22081b0     	vorr	d8, d16, d16
      ac: f4ee280f     	vld1.32	{d18[0]}, [lr]
      b0: e280e01c     	add	lr, r0, #28
      b4: f3aca1c8     	vmla.f32	q5, q14, d8[0]
      b8: f4ee580f     	vld1.32	{d21[0]}, [lr]
      bc: e280e038     	add	lr, r0, #56
      c0: f22481b4     	vorr	d8, d20, d20
      c4: f4ee380f     	vld1.32	{d19[0]}, [lr]
      c8: e280e02c     	add	lr, r0, #44
      cc: f4ee680f     	vld1.32	{d22[0]}, [lr]
      d0: e280e03c     	add	lr, r0, #60
      d4: f4ee780f     	vld1.32	{d23[0]}, [lr]
      d8: e1a0e002     	mov	lr, r2
      dc: e2822040     	add	r2, r2, #64
      e0: f3aea1c8     	vmla.f32	q5, q15, d8[0]
      e4: f3a889c6     	vmul.f32	q4, q12, d6[0]
      e8: f3aa81c2     	vmla.f32	q4, q13, d2[0]
      ec: f40daadf     	vst1.64	{d10, d11}, [sp:64]
      f0: f228a158     	vorr	q5, q4, q4
      f4: f22181b1     	vorr	d8, d17, d17
      f8: f3aca1c8     	vmla.f32	q5, q14, d8[0]
      fc: f22581b5     	vorr	d8, d21, d21
     100: f3aea1c8     	vmla.f32	q5, q15, d8[0]
     104: f3a889c5     	vmul.f32	q4, q12, d5[0]
     108: f3aa81c1     	vmla.f32	q4, q13, d1[0]
     10c: ed8dab04     	vstr	d10, [sp, #16]
     110: ed8dbb06     	vstr	d11, [sp, #24]
     114: f228a158     	vorr	q5, q4, q4
     118: f22281b2     	vorr	d8, d18, d18
     11c: f3aca1c8     	vmla.f32	q5, q14, d8[0]
     120: f22681b6     	vorr	d8, d22, d22
     124: f3aea1c8     	vmla.f32	q5, q15, d8[0]
     128: f3a889c4     	vmul.f32	q4, q12, d4[0]
     12c: f3aa81c0     	vmla.f32	q4, q13, d0[0]
     130: ed8dab08     	vstr	d10, [sp, #32]
     134: ed8dbb0a     	vstr	d11, [sp, #40]
     138: f228a158     	vorr	q5, q4, q4
     13c: f22381b3     	vorr	d8, d19, d19
     140: f3aca1c8     	vmla.f32	q5, q14, d8[0]
     144: f22781b7     	vorr	d8, d23, d23
     148: f3aea1c8     	vmla.f32	q5, q15, d8[0]
     14c: ed8dab0c     	vstr	d10, [sp, #48]
     150: ed8dbb0e     	vstr	d11, [sp, #56]
     154: ecdd8b10     	vldmia	sp, {d24, d25, d26, d27, d28, d29, d30, d31}
     158: f44e818d     	vst4.32	{d24, d26, d28, d30}, [lr]!
     15c: f44e918f     	vst4.32	{d25, d27, d29, d31}, [lr]
     160: 2affffb0     	bhs	0x28 <zz_vec_neon+0x28> @ imm = #-0x140
     164: e1a0e007     	mov	lr, r7
     168: e1540005     	cmp	r4, r5
     16c: 8a000002     	bhi	0x17c <zz_vec_neon+0x17c> @ imm = #0x8
     170: e28dd044     	add	sp, sp, #68
     174: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     178: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
     17c: e0443005     	sub	r3, r4, r5
     180: e1a05205     	lsl	r5, r5, #4
     184: e0862005     	add	r2, r6, r5
     188: e08e1005     	add	r1, lr, r5
     18c: e28dd044     	add	sp, sp, #68
     190: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
     194: e8bd40f0     	pop	{r4, r5, r6, r7, lr}
     198: eafffffe     	b	0x198 <zz_vec_neon+0x198> @ imm = #-0x8
     19c: e3a05000     	mov	r5, #0
     1a0: eafffff0     	b	0x168 <zz_vec_neon+0x168> @ imm = #-0x40
