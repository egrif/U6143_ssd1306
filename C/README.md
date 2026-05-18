# U6143 SSD1306 OLED Display — C Driver

## Install (recommended)

```bash
curl -fsSL https://raw.githubusercontent.com/egrif/U6143_ssd1306/master/install.sh | sudo bash
```

This enables I²C automatically, compiles the driver, installs the binary to `/usr/local/bin/ssd1306-display`, writes a default config to `/etc/ssd1306.conf`, and starts a systemd service that runs on every boot. Re-running the command updates to the latest version without touching your config.

> **Note:** On first install, if I²C was not previously enabled, a reboot may be required before the display activates. The installer will tell you if this is the case.

## Manual build

```bash
git clone https://github.com/egrif/U6143_ssd1306.git
cd U6143_ssd1306/C
make
sudo ./display
```

## Terminal simulator

Build and run a pixel-accurate terminal preview without any hardware:

```bash
make sim
./display_sim          # full size (128×32 → 128 cols × 16 rows)
./display_sim 0.5      # half size (64×8 half-block rows)
```

`display_sim` reads the same config file as the real binary and cycles through
all enabled screens every 3 seconds. It renders the 128×32 framebuffer to the
terminal using Unicode half-block characters — requires a UTF-8 terminal at
least 130 columns wide.

On Linux the simulator uses real system data (CPU temp from
`/sys/class/thermal`, memory from `/proc/meminfo`, etc.). On other platforms
those screens show placeholder values while screens that need only standard
POSIX calls (clock, disk, network) show real data.

The program reads an optional config file (see below), then cycles through all
enabled screens every 3 seconds in a continuous loop.

---

## Screens

Each screen is independently enabled or disabled via the config file. The three
original screens are on by default; the rest are off.

| Key | Default | What is displayed |
|-----|---------|-------------------|
| `show_temperature` | `1` | **Temperature screen** — IP address (or hostname / custom text) on the top line, CPU temperature and 1-minute load average on the bottom line. The background bitmap changes with the temperature unit. |
| `show_memory` | `1` | **RAM screen** — free and total RAM in GB (e.g. `0.3G / 1.8G`). |
| `show_disk` | `1` | **Disk screen** — used and total disk space in GB for the root filesystem. |
| `show_clock` | `0` | **Clock screen** — current time (`HH:MM:SS`) in large font, day-of-week and date (`Wed 2025-01-15`) below. |
| `show_uptime` | `0` | **Uptime screen** — how long the system has been running (`2d 3h 14m` or `45m`), and total process count. |
| `show_cpu_freq` | `0` | **CPU frequency screen** — current CPU clock speed in MHz, and throttle status (`OK`, or flags: `UV` under-voltage, `FC` frequency-capped, `TH` throttled, `TL` soft-temp-limit). Requires `vcgencmd`. |
| `show_gpu_temp` | `0` | **GPU temperature screen** — CPU and GPU temperatures side-by-side in the configured unit. GPU value requires `vcgencmd`; shows `N/A` if unavailable. |
| `show_network` | `0` | **Network throughput screen** — receive (`NET RX`) and transmit (`NET TX`) rates in B/s, KB/s, or MB/s for the interface selected by `ip_source`. |
| `show_wifi` | `0` | **Wi-Fi screen** — signal level in dBm and link quality out of 70 for `wlan0`. Shows `N/A` if the interface is not present. |
| `show_docker` | `0` | **Docker screen** — running container count (`RUN: N`) and stopped container count (`STP: M`). Requires `docker` in `$PATH`; shows `N/A` if unavailable. |
| `show_hostname` | `0` | **Hostname screen** — system hostname in large font. |
| `show_ip` | `0` | **IP address screen** — interface label (`eth0:` or `wlan0:`) and IP address in large font. |

If every screen is disabled the temperature screen is shown regardless.

---

## Configuration file

The driver looks for a config file in this order:

1. Path in the `SSD1306_CONF` environment variable
2. `./ssd1306.conf` (next to the binary)
3. `/etc/ssd1306.conf`

If no file is found, compiled-in defaults are used (same as the defaults in the
table above).

Copy the example and edit as needed:

```bash
cp ssd1306.conf.example ssd1306.conf
```

### Full reference

```ini
# Which screens to cycle through (1=enabled, 0=disabled)
show_temperature=1
show_memory=1
show_disk=1
show_clock=0
show_uptime=0
show_cpu_freq=0
show_gpu_temp=0
show_network=0
show_wifi=0
show_docker=0
show_hostname=0
show_ip=0

# Temperature unit: celsius or fahrenheit
temp_unit=fahrenheit

# How to display CPU load: percent (e.g. 15%) or cores (e.g. 0.15)
load_display=percent

# Comma-separated network interfaces for IP and throughput screens
network_interfaces=eth0,wlan0

# Seconds to show each screen before advancing
screen_time=3

# Top line shown on every screen: ip (default), hostname, custom, or none
top_line=ip

# Text shown when top_line=custom (max 63 characters)
custom_text=UCTRONICS
```

### `top_line`

Controls what appears in row 0 of every screen. When set to any value other than `none`, it replaces the screen's default row-0 label (e.g. `UPTIME`, `WIFI`) on every screen. Values are truncated to 16 characters to guarantee a single-row fit.

| Value | What is shown |
|-------|---------------|
| `ip` | IP address of the interface named by `ip_source` (`eth0` or `wlan0`). Default. |
| `hostname` | System hostname as returned by `gethostname(3)`. |
| `custom` | The literal string in `custom_text` (up to 63 characters). |
| `none` | Top line disabled; each screen shows its own label in row 0. |

---

## External dependencies

| Screen | Dependency | Notes |
|--------|-----------|-------|
| `show_cpu_freq` | `vcgencmd` | Part of `libraspberrypi-bin`; throttle status silently reads as `OK` if absent |
| `show_gpu_temp` | `vcgencmd` | GPU value shows `N/A` if absent |
| `show_docker` | `docker` CLI | Count shows `N/A` if absent |
