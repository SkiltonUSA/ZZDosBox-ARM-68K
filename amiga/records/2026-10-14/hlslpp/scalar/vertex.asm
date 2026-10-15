
amiga/records/2026-10-14/hlslpp/scalar/dosbox.elf:	file format elf32-littlearm

Disassembly of section .text:

000b6528 <zz_vertex_hlsl>:
   b6528: e52de004     	str	lr, [sp, #-0x4]!
   b652c: e24dd014     	sub	sp, sp, #20
   b6530: e5913000     	ldr	r3, [r1]
   b6534: e591e004     	ldr	lr, [r1, #0x4]
   b6538: e591c008     	ldr	r12, [r1, #0x8]
   b653c: e3a01000     	mov	r1, #0
   b6540: e58d3000     	str	r3, [sp]
   b6544: e1a03000     	mov	r3, r0
   b6548: e58d1004     	str	r1, [sp, #0x4]
   b654c: e58d1008     	str	r1, [sp, #0x8]
   b6550: e58d100c     	str	r1, [sp, #0xc]
   b6554: f42d2a9f     	vld1.32	{d2, d3}, [sp:64]
   b6558: e58de000     	str	lr, [sp]
   b655c: f42d4a9f     	vld1.32	{d4, d5}, [sp:64]
   b6560: f2637113     	vorr	d23, d3, d3
   b6564: f2655115     	vorr	d21, d5, d5
   b6568: f2626112     	vorr	d22, d2, d2
   b656c: f2644114     	vorr	d20, d4, d4
   b6570: eddfeb18     	vldr	d30, [pc, #96]          @ 0xb65d8 <zz_vertex_hlsl+0xb0>
   b6574: e58dc000     	str	r12, [sp]
   b6578: f3f669ae     	vtbl.8	d22, {d22, d23}, d30
   b657c: f3f449ae     	vtbl.8	d20, {d20, d21}, d30
   b6580: f26671b6     	vorr	d23, d22, d22
   b6584: f42d6a9f     	vld1.32	{d6, d7}, [sp:64]
   b6588: f26451b4     	vorr	d21, d20, d20
   b658c: f4630a8d     	vld1.32	{d16, d17}, [r3]!
   b6590: f2673117     	vorr	d19, d7, d7
   b6594: f2662116     	vorr	d18, d6, d6
   b6598: f463ca8f     	vld1.32	{d28, d29}, [r3]
   b659c: e2803020     	add	r3, r0, #32
   b65a0: f3400df6     	vmul.f32	q8, q8, q11
   b65a4: e2800030     	add	r0, r0, #48
   b65a8: f3f229ae     	vtbl.8	d18, {d18, d19}, d30
   b65ac: f34c4df4     	vmul.f32	q10, q14, q10
   b65b0: f26231b2     	vorr	d19, d18, d18
   b65b4: f4638a8f     	vld1.32	{d24, d25}, [r3]
   b65b8: f3482df2     	vmul.f32	q9, q12, q9
   b65bc: f460aa8f     	vld1.32	{d26, d27}, [r0]
   b65c0: f2400de4     	vadd.f32	q8, q8, q10
   b65c4: f2400de2     	vadd.f32	q8, q8, q9
   b65c8: f2400dea     	vadd.f32	q8, q8, q13
   b65cc: f4420a8f     	vst1.32	{d16, d17}, [r2]
   b65d0: e28dd014     	add	sp, sp, #20
   b65d4: e49df004     	ldr	pc, [sp], #4
