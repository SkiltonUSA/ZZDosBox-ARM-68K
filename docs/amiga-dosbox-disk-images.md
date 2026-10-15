# ZZDosBox disk images: IMG and VHD

Use a **raw `.IMG` hard disk** with ZZDosBox. The filename extension alone
cannot tell you whether an image has the right format.

| Format | Contents | ZZDosBox support |
| --- | --- | --- |
| Raw `.IMG` | Disk sectors in order, including the partition table and filesystem. | Supported within the limits below. Created by **Disks → New hard disk**. |
| `.VHD` | Microsoft's virtual-disk container, with metadata; fixed, dynamic and differencing variants exist. | Not supported directly. Convert to a separate raw image first. |
| `.VHDX` | A separate, newer virtual-disk container format. | Not supported directly. |

VHD describes the container, not the operating system or filesystem inside it.
Microsoft documents the variants in [About VHD](https://learn.microsoft.com/en-us/windows/win32/vstor/about-vhd).
Renaming `DISK.VHD` to `DISK.IMG` does **not** convert its contents.

## Creating and using an image

1. Open **Disks → New hard disk**. Choose **32, 64, 128 or 256 MiB**.
2. Choose **DOSBox DOS 5**, which requires no installation media, or
   **MS-DOS 6.22**, offered when an installed DOS image is available.
3. Save using a short name such as `DOSNEW.IMG` (8.3 filename).
4. The image appears under **Load program → OS**. **Rename** changes its
   library title without renaming the image file.

DOSBox DOS 5 uses the emulator's built-in DOS and the image as its C: storage;
it does not install a standalone bootable MS-DOS 5 system. MS-DOS 6.22 copies
boot/system files from the user's installed image. Microsoft software is not
included with ZZDosBox.

**Mount** exposes an image in Workbench for file transfer and adds it to OS
if necessary. Existing image profiles retain their settings; repeated mounts
do not create duplicate entries. **Open contents** opens the mounted drawer.
Ejecting removes the Workbench mount, not the saved OS entry. **Delete does not
erase OS images.** Apps inside a shared DOS/Windows image need their own
uninstaller; the launcher protects the OS and other installed apps.

For a booted MS-DOS/Windows image, **Eject** it from Workbench before **Launch**.
DOSBox DOS 5 image profiles manage their Workbench mount for the session.
Mounting an unfamiliar image does not prove that it contains a bootable OS.

## Limits and conversion

Workbench mounting expects a raw MBR/FAT hard disk of **32–256 MiB** and needs
`diskimage.device` plus `fat95`. BIOS hard-disk images must also have a supported
size/alignment (256 KiB increments); the guest OS must understand their layout.
Raw floppy images and ISO CD images use their own controls, not the hard-disk
mount button.

Convert VHD/VHDX on a computer with a tool that understands that format, keeping
the source file and writing a new raw output. A differencing disk needs its
parent chain to produce a complete standalone image. Conversion does not make
an oversized disk, unsupported filesystem or missing OS compatible: check the
result against the limits above. ZZDosBox currently has no built-in VHD converter.
