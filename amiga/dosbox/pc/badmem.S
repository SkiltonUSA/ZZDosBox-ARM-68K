/* Original fixture: an unmapped high guest address must fail within the core. */
.code16
.global _start
_start:
 mov $0xf0000000,%edi
 movb (%edi),%al
 int $0x20
