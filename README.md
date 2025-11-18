# fs-patch

A Switch sysmodule that patches FS and LDR, for use with the nxdumpguide.

---

## Config

**fs-patch** features a simple config. This can be manually edited or updated using the overlay.

The configuration file can be found in `/config/fs-patch/config.ini`. The file is generated once the module is ran for the first time.

```ini
[options]
patch_emummc=1   ; 1=(default) patch emummc, 0=don't patch emummc
enable_logging=1 ; 1=(default) output /config/fs-patch/log.ini 0=no log
version_skip=1   ; 1=(default) skips out of date patterns, 0=search all patterns
```

---

## Overlay

The overlay can be used to change the config options and to see what patches are applied.

- Unpatched means the patch wasn't applied (likely not found).
- Patched (green) means it was patched by fs-patch.
- Patched (yellow) means it was already patched, likely by sigpatches or a custom Atmosphere build.

---

## Building

### prerequisites
- Install [devkitpro](https://devkitpro.org/wiki/Getting_Started)
- Run the following:
  ```sh
  git clone --recurse-submodules https://github.com/DefenderOfHyrule/fs-patch.git
  cd ./fs-patch
  make
  ```

The output of `out/` can be copied to your SD card.
To activate the sys-module, reboot your switch, or, use [sysmodules overlay](https://github.com/WerWolv/ovl-sysmodules/releases/latest) with the accompanying overlay to activate it.

---

## What is being patched?

Here's a quick run down of what's being patched:

- **fs** needs new patches after every new firmware version.
- **ldr** needs new patches after every new [Atmosphere](https://github.com/Atmosphere-NX/Atmosphere/) release.

The patches are applied on boot. Once done, the sys-module stops running.
The memory footprint *(16kib)* and the binary size *(~50kib)* are both very small.

---

## FAQ:

### If I am using sigpatches already, is there any point in using this?

Yes, in 3 situations.

1. A new **ldr** patch needs to be created after every Atmosphere update. Sometimes, a new silent Atmosphere update is released. This tool will always patch **ldr** without having to update patches.

2. Building Atmosphere from src will require you to generate a new **ldr** patch for that custom built Atmosphere. This is easy enough due to the public scripts / tools that exist out there, however this will always be able to patch **ldr**.

3.  If you forget to update your patches when you update your firmware / Atmosphere, this sys-module should be able to patch everything. So it can be used as a fallback.

---

## Credits / Thanks

Software is built on the shoulders of giants. This tool wouldn't be possible without these people:

- MrDude
- BornToHonk (farni)
- TeJay
- ArchBox
- Switchbrew (libnx, switch-examples)
- DevkitPro (toolchain)
- [minIni](https://github.com/compuphase/minIni)
- [libtesla](https://github.com/WerWolv/libtesla)

