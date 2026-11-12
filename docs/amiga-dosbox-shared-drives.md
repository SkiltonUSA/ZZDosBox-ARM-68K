# USB/SD and shared DOS drives

Use **Disks → USB / SD and shared folders** to turn a mounted Amiga volume or
folder into DOS `C:`. Files remain ordinary files accessible from Workbench.
No disk image, formatting or MS-DOS installation media is required.

1. Insert the card and wait for its volume to appear in Workbench.
2. Choose **Browse...** and select that volume or a folder inside it.
3. Optionally choose **Create DOSSHARE** to keep DOS files in their own folder.
4. Enter a name and choose **Add DOS drive**. The drive appears under **OS**.
5. Choose **Open in Workbench** and copy your DOS programs/files into the folder.
6. Select the drive under **OS** and launch it. Use `DIR` to list its contents.

DOS can read, create and modify files in the shared folder. Files written in DOS
are available to Amiga applications after the session closes. Copy files in
Workbench between DOS sessions and launch again to refresh DOS directory caches.
Do not edit the same file from both systems or remove a card during a session.

Adding the same folder again selects its existing DOS-drive entry; it does not
copy the folder or replace a game profile. The usual program settings choose
memory, graphics, sound and window/fullscreen mode. Use **Add program** for a
separate game entry pointing into the shared directory.

## Requirements and limits

- USB storage must already be mounted by AmigaOS, using Poseidon, a supported
  storage driver and an appropriate filesystem. For FAT cards this may be fat95;
  FAT32 support depends on the installed filesystem. The launcher does not
  install USB drivers, format cards or mount raw physical devices.
- SD cards exposed by IDE or another Amiga controller work through their mounted
  volume in the same way. The source does not have to be USB.
- Writable media is required; the launcher writes its startup batch and welcome
  helper into the selected folder. Prefer a dedicated DOS folder.
- Use DOS-compatible **8.3 filenames** within the shared folder. The Amiga path
  to that folder may contain spaces.
- This is built-in **DOSBox DOS 5** with a folder-backed `C:`. It is not an extra
  drive injected into a booted MS-DOS 6.22 or Windows disk image. Those images
  retain the existing image mount/eject workflow.
- The new panel does not add hot-removal support or bypass media write protection.

Validation: native profile serialization and invalid-path tests passed with
ASan/UBSan. On the A4000TX, DOS read an Amiga-created file byte-for-byte and
created a second file read back by AmigaOS on SD032G; the player released Core1
cleanly. The MUI layout probe passed all 12 pages at a 640×480 screen budget.
No USB storage volume was mounted during this test, so USB-controller and
filesystem compatibility remains dependent on the existing AmigaOS mount.

## CDs visible in Workbench

Select the game or DOS drive, then open **DOS → CD-ROM → Use Workbench CD…**.
Choose the mounted disc in the drawer requester. The launcher resolves its
Workbench volume name to the Amiga device, checks for a readable ISO9660 data
CD, and saves it for the selected program. On launch, the disc is DOS **D:**.
**Disks → CD-ROM settings…** opens the same controls. Physical-CD mode's
**Browse…** button also selects a Workbench disc; ISO mode browses image files.

IDE/ATAPI, SCSI and USB CD-ROMs use the device driver, unit and flags from
their existing AmigaOS mount. No particular controller or device name is
hard-coded. The mounted driver must provide 2048-byte CD sector reads. If Workbench cannot mount the disc, configure that first. No driver,
mountlist or boot configuration is changed here. Audio-only discs, CD audio
playback and burning are not supported by this read-only data-CD backend.
Keep the disc inserted until DOS closes; select a replacement between sessions.
