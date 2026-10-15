
amiga/records/2026-10-14/hlslpp/micro/dosbox.elf:	file format elf32-littlearm

Disassembly of section .text:

000b6000 <zz_mat_hlsl>:
   b6000: e2803030     	add	r3, r0, #48
   b6004: e92d4010     	push	{r4, lr}
   b6008: e280c020     	add	r12, r0, #32
   b600c: e1a04002     	mov	r4, r2
   b6010: e2812020     	add	r2, r1, #32
   b6014: e24dd0c0     	sub	sp, sp, #192
   b6018: f460ea8d     	vld1.32	{d30, d31}, [r0]!
   b601c: f4638a8f     	vld1.32	{d24, d25}, [r3]
   b6020: e2813030     	add	r3, r1, #48
   b6024: f4616a8d     	vld1.32	{d22, d23}, [r1]!
   b6028: f460ca8f     	vld1.32	{d28, d29}, [r0]
   b602c: e28d0080     	add	r0, sp, #128
   b6030: f4622a8f     	vld1.32	{d18, d19}, [r2]
   b6034: e28d2040     	add	r2, sp, #64
   b6038: f4630a8f     	vld1.32	{d16, d17}, [r3]
   b603c: f4614a8f     	vld1.32	{d20, d21}, [r1]
   b6040: e1a0100d     	mov	r1, sp
   b6044: f46caa8f     	vld1.32	{d26, d27}, [r12]
   b6048: edcd6b10     	vstr	d22, [sp, #64]
   b604c: edcd7b12     	vstr	d23, [sp, #72]
   b6050: edcd4b14     	vstr	d20, [sp, #80]
   b6054: edcd5b16     	vstr	d21, [sp, #88]
   b6058: edcd2b18     	vstr	d18, [sp, #96]
   b605c: edcd3b1a     	vstr	d19, [sp, #104]
   b6060: edcd0b1c     	vstr	d16, [sp, #112]
   b6064: edcd1b1e     	vstr	d17, [sp, #120]
   b6068: f44deadf     	vst1.64	{d30, d31}, [sp:64]
   b606c: edcdcb04     	vstr	d28, [sp, #16]
   b6070: edcddb06     	vstr	d29, [sp, #24]
   b6074: edcdab08     	vstr	d26, [sp, #32]
   b6078: edcdbb0a     	vstr	d27, [sp, #40]
   b607c: edcd8b0c     	vstr	d24, [sp, #48]
   b6080: edcd9b0e     	vstr	d25, [sp, #56]
   b6084: ebffff59     	bl	0xb5df0 <_ZN6hlslpp3mulERKNS_8float4x4ES2_> @ imm = #-0x29c
   b6088: e1a02004     	mov	r2, r4
   b608c: eddd4b24     	vldr	d20, [sp, #144]
   b6090: eddd5b26     	vldr	d21, [sp, #152]
   b6094: e2841020     	add	r1, r4, #32
   b6098: e2843030     	add	r3, r4, #48
   b609c: eddd2b28     	vldr	d18, [sp, #160]
   b60a0: eddd3b2a     	vldr	d19, [sp, #168]
   b60a4: eddd0b2c     	vldr	d16, [sp, #176]
   b60a8: eddd1b2e     	vldr	d17, [sp, #184]
   b60ac: eddd6b20     	vldr	d22, [sp, #128]
   b60b0: eddd7b22     	vldr	d23, [sp, #136]
   b60b4: f4426a8d     	vst1.32	{d22, d23}, [r2]!
   b60b8: f4424a8f     	vst1.32	{d20, d21}, [r2]
   b60bc: f4412a8f     	vst1.32	{d18, d19}, [r1]
   b60c0: f4430a8f     	vst1.32	{d16, d17}, [r3]
   b60c4: e28dd0c0     	add	sp, sp, #192
   b60c8: e8bd8010     	pop	{r4, pc}
   b60cc: 00000000     	andeq	r0, r0, r0

000b60d0 <zz_vec_hlsl>:
   b60d0: e3530000     	cmp	r3, #0
   b60d4: 012fff1e     	bxeq	lr
   b60d8: e1a0c000     	mov	r12, r0
   b60dc: e92d4010     	push	{r4, lr}
   b60e0: ed2d8b0c     	vpush	{d8, d9, d10, d11, d12, d13}
   b60e4: e24dd010     	sub	sp, sp, #16
   b60e8: f42c0a8d     	vld1.32	{d0, d1}, [r12]!
   b60ec: f42c2a8f     	vld1.32	{d2, d3}, [r12]
   b60f0: e280c020     	add	r12, r0, #32
   b60f4: e2800030     	add	r0, r0, #48
   b60f8: f42c4a8f     	vld1.32	{d4, d5}, [r12]
   b60fc: e3a0c000     	mov	r12, #0
   b6100: e1a0e00c     	mov	lr, r12
   b6104: f4206a8f     	vld1.32	{d6, d7}, [r0]
   b6108: e3e00000     	mvn	r0, #0
   b610c: eddf7b31     	vldr	d23, [pc, #196]         @ 0xb61d8 <zz_vec_hlsl+0x108>
   b6110: eddf6b32     	vldr	d22, [pc, #200]         @ 0xb61e0 <zz_vec_hlsl+0x110>
   b6114: e58de000     	str	lr, [sp]
   b6118: e28cc001     	add	r12, r12, #1
   b611c: f4614a8d     	vld1.32	{d20, d21}, [r1]!
   b6120: e153000c     	cmp	r3, r12
   b6124: f3400d74     	vmul.f32	q8, q0, q10
   b6128: f302ad74     	vmul.f32	q5, q1, q10
   b612c: e98d4001     	stmib	sp, {r0, lr}
   b6130: e58d000c     	str	r0, [sp, #0xc]
   b6134: f3442d74     	vmul.f32	q9, q2, q10
   b6138: f3464d74     	vmul.f32	q10, q3, q10
   b613c: f3b88060     	vrev64.32	q4, q8
   b6140: f2400dc8     	vadd.f32	q8, q8, q4
   b6144: f3b8804a     	vrev64.32	q4, q5
   b6148: f20aad48     	vadd.f32	q5, q5, q4
   b614c: f3b88062     	vrev64.32	q4, q9
   b6150: f221c1b1     	vorr	d12, d17, d17
   b6154: f220d1b0     	vorr	d13, d16, d16
   b6158: f2422dc8     	vadd.f32	q9, q9, q4
   b615c: f2400dcc     	vadd.f32	q8, q8, q6
   b6160: f3b88064     	vrev64.32	q4, q10
   b6164: f2048dc8     	vadd.f32	q4, q10, q4
   b6168: f46d4a9f     	vld1.32	{d20, d21}, [sp:64]
   b616c: f260c1b0     	vorr	d28, d16, d16
   b6170: f261d1b1     	vorr	d29, d17, d17
   b6174: f26b011b     	vorr	d16, d11, d11
   b6178: f26a111a     	vorr	d17, d10, d10
   b617c: f20aad60     	vadd.f32	q5, q5, q8
   b6180: f26301b3     	vorr	d16, d19, d19
   b6184: f26211b2     	vorr	d17, d18, d18
   b6188: f2422de0     	vadd.f32	q9, q9, q8
   b618c: f2690119     	vorr	d16, d9, d9
   b6190: f2681118     	vorr	d17, d8, d8
   b6194: f26a811a     	vorr	d24, d10, d10
   b6198: f2480d60     	vadd.f32	q8, q4, q8
   b619c: f262e1b2     	vorr	d30, d18, d18
   b61a0: f26b911b     	vorr	d25, d11, d11
   b61a4: f263f1b3     	vorr	d31, d19, d19
   b61a8: f260a1b0     	vorr	d26, d16, d16
   b61ac: f261b1b1     	vorr	d27, d17, d17
   b61b0: f3fc2ba7     	vtbl.8	d18, {d28, d29, d30, d31}, d23
   b61b4: f3fc3ba6     	vtbl.8	d19, {d28, d29, d30, d31}, d22
   b61b8: f3f80ba7     	vtbl.8	d16, {d24, d25, d26, d27}, d23
   b61bc: f3f81ba6     	vtbl.8	d17, {d24, d25, d26, d27}, d22
   b61c0: f35041f2     	vbsl	q10, q8, q9
   b61c4: f4424a8d     	vst1.32	{d20, d21}, [r2]!
   b61c8: 1affffd1     	bne	0xb6114 <zz_vec_hlsl+0x44> @ imm = #-0xbc
   b61cc: e28dd010     	add	sp, sp, #16
   b61d0: ecbd8b0c     	vpop	{d8, d9, d10, d11, d12, d13}
   b61d4: e8bd8010     	pop	{r4, pc}

000b61e8 <zz_mat_neon>:
   b61e8: f2803010     	vmov.i32	d3, #0x0
   b61ec: e280c010     	add	r12, r0, #16
   b61f0: e1a03001     	mov	r3, r1
   b61f4: f2235113     	vorr	d5, d3, d3
   b61f8: f2237113     	vorr	d7, d3, d3
   b61fc: f2230113     	vorr	d0, d3, d3
   b6200: f4ac580f     	vld1.32	{d5[0]}, [r12]
   b6204: e280c020     	add	r12, r0, #32
   b6208: f2231113     	vorr	d1, d3, d3
   b620c: f4ac780f     	vld1.32	{d7[0]}, [r12]
   b6210: e280c030     	add	r12, r0, #48
   b6214: f4ac080f     	vld1.32	{d0[0]}, [r12]
   b6218: e280c004     	add	r12, r0, #4
   b621c: f2212111     	vorr	d2, d1, d1
   b6220: f4a0380f     	vld1.32	{d3[0]}, [r0]
   b6224: f2214111     	vorr	d4, d1, d1
   b6228: f2216111     	vorr	d6, d1, d1
   b622c: f4ac180f     	vld1.32	{d1[0]}, [r12]
   b6230: e280c014     	add	r12, r0, #20
   b6234: f4630a8d     	vld1.32	{d16, d17}, [r3]!
   b6238: f4ac280f     	vld1.32	{d2[0]}, [r12]
   b623c: e280c024     	add	r12, r0, #36
   b6240: f3e069c3     	vmul.f32	q11, q8, d3[0]
   b6244: f4ac480f     	vld1.32	{d4[0]}, [r12]
   b6248: e280c034     	add	r12, r0, #52
   b624c: f3e049c5     	vmul.f32	q10, q8, d5[0]
   b6250: f4ac680f     	vld1.32	{d6[0]}, [r12]
   b6254: e280c008     	add	r12, r0, #8
   b6258: f3e029c7     	vmul.f32	q9, q8, d7[0]
   b625c: f4638a8f     	vld1.32	{d24, d25}, [r3]
   b6260: e2813020     	add	r3, r1, #32
   b6264: f3e009c0     	vmul.f32	q8, q8, d0[0]
   b6268: e2811030     	add	r1, r1, #48
   b626c: f2803010     	vmov.i32	d3, #0x0
   b6270: f3e861c1     	vmla.f32	q11, q12, d1[0]
   b6274: f3e821c4     	vmla.f32	q9, q12, d4[0]
   b6278: f3e801c6     	vmla.f32	q8, q12, d6[0]
   b627c: f3e841c2     	vmla.f32	q10, q12, d2[0]
   b6280: f4638a8f     	vld1.32	{d24, d25}, [r3]
   b6284: e2803038     	add	r3, r0, #56
   b6288: f2235113     	vorr	d5, d3, d3
   b628c: f2237113     	vorr	d7, d3, d3
   b6290: f2236113     	vorr	d6, d3, d3
   b6294: f4ac380f     	vld1.32	{d3[0]}, [r12]
   b6298: e280c018     	add	r12, r0, #24
   b629c: f2804010     	vmov.i32	d4, #0x0
   b62a0: f4ac580f     	vld1.32	{d5[0]}, [r12]
   b62a4: e280c028     	add	r12, r0, #40
   b62a8: f3e861c3     	vmla.f32	q11, q12, d3[0]
   b62ac: f4a3680f     	vld1.32	{d6[0]}, [r3]
   b62b0: e280300c     	add	r3, r0, #12
   b62b4: f3e841c5     	vmla.f32	q10, q12, d5[0]
   b62b8: f4ac780f     	vld1.32	{d7[0]}, [r12]
   b62bc: f3e801c6     	vmla.f32	q8, q12, d6[0]
   b62c0: f3e821c7     	vmla.f32	q9, q12, d7[0]
   b62c4: f4618a8f     	vld1.32	{d24, d25}, [r1]
   b62c8: e2821020     	add	r1, r2, #32
   b62cc: f2245114     	vorr	d5, d4, d4
   b62d0: f2246114     	vorr	d6, d4, d4
   b62d4: f2247114     	vorr	d7, d4, d4
   b62d8: f4a3480f     	vld1.32	{d4[0]}, [r3]
   b62dc: e280301c     	add	r3, r0, #28
   b62e0: f4a3580f     	vld1.32	{d5[0]}, [r3]
   b62e4: e280302c     	add	r3, r0, #44
   b62e8: f3e861c4     	vmla.f32	q11, q12, d4[0]
   b62ec: f4a3680f     	vld1.32	{d6[0]}, [r3]
   b62f0: e280303c     	add	r3, r0, #60
   b62f4: f3e841c5     	vmla.f32	q10, q12, d5[0]
   b62f8: f4a3780f     	vld1.32	{d7[0]}, [r3]
   b62fc: e2823030     	add	r3, r2, #48
   b6300: f3e821c6     	vmla.f32	q9, q12, d6[0]
   b6304: f3e801c7     	vmla.f32	q8, q12, d7[0]
   b6308: f4426a8d     	vst1.32	{d22, d23}, [r2]!
   b630c: f4424a8f     	vst1.32	{d20, d21}, [r2]
   b6310: f4412a8f     	vst1.32	{d18, d19}, [r1]
   b6314: f4430a8f     	vst1.32	{d16, d17}, [r3]
   b6318: e12fff1e     	bx	lr

000b631c <zz_vec_neon>:
   b631c: e3530003     	cmp	r3, #3
   b6320: e92d40f0     	push	{r4, r5, r6, r7, lr}
   b6324: e1a04003     	mov	r4, r3
   b6328: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
   b632c: e1a0e001     	mov	lr, r1
   b6330: e24dd044     	sub	sp, sp, #68
   b6334: e1a06002     	mov	r6, r2
   b6338: 9a00005e     	bls	0xb64b8 <zz_vec_neon+0x19c> @ imm = #0x178
   b633c: e3a0c004     	mov	r12, #4
   b6340: e1a07001     	mov	r7, r1
   b6344: e1a03001     	mov	r3, r1
   b6348: f4a0780f     	vld1.32	{d7[0]}, [r0]
   b634c: e280e004     	add	lr, r0, #4
   b6350: e1a0500c     	mov	r5, r12
   b6354: f463818d     	vld4.32	{d24, d26, d28, d30}, [r3]!
   b6358: e28cc004     	add	r12, r12, #4
   b635c: e154000c     	cmp	r4, r12
   b6360: e2811040     	add	r1, r1, #64
   b6364: f4ae380f     	vld1.32	{d3[0]}, [lr]
   b6368: e280e010     	add	lr, r0, #16
   b636c: f463918f     	vld4.32	{d25, d27, d29, d31}, [r3]
   b6370: f4ae680f     	vld1.32	{d6[0]}, [lr]
   b6374: e280e014     	add	lr, r0, #20
   b6378: f3a889c7     	vmul.f32	q4, q12, d7[0]
   b637c: f4ae280f     	vld1.32	{d2[0]}, [lr]
   b6380: e280e008     	add	lr, r0, #8
   b6384: f4ee080f     	vld1.32	{d16[0]}, [lr]
   b6388: e280e020     	add	lr, r0, #32
   b638c: f4ae580f     	vld1.32	{d5[0]}, [lr]
   b6390: e280e024     	add	lr, r0, #36
   b6394: f3aa81c3     	vmla.f32	q4, q13, d3[0]
   b6398: f4ae180f     	vld1.32	{d1[0]}, [lr]
   b639c: e280e018     	add	lr, r0, #24
   b63a0: f4ee180f     	vld1.32	{d17[0]}, [lr]
   b63a4: e280e00c     	add	lr, r0, #12
   b63a8: f4ee480f     	vld1.32	{d20[0]}, [lr]
   b63ac: e280e030     	add	lr, r0, #48
   b63b0: f4ae480f     	vld1.32	{d4[0]}, [lr]
   b63b4: e280e034     	add	lr, r0, #52
   b63b8: f4ae080f     	vld1.32	{d0[0]}, [lr]
   b63bc: e280e028     	add	lr, r0, #40
   b63c0: f228a158     	vorr	q5, q4, q4
   b63c4: f22081b0     	vorr	d8, d16, d16
   b63c8: f4ee280f     	vld1.32	{d18[0]}, [lr]
   b63cc: e280e01c     	add	lr, r0, #28
   b63d0: f3aca1c8     	vmla.f32	q5, q14, d8[0]
   b63d4: f4ee580f     	vld1.32	{d21[0]}, [lr]
   b63d8: e280e038     	add	lr, r0, #56
   b63dc: f22481b4     	vorr	d8, d20, d20
   b63e0: f4ee380f     	vld1.32	{d19[0]}, [lr]
   b63e4: e280e02c     	add	lr, r0, #44
   b63e8: f4ee680f     	vld1.32	{d22[0]}, [lr]
   b63ec: e280e03c     	add	lr, r0, #60
   b63f0: f4ee780f     	vld1.32	{d23[0]}, [lr]
   b63f4: e1a0e002     	mov	lr, r2
   b63f8: e2822040     	add	r2, r2, #64
   b63fc: f3aea1c8     	vmla.f32	q5, q15, d8[0]
   b6400: f3a889c6     	vmul.f32	q4, q12, d6[0]
   b6404: f3aa81c2     	vmla.f32	q4, q13, d2[0]
   b6408: f40daadf     	vst1.64	{d10, d11}, [sp:64]
   b640c: f228a158     	vorr	q5, q4, q4
   b6410: f22181b1     	vorr	d8, d17, d17
   b6414: f3aca1c8     	vmla.f32	q5, q14, d8[0]
   b6418: f22581b5     	vorr	d8, d21, d21
   b641c: f3aea1c8     	vmla.f32	q5, q15, d8[0]
   b6420: f3a889c5     	vmul.f32	q4, q12, d5[0]
   b6424: f3aa81c1     	vmla.f32	q4, q13, d1[0]
   b6428: ed8dab04     	vstr	d10, [sp, #16]
   b642c: ed8dbb06     	vstr	d11, [sp, #24]
   b6430: f228a158     	vorr	q5, q4, q4
   b6434: f22281b2     	vorr	d8, d18, d18
   b6438: f3aca1c8     	vmla.f32	q5, q14, d8[0]
   b643c: f22681b6     	vorr	d8, d22, d22
   b6440: f3aea1c8     	vmla.f32	q5, q15, d8[0]
   b6444: f3a889c4     	vmul.f32	q4, q12, d4[0]
   b6448: f3aa81c0     	vmla.f32	q4, q13, d0[0]
   b644c: ed8dab08     	vstr	d10, [sp, #32]
   b6450: ed8dbb0a     	vstr	d11, [sp, #40]
   b6454: f228a158     	vorr	q5, q4, q4
   b6458: f22381b3     	vorr	d8, d19, d19
   b645c: f3aca1c8     	vmla.f32	q5, q14, d8[0]
   b6460: f22781b7     	vorr	d8, d23, d23
   b6464: f3aea1c8     	vmla.f32	q5, q15, d8[0]
   b6468: ed8dab0c     	vstr	d10, [sp, #48]
   b646c: ed8dbb0e     	vstr	d11, [sp, #56]
   b6470: ecdd8b10     	vldmia	sp, {d24, d25, d26, d27, d28, d29, d30, d31}
   b6474: f44e818d     	vst4.32	{d24, d26, d28, d30}, [lr]!
   b6478: f44e918f     	vst4.32	{d25, d27, d29, d31}, [lr]
   b647c: 2affffb0     	bhs	0xb6344 <zz_vec_neon+0x28> @ imm = #-0x140
   b6480: e1a0e007     	mov	lr, r7
   b6484: e1540005     	cmp	r4, r5
   b6488: 8a000002     	bhi	0xb6498 <zz_vec_neon+0x17c> @ imm = #0x8
   b648c: e28dd044     	add	sp, sp, #68
   b6490: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
   b6494: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   b6498: e0443005     	sub	r3, r4, r5
   b649c: e1a05205     	lsl	r5, r5, #4
   b64a0: e0862005     	add	r2, r6, r5
   b64a4: e08e1005     	add	r1, lr, r5
   b64a8: e28dd044     	add	sp, sp, #68
   b64ac: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
   b64b0: e8bd40f0     	pop	{r4, r5, r6, r7, lr}
   b64b4: ea000020     	b	0xb653c <zz_vec_scalar> @ imm = #0x80
   b64b8: e3a05000     	mov	r5, #0
   b64bc: eafffff0     	b	0xb6484 <zz_vec_neon+0x168> @ imm = #-0x40

000b64c0 <zz_mat_scalar>:
   b64c0: e92d40f0     	push	{r4, r5, r6, r7, lr}
   b64c4: e280c010     	add	r12, r0, #16
   b64c8: e1a07001     	mov	r7, r1
   b64cc: e1a06002     	mov	r6, r2
   b64d0: e2804004     	add	r4, r0, #4
   b64d4: e3a05000     	mov	r5, #0
   b64d8: e0861105     	add	r1, r6, r5, lsl #2
   b64dc: e1a0e007     	mov	lr, r7
   b64e0: e3a02000     	mov	r2, #0
   b64e4: ed5c7a04     	vldr	s15, [r12, #-16]
   b64e8: e1a0000e     	mov	r0, lr
   b64ec: ed9e7a00     	vldr	s14, [lr]
   b64f0: e1a03004     	mov	r3, r4
   b64f4: ee677a87     	vmul.f32	s15, s15, s14
   b64f8: ecf36a01     	vldmia	r3!, {s13}
   b64fc: e2800010     	add	r0, r0, #16
   b6500: ed907a00     	vldr	s14, [r0]
   b6504: e15c0003     	cmp	r12, r3
   b6508: ee467a87     	vmla.f32	s15, s13, s14
   b650c: 1afffff9     	bne	0xb64f8 <zz_mat_scalar+0x38> @ imm = #-0x1c
   b6510: e2822001     	add	r2, r2, #1
   b6514: e28ee004     	add	lr, lr, #4
   b6518: e3520004     	cmp	r2, #4
   b651c: ece17a01     	vstmia	r1!, {s15}
   b6520: 1affffef     	bne	0xb64e4 <zz_mat_scalar+0x24> @ imm = #-0x44
   b6524: e2855004     	add	r5, r5, #4
   b6528: e2844010     	add	r4, r4, #16
   b652c: e3550010     	cmp	r5, #16
   b6530: e28cc010     	add	r12, r12, #16
   b6534: 1affffe7     	bne	0xb64d8 <zz_mat_scalar+0x18> @ imm = #-0x64
   b6538: e8bd80f0     	pop	{r4, r5, r6, r7, pc}

000b653c <zz_vec_scalar>:
   b653c: e3530000     	cmp	r3, #0
   b6540: 012fff1e     	bxeq	lr
   b6544: e92d40f0     	push	{r4, r5, r6, r7, lr}
   b6548: e1a06103     	lsl	r6, r3, #2
   b654c: e281e004     	add	lr, r1, #4
   b6550: e1a04002     	mov	r4, r2
   b6554: e3a05000     	mov	r5, #0
   b6558: e2807010     	add	r7, r0, #16
   b655c: e1a01007     	mov	r1, r7
   b6560: e1a02004     	mov	r2, r4
   b6564: e3a0c000     	mov	r12, #0
   b6568: ed517a04     	vldr	s15, [r1, #-16]
   b656c: e241300c     	sub	r3, r1, #12
   b6570: ed1e7a01     	vldr	s14, [lr, #-4]
   b6574: e1a0000e     	mov	r0, lr
   b6578: ee677a87     	vmul.f32	s15, s15, s14
   b657c: ecf36a01     	vldmia	r3!, {s13}
   b6580: ecb07a01     	vldmia	r0!, {s14}
   b6584: e1510003     	cmp	r1, r3
   b6588: ee467a87     	vmla.f32	s15, s13, s14
   b658c: 1afffffa     	bne	0xb657c <zz_vec_scalar+0x40> @ imm = #-0x18
   b6590: e28cc004     	add	r12, r12, #4
   b6594: e2811010     	add	r1, r1, #16
   b6598: e35c0010     	cmp	r12, #16
   b659c: ece27a01     	vstmia	r2!, {s15}
   b65a0: 1afffff0     	bne	0xb6568 <zz_vec_scalar+0x2c> @ imm = #-0x40
   b65a4: e2855004     	add	r5, r5, #4
   b65a8: e28ee010     	add	lr, lr, #16
   b65ac: e1560005     	cmp	r6, r5
   b65b0: e2844010     	add	r4, r4, #16
   b65b4: 1affffe8     	bne	0xb655c <zz_vec_scalar+0x20> @ imm = #-0x60
   b65b8: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
